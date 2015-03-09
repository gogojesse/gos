#include <stdio.h>

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

	/* 10ms timer load. */
	tmr_ctrl_val = 1000000;	

	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 0) = tmr_ctrl_val;

	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8);
	tmr_ctrl_val &= ~(TIMER_MODE_MSK | TIMER_INT_EN | TIMER_PRS_MSK | TIMER_SIZE_MSK | TIMER_MODE_PD);
	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_ONE_SHT);
	*(volatile unsigned long *)(CONFIG_SYS_TIMER1BASE + 8) = tmr_ctrl_val;

	return 0;
}


#define writel(v,reg)	*(volatile unsigned long *)(reg) = (unsigned long)v

#define VIC_VECT_ADDR0			0x100
#define VIC_VECT_ADDR1			(0x100+4)
#define VIC_VECT_ADDR2			(0x100+8)
#define VIC_VECT_ADDR3			(0x100+12)
#define VIC_VECT_ADDR4			(0x100+16)
#define VIC_VECT_ADDR5			(0x100+20)

#define VIC_VECT_CNTL_BASE		0x200
#define VIC_VECT_CNTL_ENABLE		0x010
#define VIC_INT_CLEAR			0x014
#define VIC_SW_INT_CLEAR		0x01C
#define VIC_VECTADDR			0x030
#define VIC_PL190_DEF_VECT_ADDR		0x034
extern void _gos_irq_handler(void);
extern void _gos_irq02_handler(void);
extern void _sys_timer(void);
extern void _os_timer(void);

void os_timer_isr(void)
{
	printf("os_timer_isr\n");
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


void vic_clearint(void *base, unsigned long bitmask)
{
	void *reg;

	reg = base + VIC_INT_CLEAR;
	writel(bitmask, reg);
} 

void vic_init2(void *base)
{
        int i;
	void *reg;

	/* clear SW int */
	reg = base + VIC_SW_INT_CLEAR;
	writel(0xffffffff, reg);

	reg = base + VIC_INT_CLEAR;
	writel(0xffffffff, reg);

        for (i = 0; i < 16; i++) {
                reg = base + VIC_VECT_CNTL_BASE + (i * 4); 
                writel((0x1 << 5) | i, reg);
        }   

	/* set up default vector address. */
	reg = base + VIC_PL190_DEF_VECT_ADDR;
        writel(_gos_irq_handler, reg);

	/*
	 * HW Timer 0 is for system timer(scheduler).
	 * HW Timer 2 is for normal OS timer 0. (use this
	 * to test normal IRQ handling)
	 */
	/* Set up irq handler for system timer. */
	reg = base + VIC_VECT_ADDR0 + (4 * 4);
        writel(_sys_timer, reg);

	reg = base + VIC_VECT_ADDR0 + (5 * 4);
        writel(_os_timer, reg);

	/* Enable irq 4 for Timer 0 and 1 */
	/* Enable irq 5 for Timer 2 and 3 */
	reg = base + VIC_VECT_CNTL_ENABLE;
	writel(0x00000030, reg);
}
