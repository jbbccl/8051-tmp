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
    wifi_init();

    startCameraServer();

    Serial.print("Ready! http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop() {
    nrf_poll();
    delay(10);
}
