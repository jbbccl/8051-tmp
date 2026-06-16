#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdint.h>

#define ECHO P3_4
#define TRIG P3_5

uint16_t hc_sr04_trig();
uint16_t hc_sr04_cm();
uint16_t hc_sr04_avg_cm(uint8_t times);

#endif