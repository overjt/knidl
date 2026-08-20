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
        KEEP(*(.rom_header))
    } > ROM

    /* ===== Segments split from the former main_blob =====
     * Each section is pinned at its exact ROM VMA; boundaries match
     * docs/analysis/rom-map.md and docs/analysis/segments.txt.
     * As decompilation progresses, individual .incbin sections will be
     * replaced with compiled/assembled object files. */

    /* crt0 + master ISR (ARM code) — assembled from asm/crt0.s */
    .crt0_master_isr 0x080000C0 : {
        KEEP(*(.crt0_master_isr))
    } > ROM

    /* crt0/ISR literal pool — assembled from asm/crt0.s */
    .crt0_literals 0x08000210 : {
        KEEP(*(.crt0_literals))
    } > ROM

    /* Task/context-switch helpers (ARM code) */
    .task_switch_helpers 0x08000234 : {
        KEEP(*(.task_switch_helpers))
    } > ROM

    /* Task helper literal pool */
    .task_literals 0x080002E5 : {
        KEEP(*(.task_literals))
    } > ROM

    /* AgbInit (Thumb) */
    .agb_init 0x08000310 : {
        KEEP(*(.agb_init))
    } > ROM

    /* Game code: early subsystems (Thumb) */
    .game_code_early 0x080006FF : {
        KEEP(*(.game_code_early))
    } > ROM

    /* Game code + rodata (Thumb) — bulk of the game logic */
    .game_code_and_rodata 0x08007300 : {
        KEEP(*(.game_code_and_rodata))
    } > ROM

    /* SDK syscall wrappers (Thumb) */
    .sdk_swi_wrappers 0x080CFA40 : {
        KEEP(*(.sdk_swi_wrappers))
    } > ROM

    /* C library + SRAM driver (Thumb) */
    .sdk_lib_sram_driver 0x080CFA7F : {
        KEEP(*(.sdk_lib_sram_driver))
    } > ROM

    /* Interworking veneer (ARM) */
    .interworking_veneer 0x080CFDDC : {
        KEEP(*(.interworking_veneer))
    } > ROM

    /* Alignment padding between veneer and IRQ table */
    .gap_interworking_veneer_irq_handler_table_14 0x080CFDE4 : {
        KEEP(*(.gap_interworking_veneer_irq_handler_table_14))
    } > ROM

    /* 14-entry IRQ handler table */
    .irq_handler_table_14 0x080CFDE8 : {
        KEEP(*(.irq_handler_table_14))
    } > ROM

    /* Miscellaneous library data */
    .lib_misc 0x080CFE20 : {
        KEEP(*(.lib_misc))
    } > ROM

    /* Library rodata / FIR-style coefficient tables (m4a) */
    .lib_rodata_fir_tables 0x080CFF00 : {
        KEEP(*(.lib_rodata_fir_tables))
    } > ROM

    /* Level/map & object tables */
    .level_object_tables 0x080D0000 : {
        KEEP(*(.level_object_tables))
    } > ROM

    /* Level data / uncompressed graphics / palettes */
    .level_graphics_palettes 0x08120000 : {
        KEEP(*(.level_graphics_palettes))
    } > ROM

    /* Sound sample data (PCM), block 1 */
    .sound_samples_1 0x083356E0 : {
        KEEP(*(.sound_samples_1))
    } > ROM

    /* Interleaved level graphics/palettes, block 2 */
    .level_graphics_palettes_2 0x0834EEE8 : {
        KEEP(*(.level_graphics_palettes_2))
    } > ROM

    /* Sound sample data (PCM), block 2 */
    .sound_samples_2 0x08350AF8 : {
        KEEP(*(.sound_samples_2))
    } > ROM

    /* Interleaved level graphics/palettes, block 3 */
    .level_graphics_palettes_3 0x083A85D4 : {
        KEEP(*(.level_graphics_palettes_3))
    } > ROM

    /* Compressed graphics (LZ77/RLE-class) */
    .compressed_graphics 0x083D0000 : {
        KEEP(*(.compressed_graphics))
    } > ROM

    /* m4a songs / sequences */
    .m4a_songs 0x085C0000 : {
        KEEP(*(.m4a_songs))
    } > ROM

    /* SRAM_V112 identifier string */
    .sram_v112_string 0x0872E9F8 : {
        KEEP(*(.sram_v112_string))
    } > ROM

    /* Null padding after SRAM string */
    .gap_sram_v112_string_sram_driver_fn_table 0x0872EA02 : {
        KEEP(*(.gap_sram_v112_string_sram_driver_fn_table))
    } > ROM

    /* SRAM driver function table (4 Thumb pointers) */
    .sram_driver_fn_table 0x0872EA04 : {
        KEEP(*(.sram_driver_fn_table))
    } > ROM

    /* Data between SRAM table and asset metadata */
    .gap_sram_driver_fn_table_asset_metadata_index 0x0872EA14 : {
        KEEP(*(.gap_sram_driver_fn_table_asset_metadata_index))
    } > ROM

    /* Asset metadata / index zone */
    .asset_metadata_index 0x08730000 : {
        KEEP(*(.asset_metadata_index))
    } > ROM

    /* Song tail / misc audio data */
    .song_tail_misc_audio 0x08760000 : {
        KEEP(*(.song_tail_misc_audio))
    } > ROM

    /* End-of-ROM index (sample sets) */
    .sample_set_index 0x087E1D58 : {
        KEEP(*(.sample_set_index))
    } > ROM

    /* Zero padding to ROM end */
    .zero_padding 0x087E3088 : {
        KEEP(*(.zero_padding))
    } > ROM

    /DISCARD/ : {
        *(*)
    }
}
