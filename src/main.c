#include "config.h"
#include "seg7.h"
#include "delay.h"
#include "keypad.h"
#include "motor.h"
#include "ds18b20.h"

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

void main(void) {
    seg7_init();
    unsigned long k = 0UL;
    while (1) {
        P2_5=0;
        uint16_t key;
        uint16_t t = ds18b20_tempure();//init();//
        if(1) key = t;
        seg7_num(key);
        delay_ms(500);
        // unsigned char key = keypad_scan();
        // if(key != 233) k = key - 0x30;
        // seg7_num(k);

        // if (idp_key_scan() == 1)
        //     motor_run( k * 10 + 10 );//5ms
        // else
        //     motor_off();
        
    }
}