
#include <stdio.h>
#include <stdarg.h>
extern void printch(int);

void gos_printf(const char* format, ...)
{
	int i, n;
	char buf[128];
	va_list argv;

	va_start(argv, format);
	n = vscnprintf(buf, sizeof(buf), format, argv);
	va_end(argv);

	for (i = 0; i < n; i++) {
		if (buf[i] == '\0')
			break;
		printch(buf[i]);
	}
}

void os_main(void)
{
	int i;

	i = 4096;

	gos_printf("Starting %s.\n", "GOS");
	gos_printf("Starting %d.\n", i);
}
