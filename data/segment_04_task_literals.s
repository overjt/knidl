	.section .segment_04_task_literals, "a"
	.global segment_04_task_literals
segment_04_task_literals:
	.incbin "baserom.gba", 0x2E5, 0x2B
