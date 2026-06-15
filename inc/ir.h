#ifndef IR_H
#define IR_H

#include <stdint.h>

#define IR P3_2

extern volatile uint8_t ir_buff[4];

void ir_init(void);
uint16_t ir_isr_body(void);

#endif
