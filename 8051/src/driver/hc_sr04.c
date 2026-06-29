#include "config.h"
#include "hc_sr04.h"


uint16_t hc_sr04_trig(){
    uint16_t t = 2;
    TRIG = 1; while (t--); TRIG = 0;

    t = 0;
    while(!ECHO) if(t++ > 400) return 6665;
    t = 0;
    while(ECHO) if(t++ > 900) return 6666;

    return t;
}

uint16_t hc_sr04_cm(){
    uint16_t cm = hc_sr04_trig();
    if(cm > 6660) return cm;
    cm *= 3;
    cm >>= 3;
    return cm;
}

uint16_t hc_sr04_avg_cm(uint8_t times){
    uint16_t sum = 0;
    uint8_t n = times;
    while (n){
        uint16_t res = hc_sr04_cm();
        if(res < 6660){
            sum += res;
            n--;
        }
    }
    sum /= times;
    return sum;
}