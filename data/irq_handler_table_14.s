	.section .irq_handler_table_14, "a"
	.global irq_handler_table_14
irq_handler_table_14:
	.incbin "baserom.gba", 0xcfde8, 0x38
