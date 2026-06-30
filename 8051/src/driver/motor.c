#include "config.h"
#include "motor.h"
#include "relay.h"

#define MOTOR_PIN P1_0

void motor_on(void)  { 
    relay2_on();
 }
void motor_off(void) { 
    relay2_off();
}
void pump_off(void){
    relay1_off();
}
void pump_on(void){
    relay1_on();
}
// duty: 0-100, 周期约5.4ms (~185Hz)
void motor_run(unsigned char duty) {
    unsigned char i;
    for (i = 0; i < 100; i++) {
        MOTOR_PIN = (i < duty);
        int delay = 66;while (delay--);
    }
}
