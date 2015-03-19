#include <stdio.h>
#include "inc/rtc.h"
#include "inc/io.h"
#include "inc/platform-defs.h"

/* RTC registers offset */
#define RTC_RTCDR	0x000
#define RTC_RTCMR	0x004
#define RTC_RTCLR	0x008
#define RTC_RTCCR	0x00C
#define RTC_RTCDR	0x000
/* RTC bits offset */
#define RTC_CONTROL    (1 << 0)

void rtc_init(void)
{
	void *reg;

        reg = (void *) (RTC_REGBASE + RTC_RTCCR);
	writel((0x01 << RTC_CONTROL), reg);
        reg = (void *) (RTC_REGBASE + RTC_RTCLR);
	writel(0x00, reg);

}

void rtc_read(unsigned long * val)
{
	void *reg;

	reg = (void *) (RTC_REGBASE + RTC_RTCDR);
	*val = readl(reg);
}
