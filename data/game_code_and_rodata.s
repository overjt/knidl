	.section .game_code_and_rodata, "a"
	.global game_code_and_rodata
game_code_and_rodata:
	.incbin "baserom.gba", 0x7300, 0xC8740
