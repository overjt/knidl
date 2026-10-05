/* game_code_and_rodata_080653ec_0806ef5c 0x080BA774-0x080BB528
 * (issue #95, module M35, file 2 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BA774 0x080BB528 src/subgame_quick_draw.c --newpb
 *
 * The reaction duel's round controller (sub-game 0 of src/subgame.c).
 * Task.state is the requested state and Task.updateState the running one; each of
 * the seven states is an <entry, per-frame check> pair dispatched through
 * CallTableEntry: 0x087562FC / 0x08756318 in link play (QuickDrawEnterState /
 * QuickDrawRoundUpdate) and 0x08756334 / 0x08756350 against the computer
 * (QuickDrawEnterStateVsCpu).  A check re-dispatches as soon as Task.state changes.
 *
 *   state 0  wait for the players, then QuickDrawWaitForSignal waits a random delay
 *            (range from 0x087562F6 / 0x087562F0, row gSubGameLevel)
 *            before the signal; the check sends anyone pressing too early
 *            to QuickDrawFalseStart, which moves to state 3 once all have
 *   state 1  the signal is up: QuickDrawCountPresses collects the players that
 *            pressed into the mask Task.unk2C; QuickDrawDecideRound sends a single
 *            presser to state 4 and several to state 5, and QuickDrawIsTimeUp
 *            ends the wait (state 2) once the frame counter Task.unk20
 *            passes 98
 *   2..5     pose the player tasks through QuickDrawSetPlayerState, score, wait
 *   state 6  fade out and back to state 0
 *
 * gTasks[i] is player i's task (task type #94, src/subgame_quick_draw_objects.c),
 * gQuickDrawWins[] the per-player win counts, gQuickDrawRanking[4] the rank
 * order kept by QuickDrawUpdateRanking, and gQuickDrawBestTime the best reaction time
 * (reset to 99, lowered to Task.unk20 by each winner).  QuickDrawFindMatchWinner and
 * QuickDrawFindMatchWinnerVsCpu (single player: first to 5) report the match winner in
 * gQuickDrawMatchWinner.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "room.h"
#include "subgame.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void CallTableEntry(u32 a, u32 b, u32 *c);
s32 PlaySfx(s32 id);
void TaskSetEntry(void *fn, u32 i);

u8 QuickDrawIsTimeUp(void)
{
    if (gCurTask->quickDrawReactionTime > 98)
        return 1;
    return 0;
}

void QuickDrawEndRoundTimeUp(void)
{
    StopBgm();
    CreateQuickDrawRedrawSign();
    TaskYieldTrampoline(120);
    gCurTask->quickDrawRestartPose = 1;
    gCurTask->quickDrawPlayersReady = 1;
}

void QuickDrawEndRoundFalseStart(void)
{
    while (gCurTask->hitTimer == 0)
        TaskYieldTrampoline(1);
    StopBgm();
    CreateQuickDrawRedrawSign();
    TaskYieldTrampoline(120);
    gCurTask->quickDrawRestartPose = 1;
    gCurTask->quickDrawPlayersReady = 1;
}

void QuickDrawEndRoundWin(u8 a0)
{
    QuickDrawAwardRound();
    CreateQuickDrawSlash();
    TaskYieldTrampoline(12);
    TaskSetOthersSkipMask(0, gCurTaskIdx);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1400;
    CreateQuickDrawBurst();
    if (a0 == 1)
        PlaySfx(253);
    gCurTask->quickDrawRestartPose = 0;
    gCurTask->quickDrawPlayersReady = 0;
}

void QuickDrawEndRoundTie(void)
{
    QuickDrawPoseTiedPlayers();
    CreateQuickDrawSlash();
    TaskYieldTrampoline(12);
    TaskSetOthersSkipMask(0, gCurTaskIdx);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1400;
    PlaySfx(0x101);
    CreateQuickDrawBurst();
    TaskYieldTrampoline(68);
    CreateQuickDrawRedrawSign();
    TaskYieldTrampoline(120);
    gCurTask->quickDrawRestartPose = 0;
    gCurTask->quickDrawPlayersReady = 0;
}

void QuickDrawResetRound(void)
{
    struct Task *t;
    struct Task *o;

    QuickDrawSetAllPlayersState(gCurTask->quickDrawRestartPose);
    t = gCurTask;
    o = &gTasks[t->quickDrawTimerSlot];
    o->quickDrawObjectRunning = 0;
    o->quickDrawObjectCount = 0;
    o->frame = 0;
    o->quickDrawObjectSubFrame = 0;
    t->quickDrawReactionTime = 0;
}

void CreateQuickDrawRedrawSign(void)
{
    s32 i = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (i != -1)
    {
        struct Task *quickDrawObject = &gTasks[i];
        quickDrawObject->variant = QUICK_DRAW_OBJECT_VARIANT_LABEL;
        quickDrawObject->quickDrawObjectLabelFrame = 0;
        quickDrawObject->quickDrawObjectLabelKind = 1;
        quickDrawObject->quickDrawObjectLabelLifetime = 120;
        quickDrawObject->quickDrawObjectSubFrame = -1;
        quickDrawObject->pixelX = 88;
        quickDrawObject->pixelY = 24;
        quickDrawObject->quickDrawObjectSubTileWord = 0;
    }
}

void CreateQuickDrawSlash(void)
{
    s32 i = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gTasks[i];
        t->variant = QUICK_DRAW_OBJECT_VARIANT_SLASH;
    }
}

void CreateQuickDrawBurst(void)
{
    s32 i = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gTasks[i];
        t->variant = QUICK_DRAW_OBJECT_VARIANT_BURST;
    }
}

void QuickDrawUpdateRanking(s32 a0)
{
    u8 buf[4];
    u8 i = 0;
    u8 j = 0;
    s32 done = 0;

    while (j <= 3)
    {
        if (!done && gQuickDrawWins[gQuickDrawRanking[i]] < gQuickDrawWins[a0])
        {
            buf[j] = a0;
            done = 1;
            j++;
        }
        else
        {
            if (gQuickDrawRanking[i] != a0 || i == 3)
            {
                buf[j] = gQuickDrawRanking[i];
                j++;
            }
            i++;
        }
    }
    for (j = 0; j <= 3; j++)
        gQuickDrawRanking[j] = buf[j];
}

void QuickDrawAwardRound(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->quickDrawPressMask & bit)
        {
            QuickDrawSetPlayerState(i, 2);
            o->quickDrawObjectStrikeSpot = 4;
            gQuickDrawWins[o->quickDrawObjectPlayerIndex]++;
            QuickDrawUpdateRanking(o->quickDrawObjectPlayerIndex);
            if (gQuickDrawBestTime > gCurTask->quickDrawReactionTime)
                gQuickDrawBestTime = gCurTask->quickDrawReactionTime;
        }
        else
        {
            QuickDrawSetPlayerState(i, 3);
        }
    }
}

void QuickDrawPoseTiedPlayers(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->quickDrawPressMask & bit)
        {
            QuickDrawSetPlayerState(i, 2);
            o->quickDrawObjectStrikeSpot = i;
        }
    }
}

void QuickDrawRound(void)
{
    gCurTask->updateCallback = (u32)QuickDrawRoundUpdate;
    QuickDrawSetupRound();
    if (gPlayerCount != 1)
    {
        gCurTask->state = 0;
        CallTableEntry(gCurTask->state, 7, gQuickDrawStates);
    }
    else
    {
        CreateQuickDrawOpponent();
        gCurTask->state = 0;
        CallTableEntry(gCurTask->state, 7, gQuickDrawStatesVsCpu);
    }
    TaskSleepForever();
}

void QuickDrawRoundUpdate(void)
{
    if (gPlayerCount != 1)
        CallTableEntry(gCurTask->updateState, 7, gQuickDrawStateUpdates);
    else
        CallTableEntry(gCurTask->updateState, 7, gQuickDrawStateUpdatesVsCpu);
    SubGameCheckEnd();
}

void QuickDrawFalseStart(s32 a0)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->quickDrawPressMask & bit)
        {
            QuickDrawSetPlayerState(i, 4);
            o->unk20 = 4;
            if (a0 != 1)
                o->unk20 = i;
        }
    }
    t = gCurTask;
    t->quickDrawFalseStartCount += a0;
    t->quickDrawFalseStartMask |= t->quickDrawPressMask;
    if (gPlayerCount != 1 && (s16)t->quickDrawFalseStartCount == gPlayerCount)
    {
        t->state = QUICK_DRAW_STATE_ROUND_ALL_FALSE_START;
        if (gTaskSlotTypes[62] != -1)
            TaskFree(62);
    }
}

u8 QuickDrawFindMatchWinner(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gQuickDrawWins[i] >= gCurTask->quickDrawWinsToWin)
        {
            gQuickDrawMatchWinner = i;
            found = 1;
            break;
        }
    }
    return found;
}

void QuickDrawDecideRound(s32 a0)
{
    if (a0 == 1)
        gCurTask->state = QUICK_DRAW_STATE_ROUND_WIN;
    else
        gCurTask->state = QUICK_DRAW_STATE_ROUND_TIE;
    QuickDrawFreeze();
}

void QuickDrawEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gQuickDrawStates);
}

void QuickDrawRoundWait(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_WAIT;
    PlayBgm(0x823);
    while (gCurTask->quickDrawPlayersReady == 0)
        TaskYieldTrampoline(1);
    QuickDrawWaitForSignal();
    gCurTask->state = QUICK_DRAW_STATE_ROUND_SIGNAL;
    TaskSleepForever();
}

void QuickDrawRoundWaitUpdate(void)
{
    if (gCurTask->quickDrawPlayersReady != 0)
    {
        s32 r = QuickDrawCountPresses();
        if (r != 0)
            QuickDrawFalseStart(r);
        if (gCurTask->state != QUICK_DRAW_STATE_ROUND_WAIT)
            TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
    }
}

void QuickDrawRoundSignal(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_SIGNAL;
    QuickDrawStartTimer();
    TaskSleepForever();
}

void QuickDrawRoundSignalUpdate(void)
{
    s32 r = QuickDrawCountPresses();

    if (r != 0)
        QuickDrawDecideRound(r);
    else if (QuickDrawIsTimeUp())
        gCurTask->state = QUICK_DRAW_STATE_ROUND_TIME_UP;
    if (gCurTask->state != QUICK_DRAW_STATE_ROUND_SIGNAL)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundTimeUp(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_TIME_UP;
    QuickDrawEndRoundTimeUp();
    TaskYieldTrampoline(8);
    gCurTask->state = QUICK_DRAW_STATE_ROUND_NEXT;
    TaskSleepForever();
}

void QuickDrawRoundTimeUpUpdate(void)
{
    if (gCurTask->state != QUICK_DRAW_STATE_ROUND_TIME_UP)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundAllFalseStart(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_ALL_FALSE_START;
    QuickDrawEndRoundFalseStart();
    TaskYieldTrampoline(8);
    gCurTask->state = QUICK_DRAW_STATE_ROUND_NEXT;
    TaskSleepForever();
}

void QuickDrawRoundAllFalseStartUpdate(void)
{
    if (gCurTask->state != QUICK_DRAW_STATE_ROUND_ALL_FALSE_START)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundWin(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_WIN;
    QuickDrawEndRoundWin(1);
    TaskYieldTrampoline(80);
    gCurTask->state = QUICK_DRAW_STATE_ROUND_NEXT;
    TaskSleepForever();
}

void QuickDrawRoundWinUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != QUICK_DRAW_STATE_ROUND_WIN && t->subGameNextPhase != 2)
    {
        if (QuickDrawFindMatchWinner())
            gCurTask->subGameNextPhase = 2;
        else
            TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
    }
}

void QuickDrawRoundTie(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_TIE;
    QuickDrawEndRoundTie();
    TaskYieldTrampoline(8);
    gCurTask->state = QUICK_DRAW_STATE_ROUND_NEXT;
    TaskSleepForever();
}

void QuickDrawRoundTieUpdate(void)
{
    if (gCurTask->state != QUICK_DRAW_STATE_ROUND_TIE)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundNext(void)
{
    gCurTask->updateState = QUICK_DRAW_STATE_ROUND_NEXT;
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    QuickDrawResetRound();
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    gCurTask->state = QUICK_DRAW_STATE_ROUND_WAIT;
    TaskSleepForever();
}

void QuickDrawRoundNextUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != QUICK_DRAW_STATE_ROUND_NEXT && t->subGameNextPhase != 2)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawFalseStartVsCpu(s32 a0)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->quickDrawPressMask & bit)
        {
            QuickDrawSetPlayerState(i, 4);
            o->unk20 = 4;
            if (a0 != 1)
                o->unk20 = i;
        }
    }
    t = gCurTask;
    t->quickDrawFalseStartCount += a0;
    t->quickDrawFalseStartMask |= t->quickDrawPressMask;
    if (gPlayerCount != 1 && (s16)t->quickDrawFalseStartCount == gPlayerCount)
    {
        t->state = 2;
        if (gTaskSlotTypes[62] != -1)
            TaskFree(62);
    }
}

void CreateQuickDrawOpponent(void)
{
    s32 quickDrawObjectSlot;
    struct Task *quickDrawObject;

    QuickDrawLoadOpponentGraphics(0);
    quickDrawObjectSlot = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);
    if (quickDrawObjectSlot != -1)
    {
        quickDrawObject = &gTasks[quickDrawObjectSlot];
        quickDrawObject->parent = gCurTaskIdx;
        quickDrawObject->variant = QUICK_DRAW_OBJECT_VARIANT_OPPONENT;
        quickDrawObject->quickDrawObjectLevel = gSubGameLevel;
        quickDrawObject->u76.unk76 = 0;
        gCurTask->quickDrawOpponentSlot = quickDrawObjectSlot;
    }
}

void QuickDrawDecideRoundVsCpu(s32 a0)
{
    struct Task *t = gCurTask;
    struct Task *o = &gTasks[t->quickDrawOpponentSlot];

    if (t->quickDrawReactionTime == o->quickDrawObjectReactionTime)
    {
        if (a0 == 0)
            t->state = 4;
        else
            t->state = 5;
    }
    else if (a0 == 1)
    {
        t->state = 3;
    }
}

void QuickDrawResetRoundVsCpu(void)
{
    if (gCurTask->quickDrawCpuRoundWon != 0)
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x400;
        QuickDrawLoadOpponentGraphics(gUnk_0200B048);
    }
    QuickDrawSetOpponentState(gCurTask->quickDrawOpponentSlot, gCurTask->quickDrawRestartPose);
    QuickDrawResetRound();
}

void QuickDrawEndRoundWinVsCpu(void)
{
    QuickDrawSetOpponentState(gCurTask->quickDrawOpponentSlot, 3);
    QuickDrawEndRoundWin(1);
    gCurTask->quickDrawCpuRoundWon = 1;
}

void QuickDrawEndRoundLoseVsCpu(void)
{
    QuickDrawSetOpponentState(gCurTask->quickDrawOpponentSlot, 2);
    QuickDrawEndRoundWin(0);
    gCurTask->quickDrawCpuRoundWon = 0;
}

void QuickDrawEndRoundTieVsCpu(void)
{
    QuickDrawSetOpponentState(gCurTask->quickDrawOpponentSlot, 4);
    QuickDrawEndRoundTie();
    gCurTask->quickDrawCpuRoundWon = 0;
}

u8 QuickDrawFindMatchWinnerVsCpu(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gQuickDrawWins[i] > 4)
        {
            gQuickDrawMatchWinner = i;
            found = 1;
            break;
        }
    }
    return found;
}

void QuickDrawEnterStateVsCpu(void)
{
    CallTableEntry(gCurTask->state, 7, gQuickDrawStatesVsCpu);
}

void QuickDrawRoundWaitVsCpu(void)
{
    gCurTask->updateState = 0;
    PlayBgm(0x823);
    while (gCurTask->quickDrawPlayersReady == 0)
        TaskYieldTrampoline(1);
    QuickDrawWaitForSignal();
    gCurTask->state = 1;
    TaskSleepForever();
}

void QuickDrawRoundWaitVsCpuUpdate(void)
{
    if (gCurTask->quickDrawPlayersReady != 0)
    {
        s32 r = QuickDrawCountPresses();
        if (r != 0)
            QuickDrawFalseStartVsCpu(r);
        if (gCurTask->state != 0)
            TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
    }
}

void QuickDrawRoundSignalVsCpu(void)
{
    gCurTask->updateState = 1;
    QuickDrawStartTimer();
    TaskSleepForever();
}

void QuickDrawRoundSignalVsCpuUpdate(void)
{
    QuickDrawDecideRoundVsCpu(QuickDrawCountPresses());
    if (gCurTask->state != 1)
    {
        QuickDrawFreeze();
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
    }
}

void QuickDrawRoundFalseStartVsCpu(void)
{
    gCurTask->updateState = 2;
    QuickDrawEndRoundFalseStart();
    gCurTask->quickDrawCpuRoundWon = 0;
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundFalseStartVsCpuUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
}

void QuickDrawRoundWinVsCpu(void)
{
    gCurTask->updateState = 3;
    QuickDrawEndRoundWinVsCpu();
    TaskYieldTrampoline(80);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundWinVsCpuUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 3 && t->subGameNextPhase != 2)
    {
        if (QuickDrawFindMatchWinnerVsCpu())
            gCurTask->subGameNextPhase = 2;
        else
            TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
    }
}

void QuickDrawRoundLoseVsCpu(void)
{
    gCurTask->updateState = 4;
    QuickDrawEndRoundLoseVsCpu();
    TaskYieldTrampoline(80);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundLoseVsCpuUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 4 && t->subGameNextPhase != 2)
        t->subGameNextPhase = 2;
}

void QuickDrawRoundTieVsCpu(void)
{
    gCurTask->updateState = 5;
    QuickDrawEndRoundTieVsCpu();
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundTieVsCpuUpdate(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
}

void QuickDrawRoundNextVsCpu(void)
{
    gCurTask->updateState = 6;
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    QuickDrawResetRoundVsCpu();
    TaskYieldTrampoline(1);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1400;
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    gCurTask->state = 0;
    TaskSleepForever();
}

void QuickDrawRoundNextVsCpuUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 6 && t->subGameNextPhase != 2)
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
}
