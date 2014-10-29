/* some definition and macro reference Linux Kernel Source. */
/*
 * PSR bits
 */
#define SVC_MODE	0x00000013
#define PSR_F_BIT	0x00000040
#define PSR_I_BIT	0x00000080

#include "serial_hw/gos_serial_hw.inc"

.macro	setmode, mode, reg
	msr	cpsr_c, #\mode
.endm

.macro	addruart_current, rx, tmp1, tmp2
	addruart	\rx, \tmp1
.endm

#include "./init/gos_exception_handler.S"

_gos_reset_handler:
main:
	/*setmode	PSR_F_BIT | PSR_I_BIT | SVC_MODE, r9 @ ensure svc mode
						@ and irqs disabled
	*/
	setmode	SVC_MODE, r9 @ ensure svc mode
						@ and irqs disabled

	/* 1) Early Print Test. */
	mov	r4, #OS_Msg
	mov	r5, #(_str_end - _str_start)
	mov	r6, #0

	/* 2) Output char via UART0 in turn. */
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

	/* 3) Setup stack point before calling C. */
	ldr	sp, =_gos_stack_end
	bl	os_main

finished:
	b	.

_gos_undfd_handler:
	mov	r0, #'\n'
	bl	printch 

	/* print error message. */
	mov	r4, #undfd_instr
	mov	r5, #(_str_2end - _str_2start)
	mov	r6, #0

loop2:
	ldrb	r0, [r4]
	bl	printch 
	add	r6, r6, #1
	cmp	r6, r5
	beq	out2
	add	r4, r4, #1
	b	loop2
out2:

	mov	r0, #'\n'
	bl	printch 
	b	finished

.global _gos_irq_handler
_gos_irq_handler:
	mov	r0, #'\n'
	bl	printch 

	/* print error message. */
	mov	r4, #irq_string
	mov	r5, #(_str_3end - _str_3start)
	mov	r6, #0

loop3:
	ldrb	r0, [r4]
	bl	printch 
	add	r6, r6, #1
	cmp	r6, r5
	beq	out2
	add	r4, r4, #1
	b	loop2
out3:

	mov	r0, #'\n'
	bl	printch 
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

OS_Msg:
_str_start:
	.string	"GOS Booting..."
_str_end:

.align 2
undfd_instr:
_str_2start:
	.string	"undefined instruction"
_str_2end:

.align 2
irq_string:
_str_3start:
	.string	"irq interrupt"
_str_3end:

