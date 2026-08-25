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

    .game_code_and_rodata 0x080075B8 : {
        KEEP(*(.game_code_and_rodata)) KEEP(*(.game_code_and_rodata.tail))
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
