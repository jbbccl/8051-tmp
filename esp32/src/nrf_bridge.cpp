// ponytail: 51↔ESP NRF bridge, poll mode, single-buffer guard
#include "nrf_bridge.h"
#include <RF24.h>
#include <SPI.h>

#define CE   4
#define CSN  2
#define SCK  1
#define MOSI 15
#define MISO 13

static RF24 nrf(CE, CSN);
static const uint8_t addr[] = {0xE7,0xE7,0xE7,0xE7,0xE7};
static uint8_t rx_buf[8];
static uint8_t rx_que[32], front = 0, rear = 0, ql = 0;

void nrf_init(void) {
    SPI.begin(SCK, MISO, MOSI, CSN);
    nrf.begin();
    nrf.setChannel(2);
    nrf.setDataRate(RF24_2MBPS);
    nrf.setPALevel(RF24_PA_MIN);
    nrf.openReadingPipe(0, addr);
    nrf.startListening();
}

void nrf_poll(void) {
    if (nrf.available()) {
        nrf.read(rx_buf, 8);
        for(auto &i : rx_buf){
            rx_que[(rear++)&31] = i;
            if( ql++ ==32 ) return;
        }
    }
}

bool nrf_available(void) { return ql > 0; }

void nrf_read(uint8_t *buf, uint8_t len) {
    if( ql < len ) return;
    ql -= len;
    for (uint8_t i = 0; i < len; i++) {
        buf[i] = rx_que[(front++)&31];
    }
}

void nrf_tx(const uint8_t *data, uint8_t len) {
    nrf.stopListening();
    nrf.writeFast(data, len);
    nrf.startListening();
}
