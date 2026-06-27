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
    uint8_t addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t buf[4];
    seg7_init();
    nrf_rx_init(addr);

    while (1) {
        if (nrf_available()) {
            nrf_recv(buf, 4);
            seg7_hex(((uint32_t)buf[0] << 24)
                   | ((uint32_t)buf[1] << 16)
                   | ((uint32_t)buf[2] << 8)
                   |  (uint32_t)buf[3]);
        }
    }
}
