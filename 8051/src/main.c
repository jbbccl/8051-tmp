// ponytail: 51→ESP sensors + status, NRF 2Mbps, 8B payload
#include "config.h"
#include "delay.h"
#include "nrf.h"
#include "ds18b20.h"
#include "adc.h"
#include "ir.h"
#include "motor.h"
#include <stdint.h>
#include "seg7.h"

#define CH  2
#define RATE 0x08    // 2Mbps -18dBm
static uint8_t addr[] = {0xE7,0xE7,0xE7,0xE7,0xE7};

static uint8_t motor_enable=0, pump_enable=0, motor_limit = 30, pump_limit = 50;

void timer0_isr(void) __interrupt(1) {
    TH0 = (65536 - 914) >> 8;
    TL0 = (65536 - 914) & 0xFF;
    seg7_scan();
}

void int0_isr(void) __interrupt(0) { ir_isr_body(); }  // ponytail: IR INT0

int main(void) {
    uint8_t data[8], cmd[8];
    ir_init();
    seg7_init();
    nrf_init_global(addr, CH, RATE);
    uint8_t ir = 0;
    while (1) {
        uint16_t temp  = ds18b20_tempure();
        uint16_t light = adc_read(0xA4);
        uint8_t  tmpir = ir_read();
        if(tmpir != 0xFF) ir = tmpir;

        data[0] = 0x01;
        data[1] = temp & 0xFF;
        data[2] = (temp >> 8) & 0xFF;
        data[3] = light & 0xFF;
        data[4] = (light >> 8) & 0xFF;
        data[5] = ir;
        data[6] = 0; data[7] = 0;

        nrf_set_tx_mode();
        nrf_send(data, 8);

        data[0] = 0x02;
        data[1] = motor_enable;
        data[2] = pump_enable;
        data[3] = motor_limit;
        data[4] = pump_limit;
        data[5] = 0; data[6] = 0; data[7] = 0;
        nrf_send(data, 8);

        nrf_set_rx_mode();
        uint16_t rx_wait = 500;
        int dbg = 0;
        while (rx_wait--) {
            if (nrf_available()) {
                dbg = 1;
                nrf_recv(cmd, 8);
                switch (cmd[0]) {
                    case 0x04: motor_enable = cmd[1]; break;
                    case 0x05: pump_enable  = cmd[1]; break;
                    case 0x06: motor_limit  = cmd[1]; break;
                    case 0x07: pump_limit   = cmd[1]; break;
                }  
                break;
            }
            delay_ms(1);
        }

        if (motor_enable && temp > motor_limit * 100) {
            // motor_run(90);
            seg7_num(110000+ir);
        }
        else seg7_num(220000+ir);
    }
}
