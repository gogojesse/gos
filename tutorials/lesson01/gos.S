main:
	mov	r0, #0x0	/* set register r0 = 0x0 */
	add	r0, r0, #1	/* r0 = r0 + 1 */
	b	.		/* goto this line forever (endless loop). */
