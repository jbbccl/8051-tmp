// ponytail: /51mcu route, bufs direct — no getter
#include "ctrl_gpio.h"
#include "esp_http_server.h"
#include "nrf_bridge.h"
#include "Arduino.h"

static void sensors_json(char *json, size_t n) {
    if (!nrf_online()) {
        snprintf(json, n, R"({"temp":0,"light":0,"ir":0,"dist":0,"nrf":0})");
        return;
    }
    uint16_t t = sens_buf[1] | (sens_buf[2] << 8);
    uint16_t l = sens_buf[3] | (sens_buf[4] << 8);
    uint8_t  i = sens_buf[5];
    uint16_t d = sens_buf[6] | (sens_buf[7] << 8);
    snprintf(json, n,
        R"({"temp":%u,"light":%u,"ir":%u,"dist":%u,"motor":%u,"pump":%u,"motor_lim":%u,"pump_lim":%u,"nrf":1})",
        t, l, i, d,
        stat_buf[1], stat_buf[2], stat_buf[3], stat_buf[4]);
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
            char json[256];
            sensors_json(json, sizeof(json));
            httpd_resp_set_type(req, "application/json");
            return httpd_resp_send(req, json, -1);
        }
        if (!strcmp(cmd, "fan_enable") || !strcmp(cmd, "pump_enable") ||
            !strcmp(cmd, "fan_limit")  || !strcmp(cmd, "pump_limit")) {
            int v = 0;
            httpd_query_key_value(buf, "val", val, sizeof(val));
            v = atoi(val);
            uint8_t pkt[4] = {0};
            if (!strcmp(cmd, "fan_enable"))       { pkt[0]=0x04; pkt[1]=v; }
            else if (!strcmp(cmd, "pump_enable")) { pkt[0]=0x05; pkt[1]=v; }
            else if (!strcmp(cmd, "fan_limit"))   { pkt[0]=0x06; pkt[1]=v; }
            else                                  { pkt[0]=0x07; pkt[1]=v; }
            nrf_tx(pkt, 4);
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
