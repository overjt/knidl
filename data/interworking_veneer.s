	.section .interworking_veneer, "a"
	.global interworking_veneer
interworking_veneer:
	.incbin "baserom.gba", 0xCFDDC, 0x8
