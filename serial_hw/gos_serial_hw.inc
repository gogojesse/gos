/*
 * Copy HW definition from Linux Kernel Source.
 */
/* 01 .for goldfish. uart hw definition
 *
 */

//#define GOLDFISH_PLATFORM
#ifdef GOLDFISH_PLATFORM
#define IO_BASE			0xfe000000                 // VA of IO 
#define IO_SIZE			0x00800000                 // How much?
#define IO_START		0xff000000                 // PA of IO

#define GOLDFISH_TTY_BASE       (0x2000)
#define GOLDFISH_TTY_PUT_CHAR_PHYS (IO_START + GOLDFISH_TTY_BASE)
#define GOLDFISH_TTY_PUT_CHAR_BASE (IO_BASE + GOLDFISH_TTY_BASE)

/* Print Rountine. */
	.macro	addruart, rp, rv, rtmp
	ldr		\rp, =GOLDFISH_TTY_PUT_CHAR_PHYS
	ldr		\rv, =GOLDFISH_TTY_PUT_CHAR_BASE
	.endm

	.macro	senduart,rd,rx
	teq	\rx, #0
	strne	\rd, [\rx]
	.endm

	.macro	waituart,rd,rx
	.endm

	.macro	busyuart,rd,rx
	.endm

#else
/* 02 .for versatile uart hw definition
 *
 */

#include "serial_hw/serial_hw.h"

/* Print Rountine. */
	.macro	addruart, rp, rv, tmp
		mov	\rp,      #0x001F0000
		orr	\rp, \rp, #0x00001000
		orr	\rv, \rp, #0xf1000000	@ virtual base
		orr	\rp, \rp,  #0x10000000	@ physical base
		.endm

                .macro  senduart,rd,rx
                strb    \rd, [\rx, #UART01x_DR]
                .endm

                .macro  waituart,rd,rx
1001:           ldr     \rd, [\rx, #UART01x_FR]
                tst     \rd, #UART01x_FR_TXFF
                bne     1001b
                .endm

                .macro  busyuart,rd,rx
1001:           ldr     \rd, [\rx, #UART01x_FR]
                tst     \rd, #UART01x_FR_BUSY
                bne     1001b
                .endm
#endif
