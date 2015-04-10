#include <stdio.h>

#include <inc/timer.h>
#include <inc/io.h>

#define TIMER_ENABLE	(1 << 7)
#define TIMER_MODE_MSK	(1 << 6)
#define TIMER_MODE_FR	(0 << 6)
#define TIMER_MODE_PD	(1 << 6)

#define TIMER_INT_EN	(1 << 5)
#define TIMER_PRS_MSK	(3 << 2)
#define TIMER_PRS_8S	(1 << 3)
#define TIMER_PRS_4S	(1 << 2)
#define TIMER_SIZE_MSK	(1 << 2)
#define TIMER_ONE_SHT	(1 << 0)

#define CONFIG_SYS_TIMERBASE 0x101e2000
#define CONFIG_SYS_TIMER1BASE 0x101e3000
int timer_init (void)
{
	unsigned long	tmr_ctrl_val;

	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	/* 10ms timer load. */
	tmr_ctrl_val = 10000;
	//tmr_ctrl_val = 1000000;	

	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0) = tmr_ctrl_val;

	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8);
	tmr_ctrl_val &= ~(TIMER_MODE_MSK | TIMER_INT_EN | TIMER_PRS_MSK | TIMER_SIZE_MSK | TIMER_MODE_PD);
	//tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_PRS_8S | TIMER_ONE_SHT);
	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_ONE_SHT);
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	return 0;
}

int timer1_init (void)
{
	unsigned long	tmr_ctrl_val;

	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8) = tmr_ctrl_val;

	/* 1 sec timer load. */
	tmr_ctrl_val = 1000000;

	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 0) = tmr_ctrl_val;

	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8);
	tmr_ctrl_val &= ~(TIMER_MODE_MSK | TIMER_INT_EN | TIMER_PRS_MSK | TIMER_SIZE_MSK | TIMER_MODE_PD);
	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_ONE_SHT);
	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8) = tmr_ctrl_val;

	return 0;
}
int us_timer_init (void)
{
	unsigned long	tmr_ctrl_val;

	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0x28);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0x28) = tmr_ctrl_val;

	/* set timer period timer */
	tmr_ctrl_val = TIMER1_LOAD_VAL;

	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0x20) = tmr_ctrl_val;

	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0x28);
	tmr_ctrl_val &= ~(TIMER_INT_EN);
	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_MODE_PD);
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0x28) = tmr_ctrl_val;

	return 0;
}

void us_timer_curval(unsigned long * val)
{
        void *reg;

        reg = (void *) (CONFIG_SYS_TIMERBASE + 0x24);
        *val = readl(reg);
}

void timer0_enable(void)
{
	unsigned long	tmr_ctrl_val;
	
	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8);

	/* Re-enable INT */
	tmr_ctrl_val |= (TIMER_ENABLE);
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;
}

void timer0_clear_int(void)
{
	unsigned long	tmr_ctrl_val;
	
	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	/* Clear INT */	
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 12) = 1;
}

void timer1_enable(void)
{
	unsigned long	tmr_ctrl_val;
	
	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8);

	/* Re-enable INT */
	tmr_ctrl_val |= (TIMER_ENABLE);
	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8) = tmr_ctrl_val;
}

void timer1_clear_int(void)
{
	unsigned long	tmr_ctrl_val;
	
	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8) = tmr_ctrl_val;

	/* Clear INT */	
	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 12) = 1;
}

