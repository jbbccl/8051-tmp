#ifndef NRF_H
#define NRF_H

#include <stdint.h>

#define NRF_CE   P1_2
#define NRF_CSN  P1_3
#define NRF_SCK  P1_7
#define NRF_MOSI P1_1
#define NRF_MISO P1_6

uint8_t nrf_spi(uint8_t dat);
uint8_t nrf_read(uint8_t reg);
void nrf_write(uint8_t reg, uint8_t val);
void nrf_tx_init(uint8_t *addr, uint8_t ch, uint8_t rate);
void nrf_set_tx_addr(uint8_t *addr);
void nrf_send(uint8_t *data, uint8_t len);
void nrf_rx_init(uint8_t *addr, uint8_t ch, uint8_t rate);
uint8_t nrf_available(void);
void nrf_recv(uint8_t *buf, uint8_t len);
void nrf_set_ch(uint8_t ch);
void nrf_set_rate(uint8_t rate);
uint8_t nrf_rpd(void);

// ponytail: hot-switch without PWR_UP cycling
void nrf_init_global(uint8_t *addr, uint8_t ch, uint8_t rate);
void nrf_set_tx_mode(void);
void nrf_set_rx_mode(void);

#endif
