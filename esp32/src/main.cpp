#include <Arduino.h>
#include <WiFi.h>
#include "camera.h"
#include "wifi_ap.h"
#include "app_httpd.h"
#include "nrf_bridge.h"

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    camera_init();
    nrf_init();  // ponytail: after camera → ledc detach CE safe
    Serial.println(nrf_online() ? "NRF OK" : "NRF FAIL");
    wifi_init();

    startCameraServer();

    Serial.print("Ready! http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop() {
    nrf_poll();
    uint8_t test[] = {4,1,0,0,0,0,0,0};
    static int tx_tick;
    if (++tx_tick >= 10) { tx_tick = 0; nrf_tx(test, 8); }
    delay(10);
}
