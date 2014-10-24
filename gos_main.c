extern void gos_printf(const char* format, ...);

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void os_main(void)
{
	int i;
	char *ptr;
	int alloc_size;
	char *dst, *src;

	alloc_size = 1;
	ptr = realloc(ptr, alloc_size);
	dst = malloc(100);
	src = "test";

	i = 4096;

	strcpy(dst, src);
	gos_printf("Starting %d.\n", i);
	gos_printf("Starting %s.\n", "GOS");
	gos_printf("dst=%s.\n", dst);
	printf("ddd\n");
}
