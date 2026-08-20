	.section .segment_07_game_code_and_rodata, "a"
	.global segment_07_game_code_and_rodata
segment_07_game_code_and_rodata:
	.incbin "baserom.gba", 0x7300, 0xC8740
