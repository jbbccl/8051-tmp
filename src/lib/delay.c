#include <8051.h>
#include "delay.h"

void delay(unsigned int t) {
    while (t--) {
        unsigned int i;
        for (i = 0; i < 114; i++);
    }
}

// ~5.4us per loop @11.0592MHz 12T
void delay_us(unsigned char us) {
    while (us--) {
        __asm NOP __endasm;
        __asm NOP __endasm;
        __asm NOP __endasm;
        __asm NOP __endasm;
        __asm NOP __endasm;
    }
}
