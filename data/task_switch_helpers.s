	.section .task_switch_helpers, "a"
	.global task_switch_helpers
task_switch_helpers:
	.incbin "baserom.gba", 0x234, 0xB1
