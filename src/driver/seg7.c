#include "config.h"
#include "seg7.h"

#define DIG_SEL(d)  P2 = (P2 & 0xE3) | (((d) & 0x07) << 2)

static __code unsigned char seg_table[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F,
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
};

static unsigned char disp_buf[8];
static volatile unsigned char disp_pos;

void seg7_scan(void) {
    P0 = 0x00;
    DIG_SEL(disp_pos);
    P0 = seg_table[disp_buf[disp_pos]];
    disp_pos = (disp_pos + 1) & 0x07;
}

void seg7_init(void) {
    TMOD = (TMOD & 0xF0) | 0x01;
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    ET0 = 1;
    TR0 = 1;
    EA = 1;
}

void seg7_num(unsigned long num) {
    unsigned char i;
    // EA = 0;
    for (i = 0; i < 8; i++) {
        disp_buf[i] = num % 10;
        num /= 10;
    }
    // EA = 1;
}

void seg7_hex(uint32_t val) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        disp_buf[i] = val & 0xF;
        val >>= 4;
    }
}