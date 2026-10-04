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

/* gameover_cacf0.c (0x080CACF0-0x080CB353, issue #100).
 *
 * AgbMain state 22, the game-over / continue screen.
 *   GameOverMain   the state body: one of three screens - GameOverScreen
 *       when gMetaKnightmareMode is 0, GameOverMetaKnightmareScreen when it is set, GameOverBossEnduranceScreen after
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

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void RunLinkFrame(void);                                     /* run one frame */
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void LoadBgLayout(s32 a0);                                   /* load palette set */
void LoadGfxSet(u16 a0);                                   /* load screen graphics */

/* AgbMain state 22: the game-over / continue screen. */
void GameOverMain(void)
{
    ResetTasksAndOam();
    gKeyRepeatDelay = 10;
    gKeyRepeatInterval = 6;
    gGameOverCursor = 0;
    if (gPrevGameState != GAME_STATE_BOSS_ENDURANCE) {
        if (gMetaKnightmareMode == 0)
            GameOverScreen();
        else
            GameOverMetaKnightmareScreen();
    } else {
        GameOverBossEnduranceScreen();
    }
    LinkStopKeyExchange();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    if (gGameState == GAME_STATE_HUB) {
        gCutscenePending = 1;
        ResetPlayerRecords();
        ResetLevelStateForContinue();
        if (gStageRequest != STAGE_REQUEST_HUB)
            gGameState = GAME_STATE_STAGE_START;
    } else {
        DisconnectLink();
    }
}

/* The game-over screen when gMetaKnightmareMode == 0: scroll the
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
    PlayBgm(BGM_GAME_OVER);
    ResetFadeAndBlend();
    BeginFastFadeInFromWhite();
    while (gBg2ScrollX != 0) {
        gBg2ScrollX -= 0x78000;
        if (gBg3ScrollX != 180 << 16) {
            gBg3ScrollX -= 0x30000;
            if (gBg3ScrollX == 180 << 16) {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1F00;
                TaskCreateFrom(TASK_GAME_OVER_PALETTE, 32);
            }
        }
        RunLinkFrame();
    }
    for (i = 0; i < 8; i++) {
        struct Task *t = &gTasks[TaskCreateFrom(TASK_GAME_OVER_SPRITE, 32)];
        t->gameOverSpriteIndex = i;
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

/* The game-over screen when gMetaKnightmareMode != 0: scroll the
   banner in, spawn the eight letters (#260) and the cursor (#261), then
   run the continue choice with the clock on screen. */
void GameOverMetaKnightmareScreen(void)
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
    PlayBgm(BGM_GAME_OVER);
    ResetFadeAndBlend();
    BeginFastFadeInFromWhite();
    while (gBg2ScrollX != 0) {
        gBg2ScrollX -= 0x78000;
        if (gBg3ScrollX != 180 << 16) {
            gBg3ScrollX -= 0x30000;
            if (gBg3ScrollX == 180 << 16) {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1F00;
                TaskCreateFrom(TASK_GAME_OVER_PALETTE, 32);
            }
        }
        GameOverShowClock(1);
    }
    for (i = 0; i < 8; i++) {
        struct Task *t = &gTasks[TaskCreateFrom(TASK_GAME_OVER_SPRITE, 32)];
        t->gameOverSpriteIndex = i;
        GameOverShowClock(8);
    }
    GameOverResetWait();
    TaskCreateFrom(TASK_GAME_OVER_CURSOR, 32);
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
void GameOverBossEnduranceScreen(void)
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
    PlayBgm(BGM_GAME_OVER);
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
        PlaySfx(SE_CURSOR_MOVE);
        return 1;
    }
    return 0;
}

u8 GameOverCheckConfirm(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        if (gPlayerPressedKeys[i] & 9) {
            PlaySfx(SE_CONFIRM);
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
        if (gPrevGameState == GAME_STATE_BOSS_ENDURANCE)
            gGameState = GAME_STATE_MAIN_MENU;
        else
            gGameState = GAME_STATE_BOOT_LOGO;
    }
}

void GameOverCheckChoice(void)
{
    if (GameOverCheckConfirm()) {
        if (gGameOverCursor == 0 && gPlayerCount == 1)
            gGameState = GAME_STATE_HUB;
        else
            gGameState = GAME_STATE_BOOT_LOGO;
    }
}

/* Load the game-over screen's palette and sprite tiles (a second set in
   single-player play). */
void GameOverLoadGraphics(void)
{
    RequestCopy(2, (u32)gUnk_085E2C20, (u32)gObjPaletteBank4, 192);
    if (gPlayerCount == 1) {
        LZ77UnCompWram(gUnk_085E2CE0, gUnk_02020000);
        RequestCopy(3, (u32)gUnk_02020000, OBJ_VRAM0 + 0x2C00, 0x2A00);
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
    s32 gameOverObjectSlot;
    struct Task *t;

    if (gPlayerCount == 1) {
        TaskCreateFrom(TASK_GAME_OVER_CURSOR, 32);
        for (i = 0; i <= 2; i++) {
            gameOverObjectSlot = TaskCreateFrom(TASK_GAME_OVER_OBJECT, 32);
            if (gameOverObjectSlot != -1) {
                t = &gTasks[gameOverObjectSlot];
                t->variant = i;
                if (i == 0)
                    gGameOverPlayerTask = gameOverObjectSlot;
            }
        }
    } else {
        gameOverObjectSlot = TaskCreateFrom(TASK_GAME_OVER_OBJECT, 32);
        if (gameOverObjectSlot != -1) {
            struct Task *t2 = &gTasks[gameOverObjectSlot];
            t2->variant = 5;
        }
    }
}
