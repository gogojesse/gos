SECTIONS {
	. = 0x0;
	._os_vector_start : {
		* (._os_vector_start);
	}
	. = 0x8000;
	.text : {
		startup.o
		vic.o
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
