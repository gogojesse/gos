/* some definition and macro reference Linux Kernel Source. */
#include "serial_hw/gos_serial_hw.inc"


.macro	setmode, mode, reg
	msr	cpsr_c, #\mode
.endm

.macro	addruart_current, rx, tmp1, tmp2
	addruart	\rx, \tmp1
.endm

#include "./init/gos_exception_handler.S"

.global    gcurrtask

/*
 * Program Status Register
 */
#define USR_MODE	0b10000
#define FIQ_MODE	0b10001
#define IRQ_MODE	0b10010
#define SVC_MODE	0b10011
#define ABT_MODE	0b10111
#define UDF_MODE	0b11011
#define SYS_MODE	0b11111	
#define PSR_F_BIT	(1 << 6)
#define PSR_I_BIT	(1 << 7)

_gos_reset_handler:
main:
	setmode	SVC_MODE | PSR_F_BIT | PSR_I_BIT, r9 @ ensure svc mode
						     @ and irqs disabled
	ldr	sp, =_gos_stack_end

	setmode	IRQ_MODE | PSR_F_BIT | PSR_I_BIT, r9 @ switch irq mode
						     @ and irqs disabled
	ldr	sp, =_gos_irq_stack_end

	setmode	SVC_MODE, r9                        @ back to svc mode
						     @ with fiq&irqs enabled
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

#if 0
	SUB	lr, lr, #4	@ Adjust the return address
				@ before we save it.
	STMFD	sp!, {lr}	@ Store program counter to stack
	MRS	r14, SPSR	@ Store the SPSR to R14
	STMFD	sp!, {r0-r4, r14} @ Store the R0~R12 and SPSR into stack
#endif

	/* Check current task ID */
	ldr	r12, =gcurrtask		@ load gcurrtask pointer
	ldr	r11, [r12]		@ load task struct
	ldr	r10, [r11]		@ load task id
	@ldr	r9, [r11, #40]		@ r9 point to lr of tcb
	add	r9, r11, #40		@ r9 point to lr of tcb

	sub	lr, lr, #4		@ Adjust the return address
	stmfd	r9!, {lr}		@ Store return addr
	mrs	r14, spsr		@ Store the SPSR to R14
	stmfd	r9!, {r0-r4, r14}	@ Store the R0~R4 and
					@ SPSR into stack

	/* switch to svc mode. */
	ORR	r14, r14, #0x80 	@ !!! testing code.. Set the Irq disable
	MSR	SPSR_csxf, r14		@ Restore status register from r14.

	stmfd	r9!, {sp}		@ Store return addr

	/* context switch to next task */
	ldr	r10, [r11, #44]
	add	r9, r11, #44
	str	r9, [r12]
	@ldr	r9, [r11, #56]		@ r9 point to lr of tcb
	add	r9, r11, #56		@ r9 point to lr of tcb

	LDMFD	r9!, {sp}		@ restore sp
	LDMFD	r9!, {r0-r4, r14}	@ Restore r0~r12 and SPSR.
/*	MSR	SPSR_csxf, r14		@ Restore status register from r14. */
	LDMFD	r9!, {pc}^ 		@ Return from handler.

	@ mov	r0, r1	
	@ bl	print_taskid

	/* store regs into task's sp. */

	/* Need to clear interrupt here. */
/*
	MRS	r12, CPSR 		@ Read the status register.
	ORR	r12, r12, #0x80 	@ Set the I bit
					@ (use 0x40 for the F bit).
	MSR	CPSR_c, r12 		@ Write it back to disable interrupts.
*/
	mov	r0, #'I'
	bl	printch 
	mov	r0, #'R'
	bl	printch 
	mov	r0, #'Q'
	bl	printch 
	mov	r0, #'\n'
	bl	printch 

irq_exit:
	LDMFD	sp!, {r0-r4, r14}	@ Restore r0~r12 and SPSR.
	
	ORR	r14, r14, #0x80 	@ !!! testing code.. Set the Irq disable
	
	MSR	SPSR_csxf, r14		@ Restore status register from r14.
	LDMFD	sp!, {pc}^ 		@ Return from handler.
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

.global _jmp2ctx
_jmp2ctx:
	LDMFD	sp!, {r0-r12, r14}	@ Restore r0~r12 and SPSR.
	MSR	SPSR_csxf, r14		@ Restore status register from r14.
	LDMFD	sp!, {pc}^ 		@ Return from handler.
	b	finished



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

