	.section .sdk_libc, "a"
	.global sdk_libc
sdk_libc:
	.incbin "baserom.gba", 0xCFC30, 0x1AC

@ The _call_via_rN helpers (bx rN) live inside this verbatim blob at +0x00
@ through +0x1C (ROM VMA 0x080CFC30-0x080CFC4F). old_agbcc-compiled code
@ calls through them for indirect (interworking) calls.
	.global _call_via_r0
	.global _call_via_r1
	.global _call_via_r2
	.global _call_via_r3
	.global _call_via_r4
	.global _call_via_r5
	.global _call_via_r6
	.global _call_via_r7
_call_via_r0 = sdk_libc + 0x00
_call_via_r1 = sdk_libc + 0x04
_call_via_r2 = sdk_libc + 0x08
_call_via_r3 = sdk_libc + 0x0C
_call_via_r4 = sdk_libc + 0x10
_call_via_r5 = sdk_libc + 0x14
_call_via_r6 = sdk_libc + 0x18
_call_via_r7 = sdk_libc + 0x1C
