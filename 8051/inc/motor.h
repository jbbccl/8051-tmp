#ifndef MOTOR_H
#define MOTOR_H

#define MOTOR_PIN P1_0

void motor_on(void);
void motor_off(void);
void motor_run(unsigned char duty);

#endif
