	.section .sram_v112_string, "a"
	.global sram_v112_string
sram_v112_string:
	.incbin "baserom.gba", 0x72e9f8, 0xc
