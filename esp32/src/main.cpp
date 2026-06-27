#include <WiFi.h>
#include <WebServer.h>
#include <esp_camera.h>
#include <RF24.h>

// ── NRF pins ──────────────────
RF24 nrf(4, 2);  // CE=GPIO4, CSN=GPIO2

// ── Camera pins ───────────────
#define CAM_PIN_PWDN  -1
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK  0
#define CAM_PIN_SIOD  26
#define CAM_PIN_SIOC  27
#define CAM_PIN_D7    35
#define CAM_PIN_D6    34
#define CAM_PIN_D5    39
#define CAM_PIN_D4    36
#define CAM_PIN_D3    21
#define CAM_PIN_D2    19
#define CAM_PIN_D1    18
#define CAM_PIN_D0    5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF  23
#define CAM_PIN_PCLK  22

// ── WiFi AP ───────────────────
const char *ssid = "ESP32-CAM";
const char *pass = "12345678";
WebServer server(80);

// ── NRF rx buf ────────────────
uint8_t nrf_buf[4];
uint8_t addr[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};

void setup() {
    Serial.begin(115200);

    // Camera init
    camera_config_t cfg;
    cfg.ledc_channel = LEDC_CHANNEL_0;
    cfg.ledc_timer   = LEDC_TIMER_0;
    cfg.pin_d0  = CAM_PIN_D0;  cfg.pin_d1  = CAM_PIN_D1;
    cfg.pin_d2  = CAM_PIN_D2;  cfg.pin_d3  = CAM_PIN_D3;
    cfg.pin_d4  = CAM_PIN_D4;  cfg.pin_d5  = CAM_PIN_D5;
    cfg.pin_d6  = CAM_PIN_D6;  cfg.pin_d7  = CAM_PIN_D7;
    cfg.pin_xclk   = CAM_PIN_XCLK;
    cfg.pin_pclk   = CAM_PIN_PCLK;
    cfg.pin_vsync  = CAM_PIN_VSYNC;
    cfg.pin_href   = CAM_PIN_HREF;
    cfg.pin_sscb_sda = CAM_PIN_SIOD;
    cfg.pin_sscb_scl = CAM_PIN_SIOC;
    cfg.pin_pwdn  = CAM_PIN_PWDN;
    cfg.pin_reset = CAM_PIN_RESET;
    cfg.xclk_freq_hz = 20000000;
    cfg.pixel_format  = PIXFORMAT_JPEG;
    cfg.frame_size    = FRAMESIZE_VGA;
    cfg.jpeg_quality  = 12;
    cfg.fb_count      = 1;
    esp_camera_init(&cfg);

    // WiFi AP
    WiFi.softAP(ssid, pass);

    // HTTP /cam → JPEG snapshot
    server.on("/cam", []() {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) { server.send(500, "text/plain", "cam fail"); return; }
        server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
        esp_camera_fb_return(fb);
    });

    // HTTP /data → NRF 4-byte hex
    server.on("/data", []() {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02X%02X%02X%02X",
                 nrf_buf[0], nrf_buf[1], nrf_buf[2], nrf_buf[3]);
        server.send(200, "text/plain", buf);
    });

    server.begin();

    // NRF init
    nrf.begin();
    nrf.setChannel(2);
    nrf.setDataRate(RF24_1MBPS);
    nrf.openReadingPipe(0, addr);
    nrf.startListening();
}

void loop() {
    server.handleClient();

    if (nrf.available()) {
        nrf.read(nrf_buf, 4);
        Serial.printf("NRF: %02X %02X %02X %02X\n",
                      nrf_buf[0], nrf_buf[1], nrf_buf[2], nrf_buf[3]);
    }
}
