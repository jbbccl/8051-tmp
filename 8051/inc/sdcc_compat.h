#ifndef SDCC_COMPAT_H
#define SDCC_COMPAT_H

/* SDCC memory space qualifiers */
#define __data
#define __xdata
#define __idata
#define __pdata
#define __code
#define __near
#define __far
#define __eeprom

/* SDCC function qualifiers */
#define __interrupt(x)
#define __using(x)
#define __reentrant
#define __naked
#define __critical
#define __nonbanked
#define __shadow_regs
#define __wparam
#define __preserves_regs(...)

/* SDCC storage class extensions */
#define __at(x)
#define __at_adr(x)

/* SFR / SBIT */
#define SFR(name, addr) volatile unsigned char name
#define SFR16(name, addr) volatile unsigned int name
#define SBIT(name, addr, bit) volatile unsigned char name
#define __sfr volatile unsigned char
#define __sbit volatile unsigned char

/* SDCC inline asm */
#define __asm
#define __endasm
#define __asm__(x) 

/* SDCC bit type */
#define __bit unsigned char

/* SDCC attributes */
#define __no_init
#define __overlay

/* Common 8051 register defines if needed */
#ifndef sfr
#define sfr(x) volatile unsigned char
#endif
#ifndef sbit
#define sbit unsigned char
#endif
#ifndef bit
#define bit unsigned char
#endif

#endif
