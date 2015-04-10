#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <unistd.h>
#include <sys/time.h>

//#include "inc/task.h"
//#include "inc/spinlock.h"
//#include "inc/platform-defs.h"
#include "inc/timer.h"
#include "inc/irq.h"
//#include "inc/rtc.h"
//#include "inc/io.h"


static int ddd = 0;
int os_timer_isr(IRQ_RESOURCE res) /* called in SVC mode. */
{
	//printf("os_timer_isr() %d\n", ddd++);
	timer1_clear_int();
	timer1_enable();

	return 0;
}

