#ifndef LED_FLASH_H
#define LED_FLASH_H

#include <stdbool.h>

#if defined(LED_GPIO_NUM)
extern int led_duty;
extern bool isStreaming;
#endif

void enable_led(bool en);
void setupLedFlash(void);

#endif
