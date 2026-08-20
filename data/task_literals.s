	.section .task_literals, "a"
	.global task_literals
task_literals:
	.incbin "baserom.gba", 0x2E5, 0x2B
