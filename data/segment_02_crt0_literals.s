	.section .segment_02_crt0_literals, "a"
	.global segment_02_crt0_literals
segment_02_crt0_literals:
	.incbin "baserom.gba", 0x210, 0x24
