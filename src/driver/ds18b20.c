#include "config.h"
#include "ds18b20.h"

uint8_t delay;

uint8_t check(void) {
    uint8_t t = 0;
    while (DQ){
        if (++t > 60) return 0;
    }
    while (!DQ){
        delay++;delay++;
        if (++t > 140) return 0;
    }
    return t;
}

uint8_t init(void) {
    DQ = 0;
    // 207: 13 - 0 剧烈跳变,经常为0 200: 11-13跳变 213 : 13-8
    // 100: 13,0 113:8,11 114:7,13 115:6,13 116:5,12 117:5,11 118:5,10 119:4,10 120:7-9-8-
    // 121:7,9 122:7,8 123:678 124:13-4-7- 
    // 125:先乱动,6 127:13,0,5 130: 3,6 140:6一次13,11
    delay = 124; while (delay--);
    DQ = 1;
    return check();
}

uint8_t read() {
	uint8_t byte = 0, bit, j;
	for (j = 0; j < 8; j++) {
		DQ = 0;
		delay++;
		DQ = 1;
		delay++;
		delay++;
		bit = DQ;
		byte = (byte >> 1) | (bit << 7);
		delay = 2; while (delay--);
	}
	return byte;
}

void write(uint8_t dat) {
	uint8_t j;
	for(j = 0; j < 8; j++) {
		DQ = 0;
		delay++;
		DQ = dat & 0x01;
		delay=6;
		while(delay--);
		DQ = 1;
		dat >>= 1;
	}
}

uint16_t ReadTemp(void) {
    uint16_t temper = 0;
    init();
    write(0xcc);
    write(0x44);
    delay_ms(750);
    init();
    write(0xcc);
    write(0xbe);

    uint8_t tml = read();
    uint8_t tmh = read();
    temper = tmh;
    temper = (temper << 8) | tml;
    return temper;
}


uint16_t ds18b20_tempure(void) {
    uint16_t temper;
    temper = ReadTemp();
    if (temper == 0xFFFF) return 0xFFFF;

    if (temper < 0) {
        temper = ~temper + 1;
        temper = (temper * 25) / 4;
        return (uint16_t)(-temper);
    } else {
        return (uint16_t)((temper * 25) / 4)+1;
    }
}