	.section .sdk_swi_wrappers, "a"
	.global sdk_swi_wrappers
sdk_swi_wrappers:
	.incbin "baserom.gba", 0xcfa40, 0x3f
