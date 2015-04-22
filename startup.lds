SECTIONS {
	. = 0x0;
	.text : {
		startup.o
		/* main.o */
		*(.text);
	}
	.data : { * (.data); }
	. = ALIGN(8);
	 heap_low = .; /* for _sbrk */
	. = . + 0x10000;     /* 64kB of heap memory */
	heap_top = .; /* for _sbrk */
	. = . + 0x10000;     /* 64kB of svc stack memory */
	_os_stack_end = .;
	. = . + 0x1000;      /* 4kB of irq stack memory */
	_os_irq_stack_end = .;
}
