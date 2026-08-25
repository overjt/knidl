#include "gba/gba.h"
#include "global.h"

/* Early subsystem helpers (0x080011AC-0x08001517, issue #32 batch A2):
 * per-frame OAM/palette DMA-less flush, key input polling, display I/O
 * register flush from IWRAM shadows, the VBlank copy-request queue pump,
 * and the HBlank/VCount IRQ handler install/remove helpers.
 *
 * All shadow cells are the same gUnk_* IWRAM cells AgbInit initializes
 * (src/agb_init.c); see that file for the volatile/chained-store idioms.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork):
 *  - gUnk_030004B0 is the 7-word IRQ dispatch table AgbInit copies from
 *    0x080CFDE8 (CpuSet dest 0x030004B0).  It must be referenced as an
 *    extern array: indexing a cast constant base folds the offset into the
 *    pool word (0x030004C0 instead of 0x030004B0 + "str rX,[rY,#16]").
 *  - The BG scroll shadows (gUnk_03000010 etc.) are volatile s32: without
 *    volatile, `>> 16` narrows into an `ldrsh [rX, #2]` of the upper half;
 *    the ROM reads the whole word and shifts (`ldr; asrs #16`).
 *  - sub_08001460 (install HBlank handler) and sub_080014bc (install VCount
 *    handler) are dead exports inside the census sizes of sub_080013f8 and
 *    sub_08001488 respectively (lesson 2.13 pattern: nothing in ROM calls
 *    them, but they sit between live functions of the same unit). */

extern vu16 gUnk_0300100C; /* keys currently held */
extern vu16 gUnk_03001EF4; /* keys held last frame */
extern vu16 gUnk_03000038; /* keys newly pressed */
extern vu16 gUnk_03000B70; /* keys pressed w/ auto-repeat */
extern vu16 gUnk_03001670; /* auto-repeat countdown */
extern vu16 gUnk_03001ECC; /* auto-repeat interval (4) */
extern vu16 gUnk_03000FC8; /* auto-repeat first delay (14) */

extern vu16 gUnk_03001ED8; /* DISPCNT shadow */
extern vu16 gUnk_03001E9C; /* DISPSTAT shadow */
extern vu16 gUnk_03001188; /* BG0CNT shadow */
extern vu16 gUnk_03000B14; /* BG1CNT shadow */
extern vu16 gUnk_03000B10; /* BG2CNT shadow */
extern vu16 gUnk_03001EB4; /* BG3CNT shadow */
extern vs32 gUnk_03000010; /* BG0VOFS shadow (16.16) */
extern vs32 gUnk_0300117C; /* BG0HOFS shadow (16.16) */
extern vs32 gUnk_03000FC0; /* BG1VOFS shadow (16.16) */
extern vs32 gUnk_03001EE0; /* BG1HOFS shadow (16.16) */
extern vs32 gUnk_03001E94; /* BG2VOFS shadow (16.16) */
extern vs32 gUnk_03000F8C; /* BG2HOFS shadow (16.16) */
extern vs32 gUnk_03000FA8; /* BG3VOFS shadow (16.16) */
extern vs32 gUnk_03000B78; /* BG3HOFS shadow (16.16) */
extern vu16 gUnk_03000FD4; /* WIN0H shadow */
extern vu16 gUnk_03001018; /* WIN1H shadow */
extern vu16 gUnk_03000044; /* WIN0V shadow */
extern vu16 gUnk_03000AFC; /* WIN1V shadow */
extern vu8 gUnk_03001ED0;  /* WININ hi shadow */
extern vu8 gUnk_03000B18;  /* WININ lo shadow */
extern vu8 gUnk_03001010;  /* WINOUT hi shadow */
extern vu8 gUnk_03000F7C;  /* WINOUT lo shadow; also end of copy queue */
extern vu8 gUnk_03001000;  /* MOSAIC hi shadow */
extern vu8 gUnk_03001EB0;  /* MOSAIC lo shadow */
extern vu8 gUnk_03000040;  /* BLDCNT hi shadow */
extern vu8 gUnk_0300118C;  /* BLDCNT lo shadow */
extern vu8 gUnk_03001EAC;  /* BLDALPHA hi shadow */
extern vu8 gUnk_03000B08;  /* BLDALPHA lo shadow */
extern vu16 gUnk_03001EEC; /* BLDY shadow */

extern vu32 gUnk_03000FB0; /* palette source buffer ptr (0x03001270) */
extern u32 gUnk_03000F88;  /* copy-request queue read pointer */
extern u32 gUnk_03000FC4;  /* copy-request queue write pointer */
extern vu16 gUnk_03000018; /* IE shadow */
extern u32 gUnk_030004B0[]; /* IRQ dispatch table (copied from 0x080CFDE8) */

extern void sub_08001518(void); /* no-op IRQ handler (bx lr) */

/* Copy the OAM shadow (0x03000050) and the palette shadow buffer to
 * OAM/palette RAM. */
void sub_080011ac(void)
{
    CpuFastSet((u32 *)0x03000050, (u32 *)0x07000000, 0x100);
    CpuFastSet((u32 *)gUnk_03000FB0, (u32 *)0x05000000, 0x100);
}

/* Poll REG_KEYINPUT into the held/new/repeat key state cells. */
void sub_080011dc(void)
{
    gUnk_0300100C = REG_KEYINPUT ^ 0x3FF;
    gUnk_03000038 = gUnk_0300100C & ~gUnk_03001EF4;

    if (gUnk_0300100C != 0 && gUnk_03001EF4 == gUnk_0300100C)
    {
        gUnk_03001670--;
        if ((s16)gUnk_03001670 <= 0)
        {
            gUnk_03000B70 = gUnk_0300100C;
            gUnk_03001670 = gUnk_03001ECC;
        }
        else
        {
            gUnk_03000B70 = 0;
        }
    }
    else
    {
        gUnk_03000B70 = gUnk_03000038;
        if (!(gUnk_03001EF4 & 0xF0))
            gUnk_03001670 = gUnk_03000FC8;
    }

    gUnk_03001EF4 = gUnk_0300100C;
}

/* Flush the display I/O register shadows to the hardware registers
 * (the write-only half of the AgbInit shadow scheme). */
void sub_08001280(void)
{
    REG_DISPCNT = gUnk_03001ED8;
    REG_DISPSTAT = gUnk_03001E9C;
    REG_BG0CNT = gUnk_03001188;
    REG_BG1CNT = gUnk_03000B14;
    REG_BG2CNT = gUnk_03000B10;
    REG_BG3CNT = gUnk_03001EB4;
    REG_BG0VOFS = gUnk_03000010 >> 16;
    REG_BG0HOFS = gUnk_0300117C >> 16;
    REG_BG1VOFS = gUnk_03000FC0 >> 16;
    REG_BG1HOFS = gUnk_03001EE0 >> 16;
    REG_BG2VOFS = gUnk_03001E94 >> 16;
    REG_BG2HOFS = gUnk_03000F8C >> 16;
    REG_BG3VOFS = gUnk_03000FA8 >> 16;
    REG_BG3HOFS = gUnk_03000B78 >> 16;
    REG_WIN0H = gUnk_03000FD4;
    REG_WIN1H = gUnk_03001018;
    REG_WIN0V = gUnk_03000044;
    REG_WIN1V = gUnk_03000AFC;
    REG_WININ = (gUnk_03001ED0 << 8) | gUnk_03000B18;
    REG_WINOUT = (gUnk_03001010 << 8) | gUnk_03000F7C;
    REG_MOSAIC = (gUnk_03001000 << 8) | gUnk_03001EB0;
    REG_BLDCNT = (gUnk_03000040 << 8) | gUnk_0300118C;
    REG_BLDALPHA = (gUnk_03001EAC << 8) | gUnk_03000B08;
    REG_BLDY = gUnk_03001EEC;
}

/* Pump the copy-request ring buffer (0x03000B80..0x03000F7B, written via
 * gUnk_03000FC4): each entry is  [ctrl][src or inline word][dst], ctrl bit0
 * selects CpuFastSet vs CpuSet, bit1 means "source is the inline word",
 * ctrl>>4 is the syscall length/mode word. */
void sub_080013f8(void)
{
    u32 *p = (u32 *)gUnk_03000F88;

    while (gUnk_03000FC4 != (u32)p)
    {
        u32 ctrl = *p++;
        const u32 *src;

        if (ctrl & 2)
        {
            src = p;
            p++;
        }
        else
        {
            src = (const u32 *)*p++;
        }

        if (ctrl & 1)
            CpuFastSet(src, (u32 *)*p++, ctrl >> 4);
        else
            CpuSet(src, (void *)*p++, ctrl >> 4);

        if (p >= (u32 *)&gUnk_03000F7C)
            p = (u32 *)((u32)&gUnk_03000F7C - 0x3FC);
    }

    gUnk_03000F88 = (u32)p;
}

/* Install an HBlank IRQ handler (dispatch slot 3) and enable the IRQ.
 * Dead export: nothing in ROM calls it (census folded it into
 * sub_080013f8's size). */
void sub_08001460(void (*fn)(void))
{
    gUnk_030004B0[3] = (u32)fn;
    REG_IE |= 2;
    gUnk_03001E9C |= 0x10;
}

/* Remove the HBlank IRQ handler and disable the IRQ. */
void sub_08001488(void)
{
    REG_IE &= 0xFFFD;
    gUnk_03001E9C &= 0xFFEF;
    gUnk_030004B0[3] = (u32)sub_08001518;
}

/* Install a VCount IRQ handler (dispatch slot 4) for scanline `vcount`.
 * Dead export: nothing in ROM calls it (census folded it into
 * sub_08001488's size). */
void sub_080014bc(void (*fn)(void), u8 vcount)
{
    gUnk_030004B0[4] = (u32)fn;
    gUnk_03000018 |= 4;
    gUnk_03001E9C |= (vcount << 8) | 0x20;
}

/* Remove the VCount IRQ handler and disable the IRQ. */
void sub_080014e8(void)
{
    gUnk_030004B0[4] = (u32)sub_08001518;
    gUnk_03000018 &= 0xFFFB;
    gUnk_03001E9C &= 0xDF;
}
