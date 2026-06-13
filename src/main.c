#include <8051.h>
#include "seg7.h"
#include "delay.h"

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

void main(void) {
    seg7_init();
    unsigned long k = 87654321UL;
    while (1) {
        seg7_num(k++);
        delay(400);
    }
}