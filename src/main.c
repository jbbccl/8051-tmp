#include "config.h"
#include "seg7.h"
#include "delay.h"
#include "nrf.h"

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

int main(void) {
    uint8_t my_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t data[]   = {0xAA, 0xBB, 0xCC, 0xDD};
    seg7_init();
    nrf_tx_init(my_addr);
    nrf_send(data, 4);
    delay_ms(10);
    seg7_hex(nrf_read(0x07));   /* 预期 0x2E */
    while (1);
}
