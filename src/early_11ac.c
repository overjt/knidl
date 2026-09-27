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
 *  - gIntrTable is the 7-word IRQ dispatch table AgbInit copies from
 *    0x080CFDE8 (CpuSet dest 0x030004B0).  It must be referenced as an
 *    extern array: indexing a cast constant base folds the offset into the
 *    pool word (0x030004C0 instead of 0x030004B0 + "str rX,[rY,#16]").
 *  - The BG scroll shadows (gBg0ScrollY etc.) are volatile s32: without
 *    volatile, `>> 16` narrows into an `ldrsh [rX, #2]` of the upper half;
 *    the ROM reads the whole word and shifts (`ldr; asrs #16`).
 *  - SetHBlankIntr (install HBlank handler) and SetVCountIntr (install VCount
 *    handler) are dead exports inside the census sizes of ProcessCopyQueue and
 *    ClearHBlankIntr respectively (lesson 2.13 pattern: nothing in ROM calls
 *    them, but they sit between live functions of the same unit). */

extern vu16 gUnk_0300100C; /* keys currently held */
extern vu16 gHeldKeys; /* keys held last frame */
extern vu16 gPressedKeys; /* keys newly pressed */
extern vu16 gRepeatedKeys; /* keys pressed w/ auto-repeat */
extern vu16 gKeyRepeatTimer; /* auto-repeat countdown */
extern vu16 gKeyRepeatInterval; /* auto-repeat interval (4) */
extern vu16 gKeyRepeatDelay; /* auto-repeat first delay (14) */

extern vu16 gDispCnt; /* DISPCNT shadow */
extern vu16 gDispStat; /* DISPSTAT shadow */
extern vu16 gBg0Cnt; /* BG0CNT shadow */
extern vu16 gBg1Cnt; /* BG1CNT shadow */
extern vu16 gBg2Cnt; /* BG2CNT shadow */
extern vu16 gBg3Cnt; /* BG3CNT shadow */
extern vs32 gBg0ScrollY; /* BG0VOFS shadow (16.16) */
extern vs32 gBg0ScrollX; /* BG0HOFS shadow (16.16) */
extern vs32 gBg1ScrollY; /* BG1VOFS shadow (16.16) */
extern vs32 gBg1ScrollX; /* BG1HOFS shadow (16.16) */
extern vs32 gBg2ScrollY; /* BG2VOFS shadow (16.16) */
extern vs32 gBg2ScrollX; /* BG2HOFS shadow (16.16) */
extern vs32 gBg3ScrollY; /* BG3VOFS shadow (16.16) */
extern vs32 gBg3ScrollX; /* BG3HOFS shadow (16.16) */
extern vu16 gWin0H; /* WIN0H shadow */
extern vu16 gWin1H; /* WIN1H shadow */
extern vu16 gWin0V; /* WIN0V shadow */
extern vu16 gWin1V; /* WIN1V shadow */
extern vu8 gWinIn1;  /* WININ hi shadow */
extern vu8 gWinIn0;  /* WININ lo shadow */
extern vu8 gUnk_03001010;  /* WINOUT hi shadow */
extern vu8 gUnk_03000F7C;  /* WINOUT lo shadow; also end of copy queue */
extern vu8 gUnk_03001000;  /* MOSAIC hi shadow */
extern vu8 gBgMosaic;  /* MOSAIC lo shadow */
extern vu8 gBldCntTarget2;  /* BLDCNT hi shadow */
extern vu8 gBldCntTarget1;  /* BLDCNT lo shadow */
extern vu8 gBldAlphaEvb;  /* BLDALPHA hi shadow */
extern vu8 gBldAlphaEva;  /* BLDALPHA lo shadow */
extern vu16 gBldY; /* BLDY shadow */

extern vu32 gPaletteSource; /* palette source buffer ptr (0x03001270) */
extern u32 gCopyQueueRead;  /* copy-request queue read pointer */
extern u32 gCopyQueueWrite;  /* copy-request queue write pointer */
extern vu16 gIntrEnable; /* IE shadow */
extern u32 gIntrTable[]; /* IRQ dispatch table (copied from 0x080CFDE8) */

extern void IntrDummy(void); /* no-op IRQ handler (bx lr) */

/* Copy the OAM shadow (0x03000050) and the palette shadow buffer to
 * OAM/palette RAM. */
void CopyOamAndPalette(void)
{
    CpuFastSet((u32 *)0x03000050, (u32 *)0x07000000, 0x100);
    CpuFastSet((u32 *)gPaletteSource, (u32 *)0x05000000, 0x100);
}

/* Poll REG_KEYINPUT into the held/new/repeat key state cells. */
void ReadKeys(void)
{
    gUnk_0300100C = REG_KEYINPUT ^ 0x3FF;
    gPressedKeys = gUnk_0300100C & ~gHeldKeys;

    if (gUnk_0300100C != 0 && gHeldKeys == gUnk_0300100C)
    {
        gKeyRepeatTimer--;
        if ((s16)gKeyRepeatTimer <= 0)
        {
            gRepeatedKeys = gUnk_0300100C;
            gKeyRepeatTimer = gKeyRepeatInterval;
        }
        else
        {
            gRepeatedKeys = 0;
        }
    }
    else
    {
        gRepeatedKeys = gPressedKeys;
        if (!(gHeldKeys & 0xF0))
            gKeyRepeatTimer = gKeyRepeatDelay;
    }

    gHeldKeys = gUnk_0300100C;
}

/* Flush the display I/O register shadows to the hardware registers
 * (the write-only half of the AgbInit shadow scheme). */
void FlushDisplayRegs(void)
{
    REG_DISPCNT = gDispCnt;
    REG_DISPSTAT = gDispStat;
    REG_BG0CNT = gBg0Cnt;
    REG_BG1CNT = gBg1Cnt;
    REG_BG2CNT = gBg2Cnt;
    REG_BG3CNT = gBg3Cnt;
    REG_BG0VOFS = gBg0ScrollY >> 16;
    REG_BG0HOFS = gBg0ScrollX >> 16;
    REG_BG1VOFS = gBg1ScrollY >> 16;
    REG_BG1HOFS = gBg1ScrollX >> 16;
    REG_BG2VOFS = gBg2ScrollY >> 16;
    REG_BG2HOFS = gBg2ScrollX >> 16;
    REG_BG3VOFS = gBg3ScrollY >> 16;
    REG_BG3HOFS = gBg3ScrollX >> 16;
    REG_WIN0H = gWin0H;
    REG_WIN1H = gWin1H;
    REG_WIN0V = gWin0V;
    REG_WIN1V = gWin1V;
    REG_WININ = (gWinIn1 << 8) | gWinIn0;
    REG_WINOUT = (gUnk_03001010 << 8) | gUnk_03000F7C;
    REG_MOSAIC = (gUnk_03001000 << 8) | gBgMosaic;
    REG_BLDCNT = (gBldCntTarget2 << 8) | gBldCntTarget1;
    REG_BLDALPHA = (gBldAlphaEvb << 8) | gBldAlphaEva;
    REG_BLDY = gBldY;
}

/* Pump the copy-request ring buffer (0x03000B80..0x03000F7B, written via
 * gCopyQueueWrite): each entry is  [ctrl][src or inline word][dst], ctrl bit0
 * selects CpuFastSet vs CpuSet, bit1 means "source is the inline word",
 * ctrl>>4 is the syscall length/mode word. */
void ProcessCopyQueue(void)
{
    u32 *p = (u32 *)gCopyQueueRead;

    while (gCopyQueueWrite != (u32)p)
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

    gCopyQueueRead = (u32)p;
}

/* Install an HBlank IRQ handler (dispatch slot 3) and enable the IRQ.
 * Dead export: nothing in ROM calls it (census folded it into
 * ProcessCopyQueue's size). */
void SetHBlankIntr(void (*fn)(void))
{
    gIntrTable[3] = (u32)fn;
    REG_IE |= 2;
    gDispStat |= 0x10;
}

/* Remove the HBlank IRQ handler and disable the IRQ. */
void ClearHBlankIntr(void)
{
    REG_IE &= 0xFFFD;
    gDispStat &= 0xFFEF;
    gIntrTable[3] = (u32)IntrDummy;
}

/* Install a VCount IRQ handler (dispatch slot 4) for scanline `vcount`.
 * Dead export: nothing in ROM calls it (census folded it into
 * ClearHBlankIntr's size). */
void SetVCountIntr(void (*fn)(void), u8 vcount)
{
    gIntrTable[4] = (u32)fn;
    gIntrEnable |= 4;
    gDispStat |= (vcount << 8) | 0x20;
}

/* Remove the VCount IRQ handler and disable the IRQ. */
void ClearVCountIntr(void)
{
    gIntrTable[4] = (u32)IntrDummy;
    gIntrEnable &= 0xFFFB;
    gDispStat &= 0xDF;
}
