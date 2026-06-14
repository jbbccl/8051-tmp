#include "config.h"
#include "delay.h"

// 11.0592MHz 12T模式: 1机器周期 = 12/11059200 ≈ 1.085µs

void delay_ms(uint16_t ms) {
	uint16_t z;
	for(ms; ms > 0; ms--)
		for(z = 110; z > 0; z--);
}

// 1 次循环 ≈ 10 机器周期 ≈ 10.85µs
void delay_10us(uint16_t us) {
    while (us--);
}