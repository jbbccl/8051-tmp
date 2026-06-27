#ifndef CAMERA_CTRL_H
#define CAMERA_CTRL_H

#include "esp_http_server.h"

httpd_uri_t make_status_uri(void);
httpd_uri_t make_cmd_uri(void);
httpd_uri_t make_xclk_uri(void);
httpd_uri_t make_reg_uri(void);
httpd_uri_t make_greg_uri(void);
httpd_uri_t make_pll_uri(void);
httpd_uri_t make_win_uri(void);

#endif
