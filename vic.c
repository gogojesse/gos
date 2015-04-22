#include "inc/vic.h"

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

/* handlers in startup.S */
extern void _os_irq_handler(void);
extern void _sys_timer(void);
extern void _os_isr(void);

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
        writel(_os_irq_handler, reg);

	/*
	 * HW Timer 0 is for system timer(scheduler).
	 * HW Timer 2 is for normal OS timer 0. (use this
	 * to test normal IRQ handling)
	 */
	/* Set up irq handler for system timer. */
	reg = base + VIC_VECT_ADDR0 + (4 * 4);
        writel(_sys_timer, reg);

	reg = base + VIC_VECT_ADDR0 + (5 * 4);
        writel(_os_isr, reg);

	/* Enable irq 4 for Timer 0 and 1 */
	/* Enable irq 5 for Timer 2 and 3 */
	reg = base + VIC_VECT_CNTL_ENABLE;
	writel(0x00000030, reg);
}
