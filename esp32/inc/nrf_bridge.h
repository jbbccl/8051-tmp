// ponytail: NRF raw TX/RX, no protocol
#pragma once
#include <stdint.h>

void nrf_init(void);
void nrf_poll(void);
void nrf_tx(const uint8_t *data, uint8_t len);
bool nrf_rx_ready(void);
bool nrf_online(void);
void nrf_rx_get(uint8_t *buf, uint8_t len);
