#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* results_c6420.c (0x080C6420-0x080C6C63, issue #100).
 *
 * The screens around the ending and the tile-number helpers.
 *   sub_080c6420   AgbMain state 12, after the staff credits: the final screen
 *       (after AgbMain state 20 sub_080c6600's clock, in link play the clock,
 *       else this player's score), held until START.
 *   sub_080c6600   the clock drawn with large digit tiles.
 *   sub_080c6750 / sub_080c680c   a full-screen picture (screen 57 or 59) held
 *       until A or START; M02's game-state bodies and M03's file menu show it.
 *   DrawScoreToBgMap / DrawClockToBgMap   draw an 8-digit score / a clock into the BG
 *       map at 0x06001000 (M02's src/hud_0aad0.c renderers, drawn to VRAM);
 *       M02's sub_08007f9c calls DrawClockToBgMap too.
 *   sub_080c6c3c   copy n map entries to column x, row y of that BG map. */

extern vu16 gDispCnt;          /* DISPCNT shadow */
extern vs32 gBg0ScrollY;          /* BG0 16.16 scroll shadows ... */
extern vs32 gBg0ScrollX;
extern vs32 gBg2ScrollY;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern vs32 gBg3ScrollX;          /* ... BG3 */
extern u8 gUnk_03001F30;            /* link-play mode */
extern u16 gPrevGameState;           /* previous game state */
extern u16 gPlayerCount;           /* number of players */
extern u8 gExtraMode;
extern u16 gUnk_03002364;
extern u16 gUnk_02000028;
extern s32 gPlayerScores[];         /* score per player */
extern u16 gHudClock[];         /* clock (four fields) */
extern vu16 gPlayerPressedKeys[];        /* keys pressed per player */
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u16 gHudDigitTiles[2][10];    /* digit tiles, top and bottom rows */
extern u16 gUnk_085A6F5C[];         /* the clock's colon tiles */
extern s8 gDigits[];
extern vu16 gPressedKeys;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetFadeAndBlend(void);
void BeginFastFadeInFromWhite(void);
void BeginFadeOutToWhite(void);
void BeginFastFadeOutToWhite(void);
/* Declared without its parameter: early_1fd0.c defines it as
   `u32 ClearWarmBoot(u32 arg)` (it returns arg unchanged, lesson 3.391) and
   this call sets up no argument (lesson 3.428). */
void ClearWarmBoot(void);
void ResetTasksAndOam(void);
void RunFrameNoTasks(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkRequestSync(void);
void LinkSyncRandom(void);
void RunLinkFrame(void);                                     /* run one frame */
void RunFramesNoTasks(s32 count);
void RunLinkFrames(s32 count);
void RunFramesNoTasksUntilFadeDone(void);
void RunLinkFramesUntilFadeDone(void);
void IntToDigits(s16 n);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
void FadeOutBgm(s32 speed);
void SetBgmVolume(u16 volume);
void LoadBgLayout(s32 a0);                                   /* load palette set */
void LoadGfxSet(u16 a0);                                   /* load screen graphics */
void sub_080c6600(void);
void DrawScoreToBgMap(s32 v, s32 x, s32 y);
void DrawClockToBgMap(u16 *time, s32 x, s32 y);

void sub_080c6c3c(u16 *src, s32 x, s32 y, s32 n);

/* AgbMain state 12, after the staff credits: the final screen.  After
   AgbMain state 20 it shows sub_080c6600's clock screen, in link play the
   clock, otherwise this player's score; then it waits for START, fades out
   and returns (AgbMain goes back to state 0). */
void sub_080c6420(void)
{
    if (gPrevGameState != 20) {
        LoadGfxSet(4);
        if (gUnk_03001F30 == 0) {
            if (gExtraMode == 0)
                LoadGfxSet(56);
            else
                LoadGfxSet(58);
            DrawScoreToBgMap(gPlayerScores[gUnk_02000028], 22, 0);
        } else {
            LoadGfxSet(55);
            DrawClockToBgMap(gHudClock, 22, 18);
        }
    } else {
        LoadGfxSet(54);
        sub_080c6600();
    }
    ResetFadeAndBlend();
    ResetTasksAndOam();
    LoadBgLayout(7);
    gBg0ScrollX = gBg0ScrollY = 0;
    gBg2ScrollX = gBg2ScrollY = 0;
    gBg3ScrollX = gBg3ScrollY = 0;
    if (gPrevGameState == 20) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x800;
    } else if (gUnk_03001F30 == 1) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x900;
    } else if (gUnk_03002364 & (16 << gExtraMode)) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x900;
    } else {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0xD00;
    }
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    if (gPrevGameState == 20 || gUnk_03001F30 == 1) {
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
void sub_080c6600(void)
{
    LoadGfxSet(53);
    IntToDigits(gHudClock[3]);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[1] << 5), 0x0600AEA0, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 16) << 5), 0x0600B0A0, 32);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[0] << 5), 0x0600AEC0, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 16) << 5), 0x0600B0C0, 32);
    IntToDigits(gHudClock[2]);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[1] << 5), 0x0600AF00, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 16) << 5), 0x0600B100, 32);
    RequestCopy(1, (u32)gUnk_02020000 + (gDigits[0] << 5), 0x0600AF20, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 16) << 5), 0x0600B120, 32);
    IntToDigits(gHudClock[1]);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 32) << 5), 0x0600AF60, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[1] + 48) << 5), 0x0600B160, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 32) << 5), 0x0600AF80, 32);
    RequestCopy(1, (u32)gUnk_02020000 + ((gDigits[0] + 48) << 5), 0x0600B180, 32);
}

/* A full-screen picture (screen 57 or 59, by gExtraMode) shown until a
   player presses A or START; M02's game-state bodies show it once, after a
   stage when sub_080b8290() says so. */
void sub_080c6750(void)
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

/* The same picture as sub_080c6750 for save slot `slot` (screen 57 or 59),
   shown from M03's file menu until A or START; the BG3 scroll is kept. */
void sub_080c680c(s32 slot)
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
        sub_080c6c3c(&gHudDigitTiles[0][d], x, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000000;
            d++;
        }
        v += 1000000;
        sub_080c6c3c(&gHudDigitTiles[0][d], x + 1, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x + 1, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 100000;
            d++;
        }
        v += 100000;
        sub_080c6c3c(&gHudDigitTiles[0][d], x + 2, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x + 2, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 10000;
            d++;
        }
        v += 10000;
        sub_080c6c3c(&gHudDigitTiles[0][d], x + 3, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x + 3, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000;
            d++;
        }
        v += 1000;
        sub_080c6c3c(&gHudDigitTiles[0][d], x + 4, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x + 4, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 100;
            d++;
        }
        v += 100;
        sub_080c6c3c(&gHudDigitTiles[0][d], x + 5, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x + 5, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        sub_080c6c3c(&gHudDigitTiles[0][d], x + 6, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][d], x + 6, y + 1, 1);

        sub_080c6c3c(&gHudDigitTiles[0][v], x + 7, y, 1);
        sub_080c6c3c(&gHudDigitTiles[1][v], x + 7, y + 1, 1);
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
    sub_080c6c3c(&gHudDigitTiles[0][d], x, y, 1);
    sub_080c6c3c(&gHudDigitTiles[1][d], x, y + 1, 1);
    sub_080c6c3c(&gHudDigitTiles[0][v], x + 1, y, 1);
    sub_080c6c3c(&gHudDigitTiles[1][v], x + 1, y + 1, 1);
    sub_080c6c3c(gUnk_085A6F5C, x + 2, y, 1);
    sub_080c6c3c(gUnk_085A6F5C + 1, x + 2, y + 1, 1);

    v = time[2];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    sub_080c6c3c(&gHudDigitTiles[0][d], x + 3, y, 1);
    sub_080c6c3c(&gHudDigitTiles[1][d], x + 3, y + 1, 1);
    sub_080c6c3c(&gHudDigitTiles[0][v], x + 4, y, 1);
    sub_080c6c3c(&gHudDigitTiles[1][v], x + 4, y + 1, 1);
    sub_080c6c3c(gUnk_085A6F5C, x + 5, y, 1);
    sub_080c6c3c(gUnk_085A6F5C + 1, x + 5, y + 1, 1);

    v = time[1];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    sub_080c6c3c(&gHudDigitTiles[0][d], x + 6, y, 1);
    sub_080c6c3c(&gHudDigitTiles[1][d], x + 6, y + 1, 1);
    sub_080c6c3c(&gHudDigitTiles[0][v], x + 7, y, 1);
    sub_080c6c3c(&gHudDigitTiles[1][v], x + 7, y + 1, 1);
}

/* Copy n tiles from src to the BG map at 0x06001000, row y, column x. */
void sub_080c6c3c(u16 *src, s32 x, s32 y, s32 n)
{
    RequestCopy(1, (u32)src, (x + (y << 5)) * 2 + 0x06001000, n * 2);
}
