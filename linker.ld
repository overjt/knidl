/* Kirby: Nightmare in Dream Land (USA) - ROM layout.
 *
 * The sections follow each other without pinned addresses (issue #36,
 * docs/data.md section 8): ld places each one right after the previous
 * one, aligned to its own input sections, so the order and the sizes alone
 * give the layout.  Only the cartridge header keeps a fixed address (the
 * hardware reads it at 0x08000000, and the branch at its first word jumps
 * to the 0xC0 bytes after it).
 *
 * Every other block is followed by an ASSERT of the address the original
 * ROM has.  The Makefile links with --defsym MATCHING=1 by default (what
 * `make compare` and CI use), so a matching build still fails loudly at
 * the first section that moved; `make MATCHING=0` drops the assertions for
 * a modified ROM.  tools/shiftcheck.py (make shifttest) links with
 * MATCHING=0 and padding inserted to measure which pointers still do not
 * move with their targets.  New blocks are written by tools/ldblocks.py
 * (carve.py, carve_data.py, resegment.py).
 */

MEMORY
{
    /* 32 MiB, the GBA maximum; the matching image ends at 0x08800000
     * (the assertion at the end). */
    ROM : ORIGIN = 0x08000000, LENGTH = 32M
}

SECTIONS
{
    /* GBA cartridge header (0x08000000-0x080000BF), built from source in
     * asm/rom_header.s. The Nintendo logo is copyrighted and is extracted
     * from baserom.gba via .incbin instead of being committed. */
    .rom_header 0x08000000 : {
        KEEP(*(.rom_header))
    } > ROM

    /* ===== Segments split from the former main_blob =====
     * One section per docs/analysis/segments.txt row, in address order;
     * the assertion after each block is its segments.txt start.  A group
     * (tools/ldgroup.py) lists several rows, the data pieces of one
     * data/<zone>.s and the C runs between them, in one block. */

    /* crt0 + master ISR (ARM code) — assembled from asm/crt0.s */
    .crt0_master_isr : {
        KEEP(*(.crt0_master_isr))
    } > ROM
    ASSERT(!MATCHING || ADDR(.crt0_master_isr) == 0x080000C0, "MATCHING: .crt0_master_isr moved")

    /* crt0/ISR literal pool — assembled from asm/crt0.s */
    .crt0_literals : {
        KEEP(*(.crt0_literals))
    } > ROM
    ASSERT(!MATCHING || ADDR(.crt0_literals) == 0x08000210, "MATCHING: .crt0_literals moved")

    /* Task/context-switch helpers (ARM code) */
    .task_switch_helpers : {
        KEEP(*(.task_switch_helpers))
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_switch_helpers) == 0x08000234, "MATCHING: .task_switch_helpers moved")

    /* Task helper literal pool */
    .task_literals : {
        KEEP(*(.task_literals))
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_literals) == 0x080002E8, "MATCHING: .task_literals moved")

    /* AgbInit (Thumb) — decompiled to C (src/agb_init.c, issue #28).
     * The section spans 0x08000310-0x080008E7: the function body, the
     * compiler's pool-skip branch at 0x0700 and its literal pool. */
    .agb_init : {
        build/src/agb_init.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.agb_init) == 0x08000310, "MATCHING: .agb_init moved")

    /* Game code: early subsystems (Thumb) */
    /* early_08e8 — decompiled to C (src/early_08e8.c, carved by tools/carve.py) */
    .early_08e8 : {
        build/src/early_08e8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_08e8) == 0x080008E8, "MATCHING: .early_08e8 moved")

    /* early_0de4 — decompiled to C (src/early_0de4.c, carved by tools/carve.py) */
    .early_0de4 : {
        build/src/early_0de4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_0de4) == 0x08000DE4, "MATCHING: .early_0de4 moved")

    /* early_10cc — decompiled to C (src/early_10cc.c, carved by tools/carve.py) */
    .early_10cc : {
        build/src/early_10cc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_10cc) == 0x080010CC, "MATCHING: .early_10cc moved")

    /* early_11ac — decompiled to C (src/early_11ac.c, carved by tools/carve.py) */
    .early_11ac : {
        build/src/early_11ac.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_11ac) == 0x080011AC, "MATCHING: .early_11ac moved")

    /* early_1518 — decompiled to C (src/early_1518.c, carved by tools/carve.py) */
    .early_1518 : {
        build/src/early_1518.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_1518) == 0x08001518, "MATCHING: .early_1518 moved")

    /* early_1b08 — decompiled to C (src/early_1b08.c, carved by tools/carve.py) */
    .early_1b08 : {
        build/src/early_1b08.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_1b08) == 0x08001B08, "MATCHING: .early_1b08 moved")

    /* early_1cc8 — decompiled to C (src/early_1cc8.c, carved by tools/carve.py) */
    .early_1cc8 : {
        build/src/early_1cc8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_1cc8) == 0x08001CC8, "MATCHING: .early_1cc8 moved")

    /* early_1fd0 — decompiled to C (src/early_1fd0.c, carved by tools/carve.py) */
    .early_1fd0 : {
        build/src/early_1fd0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_1fd0) == 0x08001FD0, "MATCHING: .early_1fd0 moved")

    /* early_2378 — decompiled to C (src/early_2378.c, carved by tools/carve.py) */
    .early_2378 : {
        build/src/early_2378.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_2378) == 0x08002378, "MATCHING: .early_2378 moved")

    /* early_2668 — decompiled to C (src/early_2668.c, carved by tools/carve.py) */
    .early_2668 : {
        build/src/early_2668.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_2668) == 0x08002668, "MATCHING: .early_2668 moved")

    /* early_293c — decompiled to C (src/early_293c.c, carved by tools/carve.py) */
    .early_293c : {
        build/src/early_293c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_293c) == 0x0800293C, "MATCHING: .early_293c moved")

    /* early_2b04 — decompiled to C (src/early_2b04.c, carved by tools/carve.py) */
    .early_2b04 : {
        build/src/early_2b04.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_2b04) == 0x08002B04, "MATCHING: .early_2b04 moved")

    /* early_3110 — decompiled to C (src/early_3110.c, carved by tools/carve.py) */
    .early_3110 : {
        build/src/early_3110.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_3110) == 0x08003110, "MATCHING: .early_3110 moved")

    /* early_31b8 — decompiled to C (src/early_31b8.c, carved by tools/carve.py) */
    .early_31b8 : {
        build/src/early_31b8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_31b8) == 0x080031B8, "MATCHING: .early_31b8 moved")

    /* early_3484 — decompiled to C (src/early_3484.c, carved by tools/carve.py) */
    .early_3484 : {
        build/src/early_3484.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_3484) == 0x08003484, "MATCHING: .early_3484 moved")

    /* early_3888 — decompiled to C (src/early_3888.c, carved by tools/carve.py) */
    .early_3888 : {
        build/src/early_3888.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_3888) == 0x08003888, "MATCHING: .early_3888 moved")

    /* early_3964 — decompiled to C (src/early_3964.c, carved by tools/carve.py) */
    .early_3964 : {
        build/src/early_3964.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_3964) == 0x08003964, "MATCHING: .early_3964 moved")

    /* early_4000 — decompiled to C (src/early_4000.c, carved by tools/carve.py) */
    .early_4000 : {
        build/src/early_4000.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_4000) == 0x08004000, "MATCHING: .early_4000 moved")

    /* early_4734 — decompiled to C (src/early_4734.c, carved by tools/carve.py) */
    .early_4734 : {
        build/src/early_4734.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_4734) == 0x08004734, "MATCHING: .early_4734 moved")

    /* early_4984 — decompiled to C (src/early_4984.c, carved by tools/carve.py) */
    .early_4984 : {
        build/src/early_4984.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_4984) == 0x08004984, "MATCHING: .early_4984 moved")

    /* early_4d6c — decompiled to C (src/early_4d6c.c, carved by tools/carve.py) */
    .early_4d6c : {
        build/src/early_4d6c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_4d6c) == 0x08004D6C, "MATCHING: .early_4d6c moved")

    /* early_4fec — decompiled to C (src/early_4fec.c, carved by tools/carve.py) */
    .early_4fec : {
        build/src/early_4fec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_4fec) == 0x08004FEC, "MATCHING: .early_4fec moved")

    /* early_5228 — decompiled to C (src/early_5228.c, carved by tools/carve.py) */
    .early_5228 : {
        build/src/early_5228.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_5228) == 0x08005228, "MATCHING: .early_5228 moved")

    /* early_55b0 — decompiled to C (src/early_55b0.c, carved by tools/carve.py) */
    .early_55b0 : {
        build/src/early_55b0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_55b0) == 0x080055B0, "MATCHING: .early_55b0 moved")

    /* early_5654 — decompiled to C (src/early_5654.c, carved by tools/carve.py) */
    .early_5654 : {
        build/src/early_5654.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_5654) == 0x08005654, "MATCHING: .early_5654 moved")

    /* early_58e4 — decompiled to C (src/early_58e4.c, carved by tools/carve.py) */
    .early_58e4 : {
        build/src/early_58e4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_58e4) == 0x080058E4, "MATCHING: .early_58e4 moved")

    /* early_5acc — decompiled to C (src/early_5acc.c, carved by tools/carve.py) */
    .early_5acc : {
        build/src/early_5acc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_5acc) == 0x08005ACC, "MATCHING: .early_5acc moved")

    /* early_5c4c — decompiled to C (src/early_5c4c.c, carved by tools/carve.py) */
    .early_5c4c : {
        build/src/early_5c4c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_5c4c) == 0x08005C4C, "MATCHING: .early_5c4c moved")

    /* early_5d9c — decompiled to C (src/early_5d9c.c, carved by tools/carve.py) */
    .early_5d9c : {
        build/src/early_5d9c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_5d9c) == 0x08005D9C, "MATCHING: .early_5d9c moved")

    /* early_6464 — decompiled to C (src/early_6464.c, carved by tools/carve.py) */
    .early_6464 : {
        build/src/early_6464.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6464) == 0x08006464, "MATCHING: .early_6464 moved")

    /* early_6ac8 — decompiled to C (src/early_6ac8.c, carved by tools/carve.py) */
    .early_6ac8 : {
        build/src/early_6ac8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6ac8) == 0x08006AC8, "MATCHING: .early_6ac8 moved")

    /* early_6cd4 — decompiled to C (src/early_6cd4.c, carved by tools/carve.py) */
    .early_6cd4 : {
        build/src/early_6cd4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6cd4) == 0x08006CD4, "MATCHING: .early_6cd4 moved")

    /* early_6d18 — decompiled to C (src/early_6d18.c, carved by tools/carve.py) */
    .early_6d18 : {
        build/src/early_6d18.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6d18) == 0x08006D18, "MATCHING: .early_6d18 moved")

    /* early_6d28 — decompiled to C (src/early_6d28.c, carved by tools/carve.py) */
    .early_6d28 : {
        build/src/early_6d28.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6d28) == 0x08006D28, "MATCHING: .early_6d28 moved")

    /* early_6e8c — decompiled to C (src/early_6e8c.c, carved by tools/carve.py) */
    .early_6e8c : {
        build/src/early_6e8c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6e8c) == 0x08006E8C, "MATCHING: .early_6e8c moved")

    /* early_6e9c — decompiled to C (src/early_6e9c.c, carved by tools/carve.py) */
    .early_6e9c : {
        build/src/early_6e9c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_6e9c) == 0x08006E9C, "MATCHING: .early_6e9c moved")

    /* early_7004 — decompiled to C (src/early_7004.c, carved by tools/carve.py) */
    .early_7004 : {
        build/src/early_7004.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.early_7004) == 0x08007004, "MATCHING: .early_7004 moved")

    /* Game code + rodata (Thumb) — bulk of the game logic */
    /* main — decompiled to C (src/main.c, carved by tools/carve.py) */
    .main : {
        build/src/main.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.main) == 0x08007300, "MATCHING: .main moved")

    /* mode_075b8 — decompiled to C (src/mode_075b8.c, carved by tools/carve.py) */
    .mode_075b8 : {
        build/src/mode_075b8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_075b8) == 0x080075B8, "MATCHING: .mode_075b8 moved")

    /* mode_07b68 — decompiled to C (src/mode_07b68.c, carved by tools/carve.py) */
    .mode_07b68 : {
        build/src/mode_07b68.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_07b68) == 0x08007B68, "MATCHING: .mode_07b68 moved")

    /* mode_082d0 — decompiled to C (src/mode_082d0.c, carved by tools/carve.py) */
    .mode_082d0 : {
        build/src/mode_082d0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_082d0) == 0x080082D0, "MATCHING: .mode_082d0 moved")

    /* mode_08664 — decompiled to C (src/mode_08664.c, carved by tools/carve.py) */
    .mode_08664 : {
        build/src/mode_08664.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_08664) == 0x08008664, "MATCHING: .mode_08664 moved")

    /* gfx_08b8c — decompiled to C (src/gfx_08b8c.c, carved by tools/carve.py) */
    .gfx_08b8c : {
        build/src/gfx_08b8c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.gfx_08b8c) == 0x08008B8C, "MATCHING: .gfx_08b8c moved")

    /* boot_091ac — decompiled to C (src/boot_091ac.c, carved by tools/carve.py) */
    .boot_091ac : {
        build/src/boot_091ac.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.boot_091ac) == 0x080091AC, "MATCHING: .boot_091ac moved")

    /* hud_099fc — decompiled to C (src/hud_099fc.c, carved by tools/carve.py) */
    .hud_099fc : {
        build/src/hud_099fc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_099fc) == 0x080099FC, "MATCHING: .hud_099fc moved")

    /* hud_0a130 — decompiled to C (src/hud_0a130.c, carved by tools/carve.py) */
    .hud_0a130 : {
        build/src/hud_0a130.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_0a130) == 0x0800A130, "MATCHING: .hud_0a130 moved")

    /* hud_0aad0 — decompiled to C (src/hud_0aad0.c, carved by tools/carve.py) */
    .hud_0aad0 : {
        build/src/hud_0aad0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_0aad0) == 0x0800AAD0, "MATCHING: .hud_0aad0 moved")

    /* hud_0b318 — decompiled to C (src/hud_0b318.c, carved by tools/carve.py) */
    .hud_0b318 : {
        build/src/hud_0b318.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_0b318) == 0x0800B318, "MATCHING: .hud_0b318 moved")

    /* mode_0b44c — decompiled to C (src/mode_0b44c.c, carved by tools/carve.py) */
    .mode_0b44c : {
        build/src/mode_0b44c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_0b44c) == 0x0800B44C, "MATCHING: .mode_0b44c moved")

    /* menu_0b920 — decompiled to C (src/menu_0b920.c, carved by tools/carve.py) */
    .menu_0b920 : {
        build/src/menu_0b920.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_0b920) == 0x0800B920, "MATCHING: .menu_0b920 moved")

    /* menu_0c09c — decompiled to C (src/menu_0c09c.c, carved by tools/carve.py) */
    .menu_0c09c : {
        build/src/menu_0c09c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_0c09c) == 0x0800C09C, "MATCHING: .menu_0c09c moved")

    /* menu_0ca10 — decompiled to C (src/menu_0ca10.c, carved by tools/carve.py) */
    .menu_0ca10 : {
        build/src/menu_0ca10.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_0ca10) == 0x0800CA10, "MATCHING: .menu_0ca10 moved")

    /* menu_0d450 — decompiled to C (src/menu_0d450.c, carved by tools/carve.py) */
    .menu_0d450 : {
        build/src/menu_0d450.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menu_0d450) == 0x0800D450, "MATCHING: .menu_0d450 moved")

    /* menutask_0daf8 — decompiled to C (src/menutask_0daf8.c, carved by tools/carve.py) */
    .menutask_0daf8 : {
        build/src/menutask_0daf8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menutask_0daf8) == 0x0800DAF8, "MATCHING: .menutask_0daf8 moved")

    /* menutask_0e314 — decompiled to C (src/menutask_0e314.c, carved by tools/carve.py) */
    .menutask_0e314 : {
        build/src/menutask_0e314.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menutask_0e314) == 0x0800E314, "MATCHING: .menutask_0e314 moved")

    /* menutask_0ea0c — decompiled to C (src/menutask_0ea0c.c, carved by tools/carve.py) */
    .menutask_0ea0c : {
        build/src/menutask_0ea0c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menutask_0ea0c) == 0x0800EA0C, "MATCHING: .menutask_0ea0c moved")

    /* menutask_0f180 — decompiled to C (src/menutask_0f180.c, carved by tools/carve.py) */
    .menutask_0f180 : {
        build/src/menutask_0f180.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.menutask_0f180) == 0x0800F180, "MATCHING: .menutask_0f180 moved")

    /* bgscroll_0fcbc — decompiled to C (src/bgscroll_0fcbc.c, carved by tools/carve.py) */
    .bgscroll_0fcbc : {
        build/src/bgscroll_0fcbc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.bgscroll_0fcbc) == 0x0800FCBC, "MATCHING: .bgscroll_0fcbc moved")

    /* mode_100ac — decompiled to C (src/mode_100ac.c, carved by tools/carve.py) */
    .mode_100ac : {
        build/src/mode_100ac.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_100ac) == 0x080100AC, "MATCHING: .mode_100ac moved")

    /* player_10358 — decompiled to C (src/player_10358.c, carved by tools/carve.py) */
    .player_10358 : {
        build/src/player_10358.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_10358) == 0x08010358, "MATCHING: .player_10358 moved")

    /* player_109c8 — decompiled to C (src/player_109c8.c, carved by tools/carve.py) */
    .player_109c8 : {
        build/src/player_109c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_109c8) == 0x080109C8, "MATCHING: .player_109c8 moved")

    /* player_10b38 — decompiled to C (src/player_10b38.c, carved by tools/carve.py) */
    .player_10b38 : {
        build/src/player_10b38.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_10b38) == 0x08010B38, "MATCHING: .player_10b38 moved")

    /* player_17668 — decompiled to C (src/player_17668.c, carved by tools/carve.py) */
    .player_17668 : {
        build/src/player_17668.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_17668) == 0x08017668, "MATCHING: .player_17668 moved")

    /* player_18b84 — decompiled to C (src/player_18b84.c, carved by tools/carve.py) */
    .player_18b84 : {
        build/src/player_18b84.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_18b84) == 0x08018B84, "MATCHING: .player_18b84 moved")

    /* player_18e14 — decompiled to C (src/player_18e14.c, carved by tools/carve.py) */
    .player_18e14 : {
        build/src/player_18e14.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_18e14) == 0x08018E14, "MATCHING: .player_18e14 moved")

    /* player_19000 — decompiled to C (src/player_19000.c, carved by tools/carve.py) */
    .player_19000 : {
        build/src/player_19000.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_19000) == 0x08019000, "MATCHING: .player_19000 moved")

    /* player_19eec — decompiled to C (src/player_19eec.c, carved by tools/carve.py) */
    .player_19eec : {
        build/src/player_19eec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_19eec) == 0x08019EEC, "MATCHING: .player_19eec moved")

    /* player_1a07c — decompiled to C (src/player_1a07c.c, carved by tools/carve.py) */
    .player_1a07c : {
        build/src/player_1a07c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_1a07c) == 0x0801A07C, "MATCHING: .player_1a07c moved")

    /* player_1a3e4 — decompiled to C (src/player_1a3e4.c, carved by tools/carve.py) */
    .player_1a3e4 : {
        build/src/player_1a3e4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_1a3e4) == 0x0801A3E4, "MATCHING: .player_1a3e4 moved")

    /* player_1a76c — decompiled to C (src/player_1a76c.c, carved by tools/carve.py) */
    .player_1a76c : {
        build/src/player_1a76c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_1a76c) == 0x0801A76C, "MATCHING: .player_1a76c moved")

    /* hitbox_1a8c8 — decompiled to C (src/hitbox_1a8c8.c, carved by tools/carve.py) */
    .hitbox_1a8c8 : {
        build/src/hitbox_1a8c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hitbox_1a8c8) == 0x0801A8C8, "MATCHING: .hitbox_1a8c8 moved")

    /* hitbox_1b24c — decompiled to C (src/hitbox_1b24c.c, carved by tools/carve.py) */
    .hitbox_1b24c : {
        build/src/hitbox_1b24c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hitbox_1b24c) == 0x0801B24C, "MATCHING: .hitbox_1b24c moved")

    /* hitbox_1b7dc — decompiled to C (src/hitbox_1b7dc.c, carved by tools/carve.py) */
    .hitbox_1b7dc : {
        build/src/hitbox_1b7dc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hitbox_1b7dc) == 0x0801B7DC, "MATCHING: .hitbox_1b7dc moved")

    /* terrain_1baa4 — decompiled to C (src/terrain_1baa4.c, carved by tools/carve.py) */
    .terrain_1baa4 : {
        build/src/terrain_1baa4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1baa4) == 0x0801BAA4, "MATCHING: .terrain_1baa4 moved")

    /* terrain_1bcac — decompiled to C (src/terrain_1bcac.c, carved by tools/carve.py) */
    .terrain_1bcac : {
        build/src/terrain_1bcac.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1bcac) == 0x0801BCAC, "MATCHING: .terrain_1bcac moved")

    /* terrain_1c30c — decompiled to C (src/terrain_1c30c.c, carved by tools/carve.py) */
    .terrain_1c30c : {
        build/src/terrain_1c30c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1c30c) == 0x0801C30C, "MATCHING: .terrain_1c30c moved")

    /* terrain_1c444 — decompiled to C (src/terrain_1c444.c, carved by tools/carve.py) */
    .terrain_1c444 : {
        build/src/terrain_1c444.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1c444) == 0x0801C444, "MATCHING: .terrain_1c444 moved")

    /* terrain_1c51c — decompiled to C (src/terrain_1c51c.c, carved by tools/carve.py) */
    .terrain_1c51c : {
        build/src/terrain_1c51c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1c51c) == 0x0801C51C, "MATCHING: .terrain_1c51c moved")

    /* terrain_1c690 — decompiled to C (src/terrain_1c690.c, carved by tools/carve.py) */
    .terrain_1c690 : {
        build/src/terrain_1c690.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1c690) == 0x0801C690, "MATCHING: .terrain_1c690 moved")

    /* terrain_1c8dc — decompiled to C (src/terrain_1c8dc.c, carved by tools/carve.py) */
    .terrain_1c8dc : {
        build/src/terrain_1c8dc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1c8dc) == 0x0801C8DC, "MATCHING: .terrain_1c8dc moved")

    /* terrain_1c930 — decompiled to C (src/terrain_1c930.c, carved by tools/carve.py) */
    .terrain_1c930 : {
        build/src/terrain_1c930.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1c930) == 0x0801C930, "MATCHING: .terrain_1c930 moved")

    /* terrain_1d394 — decompiled to C (src/terrain_1d394.c, carved by tools/carve.py) */
    .terrain_1d394 : {
        build/src/terrain_1d394.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1d394) == 0x0801D394, "MATCHING: .terrain_1d394 moved")

    /* terrain_1d9c8 — decompiled to C (src/terrain_1d9c8.c, carved by tools/carve.py) */
    .terrain_1d9c8 : {
        build/src/terrain_1d9c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1d9c8) == 0x0801D9C8, "MATCHING: .terrain_1d9c8 moved")

    /* terrain_1e178 — decompiled to C (src/terrain_1e178.c, carved by tools/carve.py) */
    .terrain_1e178 : {
        build/src/terrain_1e178.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1e178) == 0x0801E178, "MATCHING: .terrain_1e178 moved")

    /* terrain_1ecd0 — decompiled to C (src/terrain_1ecd0.c, carved by tools/carve.py) */
    .terrain_1ecd0 : {
        build/src/terrain_1ecd0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1ecd0) == 0x0801ECD0, "MATCHING: .terrain_1ecd0 moved")

    /* terrain_1f540 — decompiled to C (src/terrain_1f540.c, carved by tools/carve.py) */
    .terrain_1f540 : {
        build/src/terrain_1f540.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1f540) == 0x0801F540, "MATCHING: .terrain_1f540 moved")

    /* terrain_1ff84 — decompiled to C (src/terrain_1ff84.c, carved by tools/carve.py) */
    .terrain_1ff84 : {
        build/src/terrain_1ff84.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_1ff84) == 0x0801FF84, "MATCHING: .terrain_1ff84 moved")

    /* terrain_2069c — decompiled to C (src/terrain_2069c.c, carved by tools/carve.py) */
    .terrain_2069c : {
        build/src/terrain_2069c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_2069c) == 0x0802069C, "MATCHING: .terrain_2069c moved")

    /* terrain_207a0 — decompiled to C (src/terrain_207a0.c, carved by tools/carve.py) */
    .terrain_207a0 : {
        build/src/terrain_207a0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_207a0) == 0x080207A0, "MATCHING: .terrain_207a0 moved")

    /* terrain_21130 — decompiled to C (src/terrain_21130.c, carved by tools/carve.py) */
    .terrain_21130 : {
        build/src/terrain_21130.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_21130) == 0x08021130, "MATCHING: .terrain_21130 moved")

    /* terrain_2136c — decompiled to C (src/terrain_2136c.c, carved by tools/carve.py) */
    .terrain_2136c : {
        build/src/terrain_2136c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_2136c) == 0x0802136C, "MATCHING: .terrain_2136c moved")

    /* terrain_214e0 — decompiled to C (src/terrain_214e0.c, carved by tools/carve.py) */
    .terrain_214e0 : {
        build/src/terrain_214e0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_214e0) == 0x080214E0, "MATCHING: .terrain_214e0 moved")

    /* terrain_21b18 — decompiled to C (src/terrain_21b18.c, carved by tools/carve.py) */
    .terrain_21b18 : {
        build/src/terrain_21b18.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.terrain_21b18) == 0x08021B18, "MATCHING: .terrain_21b18 moved")

    /* level_2296c — decompiled to C (src/level_2296c.c, carved by tools/carve.py) */
    .level_2296c : {
        build/src/level_2296c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.level_2296c) == 0x0802296C, "MATCHING: .level_2296c moved")

    /* roomtask_23618 — decompiled to C (src/roomtask_23618.c, carved by tools/carve.py) */
    .roomtask_23618 : {
        build/src/roomtask_23618.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.roomtask_23618) == 0x08023618, "MATCHING: .roomtask_23618 moved")

    /* level_23948 — decompiled to C (src/level_23948.c, carved by tools/carve.py) */
    .level_23948 : {
        build/src/level_23948.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.level_23948) == 0x08023948, "MATCHING: .level_23948 moved")

    /* level_242d0 — decompiled to C (src/level_242d0.c, carved by tools/carve.py) */
    .level_242d0 : {
        build/src/level_242d0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.level_242d0) == 0x080242D0, "MATCHING: .level_242d0 moved")

    /* stage_261c0 — decompiled to C (src/stage_261c0.c, carved by tools/carve.py) */
    .stage_261c0 : {
        build/src/stage_261c0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_261c0) == 0x080261C0, "MATCHING: .stage_261c0 moved")

    /* door_26b60 — decompiled to C (src/door_26b60.c, carved by tools/carve.py) */
    .door_26b60 : {
        build/src/door_26b60.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.door_26b60) == 0x08026B60, "MATCHING: .door_26b60 moved")

    /* stage_270d0 — decompiled to C (src/stage_270d0.c, carved by tools/carve.py) */
    .stage_270d0 : {
        build/src/stage_270d0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_270d0) == 0x080270D0, "MATCHING: .stage_270d0 moved")

    /* stage_273a0 — decompiled to C (src/stage_273a0.c, carved by tools/carve.py) */
    .stage_273a0 : {
        build/src/stage_273a0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_273a0) == 0x080273A0, "MATCHING: .stage_273a0 moved")

    /* level_27a6c — decompiled to C (src/level_27a6c.c, carved by tools/carve.py) */
    .level_27a6c : {
        build/src/level_27a6c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.level_27a6c) == 0x08027A6C, "MATCHING: .level_27a6c moved")

    /* room_27e28 — decompiled to C (src/room_27e28.c, carved by tools/carve.py) */
    .room_27e28 : {
        build/src/room_27e28.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_27e28) == 0x08027E28, "MATCHING: .room_27e28 moved")

    /* room_28320 — decompiled to C (src/room_28320.c, carved by tools/carve.py) */
    .room_28320 : {
        build/src/room_28320.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_28320) == 0x08028320, "MATCHING: .room_28320 moved")

    /* camera_28b8c — decompiled to C (src/camera_28b8c.c, carved by tools/carve.py) */
    .camera_28b8c : {
        build/src/camera_28b8c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_28b8c) == 0x08028B8C, "MATCHING: .camera_28b8c moved")

    /* camera_296a0 — decompiled to C (src/camera_296a0.c, carved by tools/carve.py) */
    .camera_296a0 : {
        build/src/camera_296a0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_296a0) == 0x080296A0, "MATCHING: .camera_296a0 moved")

    /* camera_29c74 — decompiled to C (src/camera_29c74.c, carved by tools/carve.py) */
    .camera_29c74 : {
        build/src/camera_29c74.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_29c74) == 0x08029C74, "MATCHING: .camera_29c74 moved")

    /* bgmap_2a9cc — decompiled to C (src/bgmap_2a9cc.c, carved by tools/carve.py) */
    .bgmap_2a9cc : {
        build/src/bgmap_2a9cc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.bgmap_2a9cc) == 0x0802A9CC, "MATCHING: .bgmap_2a9cc moved")

    /* bgmap_2b2f0 — decompiled to C (src/bgmap_2b2f0.c, carved by tools/carve.py) */
    .bgmap_2b2f0 : {
        build/src/bgmap_2b2f0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.bgmap_2b2f0) == 0x0802B2F0, "MATCHING: .bgmap_2b2f0 moved")

    /* camera_2b4bc — decompiled to C (src/camera_2b4bc.c, carved by tools/carve.py) */
    .camera_2b4bc : {
        build/src/camera_2b4bc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_2b4bc) == 0x0802B4BC, "MATCHING: .camera_2b4bc moved")

    /* camera_2c42c — decompiled to C (src/camera_2c42c.c, carved by tools/carve.py) */
    .camera_2c42c : {
        build/src/camera_2c42c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_2c42c) == 0x0802C42C, "MATCHING: .camera_2c42c moved")

    /* camera_2d01c — decompiled to C (src/camera_2d01c.c, carved by tools/carve.py) */
    .camera_2d01c : {
        build/src/camera_2d01c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camera_2d01c) == 0x0802D01C, "MATCHING: .camera_2d01c moved")

    /* camtask_2d38c — decompiled to C (src/camtask_2d38c.c, carved by tools/carve.py) */
    .camtask_2d38c : {
        build/src/camtask_2d38c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.camtask_2d38c) == 0x0802D38C, "MATCHING: .camtask_2d38c moved")

    /* obj_2eac8 — decompiled to C (src/obj_2eac8.c, carved by tools/carve.py) */
    .obj_2eac8 : {
        build/src/obj_2eac8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.obj_2eac8) == 0x0802EAC8, "MATCHING: .obj_2eac8 moved")

    /* obj_2f62c — decompiled to C (src/obj_2f62c.c, carved by tools/carve.py) */
    .obj_2f62c : {
        build/src/obj_2f62c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.obj_2f62c) == 0x0802F62C, "MATCHING: .obj_2f62c moved")

    /* obj_30238 — decompiled to C (src/obj_30238.c, carved by tools/carve.py) */
    .obj_30238 : {
        build/src/obj_30238.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.obj_30238) == 0x08030238, "MATCHING: .obj_30238 moved")

    /* obj_306b4 — decompiled to C (src/obj_306b4.c, carved by tools/carve.py) */
    .obj_306b4 : {
        build/src/obj_306b4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.obj_306b4) == 0x080306B4, "MATCHING: .obj_306b4 moved")

    /* block_30804 — decompiled to C (src/block_30804.c, carved by tools/carve.py) */
    .block_30804 : {
        build/src/block_30804.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.block_30804) == 0x08030804, "MATCHING: .block_30804 moved")

    /* block_318b4 — decompiled to C (src/block_318b4.c, carved by tools/carve.py) */
    .block_318b4 : {
        build/src/block_318b4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.block_318b4) == 0x080318B4, "MATCHING: .block_318b4 moved")

    /* player_32688 — decompiled to C (src/player_32688.c, carved by tools/carve.py) */
    .player_32688 : {
        build/src/player_32688.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_32688) == 0x08032688, "MATCHING: .player_32688 moved")

    /* player_337f4 — decompiled to C (src/player_337f4.c, carved by tools/carve.py) */
    .player_337f4 : {
        build/src/player_337f4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_337f4) == 0x080337F4, "MATCHING: .player_337f4 moved")

    /* player_343c0 — decompiled to C (src/player_343c0.c, carved by tools/carve.py) */
    .player_343c0 : {
        build/src/player_343c0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_343c0) == 0x080343C0, "MATCHING: .player_343c0 moved")

    /* player_34f8c — decompiled to C (src/player_34f8c.c, carved by tools/carve.py) */
    .player_34f8c : {
        build/src/player_34f8c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_34f8c) == 0x08034F8C, "MATCHING: .player_34f8c moved")

    /* player_36280 — decompiled to C (src/player_36280.c, carved by tools/carve.py) */
    .player_36280 : {
        build/src/player_36280.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_36280) == 0x08036280, "MATCHING: .player_36280 moved")

    /* player_36c94 — decompiled to C (src/player_36c94.c, carved by tools/carve.py) */
    .player_36c94 : {
        build/src/player_36c94.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_36c94) == 0x08036C94, "MATCHING: .player_36c94 moved")

    /* player_37ed8 — decompiled to C (src/player_37ed8.c, carved by tools/carve.py) */
    .player_37ed8 : {
        build/src/player_37ed8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_37ed8) == 0x08037ED8, "MATCHING: .player_37ed8 moved")

    /* player_3919c — decompiled to C (src/player_3919c.c, carved by tools/carve.py) */
    .player_3919c : {
        build/src/player_3919c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_3919c) == 0x0803919C, "MATCHING: .player_3919c moved")

    /* player_39c24 — decompiled to C (src/player_39c24.c, carved by tools/carve.py) */
    .player_39c24 : {
        build/src/player_39c24.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_39c24) == 0x08039C24, "MATCHING: .player_39c24 moved")

    /* player_3aa64 — decompiled to C (src/player_3aa64.c, carved by tools/carve.py) */
    .player_3aa64 : {
        build/src/player_3aa64.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_3aa64) == 0x0803AA64, "MATCHING: .player_3aa64 moved")

    /* player_3bde8 — decompiled to C (src/player_3bde8.c, carved by tools/carve.py) */
    .player_3bde8 : {
        build/src/player_3bde8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_3bde8) == 0x0803BDE8, "MATCHING: .player_3bde8 moved")

    /* stage_3cd60 — decompiled to C (src/stage_3cd60.c, carved by tools/carve.py) */
    .stage_3cd60 : {
        build/src/stage_3cd60.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_3cd60) == 0x0803CD60, "MATCHING: .stage_3cd60 moved")

    /* stage_40b40 — decompiled to C (src/stage_40b40.c, carved by tools/carve.py) */
    .stage_40b40 : {
        build/src/stage_40b40.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_40b40) == 0x08040B40, "MATCHING: .stage_40b40 moved")

    /* stage_413a4 — decompiled to C (src/stage_413a4.c, carved by tools/carve.py) */
    .stage_413a4 : {
        build/src/stage_413a4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_413a4) == 0x080413A4, "MATCHING: .stage_413a4 moved")

    /* stage_4335c — decompiled to C (src/stage_4335c.c, carved by tools/carve.py) */
    .stage_4335c : {
        build/src/stage_4335c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_4335c) == 0x0804335C, "MATCHING: .stage_4335c moved")

    /* stage_43654 — decompiled to C (src/stage_43654.c, carved by tools/carve.py) */
    .stage_43654 : {
        build/src/stage_43654.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.stage_43654) == 0x08043654, "MATCHING: .stage_43654 moved")

    /* player_449c8 — decompiled to C (src/player_449c8.c, carved by tools/carve.py) */
    .player_449c8 : {
        build/src/player_449c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_449c8) == 0x080449C8, "MATCHING: .player_449c8 moved")

    /* player_44d04 — decompiled to C (src/player_44d04.c, carved by tools/carve.py) */
    .player_44d04 : {
        build/src/player_44d04.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_44d04) == 0x08044D04, "MATCHING: .player_44d04 moved")

    /* player_455c8 — decompiled to C (src/player_455c8.c, carved by tools/carve.py) */
    .player_455c8 : {
        build/src/player_455c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_455c8) == 0x080455C8, "MATCHING: .player_455c8 moved")

    /* player_45d34 — decompiled to C (src/player_45d34.c, carved by tools/carve.py) */
    .player_45d34 : {
        build/src/player_45d34.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_45d34) == 0x08045D34, "MATCHING: .player_45d34 moved")

    /* player_46330 — decompiled to C (src/player_46330.c, carved by tools/carve.py) */
    .player_46330 : {
        build/src/player_46330.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_46330) == 0x08046330, "MATCHING: .player_46330 moved")

    /* player_46c00 — decompiled to C (src/player_46c00.c, carved by tools/carve.py) */
    .player_46c00 : {
        build/src/player_46c00.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_46c00) == 0x08046C00, "MATCHING: .player_46c00 moved")

    /* player_474e8 — decompiled to C (src/player_474e8.c, carved by tools/carve.py) */
    .player_474e8 : {
        build/src/player_474e8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_474e8) == 0x080474E8, "MATCHING: .player_474e8 moved")

    /* player_47fe8 — decompiled to C (src/player_47fe8.c, carved by tools/carve.py) */
    .player_47fe8 : {
        build/src/player_47fe8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_47fe8) == 0x08047FE8, "MATCHING: .player_47fe8 moved")

    /* player_49738 — decompiled to C (src/player_49738.c, carved by tools/carve.py) */
    .player_49738 : {
        build/src/player_49738.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_49738) == 0x08049738, "MATCHING: .player_49738 moved")

    /* player_49b48 — decompiled to C (src/player_49b48.c, carved by tools/carve.py) */
    .player_49b48 : {
        build/src/player_49b48.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_49b48) == 0x08049B48, "MATCHING: .player_49b48 moved")

    /* player_49f98 — decompiled to C (src/player_49f98.c, carved by tools/carve.py) */
    .player_49f98 : {
        build/src/player_49f98.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_49f98) == 0x08049F98, "MATCHING: .player_49f98 moved")

    /* player_4a54c — decompiled to C (src/player_4a54c.c, carved by tools/carve.py) */
    .player_4a54c : {
        build/src/player_4a54c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4a54c) == 0x0804A54C, "MATCHING: .player_4a54c moved")

    /* player_4ab70 — decompiled to C (src/player_4ab70.c, carved by tools/carve.py) */
    .player_4ab70 : {
        build/src/player_4ab70.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4ab70) == 0x0804AB70, "MATCHING: .player_4ab70 moved")

    /* player_4b5b4 — decompiled to C (src/player_4b5b4.c, carved by tools/carve.py) */
    .player_4b5b4 : {
        build/src/player_4b5b4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4b5b4) == 0x0804B5B4, "MATCHING: .player_4b5b4 moved")

    /* player_4b858 — decompiled to C (src/player_4b858.c, carved by tools/carve.py) */
    .player_4b858 : {
        build/src/player_4b858.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4b858) == 0x0804B858, "MATCHING: .player_4b858 moved")

    /* player_4c64c — decompiled to C (src/player_4c64c.c, carved by tools/carve.py) */
    .player_4c64c : {
        build/src/player_4c64c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4c64c) == 0x0804C64C, "MATCHING: .player_4c64c moved")

    /* player_4cc7c — decompiled to C (src/player_4cc7c.c, carved by tools/carve.py) */
    .player_4cc7c : {
        build/src/player_4cc7c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4cc7c) == 0x0804CC7C, "MATCHING: .player_4cc7c moved")

    /* player_4dc08 — decompiled to C (src/player_4dc08.c, carved by tools/carve.py) */
    .player_4dc08 : {
        build/src/player_4dc08.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4dc08) == 0x0804DC08, "MATCHING: .player_4dc08 moved")

    /* sub_0804e3a0 — decompiled to C (src/sub_0804e3a0.c, carved by tools/carve.py) */
    .sub_0804e3a0 : {
        build/src/sub_0804e3a0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sub_0804e3a0) == 0x0804E3A0, "MATCHING: .sub_0804e3a0 moved")

    /* player_4e5a4 — decompiled to C (src/player_4e5a4.c, carved by tools/carve.py) */
    .player_4e5a4 : {
        build/src/player_4e5a4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4e5a4) == 0x0804E5A4, "MATCHING: .player_4e5a4 moved")

    /* player_4e78c — decompiled to C (src/player_4e78c.c, carved by tools/carve.py) */
    .player_4e78c : {
        build/src/player_4e78c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4e78c) == 0x0804E78C, "MATCHING: .player_4e78c moved")

    /* player_4ee08 — decompiled to C (src/player_4ee08.c, carved by tools/carve.py) */
    .player_4ee08 : {
        build/src/player_4ee08.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4ee08) == 0x0804EE08, "MATCHING: .player_4ee08 moved")

    /* player_4f614 — decompiled to C (src/player_4f614.c, carved by tools/carve.py) */
    .player_4f614 : {
        build/src/player_4f614.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4f614) == 0x0804F614, "MATCHING: .player_4f614 moved")

    /* player_4f948 — decompiled to C (src/player_4f948.c, carved by tools/carve.py) */
    .player_4f948 : {
        build/src/player_4f948.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4f948) == 0x0804F948, "MATCHING: .player_4f948 moved")

    /* player_4ffdc — decompiled to C (src/player_4ffdc.c, carved by tools/carve.py) */
    .player_4ffdc : {
        build/src/player_4ffdc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_4ffdc) == 0x0804FFDC, "MATCHING: .player_4ffdc moved")

    /* plobj_507bc — decompiled to C (src/plobj_507bc.c, carved by tools/carve.py) */
    .plobj_507bc : {
        build/src/plobj_507bc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.plobj_507bc) == 0x080507BC, "MATCHING: .plobj_507bc moved")

    /* plobj_509ec — decompiled to C (src/plobj_509ec.c, carved by tools/carve.py) */
    .plobj_509ec : {
        build/src/plobj_509ec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.plobj_509ec) == 0x080509EC, "MATCHING: .plobj_509ec moved")

    /* plobj_514f8 — decompiled to C (src/plobj_514f8.c, carved by tools/carve.py) */
    .plobj_514f8 : {
        build/src/plobj_514f8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.plobj_514f8) == 0x080514F8, "MATCHING: .plobj_514f8 moved")

    /* plobj_5239c — decompiled to C (src/plobj_5239c.c, carved by tools/carve.py) */
    .plobj_5239c : {
        build/src/plobj_5239c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.plobj_5239c) == 0x0805239C, "MATCHING: .plobj_5239c moved")

    /* plobj_52f6c — decompiled to C (src/plobj_52f6c.c, carved by tools/carve.py) */
    .plobj_52f6c : {
        build/src/plobj_52f6c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.plobj_52f6c) == 0x08052F6C, "MATCHING: .plobj_52f6c moved")

    /* effect_53af4 — decompiled to C (src/effect_53af4.c, carved by tools/carve.py) */
    .effect_53af4 : {
        build/src/effect_53af4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_53af4) == 0x08053AF4, "MATCHING: .effect_53af4 moved")

    /* effect_54330 — decompiled to C (src/effect_54330.c, carved by tools/carve.py) */
    .effect_54330 : {
        build/src/effect_54330.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_54330) == 0x08054330, "MATCHING: .effect_54330 moved")

    /* effect_54a80 — decompiled to C (src/effect_54a80.c, carved by tools/carve.py) */
    .effect_54a80 : {
        build/src/effect_54a80.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_54a80) == 0x08054A80, "MATCHING: .effect_54a80 moved")

    /* effect_55460 — decompiled to C (src/effect_55460.c, carved by tools/carve.py) */
    .effect_55460 : {
        build/src/effect_55460.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_55460) == 0x08055460, "MATCHING: .effect_55460 moved")

    /* effect_55b24 — decompiled to C (src/effect_55b24.c, carved by tools/carve.py) */
    .effect_55b24 : {
        build/src/effect_55b24.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_55b24) == 0x08055B24, "MATCHING: .effect_55b24 moved")

    /* effect_56448 — decompiled to C (src/effect_56448.c, carved by tools/carve.py) */
    .effect_56448 : {
        build/src/effect_56448.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_56448) == 0x08056448, "MATCHING: .effect_56448 moved")

    /* effect_56dd4 — decompiled to C (src/effect_56dd4.c, carved by tools/carve.py) */
    .effect_56dd4 : {
        build/src/effect_56dd4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_56dd4) == 0x08056DD4, "MATCHING: .effect_56dd4 moved")

    /* effect_57494 — decompiled to C (src/effect_57494.c, carved by tools/carve.py) */
    .effect_57494 : {
        build/src/effect_57494.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_57494) == 0x08057494, "MATCHING: .effect_57494 moved")

    /* effect_57ce0 — decompiled to C (src/effect_57ce0.c, carved by tools/carve.py) */
    .effect_57ce0 : {
        build/src/effect_57ce0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_57ce0) == 0x08057CE0, "MATCHING: .effect_57ce0 moved")

    /* effect_58810 — decompiled to C (src/effect_58810.c, carved by tools/carve.py) */
    .effect_58810 : {
        build/src/effect_58810.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_58810) == 0x08058810, "MATCHING: .effect_58810 moved")

    /* effect_59570 — decompiled to C (src/effect_59570.c, carved by tools/carve.py) */
    .effect_59570 : {
        build/src/effect_59570.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_59570) == 0x08059570, "MATCHING: .effect_59570 moved")

    /* effect_5a358 — decompiled to C (src/effect_5a358.c, carved by tools/carve.py) */
    .effect_5a358 : {
        build/src/effect_5a358.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_5a358) == 0x0805A358, "MATCHING: .effect_5a358 moved")

    /* effect_5afac — decompiled to C (src/effect_5afac.c, carved by tools/carve.py) */
    .effect_5afac : {
        build/src/effect_5afac.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.effect_5afac) == 0x0805AFAC, "MATCHING: .effect_5afac moved")

    /* actor_62584 — decompiled to C (src/actor_62584.c, carved by tools/carve.py) */
    .actor_62584 : {
        build/src/actor_62584.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_62584) == 0x08062584, "MATCHING: .actor_62584 moved")

    /* actor_63698 — decompiled to C (src/actor_63698.c, carved by tools/carve.py) */
    .actor_63698 : {
        build/src/actor_63698.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_63698) == 0x08063698, "MATCHING: .actor_63698 moved")

    /* actor_653ec — decompiled to C (src/actor_653ec.c, carved by tools/carve.py) */
    .actor_653ec : {
        build/src/actor_653ec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_653ec) == 0x080653EC, "MATCHING: .actor_653ec moved")

    /* actor_673ec — decompiled to C (src/actor_673ec.c, carved by tools/carve.py) */
    .actor_673ec : {
        build/src/actor_673ec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_673ec) == 0x080673EC, "MATCHING: .actor_673ec moved")

    /* actor_692fc — decompiled to C (src/actor_692fc.c, carved by tools/carve.py) */
    .actor_692fc : {
        build/src/actor_692fc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_692fc) == 0x080692FC, "MATCHING: .actor_692fc moved")

    /* actor_6a344 — decompiled to C (src/actor_6a344.c, carved by tools/carve.py) */
    .actor_6a344 : {
        build/src/actor_6a344.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6a344) == 0x0806A344, "MATCHING: .actor_6a344 moved")

    /* actor_6ad18 — decompiled to C (src/actor_6ad18.c, carved by tools/carve.py) */
    .actor_6ad18 : {
        build/src/actor_6ad18.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6ad18) == 0x0806AD18, "MATCHING: .actor_6ad18 moved")

    /* actor_6b2e4 — decompiled to C (src/actor_6b2e4.c, carved by tools/carve.py) */
    .actor_6b2e4 : {
        build/src/actor_6b2e4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6b2e4) == 0x0806B2E4, "MATCHING: .actor_6b2e4 moved")

    /* actor_6c2a4 — decompiled to C (src/actor_6c2a4.c, carved by tools/carve.py) */
    .actor_6c2a4 : {
        build/src/actor_6c2a4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6c2a4) == 0x0806C2A4, "MATCHING: .actor_6c2a4 moved")

    /* actor_6cd40 — decompiled to C (src/actor_6cd40.c, carved by tools/carve.py) */
    .actor_6cd40 : {
        build/src/actor_6cd40.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6cd40) == 0x0806CD40, "MATCHING: .actor_6cd40 moved")

    /* actor_6d22c — decompiled to C (src/actor_6d22c.c, carved by tools/carve.py) */
    .actor_6d22c : {
        build/src/actor_6d22c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6d22c) == 0x0806D22C, "MATCHING: .actor_6d22c moved")

    /* actor_6e0f0 — decompiled to C (src/actor_6e0f0.c, carved by tools/carve.py) */
    .actor_6e0f0 : {
        build/src/actor_6e0f0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6e0f0) == 0x0806E0F0, "MATCHING: .actor_6e0f0 moved")

    /* actor_6ef5c — decompiled to C (src/actor_6ef5c.c, carved by tools/carve.py) */
    .actor_6ef5c : {
        build/src/actor_6ef5c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6ef5c) == 0x0806EF5C, "MATCHING: .actor_6ef5c moved")

    /* actor_6ff24 — decompiled to C (src/actor_6ff24.c, carved by tools/carve.py) */
    .actor_6ff24 : {
        build/src/actor_6ff24.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_6ff24) == 0x0806FF24, "MATCHING: .actor_6ff24 moved")

    /* actor_70ec0 — decompiled to C (src/actor_70ec0.c, carved by tools/carve.py) */
    .actor_70ec0 : {
        build/src/actor_70ec0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_70ec0) == 0x08070EC0, "MATCHING: .actor_70ec0 moved")

    /* actor_72d8c — decompiled to C (src/actor_72d8c.c, carved by tools/carve.py) */
    .actor_72d8c : {
        build/src/actor_72d8c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_72d8c) == 0x08072D8C, "MATCHING: .actor_72d8c moved")

    /* actor_74c0c — decompiled to C (src/actor_74c0c.c, carved by tools/carve.py) */
    .actor_74c0c : {
        build/src/actor_74c0c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_74c0c) == 0x08074C0C, "MATCHING: .actor_74c0c moved")

    /* actor_763e8 — decompiled to C (src/actor_763e8.c, carved by tools/carve.py) */
    .actor_763e8 : {
        build/src/actor_763e8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_763e8) == 0x080763E8, "MATCHING: .actor_763e8 moved")

    /* actor_77ae0 — decompiled to C (src/actor_77ae0.c, carved by tools/carve.py) */
    .actor_77ae0 : {
        build/src/actor_77ae0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_77ae0) == 0x08077AE0, "MATCHING: .actor_77ae0 moved")

    /* enemy_78b68 — decompiled to C (src/enemy_78b68.c, carved by tools/carve.py) */
    .enemy_78b68 : {
        build/src/enemy_78b68.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_78b68) == 0x08078B68, "MATCHING: .enemy_78b68 moved")

    /* enemy_7aa5c — decompiled to C (src/enemy_7aa5c.c, carved by tools/carve.py) */
    .enemy_7aa5c : {
        build/src/enemy_7aa5c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_7aa5c) == 0x0807AA5C, "MATCHING: .enemy_7aa5c moved")

    /* enemy_7d3b0 — decompiled to C (src/enemy_7d3b0.c, carved by tools/carve.py) */
    .enemy_7d3b0 : {
        build/src/enemy_7d3b0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_7d3b0) == 0x0807D3B0, "MATCHING: .enemy_7d3b0 moved")

    /* enemy_7f044 — decompiled to C (src/enemy_7f044.c, carved by tools/carve.py) */
    .enemy_7f044 : {
        build/src/enemy_7f044.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_7f044) == 0x0807F044, "MATCHING: .enemy_7f044 moved")

    /* enemy_80b70 — decompiled to C (src/enemy_80b70.c, carved by tools/carve.py) */
    .enemy_80b70 : {
        build/src/enemy_80b70.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_80b70) == 0x08080B70, "MATCHING: .enemy_80b70 moved")

    /* enemy_820b8 — decompiled to C (src/enemy_820b8.c, carved by tools/carve.py) */
    .enemy_820b8 : {
        build/src/enemy_820b8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_820b8) == 0x080820B8, "MATCHING: .enemy_820b8 moved")

    /* enemy_82e68 — decompiled to C (src/enemy_82e68.c, carved by tools/carve.py) */
    .enemy_82e68 : {
        build/src/enemy_82e68.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_82e68) == 0x08082E68, "MATCHING: .enemy_82e68 moved")

    /* enemy_844c4 — decompiled to C (src/enemy_844c4.c, carved by tools/carve.py) */
    .enemy_844c4 : {
        build/src/enemy_844c4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_844c4) == 0x080844C4, "MATCHING: .enemy_844c4 moved")

    /* enemy_84d14 — decompiled to C (src/enemy_84d14.c, carved by tools/carve.py) */
    .enemy_84d14 : {
        build/src/enemy_84d14.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_84d14) == 0x08084D14, "MATCHING: .enemy_84d14 moved")

    /* enemy_860f8 — decompiled to C (src/enemy_860f8.c, carved by tools/carve.py) */
    .enemy_860f8 : {
        build/src/enemy_860f8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_860f8) == 0x080860F8, "MATCHING: .enemy_860f8 moved")

    /* enemy_88000 — decompiled to C (src/enemy_88000.c, carved by tools/carve.py) */
    .enemy_88000 : {
        build/src/enemy_88000.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_88000) == 0x08088000, "MATCHING: .enemy_88000 moved")

    /* enemy_8aa68 — decompiled to C (src/enemy_8aa68.c, carved by tools/carve.py) */
    .enemy_8aa68 : {
        build/src/enemy_8aa68.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_8aa68) == 0x0808AA68, "MATCHING: .enemy_8aa68 moved")

    /* enemy_8cce8 — decompiled to C (src/enemy_8cce8.c, carved by tools/carve.py) */
    .enemy_8cce8 : {
        build/src/enemy_8cce8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_8cce8) == 0x0808CCE8, "MATCHING: .enemy_8cce8 moved")

    /* enemy_8e404 — decompiled to C (src/enemy_8e404.c, carved by tools/carve.py) */
    .enemy_8e404 : {
        build/src/enemy_8e404.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_8e404) == 0x0808E404, "MATCHING: .enemy_8e404 moved")

    /* enemy_8f41c — decompiled to C (src/enemy_8f41c.c, carved by tools/carve.py) */
    .enemy_8f41c : {
        build/src/enemy_8f41c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_8f41c) == 0x0808F41C, "MATCHING: .enemy_8f41c moved")

    /* enemy_9000c — decompiled to C (src/enemy_9000c.c, carved by tools/carve.py) */
    .enemy_9000c : {
        build/src/enemy_9000c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9000c) == 0x0809000C, "MATCHING: .enemy_9000c moved")

    /* enemy_9113c — decompiled to C (src/enemy_9113c.c, carved by tools/carve.py) */
    .enemy_9113c : {
        build/src/enemy_9113c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9113c) == 0x0809113C, "MATCHING: .enemy_9113c moved")

    /* enemy_91f9c — decompiled to C (src/enemy_91f9c.c, carved by tools/carve.py) */
    .enemy_91f9c : {
        build/src/enemy_91f9c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_91f9c) == 0x08091F9C, "MATCHING: .enemy_91f9c moved")

    /* enemy_93f64 — decompiled to C (src/enemy_93f64.c, carved by tools/carve.py) */
    .enemy_93f64 : {
        build/src/enemy_93f64.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_93f64) == 0x08093F64, "MATCHING: .enemy_93f64 moved")

    /* enemy_957bc — decompiled to C (src/enemy_957bc.c, carved by tools/carve.py) */
    .enemy_957bc : {
        build/src/enemy_957bc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_957bc) == 0x080957BC, "MATCHING: .enemy_957bc moved")

    /* enemy_970c4 — decompiled to C (src/enemy_970c4.c, carved by tools/carve.py) */
    .enemy_970c4 : {
        build/src/enemy_970c4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_970c4) == 0x080970C4, "MATCHING: .enemy_970c4 moved")

    /* enemy_974c8 — decompiled to C (src/enemy_974c8.c, carved by tools/carve.py) */
    .enemy_974c8 : {
        build/src/enemy_974c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_974c8) == 0x080974C8, "MATCHING: .enemy_974c8 moved")

    /* enemy_988f8 — decompiled to C (src/enemy_988f8.c, carved by tools/carve.py) */
    .enemy_988f8 : {
        build/src/enemy_988f8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_988f8) == 0x080988F8, "MATCHING: .enemy_988f8 moved")

    /* enemy_99b20 — decompiled to C (src/enemy_99b20.c, carved by tools/carve.py) */
    .enemy_99b20 : {
        build/src/enemy_99b20.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_99b20) == 0x08099B20, "MATCHING: .enemy_99b20 moved")

    /* enemy_9ba44 — decompiled to C (src/enemy_9ba44.c, carved by tools/carve.py) */
    .enemy_9ba44 : {
        build/src/enemy_9ba44.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9ba44) == 0x0809BA44, "MATCHING: .enemy_9ba44 moved")

    /* enemy_9c028 — decompiled to C (src/enemy_9c028.c, carved by tools/carve.py) */
    .enemy_9c028 : {
        build/src/enemy_9c028.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9c028) == 0x0809C028, "MATCHING: .enemy_9c028 moved")

    /* enemy_9c0a8 — decompiled to C (src/enemy_9c0a8.c, carved by tools/carve.py) */
    .enemy_9c0a8 : {
        build/src/enemy_9c0a8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9c0a8) == 0x0809C0A8, "MATCHING: .enemy_9c0a8 moved")

    /* enemy_9cb90 — decompiled to C (src/enemy_9cb90.c, carved by tools/carve.py) */
    .enemy_9cb90 : {
        build/src/enemy_9cb90.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9cb90) == 0x0809CB90, "MATCHING: .enemy_9cb90 moved")

    /* enemy_9cc24 — decompiled to C (src/enemy_9cc24.c, carved by tools/carve.py) */
    .enemy_9cc24 : {
        build/src/enemy_9cc24.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9cc24) == 0x0809CC24, "MATCHING: .enemy_9cc24 moved")

    /* enemy_9d994 — decompiled to C (src/enemy_9d994.c, carved by tools/carve.py) */
    .enemy_9d994 : {
        build/src/enemy_9d994.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9d994) == 0x0809D994, "MATCHING: .enemy_9d994 moved")

    /* enemy_9da1c — decompiled to C (src/enemy_9da1c.c, carved by tools/carve.py) */
    .enemy_9da1c : {
        build/src/enemy_9da1c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9da1c) == 0x0809DA1C, "MATCHING: .enemy_9da1c moved")

    /* enemy_9f2f4 — decompiled to C (src/enemy_9f2f4.c, carved by tools/carve.py) */
    .enemy_9f2f4 : {
        build/src/enemy_9f2f4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9f2f4) == 0x0809F2F4, "MATCHING: .enemy_9f2f4 moved")

    /* enemy_9f37c — decompiled to C (src/enemy_9f37c.c, carved by tools/carve.py) */
    .enemy_9f37c : {
        build/src/enemy_9f37c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9f37c) == 0x0809F37C, "MATCHING: .enemy_9f37c moved")

    /* enemy_9f9dc — decompiled to C (src/enemy_9f9dc.c, carved by tools/carve.py) */
    .enemy_9f9dc : {
        build/src/enemy_9f9dc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9f9dc) == 0x0809F9DC, "MATCHING: .enemy_9f9dc moved")

    /* enemy_9fbd0 — decompiled to C (src/enemy_9fbd0.c, carved by tools/carve.py) */
    .enemy_9fbd0 : {
        build/src/enemy_9fbd0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_9fbd0) == 0x0809FBD0, "MATCHING: .enemy_9fbd0 moved")

    /* enemy_a00ec — decompiled to C (src/enemy_a00ec.c, carved by tools/carve.py) */
    .enemy_a00ec : {
        build/src/enemy_a00ec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a00ec) == 0x080A00EC, "MATCHING: .enemy_a00ec moved")

    /* enemy_a0274 — decompiled to C (src/enemy_a0274.c, carved by tools/carve.py) */
    .enemy_a0274 : {
        build/src/enemy_a0274.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a0274) == 0x080A0274, "MATCHING: .enemy_a0274 moved")

    /* enemy_a1590 — decompiled to C (src/enemy_a1590.c, carved by tools/carve.py) */
    .enemy_a1590 : {
        build/src/enemy_a1590.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a1590) == 0x080A1590, "MATCHING: .enemy_a1590 moved")

    /* enemy_a5644 — decompiled to C (src/enemy_a5644.c, carved by tools/carve.py) */
    .enemy_a5644 : {
        build/src/enemy_a5644.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a5644) == 0x080A5644, "MATCHING: .enemy_a5644 moved")

    /* enemy_a78a0 — decompiled to C (src/enemy_a78a0.c, carved by tools/carve.py) */
    .enemy_a78a0 : {
        build/src/enemy_a78a0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a78a0) == 0x080A78A0, "MATCHING: .enemy_a78a0 moved")

    /* enemy_a7998 — decompiled to C (src/enemy_a7998.c, carved by tools/carve.py) */
    .enemy_a7998 : {
        build/src/enemy_a7998.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a7998) == 0x080A7998, "MATCHING: .enemy_a7998 moved")

    /* enemy_a860c — decompiled to C (src/enemy_a860c.c, carved by tools/carve.py) */
    .enemy_a860c : {
        build/src/enemy_a860c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a860c) == 0x080A860C, "MATCHING: .enemy_a860c moved")

    /* enemy_a87c8 — decompiled to C (src/enemy_a87c8.c, carved by tools/carve.py) */
    .enemy_a87c8 : {
        build/src/enemy_a87c8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a87c8) == 0x080A87C8, "MATCHING: .enemy_a87c8 moved")

    /* enemy_a932c — decompiled to C (src/enemy_a932c.c, carved by tools/carve.py) */
    .enemy_a932c : {
        build/src/enemy_a932c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a932c) == 0x080A932C, "MATCHING: .enemy_a932c moved")

    /* enemy_a93ec — decompiled to C (src/enemy_a93ec.c, carved by tools/carve.py) */
    .enemy_a93ec : {
        build/src/enemy_a93ec.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_a93ec) == 0x080A93EC, "MATCHING: .enemy_a93ec moved")

    /* enemy_aa338 — decompiled to C (src/enemy_aa338.c, carved by tools/carve.py) */
    .enemy_aa338 : {
        build/src/enemy_aa338.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_aa338) == 0x080AA338, "MATCHING: .enemy_aa338 moved")

    /* enemy_ae3bc — decompiled to C (src/enemy_ae3bc.c, carved by tools/carve.py) */
    .enemy_ae3bc : {
        build/src/enemy_ae3bc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.enemy_ae3bc) == 0x080AE3BC, "MATCHING: .enemy_ae3bc moved")

    /* hud_b2fe8 — decompiled to C (src/hud_b2fe8.c, carved by tools/carve.py) */
    .hud_b2fe8 : {
        build/src/hud_b2fe8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_b2fe8) == 0x080B2FE8, "MATCHING: .hud_b2fe8 moved")

    /* hud_b4ea8 — decompiled to C (src/hud_b4ea8.c, carved by tools/carve.py) */
    .hud_b4ea8 : {
        build/src/hud_b4ea8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_b4ea8) == 0x080B4EA8, "MATCHING: .hud_b4ea8 moved")

    /* hud_b5024 — decompiled to C (src/hud_b5024.c, carved by tools/carve.py) */
    .hud_b5024 : {
        build/src/hud_b5024.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_b5024) == 0x080B5024, "MATCHING: .hud_b5024 moved")

    /* hud_b5670 — decompiled to C (src/hud_b5670.c, carved by tools/carve.py) */
    .hud_b5670 : {
        build/src/hud_b5670.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_b5670) == 0x080B5670, "MATCHING: .hud_b5670 moved")

    /* hud_b5840 — decompiled to C (src/hud_b5840.c, carved by tools/carve.py) */
    .hud_b5840 : {
        build/src/hud_b5840.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.hud_b5840) == 0x080B5840, "MATCHING: .hud_b5840 moved")

    /* save_b6154 — decompiled to C (src/save_b6154.c, carved by tools/carve.py) */
    .save_b6154 : {
        build/src/save_b6154.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6154) == 0x080B6154, "MATCHING: .save_b6154 moved")

    /* save_b6290 — decompiled to C (src/save_b6290.c, carved by tools/carve.py) */
    .save_b6290 : {
        build/src/save_b6290.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6290) == 0x080B6290, "MATCHING: .save_b6290 moved")

    /* save_b63a4 — decompiled to C (src/save_b63a4.c, carved by tools/carve.py) */
    .save_b63a4 : {
        build/src/save_b63a4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b63a4) == 0x080B63A4, "MATCHING: .save_b63a4 moved")

    /* save_b6474 — decompiled to C (src/save_b6474.c, carved by tools/carve.py) */
    .save_b6474 : {
        build/src/save_b6474.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6474) == 0x080B6474, "MATCHING: .save_b6474 moved")

    /* save_b6a90 — decompiled to C (src/save_b6a90.c, carved by tools/carve.py) */
    .save_b6a90 : {
        build/src/save_b6a90.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6a90) == 0x080B6A90, "MATCHING: .save_b6a90 moved")

    /* save_b6b08 — decompiled to C (src/save_b6b08.c, carved by tools/carve.py) */
    .save_b6b08 : {
        build/src/save_b6b08.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6b08) == 0x080B6B08, "MATCHING: .save_b6b08 moved")

    /* save_b6c40 — decompiled to C (src/save_b6c40.c, carved by tools/carve.py) */
    .save_b6c40 : {
        build/src/save_b6c40.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6c40) == 0x080B6C40, "MATCHING: .save_b6c40 moved")

    /* save_b6d04 — decompiled to C (src/save_b6d04.c, carved by tools/carve.py) */
    .save_b6d04 : {
        build/src/save_b6d04.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6d04) == 0x080B6D04, "MATCHING: .save_b6d04 moved")

    /* save_b6e44 — decompiled to C (src/save_b6e44.c, carved by tools/carve.py) */
    .save_b6e44 : {
        build/src/save_b6e44.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6e44) == 0x080B6E44, "MATCHING: .save_b6e44 moved")

    /* save_b6f38 — decompiled to C (src/save_b6f38.c, carved by tools/carve.py) */
    .save_b6f38 : {
        build/src/save_b6f38.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b6f38) == 0x080B6F38, "MATCHING: .save_b6f38 moved")

    /* save_b72bc — decompiled to C (src/save_b72bc.c, carved by tools/carve.py) */
    .save_b72bc : {
        build/src/save_b72bc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b72bc) == 0x080B72BC, "MATCHING: .save_b72bc moved")

    /* save_b75a4 — decompiled to C (src/save_b75a4.c, carved by tools/carve.py) */
    .save_b75a4 : {
        build/src/save_b75a4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b75a4) == 0x080B75A4, "MATCHING: .save_b75a4 moved")

    /* save_b77d4 — decompiled to C (src/save_b77d4.c, carved by tools/carve.py) */
    .save_b77d4 : {
        build/src/save_b77d4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b77d4) == 0x080B77D4, "MATCHING: .save_b77d4 moved")

    /* save_b79b8 — decompiled to C (src/save_b79b8.c, carved by tools/carve.py) */
    .save_b79b8 : {
        build/src/save_b79b8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b79b8) == 0x080B79B8, "MATCHING: .save_b79b8 moved")

    /* save_b7a9c — decompiled to C (src/save_b7a9c.c, carved by tools/carve.py) */
    .save_b7a9c : {
        build/src/save_b7a9c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b7a9c) == 0x080B7A9C, "MATCHING: .save_b7a9c moved")

    /* save_b7df4 — decompiled to C (src/save_b7df4.c, carved by tools/carve.py) */
    .save_b7df4 : {
        build/src/save_b7df4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b7df4) == 0x080B7DF4, "MATCHING: .save_b7df4 moved")

    /* save_b7e14 — decompiled to C (src/save_b7e14.c, carved by tools/carve.py) */
    .save_b7e14 : {
        build/src/save_b7e14.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b7e14) == 0x080B7E14, "MATCHING: .save_b7e14 moved")

    /* save_b8694 — decompiled to C (src/save_b8694.c, carved by tools/carve.py) */
    .save_b8694 : {
        build/src/save_b8694.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b8694) == 0x080B8694, "MATCHING: .save_b8694 moved")

    /* save_b8888 — decompiled to C (src/save_b8888.c, carved by tools/carve.py) */
    .save_b8888 : {
        build/src/save_b8888.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b8888) == 0x080B8888, "MATCHING: .save_b8888 moved")

    /* save_b8918 — decompiled to C (src/save_b8918.c, carved by tools/carve.py) */
    .save_b8918 : {
        build/src/save_b8918.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b8918) == 0x080B8918, "MATCHING: .save_b8918 moved")

    /* save_b8ea0 — decompiled to C (src/save_b8ea0.c, carved by tools/carve.py) */
    .save_b8ea0 : {
        build/src/save_b8ea0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.save_b8ea0) == 0x080B8EA0, "MATCHING: .save_b8ea0 moved")

    /* subgame_b9d0c — decompiled to C (src/subgame_b9d0c.c, carved by tools/carve.py) */
    .subgame_b9d0c : {
        build/src/subgame_b9d0c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_b9d0c) == 0x080B9D0C, "MATCHING: .subgame_b9d0c moved")

    /* subgame_ba774 — decompiled to C (src/subgame_ba774.c, carved by tools/carve.py) */
    .subgame_ba774 : {
        build/src/subgame_ba774.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_ba774) == 0x080BA774, "MATCHING: .subgame_ba774 moved")

    /* subgame_bb528 — decompiled to C (src/subgame_bb528.c, carved by tools/carve.py) */
    .subgame_bb528 : {
        build/src/subgame_bb528.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bb528) == 0x080BB528, "MATCHING: .subgame_bb528 moved")

    /* subgame_bc0cc — decompiled to C (src/subgame_bc0cc.c, carved by tools/carve.py) */
    .subgame_bc0cc : {
        build/src/subgame_bc0cc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bc0cc) == 0x080BC0CC, "MATCHING: .subgame_bc0cc moved")

    /* subgame_bd9e8 — decompiled to C (src/subgame_bd9e8.c, carved by tools/carve.py) */
    .subgame_bd9e8 : {
        build/src/subgame_bd9e8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bd9e8) == 0x080BD9E8, "MATCHING: .subgame_bd9e8 moved")

    /* subgame_bda2c — decompiled to C (src/subgame_bda2c.c, carved by tools/carve.py) */
    .subgame_bda2c : {
        build/src/subgame_bda2c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bda2c) == 0x080BDA2C, "MATCHING: .subgame_bda2c moved")

    /* subgame_bf994 — decompiled to C (src/subgame_bf994.c, carved by tools/carve.py) */
    .subgame_bf994 : {
        build/src/subgame_bf994.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_bf994) == 0x080BF994, "MATCHING: .subgame_bf994 moved")

    /* subgame_c0de8 — decompiled to C (src/subgame_c0de8.c, carved by tools/carve.py) */
    .subgame_c0de8 : {
        build/src/subgame_c0de8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c0de8) == 0x080C0DE8, "MATCHING: .subgame_c0de8 moved")

    /* subgame_c1ffc — decompiled to C (src/subgame_c1ffc.c, carved by tools/carve.py) */
    .subgame_c1ffc : {
        build/src/subgame_c1ffc.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c1ffc) == 0x080C1FFC, "MATCHING: .subgame_c1ffc moved")

    /* subgame_c243c — decompiled to C (src/subgame_c243c.c, carved by tools/carve.py) */
    .subgame_c243c : {
        build/src/subgame_c243c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c243c) == 0x080C243C, "MATCHING: .subgame_c243c moved")

    /* subgame_c2ff8 — decompiled to C (src/subgame_c2ff8.c, carved by tools/carve.py) */
    .subgame_c2ff8 : {
        build/src/subgame_c2ff8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c2ff8) == 0x080C2FF8, "MATCHING: .subgame_c2ff8 moved")

    /* subgame_c3648 — decompiled to C (src/subgame_c3648.c, carved by tools/carve.py) */
    .subgame_c3648 : {
        build/src/subgame_c3648.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c3648) == 0x080C3648, "MATCHING: .subgame_c3648 moved")

    /* subgame_c3f44 — decompiled to C (src/subgame_c3f44.c, carved by tools/carve.py) */
    .subgame_c3f44 : {
        build/src/subgame_c3f44.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c3f44) == 0x080C3F44, "MATCHING: .subgame_c3f44 moved")

    /* subgame_c4630 — decompiled to C (src/subgame_c4630.c, carved by tools/carve.py) */
    .subgame_c4630 : {
        build/src/subgame_c4630.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c4630) == 0x080C4630, "MATCHING: .subgame_c4630 moved")

    /* subgame_c4d08 — decompiled to C (src/subgame_c4d08.c, carved by tools/carve.py) */
    .subgame_c4d08 : {
        build/src/subgame_c4d08.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c4d08) == 0x080C4D08, "MATCHING: .subgame_c4d08 moved")

    /* subgame_c5284 — decompiled to C (src/subgame_c5284.c, carved by tools/carve.py) */
    .subgame_c5284 : {
        build/src/subgame_c5284.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c5284) == 0x080C5284, "MATCHING: .subgame_c5284 moved")

    /* subgame_c623c — decompiled to C (src/subgame_c623c.c, carved by tools/carve.py) */
    .subgame_c623c : {
        build/src/subgame_c623c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.subgame_c623c) == 0x080C623C, "MATCHING: .subgame_c623c moved")

    /* sub_080c6258 — decompiled to C (src/sub_080c6258.c, carved by tools/carve.py) */
    .sub_080c6258 : {
        build/src/sub_080c6258.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sub_080c6258) == 0x080C6258, "MATCHING: .sub_080c6258 moved")

    /* mode_c6260 — decompiled to C (src/mode_c6260.c, carved by tools/carve.py) */
    .mode_c6260 : {
        build/src/mode_c6260.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.mode_c6260) == 0x080C6260, "MATCHING: .mode_c6260 moved")

    /* results_c6420 — decompiled to C (src/results_c6420.c, carved by tools/carve.py) */
    .results_c6420 : {
        build/src/results_c6420.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.results_c6420) == 0x080C6420, "MATCHING: .results_c6420 moved")

    /* ending_c6c64 — decompiled to C (src/ending_c6c64.c, carved by tools/carve.py) */
    .ending_c6c64 : {
        build/src/ending_c6c64.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_c6c64) == 0x080C6C64, "MATCHING: .ending_c6c64 moved")

    /* ending_c7e4c — decompiled to C (src/ending_c7e4c.c, carved by tools/carve.py) */
    .ending_c7e4c : {
        build/src/ending_c7e4c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_c7e4c) == 0x080C7E4C, "MATCHING: .ending_c7e4c moved")

    /* ending_c9004 — decompiled to C (src/ending_c9004.c, carved by tools/carve.py) */
    .ending_c9004 : {
        build/src/ending_c9004.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.ending_c9004) == 0x080C9004, "MATCHING: .ending_c9004 moved")

    /* boot_caa3c — decompiled to C (src/boot_caa3c.c, carved by tools/carve.py) */
    .boot_caa3c : {
        build/src/boot_caa3c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.boot_caa3c) == 0x080CAA3C, "MATCHING: .boot_caa3c moved")

    /* boot_caab8 — decompiled to C (src/boot_caab8.c, carved by tools/carve.py) */
    .boot_caab8 : {
        build/src/boot_caab8.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.boot_caab8) == 0x080CAAB8, "MATCHING: .boot_caab8 moved")

    /* gameover_cacf0 — decompiled to C (src/gameover_cacf0.c, carved by tools/carve.py) */
    .gameover_cacf0 : {
        build/src/gameover_cacf0.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.gameover_cacf0) == 0x080CACF0, "MATCHING: .gameover_cacf0 moved")

    /* gameover_cb354 — decompiled to C (src/gameover_cb354.c, carved by tools/carve.py) */
    .gameover_cb354 : {
        build/src/gameover_cb354.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.gameover_cb354) == 0x080CB354, "MATCHING: .gameover_cb354 moved")

    /* gameover_cb64c — decompiled to C (src/gameover_cb64c.c, carved by tools/carve.py) */
    .gameover_cb64c : {
        build/src/gameover_cb64c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.gameover_cb64c) == 0x080CB64C, "MATCHING: .gameover_cb64c moved")

    /* gameover_cbed4 — decompiled to C (src/gameover_cbed4.c, carved by tools/carve.py) */
    .gameover_cbed4 : {
        build/src/gameover_cbed4.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.gameover_cbed4) == 0x080CBED4, "MATCHING: .gameover_cbed4 moved")

    /* gameover_ccd4c — decompiled to C (src/gameover_ccd4c.c, carved by tools/carve.py) */
    .gameover_ccd4c : {
        build/src/gameover_ccd4c.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.gameover_ccd4c) == 0x080CCD4C, "MATCHING: .gameover_ccd4c moved")

    /* credits_cd330 — decompiled to C (src/credits_cd330.c, carved by tools/carve.py) */
    .credits_cd330 : {
        build/src/credits_cd330.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.credits_cd330) == 0x080CD330, "MATCHING: .credits_cd330 moved")

    /* m4a sound engine: asm core (pret m4a_1.s equivalent; hand-scheduled
     * SDK asm, stays asm forever — issue #52). SoundMainRAM's ROM image
     * embeds an ARM-mode mixer loop (see rom-map section 8.2). */
    .m4a_1 : {
        KEEP(*(.m4a_1))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_1) == 0x080CD89C, "MATCHING: .m4a_1 moved")

    /* m4a sound engine: C driver (pret m4a.c equivalent; to be decompiled
     * in issues #53-#55) */
    /* m4a_c1 — decompiled to C (src/m4a_c1.c, carved by tools/carve.py) */
    .m4a_c1 : {
        build/src/m4a_c1.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_c1) == 0x080CE4B8, "MATCHING: .m4a_c1 moved")

    /* m4a_cgb — decompiled to C (src/m4a_cgb.c, carved by tools/carve.py) */
    .m4a_cgb : {
        build/src/m4a_cgb.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_cgb) == 0x080CEFB4, "MATCHING: .m4a_cgb moved")

    /* m4a_ctrl — decompiled to C (src/m4a_ctrl.c, carved by tools/carve.py) */
    .m4a_ctrl : {
        build/src/m4a_ctrl.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_ctrl) == 0x080CF588, "MATCHING: .m4a_ctrl moved")

    /* SDK syscall wrappers (Thumb) */
    .sdk_swi_wrappers : {
        KEEP(*(.sdk_swi_wrappers))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sdk_swi_wrappers) == 0x080CFA4C, "MATCHING: .sdk_swi_wrappers moved")

    /* SDK SoftReset (Thumb) and its literal pool */
    .sdk_reset_helper : {
        KEEP(*(.sdk_reset_helper))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sdk_reset_helper) == 0x080CFA80, "MATCHING: .sdk_reset_helper moved")

    /* SDK SRAM driver (libagbsrd, "SRAM_V112") — decompiled to C
     * (src/agb_sram.c, compiled with old_agbcc). */
    .sram_driver : {
        build/src/agb_sram.o(.text)
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_driver) == 0x080CFA9C, "MATCHING: .sram_driver moved")

    /* C library: call_via_rN, copy loops, division routines (verbatim) */
    .sdk_libc : {
        KEEP(*(.sdk_libc))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sdk_libc) == 0x080CFC30, "MATCHING: .sdk_libc moved")

    /* Interworking veneer (ARM) */
    .interworking_veneer : {
        KEEP(*(.interworking_veneer))
    } > ROM
    ASSERT(!MATCHING || ADDR(.interworking_veneer) == 0x080CFDDC, "MATCHING: .interworking_veneer moved")

    /* Alignment padding between veneer and IRQ table */
    .gap_interworking_veneer_irq_handler_table_14 : {
        KEEP(*(.gap_interworking_veneer_irq_handler_table_14))
    } > ROM
    ASSERT(!MATCHING || ADDR(.gap_interworking_veneer_irq_handler_table_14) == 0x080CFDE4, "MATCHING: .gap_interworking_veneer_irq_handler_table_14 moved")

    /* 14-entry IRQ handler table */
    .irq_handler_table_14 : {
        KEEP(*(.irq_handler_table_14))
    } > ROM
    ASSERT(!MATCHING || ADDR(.irq_handler_table_14) == 0x080CFDE8, "MATCHING: .irq_handler_table_14 moved")

    /* The SRAM id string "AGB  KIRBY" (gSramIdString), WriteSramEx(.., 0x0E000000, 10) src/save_b7a9c.c:29 (#36 phase 2 run 3; formerly the head of lib_misc) */
    .sram_id_string : {
        KEEP(*(.sram_id_string))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_id_string) == 0x080CFE20, "MATCHING: .sram_id_string moved")

    /* Air Grind (M37) rodata (#36 phase 2 run 3; formerly lib_misc, lib_rodata_fir_tables and the head of level_object_tables, not sound-driver or FIR tables, #98): gUnk_080CFE2C-gUnk_080CFFDA (src/subgame_c243c.c ... src/subgame_c4d08.c), palette gUnk_080D0198, sine table gAirGrindSineTable, gUnk_080D059A-gUnk_080D0766 (src/subgame_c5284.c); 0x080D0768-0x080D0788 has no consumer */
    .air_grind_rodata : {
        KEEP(*(.air_grind_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.air_grind_rodata) == 0x080CFE2C, "MATCHING: .air_grind_rodata moved")

    /* Sprite sheets (asset, #36 phase 2 run 3): OAM template streams, tiles, palettes and struct TaskGfx records the frame tables reach; its head holds a RequestCopyList palette and the LZ77 streams gUnk_080D07C8/gUnk_080D1B78, its last 12 bytes the stage-count table gUnk_08334EB4 */
    .sprite_sheets : {
        KEEP(*(.sprite_sheets))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sprite_sheets) == 0x080D0788, "MATCHING: .sprite_sheets moved")

    /* Room BG animations (#36 phase 2): 30 scripts, 181 tile frames, 14 palette fades, 28 palettes (LoadRoomBgAnims, src/camera_2d01c.c) (ldgroup.py: 80 rows in one output section) */
    .room_bg_anims : {
        KEEP(*(.room_bg_anims))
        build/src/data/bg_anim_scripts.o(.bg_anim_083356e0)
        KEEP(*(.room_bg_anims_08335728))
        build/src/data/bg_anim_scripts.o(.bg_anim_08335b48)
        KEEP(*(.room_bg_anims_08335b90))
        build/src/data/bg_anim_scripts.o(.bg_anim_08335db0)
        KEEP(*(.room_bg_anims_08335df8))
        build/src/data/bg_anim_scripts.o(.bg_anim_08336618)
        KEEP(*(.room_bg_anims_08336660))
        build/src/data/bg_anim_scripts.o(.bg_anim_08336e80)
        KEEP(*(.room_bg_anims_08336ec8))
        build/src/data/bg_anim_scripts.o(.bg_anim_083376e8)
        KEEP(*(.room_bg_anims_08337730))
        build/src/data/bg_anim_scripts.o(.bg_anim_08337b50)
        KEEP(*(.room_bg_anims_08337b98))
        build/src/data/bg_anim_scripts.o(.bg_anim_083396b8)
        KEEP(*(.room_bg_anims_08339700))
        build/src/data/bg_anim_scripts.o(.bg_anim_08339b20)
        KEEP(*(.room_bg_anims_08339b68))
        build/src/data/bg_anim_scripts.o(.bg_anim_08339d88)
        KEEP(*(.room_bg_anims_08339dd0))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833a0e8)
        KEEP(*(.room_bg_anims_0833a120))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833abb8)
        KEEP(*(.room_bg_anims_0833abf0))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833ec10)
        KEEP(*(.room_bg_anims_0833ec58))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833f45c)
        KEEP(*(.room_bg_anims_0833f4c4))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833fcc8)
        KEEP(*(.room_bg_anims_0833fd30))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833fe30)
        KEEP(*(.room_bg_anims_0833fe40))
        build/src/data/bg_anim_scripts.o(.bg_anim_0833ff40)
        KEEP(*(.room_bg_anims_0833ff50))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340050)
        KEEP(*(.room_bg_anims_08340088))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340108)
        KEEP(*(.room_bg_anims_08340118))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340198)
        KEEP(*(.room_bg_anims_083401a8))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340228)
        KEEP(*(.room_bg_anims_08340260))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340360)
        KEEP(*(.room_bg_anims_08340370))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340470)
        KEEP(*(.room_bg_anims_08340480))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340580)
        KEEP(*(.room_bg_anims_08340590))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340690)
        KEEP(*(.room_bg_anims_083406d0))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340750)
        KEEP(*(.room_bg_anims_08340760))
        build/src/data/bg_anim_scripts.o(.bg_anim_083407e0)
        KEEP(*(.room_bg_anims_083407f0))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340870)
        KEEP(*(.room_bg_anims_08340880))
        build/src/data/bg_anim_scripts.o(.bg_anim_08340900)
        KEEP(*(.room_bg_anims_08340940))
        build/src/data/bg_anim_scripts.o(.bg_anim_083447d0)
        KEEP(*(.room_bg_anims_083447f8))
        build/src/data/bg_anim_scripts.o(.bg_anim_08344c08)
        KEEP(*(.room_bg_anims_08344c30))
        build/src/data/bg_anim_scripts.o(.bg_anim_08348248)
        KEEP(*(.room_bg_anims_08348280))
        build/src/data/bg_anim_scripts.o(.bg_anim_0834b898)
        KEEP(*(.room_bg_anims_0834b8d0))
        build/src/data/bg_anim_scripts.o(.bg_anim_0834eee8)
        KEEP(*(.room_bg_anims_0834ef20))
        build/src/data/bg_anim_scripts.o(.bg_anim_08350af8)
        KEEP(*(.room_bg_anims_08350b30))
        build/src/data/bg_anim_scripts.o(.bg_anim_08353248)
        KEEP(*(.room_bg_anims_08353280))
        build/src/data/bg_anim_scripts.o(.bg_anim_08355998)
        KEEP(*(.room_bg_anims_083559d0))
        build/src/data/bg_anim_scripts.o(.bg_anim_083580e8)
        KEEP(*(.room_bg_anims_08358120))
        build/src/data/bg_anim_scripts.o(.bg_anim_0835b5bc)
        KEEP(*(.room_bg_anims_0835b604))
        build/src/data/bg_anim_scripts.o(.bg_anim_0835cf9c)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg_anims) == 0x08334EC0, "MATCHING: .room_bg_anims moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script0 == 0x083356E0, "MATCHING: bg_anim_083356e0 moved")
    ASSERT(!MATCHING || room_bg_anims_08335728 == 0x08335728, "MATCHING: room_bg_anims_08335728 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script1 == 0x08335B48, "MATCHING: bg_anim_08335b48 moved")
    ASSERT(!MATCHING || room_bg_anims_08335b90 == 0x08335B90, "MATCHING: room_bg_anims_08335b90 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script2 == 0x08335DB0, "MATCHING: bg_anim_08335db0 moved")
    ASSERT(!MATCHING || room_bg_anims_08335df8 == 0x08335DF8, "MATCHING: room_bg_anims_08335df8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script3 == 0x08336618, "MATCHING: bg_anim_08336618 moved")
    ASSERT(!MATCHING || room_bg_anims_08336660 == 0x08336660, "MATCHING: room_bg_anims_08336660 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script4 == 0x08336E80, "MATCHING: bg_anim_08336e80 moved")
    ASSERT(!MATCHING || room_bg_anims_08336ec8 == 0x08336EC8, "MATCHING: room_bg_anims_08336ec8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script5 == 0x083376E8, "MATCHING: bg_anim_083376e8 moved")
    ASSERT(!MATCHING || room_bg_anims_08337730 == 0x08337730, "MATCHING: room_bg_anims_08337730 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script6 == 0x08337B50, "MATCHING: bg_anim_08337b50 moved")
    ASSERT(!MATCHING || room_bg_anims_08337b98 == 0x08337B98, "MATCHING: room_bg_anims_08337b98 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script7 == 0x083396B8, "MATCHING: bg_anim_083396b8 moved")
    ASSERT(!MATCHING || room_bg_anims_08339700 == 0x08339700, "MATCHING: room_bg_anims_08339700 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script8 == 0x08339B20, "MATCHING: bg_anim_08339b20 moved")
    ASSERT(!MATCHING || room_bg_anims_08339b68 == 0x08339B68, "MATCHING: room_bg_anims_08339b68 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet1Script9 == 0x08339D88, "MATCHING: bg_anim_08339d88 moved")
    ASSERT(!MATCHING || room_bg_anims_08339dd0 == 0x08339DD0, "MATCHING: room_bg_anims_08339dd0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet2Script0 == 0x0833A0E8, "MATCHING: bg_anim_0833a0e8 moved")
    ASSERT(!MATCHING || room_bg_anims_0833a120 == 0x0833A120, "MATCHING: room_bg_anims_0833a120 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet3Script0 == 0x0833ABB8, "MATCHING: bg_anim_0833abb8 moved")
    ASSERT(!MATCHING || room_bg_anims_0833abf0 == 0x0833ABF0, "MATCHING: room_bg_anims_0833abf0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet4Script0 == 0x0833EC10, "MATCHING: bg_anim_0833ec10 moved")
    ASSERT(!MATCHING || room_bg_anims_0833ec58 == 0x0833EC58, "MATCHING: room_bg_anims_0833ec58 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet5Script2 == 0x0833F45C, "MATCHING: bg_anim_0833f45c moved")
    ASSERT(!MATCHING || room_bg_anims_0833f4c4 == 0x0833F4C4, "MATCHING: room_bg_anims_0833f4c4 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet6Script0 == 0x0833FCC8, "MATCHING: bg_anim_0833fcc8 moved")
    ASSERT(!MATCHING || room_bg_anims_0833fd30 == 0x0833FD30, "MATCHING: room_bg_anims_0833fd30 moved")
    ASSERT(!MATCHING || gUnk_0833FE30 == 0x0833FE30, "MATCHING: bg_anim_0833fe30 moved")
    ASSERT(!MATCHING || room_bg_anims_0833fe40 == 0x0833FE40, "MATCHING: room_bg_anims_0833fe40 moved")
    ASSERT(!MATCHING || gUnk_0833FF40 == 0x0833FF40, "MATCHING: bg_anim_0833ff40 moved")
    ASSERT(!MATCHING || room_bg_anims_0833ff50 == 0x0833FF50, "MATCHING: room_bg_anims_0833ff50 moved")
    ASSERT(!MATCHING || gUnk_08340050 == 0x08340050, "MATCHING: bg_anim_08340050 moved")
    ASSERT(!MATCHING || room_bg_anims_08340088 == 0x08340088, "MATCHING: room_bg_anims_08340088 moved")
    ASSERT(!MATCHING || gUnk_08340108 == 0x08340108, "MATCHING: bg_anim_08340108 moved")
    ASSERT(!MATCHING || room_bg_anims_08340118 == 0x08340118, "MATCHING: room_bg_anims_08340118 moved")
    ASSERT(!MATCHING || gUnk_08340198 == 0x08340198, "MATCHING: bg_anim_08340198 moved")
    ASSERT(!MATCHING || room_bg_anims_083401a8 == 0x083401A8, "MATCHING: room_bg_anims_083401a8 moved")
    ASSERT(!MATCHING || gUnk_08340228 == 0x08340228, "MATCHING: bg_anim_08340228 moved")
    ASSERT(!MATCHING || room_bg_anims_08340260 == 0x08340260, "MATCHING: room_bg_anims_08340260 moved")
    ASSERT(!MATCHING || gUnk_08340360 == 0x08340360, "MATCHING: bg_anim_08340360 moved")
    ASSERT(!MATCHING || room_bg_anims_08340370 == 0x08340370, "MATCHING: room_bg_anims_08340370 moved")
    ASSERT(!MATCHING || gUnk_08340470 == 0x08340470, "MATCHING: bg_anim_08340470 moved")
    ASSERT(!MATCHING || room_bg_anims_08340480 == 0x08340480, "MATCHING: room_bg_anims_08340480 moved")
    ASSERT(!MATCHING || gUnk_08340580 == 0x08340580, "MATCHING: bg_anim_08340580 moved")
    ASSERT(!MATCHING || room_bg_anims_08340590 == 0x08340590, "MATCHING: room_bg_anims_08340590 moved")
    ASSERT(!MATCHING || gUnk_08340690 == 0x08340690, "MATCHING: bg_anim_08340690 moved")
    ASSERT(!MATCHING || room_bg_anims_083406d0 == 0x083406D0, "MATCHING: room_bg_anims_083406d0 moved")
    ASSERT(!MATCHING || gUnk_08340750 == 0x08340750, "MATCHING: bg_anim_08340750 moved")
    ASSERT(!MATCHING || room_bg_anims_08340760 == 0x08340760, "MATCHING: room_bg_anims_08340760 moved")
    ASSERT(!MATCHING || gUnk_083407E0 == 0x083407E0, "MATCHING: bg_anim_083407e0 moved")
    ASSERT(!MATCHING || room_bg_anims_083407f0 == 0x083407F0, "MATCHING: room_bg_anims_083407f0 moved")
    ASSERT(!MATCHING || gUnk_08340870 == 0x08340870, "MATCHING: bg_anim_08340870 moved")
    ASSERT(!MATCHING || room_bg_anims_08340880 == 0x08340880, "MATCHING: room_bg_anims_08340880 moved")
    ASSERT(!MATCHING || gUnk_08340900 == 0x08340900, "MATCHING: bg_anim_08340900 moved")
    ASSERT(!MATCHING || room_bg_anims_08340940 == 0x08340940, "MATCHING: room_bg_anims_08340940 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet7Script0 == 0x083447D0, "MATCHING: bg_anim_083447d0 moved")
    ASSERT(!MATCHING || room_bg_anims_083447f8 == 0x083447F8, "MATCHING: room_bg_anims_083447f8 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet8Script0 == 0x08344C08, "MATCHING: bg_anim_08344c08 moved")
    ASSERT(!MATCHING || room_bg_anims_08344c30 == 0x08344C30, "MATCHING: room_bg_anims_08344c30 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet10Script0 == 0x08348248, "MATCHING: bg_anim_08348248 moved")
    ASSERT(!MATCHING || room_bg_anims_08348280 == 0x08348280, "MATCHING: room_bg_anims_08348280 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet10Script1 == 0x0834B898, "MATCHING: bg_anim_0834b898 moved")
    ASSERT(!MATCHING || room_bg_anims_0834b8d0 == 0x0834B8D0, "MATCHING: room_bg_anims_0834b8d0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet10Script2 == 0x0834EEE8, "MATCHING: bg_anim_0834eee8 moved")
    ASSERT(!MATCHING || room_bg_anims_0834ef20 == 0x0834EF20, "MATCHING: room_bg_anims_0834ef20 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet12Script0 == 0x08350AF8, "MATCHING: bg_anim_08350af8 moved")
    ASSERT(!MATCHING || room_bg_anims_08350b30 == 0x08350B30, "MATCHING: room_bg_anims_08350b30 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet9Script0 == 0x08353248, "MATCHING: bg_anim_08353248 moved")
    ASSERT(!MATCHING || room_bg_anims_08353280 == 0x08353280, "MATCHING: room_bg_anims_08353280 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet9Script1 == 0x08355998, "MATCHING: bg_anim_08355998 moved")
    ASSERT(!MATCHING || room_bg_anims_083559d0 == 0x083559D0, "MATCHING: room_bg_anims_083559d0 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet9Script2 == 0x083580E8, "MATCHING: bg_anim_083580e8 moved")
    ASSERT(!MATCHING || room_bg_anims_08358120 == 0x08358120, "MATCHING: room_bg_anims_08358120 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet11Script1 == 0x0835B5BC, "MATCHING: bg_anim_0835b5bc moved")
    ASSERT(!MATCHING || room_bg_anims_0835b604 == 0x0835B604, "MATCHING: room_bg_anims_0835b604 moved")
    ASSERT(!MATCHING || gRoomBgAnimSet11Script0 == 0x0835CF9C, "MATCHING: bg_anim_0835cf9c moved")

    /* Room records (#36 phase 2): 333 RoomDefs, each after its metatile map, block layer, block table, doors and object list */
    .room_data : {
        KEEP(*(.room_data))
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_data) == 0x0835CFD4, "MATCHING: .room_data moved")

    /* Room metatile tables (#36 phase 2): 23 LZ77 RoomDef.metatileTiles streams */
    .room_metatiles : {
        KEEP(*(.room_metatiles))
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_metatiles) == 0x083A862C, "MATCHING: .room_metatiles moved")

    /* Room BG3 maps (#36 phase 2): 32 struct BgMap, 25 LZ77 and 7 raw */
    .room_bg3_maps : {
        KEEP(*(.room_bg3_maps))
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg3_maps) == 0x083B5538, "MATCHING: .room_bg3_maps moved")

    /* Compressed graphics (LZ77/RLE-class) */
    .compressed_graphics : {
        KEEP(*(.compressed_graphics))
    } > ROM
    ASSERT(!MATCHING || ADDR(.compressed_graphics) == 0x083D0148, "MATCHING: .compressed_graphics moved")

    /* Sprite sheets, part 2 (asset, #36 phase 2 run 3; formerly m4a_songs, no m4a data): OAM streams, raw and LZ77 tiles, palettes, TaskGfx records; opens with the tail of compressed_graphics' LZ77 stream gUnk_085BF484 */
    .sprite_sheets_2 : {
        KEEP(*(.sprite_sheets_2))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sprite_sheets_2) == 0x085C0000, "MATCHING: .sprite_sheets_2 moved")

    /* m4a engine rodata tables (issue #51): gMPlayJumpTableTemplate..gXcmdTable */
    .m4a_engine_rodata : {
        KEEP(*(.m4a_engine_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_engine_rodata) == 0x0860A140, "MATCHING: .m4a_engine_rodata moved")

    /* m4a voicegroups (asset, #36 phase 2 run 3; formerly m4a_song_tracks): the five voicegroups and five 4-bit CGB waves tools/m4a_struct.py parses */
    .m4a_voicegroups : {
        KEEP(*(.m4a_voicegroups))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_voicegroups) == 0x0860A418, "MATCHING: .m4a_voicegroups moved")

    /* gMPlayTable + gSongTable (issue #51) */
    .m4a_song_table : {
        KEEP(*(.m4a_song_table))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_song_table) == 0x0860B430, "MATCHING: .m4a_song_table moved")

    /* m4a song data (asset, #36 phase 2 run 3; formerly m4a_songs_2): song headers, track streams, ToneData and all 129 WaveData PCM samples (tools/m4a_struct.py) */
    .m4a_song_data : {
        KEEP(*(.m4a_song_data))
    } > ROM
    ASSERT(!MATCHING || ADDR(.m4a_song_data) == 0x0860C678, "MATCHING: .m4a_song_data moved")

    /* SRAM_V112 identifier string */
    .sram_v112_string : {
        KEEP(*(.sram_v112_string))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_v112_string) == 0x0872E9F8, "MATCHING: .sram_v112_string moved")

    /* Null padding after SRAM string */
    .gap_sram_v112_string_sram_driver_fn_table : {
        KEEP(*(.gap_sram_v112_string_sram_driver_fn_table))
    } > ROM
    ASSERT(!MATCHING || ADDR(.gap_sram_v112_string_sram_driver_fn_table) == 0x0872EA02, "MATCHING: .gap_sram_v112_string_sram_driver_fn_table moved")

    /* SRAM driver function table (4 Thumb pointers) */
    .sram_driver_fn_table : {
        KEEP(*(.sram_driver_fn_table))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sram_driver_fn_table) == 0x0872EA04, "MATCHING: .sram_driver_fn_table moved")

    /* Engine-zone rodata (#36 phase 2 run 3; formerly gap_sram_driver_fn_table_asset_metadata_index): 256 zero bytes, sprite half-dimensions gUnk_0872EB14, gBootSignature, the SE table gSfxTable, gCosTable; gTaskTypes follows */
    .engine_rodata : {
        KEEP(*(.engine_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.engine_rodata) == 0x0872EA14, "MATCHING: .engine_rodata moved")

    /* task_types - functional table in C (src/data/task_types.c, carved by tools/carve_data.py) */
    .task_types : {
        build/src/data/task_types.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.task_types) == 0x0872FF30, "MATCHING: .task_types moved")

    /* Game rodata in link order, M02-M18 (#36 phase 2 run 3; formerly asset_metadata_index): mode, menu, player, collision, room, camera and terrain tables, blocks, player actions, effects, actor core */
    .game_rodata : {
        KEEP(*(.game_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.game_rodata) == 0x08730780, "MATCHING: .game_rodata moved")

    /* actor_defs - functional table in C (src/data/actor_defs.c, carved by tools/carve_data.py) */
    .actor_defs : {
        build/src/data/actor_defs.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_defs) == 0x0873ECEC, "MATCHING: .actor_defs moved")

    /* Actor and enemy rodata, M17-M32 (#36 phase 2 run 3; formerly asset_metadata_index_0873eea0): behaviour tables, ActorDef/ActorAux records, HitBoxSets (ldgroup.py: 69 rows in one output section) */
    .actor_rodata : {
        KEEP(*(.actor_rodata))
        build/src/data/actor_records.o(.actor_rec_0873f2b8)
        KEEP(*(.actor_rodata_0873f4c8))
        build/src/data/actor_records.o(.actor_rec_0873f664)
        KEEP(*(.actor_rodata_0873f6e8))
        build/src/data/actor_records.o(.actor_rec_0873f8ec)
        KEEP(*(.actor_rodata_0873f8f4))
        build/src/data/actor_records.o(.actor_rec_08740bd4)
        KEEP(*(.actor_rodata_08740de4))
        build/src/data/actor_records.o(.actor_rec_087410e4)
        KEEP(*(.actor_rodata_0874113c))
        build/src/data/actor_records.o(.actor_rec_087417b8)
        KEEP(*(.actor_rodata_08741aa4))
        build/src/data/actor_records.o(.actor_rec_08741eb4)
        KEEP(*(.actor_rodata_08741f0c))
        build/src/data/actor_records.o(.actor_rec_087429e8)
        KEEP(*(.actor_rodata_08742bf8))
        build/src/data/actor_records.o(.actor_rec_08742fbc)
        KEEP(*(.actor_rodata_0874306c))
        build/src/data/actor_records.o(.actor_rec_0874330c)
        KEEP(*(.actor_rodata_08743414))
        build/src/data/actor_records.o(.actor_rec_08743644)
        KEEP(*(.actor_rodata_0874369c))
        build/src/data/actor_records.o(.actor_rec_08743b4c)
        KEEP(*(.actor_rodata_08743bd0))
        build/src/data/actor_records.o(.actor_rec_0874402c)
        KEEP(*(.actor_rodata_0874407c))
        build/src/data/actor_records.o(.actor_rec_087441d8)
        KEEP(*(.actor_rodata_0874425c))
        build/src/data/actor_records.o(.actor_rec_08744964)
        KEEP(*(.actor_rodata_087449e8))
        build/src/data/actor_records.o(.actor_rec_0874531c)
        KEEP(*(.actor_rodata_087453bc))
        build/src/data/actor_records.o(.actor_rec_08745530)
        KEEP(*(.actor_rodata_08745588))
        build/src/data/actor_records.o(.actor_rec_08745810)
        KEEP(*(.actor_rodata_08745868))
        build/src/data/actor_records.o(.actor_rec_08745a2c)
        KEEP(*(.actor_rodata_08745a34))
        build/src/data/actor_records.o(.actor_rec_08745b30)
        KEEP(*(.actor_rodata_08745bb4))
        build/src/data/actor_records.o(.actor_rec_08747c80)
        KEEP(*(.actor_rodata_08747ebc))
        build/src/data/actor_records.o(.actor_rec_08748198)
        KEEP(*(.actor_rodata_087481a0))
        build/src/data/actor_records.o(.actor_rec_08748764)
        KEEP(*(.actor_rodata_087487e8))
        build/src/data/actor_records.o(.actor_rec_08748918)
        KEEP(*(.actor_rodata_08748930))
        build/src/data/actor_records.o(.actor_rec_08748ab0)
        KEEP(*(.actor_rodata_08748bb8))
        build/src/data/actor_records.o(.actor_rec_08749514)
        KEEP(*(.actor_rodata_08749598))
        build/src/data/actor_records.o(.actor_rec_08749ae8)
        KEEP(*(.actor_rodata_08749b08))
        build/src/data/actor_records.o(.actor_rec_08749bec)
        KEEP(*(.actor_rodata_08749c70))
        build/src/data/actor_records.o(.actor_rec_0874b2dc)
        KEEP(*(.actor_rodata_0874b38c))
        build/src/data/actor_records.o(.actor_rec_0874b4c8)
        KEEP(*(.actor_rodata_0874b4d8))
        build/src/data/actor_records.o(.actor_rec_0874b940)
        KEEP(*(.actor_rodata_0874bb7c))
        build/src/data/actor_records.o(.actor_rec_0874c158)
        KEEP(*(.actor_rodata_0874c184))
        build/src/data/actor_records.o(.actor_rec_0874c1fc)
        KEEP(*(.actor_rodata_0874c204))
        build/src/data/actor_records.o(.actor_rec_0874c378)
        KEEP(*(.actor_rodata_0874c3d0))
    } > ROM
    ASSERT(!MATCHING || ADDR(.actor_rodata) == 0x0873EEA0, "MATCHING: .actor_rodata moved")
    ASSERT(!MATCHING || gAbilityStarDef == 0x0873F2B8, "MATCHING: actor_rec_0873f2b8 moved")
    ASSERT(!MATCHING || actor_rodata_0873f4c8 == 0x0873F4C8, "MATCHING: actor_rodata_0873f4c8 moved")
    ASSERT(!MATCHING || gInhalableStarDef == 0x0873F664, "MATCHING: actor_rec_0873f664 moved")
    ASSERT(!MATCHING || actor_rodata_0873f6e8 == 0x0873F6E8, "MATCHING: actor_rodata_0873f6e8 moved")
    ASSERT(!MATCHING || gUnk_0873F8EC == 0x0873F8EC, "MATCHING: actor_rec_0873f8ec moved")
    ASSERT(!MATCHING || actor_rodata_0873f8f4 == 0x0873F8F4, "MATCHING: actor_rodata_0873f8f4 moved")
    ASSERT(!MATCHING || gWaddleDeeDef == 0x08740BD4, "MATCHING: actor_rec_08740bd4 moved")
    ASSERT(!MATCHING || actor_rodata_08740de4 == 0x08740DE4, "MATCHING: actor_rodata_08740de4 moved")
    ASSERT(!MATCHING || gPengyIceBreathDef == 0x087410E4, "MATCHING: actor_rec_087410e4 moved")
    ASSERT(!MATCHING || actor_rodata_0874113c == 0x0874113C, "MATCHING: actor_rodata_0874113c moved")
    ASSERT(!MATCHING || gRockyDef == 0x087417B8, "MATCHING: actor_rec_087417b8 moved")
    ASSERT(!MATCHING || actor_rodata_08741aa4 == 0x08741AA4, "MATCHING: actor_rodata_08741aa4 moved")
    ASSERT(!MATCHING || gSirKibbleCutterDef == 0x08741EB4, "MATCHING: actor_rec_08741eb4 moved")
    ASSERT(!MATCHING || actor_rodata_08741f0c == 0x08741F0C, "MATCHING: actor_rodata_08741f0c moved")
    ASSERT(!MATCHING || gNoddyDef == 0x087429E8, "MATCHING: actor_rec_087429e8 moved")
    ASSERT(!MATCHING || actor_rodata_08742bf8 == 0x08742BF8, "MATCHING: actor_rodata_08742bf8 moved")
    ASSERT(!MATCHING || gChillyFreezeDef == 0x08742FBC, "MATCHING: actor_rec_08742fbc moved")
    ASSERT(!MATCHING || actor_rodata_0874306c == 0x0874306C, "MATCHING: actor_rodata_0874306c moved")
    ASSERT(!MATCHING || gBroomHatterDef == 0x0874330C, "MATCHING: actor_rec_0874330c moved")
    ASSERT(!MATCHING || actor_rodata_08743414 == 0x08743414, "MATCHING: actor_rodata_08743414 moved")
    ASSERT(!MATCHING || gLaserBallLaserDef == 0x08743644, "MATCHING: actor_rec_08743644 moved")
    ASSERT(!MATCHING || actor_rodata_0874369c == 0x0874369C, "MATCHING: actor_rodata_0874369c moved")
    ASSERT(!MATCHING || gBonkersDef == 0x08743B4C, "MATCHING: actor_rec_08743b4c moved")
    ASSERT(!MATCHING || actor_rodata_08743bd0 == 0x08743BD0, "MATCHING: actor_rodata_08743bd0 moved")
    ASSERT(!MATCHING || gUnk_0874402C == 0x0874402C, "MATCHING: actor_rec_0874402c moved")
    ASSERT(!MATCHING || actor_rodata_0874407c == 0x0874407C, "MATCHING: actor_rodata_0874407c moved")
    ASSERT(!MATCHING || gBonkersNutDef == 0x087441D8, "MATCHING: actor_rec_087441d8 moved")
    ASSERT(!MATCHING || actor_rodata_0874425c == 0x0874425C, "MATCHING: actor_rodata_0874425c moved")
    ASSERT(!MATCHING || gGrandWheelieDef == 0x08744964, "MATCHING: actor_rec_08744964 moved")
    ASSERT(!MATCHING || actor_rodata_087449e8 == 0x087449E8, "MATCHING: actor_rodata_087449e8 moved")
    ASSERT(!MATCHING || gUnk_0874531C == 0x0874531C, "MATCHING: actor_rec_0874531c moved")
    ASSERT(!MATCHING || actor_rodata_087453bc == 0x087453BC, "MATCHING: actor_rodata_087453bc moved")
    ASSERT(!MATCHING || gGrandWheelieMiniWheelieDef == 0x08745530, "MATCHING: actor_rec_08745530 moved")
    ASSERT(!MATCHING || actor_rodata_08745588 == 0x08745588, "MATCHING: actor_rodata_08745588 moved")
    ASSERT(!MATCHING || gMrFrostyDef == 0x08745810, "MATCHING: actor_rec_08745810 moved")
    ASSERT(!MATCHING || actor_rodata_08745868 == 0x08745868, "MATCHING: actor_rodata_08745868 moved")
    ASSERT(!MATCHING || gUnk_08745A2C == 0x08745A2C, "MATCHING: actor_rec_08745a2c moved")
    ASSERT(!MATCHING || actor_rodata_08745a34 == 0x08745A34, "MATCHING: actor_rodata_08745a34 moved")
    ASSERT(!MATCHING || gMrFrostyIceCubeDef == 0x08745B30, "MATCHING: actor_rec_08745b30 moved")
    ASSERT(!MATCHING || actor_rodata_08745bb4 == 0x08745BB4, "MATCHING: actor_rodata_08745bb4 moved")
    ASSERT(!MATCHING || gUnk_08747C80 == 0x08747C80, "MATCHING: actor_rec_08747c80 moved")
    ASSERT(!MATCHING || actor_rodata_08747ebc == 0x08747EBC, "MATCHING: actor_rodata_08747ebc moved")
    ASSERT(!MATCHING || gUnk_08748198 == 0x08748198, "MATCHING: actor_rec_08748198 moved")
    ASSERT(!MATCHING || actor_rodata_087481a0 == 0x087481A0, "MATCHING: actor_rodata_087481a0 moved")
    ASSERT(!MATCHING || gKingDededeDef == 0x08748764, "MATCHING: actor_rec_08748764 moved")
    ASSERT(!MATCHING || actor_rodata_087487e8 == 0x087487E8, "MATCHING: actor_rodata_087487e8 moved")
    ASSERT(!MATCHING || gUnk_08748918 == 0x08748918, "MATCHING: actor_rec_08748918 moved")
    ASSERT(!MATCHING || actor_rodata_08748930 == 0x08748930, "MATCHING: actor_rodata_08748930 moved")
    ASSERT(!MATCHING || gKingDededeStarDef == 0x08748AB0, "MATCHING: actor_rec_08748ab0 moved")
    ASSERT(!MATCHING || actor_rodata_08748bb8 == 0x08748BB8, "MATCHING: actor_rodata_08748bb8 moved")
    ASSERT(!MATCHING || gMetaKnightDef == 0x08749514, "MATCHING: actor_rec_08749514 moved")
    ASSERT(!MATCHING || actor_rodata_08749598 == 0x08749598, "MATCHING: actor_rodata_08749598 moved")
    ASSERT(!MATCHING || gUnk_08749AE8 == 0x08749AE8, "MATCHING: actor_rec_08749ae8 moved")
    ASSERT(!MATCHING || actor_rodata_08749b08 == 0x08749B08, "MATCHING: actor_rodata_08749b08 moved")
    ASSERT(!MATCHING || gMetaKnightSwordDef == 0x08749BEC, "MATCHING: actor_rec_08749bec moved")
    ASSERT(!MATCHING || actor_rodata_08749c70 == 0x08749C70, "MATCHING: actor_rodata_08749c70 moved")
    ASSERT(!MATCHING || gPaintRollerDef == 0x0874B2DC, "MATCHING: actor_rec_0874b2dc moved")
    ASSERT(!MATCHING || actor_rodata_0874b38c == 0x0874B38C, "MATCHING: actor_rodata_0874b38c moved")
    ASSERT(!MATCHING || gUnk_0874B4C8 == 0x0874B4C8, "MATCHING: actor_rec_0874b4c8 moved")
    ASSERT(!MATCHING || actor_rodata_0874b4d8 == 0x0874B4D8, "MATCHING: actor_rodata_0874b4d8 moved")
    ASSERT(!MATCHING || gPaintRollerPaintingDef == 0x0874B940, "MATCHING: actor_rec_0874b940 moved")
    ASSERT(!MATCHING || actor_rodata_0874bb7c == 0x0874BB7C, "MATCHING: actor_rodata_0874bb7c moved")
    ASSERT(!MATCHING || gWhispyWoodsDef == 0x0874C158, "MATCHING: actor_rec_0874c158 moved")
    ASSERT(!MATCHING || actor_rodata_0874c184 == 0x0874C184, "MATCHING: actor_rodata_0874c184 moved")
    ASSERT(!MATCHING || gUnk_0874C1FC == 0x0874C1FC, "MATCHING: actor_rec_0874c1fc moved")
    ASSERT(!MATCHING || actor_rodata_0874c204 == 0x0874C204, "MATCHING: actor_rodata_0874c204 moved")
    ASSERT(!MATCHING || gWhispyWoodsAppleDef == 0x0874C378, "MATCHING: actor_rec_0874c378 moved")
    ASSERT(!MATCHING || actor_rodata_0874c3d0 == 0x0874C3D0, "MATCHING: actor_rodata_0874c3d0 moved")

    /* Sprite frame tables (#36 phase 2 run 3): the struct TaskGfx * arrays the code installs as Task.frameTable, the player's gUnk_0874CFEC among them */
    .frame_tables : {
        KEEP(*(.frame_tables))
    } > ROM
    ASSERT(!MATCHING || ADDR(.frame_tables) == 0x0874C44C, "MATCHING: .frame_tables moved")

    /* Rodata of M33-M38 in link order (#36 phase 2 run 3): pickups, HUD effects, save, sub-games, ending, boot logo objects, game over, credits */
    .late_game_rodata : {
        KEEP(*(.late_game_rodata))
    } > ROM
    ASSERT(!MATCHING || ADDR(.late_game_rodata) == 0x0875607C, "MATCHING: .late_game_rodata moved")

    /* The 14 recorded input demos of the staff credits' rows 1 and 2 (gUnk_087583CC, CreditsMain src/credits_cd330.c; InputRecorderStart) */
    .credits_demos : {
        KEEP(*(.credits_demos))
    } > ROM
    ASSERT(!MATCHING || ADDR(.credits_demos) == 0x08758448, "MATCHING: .credits_demos moved")

    /* Player frame records {oam | 1, palette, tiles, palette2, tiles2} (PlayerLoadFrameTilesAndPalette, src/stage_3cd60.c) (#36 phase 2 run 3) */
    .player_frame_records : {
        KEEP(*(.player_frame_records))
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_frame_records) == 0x08759DC8, "MATCHING: .player_frame_records moved")

    /* The player sheets' frame lists, format-only (docs/data.md 5.3) (#36 phase 2 run 3) */
    .player_frame_lists : {
        KEEP(*(.player_frame_lists))
    } > ROM
    ASSERT(!MATCHING || ADDR(.player_frame_lists) == 0x08769250, "MATCHING: .player_frame_lists moved")

    /* Separately linked GBA program (asset): the single-pak link client, MultiBootInitWithParams(gUnk_0876B1FC, gUnk_0876F690) src/menu_0d450.c, ExtraModeTitleLoadMultiBootImage src/mode_07b68.c */
    .multiboot_program : {
        KEEP(*(.multiboot_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.multiboot_program) == 0x0876B1FC, "MATCHING: .multiboot_program moved")

    /* Separately linked GBA program (asset): sub-game 0 (Quick Draw), sent to the other players by ExtraModeTitleSendModeData (src/mode_07b68.c) */
    .quick_draw_program : {
        KEEP(*(.quick_draw_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.quick_draw_program) == 0x0876F690, "MATCHING: .quick_draw_program moved")

    /* Separately linked GBA program (asset): sub-game 1 (Bomb Rally), sent to the other players by ExtraModeTitleSendModeData (src/mode_07b68.c) */
    .bomb_rally_program : {
        KEEP(*(.bomb_rally_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.bomb_rally_program) == 0x087954C0, "MATCHING: .bomb_rally_program moved")

    /* Separately linked GBA program (asset): sub-game 2 (Air Grind), sent to the other players by ExtraModeTitleSendModeData (src/mode_07b68.c) */
    .air_grind_program : {
        KEEP(*(.air_grind_program))
    } > ROM
    ASSERT(!MATCHING || ADDR(.air_grind_program) == 0x087C0A4C, "MATCHING: .air_grind_program moved")

    /* room_table - functional table in C (src/data/room_table.c, carved by tools/carve_data.py) */
    .room_table : {
        build/src/data/room_table.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_table) == 0x087E1D58, "MATCHING: .room_table moved")

    /* room_bg_anim_lists - functional table in C (src/data/room_bg_anim_lists.c, carved by tools/carve_data.py) */
    .room_bg_anim_lists : {
        build/src/data/room_bg_anim_lists.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_bg_anim_lists) == 0x087E1E78, "MATCHING: .room_bg_anim_lists moved")

    /* room_lists - functional table in C (src/data/room_lists.c, carved by tools/carve_data.py) */
    .room_lists : {
        build/src/data/room_lists.o(.rodata)
    } > ROM
    ASSERT(!MATCHING || ADDR(.room_lists) == 0x087E1F58, "MATCHING: .room_lists moved")

    /* Sprite frame lists (#36 phase 2): 23 per-sheet lists of frame pointers, one per extended GfxHeader trailer; no code reference found */
    .sprite_frame_lists : {
        KEEP(*(.sprite_frame_lists))
    } > ROM
    ASSERT(!MATCHING || ADDR(.sprite_frame_lists) == 0x087E2570, "MATCHING: .sprite_frame_lists moved")

    /* Zero padding to ROM end */
    .zero_padding : {
        KEEP(*(.zero_padding))
    } > ROM
    ASSERT(!MATCHING || ADDR(.zero_padding) == 0x087E3088, "MATCHING: .zero_padding moved")
    ASSERT(!MATCHING || ADDR(.zero_padding) + SIZEOF(.zero_padding) == 0x08800000, "MATCHING: the ROM does not end at 0x08800000")

    /DISCARD/ : {
        *(*)
    }
}
