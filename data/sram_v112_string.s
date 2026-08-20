	.section .sram_v112_string, "a"
	.global sram_v112_string
sram_v112_string:
	.incbin "baserom.gba", 0x72E9F8, 0xA
