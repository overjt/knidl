@ Kirby: Nightmare in Dream Land (GBA, 2002) - the cartridge header
@ (0x08000000-0x080000BF).
@
@ Stays asm: the header is the entry branch, the Nintendo logo (an .incbin
@   slice of the user's own baserom.gba, never committed), the title, the
@   codes and the checksum that tools/gbafix.py writes; its fields are data
@   by nature, written as asm as in pret's rom_header.s (docs/audit.md
@   section 2).

	.section .rom_header, "a"
	.global rom_header
rom_header:
	/* Entry point: ARM branch to Start (crt0, 0x080000C0) */
	.arm
	b	Start

	/* Nintendo logo (0x04-0x9F), copyrighted: extracted from baserom */
	.incbin "baserom.gba", 0x04, 0x9C

	/* Game title (12 bytes) */
	.ascii "AGB KIRBY DX"

	/* Game code, maker code */
	.ascii "A7KE"
	.ascii "01"

	/* Fixed value, main unit code, device type */
	.byte 0x96	@ raw: header field "fixed value" (must be 0x96)
	.byte 0x00	@ raw: header field "main unit code" (0 = GBA)
	.byte 0x00	@ raw: header field "device type"

	/* Reserved (0xB5-0xBB) */
	.space 7, 0

	/* Software version */
	.byte 0x00	@ raw: header field "software version" (0)

	/* Complement check, computed by tools/gbafix.py */
	.byte 0x00	@ raw: header field "complement check", a placeholder tools/gbafix.py fills in

	/* Reserved (0xBE-0xBF) */
	.space 2, 0
