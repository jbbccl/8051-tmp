// ponytail: 51↔ESP NRF raw bridge, no protocol
#include "nrf_bridge.h"
#include <RF24.h>
#include <SPI.h>

#define CE   15
#define CSN  13
#define MOSI 12
#define MISO 2
#define SCK  14

static RF24 nrf(CE, CSN);
static const uint8_t addr[] = {0xE7,0xE7,0xE7,0xE7,0xE7};
static uint8_t rx_buf[8];
static bool rx_ready;
static uint32_t dbg_poll, dbg_avail;  // ponytail: debug counters

void nrf_init(void) {
    SPI.begin(SCK, MISO, MOSI);
    nrf.begin();
    nrf.setChannel(2);
    nrf.setDataRate(RF24_2MBPS);
    nrf.setPALevel(RF24_PA_MIN);
    nrf.setAutoAck(false);
    nrf.setPayloadSize(8);
    nrf.openReadingPipe(0, addr);
    nrf.startListening();
}

void nrf_poll(void) {
    dbg_poll++;
    if (!rx_ready && nrf.available()) {
        dbg_avail++;
        nrf.read(rx_buf, 8);
        rx_ready = true;
    }
}

bool nrf_rx_ready(void) { return rx_ready; }

void nrf_rx_get(uint8_t *buf, uint8_t len) {
    for (uint8_t i = 0; i < len && i < 8; i++) buf[i] = rx_buf[i];
    rx_ready = false;  // ponytail: consumed, allow next poll
}

bool nrf_online(void) { return nrf.isChipConnected(); }

void nrf_tx(const uint8_t *data, uint8_t len) {
    nrf.stopListening();
    nrf.writeFast(data, len);
    nrf.startListening();
}

#include <stdio.h>
void nrf_debug(char *json, size_t n) {
    snprintf(json, n, R"({"poll":%lu,"avail":%lu,"online":%d,"rx_ready":%d,"now":%d})",
             dbg_poll, dbg_avail, nrf_online(), rx_ready, nrf.available());
}
