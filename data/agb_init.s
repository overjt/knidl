	.section .agb_init, "a"
	.global agb_init
agb_init:
	.incbin "baserom.gba", 0x310, 0x3EF
