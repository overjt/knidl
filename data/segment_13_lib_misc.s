	.section .segment_13_lib_misc, "a"
	.global segment_13_lib_misc
segment_13_lib_misc:
	.incbin "baserom.gba", 0xCFE20, 0xE0
