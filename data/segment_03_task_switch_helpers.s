	.section .segment_03_task_switch_helpers, "a"
	.global segment_03_task_switch_helpers
segment_03_task_switch_helpers:
	.incbin "baserom.gba", 0x234, 0xB1
