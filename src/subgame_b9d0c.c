/* game_code_and_rodata_080653ec_0806ef5c 0x080B9D0C-0x080BA774
 * (issue #95, module M35, file 1 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080B9D0C 0x080BA774 src/subgame_b9d0c.c --newpb
 *
 * The sub-game framework: the code AgbMain enters for its sub-game state
 * (SubGameMain) and that every sub-game shares.  gUnk_02007FCC selects the
 * sub-game (0 = the reaction duel in this module, 1 = the four-slot
 * bomb-pass game of M36, 2 = the game whose body is in M37) and indexes the
 * per-game tables 0x087562A8 (graphics set), 0x087562C0 (BGM), 0x087562CC
 * (init hook) and 0x087562D8 (task body).  gSubGamePhase is the sub-game's
 * phase (0/1 running, 2-4 finished; the per-game body dispatches on it).
 *
 *   SubGameMain   entry from AgbMain: link handshake, RNG warm-up, spawn
 *                  the task type #93 controller, run both screens
 *   SubGameRunScreen   one screen: load, fade in, wait for the phase to leave
 *                  0/1, optional link resync (SubGameSyncLink), fade out, stop
 *                  DMA0 and hand the task over to Task_SubGame
 *   SubGameSyncLink   the SIO handshake: 0x7755 / 0xAA00 / 0xAA01 / 0xAA02
 *                  exchanged through gLinkCommand and the send/receive
 *                  buffers gSendCmd / gRecvCmds until every
 *                  linked player reports 0xAA02
 *   Task_SubGame   task type #93: kill every other task, then run the
 *                  per-game body from 0x087562D8
 *   QuickDrawInit.. the reaction duel's set-up and the helpers its round
 *                  controller (next file) calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "sound.h"
#include "mode.h"
#include "room.h"
#include "subgame.h"

/* Not from main.h or link.h: this file's view of gFrameCallback and gRecvCmds
   differs (lesson 3.517). */
extern u32 gLinkDriverMode;
extern vs32 gLinkSetupMode;
extern vs32 gBg0ScrollY;
extern u32 gFrameCallback;
extern vu16 gFadeBlankAtWhite;
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern u32 gVBlankCallback;
extern vs32 gBg3ScrollY;
extern vu16 gVBlankCount;
extern vs32 gBg1ScrollY;
extern vs32 gBg0ScrollX;
extern vu8 gBldCntTarget1;
extern u16 gBgPalette[];
extern vu16 gFadeSteps;
extern vs32 gBg2ScrollY;    /* vs32 here (vu32 elsewhere): see SubGameRunScreen */
extern vu16 gPlayerPressedKeys[];
extern vu16 gDispCnt;
extern vs32 gBg1ScrollX;
extern vu16 gBldY;
extern u16 gLinkIsMaster;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern vu16 gLinkPlayerCount;
extern u32 gLinkErrorMask;
extern u16 gRecvCmds[];
extern u32 gLinkStatus;
extern u32 gSerialIntrCount;
extern u16 gShouldAdvanceLinkState[];
extern u16 gSendCmd[4];
extern u16 gLinkCommand;     /* SIO handshake word; see SubGameSyncLink */

void EndFrame(void);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetFadeAndBlend(void);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void RunFrameNoTasks(void);
void RunFrame(void);
void LinkStartKeyExchange(void);
void LinkStopKeyExchange(void);
void LinkRequestSync(void);
void LinkSyncRandom(void);
void RunLinkFrame(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void CallTableEntry(u32 a, u32 b, u32 *c);
u32 Random(void);
u32 RandomRange(u32 range);
s32 PlaySfx(s32 id);
void TaskSleepForever(void);
void TaskSetEntry(void *fn, u32 i);
void DisableSerial(void);
void LinkMain1(void *cmd, void *send, void *recv);
u32 ConnectLink(void);
u32 IsLinkError(void);

void SubGameReplay(s32 a0)
{
    gSubGameLevel = a0;
    gCurTask->subGameNextPhase = 3;
}

void SubGameQuit(void)
{
    if (gPrevGameState == GAME_STATE_MAIN_MENU)
        gLinkErrorMask = 0;
    gCurTask->subGameNextPhase = 4;
}

s32 SubGameInit(void)
{
    return gSubGameInitHooks[gUnk_02007FCC]();
}

u8 SubGameAnyPressedAOrStart(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerPressedKeys[i] & 9)
        {
            found = 1;
            break;
        }
    }
    return found;
}

u8 SubGameAnyPressedB(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerPressedKeys[i] & 2)
        {
            found = 1;
            break;
        }
    }
    return found;
}

void SubGameDimAndHalt(void)
{
    s32 i;

    TaskSetSkipMask((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE), gSubGameTaskIdx);
    gDispCnt |= 0x200;
    gBldCntTarget1 = 0xFD;
    for (i = 0; i <= 4; i++)
    {
        gBldY = i;
        SubGameRunFrame();
    }
    while (1)
        SubGameRunFrame();
}

void SubGameCheckEnd(void)
{
    if (gSubGamePhase <= 1)
    {
        s32 v = gCurTask->subGameNextPhase;
        if (v != 0)
            gSubGamePhase = v;
    }
}

void SubGameLoadScreen(s32 a0)
{
    s32 m = gUnk_02007FCC;

    LoadBgLayout(gSubGameBgLayouts[m][a0]);
    m = m * 2 + a0;
    if (gSubGameGfxSets[m] != 0)
        LoadGfxSet(gSubGameGfxSets[m]);
    SubGameLoadObjTiles(gUnk_02007FCC, a0);
    gSubGamePhase = a0;
}

void SubGameSetDisplayLayers(s32 a0)
{
    switch (gUnk_02007FCC)
    {
    case 0:
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1400;
        break;
    case 1:
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1800;
        break;
    case 2:
        if (a0 != 0)
        {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1C00;
        }
        else
        {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1F00;
        }
        break;
    }
}

void SubGameRunScreen(s32 a0)
{
    s32 i;

    ResetFadeAndBlend();
    SubGameLoadScreen(a0);
    if (a0 != 0 || gUnk_02007FCC != 2)
    {
        gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
        /* Every link of a volatile chain is re-read after its store, but
           only while neighbouring links have the same type: a signedness
           change wraps the inner assignment in a conversion that fold()
           turns into `(y = 0, (T)0)`, dropping the re-read.  The ROM
           re-reads gBg3ScrollY, so gBg2ScrollY is vs32 in this file. */
        gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0;
    }
    else
    {
        AirGrindBuildCourse(gSubGameLevel, 1);
        sub_080c1f88();
    }
    LinkRequestSync();
    LinkSyncRandom();
    SubGameRunFrame();
    LinkStartKeyExchange();
    SubGameSetDisplayLayers(a0);
    if (gUnk_02007FCC != 2)
    {
        BeginFastFadeInFromWhite();
        while (gFadeSteps != 0)
            SubGameRunLinkFrame();
        goto wait;
        /* The ROM places this call between the two arms: a labelled
           block reached from the phase test below (lesson 4.67). */
    de8:
        SubGameDimAndHalt();
        goto tail;
    }
    else
    {
        gBldCntTarget1 = 0xBF;
        gDispCnt &= 0xFF7F;
        for (i = 16; i >= 0; i--)
        {
            gBldY = i;
            SubGameRunLinkFrame();
        }
    }
wait:
    gFadeBlankAtWhite = 0;
    /* goto loop, not do/while: the ROM re-loads the cell's address every
       iteration, and a loop note would hoist it (lesson 3.21). */
loop:
    SubGameRunLinkFrame();
    if (gSubGamePhase <= 1)
        goto loop;
    if (gSubGamePhase == 4 && gPrevGameState == GAME_STATE_MAIN_MENU)
    {
        if (gLinkSetupMode == 2)
            goto de8;
        SubGameSyncLink();
    }
tail:
    LinkStopKeyExchange();
    if (gUnk_02007FCC != 2)
    {
        BeginFastFadeOutToWhite();
        while (gFadeSteps != 0)
            SubGameRunLinkFrame();
    }
    else
    {
        gBldCntTarget1 = 0xBF;
        for (i = 0; i <= 16; i++)
        {
            gBldY = i;
            SubGameRunLinkFrame();
        }
        ResetFadeAndBlend();
        gDispCnt |= 0x80;
        EndFrame();
    }
    gFadeBlankAtWhite = 0;
    gFrameCallback = gVBlankCallback = 0;
    REG_DMA0CNT_L = REG_DMA0CNT_H = 0;
    TaskSetEntry(Task_SubGame, gSubGameTaskIdx);
}

void SubGameRunLinkFrame(void)
{
    RunLinkFrame();
    if (gUnk_02007FCC == 2)
        sub_080c1f88();
}

void SubGameRunFrame(void)
{
    RunFrame();
    if (gUnk_02007FCC == 2)
        sub_080c1f88();
}

void SubGameSyncLink(void)
{
    s32 n;
    u32 stall;
    s32 timer;
    s32 i;
    u32 old;

    if (gLinkIsMaster != 0)
        gLinkCommand = 0x7755;
    else
        gLinkCommand = 0x9900;
    if (gLinkPlayerCount <= 1)
        return;
    n = 0;
    stall = 0;
    timer = 0;     /* nothing ever sets it non-zero, but the ROM keeps it */
    for (;;)
    {
        /* gLinkCommand is NOT volatile here: a vu16 switch operand costs a
           register copy the ROM does not have, and the cell is re-read every
           iteration anyway because the loop calls out. */
        switch (gLinkCommand)
        {
        case 0x7755:
            gSendCmd[0] = 0x7755;
            gLinkCommand = 0xAA00;
            break;
        case 0xAA00:
            timer = 0;
            gSendCmd[0] = 0xAA00;
            gLinkCommand = 0x9900;
            break;
        case 0xAA01:
            gSendCmd[0] = 0xAA01;
            gLinkCommand = 0x9900;
            break;
        case 0xAA02:
            gSendCmd[0] = 0xAA02;
            break;
        }
        old = gSerialIntrCount;
        RunFrame();
        LinkMain1(gShouldAdvanceLinkState, gSendCmd, gRecvCmds);
        if (IsLinkError() != 0)
            LinkErrorScreen();
        if (old == gSerialIntrCount && ++stall > 30)
            LinkErrorScreen();
        for (i = 0; i <= 3; i++)
        {
            switch (gRecvCmds[i])
            {
            case 0x7755:    /* empty, but it roots the tree at 0xAA00 (3.42) */
                break;
            case 0xAA00:
                gLinkCommand = 0xAA01;
                break;
            case 0xAA01:
                if (gLinkIsMaster != 0 && ++n >= gLinkPlayerCount)
                    gLinkCommand = 0xAA02;
                break;
            case 0xAA02:
                goto done;
            }
        }
        if (timer != 0)
        {
            if (n == gLinkPlayerCount)
                timer = 0;
            else if (--timer == 0)
            {
                gLinkCommand = 0xAA00;
                n = 0;
            }
        }
        if (gUnk_02007FCC == 2)
            sub_080c1f88();
    }
done:
    if (gLinkIsMaster != 0)
        gLinkDriverMode = 0;
    for (i = 4; i >= 0; i--)
        SubGameRunFrame();
    DisableSerial();
    gLinkStatus = 0;
}

s32 FreeOtherTasks(void)
{
    s32 i;

    for (i = 0; i <= 62; i++)
    {
        if (gTaskSlotTypes[i] != -1 && i != gCurTaskIdx)
            TaskFree(i);
    }
}

void SubGameMain(void)
{
    s32 i;

    ResetTasksAndOam();
    if (gLinkSetupMode == 2 && ConnectLink() != 0)
        return;
    RunFrameNoTasks();
    RunFrameNoTasks();
    if (gLocalPlayer == 0)
    {
        for (i = 0; i < (gVBlankCount & 0xFF); i++)
            Random();
    }
    LinkRequestSync();
    LinkSyncRandom();
    gSubGameTaskIdx = TaskCreateFrom(TASK_SUB_GAME, 63);
    SubGameInit();
    SubGameRunScreen(0);
    SubGameRunScreen(1);
    ResetTasksAndOam();
    if (gSubGamePhase != 3)
    {
        gGameState = gPrevGameState;
        gPrevGameState = gUnk_02007FCC + 14;
    }
}

void Task_SubGame(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)SubGameStartBody;
    t->subGameNextPhase = 0;
    FreeOtherTasks();
    TaskSleepForever();
}

void SubGameStartBody(void)
{
    TaskSetEntry(gSubGameBodies[gUnk_02007FCC], gCurTaskIdx);
}

void QuickDrawInit(void)
{
    struct Task *t = &gTasks[gSubGameTaskIdx];
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        gQuickDrawWins[i] = 0;
        gQuickDrawRanking[i] = i;
    }
    gQuickDrawMatchWinner = 0xFF;
    gQuickDrawBestTime = 99;
    gUnk_0200B048 = 0;
    t->quickDrawWinsToWin = 3;
    RequestCopy(2, (u32)gUnk_087562E4, (u32)gBgPalette, 2);
}

void QuickDrawMain(void)
{
    gCurTask->updateCallback = 0;
    CallTableEntry(gSubGamePhase, 2, gQuickDrawPhases);
    TaskSleepForever();
}

void QuickDrawFreeze(void)
{
    struct Task *t;

    TaskSetOthersSkipMask((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE | TASK_SKIP_LATE_UPDATE | TASK_SKIP_DRAW), gCurTaskIdx);
    if (gTaskSlotTypes[62] != -1)
        TaskFree(62);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1000;
    t = &gTasks[gCurTask->quickDrawTimerSlot];
    t->quickDrawObjectRunning = 0;
}

void CreateQuickDrawTimer(void)
{
    s32 quickDrawObjectSlot = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (quickDrawObjectSlot != -1)
    {
        struct Task *t = &gTasks[quickDrawObjectSlot];

        t->parent = gCurTaskIdx;
        t->variant = QUICK_DRAW_OBJECT_VARIANT_TIMER;
        gCurTask->quickDrawTimerSlot = quickDrawObjectSlot;
    }
}

void CreateQuickDrawPlayers(s32 a0)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        s32 quickDrawObjectSlot = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 0);

        if (quickDrawObjectSlot != -1)
        {
            struct Task *quickDrawObject = &gTasks[quickDrawObjectSlot];

            quickDrawObject->parent = gCurTaskIdx;
            quickDrawObject->quickDrawObjectPlayerIndex = quickDrawObjectSlot;
            quickDrawObject->variant = QUICK_DRAW_OBJECT_VARIANT_PLAYER;
            quickDrawObject->quickDrawObjectStartMode = a0;
            quickDrawObject->unk28 = 0;
        }
    }
}

void QuickDrawSetupRound(void)
{
    struct Task *t;

    CreateQuickDrawPlayers(0);
    CreateQuickDrawTimer();
    t = gCurTask;
    t->quickDrawPlayersReady = 0;
    t->hitTimer = 0;
}

void CreateQuickDrawSignal(void)
{
    s32 idx = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 62);

    if (idx != -1)
    {
        struct Task *quickDrawObject = &gTasks[idx];

        quickDrawObject->variant = QUICK_DRAW_OBJECT_VARIANT_LABEL;
        quickDrawObject->quickDrawObjectLabelFrame = 0;
        quickDrawObject->quickDrawObjectLabelKind = 3;
        quickDrawObject->quickDrawObjectLabelLifetime = 16;
        quickDrawObject->quickDrawObjectSubFrame = -1;
        quickDrawObject->pixelX = 120;
        quickDrawObject->pixelY = 88;
        quickDrawObject->quickDrawObjectSubTileWord = 0;
    }
}

void QuickDrawStartTimer(void)
{
    struct Task *t;

    PlaySfx(234);
    StopBgm();
    t = &gTasks[gCurTask->quickDrawTimerSlot];
    t->quickDrawObjectRunning = 1;
}

void QuickDrawWaitForSignal(void)
{
    struct Task *t = gCurTask;

    t->quickDrawFalseStartCount = 0;
    t->quickDrawFalseStartMask = 0;
    TaskYieldTrampoline(RandomRange(gQuickDrawSignalDelayRange[gSubGameLevel]) + gQuickDrawSignalDelayMin[gSubGameLevel]);
    CreateQuickDrawSignal();
}

s32 QuickDrawCountPresses(void)
{
    s32 mask = 0;
    s32 i;
    s32 n;

    for (i = 0, n = 0; i < gPlayerCount; i++)
    {
        if ((gPlayerPressedKeys[i] & 1) && !((gCurTask->quickDrawFalseStartMask >> i) & 1))
        {
            n++;
            mask |= 1 << i;
        }
    }
    gCurTask->quickDrawPressMask = mask;
    return n;
}
