	.section .zero_padding, "a"
	.global zero_padding
zero_padding:
	.incbin "baserom.gba", 0x7e3088, 0x1cf78
