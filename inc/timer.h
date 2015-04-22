#ifndef __timer_h__
#define __timer_h__

#define TIMER1_LOAD_VAL 1000000

int timer_init (void);
int timer1_init (void);
int us_timer_init (void);
void us_timer_curval(unsigned long * val);
inline void udelay(unsigned long count);

#endif /* __timer_h__ */
