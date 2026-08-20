	.section .game_code_early, "a"
	.global game_code_early
game_code_early:
	.incbin "baserom.gba", 0x6ff, 0x6c01
