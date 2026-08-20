	.section .lib_rodata_fir_tables, "a"
	.global lib_rodata_fir_tables
lib_rodata_fir_tables:
	.incbin "baserom.gba", 0xCFF00, 0x100
