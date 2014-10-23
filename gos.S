/*
 * PSR bits
 */
#define SVC_MODE	0x00000013
#define PSR_F_BIT	0x00000040
#define PSR_I_BIT	0x00000080

#include "gos_serial_hw.inc"

.macro	setmode, mode, reg
	msr	cpsr_c, #\mode
.endm

.macro	addruart_current, rx, tmp1, tmp2
	addruart	\rx, \tmp1
.endm

main:
	setmode	PSR_F_BIT | PSR_I_BIT | SVC_MODE, r9 @ ensure svc mode
						@ and irqs disabled
	mrc	p15, 0, r9, c0, c0		@ get processor id

	mov	r4, #OS_Maintainer

	mov	r5, #(_str_end - _str_start)
	mov	r6, #0

loop:
	ldrb	r0, [r4]
	bl	printch 
	add	r6, r6, #1
	cmp	r6, r5
	beq	out
	add	r4, r4, #1
	b	loop
out:

	mov	r0, #'\n'
	bl	printch 

	mov	sp, #0x4000
	bl	os_main

finished:
	mov	r0, r0
	b	finished

.global printascii
printascii:
		addruart_current r3, r1, r2
		b	2f
1:		waituart r2, r3
		senduart r1, r3
		busyuart r2, r3
		teq	r1, #'\n'
		moveq	r1, #'\r'
		beq	1b
2:		teq	r0, #0
		ldrneb	r1, [r0], #1
		teqne	r1, #0
		bne	1b
		mov	pc, lr

.global printch
printch:
		addruart_current r3, r1, r2
		mov	r1, r0
		mov	r0, #0
		b	1b

OS_Maintainer:
_str_start:
	.string	"gogojesse <gogojesseco@gmail.com>"
_str_end:

