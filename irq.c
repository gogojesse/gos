#include <stdio.h>
#include "inc/irq.h"
#include "inc/platform-defs.h"

IRQ_HANDLER gisr_hdl[MAX_IRQ][MAX_SHARED_ISR];
IRQ_RESOURCE gisr_res[MAX_IRQ][MAX_SHARED_ISR];

extern void timer1_clear_int(void);

#if 0
void os_timer_isr(void)	/* called in IRQ mode. */
{
	unsigned int val;
	void *reg;

	printf("os_timer_isr\n");

	reg = (void*)VIC_REGBASE;
	val = *(volatile unsigned long *)(VIC_REGBASE);
	timer1_clear_int();
	printf("VIC Status=<0x%08x>\n", val);
}
#endif

void irq_init(void)
{
	int i, j;

	for (i = 0; i < MAX_IRQ; i++) {
		for (j = 0; j < MAX_SHARED_ISR; j++) {
			gisr_hdl[i][j] = 0;
			gisr_res[i][j] = 0;
		}
	}
}

void shared_isr(void) /* In IRQ mode, called if IRQ is shared. */
{
	int irq, i;
	ISR_CHECK rt;
	unsigned int val;
	void *reg;
	
	//printf("Enter shared_isr()\n");

	/* 1. check IRQ. */
	reg = (void*)VIC_REGBASE;
	val = *(volatile unsigned long *)(VIC_REGBASE);

	for (i = 0, irq=0; i < MAX_IRQ; i++) {
		if ((val >> i) & 0x1) 
			irq = i;
	}

//	timer1_clear_int();
//	printf("VIC Status=<0x%08x>\n", val);

	/* 2. Sequentially enter ISRs. */
	for (i = 0; i < MAX_SHARED_ISR; i++) {
		if (gisr_hdl[irq][i]) {
			rt = gisr_hdl[irq][i](gisr_res[irq][i]);
			if (IRQ_HANDLED == rt)
				return;
		}
	}
}

int irq_reg(const int irq, IRQ_HANDLER handler, IRQ_RESOURCE res, IRQ_MODE mode)
{
	void *reg;
	int i, limit;

	if (handler != 0 && irq >=0 && irq < MAX_IRQ) {
		if (mode & SHARED_IRQ)
			limit = MAX_SHARED_ISR;
		else
			limit = 1;

		for (i = 0; i < limit; i++) {
			if (gisr_hdl[irq][i] == 0) {
				gisr_hdl[irq][i] = handler;
				gisr_res[irq][i] = res;
					return 0;
			}
		}

		/* IRQ is occupied. */
		return -2;
	}

	/* Parameter Error. */
	return -1;
}

int irq_unreg(const int irq)
{
	int i;

	if (irq >= 0 && irq < MAX_IRQ) {

		/* Reset ISR. */
		for (i = 0; i < MAX_SHARED_ISR; i++) {
			gisr_hdl[irq][i] = 0;
			gisr_res[irq][i] = 0;
		}

		/* Successful. */
		return 0;
	}

	/* Parameter Error. */
	return -1;
}

