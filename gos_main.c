extern void gos_printf(const char* format, ...);

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static unsigned int gos_cpu_info = 0x0;

void print_cpuinfo(void)
{
#define L1_DCache	(1 << 2)
#define L1_ICache	(1 << 12)

	asm (
		"MRC p15, 0, r0, c1, c0, 0\n\t"
		"mov %[result], r0"
			: [result] "=r" (gos_cpu_info)
	);

	printf("\nCurrent CPU Cache Setting : 0x%08x\n", gos_cpu_info);
	printf("------------------------------------------\n");
	printf("L1 Instruction Cache      : %s\n", (gos_cpu_info & L1_ICache) ? "On":"Off");
	printf("L1 Data        Cache      : %s\n", (gos_cpu_info & L1_DCache) ? "On":"Off");

	printf("\n");
}

int gos_enable_idcache(void)
{
	gos_cpu_info |= L1_DCache|L1_ICache;

	printf("Enable ICache and DCache\n");
	asm (
		"MCR p15, 0, %[value], c1, c0, 0\n\t" : : [value] "r" (gos_cpu_info) :
	);


	return 0;
}

void os_main(void)
{
	int i;
	char *ptr;
	int alloc_size;
	char *dst, *src;

	/* Print CPU Info */
	print_cpuinfo();
	gos_enable_idcache();
	print_cpuinfo();

	/* Enable i&d cache. */

	/* C library Tests. */
	printf("Test Some C Library APIs\n");
	printf("------------------------------------------\n");
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
