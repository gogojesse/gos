#include <sys/time.h>
#include <inc/timer.h>
#include <inc/rtc.h>

int _gettimeofday( struct timeval *tv, void *tzvp )
{
	unsigned long sec = 0;
	unsigned long us = 0;
	unsigned long t = 0;

	rtc_read(&sec);
	us_timer_curval(&us);
	t = (sec * 1000000) + (TIMER1_LOAD_VAL - us);  // timer1 decrease to 0, get uptime in us
	tv->tv_sec = t / 1000000;  // convert to seconds
	tv->tv_usec = ( t % 1000000 ) ;  // get remaining microseconds

	return 0;  // return non-zero for error
} // end _gettimeofday()
