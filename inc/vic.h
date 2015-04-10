#ifndef __VIC_H__
#define __VIC_H__

#include "inc/io.h"

void vic_clearint(void *base, unsigned long bitmask);
void vic_init2(void *base);

#endif /* __VIC_H__ */
