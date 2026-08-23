	.section .m4a_songs, "a"
	.global m4a_songs
m4a_songs:
	.incbin "baserom.gba", 0x5C0000, 0x4A140
