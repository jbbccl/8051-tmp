#include "config.h"
#include "nrf.h"

uint8_t nrf_spi(uint8_t dat) {
    uint8_t i, val = 0;
    for (i = 0; i < 8; i++) {
        NRF_MOSI = dat & 0x80;
        dat <<= 1;
        NRF_SCK = 1;
        val <<= 1;
        if (NRF_MISO) val |= 1;
        NRF_SCK = 0;
    }
    return val;
}

uint8_t nrf_read(uint8_t reg) {
    uint8_t v;
    NRF_CSN = 0;
    nrf_spi(reg);           /* 第1字节: STATUS, 忽略 */
    v = nrf_spi(0xFF);      /* 第2字节: 寄存器值 */
    NRF_CSN = 1;
    return v;
}

void nrf_write(uint8_t reg, uint8_t val) {
    NRF_CSN = 0;
    nrf_spi(0x20 | reg);   /* W_REGISTER */
    nrf_spi(val);
    NRF_CSN = 1;
}

void nrf_tx_init(uint8_t *addr) {
    NRF_CE = 0;
    nrf_write(0x00, 0x08);
    nrf_write(0x00, 0x0E);
    nrf_write(0x01, 0x00);
    nrf_write(0x02, 0x01);
    nrf_write(0x03, 0x03);
    nrf_write(0x04, 0x00);
    nrf_write(0x05, 0x02);
    nrf_write(0x06, 0x06);
    nrf_set_tx_addr(addr);
    nrf_write(0x11, 4);
}

void nrf_set_tx_addr(uint8_t *addr) {
    uint8_t i;
    NRF_CSN = 0;
    nrf_spi(0x20 | 0x10);
    for (i = 0; i < 5; i++) nrf_spi(addr[i]);
    NRF_CSN = 1;
}

void nrf_send(uint8_t *data, uint8_t len) {
    uint8_t i;
    NRF_CSN = 0;
    nrf_spi(0xA0);              /* W_TX_PAYLOAD */
    for (i = 0; i < len; i++) nrf_spi(data[i]);
    NRF_CSN = 1;

    NRF_CE = 1;
    __asm__("nop"); __asm__("nop"); __asm__("nop");
    NRF_CE = 0;
}