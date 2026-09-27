#include "gba/gba.h"
#include "global.h"
#include "main.h"

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
    REG_WINOUT = (gWinObj << 8) | gWinOut;
    REG_MOSAIC = (gObjMosaic << 8) | gBgMosaic;
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

        if (p >= (u32 *)&gWinOut)
            p = (u32 *)((u32)&gWinOut - 0x3FC);
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
