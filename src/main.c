#include "config.h"
#include "seg7.h"
#include "delay.h"
#include "adc.h"

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

int main(void) {
    seg7_init();
    while (1) {
        seg7_num(adc_read(0xA4));  /* AIN0 电位器 */
        delay_ms(200);
    }
}
