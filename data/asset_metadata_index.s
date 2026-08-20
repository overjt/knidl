	.section .asset_metadata_index, "a"
	.global asset_metadata_index
asset_metadata_index:
	.incbin "baserom.gba", 0x730000, 0x30000
