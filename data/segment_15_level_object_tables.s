	.section .segment_15_level_object_tables, "a"
	.global segment_15_level_object_tables
segment_15_level_object_tables:
	.incbin "baserom.gba", 0xD0000, 0x50000
