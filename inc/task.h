#ifndef _task_h_
#define _task_h_

typedef int taskid;
typedef unsigned int cpureg;
typedef int (*task_func)(void *data); 

typedef enum {
	Task_Unused	 = 0x0,
	Task_Ready	 = 0x1,
	Task_Pause	 = 0x2,
	Task_Running     = 0x4,
	Task_Stop	 = 0x8,
	Task_Error	 = 0x100
} taskstate;

struct task_tcb {
	cpureg sp;		/*                    12 */
	cpureg lr;		/*                    16 */
	cpureg r0;		/*                    20 */
	cpureg r1;		/*                    24 */
	cpureg r2;		/*                    28 */
	cpureg r3;		/*                    32 */
	cpureg r4;		/*                    36 */
	cpureg r5;		/*                    40 */
	cpureg r6;		/*                    44 */
	cpureg r7;		/*                    48 */
	cpureg r8;		/*                    52 */
	cpureg r9;		/*                    56 */
	cpureg r10;		/*                    60 */
	cpureg r11;		/*                    64 */
	cpureg r12;		/*                    68 */
	cpureg spsr;            /*                    72 */
	cpureg pc;		/*                    76 */
};

struct task_struct {
	taskid id;		/* task id             0 */
	unsigned int stacksize; /* stack size          4 */
	taskstate state;	/* task state          8 */
	struct task_tcb tcb;	/* task control block 12 */
};

//#define MAX_TASK	10
#define MAX_TASK	3

int init_task_struct(void);
taskid task_create(unsigned int stacksize, task_func func); 
taskstate task_state(taskid id); 
int task_scheduler(void);
void yield_cpu(void);

#endif /* _task_h_h */
