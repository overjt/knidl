	.section .rom_header, "a"
	.global rom_header
rom_header:
	/* Entry point: ARM branch to 0x080000C0 */
	.word 0xEA00002E

	/* Nintendo logo (0x04-0x9F), copyrighted: extracted from baserom */
	.incbin "baserom.gba", 0x04, 0x9C

	/* Game title (12 bytes) */
	.ascii "AGB KIRBY DX"

	/* Game code, maker code */
	.ascii "A7KE"
	.ascii "01"

	/* Fixed value, main unit code, device type */
	.byte 0x96
	.byte 0x00
	.byte 0x00

	/* Reserved (0xB5-0xBB) */
	.space 7, 0

	/* Software version */
	.byte 0x00

	/* Complement check, computed by tools/gbafix.py */
	.byte 0x00

	/* Reserved (0xBE-0xBF) */
	.space 2, 0
