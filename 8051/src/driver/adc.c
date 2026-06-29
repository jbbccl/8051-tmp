#include "config.h"
#include "adc.h"

uint16_t adc_read(uint8_t cmd) {
    uint8_t i;
    uint16_t val = 0;

    ADC_DCLK = 0;
    ADC_CS = 0;

    /* 写 8 位控制字 */
    for (i = 0; i < 8; i++) {
        ADC_DIN = cmd & 0x80;
        ADC_DCLK = 1;
        cmd <<= 1;
        ADC_DCLK = 0;
    }

    /* 等转换 + 读 12 位结果 */
    ADC_DCLK = 1; ADC_DCLK = 0;  /* 第1个时钟空读 */

    for (i = 0; i < 12; i++) {
        ADC_DCLK = 1;
        val <<= 1;
        if (ADC_DOUT) val |= 1;
        ADC_DCLK = 0;
    }

    ADC_CS = 1;
    return val;
}
