	.section .sound_samples_1, "a"
	.global sound_samples_1
sound_samples_1:
	.incbin "baserom.gba", 0x3356E0, 0x19808
