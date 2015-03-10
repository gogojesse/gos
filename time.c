#include <sys/time.h>

int _gettimeofday( struct timeval *tv, void *tzvp )
{
    //uint64_t t = __your_system_time_function_here__();  // get uptime in micro-seconds
    unsigned long t = 1000000;  // get uptime in micro-seconds
    tv->tv_sec = t / 1000000;  // convert to seconds
    tv->tv_usec = ( t % 1000000 ) ;  // get remaining microseconds
    return 0;  // return non-zero for error
} // end _gettimeofday()

