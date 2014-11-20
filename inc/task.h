#ifndef _task_h_
#define _task_h_

typedef int taskid;
typedef unsigned int cpureg;
typedef int (*task_func)(void *data); 

typedef enum {
	Task_Unused	= 0x0,
	Task_Ready	= 0x1,
	Task_Pause	= 0x2,
	Task_Running	= 0x4,
	Task_Stop	= 0x8,
	Task_Error	= 0x100
} taskstate;

struct task_tcb {
	cpureg sp;		/*                    12 */
	cpureg r0;		/*                    16 */
	cpureg r1;		/*                    20 */
	cpureg r2;		/*                    24 */
	cpureg r3;		/*                    28 */
	cpureg r4;		/*                    32 */
	cpureg spsr;		/*                    36 */
	cpureg lr;		/*                    40 */
};

struct task_struct {
	taskid id;		/* task id             0 */
	unsigned int stacksize; /* stack size          4 */
	taskstate state;	/* task state          8 */
	struct task_tcb tcb;	/* task control block 12 */
};

#define MAX_TASK	10

int init_task_struct(void);
taskid task_create(unsigned int stacksize, task_func *func); 
taskstate task_state(taskid id); 
int task_scheduler(void);
void yield_cpu(void);

#endif /* _task_h_h */
