	.section .sound_samples_2, "a"
	.global sound_samples_2
sound_samples_2:
	.incbin "baserom.gba", 0x350AF8, 0x57ADC
