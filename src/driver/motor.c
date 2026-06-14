#include "config.h"
#include "motor.h"
#include "delay.h"

#define MOTOR_PIN P1_0

void motor_on(void)  { MOTOR_PIN = 1; }
void motor_off(void) { MOTOR_PIN = 0; }

// duty: 0-100, 周期约5.4ms (~185Hz)
void motor_run(unsigned char duty) {
    unsigned char i;
    for (i = 0; i < 100; i++) {
        MOTOR_PIN = (i < duty);
        delay_us(10);
    }
}
