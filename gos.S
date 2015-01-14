/* some definition and macro reference Linux Kernel Source. */
#include "serial_hw/gos_serial_hw.inc"


.macro	setmode, mode
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
	setmode	SVC_MODE | PSR_F_BIT | PSR_I_BIT     @ ensure svc mode
						     @ and irqs disabled
	ldr	sp, =_gos_stack_end

	setmode	IRQ_MODE | PSR_F_BIT | PSR_I_BIT     @ switch irq mode
						     @ and irqs disabled
	ldr	sp, =_gos_irq_stack_end

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

	mov	r0, #'Q'
	bl	printch 
1:

	/* Clear INT & Reset Timer. */
	bl	timer0_clear_int
	bl	timer0_enable

	/* Context Switch */
	LDMFD	sp!, {r0-r4, r14}	@ Restore r0~r12 and SPSR.
	MSR	SPSR_csxf, r14		@ Restore status register from r14.
	LDMFD	sp!, {pc}^ 		@ Return from handler.
#endif

	stmfd sp!, {r0-r2}
	stmfd sp!, {r3-r7}
	stmfd sp!, {r8-r12}
	mrs r0, spsr

	/* Check current task ID */
	ldr	r8, =gcurrtask		@ load gcurrtask pointer
	ldr	r7, [r8]		@ load task struct
	ldr	r6, [r7]		@ load task id
	ldr	r6, [r7, #8]		@ load task state
	add	r9, r7, #80		@ r9 point to pc of tcb

	/* calc lr */
	cmp	r6, #1
	#ldreq	lr, =idle_task
	#subne	lr, lr, #4		@ Adjust the return address
	sub	lr, lr, #4		@ Adjust the return address

	/* Save current context. */
	stmfd	r9!, {lr}		@ Store return addr
	stmfd	r9!, {r0}		@ Store spsr to stack
	ldmfd	sp!, {r1-r5}
	stmfd	r9!, {r1-r5}		@ Store r8 ~ r12
	ldmfd	sp!, {r1-r5}
	stmfd	r9!, {r1-r5}		@ Store r3 ~ r7
	ldmfd	sp!, {r1-r3}
	stmfd	r9!, {r1-r3}		@ Store r0 ~ r2

	/* Clear INT & Reset Timer. */
	bl	timer0_clear_int

	/* switch to svc mode. */
	setmode	SVC_MODE                            @ back to svc mode

	#cmp	r6, #1
	#ldreq	sp, =idle_sp

	/* Save SVC sp.  */
	stmfd	r9!, {lr}		@ Store lr
	stmfd	r9!, {sp}		@ Store sp

	/* Run scheduler */
#if 1
	bl	task_scheduler
#else
	/* context switch to next task */
	cmp	r10, #2
	ldrne	r10, [r11, #44]
	ldreq	r10, [r11, #-44]
	addne	r9, r11, #44
	addeq	r9, r11, #-44
	str	r9, [r12]
	add	r9, r11, #56		@ r9 point to lr of tcb

	LDMFD	r9!, {sp}		@ restore sp
	LDMFD	r9!, {lr} 		@ Return from handler.
	LDMFD	r9!, {r0-r4, r12}	@ Restore r0~r4 and SPSR.
	MSR	SPSR_csxf, r12		@ Restore status register from r12.
	LDMFD	r9!, {pc}^ 		@ Return from handler.
	b	.
#endif

	bl	timer0_enable

	/* context switch to next task */
	ldr	r8, =gcurrtask		@ load gcurrtask pointer
	ldr	r7, [r8]		@ load task struct
	ldr	r6, [r7]		@ load task id
	add	r12, r7, #12		@ r12 point to sp of tcb
	LDMFD	r12!, {sp}		@ restore sp
	LDMFD	r12!, {lr}		@ restore lr
	LDMFD	r12!, {r0-r10}	@ Restore r0~r10
	LDR	r11, [r12, #12]
	push	{r11}		@ Push pc
	LDR	r11, [r12, #8]
	MSR	SPSR_csxf, r11		@ Restore status register from r11.
	LDR	r11, [r12]
	LDR	r12, [r12, #4]
	pop	{pc}^

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

