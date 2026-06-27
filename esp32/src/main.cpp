#include <Arduino.h>
#include <WiFi.h>
#include "camera.h"
#include "wifi_ap.h"

void startCameraServer();

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println();

    camera_init();
    wifi_init();

    startCameraServer();

    Serial.print("Ready! http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
}

void loop() {
    delay(10000);
}
