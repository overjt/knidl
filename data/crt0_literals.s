	.section .crt0_literals, "a"
	.global crt0_literals
crt0_literals:
	.incbin "baserom.gba", 0x210, 0x24
