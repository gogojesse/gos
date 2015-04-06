#include <sys/unistd.h>
#include <sys/time.h>
#include "inc/task.h"

unsigned sleep(unsigned int __seconds)
{
	struct timeval tv1, tv2;
	struct timezone tz1, tz2;

	if (__seconds <= 0)
		return 0;

	/* Get current time. */	
	gettimeofday (&tv1, &tz1);

	do {
		yield_cpu();

		/* check time expired. */
		gettimeofday (&tv2, &tz2);
	} while (((tv2.tv_sec - tv1.tv_sec) < __seconds) || (tv2.tv_usec < tv1.tv_usec));

	return 0; /* requested time has elapsed */
}

int usleep(useconds_t __useconds)
{
	return 0;
}
