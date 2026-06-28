// ponytail: unified /51mcu, serial-only, NRF pending
#include "ctrl_gpio.h"
#include "esp_http_server.h"
#include "Arduino.h"

static esp_err_t mcu_handler(httpd_req_t *req) {
    char buf[64], cmd[16], val[8];
    if (httpd_req_get_url_query_len(req) + 1 <= (int)sizeof(buf))
        httpd_req_get_url_query_str(req, buf, sizeof(buf));

    if (!httpd_query_key_value(buf, "cmd", cmd, sizeof(cmd))) {
        if (!strcmp(cmd, "sensors")) {
            Serial.println("22222");
            httpd_resp_set_type(req, "application/json");
            return httpd_resp_send(req, R"({"temp":0,"light":2,"dist":1,"ir":3})", -1);
        }
        if (!strcmp(cmd, "fan") || !strcmp(cmd, "pump")) {
            // int v = httpd_query_key_value(buf, "val", val, sizeof(val)) ? 0 : atoi(val);
            Serial.println("11111");
            httpd_resp_set_type(req, "application/json");
            return httpd_resp_send(req, R"({"temp":0,"light":2,"dist":1,"ir":3})", -1);
        }
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

httpd_uri_t make_51mcu_uri(void) {
    httpd_uri_t u = {.uri = "/51mcu", .method = HTTP_GET, .handler = mcu_handler, .user_ctx = NULL};
    return u;
}
