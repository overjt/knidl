	.section .sample_set_index, "a"
	.global sample_set_index
sample_set_index:
	.incbin "baserom.gba", 0x7E1D58, 0x1330
