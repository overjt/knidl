	.section .lib_misc, "a"
	.global lib_misc
lib_misc:
	.incbin "baserom.gba", 0xcfe20, 0xe0
