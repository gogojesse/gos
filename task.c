#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/task.h"

struct task_struct gtask[MAX_TASK];
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
	}

	printf("init task structure\n");
}

extern int idle_task(void *data);
cpureg idle_sp = 0x0;

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
			tsk->tcb.pc	= (cpureg)func;
			tsk->tcb.sp	= (cpureg)stack;

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
			//printf("Switch to task %02d.\n", gcurrtask->id);
			break;
		}
	}

context_switch:
	/* context switch to new task.*/
	gcurrtask->state = Task_Running;

	return 0;
}

void yield_cpu(void)
{
        timer0_clear_int();

        asm ("pop {fp, lr}\n\t"
             /* Save Address, LR-8 */
             "stmfd sp!, {r0-r2}\n\t"
             "stmfd sp!, {r3-r7}\n\t"
             "stmfd sp!, {r8-r12}\n\t"
             "mrs r0, spsr\n\t"
             /* Check current task ID */
             "ldr     r8, =gcurrtask\n\t"
             "ldr     r7, [r8]\n\t"
             "ldr     r6, [r7]\n\t"
             "ldr     r6, [r7, #8]\n\t"
             "add     r9, r7, #80\n\t"
             /* Save current context. */
             "stmfd   r9!, {lr}\n\t"
             "stmfd   r9!, {r0}\n\t"
             "ldmfd   sp!, {r1-r5}\n\t"
             "stmfd   r9!, {r1-r5}\n\t"
             "ldmfd   sp!, {r1-r5}\n\t"
             "stmfd   r9!, {r1-r5}\n\t"
             "ldmfd   sp!, {r1-r3}\n\t"
             "stmfd   r9!, {r1-r3}\n\t"
             "stmfd   r9!, {lr}\n\t"
             "stmfd   r9!, {sp}");

        task_scheduler();
        timer0_enable();
        /* Switch to another task. */

        asm ("ldr     r8, =gcurrtask\n\t"
             "ldr     r7, [r8]\n\t"
             "ldr     r6, [r7]\n\t"
             "add     r12, r7, #12\n\t"
             "LDMFD   r12!, {sp}\n\t"
             "LDMFD   r12!, {lr}\n\t"
             "LDMFD   r12!, {r0-r10}\n\t"
             "LDR     r11, [r12, #12]\n\t"
             "push    {r11}\n\t"
             "LDR     r11, [r12, #8]\n\t"
             "MSR     SPSR_csxf, r11\n\t"
             "LDR     r11, [r12]\n\t"
             "LDR     r12, [r12, #4]\n\t"
             "pop     {pc}^");
}


