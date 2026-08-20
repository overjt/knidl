	.section .segment_01_crt0_master_isr, "a"
	.global segment_01_crt0_master_isr
segment_01_crt0_master_isr:
	.incbin "baserom.gba", 0xC0, 0x150
