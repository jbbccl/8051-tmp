// ponytail: 51→ESP sensors + ESP→51 motor ctl, NRF 2Mbps, 8B payload
#include "config.h"
#include "delay.h"
#include "seg7.h"
#include "nrf.h"
#include "ds18b20.h"
#include "adc.h"
#include "ir.h"
#include "motor.h"

#define CH  2        // match ESP channel
#define RATE 0x08    // 2Mbps -18dBm → match ESP RF24_2MBPS+PA_MIN
static uint8_t addr[] = {0xE7,0xE7,0xE7,0xE7,0xE7};

// 8B packed format: [0]=type(0x01=sensors), [1..7]=data
//   [1]=temp_l, [2]=temp_h, [3]=light_l, [4]=light_h
//   [5]=ir, [6]=dist_l, [7]=dist_h

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

int main(void) {
    uint8_t data[8], cmd[4];
    uint16_t dist;  // ponytail: HC_SR04, remap P3 when ADC not needed
    // seg7_init();
    ir_init();

    while (1) {
        uint16_t temp  = ds18b20_tempure();
        uint16_t light = adc_read(0x94);
        uint8_t  ir    = ir_read();
        dist = 0;  // ponytail: pin conflict, remap to enable

        data[0] = 0x01;                  // type: sensors
        data[1] = temp & 0xFF;
        data[2] = (temp >> 8) & 0xFF;
        data[3] = light & 0xFF;
        data[4] = (light >> 8) & 0xFF;
        data[5] = ir;
        data[6] = dist & 0xFF;
        data[7] = (dist >> 8) & 0xFF;

        nrf_tx_init(addr, CH, RATE);
        nrf_write(0x11, 8);
        nrf_send(data, 8);

        nrf_rx_init(addr, CH, RATE);
        delay_ms(50);
        if (nrf_available()) {
            nrf_recv(cmd, 4);
            switch (cmd[0]) {
                case 0x01: motor_on();          break;
                case 0x02: motor_off();         break;
                case 0x03: motor_run(cmd[1]);   break;
            }
            nrf_write(0x07, 0x70);
        }
    }
}
