#include <8051.h>
#include "delay.h"

void delay(unsigned int t) {
    while (t--) {
        unsigned int i;
        for (i = 0; i < 114; i++);
    }
}