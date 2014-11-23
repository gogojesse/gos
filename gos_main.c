extern void gos_printf(const char* format, ...);

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <unistd.h>
#include "inc/task.h"

static unsigned int gos_cpu_info = 0x0;

void print_cpuinfo(void)
{
#define L1_DCache	(1 << 2)
#define L1_ICache	(1 << 12)

	asm (
		"MRC p15, 0, r0, c1, c0, 0\n\t"
		"mov %[result], r0"
			: [result] "=r" (gos_cpu_info)
	);

	printf("\nCurrent CPU Cache Setting : 0x%08x\n", gos_cpu_info);
	printf("------------------------------------------\n");
	printf("L1 Instruction Cache      : %s\n", (gos_cpu_info & L1_ICache) ? "On":"Off");
	printf("L1 Data        Cache      : %s\n", (gos_cpu_info & L1_DCache) ? "On":"Off");

	printf("\n");
}

int gos_enable_idcache(void)
{
	gos_cpu_info |= L1_DCache|L1_ICache;

	printf("Enable ICache and DCache\n");
	asm (
		"MCR p15, 0, %[value], c1, c0, 0\n\t" : : [value] "r" (gos_cpu_info) :
	);


	return 0;
}

void print_taskid(unsigned int taskid)
{
	printf("Task ID = %d\n", taskid);
}

extern int timer_init (void);
extern void vic_init2(void *base);

int task01_func(void *data)
{

	printf("task01\n");

	while(1) { ; }

	return 0;
}

int task02_func(void *data)
{
	printf("task02\n");
	while(1) { ; }

	return 0;
}

int idle_task(void *data)
{
	printf("idle task\n");
	while(1) { ; }
	return 0;
}

void os_main(void)
{
	int i;
	char *ptr;
	int alloc_size;
	char *dst, *src;

	/* Print CPU Info */
	print_cpuinfo();

	/* Enable i&d cache. */
	gos_enable_idcache();
	print_cpuinfo();

	/* Initialize the task structures. */
	init_task_struct();

	task_create(1024, idle_task);
	task_create(1024, task01_func);
	task_create(1024, task02_func);

	/* enable interrupt controller. */
	vic_init2((void *)0x10140000);

	/* Setup Timer. */
	printf("Setup a free running timer\n");
	timer_init();

	/* C library Tests. */
	printf("Test Some C Library APIs\n");
	printf("------------------------------------------\n");
	alloc_size = 1;
	ptr = realloc(ptr, alloc_size);
	dst = malloc(100);
	src = "test";

	printf("os_main is finished.\n");
	while(1) {
	//	printf("o");
	}

}
