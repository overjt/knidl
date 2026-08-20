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

    /* Remainder of the ROM, extracted verbatim from baserom.gba.
     * Code/data sections will be carved out of this blob incrementally
     * as decompilation progresses. */
    .main 0x080000C0 : {
        KEEP(*(.main))
    } > ROM

    /DISCARD/ : {
        *(*)
    }
}
