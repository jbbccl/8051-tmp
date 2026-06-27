#include <WiFi.h>
#include "wifi_ap.h"

const char *ssid     = "ESP32-CAM";
const char *password = "12345678";

void wifi_init(void) {
    WiFi.softAP(ssid, password);
    Serial.println("AP up");
}
