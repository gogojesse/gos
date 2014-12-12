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

    asm ("pop {fp, lr}");

    /* Save Address, LR-8 */
    asm ("ldr r12, =gcurrtask");
    asm ("ldr r11, [r12]");
    asm ("ldr r10, [r11]");
    asm ("ldr r10, [r11, #8]");
    asm ("add r9, r11, #48");

    asm ("stmfd r9!, {lr}");
    asm ("mrs r14, spsr");
    asm ("stmfd r9!, {r0-r4, r14}");

    asm ("cmp r10, #1");
    asm ("ldreq sp, =idle_sp");

    asm ("stmfd r9!, {lr}");
    asm ("stmfd r9!, {sp}");

    task_scheduler();
    timer0_enable();

    /* Switch to another task. */
    asm ("ldr r12, =gcurrtask");
    asm ("ldr r11, [r12]");
    asm ("ldr r10, [r11]");
    asm ("add r9, r11, #12");
    asm ("LDMFD r9!, {sp}");
    asm ("LDMFD r9!, {lr}");
    asm ("LDMFD r9!, {r0-r4, r12}");
    asm ("MSR SPSR_csxf, r12");
    asm ("LDMFD r9!, {pc}^");
}


