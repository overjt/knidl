	.section .sdk_reset_helper, "a"
	.global sdk_reset_helper
sdk_reset_helper:
	.incbin "baserom.gba", 0xCFA7F, 0x1D
