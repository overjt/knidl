	.section .lib_misc, "a"
	.global lib_misc
lib_misc:
	.incbin "baserom.gba", 0xCFE20, 0xE0
