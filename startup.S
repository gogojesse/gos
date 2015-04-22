/* some definition and macro reference Linux Kernel Source. */
#include "serial_hw/serial_hw.inc"
#include "inc/asm_defines.h"

.macro	setmode, mode
	msr	cpsr_c, #\mode
.endm

.macro	addruart_current, rx, tmp1, tmp2
	addruart	\rx, \tmp1
.endm

#include "./init/exception_handler.S"

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

_os_reset_handler:
main:
	setmode	SVC_MODE | PSR_F_BIT | PSR_I_BIT     @ ensure svc mode
						     @ and irqs disabled
	ldr	sp, =_os_stack_end

	setmode	IRQ_MODE | PSR_F_BIT | PSR_I_BIT     @ switch irq mode
						     @ and irqs disabled
	ldr	sp, =_os_irq_stack_end

	setmode	SVC_MODE                            @ back to svc mode
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
	ldr	sp, =_os_stack_end
	bl	os_main

finished:
	b	.

_os_undfd_handler:
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

.macro	_ctx_sw_bh
	/* schduler should assign new task to gcurrtask. */
	ldr	r1, =gcurrtask			@ load gcurrtask pointer
	ldr	r1, [r1]			@ load task struct
	ldr	r2, [r1]			@ load task id
	add	r12, r1, #TASK_TCB_OFFSET	@ r12 point to sp of tcb

	/* restore sp and lr. */
	LDMFD	r12!, {sp, lr}		@ Restore sp, lr

	/* Switch back to IRQ Mode */
	setmode	IRQ_MODE | PSR_F_BIT | PSR_I_BIT	@ switch irq mode
							@ fiq disabled
							@ irq disabled

	/* restore spsr */
	LDMFD	r12!, {r11}
	MSR	SPSR_csxf, r11

	mov	lr, r12

	/* restore. r0~r12, pc, should never return. */
	LDMFD	lr!, {r0-r12, pc}^	@ Restore r0~r12, pc

	b	finished
.endm


.global _os_irq_handler
_os_irq_handler:

	/* Save partial context to irq_sp temporarily. */
	stmfd sp!, {r9-r10}	/* store r9~r10 to irq_sp */	
	stmfd sp!, {r11-r12}	/* store r11~r12 to irq_sp */	

	/* At the moment, r9~r12 should be available to use. */
	mrs r9, spsr			@ store spsr to r9

	/* Get tcb of current task */
	ldr	r10, =gcurrtask		@ load gcurrtask pointer
	ldr	r10, [r10]		@ load task struct
	ldr	r11, [r10]		@ load task id
	ldr	r11, [r10, #TASK_STATE_OFFSET]	@ load task state
	add	r12, r10, #TASK_REENT_OFFSET	@ r9 point to pc of tcb

	/* calc lr for task resume */
	@cmp	r11, #1
	@sub	lr, lr, #8		@ Adjust the return address
	sub	lr, lr, #4		@ Adjust the return address

	/* Save current context. Please refer to inc/task.h */
	stmfd	r12!, {lr}		@ Store PC 

	/* Save r12 ~ r0 */
	ldmfd	sp!, {r10-r11}
	stmfd	r12!, {r10-r11}		@ Store r11 ~ r12
	ldmfd	sp!, {r10-r11}
	stmfd	r12!, {r10-r11}		@ Store r9 ~ r10
	stmfd	r12!, {r0-r8}		@ Store r0 ~ r8
	stmfd	r12!, {r9}		@ Store spsr

load_isr_vec:
	LDR	r8, =0x10140030

	/*
	 * Switch to SVC to get sp and lr.
	 * Keep FIQ and IRQ in disabled.
	 */
	setmode	SVC_MODE | PSR_F_BIT | PSR_I_BIT	@ ensure svc mode
							@ fiq disabled
							@ irq disabled

	/* Save SVC sp.  */
	stmfd	r12!, {lr}		@ Store lr
	stmfd	r12!, {sp}		@ Store sp

	/* Context is saved, Jump to vector. */
	LDR	pc, [r8]

	b	finished

.global _os_isr
_os_isr:

	/* ACK VIC to clear INTR */
	str	r0, [r8]

	bl	shared_isr

	_ctx_sw_bh

/*
 * Enter this routine under SVC Mode with FIQ and IRQ disabled.
 */
.global _sys_timer
_sys_timer:

	/* ACK VIC to clear INTR */
	str	r0, [r8]

	/* We've stored all context of current task. */
	/* Run scheduler to pick up next task. */
	bl	task_scheduler

	_ctx_sw_bh

/* For print out debugging. */
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

/* Define msg for some exception. */
OS_Msg:
_str_start:
	.string	"GOS Booting..."
_str_end:

.align 2
undfd_instr:
_str_2start:
	.string	"undefined instruction"
_str_2end:

