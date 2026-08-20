	.section .segment_06_game_code_early, "a"
	.global segment_06_game_code_early
segment_06_game_code_early:
	.incbin "baserom.gba", 0x6FF, 0x6C01
