#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <unistd.h>
#include <sys/time.h>

#include "inc/task.h"
#include "inc/platform-defs.h"
#include "inc/timer.h"
#include "inc/irq.h"
#include "inc/rtc.h"
//#include "inc/io.h"

static unsigned int os_cpu_info = 0x0;

void print_cpuinfo(void)
{
#define L1_DCache	(1 << 2)
#define L1_ICache	(1 << 12)

	asm (
		"MRC p15, 0, r0, c1, c0, 0\n\t"
		"mov %[result], r0"
			: [result] "=r" (os_cpu_info)
	);

	printf("\nCurrent CPU Cache Setting : 0x%08x\n", os_cpu_info);
	printf("------------------------------------------\n");
	printf("L1 Instruction Cache      : %s\n", (os_cpu_info & L1_ICache) ? "On":"Off");
	printf("L1 Data        Cache      : %s\n", (os_cpu_info & L1_DCache) ? "On":"Off");

	printf("\n");
}

int os_enable_idcache(void)
{
	os_cpu_info |= L1_DCache|L1_ICache;

	printf("Enable ICache and DCache\n");
	asm (
		"MCR p15, 0, %[value], c1, c0, 0\n\t" : : [value] "r" (os_cpu_info) :
	);


	return 0;
}

void print_taskid(unsigned int taskid)
{
	printf("Task ID = %d\n", taskid);
}

/* tasks and ISR declaration. */
extern int task01_func(void *data);
extern int task02_func(void *data);
extern int idle_task(void *data);
extern int os_timer_isr(IRQ_RESOURCE res);

void os_main(void)
{
	int i;
	char *ptr;
	int alloc_size;
	char *dst, *src;

	/* Print CPU Info */
	print_cpuinfo();

	/* Enable i&d cache. */
	os_enable_idcache();
	print_cpuinfo();

	/* Initialize the task structures. */
	init_task_struct();

	task_create(1024, idle_task);
	task_create(1024, task01_func);
	task_create(1024, task02_func);

	/* enable interrupt controller. */
	vic_init2((void *)VIC_REGBASE);

#if 0	/* unmark these lines for basic tests of C library. */
	/* C library Tests. */
	printf("Test Some C Library APIs\n");
	printf("------------------------------------------\n");
	alloc_size = 1;
	ptr = realloc(ptr, alloc_size);
	dst = malloc(100);
	src = "test";
#endif

	/* init irq mechanism. */
	irq_init();

	/* set up time. (ustimer+RTC) */
	us_timer_init();
	rtc_init();

	/* set up a isr for HW timer 2. */
	timer1_init();
	irq_reg(5, os_timer_isr, 0, SHARED_IRQ); 

	/* Setup System Timer. */
	timer_init();

	/* os_main is finished. Enter idle task and never return. */
	idle_task(0);
}
