#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/task.h"

/* newlib context data. */
extern struct _reent * _impure_ptr;

struct task_struct gtask[MAX_TASK];
struct _reent _reent_ctx[MAX_TASK];
struct task_struct *gcurrtask = gtask;

int init_task_struct(void)
{
	int i;
	struct task_struct *tsk = gtask;

	/* init task structure. */
	memset(tsk, 0x0, sizeof(struct task_struct) * MAX_TASK);

	/* init id. */
	for (i = 0; i < MAX_TASK; i++, tsk++) {
		tsk->id = i;
		if (0 == i) {
			tsk->reent = _impure_ptr;
		} else {
			tsk->reent = &_reent_ctx[i];
			_REENT_INIT_PTR(tsk->reent);
		}
	}

	printf("init task structure\n");
}

extern int idle_task(void *data);
cpureg idle_sp = 0x0;

/*
 * Program Status Register
 * #define USR_MODE	0b10000
 * #define FIQ_MODE	0b10001
 * #define IRQ_MODE	0b10010
 * #define SVC_MODE	0b10011
 * #define ABT_MODE	0b10111
 * #define UDF_MODE	0b11011
 * #define SYS_MODE	0b11111	
 * #define PSR_F_BIT	(1 << 6)
 * #define PSR_I_BIT	(1 << 7)
 */



taskid task_create(unsigned int stacksize, task_func func)
{
	taskid id = -1;
	struct task_struct *tsk = gtask;
	int i;
	cpureg stack;

	stack = (cpureg)malloc(stacksize);
	/* Look for unused task space for new request. */
	for (i = 0; i < MAX_TASK; i++, tsk++) {
		if (tsk->state == Task_Unused) {
			id = tsk->id;
			tsk->stacksize	= stacksize;
			tsk->state	= Task_Ready;
			tsk->tcb.sp	= (cpureg)stack;
			tsk->tcb.lr	= (cpureg)0x123456;
			tsk->tcb.spsr	= (cpureg)0x13;		/* SVC Mode */
			tsk->tcb.pc	= (cpureg)func;

			printf("Create Task %d\n", id);	
			printf("    pc=0x%08x\n", tsk->tcb.pc);
			printf("    sp=0x%08x\n", tsk->tcb.sp);
			break;
		}
	}

	if (0 == id) {
		idle_sp = stack;
	}


	return id;
}

taskstate task_state(taskid id)
{
	taskstate state = Task_Error;
	struct task_struct *tsk = gtask;

	state = tsk[id].state;

	return state;
}

extern void vic_clearint(void *base, unsigned long bitmask);
int task_scheduler(void)
{
	int i, j;
	taskid nextid;

	/*
	 * 0. Clear systeim timer.
	 */
	timer0_clear_int();

	/*
	 * 1. Update current task state.
	 *    tcb was store in handler already.
	 */
	gcurrtask->state = Task_Pause;

	/*
	 * 2. Prepare forContext Switch to next
	 *    task. Round-Robin for now. 
	 */
	j = 1;
	for (i = j; i <= (MAX_TASK * j); i+=j) {
		nextid = (gcurrtask->id + i) % MAX_TASK;
		//printf("try %d.\n", nextid);
			
		if (gtask[nextid].state & (Task_Ready|Task_Pause)) {
			/* pick next task. */
			gcurrtask = &(gtask[nextid]);
			_impure_ptr = gcurrtask->reent;
			//printf("Switch to task %02d.\n", gcurrtask->id);
			break;
		}
	}

context_switch:
	/* context switch to new task.*/
	gcurrtask->state = Task_Running;

	/* Re-enable system timer. */
	timer0_enable();

	return 0;
}

void yield_cpu(void)
{
	/* disable system timer first. */
        timer0_clear_int();

	/*
	 * Pre-save context
	 * 1. pop fp, lr
	 * 2. save r0 to sp
	 * 3. save current spsr to r0
	 * 4. disable IRQ and FIQ
	 * */
        asm ("pop {fp, lr}\n\t"
             /* Save Address, LR-8 */
             "stmfd sp!, {r0-r2}\n\t"
             "stmfd sp!, {r3-r7}\n\t"
             "stmfd sp!, {r8-r12}\n\t"
             "mrs r0, cpsr\n\t"
             "msr cpsr_c, 0b11010011\n\t"	/* switch to SVC Mode. */
             /* Check current task ID */
             "ldr     r8, =gcurrtask\n\t"
             "ldr     r7, [r8]\n\t"
             "ldr     r6, [r7]\n\t"
             "ldr     r6, [r7, #8]\n\t"
             "add     r9, r7, #80\n\t"
             /* Save current context. */
             "stmfd   r9!, {lr}\n\t"		/* pc */
             "ldmfd   sp!, {r1-r5}\n\t"		/* r12~r8 */
             "stmfd   r9!, {r1-r5}\n\t"
             "ldmfd   sp!, {r1-r5}\n\t"		/* r7~r3 */
             "stmfd   r9!, {r1-r5}\n\t"
             "ldmfd   sp!, {r1-r3}\n\t"		/* r2~r0 */
             "stmfd   r9!, {r1-r3}\n\t"
             "stmfd   r9!, {r0}\n\t"		/* spsr */
             "stmfd   r9!, {lr}\n\t"		/* lr */
             "stmfd   r9!, {sp}");		/* sp */

	/* pick up new task */
        task_scheduler();

	/* Re-enable system timer. */
        timer0_enable();

        /* Switch to another task. */
        asm ("ldr     r8, =gcurrtask\n\t"
             "ldr     r7, [r8]\n\t"
             "ldr     r6, [r7]\n\t"
             "add     r12, r7, #12\n\t"
             "LDMFD   r12!, {sp}\n\t"		/* sp */
             "LDMFD   r12!, {lr}\n\t"		/* lr */
	     "LDMFD   r12!, {r11}\n\t"		/* spsr */
	     "msr     cpsr_c, 0b11010010\n\t"	/* switch to IRQ Mode. */
	     "MSR     SPSR_csxf, r11\n\t"
	     "mov     lr, r12\n\t"		/* use irq_lr as stack. */
	     "LDMFD	lr!, {r0-r12, pc}^\n\t"); /* @ Restore r0~r12, pc */
}


