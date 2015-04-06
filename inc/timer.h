#ifndef __timer_h__
#define __timer_h__

int timer_init (void);
int timer1_init (void);
void vic_init2(void *base);
void timer1_curval(unsigned long * val);
inline void udelay(unsigned long count);

#endif /* __timer_h__ */
