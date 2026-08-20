	.section .segment_08_sdk_swi_wrappers, "a"
	.global segment_08_sdk_swi_wrappers
segment_08_sdk_swi_wrappers:
	.incbin "baserom.gba", 0xCFA40, 0x3F
