	.section .crt0_master_isr, "a"
	.global crt0_master_isr
crt0_master_isr:
	.incbin "baserom.gba", 0xC0, 0x150
