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
#include "inc/io.h"

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

		sleep(3);
		/* gettimeofday */
		{
			unsigned long i = 0;

			struct timeval tv;
			struct timezone tz;

			buf2 = (char *)malloc(128);
			sprintf(buf, "=================");
			printf("%s\n", buf);
			gettimeofday (&tv, &tz);
			printf("tv_sec; %d\n", tv.tv_sec);
			printf("tv_usec; %d\n", tv.tv_usec);
			printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
			printf("tz_dsttime, %d\n", tz.tz_dsttime);
			printf("os time test\n");
			gettimeofday (&tv, &tz);
			printf("tv_sec; %d\n", tv.tv_sec);
			printf("tv_usec; %d\n", tv.tv_usec);
			printf("tz_minuteswest; %d\n", tz.tz_minuteswest);
			printf("tz_dsttime, %d\n", tz.tz_dsttime);
			printf("%s\n", buf);
			free(buf2);
		}

		free(buf);
	}
	return 0;
}


