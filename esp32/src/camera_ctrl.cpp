#include "Arduino.h"
#include "board_config.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "led_flash.h"
#include "camera_ctrl.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#endif

// --- query-string parser ---
static esp_err_t parse_get(httpd_req_t *req, char **obuf) {
    char *buf = NULL;
    size_t buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char *)malloc(buf_len);
        if (!buf) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

static int parse_get_var(char *buf, const char *key, int def) {
    char _int[16];
    if (httpd_query_key_value(buf, key, _int, sizeof(_int)) != ESP_OK) return def;
    return atoi(_int);
}

// --- register dump helper ---
static int print_reg(char *p, char *end, sensor_t *s, uint16_t reg, uint32_t mask) {
    return snprintf(p, end - p, "\"0x%04x\":%d,", reg, s->get_reg(s, reg, mask));
}

// --- cmd handler ---
static esp_err_t cmd_handler(httpd_req_t *req) {
    char *buf = NULL;
    char variable[32];
    char value[32];

    if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;
    if (httpd_query_key_value(buf, "var", variable, sizeof(variable)) != ESP_OK ||
        httpd_query_key_value(buf, "val", value, sizeof(value)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int val = atoi(value);
    log_i("%s = %d", variable, val);
    sensor_t *s = esp_camera_sensor_get();
    int res = 0;

    if (!strcmp(variable, "framesize")) {
        if (s->pixformat == PIXFORMAT_JPEG) res = s->set_framesize(s, (framesize_t)val);
    } else if (!strcmp(variable, "quality")) {
        res = s->set_quality(s, val);
    } else if (!strcmp(variable, "contrast")) {
        res = s->set_contrast(s, val);
    } else if (!strcmp(variable, "brightness")) {
        res = s->set_brightness(s, val);
    } else if (!strcmp(variable, "saturation")) {
        res = s->set_saturation(s, val);
    } else if (!strcmp(variable, "gainceiling")) {
        res = s->set_gainceiling(s, (gainceiling_t)val);
    } else if (!strcmp(variable, "colorbar")) {
        res = s->set_colorbar(s, val);
    } else if (!strcmp(variable, "awb")) {
        res = s->set_whitebal(s, val);
    } else if (!strcmp(variable, "agc")) {
        res = s->set_gain_ctrl(s, val);
    } else if (!strcmp(variable, "aec")) {
        res = s->set_exposure_ctrl(s, val);
    } else if (!strcmp(variable, "hmirror")) {
        res = s->set_hmirror(s, val);
    } else if (!strcmp(variable, "vflip")) {
        res = s->set_vflip(s, val);
    } else if (!strcmp(variable, "awb_gain")) {
        res = s->set_awb_gain(s, val);
    } else if (!strcmp(variable, "agc_gain")) {
        res = s->set_agc_gain(s, val);
    } else if (!strcmp(variable, "aec_value")) {
        res = s->set_aec_value(s, val);
    } else if (!strcmp(variable, "aec2")) {
        res = s->set_aec2(s, val);
    } else if (!strcmp(variable, "dcw")) {
        res = s->set_dcw(s, val);
    } else if (!strcmp(variable, "bpc")) {
        res = s->set_bpc(s, val);
    } else if (!strcmp(variable, "wpc")) {
        res = s->set_wpc(s, val);
    } else if (!strcmp(variable, "raw_gma")) {
        res = s->set_raw_gma(s, val);
    } else if (!strcmp(variable, "lenc")) {
        res = s->set_lenc(s, val);
    } else if (!strcmp(variable, "special_effect")) {
        res = s->set_special_effect(s, val);
    } else if (!strcmp(variable, "wb_mode")) {
        res = s->set_wb_mode(s, val);
    } else if (!strcmp(variable, "ae_level")) {
        res = s->set_ae_level(s, val);
    }
#if defined(LED_GPIO_NUM)
    else if (!strcmp(variable, "led_intensity")) {
        led_duty = val;
        if (isStreaming) enable_led(true);
    }
#endif
    else {
        log_i("Unknown command: %s", variable);
        res = -1;
    }

    if (res < 0) return httpd_resp_send_500(req);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

// --- status handler ---
static esp_err_t status_handler(httpd_req_t *req) {
    static char json_response[1024];

    sensor_t *s = esp_camera_sensor_get();
    char *p = json_response;
    char *end = json_response + sizeof(json_response);
    *p++ = '{';

    if (s->id.PID == OV5640_PID || s->id.PID == OV3660_PID) {
        for (int reg = 0x3400; reg < 0x3406; reg += 2) p += print_reg(p, end, s, reg, 0xFFF);
        p += print_reg(p, end, s, 0x3406, 0xFF);
        p += print_reg(p, end, s, 0x3500, 0xFFFF0);
        p += print_reg(p, end, s, 0x3503, 0xFF);
        p += print_reg(p, end, s, 0x350a, 0x3FF);
        p += print_reg(p, end, s, 0x350c, 0xFFFF);
        for (int reg = 0x5480; reg <= 0x5490; reg++) p += print_reg(p, end, s, reg, 0xFF);
        for (int reg = 0x5380; reg <= 0x538b; reg++) p += print_reg(p, end, s, reg, 0xFF);
        for (int reg = 0x5580; reg < 0x558a; reg++) p += print_reg(p, end, s, reg, 0xFF);
        p += print_reg(p, end, s, 0x558a, 0x1FF);
    } else if (s->id.PID == OV2640_PID) {
        p += print_reg(p, end, s, 0xd3, 0xFF);
        p += print_reg(p, end, s, 0x111, 0xFF);
        p += print_reg(p, end, s, 0x132, 0xFF);
    }

    p += snprintf(p, end - p, "\"xclk\":%u,", s->xclk_freq_hz / 1000000);
    p += snprintf(p, end - p, "\"pixformat\":%u,", s->pixformat);
    p += snprintf(p, end - p, "\"framesize\":%u,", s->status.framesize);
    p += snprintf(p, end - p, "\"quality\":%u,", s->status.quality);
    p += snprintf(p, end - p, "\"brightness\":%d,", s->status.brightness);
    p += snprintf(p, end - p, "\"contrast\":%d,", s->status.contrast);
    p += snprintf(p, end - p, "\"saturation\":%d,", s->status.saturation);
    p += snprintf(p, end - p, "\"sharpness\":%d,", s->status.sharpness);
    p += snprintf(p, end - p, "\"special_effect\":%u,", s->status.special_effect);
    p += snprintf(p, end - p, "\"wb_mode\":%u,", s->status.wb_mode);
    p += snprintf(p, end - p, "\"awb\":%u,", s->status.awb);
    p += snprintf(p, end - p, "\"awb_gain\":%u,", s->status.awb_gain);
    p += snprintf(p, end - p, "\"aec\":%u,", s->status.aec);
    p += snprintf(p, end - p, "\"aec2\":%u,", s->status.aec2);
    p += snprintf(p, end - p, "\"ae_level\":%d,", s->status.ae_level);
    p += snprintf(p, end - p, "\"aec_value\":%u,", s->status.aec_value);
    p += snprintf(p, end - p, "\"agc\":%u,", s->status.agc);
    p += snprintf(p, end - p, "\"agc_gain\":%u,", s->status.agc_gain);
    p += snprintf(p, end - p, "\"gainceiling\":%u,", s->status.gainceiling);
    p += snprintf(p, end - p, "\"bpc\":%u,", s->status.bpc);
    p += snprintf(p, end - p, "\"wpc\":%u,", s->status.wpc);
    p += snprintf(p, end - p, "\"raw_gma\":%u,", s->status.raw_gma);
    p += snprintf(p, end - p, "\"lenc\":%u,", s->status.lenc);
    p += snprintf(p, end - p, "\"hmirror\":%u,", s->status.hmirror);
    p += snprintf(p, end - p, "\"vflip\":%u,", s->status.vflip);
    p += snprintf(p, end - p, "\"dcw\":%u,", s->status.dcw);
    p += snprintf(p, end - p, "\"colorbar\":%u", s->status.colorbar);
#if defined(LED_GPIO_NUM)
    p += snprintf(p, end - p, ",\"led_intensity\":%u", led_duty);
#else
    p += snprintf(p, end - p, ",\"led_intensity\":%d", -1);
#endif
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

// --- xclk handler ---
static esp_err_t xclk_handler(httpd_req_t *req) {
    char *buf = NULL;
    char _xclk[32];
    if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;
    if (httpd_query_key_value(buf, "xclk", _xclk, sizeof(_xclk)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int xclk = atoi(_xclk);
    log_i("Set XCLK: %d MHz", xclk);

    sensor_t *s = esp_camera_sensor_get();
    if (s->set_xclk(s, LEDC_TIMER_0, xclk)) return httpd_resp_send_500(req);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

// --- reg handler ---
static esp_err_t reg_handler(httpd_req_t *req) {
    char *buf = NULL;
    char _reg[32], _mask[32], _val[32];
    if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;
    if (httpd_query_key_value(buf, "reg", _reg, sizeof(_reg)) != ESP_OK ||
        httpd_query_key_value(buf, "mask", _mask, sizeof(_mask)) != ESP_OK ||
        httpd_query_key_value(buf, "val", _val, sizeof(_val)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int reg = atoi(_reg), mask = atoi(_mask), val = atoi(_val);
    log_i("Set Register: reg: 0x%02x, mask: 0x%02x, value: 0x%02x", reg, mask, val);
    sensor_t *s = esp_camera_sensor_get();
    if (s->set_reg(s, reg, mask, val)) return httpd_resp_send_500(req);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

// --- greg handler ---
static esp_err_t greg_handler(httpd_req_t *req) {
    char *buf = NULL;
    char _reg[32], _mask[32];
    if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;
    if (httpd_query_key_value(buf, "reg", _reg, sizeof(_reg)) != ESP_OK ||
        httpd_query_key_value(buf, "mask", _mask, sizeof(_mask)) != ESP_OK) {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }
    free(buf);

    int reg = atoi(_reg), mask = atoi(_mask);
    sensor_t *s = esp_camera_sensor_get();
    int res = s->get_reg(s, reg, mask);
    if (res < 0) return httpd_resp_send_500(req);
    log_i("Get Register: reg: 0x%02x, mask: 0x%02x, value: 0x%02x", reg, mask, res);

    char buffer[20];
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, itoa(res, buffer, 10), strlen(itoa(res, buffer, 10)));
}

// --- pll handler ---
static esp_err_t pll_handler(httpd_req_t *req) {
    char *buf = NULL;
    if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;

    int bypass = parse_get_var(buf, "bypass", 0);
    int mul    = parse_get_var(buf, "mul", 0);
    int sys    = parse_get_var(buf, "sys", 0);
    int root   = parse_get_var(buf, "root", 0);
    int pre    = parse_get_var(buf, "pre", 0);
    int seld5  = parse_get_var(buf, "seld5", 0);
    int pclken = parse_get_var(buf, "pclken", 0);
    int pclk   = parse_get_var(buf, "pclk", 0);
    free(buf);

    log_i("Set Pll: bypass: %d, mul: %d, sys: %d, root: %d, pre: %d, seld5: %d, pclken: %d, pclk: %d",
          bypass, mul, sys, root, pre, seld5, pclken, pclk);
    sensor_t *s = esp_camera_sensor_get();
    if (s->set_pll(s, bypass, mul, sys, root, pre, seld5, pclken, pclk))
        return httpd_resp_send_500(req);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

// --- win/resolution handler ---
static esp_err_t win_handler(httpd_req_t *req) {
    char *buf = NULL;
    if (parse_get(req, &buf) != ESP_OK) return ESP_FAIL;

    int startX  = parse_get_var(buf, "sx", 0);
    int startY  = parse_get_var(buf, "sy", 0);
    int endX    = parse_get_var(buf, "ex", 0);
    int endY    = parse_get_var(buf, "ey", 0);
    int offsetX = parse_get_var(buf, "offx", 0);
    int offsetY = parse_get_var(buf, "offy", 0);
    int totalX  = parse_get_var(buf, "tx", 0);
    int totalY  = parse_get_var(buf, "ty", 0);
    int outputX = parse_get_var(buf, "ox", 0);
    int outputY = parse_get_var(buf, "oy", 0);
    bool scale  = parse_get_var(buf, "scale", 0) == 1;
    bool binning = parse_get_var(buf, "binning", 0) == 1;
    free(buf);

    log_i("Set Window: Start: %d %d, End: %d %d, Offset: %d %d, Total: %d %d, Output: %d %d, "
          "Scale: %u, Binning: %u",
          startX, startY, endX, endY, offsetX, offsetY, totalX, totalY, outputX, outputY, scale, binning);
    sensor_t *s = esp_camera_sensor_get();
    if (s->set_res_raw(s, startX, startY, endX, endY, offsetX, offsetY, totalX, totalY,
                       outputX, outputY, scale, binning))
        return httpd_resp_send_500(req);
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

// --- public factory functions ---
httpd_uri_t make_status_uri(void) {
    httpd_uri_t u = {.uri = "/status", .method = HTTP_GET, .handler = status_handler, .user_ctx = NULL};
    return u;
}
httpd_uri_t make_cmd_uri(void) {
    httpd_uri_t u = {.uri = "/control", .method = HTTP_GET, .handler = cmd_handler, .user_ctx = NULL};
    return u;
}
httpd_uri_t make_xclk_uri(void) {
    httpd_uri_t u = {.uri = "/xclk", .method = HTTP_GET, .handler = xclk_handler, .user_ctx = NULL};
    return u;
}
httpd_uri_t make_reg_uri(void) {
    httpd_uri_t u = {.uri = "/reg", .method = HTTP_GET, .handler = reg_handler, .user_ctx = NULL};
    return u;
}
httpd_uri_t make_greg_uri(void) {
    httpd_uri_t u = {.uri = "/greg", .method = HTTP_GET, .handler = greg_handler, .user_ctx = NULL};
    return u;
}
httpd_uri_t make_pll_uri(void) {
    httpd_uri_t u = {.uri = "/pll", .method = HTTP_GET, .handler = pll_handler, .user_ctx = NULL};
    return u;
}
httpd_uri_t make_win_uri(void) {
    httpd_uri_t u = {.uri = "/resolution", .method = HTTP_GET, .handler = win_handler, .user_ctx = NULL};
    return u;
}
