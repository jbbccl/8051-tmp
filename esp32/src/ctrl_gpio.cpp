// ponytail: /51mcu route, NRF wired, protocol in app layer
#include "ctrl_gpio.h"
#include "esp_http_server.h"
#include "nrf_bridge.h"
#include "Arduino.h"

static void parse_sensors(char *json, size_t n) {
    if (!nrf_online()){
        snprintf(json, n, R"({"temp":0,"light":0,"ir":0,"dist":1,"nrf":0})");
        return;
    }
    if (!nrf_rx_ready()) {
        snprintf(json, n, R"({"temp":0,"light":0,"ir":0,"dist":2,"nrf":1})");
        return;
    }
    uint8_t buf[8];
    nrf_rx_get(buf, 8);
    if (buf[0] != 0x01) {
        snprintf(json, n, R"({"temp":0,"light":0,"ir":0,"dist":3,"nrf":1})");
        return;
    }
    uint16_t temp  = buf[1] | (buf[2] << 8);
    uint16_t light = buf[3] | (buf[4] << 8);
    uint8_t  ir    = buf[5];
    uint16_t dist  = buf[6] | (buf[7] << 8);
    snprintf(json, n, R"({"temp":%u,"light":%u,"ir":%u,"dist":%u,"nrf":1})",
             temp, light, ir, dist);
}

static esp_err_t mcu_handler(httpd_req_t *req) {
    char buf[64], cmd[16], val[8];
    if (httpd_req_get_url_query_len(req) + 1 <= (int)sizeof(buf))
        httpd_req_get_url_query_str(req, buf, sizeof(buf));

    if (!httpd_query_key_value(buf, "cmd", cmd, sizeof(cmd))) {
        if (!strcmp(cmd, "debug")) {
            char json[128];
            nrf_debug(json, sizeof(json));
            httpd_resp_set_type(req, "application/json");
            return httpd_resp_send(req, json, -1);
        }
        if (!strcmp(cmd, "sensors")) {
            char json[128];
            parse_sensors(json, sizeof(json));
            httpd_resp_set_type(req, "application/json");
            return httpd_resp_send(req, json, -1);
        }
        if (!strcmp(cmd, "fan") || !strcmp(cmd, "pump")) {
            int v = 0;
            httpd_query_key_value(buf, "val", val, sizeof(val));
            v = atoi(val);
            Serial.printf("%s:%d\n", cmd, v);  // ponytail: → nrf_tx when TX wired
            return httpd_resp_send(req, "ok", -1);
        }
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

httpd_uri_t make_51mcu_uri(void) {
    httpd_uri_t u = {.uri = "/51mcu", .method = HTTP_GET, .handler = mcu_handler, .user_ctx = NULL};
    return u;
}
