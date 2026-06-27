#ifndef CAMERA_STREAM_H
#define CAMERA_STREAM_H

#include <stddef.h>
#include "esp_http_server.h"

void camera_ra_filter_init(size_t sample_size);

httpd_uri_t make_capture_uri(void);
httpd_uri_t make_bmp_uri(void);
httpd_uri_t make_stream_uri(void);

#endif
