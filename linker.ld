MEMORY
{
    ROM : ORIGIN = 0x08000000, LENGTH = 8M
}

SECTIONS
{
    /* GBA cartridge header (0x08000000-0x080000BF), built from source in
     * asm/rom_header.s. The Nintendo logo is copyrighted and is extracted
     * from baserom.gba via .incbin instead of being committed. */
    .rom_header 0x08000000 : {
        KEEP(*(.rom_header)) KEEP(*(.rom_header.tail))
    } > ROM

    /* ===== Segments split from the former main_blob =====
     * Each section is pinned at its exact ROM VMA; boundaries match
     * docs/analysis/rom-map.md and docs/analysis/segments.txt.
     * As decompilation progresses, individual .incbin sections will be
     * replaced with compiled/assembled object files.
     *
     * The `KEEP(*(.name.tail))` patterns collect optional alignment-1 tail
     * sections emitted by tools/split.py: gas pads a section's size up to
     * its instruction alignment (2/4), so odd trailing bytes of a split
     * segment live in a separate section appended right after the main
     * content (see docs/splitting.md). */

    /* crt0 + master ISR (ARM code) — assembled from asm/crt0.s */
    .crt0_master_isr 0x080000C0 : {
        KEEP(*(.crt0_master_isr)) KEEP(*(.crt0_master_isr.tail))
    } > ROM

    /* crt0/ISR literal pool — assembled from asm/crt0.s */
    .crt0_literals 0x08000210 : {
        KEEP(*(.crt0_literals)) KEEP(*(.crt0_literals.tail))
    } > ROM

    /* Task/context-switch helpers (ARM code) */
    .task_switch_helpers 0x08000234 : {
        KEEP(*(.task_switch_helpers)) KEEP(*(.task_switch_helpers.tail))
    } > ROM

    /* Task helper literal pool */
    .task_literals 0x080002E5 : {
        KEEP(*(.task_literals)) KEEP(*(.task_literals.tail))
    } > ROM

    /* AgbInit (Thumb) — decompiled to C (src/agb_init.c, issue #28).
     * The section spans 0x08000310-0x080008E7: the function body, the
     * compiler's pool-skip branch at 0x0700 and its literal pool. */
    .agb_init 0x08000310 : {
        build/src/agb_init.o(.text)
    } > ROM

    /* Game code: early subsystems (Thumb) */
    /* early_08e8 — decompiled to C (src/early_08e8.c, carved by tools/carve.py) */
    .early_08e8 0x080008E8 : {
        build/src/early_08e8.o(.text)
    } > ROM

    /* early_0de4 — decompiled to C (src/early_0de4.c, carved by tools/carve.py) */
    .early_0de4 0x08000DE4 : {
        build/src/early_0de4.o(.text)
    } > ROM

    /* early_10cc — decompiled to C (src/early_10cc.c, carved by tools/carve.py) */
    .early_10cc 0x080010CC : {
        build/src/early_10cc.o(.text)
    } > ROM

    /* early_11ac — decompiled to C (src/early_11ac.c, carved by tools/carve.py) */
    .early_11ac 0x080011AC : {
        build/src/early_11ac.o(.text)
    } > ROM

    /* early_1518 — decompiled to C (src/early_1518.c, carved by tools/carve.py) */
    .early_1518 0x08001518 : {
        build/src/early_1518.o(.text)
    } > ROM

    /* early_1b08 — decompiled to C (src/early_1b08.c, carved by tools/carve.py) */
    .early_1b08 0x08001B08 : {
        build/src/early_1b08.o(.text)
    } > ROM

    .game_code_early_080011ac 0x08001CC8 : {
        KEEP(*(.game_code_early_080011ac)) KEEP(*(.game_code_early_080011ac.tail))
    } > ROM

    /* early_1fd0 — decompiled to C (src/early_1fd0.c, carved by tools/carve.py) */
    .early_1fd0 0x08001FD0 : {
        build/src/early_1fd0.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378 0x08002378 : {
        KEEP(*(.game_code_early_080011ac_08002378)) KEEP(*(.game_code_early_080011ac_08002378.tail))
    } > ROM

    /* early_2668 — decompiled to C (src/early_2668.c, carved by tools/carve.py) */
    .early_2668 0x08002668 : {
        build/src/early_2668.o(.text)
    } > ROM

    /* early_293c — decompiled to C (src/early_293c.c, carved by tools/carve.py) */
    .early_293c 0x0800293C : {
        build/src/early_293c.o(.text)
    } > ROM

    /* early_2b04 — decompiled to C (src/early_2b04.c, carved by tools/carve.py) */
    .early_2b04 0x08002B04 : {
        build/src/early_2b04.o(.text)
    } > ROM

    /* early_3110 — decompiled to C (src/early_3110.c, carved by tools/carve.py) */
    .early_3110 0x08003110 : {
        build/src/early_3110.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110 0x080031B8 : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110)) KEEP(*(.game_code_early_080011ac_08002378_08003110.tail))
    } > ROM

    /* early_3484 — decompiled to C (src/early_3484.c, carved by tools/carve.py) */
    .early_3484 0x08003484 : {
        build/src/early_3484.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888 0x08003888 : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888.tail))
    } > ROM

    /* early_3964 — decompiled to C (src/early_3964.c, carved by tools/carve.py) */
    .early_3964 0x08003964 : {
        build/src/early_3964.o(.text)
    } > ROM

    /* early_4000 — decompiled to C (src/early_4000.c, carved by tools/carve.py) */
    .early_4000 0x08004000 : {
        build/src/early_4000.o(.text)
    } > ROM

    /* early_4734 — decompiled to C (src/early_4734.c, carved by tools/carve.py) */
    .early_4734 0x08004734 : {
        build/src/early_4734.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888_08004000 0x08004984 : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000.tail))
    } > ROM

    /* early_4d6c — decompiled to C (src/early_4d6c.c, carved by tools/carve.py) */
    .early_4d6c 0x08004D6C : {
        build/src/early_4d6c.o(.text)
    } > ROM

    /* early_4fec — decompiled to C (src/early_4fec.c, carved by tools/carve.py) */
    .early_4fec 0x08004FEC : {
        build/src/early_4fec.o(.text)
    } > ROM

    /* early_5228 — decompiled to C (src/early_5228.c, carved by tools/carve.py) */
    .early_5228 0x08005228 : {
        build/src/early_5228.o(.text)
    } > ROM

    /* early_55b0 — decompiled to C (src/early_55b0.c, carved by tools/carve.py) */
    .early_55b0 0x080055B0 : {
        build/src/early_55b0.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654 0x08005654 : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654.tail))
    } > ROM

    /* early_58e4 — decompiled to C (src/early_58e4.c, carved by tools/carve.py) */
    .early_58e4 0x080058E4 : {
        build/src/early_58e4.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc 0x08005ACC : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc.tail))
    } > ROM

    /* early_5c4c — decompiled to C (src/early_5c4c.c, carved by tools/carve.py) */
    .early_5c4c 0x08005C4C : {
        build/src/early_5c4c.o(.text)
    } > ROM

    /* early_5d9c — decompiled to C (src/early_5d9c.c, carved by tools/carve.py) */
    .early_5d9c 0x08005D9C : {
        build/src/early_5d9c.o(.text)
    } > ROM

    /* early_6464 — decompiled to C (src/early_6464.c, carved by tools/carve.py) */
    .early_6464 0x08006464 : {
        build/src/early_6464.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c 0x08006AC8 : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c.tail))
    } > ROM

    /* early_6cd4 — decompiled to C (src/early_6cd4.c, carved by tools/carve.py) */
    .early_6cd4 0x08006CD4 : {
        build/src/early_6cd4.o(.text)
    } > ROM

    /* early_6d18 — decompiled to C (src/early_6d18.c, carved by tools/carve.py) */
    .early_6d18 0x08006D18 : {
        build/src/early_6d18.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c_08006d18 0x08006D28 : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c_08006d18)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c_08006d18.tail))
    } > ROM

    /* early_6e8c — decompiled to C (src/early_6e8c.c, carved by tools/carve.py) */
    .early_6e8c 0x08006E8C : {
        build/src/early_6e8c.o(.text)
    } > ROM

    .game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c_08006d18_08006e9c 0x08006E9C : {
        KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c_08006d18_08006e9c)) KEEP(*(.game_code_early_080011ac_08002378_08003110_08003888_08004000_08004fec_08005654_08005acc_08005d9c_08006d18_08006e9c.tail))
    } > ROM

    /* early_7004 — decompiled to C (src/early_7004.c, carved by tools/carve.py) */
    .early_7004 0x08007004 : {
        build/src/early_7004.o(.text)
    } > ROM

    /* Game code + rodata (Thumb) — bulk of the game logic */
    /* main — decompiled to C (src/main.c, carved by tools/carve.py) */
    .main 0x08007300 : {
        build/src/main.o(.text)
    } > ROM

    /* mode_075b8 — decompiled to C (src/mode_075b8.c, carved by tools/carve.py) */
    .mode_075b8 0x080075B8 : {
        build/src/mode_075b8.o(.text)
    } > ROM

    /* mode_07b68 — decompiled to C (src/mode_07b68.c, carved by tools/carve.py) */
    .mode_07b68 0x08007B68 : {
        build/src/mode_07b68.o(.text)
    } > ROM

    /* mode_082d0 — decompiled to C (src/mode_082d0.c, carved by tools/carve.py) */
    .mode_082d0 0x080082D0 : {
        build/src/mode_082d0.o(.text)
    } > ROM

    /* mode_08664 — decompiled to C (src/mode_08664.c, carved by tools/carve.py) */
    .mode_08664 0x08008664 : {
        build/src/mode_08664.o(.text)
    } > ROM

    /* gfx_08b8c — decompiled to C (src/gfx_08b8c.c, carved by tools/carve.py) */
    .gfx_08b8c 0x08008B8C : {
        build/src/gfx_08b8c.o(.text)
    } > ROM

    /* boot_091ac — decompiled to C (src/boot_091ac.c, carved by tools/carve.py) */
    .boot_091ac 0x080091AC : {
        build/src/boot_091ac.o(.text)
    } > ROM

    /* hud_099fc — decompiled to C (src/hud_099fc.c, carved by tools/carve.py) */
    .hud_099fc 0x080099FC : {
        build/src/hud_099fc.o(.text)
    } > ROM

    /* hud_0a130 — decompiled to C (src/hud_0a130.c, carved by tools/carve.py) */
    .hud_0a130 0x0800A130 : {
        build/src/hud_0a130.o(.text)
    } > ROM

    /* hud_0aad0 — decompiled to C (src/hud_0aad0.c, carved by tools/carve.py) */
    .hud_0aad0 0x0800AAD0 : {
        build/src/hud_0aad0.o(.text)
    } > ROM

    /* hud_0b318 — decompiled to C (src/hud_0b318.c, carved by tools/carve.py) */
    .hud_0b318 0x0800B318 : {
        build/src/hud_0b318.o(.text)
    } > ROM

    /* mode_0b44c — decompiled to C (src/mode_0b44c.c, carved by tools/carve.py) */
    .mode_0b44c 0x0800B44C : {
        build/src/mode_0b44c.o(.text)
    } > ROM

    /* menu_0b920 — decompiled to C (src/menu_0b920.c, carved by tools/carve.py) */
    .menu_0b920 0x0800B920 : {
        build/src/menu_0b920.o(.text)
    } > ROM

    /* menu_0c09c — decompiled to C (src/menu_0c09c.c, carved by tools/carve.py) */
    .menu_0c09c 0x0800C09C : {
        build/src/menu_0c09c.o(.text)
    } > ROM

    /* menu_0ca10 — decompiled to C (src/menu_0ca10.c, carved by tools/carve.py) */
    .menu_0ca10 0x0800CA10 : {
        build/src/menu_0ca10.o(.text)
    } > ROM

    /* menu_0d450 — decompiled to C (src/menu_0d450.c, carved by tools/carve.py) */
    .menu_0d450 0x0800D450 : {
        build/src/menu_0d450.o(.text)
    } > ROM

    /* menutask_0daf8 — decompiled to C (src/menutask_0daf8.c, carved by tools/carve.py) */
    .menutask_0daf8 0x0800DAF8 : {
        build/src/menutask_0daf8.o(.text)
    } > ROM

    /* menutask_0e314 — decompiled to C (src/menutask_0e314.c, carved by tools/carve.py) */
    .menutask_0e314 0x0800E314 : {
        build/src/menutask_0e314.o(.text)
    } > ROM

    /* menutask_0ea0c — decompiled to C (src/menutask_0ea0c.c, carved by tools/carve.py) */
    .menutask_0ea0c 0x0800EA0C : {
        build/src/menutask_0ea0c.o(.text)
    } > ROM

    /* menutask_0f180 — decompiled to C (src/menutask_0f180.c, carved by tools/carve.py) */
    .menutask_0f180 0x0800F180 : {
        build/src/menutask_0f180.o(.text)
    } > ROM

    /* bgscroll_0fcbc — decompiled to C (src/bgscroll_0fcbc.c, carved by tools/carve.py) */
    .bgscroll_0fcbc 0x0800FCBC : {
        build/src/bgscroll_0fcbc.o(.text)
    } > ROM

    /* mode_100ac — decompiled to C (src/mode_100ac.c, carved by tools/carve.py) */
    .mode_100ac 0x080100AC : {
        build/src/mode_100ac.o(.text)
    } > ROM

    /* player_10358 — decompiled to C (src/player_10358.c, carved by tools/carve.py) */
    .player_10358 0x08010358 : {
        build/src/player_10358.o(.text)
    } > ROM

    /* player_109c8 — decompiled to C (src/player_109c8.c, carved by tools/carve.py) */
    .player_109c8 0x080109C8 : {
        build/src/player_109c8.o(.text)
    } > ROM

    /* player_10b38 — decompiled to C (src/player_10b38.c, carved by tools/carve.py) */
    .player_10b38 0x08010B38 : {
        build/src/player_10b38.o(.text)
    } > ROM

    /* player_17668 — decompiled to C (src/player_17668.c, carved by tools/carve.py) */
    .player_17668 0x08017668 : {
        build/src/player_17668.o(.text)
    } > ROM

    /* player_18b84 — decompiled to C (src/player_18b84.c, carved by tools/carve.py) */
    .player_18b84 0x08018B84 : {
        build/src/player_18b84.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84 0x08018E14 : {
        KEEP(*(.game_code_and_rodata_08018b84)) KEEP(*(.game_code_and_rodata_08018b84.tail))
    } > ROM

    /* player_19000 — decompiled to C (src/player_19000.c, carved by tools/carve.py) */
    .player_19000 0x08019000 : {
        build/src/player_19000.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec 0x08019EEC : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec)) KEEP(*(.game_code_and_rodata_08018b84_08019eec.tail))
    } > ROM

    /* player_1a07c — decompiled to C (src/player_1a07c.c, carved by tools/carve.py) */
    .player_1a07c 0x0801A07C : {
        build/src/player_1a07c.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4 0x0801A3E4 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4.tail))
    } > ROM

    /* player_1a76c — decompiled to C (src/player_1a76c.c, carved by tools/carve.py) */
    .player_1a76c 0x0801A76C : {
        build/src/player_1a76c.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8 0x0801A8C8 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8.tail))
    } > ROM

    /* terrain_1bcac — decompiled to C (src/terrain_1bcac.c, carved by tools/carve.py) */
    .terrain_1bcac 0x0801BCAC : {
        build/src/terrain_1bcac.o(.text)
    } > ROM

    /* terrain_1c30c — decompiled to C (src/terrain_1c30c.c, carved by tools/carve.py) */
    .terrain_1c30c 0x0801C30C : {
        build/src/terrain_1c30c.o(.text)
    } > ROM

    /* terrain_1c444 — decompiled to C (src/terrain_1c444.c, carved by tools/carve.py) */
    .terrain_1c444 0x0801C444 : {
        build/src/terrain_1c444.o(.text)
    } > ROM

    /* terrain_1c51c — decompiled to C (src/terrain_1c51c.c, carved by tools/carve.py) */
    .terrain_1c51c 0x0801C51C : {
        build/src/terrain_1c51c.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_0801c444_0801c690 0x0801C690 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_0801c444_0801c690)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_0801c444_0801c690.tail))
    } > ROM

    /* terrain_1c8dc — decompiled to C (src/terrain_1c8dc.c, carved by tools/carve.py) */
    .terrain_1c8dc 0x0801C8DC : {
        build/src/terrain_1c8dc.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_0801c444_0801c690_0801c930 0x0801C930 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_0801c444_0801c690_0801c930)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_0801c444_0801c690_0801c930.tail))
    } > ROM

    /* terrain_2069c — decompiled to C (src/terrain_2069c.c, carved by tools/carve.py) */
    .terrain_2069c 0x0802069C : {
        build/src/terrain_2069c.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_080207a0 0x080207A0 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_080207a0)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_080207a0.tail))
    } > ROM

    /* terrain_21130 — decompiled to C (src/terrain_21130.c, carved by tools/carve.py) */
    .terrain_21130 0x08021130 : {
        build/src/terrain_21130.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_080207a0_0802136c 0x0802136C : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_080207a0_0802136c)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_080207a0_0802136c.tail))
    } > ROM

    /* terrain_214e0 — decompiled to C (src/terrain_214e0.c, carved by tools/carve.py) */
    .terrain_214e0 0x080214E0 : {
        build/src/terrain_214e0.o(.text)
    } > ROM

    /* terrain_21b18 — decompiled to C (src/terrain_21b18.c, carved by tools/carve.py) */
    .terrain_21b18 0x08021B18 : {
        build/src/terrain_21b18.o(.text)
    } > ROM

    /* level_2296c — decompiled to C (src/level_2296c.c, carved by tools/carve.py) */
    .level_2296c 0x0802296C : {
        build/src/level_2296c.o(.text)
    } > ROM

    /* roomtask_23618 — decompiled to C (src/roomtask_23618.c, carved by tools/carve.py) */
    .roomtask_23618 0x08023618 : {
        build/src/roomtask_23618.o(.text)
    } > ROM

    /* level_23948 — decompiled to C (src/level_23948.c, carved by tools/carve.py) */
    .level_23948 0x08023948 : {
        build/src/level_23948.o(.text)
    } > ROM

    /* level_242d0 — decompiled to C (src/level_242d0.c, carved by tools/carve.py) */
    .level_242d0 0x080242D0 : {
        build/src/level_242d0.o(.text)
    } > ROM

    /* stage_261c0 — decompiled to C (src/stage_261c0.c, carved by tools/carve.py) */
    .stage_261c0 0x080261C0 : {
        build/src/stage_261c0.o(.text)
    } > ROM

    /* door_26b60 — decompiled to C (src/door_26b60.c, carved by tools/carve.py) */
    .door_26b60 0x08026B60 : {
        build/src/door_26b60.o(.text)
    } > ROM

    /* stage_270d0 — decompiled to C (src/stage_270d0.c, carved by tools/carve.py) */
    .stage_270d0 0x080270D0 : {
        build/src/stage_270d0.o(.text)
    } > ROM

    /* stage_273a0 — decompiled to C (src/stage_273a0.c, carved by tools/carve.py) */
    .stage_273a0 0x080273A0 : {
        build/src/stage_273a0.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08021b18_08023948_08024e40_080270d0_08027a6c 0x08027A6C : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08021b18_08023948_08024e40_080270d0_08027a6c)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08021b18_08023948_08024e40_080270d0_08027a6c.tail))
    } > ROM

    /* room_27e28 — decompiled to C (src/room_27e28.c, carved by tools/carve.py) */
    .room_27e28 0x08027E28 : {
        build/src/room_27e28.o(.text)
    } > ROM

    /* room_28320 — decompiled to C (src/room_28320.c, carved by tools/carve.py) */
    .room_28320 0x08028320 : {
        build/src/room_28320.o(.text)
    } > ROM

    /* camera_28b8c — decompiled to C (src/camera_28b8c.c, carved by tools/carve.py) */
    .camera_28b8c 0x08028B8C : {
        build/src/camera_28b8c.o(.text)
    } > ROM

    /* camera_296a0 — decompiled to C (src/camera_296a0.c, carved by tools/carve.py) */
    .camera_296a0 0x080296A0 : {
        build/src/camera_296a0.o(.text)
    } > ROM

    /* camera_29c74 — decompiled to C (src/camera_29c74.c, carved by tools/carve.py) */
    .camera_29c74 0x08029C74 : {
        build/src/camera_29c74.o(.text)
    } > ROM

    /* bgmap_2a9cc — decompiled to C (src/bgmap_2a9cc.c, carved by tools/carve.py) */
    .bgmap_2a9cc 0x0802A9CC : {
        build/src/bgmap_2a9cc.o(.text)
    } > ROM

    /* bgmap_2b2f0 — decompiled to C (src/bgmap_2b2f0.c, carved by tools/carve.py) */
    .bgmap_2b2f0 0x0802B2F0 : {
        build/src/bgmap_2b2f0.o(.text)
    } > ROM

    /* camera_2b4bc — decompiled to C (src/camera_2b4bc.c, carved by tools/carve.py) */
    .camera_2b4bc 0x0802B4BC : {
        build/src/camera_2b4bc.o(.text)
    } > ROM

    /* camera_2c42c — decompiled to C (src/camera_2c42c.c, carved by tools/carve.py) */
    .camera_2c42c 0x0802C42C : {
        build/src/camera_2c42c.o(.text)
    } > ROM

    /* camera_2d01c — decompiled to C (src/camera_2d01c.c, carved by tools/carve.py) */
    .camera_2d01c 0x0802D01C : {
        build/src/camera_2d01c.o(.text)
    } > ROM

    /* camtask_2d38c — decompiled to C (src/camtask_2d38c.c, carved by tools/carve.py) */
    .camtask_2d38c 0x0802D38C : {
        build/src/camtask_2d38c.o(.text)
    } > ROM

    /* obj_2eac8 — decompiled to C (src/obj_2eac8.c, carved by tools/carve.py) */
    .obj_2eac8 0x0802EAC8 : {
        build/src/obj_2eac8.o(.text)
    } > ROM

    /* obj_2f62c — decompiled to C (src/obj_2f62c.c, carved by tools/carve.py) */
    .obj_2f62c 0x0802F62C : {
        build/src/obj_2f62c.o(.text)
    } > ROM

    /* obj_30238 — decompiled to C (src/obj_30238.c, carved by tools/carve.py) */
    .obj_30238 0x08030238 : {
        build/src/obj_30238.o(.text)
    } > ROM

    /* obj_306b4 — decompiled to C (src/obj_306b4.c, carved by tools/carve.py) */
    .obj_306b4 0x080306B4 : {
        build/src/obj_306b4.o(.text)
    } > ROM

    /* block_30804 — decompiled to C (src/block_30804.c, carved by tools/carve.py) */
    .block_30804 0x08030804 : {
        build/src/block_30804.o(.text)
    } > ROM

    /* block_318b4 — decompiled to C (src/block_318b4.c, carved by tools/carve.py) */
    .block_318b4 0x080318B4 : {
        build/src/block_318b4.o(.text)
    } > ROM

    /* player_32688 — decompiled to C (src/player_32688.c, carved by tools/carve.py) */
    .player_32688 0x08032688 : {
        build/src/player_32688.o(.text)
    } > ROM

    /* player_337f4 — decompiled to C (src/player_337f4.c, carved by tools/carve.py) */
    .player_337f4 0x080337F4 : {
        build/src/player_337f4.o(.text)
    } > ROM

    /* player_343c0 — decompiled to C (src/player_343c0.c, carved by tools/carve.py) */
    .player_343c0 0x080343C0 : {
        build/src/player_343c0.o(.text)
    } > ROM

    /* player_34f8c — decompiled to C (src/player_34f8c.c, carved by tools/carve.py) */
    .player_34f8c 0x08034F8C : {
        build/src/player_34f8c.o(.text)
    } > ROM

    /* player_36280 — decompiled to C (src/player_36280.c, carved by tools/carve.py) */
    .player_36280 0x08036280 : {
        build/src/player_36280.o(.text)
    } > ROM

    /* player_36c94 — decompiled to C (src/player_36c94.c, carved by tools/carve.py) */
    .player_36c94 0x08036C94 : {
        build/src/player_36c94.o(.text)
    } > ROM

    /* player_37ed8 — decompiled to C (src/player_37ed8.c, carved by tools/carve.py) */
    .player_37ed8 0x08037ED8 : {
        build/src/player_37ed8.o(.text)
    } > ROM

    /* player_3919c — decompiled to C (src/player_3919c.c, carved by tools/carve.py) */
    .player_3919c 0x0803919C : {
        build/src/player_3919c.o(.text)
    } > ROM

    /* player_39c24 — decompiled to C (src/player_39c24.c, carved by tools/carve.py) */
    .player_39c24 0x08039C24 : {
        build/src/player_39c24.o(.text)
    } > ROM

    /* player_3aa64 — decompiled to C (src/player_3aa64.c, carved by tools/carve.py) */
    .player_3aa64 0x0803AA64 : {
        build/src/player_3aa64.o(.text)
    } > ROM

    /* player_3bde8 — decompiled to C (src/player_3bde8.c, carved by tools/carve.py) */
    .player_3bde8 0x0803BDE8 : {
        build/src/player_3bde8.o(.text)
    } > ROM

    /* stage_3cd60 — decompiled to C (src/stage_3cd60.c, carved by tools/carve.py) */
    .stage_3cd60 0x0803CD60 : {
        build/src/stage_3cd60.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40 0x08040B40 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40.tail))
    } > ROM

    /* stage_413a4 — decompiled to C (src/stage_413a4.c, carved by tools/carve.py) */
    .stage_413a4 0x080413A4 : {
        build/src/stage_413a4.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40_0804335c 0x0804335C : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40_0804335c)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40_0804335c.tail))
    } > ROM

    /* stage_43654 — decompiled to C (src/stage_43654.c, carved by tools/carve.py) */
    .stage_43654 0x08043654 : {
        build/src/stage_43654.o(.text)
    } > ROM

    /* player_449c8 — decompiled to C (src/player_449c8.c, carved by tools/carve.py) */
    .player_449c8 0x080449C8 : {
        build/src/player_449c8.o(.text)
    } > ROM

    /* player_44d04 — decompiled to C (src/player_44d04.c, carved by tools/carve.py) */
    .player_44d04 0x08044D04 : {
        build/src/player_44d04.o(.text)
    } > ROM

    /* player_455c8 — decompiled to C (src/player_455c8.c, carved by tools/carve.py) */
    .player_455c8 0x080455C8 : {
        build/src/player_455c8.o(.text)
    } > ROM

    /* player_45d34 — decompiled to C (src/player_45d34.c, carved by tools/carve.py) */
    .player_45d34 0x08045D34 : {
        build/src/player_45d34.o(.text)
    } > ROM

    /* player_46330 — decompiled to C (src/player_46330.c, carved by tools/carve.py) */
    .player_46330 0x08046330 : {
        build/src/player_46330.o(.text)
    } > ROM

    /* player_46c00 — decompiled to C (src/player_46c00.c, carved by tools/carve.py) */
    .player_46c00 0x08046C00 : {
        build/src/player_46c00.o(.text)
    } > ROM

    /* player_474e8 — decompiled to C (src/player_474e8.c, carved by tools/carve.py) */
    .player_474e8 0x080474E8 : {
        build/src/player_474e8.o(.text)
    } > ROM

    /* player_47fe8 — decompiled to C (src/player_47fe8.c, carved by tools/carve.py) */
    .player_47fe8 0x08047FE8 : {
        build/src/player_47fe8.o(.text)
    } > ROM

    /* player_49738 — decompiled to C (src/player_49738.c, carved by tools/carve.py) */
    .player_49738 0x08049738 : {
        build/src/player_49738.o(.text)
    } > ROM

    /* player_49b48 — decompiled to C (src/player_49b48.c, carved by tools/carve.py) */
    .player_49b48 0x08049B48 : {
        build/src/player_49b48.o(.text)
    } > ROM

    /* player_49f98 — decompiled to C (src/player_49f98.c, carved by tools/carve.py) */
    .player_49f98 0x08049F98 : {
        build/src/player_49f98.o(.text)
    } > ROM

    /* player_4a54c — decompiled to C (src/player_4a54c.c, carved by tools/carve.py) */
    .player_4a54c 0x0804A54C : {
        build/src/player_4a54c.o(.text)
    } > ROM

    /* player_4ab70 — decompiled to C (src/player_4ab70.c, carved by tools/carve.py) */
    .player_4ab70 0x0804AB70 : {
        build/src/player_4ab70.o(.text)
    } > ROM

    /* player_4b5b4 — decompiled to C (src/player_4b5b4.c, carved by tools/carve.py) */
    .player_4b5b4 0x0804B5B4 : {
        build/src/player_4b5b4.o(.text)
    } > ROM

    /* player_4b858 — decompiled to C (src/player_4b858.c, carved by tools/carve.py) */
    .player_4b858 0x0804B858 : {
        build/src/player_4b858.o(.text)
    } > ROM

    /* player_4c64c — decompiled to C (src/player_4c64c.c, carved by tools/carve.py) */
    .player_4c64c 0x0804C64C : {
        build/src/player_4c64c.o(.text)
    } > ROM

    /* player_4cc7c — decompiled to C (src/player_4cc7c.c, carved by tools/carve.py) */
    .player_4cc7c 0x0804CC7C : {
        build/src/player_4cc7c.o(.text)
    } > ROM

    /* player_4dc08 — decompiled to C (src/player_4dc08.c, carved by tools/carve.py) */
    .player_4dc08 0x0804DC08 : {
        build/src/player_4dc08.o(.text)
    } > ROM

    /* sub_0804e3a0 — decompiled to C (src/sub_0804e3a0.c, carved by tools/carve.py) */
    .sub_0804e3a0 0x0804E3A0 : {
        build/src/sub_0804e3a0.o(.text)
    } > ROM

    /* player_4e5a4 — decompiled to C (src/player_4e5a4.c, carved by tools/carve.py) */
    .player_4e5a4 0x0804E5A4 : {
        build/src/player_4e5a4.o(.text)
    } > ROM

    /* player_4e78c — decompiled to C (src/player_4e78c.c, carved by tools/carve.py) */
    .player_4e78c 0x0804E78C : {
        build/src/player_4e78c.o(.text)
    } > ROM

    /* player_4ee08 — decompiled to C (src/player_4ee08.c, carved by tools/carve.py) */
    .player_4ee08 0x0804EE08 : {
        build/src/player_4ee08.o(.text)
    } > ROM

    /* player_4f614 — decompiled to C (src/player_4f614.c, carved by tools/carve.py) */
    .player_4f614 0x0804F614 : {
        build/src/player_4f614.o(.text)
    } > ROM

    /* player_4f948 — decompiled to C (src/player_4f948.c, carved by tools/carve.py) */
    .player_4f948 0x0804F948 : {
        build/src/player_4f948.o(.text)
    } > ROM

    /* player_4ffdc — decompiled to C (src/player_4ffdc.c, carved by tools/carve.py) */
    .player_4ffdc 0x0804FFDC : {
        build/src/player_4ffdc.o(.text)
    } > ROM

    /* plobj_507bc — decompiled to C (src/plobj_507bc.c, carved by tools/carve.py) */
    .plobj_507bc 0x080507BC : {
        build/src/plobj_507bc.o(.text)
    } > ROM

    /* plobj_509ec — decompiled to C (src/plobj_509ec.c, carved by tools/carve.py) */
    .plobj_509ec 0x080509EC : {
        build/src/plobj_509ec.o(.text)
    } > ROM

    /* plobj_514f8 — decompiled to C (src/plobj_514f8.c, carved by tools/carve.py) */
    .plobj_514f8 0x080514F8 : {
        build/src/plobj_514f8.o(.text)
    } > ROM

    /* plobj_5239c — decompiled to C (src/plobj_5239c.c, carved by tools/carve.py) */
    .plobj_5239c 0x0805239C : {
        build/src/plobj_5239c.o(.text)
    } > ROM

    /* plobj_52f6c — decompiled to C (src/plobj_52f6c.c, carved by tools/carve.py) */
    .plobj_52f6c 0x08052F6C : {
        build/src/plobj_52f6c.o(.text)
    } > ROM

    .game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40_0804335c_080449c8_0804e5a4_08053af4 0x08053AF4 : {
        KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40_0804335c_080449c8_0804e5a4_08053af4)) KEEP(*(.game_code_and_rodata_08018b84_08019eec_0801a3e4_0801a8c8_08040b40_0804335c_080449c8_0804e5a4_08053af4.tail))
    } > ROM

    /* effect_5afac — decompiled to C (src/effect_5afac.c, carved by tools/carve.py) */
    .effect_5afac 0x0805AFAC : {
        build/src/effect_5afac.o(.text)
    } > ROM

    /* actor_62584 — decompiled to C (src/actor_62584.c, carved by tools/carve.py) */
    .actor_62584 0x08062584 : {
        build/src/actor_62584.o(.text)
    } > ROM

    /* actor_63698 — decompiled to C (src/actor_63698.c, carved by tools/carve.py) */
    .actor_63698 0x08063698 : {
        build/src/actor_63698.o(.text)
    } > ROM

    /* actor_653ec — decompiled to C (src/actor_653ec.c, carved by tools/carve.py) */
    .actor_653ec 0x080653EC : {
        build/src/actor_653ec.o(.text)
    } > ROM

    /* actor_673ec — decompiled to C (src/actor_673ec.c, carved by tools/carve.py) */
    .actor_673ec 0x080673EC : {
        build/src/actor_673ec.o(.text)
    } > ROM

    /* actor_692fc — decompiled to C (src/actor_692fc.c, carved by tools/carve.py) */
    .actor_692fc 0x080692FC : {
        build/src/actor_692fc.o(.text)
    } > ROM

    /* actor_6a344 — decompiled to C (src/actor_6a344.c, carved by tools/carve.py) */
    .actor_6a344 0x0806A344 : {
        build/src/actor_6a344.o(.text)
    } > ROM

    /* actor_6ad18 — decompiled to C (src/actor_6ad18.c, carved by tools/carve.py) */
    .actor_6ad18 0x0806AD18 : {
        build/src/actor_6ad18.o(.text)
    } > ROM

    /* actor_6b2e4 — decompiled to C (src/actor_6b2e4.c, carved by tools/carve.py) */
    .actor_6b2e4 0x0806B2E4 : {
        build/src/actor_6b2e4.o(.text)
    } > ROM

    /* actor_6c2a4 — decompiled to C (src/actor_6c2a4.c, carved by tools/carve.py) */
    .actor_6c2a4 0x0806C2A4 : {
        build/src/actor_6c2a4.o(.text)
    } > ROM

    /* actor_6cd40 — decompiled to C (src/actor_6cd40.c, carved by tools/carve.py) */
    .actor_6cd40 0x0806CD40 : {
        build/src/actor_6cd40.o(.text)
    } > ROM

    /* actor_6d22c — decompiled to C (src/actor_6d22c.c, carved by tools/carve.py) */
    .actor_6d22c 0x0806D22C : {
        build/src/actor_6d22c.o(.text)
    } > ROM

    /* actor_6e0f0 — decompiled to C (src/actor_6e0f0.c, carved by tools/carve.py) */
    .actor_6e0f0 0x0806E0F0 : {
        build/src/actor_6e0f0.o(.text)
    } > ROM

    /* actor_6ef5c — decompiled to C (src/actor_6ef5c.c, carved by tools/carve.py) */
    .actor_6ef5c 0x0806EF5C : {
        build/src/actor_6ef5c.o(.text)
    } > ROM

    /* actor_6ff24 — decompiled to C (src/actor_6ff24.c, carved by tools/carve.py) */
    .actor_6ff24 0x0806FF24 : {
        build/src/actor_6ff24.o(.text)
    } > ROM

    /* actor_70ec0 — decompiled to C (src/actor_70ec0.c, carved by tools/carve.py) */
    .actor_70ec0 0x08070EC0 : {
        build/src/actor_70ec0.o(.text)
    } > ROM

    /* actor_72d8c — decompiled to C (src/actor_72d8c.c, carved by tools/carve.py) */
    .actor_72d8c 0x08072D8C : {
        build/src/actor_72d8c.o(.text)
    } > ROM

    /* actor_74c0c — decompiled to C (src/actor_74c0c.c, carved by tools/carve.py) */
    .actor_74c0c 0x08074C0C : {
        build/src/actor_74c0c.o(.text)
    } > ROM

    /* actor_763e8 — decompiled to C (src/actor_763e8.c, carved by tools/carve.py) */
    .actor_763e8 0x080763E8 : {
        build/src/actor_763e8.o(.text)
    } > ROM

    /* actor_77ae0 — decompiled to C (src/actor_77ae0.c, carved by tools/carve.py) */
    .actor_77ae0 0x08077AE0 : {
        build/src/actor_77ae0.o(.text)
    } > ROM

    /* enemy_78b68 — decompiled to C (src/enemy_78b68.c, carved by tools/carve.py) */
    .enemy_78b68 0x08078B68 : {
        build/src/enemy_78b68.o(.text)
    } > ROM

    /* enemy_7aa5c — decompiled to C (src/enemy_7aa5c.c, carved by tools/carve.py) */
    .enemy_7aa5c 0x0807AA5C : {
        build/src/enemy_7aa5c.o(.text)
    } > ROM

    /* enemy_7d3b0 — decompiled to C (src/enemy_7d3b0.c, carved by tools/carve.py) */
    .enemy_7d3b0 0x0807D3B0 : {
        build/src/enemy_7d3b0.o(.text)
    } > ROM

    /* enemy_7f044 — decompiled to C (src/enemy_7f044.c, carved by tools/carve.py) */
    .enemy_7f044 0x0807F044 : {
        build/src/enemy_7f044.o(.text)
    } > ROM

    /* enemy_80b70 — decompiled to C (src/enemy_80b70.c, carved by tools/carve.py) */
    .enemy_80b70 0x08080B70 : {
        build/src/enemy_80b70.o(.text)
    } > ROM

    /* enemy_820b8 — decompiled to C (src/enemy_820b8.c, carved by tools/carve.py) */
    .enemy_820b8 0x080820B8 : {
        build/src/enemy_820b8.o(.text)
    } > ROM

    /* enemy_82e68 — decompiled to C (src/enemy_82e68.c, carved by tools/carve.py) */
    .enemy_82e68 0x08082E68 : {
        build/src/enemy_82e68.o(.text)
    } > ROM

    /* enemy_844c4 — decompiled to C (src/enemy_844c4.c, carved by tools/carve.py) */
    .enemy_844c4 0x080844C4 : {
        build/src/enemy_844c4.o(.text)
    } > ROM

    /* enemy_84d14 — decompiled to C (src/enemy_84d14.c, carved by tools/carve.py) */
    .enemy_84d14 0x08084D14 : {
        build/src/enemy_84d14.o(.text)
    } > ROM

    /* enemy_860f8 — decompiled to C (src/enemy_860f8.c, carved by tools/carve.py) */
    .enemy_860f8 0x080860F8 : {
        build/src/enemy_860f8.o(.text)
    } > ROM

    /* enemy_88000 — decompiled to C (src/enemy_88000.c, carved by tools/carve.py) */
    .enemy_88000 0x08088000 : {
        build/src/enemy_88000.o(.text)
    } > ROM

    /* enemy_8aa68 — decompiled to C (src/enemy_8aa68.c, carved by tools/carve.py) */
    .enemy_8aa68 0x0808AA68 : {
        build/src/enemy_8aa68.o(.text)
    } > ROM

    /* enemy_8cce8 — decompiled to C (src/enemy_8cce8.c, carved by tools/carve.py) */
    .enemy_8cce8 0x0808CCE8 : {
        build/src/enemy_8cce8.o(.text)
    } > ROM

    /* enemy_8e404 — decompiled to C (src/enemy_8e404.c, carved by tools/carve.py) */
    .enemy_8e404 0x0808E404 : {
        build/src/enemy_8e404.o(.text)
    } > ROM

    /* enemy_8f41c — decompiled to C (src/enemy_8f41c.c, carved by tools/carve.py) */
    .enemy_8f41c 0x0808F41C : {
        build/src/enemy_8f41c.o(.text)
    } > ROM

    /* enemy_9000c — decompiled to C (src/enemy_9000c.c, carved by tools/carve.py) */
    .enemy_9000c 0x0809000C : {
        build/src/enemy_9000c.o(.text)
    } > ROM

    /* enemy_9113c — decompiled to C (src/enemy_9113c.c, carved by tools/carve.py) */
    .enemy_9113c 0x0809113C : {
        build/src/enemy_9113c.o(.text)
    } > ROM

    /* enemy_91f9c — decompiled to C (src/enemy_91f9c.c, carved by tools/carve.py) */
    .enemy_91f9c 0x08091F9C : {
        build/src/enemy_91f9c.o(.text)
    } > ROM

    /* enemy_93f64 — decompiled to C (src/enemy_93f64.c, carved by tools/carve.py) */
    .enemy_93f64 0x08093F64 : {
        build/src/enemy_93f64.o(.text)
    } > ROM

    /* enemy_957bc — decompiled to C (src/enemy_957bc.c, carved by tools/carve.py) */
    .enemy_957bc 0x080957BC : {
        build/src/enemy_957bc.o(.text)
    } > ROM

    /* enemy_970c4 — decompiled to C (src/enemy_970c4.c, carved by tools/carve.py) */
    .enemy_970c4 0x080970C4 : {
        build/src/enemy_970c4.o(.text)
    } > ROM

    /* enemy_974c8 — decompiled to C (src/enemy_974c8.c, carved by tools/carve.py) */
    .enemy_974c8 0x080974C8 : {
        build/src/enemy_974c8.o(.text)
    } > ROM

    /* enemy_988f8 — decompiled to C (src/enemy_988f8.c, carved by tools/carve.py) */
    .enemy_988f8 0x080988F8 : {
        build/src/enemy_988f8.o(.text)
    } > ROM

    /* enemy_99b20 — decompiled to C (src/enemy_99b20.c, carved by tools/carve.py) */
    .enemy_99b20 0x08099B20 : {
        build/src/enemy_99b20.o(.text)
    } > ROM

    /* enemy_9ba44 — decompiled to C (src/enemy_9ba44.c, carved by tools/carve.py) */
    .enemy_9ba44 0x0809BA44 : {
        build/src/enemy_9ba44.o(.text)
    } > ROM

    /* enemy_9c028 — decompiled to C (src/enemy_9c028.c, carved by tools/carve.py) */
    .enemy_9c028 0x0809C028 : {
        build/src/enemy_9c028.o(.text)
    } > ROM

    /* enemy_9c0a8 — decompiled to C (src/enemy_9c0a8.c, carved by tools/carve.py) */
    .enemy_9c0a8 0x0809C0A8 : {
        build/src/enemy_9c0a8.o(.text)
    } > ROM

    /* enemy_9cb90 — decompiled to C (src/enemy_9cb90.c, carved by tools/carve.py) */
    .enemy_9cb90 0x0809CB90 : {
        build/src/enemy_9cb90.o(.text)
    } > ROM

    /* enemy_9cc24 — decompiled to C (src/enemy_9cc24.c, carved by tools/carve.py) */
    .enemy_9cc24 0x0809CC24 : {
        build/src/enemy_9cc24.o(.text)
    } > ROM

    /* enemy_9d994 — decompiled to C (src/enemy_9d994.c, carved by tools/carve.py) */
    .enemy_9d994 0x0809D994 : {
        build/src/enemy_9d994.o(.text)
    } > ROM

    /* enemy_9da1c — decompiled to C (src/enemy_9da1c.c, carved by tools/carve.py) */
    .enemy_9da1c 0x0809DA1C : {
        build/src/enemy_9da1c.o(.text)
    } > ROM

    /* enemy_9f2f4 — decompiled to C (src/enemy_9f2f4.c, carved by tools/carve.py) */
    .enemy_9f2f4 0x0809F2F4 : {
        build/src/enemy_9f2f4.o(.text)
    } > ROM

    /* enemy_9f37c — decompiled to C (src/enemy_9f37c.c, carved by tools/carve.py) */
    .enemy_9f37c 0x0809F37C : {
        build/src/enemy_9f37c.o(.text)
    } > ROM

    /* enemy_9f9dc — decompiled to C (src/enemy_9f9dc.c, carved by tools/carve.py) */
    .enemy_9f9dc 0x0809F9DC : {
        build/src/enemy_9f9dc.o(.text)
    } > ROM

    /* enemy_9fbd0 — decompiled to C (src/enemy_9fbd0.c, carved by tools/carve.py) */
    .enemy_9fbd0 0x0809FBD0 : {
        build/src/enemy_9fbd0.o(.text)
    } > ROM

    /* enemy_a00ec — decompiled to C (src/enemy_a00ec.c, carved by tools/carve.py) */
    .enemy_a00ec 0x080A00EC : {
        build/src/enemy_a00ec.o(.text)
    } > ROM

    /* enemy_a0274 — decompiled to C (src/enemy_a0274.c, carved by tools/carve.py) */
    .enemy_a0274 0x080A0274 : {
        build/src/enemy_a0274.o(.text)
    } > ROM

    /* enemy_a1590 — decompiled to C (src/enemy_a1590.c, carved by tools/carve.py) */
    .enemy_a1590 0x080A1590 : {
        build/src/enemy_a1590.o(.text)
    } > ROM

    /* enemy_a5644 — decompiled to C (src/enemy_a5644.c, carved by tools/carve.py) */
    .enemy_a5644 0x080A5644 : {
        build/src/enemy_a5644.o(.text)
    } > ROM

    /* enemy_a78a0 — decompiled to C (src/enemy_a78a0.c, carved by tools/carve.py) */
    .enemy_a78a0 0x080A78A0 : {
        build/src/enemy_a78a0.o(.text)
    } > ROM

    /* enemy_a7998 — decompiled to C (src/enemy_a7998.c, carved by tools/carve.py) */
    .enemy_a7998 0x080A7998 : {
        build/src/enemy_a7998.o(.text)
    } > ROM

    /* enemy_a860c — decompiled to C (src/enemy_a860c.c, carved by tools/carve.py) */
    .enemy_a860c 0x080A860C : {
        build/src/enemy_a860c.o(.text)
    } > ROM

    /* enemy_a87c8 — decompiled to C (src/enemy_a87c8.c, carved by tools/carve.py) */
    .enemy_a87c8 0x080A87C8 : {
        build/src/enemy_a87c8.o(.text)
    } > ROM

    /* enemy_a932c — decompiled to C (src/enemy_a932c.c, carved by tools/carve.py) */
    .enemy_a932c 0x080A932C : {
        build/src/enemy_a932c.o(.text)
    } > ROM

    /* enemy_a93ec — decompiled to C (src/enemy_a93ec.c, carved by tools/carve.py) */
    .enemy_a93ec 0x080A93EC : {
        build/src/enemy_a93ec.o(.text)
    } > ROM

    /* enemy_aa338 — decompiled to C (src/enemy_aa338.c, carved by tools/carve.py) */
    .enemy_aa338 0x080AA338 : {
        build/src/enemy_aa338.o(.text)
    } > ROM

    /* enemy_ae3bc — decompiled to C (src/enemy_ae3bc.c, carved by tools/carve.py) */
    .enemy_ae3bc 0x080AE3BC : {
        build/src/enemy_ae3bc.o(.text)
    } > ROM

    /* hud_b2fe8 — decompiled to C (src/hud_b2fe8.c, carved by tools/carve.py) */
    .hud_b2fe8 0x080B2FE8 : {
        build/src/hud_b2fe8.o(.text)
    } > ROM

    /* hud_b4ea8 — decompiled to C (src/hud_b4ea8.c, carved by tools/carve.py) */
    .hud_b4ea8 0x080B4EA8 : {
        build/src/hud_b4ea8.o(.text)
    } > ROM

    /* hud_b5024 — decompiled to C (src/hud_b5024.c, carved by tools/carve.py) */
    .hud_b5024 0x080B5024 : {
        build/src/hud_b5024.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670 0x080B5670 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670.tail))
    } > ROM

    /* hud_b5840 — decompiled to C (src/hud_b5840.c, carved by tools/carve.py) */
    .hud_b5840 0x080B5840 : {
        build/src/hud_b5840.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154 0x080B6154 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154.tail))
    } > ROM

    /* save_b6290 — decompiled to C (src/save_b6290.c, carved by tools/carve.py) */
    .save_b6290 0x080B6290 : {
        build/src/save_b6290.o(.text)
    } > ROM

    /* save_b63a4 — decompiled to C (src/save_b63a4.c, carved by tools/carve.py) */
    .save_b63a4 0x080B63A4 : {
        build/src/save_b63a4.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6474 0x080B6474 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6474)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6474.tail))
    } > ROM

    /* save_b6a90 — decompiled to C (src/save_b6a90.c, carved by tools/carve.py) */
    .save_b6a90 0x080B6A90 : {
        build/src/save_b6a90.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08 0x080B6B08 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08.tail))
    } > ROM

    /* save_b6c40 — decompiled to C (src/save_b6c40.c, carved by tools/carve.py) */
    .save_b6c40 0x080B6C40 : {
        build/src/save_b6c40.o(.text)
    } > ROM

    /* save_b6d04 — decompiled to C (src/save_b6d04.c, carved by tools/carve.py) */
    .save_b6d04 0x080B6D04 : {
        build/src/save_b6d04.o(.text)
    } > ROM

    /* save_b6e44 — decompiled to C (src/save_b6e44.c, carved by tools/carve.py) */
    .save_b6e44 0x080B6E44 : {
        build/src/save_b6e44.o(.text)
    } > ROM

    /* save_b6f38 — decompiled to C (src/save_b6f38.c, carved by tools/carve.py) */
    .save_b6f38 0x080B6F38 : {
        build/src/save_b6f38.o(.text)
    } > ROM

    /* save_b72bc — decompiled to C (src/save_b72bc.c, carved by tools/carve.py) */
    .save_b72bc 0x080B72BC : {
        build/src/save_b72bc.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08_080b6f38_080b75a4 0x080B75A4 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08_080b6f38_080b75a4)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08_080b6f38_080b75a4.tail))
    } > ROM

    /* save_b77d4 — decompiled to C (src/save_b77d4.c, carved by tools/carve.py) */
    .save_b77d4 0x080B77D4 : {
        build/src/save_b77d4.o(.text)
    } > ROM

    /* save_b79b8 — decompiled to C (src/save_b79b8.c, carved by tools/carve.py) */
    .save_b79b8 0x080B79B8 : {
        build/src/save_b79b8.o(.text)
    } > ROM

    /* save_b7a9c — decompiled to C (src/save_b7a9c.c, carved by tools/carve.py) */
    .save_b7a9c 0x080B7A9C : {
        build/src/save_b7a9c.o(.text)
    } > ROM

    /* save_b7df4 — decompiled to C (src/save_b7df4.c, carved by tools/carve.py) */
    .save_b7df4 0x080B7DF4 : {
        build/src/save_b7df4.o(.text)
    } > ROM

    /* save_b7e14 — decompiled to C (src/save_b7e14.c, carved by tools/carve.py) */
    .save_b7e14 0x080B7E14 : {
        build/src/save_b7e14.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08_080b6f38_080b79b8_080b7df4_080b8694 0x080B8694 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08_080b6f38_080b79b8_080b7df4_080b8694)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_0809113c_08093f64_08099b20_0809cb90_0809d994_0809f2f4_0809f9dc_080a00ec_080a1590_080b2fe8_080b5670_080b6154_080b6b08_080b6f38_080b79b8_080b7df4_080b8694.tail))
    } > ROM

    /* save_b8888 — decompiled to C (src/save_b8888.c, carved by tools/carve.py) */
    .save_b8888 0x080B8888 : {
        build/src/save_b8888.o(.text)
    } > ROM

    /* save_b8918 — decompiled to C (src/save_b8918.c, carved by tools/carve.py) */
    .save_b8918 0x080B8918 : {
        build/src/save_b8918.o(.text)
    } > ROM

    /* save_b8ea0 — decompiled to C (src/save_b8ea0.c, carved by tools/carve.py) */
    .save_b8ea0 0x080B8EA0 : {
        build/src/save_b8ea0.o(.text)
    } > ROM

    /* subgame_b9d0c — decompiled to C (src/subgame_b9d0c.c, carved by tools/carve.py) */
    .subgame_b9d0c 0x080B9D0C : {
        build/src/subgame_b9d0c.o(.text)
    } > ROM

    /* subgame_ba774 — decompiled to C (src/subgame_ba774.c, carved by tools/carve.py) */
    .subgame_ba774 0x080BA774 : {
        build/src/subgame_ba774.o(.text)
    } > ROM

    /* subgame_bb528 — decompiled to C (src/subgame_bb528.c, carved by tools/carve.py) */
    .subgame_bb528 0x080BB528 : {
        build/src/subgame_bb528.o(.text)
    } > ROM

    /* subgame_bc0cc — decompiled to C (src/subgame_bc0cc.c, carved by tools/carve.py) */
    .subgame_bc0cc 0x080BC0CC : {
        build/src/subgame_bc0cc.o(.text)
    } > ROM

    /* subgame_bd9e8 — decompiled to C (src/subgame_bd9e8.c, carved by tools/carve.py) */
    .subgame_bd9e8 0x080BD9E8 : {
        build/src/subgame_bd9e8.o(.text)
    } > ROM

    /* subgame_bda2c — decompiled to C (src/subgame_bda2c.c, carved by tools/carve.py) */
    .subgame_bda2c 0x080BDA2C : {
        build/src/subgame_bda2c.o(.text)
    } > ROM

    /* subgame_bf994 — decompiled to C (src/subgame_bf994.c, carved by tools/carve.py) */
    .subgame_bf994 0x080BF994 : {
        build/src/subgame_bf994.o(.text)
    } > ROM

    /* subgame_c0de8 — decompiled to C (src/subgame_c0de8.c, carved by tools/carve.py) */
    .subgame_c0de8 0x080C0DE8 : {
        build/src/subgame_c0de8.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_080bf994_080c1ffc 0x080C1FFC : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_080bf994_080c1ffc)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_080bf994_080c1ffc.tail))
    } > ROM

    /* sub_080c6258 — decompiled to C (src/sub_080c6258.c, carved by tools/carve.py) */
    .sub_080c6258 0x080C6258 : {
        build/src/sub_080c6258.o(.text)
    } > ROM

    .game_code_and_rodata_080653ec_0806ef5c_080bf994_080c1ffc_080c6260 0x080C6260 : {
        KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_080bf994_080c1ffc_080c6260)) KEEP(*(.game_code_and_rodata_080653ec_0806ef5c_080bf994_080c1ffc_080c6260.tail))
    } > ROM

    /* m4a sound engine: asm core (pret m4a_1.s equivalent; hand-scheduled
     * SDK asm, stays asm forever — issue #52). SoundMainRAM's ROM image
     * embeds an ARM-mode mixer loop (see rom-map section 8.2). */
    .m4a_1 0x080CD89C : {
        KEEP(*(.m4a_1)) KEEP(*(.m4a_1.tail))
    } > ROM

    /* m4a sound engine: C driver (pret m4a.c equivalent; to be decompiled
     * in issues #53-#55) */
    /* m4a_c1 — decompiled to C (src/m4a_c1.c, carved by tools/carve.py) */
    .m4a_c1 0x080CE520 : {
        build/src/m4a_c1.o(.text)
    } > ROM

    /* m4a_cgb — decompiled to C (src/m4a_cgb.c, carved by tools/carve.py) */
    .m4a_cgb 0x080CEFB4 : {
        build/src/m4a_cgb.o(.text)
    } > ROM

    /* m4a_ctrl — decompiled to C (src/m4a_ctrl.c, carved by tools/carve.py) */
    .m4a_ctrl 0x080CF588 : {
        build/src/m4a_ctrl.o(.text)
    } > ROM

    /* SDK syscall wrappers (Thumb) */
    .sdk_swi_wrappers 0x080CFA4C : {
        KEEP(*(.sdk_swi_wrappers)) KEEP(*(.sdk_swi_wrappers.tail))
    } > ROM

    /* SDK swi reset helper tail + literals */
    .sdk_reset_helper 0x080CFA7F : {
        KEEP(*(.sdk_reset_helper)) KEEP(*(.sdk_reset_helper.tail))
    } > ROM

    /* SDK SRAM driver (libagbsrd, "SRAM_V112") — decompiled to C
     * (src/agb_sram.c, compiled with old_agbcc). */
    .sram_driver 0x080CFA9C : {
        build/src/agb_sram.o(.text)
    } > ROM

    /* C library: call_via_rN, copy loops, division routines (verbatim) */
    .sdk_libc 0x080CFC30 : {
        KEEP(*(.sdk_libc)) KEEP(*(.sdk_libc.tail))
    } > ROM

    /* Interworking veneer (ARM) */
    .interworking_veneer 0x080CFDDC : {
        KEEP(*(.interworking_veneer)) KEEP(*(.interworking_veneer.tail))
    } > ROM

    /* Alignment padding between veneer and IRQ table */
    .gap_interworking_veneer_irq_handler_table_14 0x080CFDE4 : {
        KEEP(*(.gap_interworking_veneer_irq_handler_table_14)) KEEP(*(.gap_interworking_veneer_irq_handler_table_14.tail))
    } > ROM

    /* 14-entry IRQ handler table */
    .irq_handler_table_14 0x080CFDE8 : {
        KEEP(*(.irq_handler_table_14)) KEEP(*(.irq_handler_table_14.tail))
    } > ROM

    /* Miscellaneous library data */
    .lib_misc 0x080CFE20 : {
        KEEP(*(.lib_misc)) KEEP(*(.lib_misc.tail))
    } > ROM

    /* Library rodata / FIR-style coefficient tables (m4a) */
    .lib_rodata_fir_tables 0x080CFF00 : {
        KEEP(*(.lib_rodata_fir_tables)) KEEP(*(.lib_rodata_fir_tables.tail))
    } > ROM

    /* Level/map & object tables */
    .level_object_tables 0x080D0000 : {
        KEEP(*(.level_object_tables)) KEEP(*(.level_object_tables.tail))
    } > ROM

    /* Level data / uncompressed graphics / palettes */
    .level_graphics_palettes 0x08120000 : {
        KEEP(*(.level_graphics_palettes)) KEEP(*(.level_graphics_palettes.tail))
    } > ROM

    /* Sound sample data (PCM), block 1 */
    .sound_samples_1 0x083356E0 : {
        KEEP(*(.sound_samples_1)) KEEP(*(.sound_samples_1.tail))
    } > ROM

    /* Interleaved level graphics/palettes, block 2 */
    .level_graphics_palettes_2 0x0834EEE8 : {
        KEEP(*(.level_graphics_palettes_2)) KEEP(*(.level_graphics_palettes_2.tail))
    } > ROM

    /* Sound sample data (PCM), block 2 */
    .sound_samples_2 0x08350AF8 : {
        KEEP(*(.sound_samples_2)) KEEP(*(.sound_samples_2.tail))
    } > ROM

    /* Interleaved level graphics/palettes, block 3 */
    .level_graphics_palettes_3 0x083A85D4 : {
        KEEP(*(.level_graphics_palettes_3)) KEEP(*(.level_graphics_palettes_3.tail))
    } > ROM

    /* Compressed graphics (LZ77/RLE-class) */
    .compressed_graphics 0x083D0000 : {
        KEEP(*(.compressed_graphics)) KEEP(*(.compressed_graphics.tail))
    } > ROM

    /* m4a songs / sequences (head: song track data before the engine tables) */
    .m4a_songs 0x085C0000 : {
        KEEP(*(.m4a_songs)) KEEP(*(.m4a_songs.tail))
    } > ROM

    /* m4a engine rodata tables (issue #51): gMPlayJumpTableTemplate..gXcmdTable */
    .m4a_engine_rodata 0x0860A140 : {
        KEEP(*(.m4a_engine_rodata)) KEEP(*(.m4a_engine_rodata.tail))
    } > ROM

    /* Song track data between the engine tables and the song table */
    .m4a_song_tracks 0x0860A418 : {
        KEEP(*(.m4a_song_tracks)) KEEP(*(.m4a_song_tracks.tail))
    } > ROM

    /* gMPlayTable + gSongTable (issue #51) */
    .m4a_song_table 0x0860B430 : {
        KEEP(*(.m4a_song_table)) KEEP(*(.m4a_song_table.tail))
    } > ROM

    /* m4a songs / sequences (tail: song headers, tone data, track data) */
    .m4a_songs_2 0x0860C678 : {
        KEEP(*(.m4a_songs_2)) KEEP(*(.m4a_songs_2.tail))
    } > ROM

    /* SRAM_V112 identifier string */
    .sram_v112_string 0x0872E9F8 : {
        KEEP(*(.sram_v112_string)) KEEP(*(.sram_v112_string.tail))
    } > ROM

    /* Null padding after SRAM string */
    .gap_sram_v112_string_sram_driver_fn_table 0x0872EA02 : {
        KEEP(*(.gap_sram_v112_string_sram_driver_fn_table)) KEEP(*(.gap_sram_v112_string_sram_driver_fn_table.tail))
    } > ROM

    /* SRAM driver function table (4 Thumb pointers) */
    .sram_driver_fn_table 0x0872EA04 : {
        KEEP(*(.sram_driver_fn_table)) KEEP(*(.sram_driver_fn_table.tail))
    } > ROM

    /* Data between SRAM table and asset metadata */
    .gap_sram_driver_fn_table_asset_metadata_index 0x0872EA14 : {
        KEEP(*(.gap_sram_driver_fn_table_asset_metadata_index)) KEEP(*(.gap_sram_driver_fn_table_asset_metadata_index.tail))
    } > ROM

    /* Asset metadata / index zone */
    .asset_metadata_index 0x08730000 : {
        KEEP(*(.asset_metadata_index)) KEEP(*(.asset_metadata_index.tail))
    } > ROM

    /* Song tail / misc audio data */
    .song_tail_misc_audio 0x08760000 : {
        KEEP(*(.song_tail_misc_audio)) KEEP(*(.song_tail_misc_audio.tail))
    } > ROM

    /* End-of-ROM index (sample sets) */
    .sample_set_index 0x087E1D58 : {
        KEEP(*(.sample_set_index)) KEEP(*(.sample_set_index.tail))
    } > ROM

    /* Zero padding to ROM end */
    .zero_padding 0x087E3088 : {
        KEEP(*(.zero_padding)) KEEP(*(.zero_padding.tail))
    } > ROM

    /DISCARD/ : {
        *(*)
    }
}
