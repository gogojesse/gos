.global __os_vectors
__os_vectors:
	b	_os_reset_handler	/* Reset */
	b	_os_undfd_handler	/* Undefined */
	b	.			/* SWI */
	b	.			/* Prefetch Abort */
	b	.			/* Data Abort */
	b	.			/* reserved */
	ldr	pc, =_os_irq_handler	/* IRQ */
	b	.			/* FIQ */

