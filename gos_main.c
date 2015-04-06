extern void gos_printf(const char* format, ...);

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <unistd.h>
#include <sys/time.h>

#include "inc/task.h"
#include "inc/spinlock.h"
#include "inc/platform-defs.h"
#include "inc/timer.h"
#include "inc/irq.h"
#include "inc/rtc.h"
#include "inc/io.h"

spinlock_t test = spinlock_locked;
static unsigned int gos_cpu_info = 0x0;

extern int timer_init (void);
extern int timer1_init (void);
extern void vic_init2(void *base);

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

#define DBG_LOOP_COUNT	100000000

int task01_func(void *data)
{
	int i = 0;
	spinlock_lock(&test);
	printf("task01_1\n");
	yield_cpu();
	printf("task01_2\n");

	while(1)
	{
		if ((i++ % DBG_LOOP_COUNT) == 0)
			printf("1.\n");
		//printf("task01_func print\n");
	}

	return 0;
}

int task02_func(void *data)
{
	int i = 0;
	printf("task02_1\n");
	yield_cpu();
	printf("task02_2\n");
	spinlock_unlock(&test);

	while (1)
	{
		if ((i++ % DBG_LOOP_COUNT) == 0)
			printf("2.\n");
		//printf("task02_func print\n");
	}

	return 0;
}

int idle_task(void *data)
{
	int i = 0;
	char *buf;
	char *buf2;

	printf("idle task_1\n");
	yield_cpu();
	printf("idle task_2\n");

	while(1)
	{
		buf = (char *)malloc(128);

		if ((i++ % (DBG_LOOP_COUNT/100)) == 0)
		/* gettimeofday */
		{
			unsigned long i = 0;

			struct timeval tv;
			struct timezone tz;

			buf2 = (char *)malloc(128);
			gettimeofday (&tv, &tz);
			printf("tv_sec; %d\n", tv.tv_sec);
			printf("tv_usec; %d\n", tv.tv_usec);
			printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
			printf("tz_dsttime, %d\n", tz.tz_dsttime);
			printf("gos time test\n");
			gettimeofday (&tv, &tz);
			printf("tv_sec; %d\n", tv.tv_sec);
			printf("tv_usec; %d\n", tv.tv_usec);
			printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
			printf("tz_dsttime, %d\n", tz.tz_dsttime);

			free(buf2);
		}

		free(buf);
//		printf("idle_task print 1.\n");
	}
	return 0;
}

static int ddd = 0;
int os_timer_isr(IRQ_RESOURCE res) /* called in IRQ mode. */
{
	printf("os_timer_isr() %d\n", ddd++);
	timer1_clear_int();
	timer1_enable();

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

	/* set up a isr for HW timer 2. */
	timer1_init();
	irq_reg(5, os_timer_isr, 0, SHARED_IRQ); 

	/* Setup System Timer. */
	printf("Setup a free running timer\n");
	timer_init();

	/* init rtc */
	rtc_init();

	printf("os_main is finished. Enter idle task and never return.\n");
	idle_task(0);
}
