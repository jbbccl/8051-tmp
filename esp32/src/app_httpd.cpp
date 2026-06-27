// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Arduino.h"
#include "board_config.h"
#include "camera_index.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "sdkconfig.h"
#include "app_httpd.h"
#include "camera_stream.h"
#include "camera_ctrl.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#endif

static httpd_handle_t stream_httpd = NULL;
static httpd_handle_t camera_httpd = NULL;

// --- index handler ---
static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    sensor_t *s = esp_camera_sensor_get();
    if (s == NULL) {
        log_e("Camera sensor not found");
        return httpd_resp_send_500(req);
    }
    return httpd_resp_send(req, (const char *)index_ov2640_html_gz, index_ov2640_html_gz_len);
}

void startCameraServer() {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.max_uri_handlers = 16;

    httpd_uri_t index_uri = {.uri = "/", .method = HTTP_GET, .handler = index_handler, .user_ctx = NULL};

    httpd_uri_t status_uri  = make_status_uri();
    httpd_uri_t cmd_uri     = make_cmd_uri();
    httpd_uri_t capture_uri = make_capture_uri();
    httpd_uri_t bmp_uri     = make_bmp_uri();
    httpd_uri_t xclk_uri    = make_xclk_uri();
    httpd_uri_t reg_uri     = make_reg_uri();
    httpd_uri_t greg_uri    = make_greg_uri();
    httpd_uri_t pll_uri     = make_pll_uri();
    httpd_uri_t win_uri     = make_win_uri();

    camera_ra_filter_init(20);

    log_i("Starting web server on port: '%u'", cfg.server_port);
    if (httpd_start(&camera_httpd, &cfg) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &status_uri);
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &bmp_uri);
        httpd_register_uri_handler(camera_httpd, &xclk_uri);
        httpd_register_uri_handler(camera_httpd, &reg_uri);
        httpd_register_uri_handler(camera_httpd, &greg_uri);
        httpd_register_uri_handler(camera_httpd, &pll_uri);
        httpd_register_uri_handler(camera_httpd, &win_uri);
    }

    cfg.server_port += 1;
    cfg.ctrl_port += 1;
    log_i("Starting stream server on port: '%u'", cfg.server_port);
    if (httpd_start(&stream_httpd, &cfg) == ESP_OK) {
        httpd_uri_t stream_uri = make_stream_uri();
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}
