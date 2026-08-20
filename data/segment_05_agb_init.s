	.section .segment_05_agb_init, "a"
	.global segment_05_agb_init
segment_05_agb_init:
	.incbin "baserom.gba", 0x310, 0x3EF
