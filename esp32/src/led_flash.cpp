#include "Arduino.h"
#include "board_config.h"
#include "esp32-hal-ledc.h"
#include "led_flash.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#endif

#if defined(LED_GPIO_NUM)
#define CONFIG_LED_MAX_INTENSITY 255

int led_duty = 0;
bool isStreaming = false;

void enable_led(bool en) {
    int duty = en ? led_duty : 0;
    if (en && isStreaming && (led_duty > CONFIG_LED_MAX_INTENSITY)) {
        duty = CONFIG_LED_MAX_INTENSITY;
    }
    ledcWrite(1, duty);
    log_i("Set LED intensity to %d", duty);
}

#endif

void setupLedFlash() {
#if defined(LED_GPIO_NUM)
    ledcSetup(1, 5000, 8);
    ledcAttachPin(LED_GPIO_NUM, 1);
#else
    log_i("LED flash is disabled -> LED_GPIO_NUM undefined");
#endif
}
