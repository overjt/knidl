	.section .sdk_lib_sram_driver, "a"
	.global sdk_lib_sram_driver
sdk_lib_sram_driver:
	.incbin "baserom.gba", 0xcfa7f, 0x35d
