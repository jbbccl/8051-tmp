#include "config.h"
#include "seg7.h"
#include "delay.h"
#include "keypad.h"
#include "motor.h"
#include "ds18b20.h"
#include "ir.h"

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

/* INT0 ISR 必须在 main.c */
void int0_isr(void) __interrupt(0) {
    ir_isr_body();
}

void main(void) {
    seg7_init();
    ir_init();
    unsigned long k = 0UL;

    // while (1){
    //     seg7_num(k++);
    //     delay_ms(200);
    // }

    while (1) {
        k = ((uint32_t)ir_buff[0] << 24)
          | ((uint32_t)ir_buff[1] << 16)
          | ((uint32_t)ir_buff[2] << 8)
          |  (uint32_t)ir_buff[3];
        seg7_hex(k);
    }
}
