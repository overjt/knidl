#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "cutscene.h"
#include "room.h"
#include "player.h"
#include "ending.h"
#include "effect.h"

/* mode_100ac.c (0x080100AC-0x08010357, issue #99).
 *
 * AgbMain state 7 (CutsceneMain), entered instead of states 5/6 while
 * gCutscenePending is set: the scripted sequence of stage gCurLevel.
 * It clears the blend and window shadows, loads the sequence's palette
 * set and pictures (sub_08008d98, and sub_080102c0: the sprite sheet
 * gUnk_08731F78[stage] plus, in link play, the player palette), opens
 * window 0 (full width for sequence 7), spawns M04's director, task
 * type #91, and pumps frames until the director leaves state 7. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void CutsceneMain(void)
{
    s32 i;

    ResetTasksAndOam();
    if (gMetaKnightmareMode != 1) {
        gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = gBldY = 0;
        LoadGfxSet(0);
        gUnk_0200AF04 = 1;
        LoadCutsceneRoom();
        if (gCurLevel != 7)
            LoadBgLayout(8);
        else
            LoadBgLayout(9);
        sub_08008d98(gCurLevel);
        gBg2ScrollX = gBg3ScrollX = 0;
        gBg2ScrollY = gBg3ScrollY = 0;
        gBg0ScrollY = 0x280000;
        gBg0ScrollX = 0x100000;
        gBg1ScrollY = 0x280000;
        gBg1ScrollX = 0x100000;
        if (gCurLevel == 7) {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x3F00;
        } else {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x3D00;
        }
        if (gCurLevel == 7) {
            gWin0H = 240;
            gWin0V = 0x1090;
            gWinIn0 = 63;
            gWinOut = 47;
        } else {
            gWin0H = 0x28D0;
            gWin0V = 0x1090;
            gWinIn0 = 63;
            gWinOut = 47;
        }
        for (i = 0; i <= 3; i++)
            InitPlayerState(i);
        sub_080102c0();
        TaskCreateFrom(91, 32);
        LinkRequestSync();
        LinkSyncRandom();
        LinkStartKeyExchange();
        BeginFastFadeInFromWhite();
        RunLinkFramesUntilFadeDone();
        gLinkCommand = 0x8800;
        do
            RunLinkFrame();
        while (gGameState == 7);
        LinkStopKeyExchange();
        BeginFastFadeOutToWhite();
        RunLinkFramesUntilFadeDone();
        gDispCnt &= 0xDFFF;
        gWin0H = gWin0V = gWinIn0 = gWinOut = 0;
        gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
        sub_08027178();
    }
}

void sub_080102c0(void)
{
    struct GfxHeader *h = gUnk_08731F78[gCurLevel];

    if (h != NULL) {
        LZ77UnCompWram(h->tiles, gUnk_02020000);
        RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, h->tileCount << 5);
        RequestCopy(2, (u32)h->palette, (u32)gUnk_03001570, h->paletteBankCount << 5);
        if (gPlayerCount > 1)
            RequestCopy(2, (u32)gPlayerPalettes[gLocalPlayer], (u32)gUnk_03001570, 22);
    }
    if (gCurLevel == 7)
        LZ77UnCompWram(gUnk_085E0090, gUnk_02020000);
}
