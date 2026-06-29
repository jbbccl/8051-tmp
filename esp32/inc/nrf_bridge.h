// ponytail: bufs exposed global — no getter, no lock
#pragma once
#include <stdint.h>
#include <stddef.h>

extern uint8_t sens_buf[8];
extern uint8_t stat_buf[8];

void nrf_init(void);
void nrf_poll(void);
void nrf_tx(const uint8_t *data, uint8_t len);
bool nrf_online(void);
void nrf_debug(char *json, size_t n);
