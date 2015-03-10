#ifndef __irq_h__
#define __irq_h__

#define MAX_IRQ 16
#define MAX_SHARED_ISR 2

typedef void * IRQ_RESOURCE;
typedef int (*IRQ_HANDLER)(IRQ_RESOURCE);
typedef int IRQ_MODE;
typedef enum {
	IRQ_HANDLED,
	IRQ_PASSED
} ISR_CHECK;

#define NORMAL_IRQ	(0x1 << 0)
#define FAST_IRQ	(0x1 << 1)
#define SHARED_IRQ	(0x1 << 2)

void irq_init(void);
int irq_reg(const int irq, IRQ_HANDLER handler, IRQ_RESOURCE res, IRQ_MODE mode);
int irq_unreg(const int irq);

#endif /* __irq_h__ */
