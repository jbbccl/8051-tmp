#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define ADC_DIN   P3_4
#define ADC_CS    P3_5
#define ADC_DCLK  P3_6
#define ADC_DOUT  P3_7

uint16_t adc_read(uint8_t cmd);

#endif
