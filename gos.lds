SECTIONS {
	. = 0x0;
	.text : {
		gos.o
		/* gos_main.o */
		*(.text);
	}
	.data : { * (.data); }
	. = ALIGN(8);
	 heap_low = .; /* for _sbrk */
	. = . + 0x10000;     /* 64kB of heap memory */
	heap_top = .; /* for _sbrk */
	. = . + 0x10000;     /* 64kB of svc stack memory */
	_gos_stack_end = .;
	. = . + 0x1000;      /* 4kB of irq stack memory */
	_gos_irq_stack_end = .;
}
