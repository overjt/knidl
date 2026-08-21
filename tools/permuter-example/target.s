    .syntax unified

    .section .text
    .align 2
    .thumb
    .thumb_func
ReadSram_Core:
	push	{r4, lr}
	adds	r4, r0, #0
	subs	r3, r2, #1
	cmp	r2, #0
	beq.n	.L_end
	movs	r2, #1
	negs	r2, r2
.L_loop:
	ldrb	r0, [r4, #0]
	strb	r0, [r1, #0]
	adds	r4, #1
	adds	r1, #1
	subs	r3, #1
	cmp	r3, r2
	bne.n	.L_loop
.L_end:
	pop	{r4}
	pop	{r0}
	bx	r0
	.short	0x0000
