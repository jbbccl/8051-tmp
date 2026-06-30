// ponytail: P2_5/6/7 relay, GPIO bit ops — no .c needed
#ifndef RELAY_H
#define RELAY_H

#include "config.h"
#define RELAY1 P2_5
#define RELAY2 P2_6
#define RELAY3 P2_7
#define RELAY_OFF   1
#define RELAY_ON    0
void relay1_on(void)  { RELAY1 = RELAY_ON; }
void relay1_off(void) { RELAY1 = RELAY_OFF; }
void relay2_on(void)  { RELAY2 = RELAY_ON; }
void relay2_off(void) { RELAY2 = RELAY_OFF; }
void relay3_on(void)  { RELAY3 = RELAY_ON; }
void relay3_off(void) { RELAY3 = RELAY_OFF; }

#endif
