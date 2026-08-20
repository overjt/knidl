	.section .segment_30_zero_padding, "a"
	.global segment_30_zero_padding
segment_30_zero_padding:
	.incbin "baserom.gba", 0x7E3088, 0x1CF78
