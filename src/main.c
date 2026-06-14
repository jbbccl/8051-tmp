#include <8051.h>
#include "seg7.h"
#include "delay.h"
#include "keypad.h"

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

void main(void) {
    seg7_init();
    unsigned long k = 0UL;
    while (1) {
        unsigned char key = idp_key_scan();
        // if(key != 0) key-= 0x30;
        seg7_num(key);
        delay(200);
    }
}