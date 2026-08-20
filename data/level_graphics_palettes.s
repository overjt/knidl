	.section .level_graphics_palettes, "a"
	.global level_graphics_palettes
level_graphics_palettes:
	.incbin "baserom.gba", 0x120000, 0x2156E0
