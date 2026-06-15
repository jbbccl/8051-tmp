#include "config.h"
#include "delay.h"
#include "ir.h"

volatile uint8_t ir_buff[4];

void ir_init(void) {
    IT0 = 1;
    EX0 = 1;
    EA = 1;
    IR = 1;
}

uint16_t ir_isr_body(void){
    if(!IR){
        uint16_t t = 1500;
        while (!IR) if(t-- < 900) return 0;       // ~10ms
        if (t > 1200) return 3;                   // noise < 3ms 
        while (IR) if(t-- < 500) return 0;        // ~5ms
        //success
        uint8_t ibyte, ibit, byte=0;
        for(ibyte = 0; ibyte < 4; ibyte++){
            for(ibit = 0; ibit < 8; ibit++){
                t=160;
                while (!IR) if(t-- <110 ) return 0; // ~0.8ms
                while (IR) if(t-- < 5 ) return 0;
                //104~0; 44~1
                if(t < 74) byte = (byte >> 1) | 0x80;
                else       byte = byte >> 1;
            }
            ir_buff[ibyte] = byte;
        }
        return 1;
    }
    return 3; // noise
}

uint8_t ir_map(uint8_t cmd) {
    switch (cmd) {
        case 0x07: return 16;
        case 0x08: return 4;
        case 0x09: return 18;
        case 0x0C: return 1;
        case 0x0D: return 20;
        case 0x15: return 17;
        case 0x16: return 0;
        case 0x18: return 2;
        case 0x19: return 19;
        case 0x1C: return 5;
        case 0x40: return 14;
        case 0x42: return 7;
        case 0x43: return 15;
        case 0x44: return 13;
        case 0x45: return 10;
        case 0x46: return 11;
        case 0x47: return 12;
        case 0x4A: return 9;
        case 0x52: return 8;
        case 0x5A: return 6;
        case 0x5E: return 3;
        default:   return cmd;
    }
}

uint8_t ir_read(){
    if(ir_buff[0] != (uint8_t)~ir_buff[1] || ir_buff[2] != (uint8_t)~ir_buff[3])
        return 0xFF;
    return ir_map(ir_buff[2]);
}