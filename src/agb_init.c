#include "gba/gba.h"
#include "global.h"
#include "mode.h"
#include "save.h"

/* AgbInit (0x08000310-0x080008E7, issue #28): boot-time initializer called
 * from crt0 before main.  Clears EWRAM/IWRAM/VRAM/OAM/palette via CpuFastSet,
 * copies the IRQ handler table and the master ISR into IWRAM via CpuSet, then
 * initializes the I/O register block through IWRAM shadow cells
 * (rom-map.md §4).  The function is ~1KB, so the compiler emits its literal
 * pool behind a pool-skip branch after the epilogue (0x08000700-0x080008E7);
 * the "function" the symbol census saw at 0x08000700 is that branch, not code.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork; see docs/lessons-learned.md):
 *  - The IWRAM/EWRAM cells are unnamed state (gUnk_<address>), defined as
 *    absolute symbols in asm/rom_syms.s via tools/split_config.json
 *    "data_symbols".  They must stay symbolic: gcc derives nearby *constant*
 *    addresses with add/sub chains but never across distinct symbols, which
 *    is how the ROM's pool layout proves these were named globals.
 *  - `REG = shadow = value` chained assignments: the volatile inner cell is
 *    re-read for the outer store; non-volatile inners forward the value.
 *  - Volatile *indexed* stores (array element / struct field) emit a dead
 *    pre-read; volatile pointer-deref and bare-symbol stores do not.
 *  - The zeroA/zeroB/zeroC locals pin the per-region zero registers: each is
 *    block-local (single region, allocated r4 by local-alloc), and re-using
 *    one variable across regions would change the whole allocation. */

/* Not from main.h, link.h or sound.h: this file's view of gSfxSlotSongs,
   gUnk_03000F90 and gUnk_030023A8 differs (lesson 3.517). */
extern vu16 gUnk_03001004;
extern vu32 gUnk_03000FA0;
extern vu16 gFrameCount;
extern vu16 gVBlankCount;
extern vu16 gFrameInProgress;
extern vu16 gWaitingForVBlank;
extern u16 gPlayTime[4];
extern u32 gCopyQueueRead;
extern u32 gCopyQueueWrite;
extern u32 gUnk_03000020[4];
extern vu16 gHeldKeys;
extern vu16 gRepeatedKeys;
extern vu16 gPressedKeys;
extern vu16 gKeyRepeatDelay;
extern vu16 gKeyRepeatInterval;
extern vu16 gKeyRepeatTimer;
extern vu16 gPlayerPressedKeys[];
extern vu16 gPlayerHeldKeys[];
extern vu32 gRngValue;
extern u32 gVBlankEndCallback;
extern vu32 gUnk_03000F90;
extern u32 gBlockAnimHook;
extern vu32 gFrameEndCallback;
extern vu16 gDispCnt;
extern vu16 gDispStat;
extern vu16 gBg0Cnt;
extern vu16 gBg1Cnt;
extern vu16 gBg2Cnt;
extern vu16 gBg3Cnt;
extern u32 gBg0ScrollY;
extern u32 gBg0ScrollX;
extern u32 gBg1ScrollY;
extern u32 gBg1ScrollX;
extern u32 gBg2ScrollY;
extern u32 gBg2ScrollX;
extern u32 gBg3ScrollY;
extern u32 gBg3ScrollX;
extern vu16 gWin0H;
extern vu16 gWin1H;
extern vu16 gWin0V;
extern vu16 gWin1V;
extern vu8 gWinIn0;
extern vu8 gWinIn1;
extern vu8 gWinOut;
extern vu8 gWinObj;
extern vu8 gBgMosaic;
extern vu8 gObjMosaic;
extern vu8 gBldCntTarget1;
extern vu8 gBldCntTarget2;
extern vu8 gBldAlphaEva;
extern vu8 gBldAlphaEvb;
extern vu16 gBldY;
extern vu16 gUnk_0300100C;
extern vu16 gUnk_03000FD8;
extern vu16 gUnk_03001EA0;
extern vu16 gSoundDriverOn;
extern vs16 gVolumeRampMode;
extern vu16 gVolumeRampSpeed;
extern vs16 gVolumeRampLevel;
extern vs16 gCurrentBgm;
extern vu16 gSfxDisabled;
extern vu16 gSoundDisabled;
extern u16 gSfxSlotSongs[4];
extern vu8 gSfxSlotAges[];
extern vu8 gSfxSlotPlayers[];
extern vu8 gSfxPlayerSlots[];
extern vu32 gPaletteSource;
extern u32 gVBlankCallback;
extern u32 gFrameCallback;
extern u32 gUnk_03000B74;
extern vu16 gUnk_03001014;
extern vu16 gUnk_03001170;
extern vu16 gUnk_03000B7C;
extern vu16 gUnk_03001178;
extern vu16 gUnk_03001020;
extern vu16 gUnk_03000B20;
extern vu16 gUnk_03001008;
extern vu16 gIntrEnable;
extern vu16 gIntrMasterEnable;
extern vs32 gLinkSetupMode;
extern u32 gLinkDriverMode;
extern u16 gPlayerCount;
extern u16 gLinkPlayerCount;
struct Unk_030023A8
{
    s8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
};
extern struct Unk_030023A8 gUnk_030023A8;
extern s16 gUnk_0300244C;
extern vu32 gWarmBoot;

extern void ResetOamShadow(void);
extern void BuildOam(void);
extern void ResetFadeAndBlend(void);
extern void CheckWarmBoot(void);
extern void SeedRandom(u32 arg);

void AgbInit(void)
{
    u32 zeroWords[5];
    u16 zeroHalf0;
    u16 zeroHalf1;
    u32 zeroA;
    u32 zeroB;
    u16 zeroC;
    u16 zeroHwA;
    u8 zeroByteB;
    u16 *fillA;
    u16 *fillB;
    s32 i;

    zeroA = 0;
    zeroWords[0] = zeroA;
    CpuFastSet(&zeroWords[0], (u32 *)0x02000000, 0x01010000);
    zeroWords[1] = zeroA;
    CpuFastSet(&zeroWords[1], (u32 *)0x03000010, 0x01001EDC);
    zeroWords[2] = zeroA;
    CpuFastSet(&zeroWords[2], (u32 *)0x06000000, 0x01006000);
    zeroWords[3] = zeroA;
    CpuFastSet(&zeroWords[3], (u32 *)0x07000000, 0x01000100);
    zeroWords[4] = zeroA;
    CpuFastSet(&zeroWords[4], (u32 *)0x05000000, 0x01000100);

    gUnk_03001004 |= 0x4014;
    REG_WAITCNT = gUnk_03001004;

    gUnk_03000FA0 = zeroA;

    CpuSet((const void *)0x080CFDE8, (void *)0x030004B0, 28);
    CpuSet((const void *)0x08000108, (void *)0x03001030, 160);
    INTR_VECTOR = (void (*)(void))0x03001030;

    gVBlankCount = gFrameCount = zeroA;
    gWaitingForVBlank = gFrameInProgress = zeroA;

    gPlayTime[3] = zeroA;
    gPlayTime[2] = zeroA;
    gPlayTime[1] = zeroA;
    gPlayTime[0] = zeroA;

    gCopyQueueWrite = gCopyQueueRead = 0x03000B80;

    gUnk_03000020[3] = zeroA;
    gHeldKeys = zeroA;
    gRepeatedKeys = zeroA;
    gPressedKeys = zeroA;
    gKeyRepeatDelay = 14;
    gKeyRepeatInterval = 4;
    gKeyRepeatTimer = gKeyRepeatInterval;

    /* Computed here (matching the ROM), used for the CpuSet fills below;
     * both live across the calls in r8/r9. */
    fillA = &zeroHalf0;
    fillB = &zeroHalf1;

    zeroHwA = 0;
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = zeroHwA;

    gRngValue = zeroB = 0;
    gVBlankEndCallback = zeroB;
    gUnk_03000F90 = zeroB;
    gBlockAnimHook = zeroB;
    gFrameEndCallback = zeroB;

    gUnk_03001004 = REG_WAITCNT & 0x8000;

    REG_DISPCNT = gDispCnt = 128;

    zeroByteB = 0;
    REG_DISPSTAT = gDispStat = zeroB;
    REG_BG0CNT = gBg0Cnt = zeroB;
    REG_BG1CNT = gBg1Cnt = zeroB;
    REG_BG2CNT = gBg2Cnt = zeroB;
    REG_BG3CNT = gBg3Cnt = zeroB;

    REG_BG0VOFS = gBg0ScrollY = zeroB;
    REG_BG0HOFS = gBg0ScrollX = zeroB;
    REG_BG1VOFS = gBg1ScrollY = zeroB;
    REG_BG1HOFS = gBg1ScrollX = zeroB;
    REG_BG2VOFS = gBg2ScrollY = zeroB;
    REG_BG2HOFS = gBg2ScrollX = zeroB;
    REG_BG3VOFS = gBg3ScrollY = zeroB;
    REG_BG3HOFS = gBg3ScrollX = zeroB;

    REG_WIN0H = gWin0H = zeroB;
    REG_WIN1H = gWin1H = zeroB;
    REG_WIN0V = gWin0V = zeroB;
    REG_WIN1V = gWin1V = zeroB;

    gWinIn0 = gWinIn1 = zeroByteB;
    REG_WININ = (gWinIn1 << 8) | gWinIn0;
    gWinOut = gWinObj = zeroByteB;
    REG_WINOUT = (gWinObj << 8) | gWinOut;
    gBgMosaic = gObjMosaic = zeroByteB;
    REG_MOSAIC = (gObjMosaic << 8) | gBgMosaic;
    gBldCntTarget1 = gBldCntTarget2 = zeroByteB;
    REG_BLDCNT = (gBldCntTarget2 << 8) | gBldCntTarget1;
    gBldAlphaEva = gBldAlphaEvb = zeroByteB;
    REG_BLDALPHA = (gBldAlphaEvb << 8) | gBldAlphaEva;

    REG_BLDY = gBldY = zeroB;

    REG_DMA0CNT_H = zeroB;
    REG_DMA1CNT_H = zeroB;
    REG_DMA2CNT_H = zeroB;
    REG_DMA3CNT_H = zeroB;

    REG_TM0CNT_L = REG_TM0CNT_H = zeroB;
    REG_TM1CNT_L = REG_TM1CNT_H = zeroB;
    REG_TM2CNT_L = REG_TM2CNT_H = zeroB;
    REG_TM3CNT_L = REG_TM3CNT_H = zeroB;

    REG_SIOMULTI0 = zeroB;
    REG_SIOMULTI1 = zeroB;
    REG_SIOMULTI2 = zeroB;
    REG_SIOMULTI3 = zeroB;
    REG_SIOCNT = zeroB;
    REG_SIODATA8 = zeroB;

    REG_KEYINPUT = gUnk_0300100C = zeroB;
    REG_KEYCNT = gUnk_03000FD8 = zeroB;

    gUnk_03001EA0 = zeroB;
    gSramAvailable = zeroB;

    m4aSoundInit();

    gSoundDriverOn = 1;
    gVolumeRampMode = zeroB;
    gVolumeRampSpeed = zeroB;
    gVolumeRampLevel = 0x100;
    gCurrentBgm = -999;
    gSoundDisabled = gSfxDisabled = zeroB;

    /* gSfxSlotSongs must be a plain (non-volatile) array: |= on a volatile
     * indexed element would emit two reads, the ROM has one. */
    for (i = 0; i < 4; i++)
    {
        gSfxSlotSongs[i] |= 0xFFFF;
        gSfxSlotAges[i] = 0;
        gSfxSlotPlayers[i] = i;
        gSfxPlayerSlots[i] = i;
    }

    gPaletteSource = 0x03001270;
    zeroC = 0;
    *fillA = zeroC;
    CpuSet(fillA, (void *)0x03001270, 0x01000200);
    *fillB = zeroC;
    CpuSet(fillB, (void *)0x03001A90, 0x01000200);

    ResetOamShadow();

    CpuSet((const void *)((u32)BuildOam & ~1), (void *)0x03001F40, 0x100);

    ResetFadeAndBlend();

    /* Dead store, eliminated by the compiler.  It invalidates the compiler's
     * knowledge that zeroC == 0, which is what forces the fresh `movs rN, #0`
     * materializations in the tail below (the ROM has them; without this the
     * tail reuses zeroC's register across the three calls above). */
    zeroC = 2;

    gFrameCallback = gVBlankCallback = 0;
    gUnk_03001014 = gUnk_03000B74 = 0;

    gUnk_03000B24 = gUnk_03001008 = gUnk_03000B20 = gUnk_03001020 = gUnk_03001178 = gUnk_03000B7C = gUnk_03001170 = 0;

    REG_DISPSTAT = gDispStat = 8;

    REG_IF = 0;
    REG_IE = gIntrEnable = 0x2001;
    REG_IME = gIntrMasterEnable = 1;
    REG_RCNT = 0;

    gLinkSetupMode = -1;
    gLinkDriverMode = 0;

    gLinkPlayerCount = gPlayerCount = 1;

    /* One expression: gUnk_0300244C's address is materialized before the
     * byte stores but written last, exactly as in the ROM. */
    gUnk_0300244C = (gUnk_030023A8.unk3 = 255,
                     gUnk_030023A8.unk2 = -1,
                     gUnk_030023A8.unk1 = -1,
                     gUnk_030023A8.unk0 = -1);

    CheckWarmBoot();

    if (gWarmBoot == 0)
        SeedRandom(0xDEFBC);
}
