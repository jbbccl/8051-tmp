#ifndef SEG7_H
#define SEG7_H

#include <stdint.h>

void seg7_init(void);
void seg7_num(unsigned long num);
void seg7_hex(uint32_t val);
void seg7_scan(void);

#endif