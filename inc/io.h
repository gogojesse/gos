#ifndef __io_h__
#define __io_h__

#define writel(v,reg)   *(volatile unsigned long *)(reg) = (unsigned long)v

static inline unsigned long readl(void * addr)
{
	return *(volatile unsigned long *) addr;
}

#endif /* __io_h__ */
