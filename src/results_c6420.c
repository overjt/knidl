#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "ending.h"

/* results_c6420.c (0x080C6420-0x080C6C63, issue #100).
 *
 * The screens around the ending and the tile-number helpers.
 *   FinalResultsScreen   AgbMain state 12, after the staff credits: the final screen
 *       (after AgbMain state 20 DrawLargeClockScreen's clock, the clock when gMetaKnightmareMode is set,
 *       else this player's score), held until START.
 *   DrawLargeClockScreen   the clock drawn with large digit tiles.
 *   ShowMilestonePicture / ShowMilestonePictureForMode   a full-screen picture (screen 57 or 59) held
 *       until A or START; M02's game-state bodies and M03's file menu show it.
 *   DrawScoreToBgMap / DrawClockToBgMap   draw an 8-digit score / a clock into the BG
 *       map at 0x06001000 (M02's src/hud_0aad0.c renderers, drawn to VRAM);
 *       M02's ExtraModeTitleMain calls DrawClockToBgMap too.
 *   CopyToBgMap   copy n map entries to column x, row y of that BG map. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared without its parameter: early_1fd0.c defines it as
   `u32 ClearWarmBoot(u32 arg)` (it returns arg unchanged, lesson 3.391) and
   this call sets up no argument (lesson 3.428). */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void ClearWarmBoot(void);
void RunLinkFrame(void);                                     /* run one frame */
s32 PlaySfx(s32 id);                                    /* play a sound effect */
void LoadBgLayout(s32 a0);                                   /* load palette set */
void LoadGfxSet(u16 a0);                                   /* load screen graphics */

/* AgbMain state 12, after the staff credits: the final screen.  After
   AgbMain state 20 it shows DrawLargeClockScreen's clock screen, when gMetaKnightmareMode is
   set the clock, otherwise this player's score; then it waits for START, fades out
   and returns (AgbMain goes back to state 0). */
void FinalResultsScreen(void)
{
    if (gPrevGameState != GAME_STATE_BOSS_ENDURANCE) {
        LoadGfxSet(4);
        if (gMetaKnightmareMode == 0) {
            if (gExtraMode == 0)
                LoadGfxSet(56);
            else
                LoadGfxSet(58);
            DrawScoreToBgMap(gPlayerScores[gEndingLocalPlayer], 22, 0);
        } else {
            LoadGfxSet(55);
            DrawClockToBgMap(gHudClock, 22, 18);
        }
    } else {
        LoadGfxSet(54);
        DrawLargeClockScreen();
    }
    ResetFadeAndBlend();
    ResetTasksAndOam();
    LoadBgLayout(7);
    gBg0ScrollX = gBg0ScrollY = 0;
    gBg2ScrollX = gBg2ScrollY = 0;
    gBg3ScrollX = gBg3ScrollY = 0;
    if (gPrevGameState == GAME_STATE_BOSS_ENDURANCE) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x800;
    } else if (gMetaKnightmareMode == 1) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x900;
    } else if (gMilestoneFlags & (16 << gExtraMode)) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x900;
    } else {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0xD00;
    }
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    if (gPrevGameState == GAME_STATE_BOSS_ENDURANCE || gMetaKnightmareMode == 1) {
        PlayBgm(29);
        RunLinkFrames(174);
    } else {
        PlayBgm(40);
        RunLinkFrames(32);
    }
    do
        RunLinkFrame();
    while (!(gPlayerPressedKeys[0] & 8));
    PlaySfx(102);
    LinkStopKeyExchange();
    FadeOutBgm(8);
    BeginFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    RunFramesNoTasks(2);
    SetBgmVolume(255);
    ClearWarmBoot();
}

/* The final screen after AgbMain state 20: the clock drawn with the large
   digit tiles decompressed to gUnk_02020000 (two tiles per digit, the
   seconds in a second tile set). */
void DrawLargeClockScreen(void)
{
    LoadGfxSet(53);
    IntToDigits(gHudClock[3]);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[1] << 5), BG_VRAM + 0xAEA0, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 16) << 5), BG_VRAM + 0xB0A0, 32);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[0] << 5), BG_VRAM + 0xAEC0, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 16) << 5), BG_VRAM + 0xB0C0, 32);
    IntToDigits(gHudClock[2]);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[1] << 5), BG_VRAM + 0xAF00, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 16) << 5), BG_VRAM + 0xB100, 32);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[0] << 5), BG_VRAM + 0xAF20, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 16) << 5), BG_VRAM + 0xB120, 32);
    IntToDigits(gHudClock[1]);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 32) << 5), BG_VRAM + 0xAF60, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 48) << 5), BG_VRAM + 0xB160, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 32) << 5), BG_VRAM + 0xAF80, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 48) << 5), BG_VRAM + 0xB180, 32);
}

/* A full-screen picture (screen 57 or 59, by gExtraMode) shown until a
   player presses A or START; M02's game-state bodies show it once, after a
   stage when CheckNewMilestones() says so. */
void ShowMilestonePicture(void)
{
    s32 i;

    ResetFadeAndBlend();
    LoadBgLayout(7);
    if (gExtraMode == 0)
        LoadGfxSet(57);
    else
        LoadGfxSet(59);
    gBg3ScrollX = gBg3ScrollY = 0;
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x800;
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    RunLinkFrames(32);
    do {
        RunLinkFrame();
        for (i = 0; i < gPlayerCount; i++) {
            if (gPlayerPressedKeys[i] & 9) {
                PlaySfx(102);
                break;
            }
        }
    } while (i == gPlayerCount);
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
}

/* The same picture as ShowMilestonePicture for save slot `slot` (screen 57 or 59),
   shown from M03's file menu until A or START; the BG3 scroll is kept. */
void ShowMilestonePictureForMode(s32 slot)
{
    s32 x, y;

    ResetFadeAndBlend();
    x = gBg3ScrollX;
    y = gBg3ScrollY;
    LoadBgLayout(7);
    if (slot == 0)
        LoadGfxSet(57);
    else
        LoadGfxSet(59);
    gBg3ScrollX = gBg3ScrollY = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x800;
    BeginFastFadeInFromWhite();
    RunFramesNoTasksUntilFadeDone();
    RunFramesNoTasks(32);
    do
        RunFrameNoTasks();
    while (!(gPressedKeys & 9));
    PlaySfx(102);
    BeginFastFadeOutToWhite();
    RunFramesNoTasksUntilFadeDone();
    gBg3ScrollX = x;
    gBg3ScrollY = y;
}

/* Draw an 8-digit decimal score at column x, rows y and y + 1 of the BG map. */
void DrawScoreToBgMap(s32 v, s32 x, s32 y)
{
    s32 d;

    if (v >= 0) {
        d = -1;
        while (v >= 0) {
            v -= 10000000;
            d++;
        }
        v += 10000000;
        CopyToBgMap(&gHudDigitTiles[0][d], x, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000000;
            d++;
        }
        v += 1000000;
        CopyToBgMap(&gHudDigitTiles[0][d], x + 1, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x + 1, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 100000;
            d++;
        }
        v += 100000;
        CopyToBgMap(&gHudDigitTiles[0][d], x + 2, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x + 2, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 10000;
            d++;
        }
        v += 10000;
        CopyToBgMap(&gHudDigitTiles[0][d], x + 3, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x + 3, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000;
            d++;
        }
        v += 1000;
        CopyToBgMap(&gHudDigitTiles[0][d], x + 4, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x + 4, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 100;
            d++;
        }
        v += 100;
        CopyToBgMap(&gHudDigitTiles[0][d], x + 5, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x + 5, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        CopyToBgMap(&gHudDigitTiles[0][d], x + 6, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][d], x + 6, y + 1, 1);

        CopyToBgMap(&gHudDigitTiles[0][v], x + 7, y, 1);
        CopyToBgMap(&gHudDigitTiles[1][v], x + 7, y + 1, 1);
    }
}

/* Draw a clock (time[3] : time[2] : time[1], two digits each) at column x,
   rows y and y + 1 of the BG map. */
void DrawClockToBgMap(u16 *time, s32 x, s32 y)
{
    s32 v;
    s32 d;

    v = time[3];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    CopyToBgMap(&gHudDigitTiles[0][d], x, y, 1);
    CopyToBgMap(&gHudDigitTiles[1][d], x, y + 1, 1);
    CopyToBgMap(&gHudDigitTiles[0][v], x + 1, y, 1);
    CopyToBgMap(&gHudDigitTiles[1][v], x + 1, y + 1, 1);
    CopyToBgMap(gUnk_085A6F5C, x + 2, y, 1);
    CopyToBgMap(gUnk_085A6F5C + 1, x + 2, y + 1, 1);

    v = time[2];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    CopyToBgMap(&gHudDigitTiles[0][d], x + 3, y, 1);
    CopyToBgMap(&gHudDigitTiles[1][d], x + 3, y + 1, 1);
    CopyToBgMap(&gHudDigitTiles[0][v], x + 4, y, 1);
    CopyToBgMap(&gHudDigitTiles[1][v], x + 4, y + 1, 1);
    CopyToBgMap(gUnk_085A6F5C, x + 5, y, 1);
    CopyToBgMap(gUnk_085A6F5C + 1, x + 5, y + 1, 1);

    v = time[1];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    CopyToBgMap(&gHudDigitTiles[0][d], x + 6, y, 1);
    CopyToBgMap(&gHudDigitTiles[1][d], x + 6, y + 1, 1);
    CopyToBgMap(&gHudDigitTiles[0][v], x + 7, y, 1);
    CopyToBgMap(&gHudDigitTiles[1][v], x + 7, y + 1, 1);
}

/* Copy n tiles from src to the BG map at 0x06001000, row y, column x. */
void CopyToBgMap(u16 *src, s32 x, s32 y, s32 n)
{
    RequestCopy(1, (u32)src, (x + (y << 5)) * 2 + (BG_VRAM + 0x1000), n * 2);
}
