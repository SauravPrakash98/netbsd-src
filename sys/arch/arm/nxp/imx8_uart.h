#ifndef _ARM_IMX8_UART_H
#define _ARM_IMX8_UART_H

#include <arch/evbarm/fdt/platform.h>

#define IMX8_URXD0 0x0 
#define IMX8_URTX0 0x40
#define IMX8_UCR1  0x80 
#define IMX8_UCR2  0x84
#define IMX8_UCR3  0x88
#define IMX8_UCR4  0x8c
#define IMX8_UFCR  0x90
#define IMX8_USR1  0x94
#define IMX8_USR2  0x98
#define IMX8_UESC  0x9c
#define IMX8_UTIM  0xa0
#define IMX8_UBIR  0xa4
#define IMX8_UBMR  0xa8
#define IMX8_UBRC  0xac
#define IMX8_UTS   0xb4

#define UTS_TXFULL	 (1<<4)

#endif /* _ARM_IMX8_UART_H */
