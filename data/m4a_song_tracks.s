	.section .m4a_song_tracks, "a"
	.global m4a_song_tracks
m4a_song_tracks:
	.incbin "baserom.gba", 0x60A418, 0x1018
