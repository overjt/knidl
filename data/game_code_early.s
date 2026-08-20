	.section .game_code_early, "a"
	.global game_code_early
game_code_early:
	.incbin "baserom.gba", 0x6FF, 0x6C01
