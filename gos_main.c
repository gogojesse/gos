#include <stdio.h>
#include <stdarg.h>

extern void gos_printf(const char* format, ...);

void os_main(void)
{
	int i;

	i = 4096;

	gos_printf("Starting %s.\n", "GOS");
	gos_printf("Starting %d.\n", i);
}
