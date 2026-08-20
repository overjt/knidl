	.section .segment_10_interworking_veneer, "a"
	.global segment_10_interworking_veneer
segment_10_interworking_veneer:
	.incbin "baserom.gba", 0xCFDDC, 0x8
