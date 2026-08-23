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
    .game_code_early 0x080008E8 : {
        KEEP(*(.game_code_early)) KEEP(*(.game_code_early.tail))
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

    .m4a 0x080CEFB4 : {
        KEEP(*(.m4a)) KEEP(*(.m4a.tail))
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
