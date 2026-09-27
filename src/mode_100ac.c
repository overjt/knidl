#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_100ac.c (0x080100AC-0x08010357, issue #99).
 *
 * AgbMain state 7 (sub_080100ac), entered instead of states 5/6 while
 * gUnk_02007FC0 is set: the scripted sequence of stage gUnk_030023B8.
 * It clears the blend and window shadows, loads the sequence's palette
 * set and pictures (sub_08008d98, and sub_080102c0: the sprite sheet
 * gUnk_08731F78[stage] plus, in link play, the player palette), opens
 * window 0 (full width for sequence 7), spawns M04's director, task
 * type #91, and pumps frames until the director leaves state 7. */

extern u8 gUnk_0200AF04;
extern u32 gUnk_02020000[];
extern vs32 gBg0ScrollY;
extern vu8 gBldCntTarget2;
extern vu16 gWin0V;
extern vu8 gBldAlphaEva;
extern vu8 gWinIn0;
extern vs32 gBg3ScrollX;
extern vu8 gUnk_03000F7C;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern vs32 gBg1ScrollY;
extern vu16 gWin0H;
extern vs32 gBg0ScrollX;
extern vu8 gBldCntTarget1;
extern u16 gUnk_03001570[];
extern vs32 gBg2ScrollY;
extern vu8 gBldAlphaEvb;
extern vu16 gDispCnt;
extern vs32 gBg1ScrollX;
extern vu16 gBldY;
extern u8 gUnk_03001F30;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern s8 gUnk_030023B8;
extern u16 gGameState;
extern vu16 gLinkCommand;
extern u32 gObjVram[];
extern u16 gUnk_080DC628[][16];
extern struct GfxHeader *const gUnk_08731F78[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void RunLinkFrame(void);
void RunLinkFramesUntilFadeDone(void);
s32 TaskCreateFrom(u32 type, s32 idx);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);
void sub_08008d98(s32 a0);
void sub_08024300(void);
void sub_08027178(void);
void sub_0803d0a0(s32 a0);
void sub_080102c0(void);

void sub_080100ac(void)
{
    s32 i;

    ResetTasksAndOam();
    if (gUnk_03001F30 != 1) {
        gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = gBldY = 0;
        sub_08008c64(0);
        gUnk_0200AF04 = 1;
        sub_08024300();
        if (gUnk_030023B8 != 7)
            sub_08008c4c(8);
        else
            sub_08008c4c(9);
        sub_08008d98(gUnk_030023B8);
        gBg2ScrollX = gBg3ScrollX = 0;
        gBg2ScrollY = gBg3ScrollY = 0;
        gBg0ScrollY = 0x280000;
        gBg0ScrollX = 0x100000;
        gBg1ScrollY = 0x280000;
        gBg1ScrollX = 0x100000;
        if (gUnk_030023B8 == 7) {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x3F00;
        } else {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x3D00;
        }
        if (gUnk_030023B8 == 7) {
            gWin0H = 240;
            gWin0V = 0x1090;
            gWinIn0 = 63;
            gUnk_03000F7C = 47;
        } else {
            gWin0H = 0x28D0;
            gWin0V = 0x1090;
            gWinIn0 = 63;
            gUnk_03000F7C = 47;
        }
        for (i = 0; i <= 3; i++)
            sub_0803d0a0(i);
        sub_080102c0();
        TaskCreateFrom(91, 32);
        sub_08002358();
        sub_08002378();
        sub_080022fc();
        BeginFastFadeInFromWhite();
        RunLinkFramesUntilFadeDone();
        gLinkCommand = 0x8800;
        do
            RunLinkFrame();
        while (gGameState == 7);
        sub_08002338();
        BeginFastFadeOutToWhite();
        RunLinkFramesUntilFadeDone();
        gDispCnt &= 0xDFFF;
        gWin0H = gWin0V = gWinIn0 = gUnk_03000F7C = 0;
        gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
        sub_08027178();
    }
}

void sub_080102c0(void)
{
    struct GfxHeader *h = gUnk_08731F78[gUnk_030023B8];

    if (h != NULL) {
        LZ77UnCompWram(h->unk0C, gUnk_02020000);
        RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, h->unk02 << 5);
        RequestCopy(2, (u32)h->unk08, (u32)gUnk_03001570, h->unk00 << 5);
        if (gPlayerCount > 1)
            RequestCopy(2, (u32)gUnk_080DC628[gLocalPlayer], (u32)gUnk_03001570, 22);
    }
    if (gUnk_030023B8 == 7)
        LZ77UnCompWram((void *)0x085E0090, gUnk_02020000);
}
