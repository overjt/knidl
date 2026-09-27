#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gameover_cacf0.c (0x080CACF0-0x080CB353, issue #100).
 *
 * AgbMain state 22, the game-over / continue screen.
 *   GameOverMain   the state body: one of three screens - GameOverScreen
 *       outside link play, sub_080caeec in link play, sub_080cb058 after
 *       AgbMain state 20 - then, when the choice set game state 5 (continue),
 *       back into the stage (state 6 unless gStageRequest is 1), else the SIO
 *       session is torn down.
 *   GameOverLoadGraphics / CreateGameOverObjects   the screen's graphics and objects: with
 *       one player task type #261 and #264 variants 0-2 (variant 0's task
 *       index goes to gGameOverPlayerTask), else #264 variant 5.
 *   GameOverShowClock   redraw the clock for n frames.
 *   GameOverMoveCursor / GameOverIsUpDownPressed   up or down flips the cursor gGameOverCursor.
 *   GameOverCheckConfirm / GameOverCheckTimeout / GameOverCheckChoice   A or START (or the end of
 *       the 480-frame count gGameOverTimer) ends the screen (gGameOverDone)
 *       and picks the next game state.
 *   GameOverResetWait   reset the done flag and the count. */

extern vu16 gDispCnt;          /* DISPCNT shadow */
extern vs32 gBg1ScrollY;
extern vs32 gBg1ScrollX;
extern vs32 gBg2ScrollY;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern vs32 gBg3ScrollX;          /* ... BG3 */
extern u8 gUnk_03001F30;            /* link-play mode */
extern u16 gPrevGameState;           /* previous game state */
extern u16 gGameState;           /* game state (AgbMain dispatch) */
extern u16 gLocalPlayer;           /* this player's index */
extern u16 gPlayerCount;           /* number of players */
extern s32 gPlayerScores[];         /* score per player */
extern u16 gHudClock[];         /* clock (four fields) */
extern vu16 gPlayerPressedKeys[];        /* keys pressed per player */
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u8 gGameOverDone;            /* game-over screen: done flag */
extern s16 gGameOverTimer;           /* game-over screen: frames left */
extern s8 gGameOverCursor;            /* game-over screen: cursor (continue = 0?) */
extern s16 gGameOverPlayerTask;           /* game-over screen: the #264 variant-0 task's index */
extern u32 gObjVram[];         /* OBJ VRAM */
extern vu16 gKeyRepeatDelay;
extern vu16 gKeyRepeatInterval;
extern u8 gCutscenePending;
extern s8 gStageRequest;
extern u32 gUnk_085E2C20[];
extern u32 gUnk_085E2CE0[];
extern u32 gUnk_085E4064[];
extern u32 gUnk_085E5BC4[];
extern u16 gUnk_030014F0[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetFadeAndBlend(void);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkRequestSync(void);
void LinkSyncRandom(void);
void LinkSyncClock(void);
void DisconnectLink(void);
void RunLinkFrame(void);                                     /* run one frame */
void RunLinkFrames(s32 count);
void RunLinkFramesUntilFadeDone(void);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void LoadBgLayout(s32 a0);                                   /* load palette set */
void LoadGfxSet(u16 a0);                                   /* load screen graphics */
void ResetPlayerRecords(void);
void sub_08022c3c(void);
void DrawScoreToBgMap(s32 v, s32 x, s32 y);
void DrawClockToBgMap(u16 *time, s32 x, s32 y);
void GameOverScreen(void);
void sub_080caeec(void);
void GameOverShowClock(s32 n);
void sub_080cb058(void);
void GameOverMoveCursor(void);
u8 GameOverIsUpDownPressed(void);
void GameOverCheckTimeout(void);
void GameOverCheckChoice(void);
void GameOverLoadGraphics(void);
void GameOverResetWait(void);
void CreateGameOverObjects(void);

/* AgbMain state 22: the game-over / continue screen. */
void GameOverMain(void)
{
    ResetTasksAndOam();
    gKeyRepeatDelay = 10;
    gKeyRepeatInterval = 6;
    gGameOverCursor = 0;
    if (gPrevGameState != 20) {
        if (gUnk_03001F30 == 0)
            GameOverScreen();
        else
            sub_080caeec();
    } else {
        sub_080cb058();
    }
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    if (gGameState == 5) {
        gCutscenePending = 1;
        ResetPlayerRecords();
        sub_08022c3c();
        if (gStageRequest != 1)
            gGameState = 6;
    } else {
        DisconnectLink();
    }
}

/* The game-over screen outside link play (gUnk_03001F30 == 0): scroll the
   banner in, spawn the eight letters (#260) and the #261/#264 objects,
   then wait for the continue choice. */
void GameOverScreen(void)
{
    s32 i;

    gBg1ScrollX = gBg1ScrollY = 0;
    gBg2ScrollX = gBg3ScrollX = 240 << 16;
    gBg2ScrollY = gBg3ScrollY = 0;
    LoadBgLayout(6);
    LoadGfxSet(4);
    LoadGfxSet(51);
    DrawScoreToBgMap(gPlayerScores[gLocalPlayer], 22, 18);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1D00;
    GameOverLoadGraphics();
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    PlayBgm(16);
    ResetFadeAndBlend();
    BeginFastFadeInFromWhite();
    while (gBg2ScrollX != 0) {
        gBg2ScrollX -= 0x78000;
        if (gBg3ScrollX != 180 << 16) {
            gBg3ScrollX -= 0x30000;
            if (gBg3ScrollX == 180 << 16) {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1F00;
                TaskCreateFrom(262, 32);
            }
        }
        RunLinkFrame();
    }
    for (i = 0; i < 8; i++) {
        struct Task *t = &gTasks[TaskCreateFrom(260, 32)];
        t->unk18 = i;
        RunLinkFrames(8);
    }
    GameOverResetWait();
    CreateGameOverObjects();
    RunLinkFrames(8);
    do {
        RunLinkFrame();
        if (gPlayerCount != 1)
            GameOverCheckTimeout();
    } while (gGameOverDone == 0);
}

/* The game-over screen in link play (gUnk_03001F30 != 0): scroll the
   banner in, spawn the eight letters (#260) and the cursor (#261), then
   run the continue choice with the clock on screen. */
void sub_080caeec(void)
{
    s32 i;

    gBg1ScrollX = gBg1ScrollY = 0;
    gBg2ScrollX = gBg3ScrollX = 240 << 16;
    gBg2ScrollY = gBg3ScrollY = 0;
    LoadBgLayout(6);
    LoadGfxSet(4);
    LoadGfxSet(51);
    LinkRequestSync();
    LinkSyncRandom();
    LinkStartKeyExchange();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1D00;
    PlayBgm(16);
    ResetFadeAndBlend();
    BeginFastFadeInFromWhite();
    while (gBg2ScrollX != 0) {
        gBg2ScrollX -= 0x78000;
        if (gBg3ScrollX != 180 << 16) {
            gBg3ScrollX -= 0x30000;
            if (gBg3ScrollX == 180 << 16) {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1F00;
                TaskCreateFrom(262, 32);
            }
        }
        GameOverShowClock(1);
    }
    for (i = 0; i < 8; i++) {
        struct Task *t = &gTasks[TaskCreateFrom(260, 32)];
        t->unk18 = i;
        GameOverShowClock(8);
    }
    GameOverResetWait();
    TaskCreateFrom(261, 32);
    GameOverShowClock(8);
    do {
        GameOverShowClock(1);
        GameOverMoveCursor();
        GameOverCheckChoice();
    } while (gGameOverDone == 0);
}

void GameOverShowClock(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        DrawClockToBgMap(gHudClock, 22, 18);
        RunLinkFrame();
    }
}

/* The game-over screen after game state 20 (gPrevGameState == 20): the
   clock on screen until the countdown or a button ends it. */
void sub_080cb058(void)
{
    gBg3ScrollX = gBg3ScrollY = 0;
    LoadBgLayout(6);
    LoadGfxSet(4);
    LoadGfxSet(52);
    LinkRequestSync();
    LinkSyncClock();
    LinkStartKeyExchange();
    DrawClockToBgMap(gHudClock, 22, 18);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x900;
    PlayBgm(16);
    ResetFadeAndBlend();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    GameOverResetWait();
    do {
        RunLinkFrame();
        GameOverCheckTimeout();
    } while (gGameOverDone == 0);
}

void GameOverMoveCursor(void)
{
    if (GameOverIsUpDownPressed() == 1)
        gGameOverCursor ^= 1;
}

u8 GameOverIsUpDownPressed(void)
{
    if (gPlayerPressedKeys[0] & 0xC0) {
        PlaySfx(101);
        return 1;
    }
    return 0;
}

u8 GameOverCheckConfirm(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        if (gPlayerPressedKeys[i] & 9) {
            PlaySfx(102);
            gGameOverDone = 1;
            return 1;
        }
    }
    return 0;
}

void GameOverCheckTimeout(void)
{
    if (gGameOverTimer <= 0)
        gGameOverDone = 1;
    else
        GameOverCheckConfirm();
    gGameOverTimer--;
    if (gGameOverDone != 0) {
        if (gPrevGameState == 20)
            gGameState = 4;
        else
            gGameState = 1;
    }
}

void GameOverCheckChoice(void)
{
    if (GameOverCheckConfirm()) {
        if (gGameOverCursor == 0 && gPlayerCount == 1)
            gGameState = 5;
        else
            gGameState = 1;
    }
}

/* Load the game-over screen's palette and sprite tiles (a second set in
   single-player play). */
void GameOverLoadGraphics(void)
{
    RequestCopy(2, (u32)gUnk_085E2C20, (u32)gUnk_030014F0, 192);
    if (gPlayerCount == 1) {
        LZ77UnCompWram(gUnk_085E2CE0, gUnk_02020000);
        RequestCopy(3, (u32)gUnk_02020000, 0x06012C00, 0x2A00);
        LZ77UnCompWram(gUnk_085E4064, gUnk_02020000);
        RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, 0x3A00);
    } else {
        LZ77UnCompWram(gUnk_085E5BC4, gUnk_02020000);
        RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, 0xC00);
    }
}

void GameOverResetWait(void)
{
    gGameOverDone = 0;
    gGameOverTimer = 480;
}

/* Spawn the game-over screen's objects: in single-player play the cursor
   (#261) and #264 variants 0-2 (variant 0's index goes to gGameOverPlayerTask),
   otherwise #264 variant 5. */
void CreateGameOverObjects(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    if (gPlayerCount == 1) {
        TaskCreateFrom(261, 32);
        for (i = 0; i <= 2; i++) {
            id = TaskCreateFrom(264, 32);
            if (id != -1) {
                t = &gTasks[id];
                t->unk73 = i;
                if (i == 0)
                    gGameOverPlayerTask = id;
            }
        }
    } else {
        id = TaskCreateFrom(264, 32);
        if (id != -1) {
            struct Task *t2 = &gTasks[id];
            t2->unk73 = 5;
        }
    }
}
