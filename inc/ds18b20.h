#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>
#define DQ P3_7

uint16_t Temper_change(void);
uint16_t ds18b20_tempure(void);
uint8_t init(void);

#endif