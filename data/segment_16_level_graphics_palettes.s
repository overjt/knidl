	.section .segment_16_level_graphics_palettes, "a"
	.global segment_16_level_graphics_palettes
segment_16_level_graphics_palettes:
	.incbin "baserom.gba", 0x120000, 0x2156E0
