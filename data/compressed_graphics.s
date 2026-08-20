	.section .compressed_graphics, "a"
	.global compressed_graphics
compressed_graphics:
	.incbin "baserom.gba", 0x3D0000, 0x1F0000
