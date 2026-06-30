// ponytail: P2_5/6/7 relay, GPIO bit ops — no .c needed
#ifndef RELAY_H
#define RELAY_H

#include "config.h"
#define RELAY1 P2_5
#define RELAY2 P2_6
#define RELAY3 P2_7

void relay1_on(void)  { RELAY1 = 1; }
void relay1_off(void) { RELAY1 = 0; }
void relay2_on(void)  { RELAY2 = 1; }
void relay2_off(void) { RELAY2 = 0; }
void relay3_on(void)  { RELAY3 = 1; }
void relay3_off(void) { RELAY3 = 0; }

#endif
