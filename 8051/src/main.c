// ponytail: 51→ESP sensors + status, NRF 2Mbps, 8B payload
#include "config.h"
#include "delay.h"
#include "nrf.h"
#include "ds18b20.h"
#include "adc.h"
#include "ir.h"
#include "motor.h"
#include <stdint.h>

#define CH  2
#define RATE 0x08    // 2Mbps -18dBm
static uint8_t addr[] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static uint8_t motor_enable, pump_enable, motor_limit = 30, pump_limit = 50;

// 8B: [0]=0x01, [1-2]=temp, [3-4]=light, [5]=ir, [6-7]=dist
//      [0]=0x02, [1]=motor, [2]=pump, [3]=motor_lim, [4]=pump_lim

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
}

int main(void) {
    uint8_t data[8], cmd[4];
    ir_init();

    while (1) {
        uint16_t temp  = ds18b20_tempure();
        uint16_t light = adc_read(0xA4);
        uint8_t  ir    = ir_read();

        // sensors
        data[0] = 0x01;
        data[1] = temp & 0xFF;
        data[2] = (temp >> 8) & 0xFF;
        data[3] = light & 0xFF;
        data[4] = (light >> 8) & 0xFF;
        data[5] = ir;
        data[6] = 0; data[7] = 0;  // dist pin conflict

        nrf_tx_init(addr, CH, RATE);
        nrf_write(0x11, 8);
        nrf_send(data, 8);

        // status — replaces echo
        data[0] = 0x02;
        data[1] = motor_enable;
        data[2] = pump_enable;
        data[3] = motor_limit;
        data[4] = pump_limit;
        data[5] = 0; data[6] = 0; data[7] = 0;
        nrf_send(data, 8);

        // recv commands
        nrf_rx_init(addr, CH, RATE);
        delay_ms(50);
        if (nrf_available()) {
            nrf_recv(cmd, 4);
            switch (cmd[0]) {
                case 0x04: motor_enable = cmd[1]; break;
                case 0x05: pump_enable  = cmd[1]; break;
                case 0x06: motor_limit  = cmd[1]; break;
                case 0x07: pump_limit   = cmd[1]; break;
            }
        }

        if (motor_enable && temp > motor_limit * 100) motor_on();
        else motor_off();
        // ponytail: pump_on/off not in motor.h yet, add when pump driver exists
    }
}
