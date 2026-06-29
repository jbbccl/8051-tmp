#include "config.h"
#include "nrf.h"

// ── base ────────────────────────────────────
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
    nrf_spi(reg);
    v = nrf_spi(0xFF);
    NRF_CSN = 1;
    return v;
}

void nrf_write(uint8_t reg, uint8_t val) {
    NRF_CSN = 0;
    nrf_spi(0x20 | reg);
    nrf_spi(val);
    NRF_CSN = 1;
}

static void nrf_set_addr(uint8_t reg, uint8_t *addr) {
    uint8_t i;
    NRF_CSN = 0;
    nrf_spi(0x20 | reg);
    for (i = 0; i < 5; i++) nrf_spi(addr[i]);
    NRF_CSN = 1;
}

void nrf_set_ch(uint8_t ch) {
    nrf_write(0x05, ch);
}

void nrf_set_rate(uint8_t rate) {
    nrf_write(0x06, rate);
}


// ── send ────────────────────────────────────
void nrf_tx_init(uint8_t *addr, uint8_t ch, uint8_t rate) {
    NRF_CE = 0;
    nrf_write(0x00, 0x08);
    nrf_write(0x00, 0x0E);
    nrf_write(0x01, 0x00);
    nrf_write(0x02, 0x01);
    nrf_write(0x03, 0x03);
    nrf_write(0x04, 0x00);
    nrf_set_ch(ch);
    nrf_set_rate(rate);
    nrf_set_addr(0x10, addr);
    nrf_write(0x11, 4);
}

void nrf_send(uint8_t *data, uint8_t len) {
    uint8_t i;
    NRF_CSN = 0;
    nrf_spi(0xA0);
    for (i = 0; i < len; i++) nrf_spi(data[i]);
    NRF_CSN = 1;

    NRF_CE = 1;
    delay_10us(1);
    NRF_CE = 0;
}

// ── recv ────────────────────────────────────
void nrf_rx_init(uint8_t *addr, uint8_t ch, uint8_t rate) {
    NRF_CE = 0;
    nrf_write(0x00, 0x08);
    nrf_write(0x00, 0x0F);
    nrf_write(0x02, 0x01);
    nrf_write(0x03, 0x03);
    nrf_set_ch(ch);
    nrf_set_rate(rate);
    nrf_set_addr(0x0A, addr);
    nrf_write(0x11, 4);
    NRF_CE = 1;
}

uint8_t nrf_available(void) {
    uint8_t s = nrf_read(0x07);
    if (s & 0x40) {          /* RX_DR = 1 */
        nrf_write(0x07, s);  /* 写 1 清 RX_DR */
        return 1;
    }
    return 0;
}

void nrf_recv(uint8_t *buf, uint8_t len) {
    uint8_t i;
    NRF_CSN = 0;
    nrf_spi(0x61);           /* R_RX_PAYLOAD */
    for (i = 0; i < len; i++)
        buf[i] = nrf_spi(0xFF);
    NRF_CSN = 1;
}

// ── misc ────────────────────────────────────
uint8_t nrf_rpd(void) {
    NRF_CE = 1;
    delay_ms(1);
    NRF_CE = 0;
    return nrf_read(0x09) & 1;
}