// ponytail: 51↔ESP NRF raw bridge, poll routes by byte[0]
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

uint8_t sens_buf[8];
uint8_t stat_buf[8];

static uint32_t dbg_poll, dbg_avail;

void nrf_init(void) {
    SPI.begin(SCK, MISO, MOSI);
    nrf.begin();
    nrf.setChannel(2);
    nrf.setDataRate(RF24_2MBPS);
    nrf.setPALevel(RF24_PA_MIN);
    nrf.setAutoAck(false);
    nrf.setPayloadSize(8);
    // nrf.enableDynamicPayloads();
    nrf.openReadingPipe(0, addr);
    nrf.startListening();
}

void nrf_poll(void) {
    dbg_poll++;
    uint8_t tmp[8];
    while (nrf.available()) {
        nrf.read(tmp, 8);
        dbg_avail++;
        if      (tmp[0] == 0x01) memcpy(sens_buf, tmp, 8);
        else if (tmp[0] == 0x02) memcpy(stat_buf, tmp, 8);
    }
}

bool nrf_online(void) { return nrf.isChipConnected(); }

void nrf_tx(const uint8_t *data, uint8_t len) {
    nrf.stopListening();
    nrf.writeFast(data, len);
    nrf.startListening();
}

#include <stdio.h>
void nrf_debug(char *json, size_t n) {
    snprintf(json, n, R"({"poll":%lu,"avail":%lu,"online":%d})",
             dbg_poll, dbg_avail, nrf_online());
}
