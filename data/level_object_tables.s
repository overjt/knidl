	.section .level_object_tables, "a"
	.global level_object_tables
level_object_tables:
	.incbin "baserom.gba", 0xd0000, 0x50000
