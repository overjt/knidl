	.section .song_tail_misc_audio, "a"
	.global song_tail_misc_audio
song_tail_misc_audio:
	.incbin "baserom.gba", 0x760000, 0x81D58
