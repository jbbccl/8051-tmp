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

uint8_t ds18b20_init(void) {
    ET0 = 0;
    DQ = 0;
    delay = 124; while (delay--);
    DQ = 1;
    ET0 = 1;
    return check();
}

uint8_t read() {
	uint8_t byte = 0, bit, j;
    ET0 = 0;
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
    ET0 = 1;
	return byte;
}

void write(uint8_t dat) {
	uint8_t j;
    ET0 = 0;
	for(j = 0; j < 8; j++) {
		DQ = 0;
		delay++;
		DQ = dat & 0x01;
		delay=6;
		while(delay--);
		DQ = 1;
		dat >>= 1;
	}
    ET0 = 1;
}

uint16_t ReadTemp(void) {
    uint16_t temper = 0;
    ds18b20_init();
    write(0xcc);
    write(0x44);
    delay_ms(750);
    ds18b20_init();
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