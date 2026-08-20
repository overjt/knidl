	.section .segment_09_sdk_lib_sram_driver, "a"
	.global segment_09_sdk_lib_sram_driver
segment_09_sdk_lib_sram_driver:
	.incbin "baserom.gba", 0xCFA7F, 0x35D
