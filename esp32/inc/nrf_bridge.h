// ponytail: NRF bridge, poll in loop(), RF24 lib handles ack/retry
#pragma once
#include <stdint.h>

void nrf_init(void);
void nrf_poll(void);                          // call in loop()
void nrf_tx(const uint8_t *data, uint8_t len);
bool nrf_available(void);                     // true if packet received
void nrf_read(uint8_t *buf, uint8_t len);     // read last packet
