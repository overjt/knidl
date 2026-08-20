	.section .compressed_graphics, "a"
	.global compressed_graphics
compressed_graphics:
	.incbin "baserom.gba", 0x3d0000, 0x1f0000
