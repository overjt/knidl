	.section .main, "a"
	.global main_blob
main_blob:
	.incbin "baserom.gba", 0xC0
