#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inc/task.h"

struct task_struct gtask[MAX_TASK];
struct task_struct *gcurrtask = gtask;

extern _jmp2ctx(struct task_tcb *tcb);

int task00_func(void *data)
{
	printf("task00\n");
	return 0;
}

int task01_func(void *data)
{
	printf("task01\n");
	return 0;
}

int task02_func(void *data)
{
	printf("task02\n");
	return 0;
}

int init_task_struct(void)
{
	int i;
	unsigned int stack;
	struct task_struct *tsk = gtask;

	/* init task structure. */
	memset(tsk, 0x0, sizeof(struct task_struct) * MAX_TASK);

	stack = (unsigned int)malloc(1024 * 3);

	/* init id. */
	for (i = 0; i < MAX_TASK; i++, tsk++) {
		tsk->id = i;
		tsk->stacksize = 1024;

		if (i == 0)
			tsk->tcb.lr = (unsigned int)task00_func;
		else if (i == 1)
			tsk->tcb.lr = (unsigned int)task01_func;
		else if (i == 2)
			tsk->tcb.lr = (unsigned int)task02_func;
		else
			tsk->tcb.lr = (unsigned int)task00_func;
	
		if (i == 0)
			tsk->tcb.sp = stack;
		else if (i == 1)
			tsk->tcb.sp = stack+1024;
		else if (i == 2)
			tsk->tcb.sp = stack+2048;
		else
			tsk->tcb.sp = stack;
	
		printf("i=%d, lr=0x%08x\n", i , tsk->tcb.lr);
		printf("i=%d, sp=0x%08x\n", i , tsk->tcb.sp);
	}

	//gcurrtask += 3;
	printf("init task structure\n");
}

taskid task_create(unsigned int stacksize, task_func *func)
{
	taskid id = -1;
	struct task_struct *tsk = gtask;
	int i;

	/* Look for unused task space for new request. */
	for (i = 0; i < MAX_TASK; i++, tsk++) {
		if (tsk->state == Task_Unused) {
			id = tsk->id;
			tsk->stacksize	= stacksize;
			tsk->state	= Task_Ready;
			tsk->tcb.lr	= (cpureg)func;
		}
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

int task_scheduler(void)
{
	int i;
	taskid nextid;

	if (gcurrtask) {
		/* 1. Update current tcb.(We did this in assembly handler already. */

		/* 2. Context Switch to next task. Round-Robin for now.  */
		nextid = (gcurrtask->id + 1) % MAX_TASK;
		gcurrtask = &(gtask[nextid]);

	} else {
		/* check idle process is present. */
		gcurrtask = gtask;

		if (gcurrtask->state != Task_Ready) {
			printf("Error: idle task is not configured well.\n");
		}
	}

context_switch:
	/* context switch to new task.*/
	gcurrtask->state = Task_Running;
	_jmp2ctx(&gcurrtask->tcb);

	return 0;
}

void yield_cpu(void)
{
	/* Save Address, LR-8 */

	/* Switch to another task. */
}


