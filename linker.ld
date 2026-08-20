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
    .crt0_master_isr 0x080000C0 : {
        KEEP(*(.crt0_master_isr))
    } > ROM
    .crt0_literals 0x08000210 : {
        KEEP(*(.crt0_literals))
    } > ROM
    .task_switch_helpers 0x08000234 : {
        KEEP(*(.task_switch_helpers))
    } > ROM
    .task_literals 0x080002E5 : {
        KEEP(*(.task_literals))
    } > ROM
    .agb_init 0x08000310 : {
        KEEP(*(.agb_init))
    } > ROM
    .game_code_early 0x080006FF : {
        KEEP(*(.game_code_early))
    } > ROM
    .game_code_and_rodata 0x08007300 : {
        KEEP(*(.game_code_and_rodata))
    } > ROM
    .sdk_swi_wrappers 0x080CFA40 : {
        KEEP(*(.sdk_swi_wrappers))
    } > ROM
    .sdk_lib_sram_driver 0x080CFA7F : {
        KEEP(*(.sdk_lib_sram_driver))
    } > ROM
    .interworking_veneer 0x080CFDDC : {
        KEEP(*(.interworking_veneer))
    } > ROM
    .irq_handler_table_14 0x080CFDE8 : {
        KEEP(*(.irq_handler_table_14))
    } > ROM
    .lib_misc 0x080CFE20 : {
        KEEP(*(.lib_misc))
    } > ROM
    .lib_rodata_fir_tables 0x080CFF00 : {
        KEEP(*(.lib_rodata_fir_tables))
    } > ROM
    .level_object_tables 0x080D0000 : {
        KEEP(*(.level_object_tables))
    } > ROM
    .level_graphics_palettes 0x08120000 : {
        KEEP(*(.level_graphics_palettes))
    } > ROM
    .sound_samples_1 0x083356E0 : {
        KEEP(*(.sound_samples_1))
    } > ROM
    .level_graphics_palettes_2 0x0834EEE8 : {
        KEEP(*(.level_graphics_palettes_2))
    } > ROM
    .sound_samples_2 0x08350AF8 : {
        KEEP(*(.sound_samples_2))
    } > ROM
    .level_graphics_palettes_3 0x083A85D4 : {
        KEEP(*(.level_graphics_palettes_3))
    } > ROM
    .compressed_graphics 0x083D0000 : {
        KEEP(*(.compressed_graphics))
    } > ROM
    .m4a_songs 0x085C0000 : {
        KEEP(*(.m4a_songs))
    } > ROM
    .sram_v112_string 0x0872E9F8 : {
        KEEP(*(.sram_v112_string))
    } > ROM
    .sram_driver_fn_table 0x0872EA04 : {
        KEEP(*(.sram_driver_fn_table))
    } > ROM
    .asset_metadata_index 0x08730000 : {
        KEEP(*(.asset_metadata_index))
    } > ROM
    .song_tail_misc_audio 0x08760000 : {
        KEEP(*(.song_tail_misc_audio))
    } > ROM
    .sample_set_index 0x087E1D58 : {
        KEEP(*(.sample_set_index))
    } > ROM
    .zero_padding 0x087E3088 : {
        KEEP(*(.zero_padding))
    } > ROM

    /DISCARD/ : {
        *(*)
    }
}
