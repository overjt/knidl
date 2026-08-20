	.section .m4a_songs, "a"
	.global m4a_songs
m4a_songs:
	.incbin "baserom.gba", 0x5c0000, 0x16e9f8
