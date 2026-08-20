	.section .segment_25_sram_driver_fn_table, "a"
	.global segment_25_sram_driver_fn_table
segment_25_sram_driver_fn_table:
	.incbin "baserom.gba", 0x72EA04, 0x10
