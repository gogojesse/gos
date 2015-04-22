#include <sys/unistd.h>
#include <sys/time.h>
#include "inc/task.h"

static int os_sleep(unsigned int __seconds, useconds_t __useconds)
{
	struct timeval tv1, tv2;
	struct timezone tz1, tz2;

	if (__useconds < 0)
		return 0;

	/* Get current time. */	
	gettimeofday (&tv1, &tz1);

	do {
		yield_cpu();

		/* check time expired. */
		gettimeofday (&tv2, &tz2);
	} while (((tv2.tv_sec - tv1.tv_sec) < __seconds) || ((tv2.tv_usec - tv1.tv_usec) < __useconds));

	return 0; /* requested time has elapsed */
}

unsigned sleep(unsigned int __seconds)
{
	struct timeval tv1, tv2;
	struct timezone tz1, tz2;

	if (__seconds == 0)
		return 0;

	return (unsigned)os_sleep(__seconds, 0);
}

int usleep(useconds_t __useconds)
{
	unsigned int __seconds;

	if (0 >= __useconds)
		return 0;

	__seconds = __useconds / 1000000;
	__useconds = __useconds % 1000000;

	return (int)os_sleep(__seconds, __useconds);
}
