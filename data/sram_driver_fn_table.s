	.section .sram_driver_fn_table, "a"
	.global sram_driver_fn_table
sram_driver_fn_table:
	.incbin "baserom.gba", 0x72EA04, 0x10
