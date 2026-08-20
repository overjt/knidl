@ Kirby: Nightmare in Dream Land (GBA, 2002) — crt0 / entry stub + master ISR
@
@ ARM-mode startup code beginning at ROM VMA 0x080000C0 (entry from header branch).
@ Covers sections .crt0_master_isr (0x080000C0-0x0800020C) and
@ .crt0_literals (0x08000210-0x08000233).
@
@ Assembled with: arm-none-eabi-as -mcpu=arm7tdmi

	.syntax unified
	.cpu	arm7tdmi

@ ─────────────────────────────────────────────────────────────────────────────
@ Section: .crt0_master_isr  (ROM VMA 0x080000C0, size 0x14D bytes)
@ Contains two ARM functions:
@   AgbMain    - entry point, sets up CPU modes/stacks, installs ISR, calls
@                AgbInit (Thumb) then main (Thumb); loops if main returns.
@   MasterIsr  - master IRQ dispatcher; dispatches via 14-entry table at IWRAM.
@
@ NOTE: Several ldr instructions in this section use PC-relative offsets that
@ reach into the separate .crt0_literals section (VMA 0x08000210).  Those
@ loads cannot be expressed as assembler pseudo-ops across section boundaries,
@ so they are encoded as raw .word values.  The comment beside each one shows
@ the effective target address.
@ ─────────────────────────────────────────────────────────────────────────────

	.section .crt0_master_isr, "ax"
	.arm

@ ────────────────────────────────────
@ AgbMain — ROM entry point (0x080000C0)
@ Called by the GBA BIOS after verifying the header; CPU is in ARM state.
@ ────────────────────────────────────
	.global AgbMain
AgbMain:
	@ 0x080000C0: mov r0, #0x12 — switch to IRQ mode
	mov	r0, #0x12
	@ 0x080000C4: msr CPSR_fc, r0
	msr	CPSR_fc, r0
	@ 0x080000C8: ldr sp, [pc, #52]  ->  0x8000104 (_irq_stack_top = 0x03007F60)
	ldr	sp, _irq_stack_top

	@ 0x080000CC: mov r0, #0x1F — switch to System mode
	mov	r0, #0x1F
	@ 0x080000D0: msr CPSR_fc, r0
	msr	CPSR_fc, r0
	@ 0x080000D4: ldr sp, [pc, #36]  ->  0x8000100 (_sys_stack_top = 0x03007EC0)
	ldr	sp, _sys_stack_top

	@ 0x080000D8: ldr r1, [pc, #308]  ->  0x8000214 (BIOS IRQ vector = 0x03007FFC)
	@ Cross-section PC-relative load; encoded as raw word.
	@ pc = 0x080000E0, offset = 308 = 0x134, target = 0x08000214
	.word	0xE59F1134		@ ldr r1, [pc, #308]  @ 0x8000214

	@ 0x080000DC: add r0, pc, #36  ->  r0 = 0x08000108 (address of MasterIsr)
	@ pc = 0x080000E4, + 0x24 = 0x08000108
	add	r0, pc, #0x24

	@ 0x080000E0: str r0, [r1]  ->  install ISR address at 0x03007FFC
	str	r0, [r1]

	@ 0x080000E4: ldr r1, [pc, #300]  ->  0x8000218 (AgbInit Thumb ptr = 0x08000311)
	.word	0xE59F112C		@ ldr r1, [pc, #300]  @ 0x8000218

	@ 0x080000E8: mov lr, pc
	mov	lr, pc
	@ 0x080000EC: bx r1  ->  call AgbInit (Thumb)
	bx	r1

	@ 0x080000F0: ldr r1, [pc, #292]  ->  0x800021C (main Thumb ptr = 0x08007301)
	.word	0xE59F1124		@ ldr r1, [pc, #292]  @ 0x800021C

	@ 0x080000F4: mov lr, pc
	mov	lr, pc
	@ 0x080000F8: bx r1  ->  call main (Thumb)
	bx	r1

	@ 0x080000FC: b AgbMain  ->  loop forever if main returns
	b	AgbMain

@ Inline literal pool (0x08000100-0x08000107): stack pointers.
@ These are referenced by the ldr instructions above via PC-relative offsets
@ within the same section.
_sys_stack_top:
	.word	0x03007EC0	@ 0x08000100 — System-mode stack top

_irq_stack_top:
	.word	0x03007F60	@ 0x08000104 — IRQ-mode stack top

@ ────────────────────────────────────
@ MasterIsr — master IRQ handler (VMA 0x08000108)
@ Installed in IWRAM at 0x03007FFC (initially) then copied to 0x03001030 by
@ AgbInit which re-points 0x03007FFC at the IWRAM copy.
@
@ Priority order (first matching bit wins):
@   bit 7  (VBLANK)    -> ip = 0x00
@   bit 6  (HBLANK)    -> ip = 0x04
@   bit 0  (LCD VCOUNT)-> ip = 0x08
@   bit 1  (TIMER0)    -> ip = 0x0C
@   bit 4  (SERIAL)    -> ip = 0x18
@   bit 13 (GAMEPAK)   -> ip = 0x34
@ ────────────────────────────────────
	.global MasterIsr
MasterIsr:
	@ 0x08000108: mov r3, #0x04000000
	mov	r3, #0x04000000
	@ 0x0800010C: add r3, r3, #0x200  ->  r3 = 0x04000200 (REG_IE block)
	add	r3, r3, #0x200

	@ 0x08000110: ldr r2, [r3]    ->  r2 = REG_IE | (REG_IF << 16)
	ldr	r2, [r3]
	@ 0x08000114: ldrh r1, [r3, #8]  ->  r1 = REG_IME
	ldrh	r1, [r3, #8]
	@ 0x08000118: mrs r0, SPSR
	mrs	r0, SPSR

	@ 0x0800011C: push {r0, r1, r2, r3, lr}  ->  save context
	push	{r0, r1, r2, r3, lr}

	@ 0x08000120: mov r0, #1
	mov	r0, #1
	@ 0x08000124: strh r0, [r3, #8]  ->  REG_IME = 1 (keep interrupts disabled)
	strh	r0, [r3, #8]

	@ 0x08000128: and r1, r2, r2, lsr #16  ->  r1 = active & enabled IRQs
	and	r1, r2, r2, lsr #16

	@ Priority dispatch: find highest-priority IRQ bit.
	@ 0x0800012C: mov ip, #0
	mov	ip, #0
	@ 0x08000130: ands r0, r1, #0x80  ->  VBLANK?
	ands	r0, r1, #0x80
	bne	_isr_dispatch
	@ 0x08000138: mov ip, #4
	mov	ip, #4
	@ 0x0800013C: ands r0, r1, #0x40  ->  HBLANK?
	ands	r0, r1, #0x40
	bne	_isr_dispatch
	@ 0x08000144: mov ip, #8
	mov	ip, #8
	@ 0x08000148: ands r0, r1, #0x01  ->  LCD VCOUNT?
	ands	r0, r1, #0x01
	bne	_isr_dispatch
	@ 0x08000150: mov ip, #12
	mov	ip, #12
	@ 0x08000154: ands r0, r1, #0x02  ->  TIMER0?
	ands	r0, r1, #0x02
	bne	_isr_dispatch
	@ 0x0800015C: mov ip, #24
	mov	ip, #24
	@ 0x08000160: ands r0, r1, #0x10  ->  SERIAL?
	ands	r0, r1, #0x10
	bne	_isr_dispatch
	@ 0x08000168: mov ip, #0x34
	mov	ip, #0x34
	@ 0x0800016C: ands r0, r1, #0x2000  ->  GAMEPAK?
	ands	r0, r1, #0x2000

	@ 0x08000170: strbne r0, [r3, #-0x17C]  (requires .syntax unified)
	@ r3 = 0x04000200, -0x17C = -380 -> 0x04000084 (guard write)
	strbne	r0, [r3, #-0x17C]
	@ 0x08000174: bne _isr_hang  ->  loop on unrecognised IRQ
_isr_hang:
	bne	_isr_hang

_isr_dispatch:
	@ 0x08000178: strh r0, [r3, #2]  ->  acknowledge in REG_IF
	strh	r0, [r3, #2]

	@ 0x0800017C: mov r1, #0x20C0
	mov	r1, #0x20C0
	@ 0x08000180: bic r2, r2, r0
	bic	r2, r2, r0
	@ 0x08000184: and r1, r1, r2
	and	r1, r1, r2
	@ 0x08000188: strh r1, [r3]  ->  REG_IE = masked value
	strh	r1, [r3]

	@ Switch to System mode so we can be re-interrupted.
	@ 0x0800018C: mrs r3, CPSR
	mrs	r3, CPSR
	@ 0x08000190: bic r3, r3, #0xDF
	bic	r3, r3, #0xDF
	@ 0x08000194: orr r3, r3, #0x1F
	orr	r3, r3, #0x1F
	@ 0x08000198: msr CPSR_fc, r3
	msr	CPSR_fc, r3

	@ IRQ stack overflow guard.
	@ 0x0800019C: mov r0, sp
	mov	r0, sp
	@ 0x080001A0: ldr r1, [pc, #120]  ->  0x8000220 (_isr_stack_guard = 0x03007B80)
	.word	0xE59F1078		@ ldr r1, [pc, #120]  @ 0x8000220
	@ 0x080001A4: cmp r0, r1
	cmp	r0, r1
	@ 0x080001A8: bcs _isr_no_overflow
	bcs	_isr_no_overflow
	@ 0x080001AC: ldr r1, [pc, #112]  ->  0x8000224 (_isr_sp_save = 0x03000FA0)
	.word	0xE59F1070		@ ldr r1, [pc, #112]  @ 0x8000224
	@ 0x080001B0: str r0, [r1]
	str	r0, [r1]
	@ 0x080001B4: ldr r1, [pc, #108]  ->  0x8000228 (_isr_stack_top = 0x03007C80)
	.word	0xE59F106C		@ ldr r1, [pc, #108]  @ 0x8000228
	@ 0x080001B8: mov sp, r1
	mov	sp, r1
_isr_no_overflow:

	@ Tail-call the per-IRQ handler from the 14-entry table.
	@ 0x080001BC: ldr r1, [pc, #104]  ->  0x800022C (_irq_handler_table = 0x030004B0)
	.word	0xE59F1068		@ ldr r1, [pc, #104]  @ 0x800022C
	@ 0x080001C0: add r1, r1, ip
	add	r1, r1, ip
	@ 0x080001C4: ldr r0, [r1]
	ldr	r0, [r1]
	@ 0x080001C8: stmfd sp!, {lr}
	stmfd	sp!, {lr}
	@ 0x080001CC: add lr, pc, #0
	add	lr, pc, #0
	@ 0x080001D0: bx r0
	bx	r0
	@ 0x080001D4: ldmfd sp!, {lr}
	ldmfd	sp!, {lr}

	@ Restore SYS sp if we used the overflow stack.
	@ 0x080001D8: ldr r0, [pc, #72]  ->  0x8000228 (_isr_stack_top = 0x03007C80)
	.word	0xE59F0048		@ ldr r0, [pc, #72]   @ 0x8000228
	@ 0x080001DC: cmp r0, sp
	cmp	r0, sp
	@ 0x080001E0: bne _isr_restore_done
	bne	_isr_restore_done
	@ 0x080001E4: ldr r1, [pc, #68]  ->  0x8000230 (_isr_sp_save_2 = 0x03000FA0)
	.word	0xE59F1044		@ ldr r1, [pc, #68]   @ 0x8000230
	@ 0x080001E8: ldr sp, [r1]
	ldr	sp, [r1]
_isr_restore_done:

	@ Switch back to IRQ mode.
	@ 0x080001EC: mrs r3, CPSR
	mrs	r3, CPSR
	@ 0x080001F0: bic r3, r3, #0xDF
	bic	r3, r3, #0xDF
	@ 0x080001F4: orr r3, r3, #0x92
	orr	r3, r3, #0x92
	@ 0x080001F8: msr CPSR_fc, r3
	msr	CPSR_fc, r3

	@ Restore saved registers and return.
	@ 0x080001FC: pop {r0, r1, r2, r3, lr}
	pop	{r0, r1, r2, r3, lr}
	@ 0x08000200: strh r2, [r3]   ->  REG_IE restored
	strh	r2, [r3]
	@ 0x08000204: strh r1, [r3, #8]  ->  REG_IME restored
	strh	r1, [r3, #8]
	@ 0x08000208: msr SPSR_fc, r0
	msr	SPSR_fc, r0
	@ 0x0800020C: bx lr
	bx	lr


@ ─────────────────────────────────────────────────────────────────────────────
@ Section: .crt0_literals  (ROM VMA 0x08000210, size 0x24 bytes)
@ Literal pool referenced by PC-relative LDR instructions in AgbMain and
@ MasterIsr above.  Words must appear at exactly the addresses the linker
@ script pins (0x08000210-0x08000233).
@ ─────────────────────────────────────────────────────────────────────────────

	.section .crt0_literals, "a"

@ 0x08000210 — magic / sentinel word
	.word	0x89ABCDEF

@ 0x08000214 — BIOS IRQ vector address (target of ldr r1 at 0x080000D8)
	.word	0x03007FFC

@ 0x08000218 — AgbInit Thumb interwork ptr (target of ldr r1 at 0x080000E4)
	.word	0x08000311

@ 0x0800021C — main Thumb interwork ptr (target of ldr r1 at 0x080000F0)
	.word	0x08007301

@ 0x08000220 — ISR stack guard threshold (target of ldr r1 at 0x080001A0)
	.word	0x03007B80

@ 0x08000224 — ISR sp spill cell address (target of ldr r1 at 0x080001AC)
	.word	0x03000FA0

@ 0x08000228 — ISR overflow stack top (target of ldr r1/r0 at 0x080001B4 / 0x080001D8)
	.word	0x03007C80

@ 0x0800022C — 14-entry IRQ handler table base in IWRAM (target of ldr r1 at 0x080001BC)
	.word	0x030004B0

@ 0x08000230 — ISR sp spill cell address copy (target of ldr r1 at 0x080001E4)
	.word	0x03000FA0
