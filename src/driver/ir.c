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