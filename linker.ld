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

    .segment_01_crt0_master_isr 0x080000C0 : {
        KEEP(*(.segment_01_crt0_master_isr))
    } > ROM

    .segment_02_crt0_literals 0x08000210 : {
        KEEP(*(.segment_02_crt0_literals))
    } > ROM

    .segment_03_task_switch_helpers 0x08000234 : {
        KEEP(*(.segment_03_task_switch_helpers))
    } > ROM

    .segment_04_task_literals 0x080002E5 : {
        KEEP(*(.segment_04_task_literals))
    } > ROM

    .segment_05_agb_init 0x08000310 : {
        KEEP(*(.segment_05_agb_init))
    } > ROM

    .segment_06_game_code_early 0x080006FF : {
        KEEP(*(.segment_06_game_code_early))
    } > ROM

    .segment_07_game_code_and_rodata 0x08007300 : {
        KEEP(*(.segment_07_game_code_and_rodata))
    } > ROM

    .segment_08_sdk_swi_wrappers 0x080CFA40 : {
        KEEP(*(.segment_08_sdk_swi_wrappers))
    } > ROM

    .segment_09_sdk_lib_sram_driver 0x080CFA7F : {
        KEEP(*(.segment_09_sdk_lib_sram_driver))
    } > ROM

    .segment_10_interworking_veneer 0x080CFDDC : {
        KEEP(*(.segment_10_interworking_veneer))
    } > ROM

    .segment_11_gap_080cfde4_080cfde8 0x080CFDE4 : {
        KEEP(*(.segment_11_gap_080cfde4_080cfde8))
    } > ROM

    .segment_12_irq_handler_table_14 0x080CFDE8 : {
        KEEP(*(.segment_12_irq_handler_table_14))
    } > ROM

    .segment_13_lib_misc 0x080CFE20 : {
        KEEP(*(.segment_13_lib_misc))
    } > ROM

    .segment_14_lib_rodata_fir_tables 0x080CFF00 : {
        KEEP(*(.segment_14_lib_rodata_fir_tables))
    } > ROM

    .segment_15_level_object_tables 0x080D0000 : {
        KEEP(*(.segment_15_level_object_tables))
    } > ROM

    .segment_16_level_graphics_palettes 0x08120000 : {
        KEEP(*(.segment_16_level_graphics_palettes))
    } > ROM

    .segment_17_sound_samples_1 0x083356E0 : {
        KEEP(*(.segment_17_sound_samples_1))
    } > ROM

    .segment_18_level_graphics_palettes_2 0x0834EEE8 : {
        KEEP(*(.segment_18_level_graphics_palettes_2))
    } > ROM

    .segment_19_sound_samples_2 0x08350AF8 : {
        KEEP(*(.segment_19_sound_samples_2))
    } > ROM

    .segment_20_level_graphics_palettes_3 0x083A85D4 : {
        KEEP(*(.segment_20_level_graphics_palettes_3))
    } > ROM

    .segment_21_compressed_graphics 0x083D0000 : {
        KEEP(*(.segment_21_compressed_graphics))
    } > ROM

    .segment_22_m4a_songs 0x085C0000 : {
        KEEP(*(.segment_22_m4a_songs))
    } > ROM

    .segment_23_sram_v112_string 0x0872E9F8 : {
        KEEP(*(.segment_23_sram_v112_string))
    } > ROM

    .segment_24_gap_0872ea02_0872ea04 0x0872EA02 : {
        KEEP(*(.segment_24_gap_0872ea02_0872ea04))
    } > ROM

    .segment_25_sram_driver_fn_table 0x0872EA04 : {
        KEEP(*(.segment_25_sram_driver_fn_table))
    } > ROM

    .segment_26_gap_0872ea14_08730000 0x0872EA14 : {
        KEEP(*(.segment_26_gap_0872ea14_08730000))
    } > ROM

    .segment_27_asset_metadata_index 0x08730000 : {
        KEEP(*(.segment_27_asset_metadata_index))
    } > ROM

    .segment_28_song_tail_misc_audio 0x08760000 : {
        KEEP(*(.segment_28_song_tail_misc_audio))
    } > ROM

    .segment_29_sample_set_index 0x087E1D58 : {
        KEEP(*(.segment_29_sample_set_index))
    } > ROM

    .segment_30_zero_padding 0x087E3088 : {
        KEEP(*(.segment_30_zero_padding))
    } > ROM

    /DISCARD/ : {
        *(*)
    }
}
