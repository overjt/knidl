	.section .segment_21_compressed_graphics, "a"
	.global segment_21_compressed_graphics
segment_21_compressed_graphics:
	.incbin "baserom.gba", 0x3D0000, 0x1F0000
