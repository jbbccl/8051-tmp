#include <8051.h>
#include "keypad.h"

// P1_7~P1_4 行输出(低有效), P1_3~P1_0 列输入(上拉)

static __code unsigned short key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

unsigned short keypad_scan(void) {
    unsigned short row, col, tmp;
    for (row = 0; row < 4; row++) {
        P1 = ~(0x80 >> row) & ~1;
        tmp = P1 & 0x0F;
        if (tmp != 0x0F) {
            for (col = 0; col < 3 ; col++)
                if (!(tmp & (8 >> col)))
                    return key_map[row][col];
        }
    }
    return 233;
}

unsigned short idp_key_scan(void) {
    if (!P3_0) return 2;
    if (!P3_1) return 1;
    if (!P3_2) return 3;
    if (!P3_3) return 4;
    return 233;
}
