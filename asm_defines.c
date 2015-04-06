
#include "inc/task.h"

#ifndef offsetof
#define offsetof __builtin_offsetof
#endif

#define DEFINE(sym, val) \
        asm volatile("\n-> " #sym " %0 " #val : : "i" (val))

#define OFFSET(sym, str, mem) \
        DEFINE(sym, offsetof(struct str, mem))

void asm_defines()
{
	DEFINE(TASK_STRUCT_SIZE, sizeof(struct task_struct));
	DEFINE(TCB_FRAME_SIZE, sizeof(struct task_tcb));

	OFFSET(TASK_STATE_OFFSET,task_struct,state);
	OFFSET(TASK_TCB_OFFSET,task_struct,tcb);
	OFFSET(TASK_REENT_OFFSET,task_struct,reent);
}

