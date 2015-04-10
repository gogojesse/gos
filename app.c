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

int task01_func(void *data)
{
	int i = 0;
	spinlock_lock(&test);
	printf("task01_1\n");
	yield_cpu();
	printf("task01_2\n");

	while(1)
	{
		sleep(1);
		printf("1.\n");
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
		sleep(1);
		printf("2.\n");
	}

	return 0;
}


