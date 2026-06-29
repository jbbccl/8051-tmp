#include "config.h"
#include "delay.h"

void delay_ms(uint16_t ms) {
    uint16_t z;
    for (; ms > 0; ms--)
        for (z = 110; z > 0; z--);
}

void delay_10us(uint16_t us) {
    while (us--);
}

// Timer1 精确 10us 延时, 11.0592MHz 12T
// 10us / 1.085us = 9.22周期 -> 取9周期 = 9.77us (误差-2.3%)
void super_delay_10us(uint16_t n) {
    uint16_t cnt = n * 9;
    EA = 0;
    TMOD = (TMOD & 0x0F) | 0x10;
    TH1 = (65536 - cnt) >> 8;
    TL1 = (65536 - cnt) & 0xFF;
    TF1 = 0;
    TR1 = 1;
    EA = 1;
    while (!TF1);
    TR1 = 0;
}
