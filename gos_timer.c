
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
int timer_init (void)
{
	unsigned long	tmr_ctrl_val;

	/* 1st disable the Timer */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8);
	tmr_ctrl_val &= ~TIMER_ENABLE;
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	tmr_ctrl_val = 0x200;	
	printf("reload timer to <0x%08x>\n", tmr_ctrl_val);
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 0) = tmr_ctrl_val;

	/*
	 * Timer Mode : Periodic
	 * Interrupt : Enabled
	 * Prescale : 0 Stage, Clk/1
	 * Tmr Siz : 16 Bit Counter
	 * Tmr in Wrapping Mode
	 */
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8);
	//tmr_ctrl_val &= ~(TIMER_MODE_MSK | TIMER_INT_EN | TIMER_PRS_MSK | TIMER_SIZE_MSK | TIMER_ONE_SHT );
	tmr_ctrl_val &= ~(TIMER_MODE_MSK | TIMER_PRS_MSK | TIMER_SIZE_MSK | TIMER_ONE_SHT );
	//tmr_ctrl_val |= (TIMER_ENABLE);
//	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_MODE_PD | TIMER_ONE_SHT);
	//tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_MODE_PD);
	//tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN);
	tmr_ctrl_val |= (TIMER_ENABLE | TIMER_INT_EN | TIMER_ONE_SHT);

	printf("timer ctrl = <0x%08x>\n", tmr_ctrl_val);
	*(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 8) = tmr_ctrl_val;

	/*tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 10);
	printf("raw int status = <0x%08x>\n", tmr_ctrl_val);
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 14);
	printf("masked int status = <0x%08x>\n", tmr_ctrl_val);
	tmr_ctrl_val = *(volatile unsigned long *)(CONFIG_SYS_TIMERBASE + 4);
	printf("timer value = <0x%08x>\n", tmr_ctrl_val);
*/
	return 0;
}

#define writel(v,reg)	*(volatile unsigned long *)(reg) = v
#define VIC_VECT_CNTL0			0x200
#define VIC_VECT_CNTL_ENABLE		0x010
#define VIC_PL190_DEF_VECT_ADDR		0x034
extern void _gos_irq_handler(void);

void vic_init2(void *base)
{
        int i;
	void * reg;

	reg = base + 0x1c;
	writel(0xffffffff, reg);

        for (i = 0; i < 16; i++) {
                reg = base + VIC_VECT_CNTL0 + (i * 4); 
                writel(VIC_VECT_CNTL_ENABLE | i, reg);
        }   

	reg = base + VIC_PL190_DEF_VECT_ADDR;
        writel(_gos_irq_handler, reg);

	reg = base + 0x10;
	writel(0xffffffff, reg);

}
