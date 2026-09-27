/* game_code_and_rodata_080653ec_0806ef5c 0x080BA774-0x080BB528
 * (issue #95, module M35, file 2 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BA774 0x080BB528 src/subgame_ba774.c --newpb
 *
 * The reaction duel's round controller (sub-game 0 of src/subgame_b9d0c.c).
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
 * gTasks[i] is player i's task (task type #94, src/subgame_bc0cc.c),
 * gQuickDrawWins[] the per-player win counts, gQuickDrawRanking[4] the rank
 * order kept by QuickDrawUpdateRanking, and gQuickDrawBestTime the best reaction time
 * (reset to 99, lowered to Task.unk20 by each winner).  QuickDrawFindMatchWinner and
 * QuickDrawFindMatchWinnerVsCpu (single player: first to 5) report the match winner in
 * gUnk_02004B5C.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "subgame.h"

void TaskYieldTrampoline(u32 frames);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void CallTableEntry(u32 a, u32 b, u32 *c);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);
void StopBgm(void);
void TaskSetOthersSkipMask(u16 val, s32 idx);
void TaskFree(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskSleepForever(void);
void TaskSetEntry(void *fn, u32 i);
void SubGameCheckEnd(void);
void QuickDrawFreeze(void);
void sub_080ba61c(void);
void QuickDrawStartTimer(void);
void QuickDrawWaitForSignal(void);
s32 QuickDrawCountPresses(void);
void QuickDrawSetPlayerState(s32 a0, u16 a1);
void sub_080bc79c(u16 a0);
void QuickDrawLoadOpponentGraphics(s32 a0);
void QuickDrawSetOpponentState(s32 a0, u16 a1);

void sub_080ba900(void);
void sub_080baa38(void);
void sub_080ba94c(void);
void sub_080ba978(void);
void sub_080baabc(void);
void QuickDrawRoundUpdate(void);
void CreateQuickDrawOpponent(void);

u8 QuickDrawIsTimeUp(void)
{
    if (gCurTask->unk20 > 98)
        return 1;
    return 0;
}

void sub_080ba78c(void)
{
    StopBgm();
    sub_080ba900();
    TaskYieldTrampoline(120);
    gCurTask->unk30 = 1;
    gCurTask->unk24 = 1;
}

void sub_080ba7b0(void)
{
    while (gCurTask->hitTimer == 0)
        TaskYieldTrampoline(1);
    StopBgm();
    sub_080ba900();
    TaskYieldTrampoline(120);
    gCurTask->unk30 = 1;
    gCurTask->unk24 = 1;
}

void sub_080ba7fc(u8 a0)
{
    sub_080baa38();
    sub_080ba94c();
    TaskYieldTrampoline(12);
    TaskSetOthersSkipMask(0, gCurTaskIdx);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1400;
    sub_080ba978();
    if (a0 == 1)
        PlaySfx(253);
    gCurTask->unk30 = 0;
    gCurTask->unk24 = 0;
}

void sub_080ba860(void)
{
    sub_080baabc();
    sub_080ba94c();
    TaskYieldTrampoline(12);
    TaskSetOthersSkipMask(0, gCurTaskIdx);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1400;
    PlaySfx(0x101);
    sub_080ba978();
    TaskYieldTrampoline(68);
    sub_080ba900();
    TaskYieldTrampoline(120);
    gCurTask->unk30 = 0;
    gCurTask->unk24 = 0;
}

void sub_080ba8cc(void)
{
    struct Task *t;
    struct Task *o;

    sub_080bc79c(gCurTask->unk30);
    t = gCurTask;
    o = &gTasks[t->unk28];
    o->unk1C = 0;
    o->unk18 = 0;
    o->frame = 0;
    o->unk24 = 0;
    t->unk20 = 0;
}

void sub_080ba900(void)
{
    s32 i = TaskCreateFrom(94, 32);

    if (i != -1)
    {
        struct Task *t = &gTasks[i];
        t->variant = 1;
        t->unk18 = 0;
        t->unk1C = 1;
        t->unk20 = 120;
        t->unk24 = -1;
        t->pixelX = 88;
        t->pixelY = 24;
        t->unk34 = 0;
    }
}

void sub_080ba94c(void)
{
    s32 i = TaskCreateFrom(94, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gTasks[i];
        t->variant = 3;
    }
}

void sub_080ba978(void)
{
    s32 i = TaskCreateFrom(94, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gTasks[i];
        t->variant = 4;
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

void sub_080baa38(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->unk2C & bit)
        {
            QuickDrawSetPlayerState(i, 2);
            o->unk20 = 4;
            gQuickDrawWins[o->unk18]++;
            QuickDrawUpdateRanking(o->unk18);
            if (gQuickDrawBestTime > gCurTask->unk20)
                gQuickDrawBestTime = gCurTask->unk20;
        }
        else
        {
            QuickDrawSetPlayerState(i, 3);
        }
    }
}

void sub_080baabc(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->unk2C & bit)
        {
            QuickDrawSetPlayerState(i, 2);
            o->unk20 = i;
        }
    }
}

void QuickDrawRound(void)
{
    gCurTask->updateCallback = (u32)QuickDrawRoundUpdate;
    sub_080ba61c();
    if (gPlayerCount != 1)
    {
        gCurTask->state = 0;
        CallTableEntry(gCurTask->state, 7, gQuickDrawStates);
    }
    else
    {
        CreateQuickDrawOpponent();
        gCurTask->state = 0;
        CallTableEntry(gCurTask->state, 7, gUnk_08756334);
    }
    TaskSleepForever();
}

void QuickDrawRoundUpdate(void)
{
    if (gPlayerCount != 1)
        CallTableEntry(gCurTask->updateState, 7, gQuickDrawStateUpdates);
    else
        CallTableEntry(gCurTask->updateState, 7, gUnk_08756350);
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
        if (gCurTask->unk2C & bit)
        {
            QuickDrawSetPlayerState(i, 4);
            o->unk20 = 4;
            if (a0 != 1)
                o->unk20 = i;
        }
    }
    t = gCurTask;
    t->unk70 += a0;
    t->unk6E |= t->unk2C;
    if (gPlayerCount != 1 && (s16)t->unk70 == gPlayerCount)
    {
        t->state = 3;
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
        if (gQuickDrawWins[i] >= gCurTask->unk34)
        {
            gUnk_02004B5C = i;
            found = 1;
            break;
        }
    }
    return found;
}

void QuickDrawDecideRound(s32 a0)
{
    if (a0 == 1)
        gCurTask->state = 4;
    else
        gCurTask->state = 5;
    QuickDrawFreeze();
}

void QuickDrawEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gQuickDrawStates);
}

void QuickDrawRoundWait(void)
{
    gCurTask->updateState = 0;
    PlayBgm(0x823);
    while (gCurTask->unk24 == 0)
        TaskYieldTrampoline(1);
    QuickDrawWaitForSignal();
    gCurTask->state = 1;
    TaskSleepForever();
}

void QuickDrawRoundWaitUpdate(void)
{
    if (gCurTask->unk24 != 0)
    {
        s32 r = QuickDrawCountPresses();
        if (r != 0)
            QuickDrawFalseStart(r);
        if (gCurTask->state != 0)
            TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
    }
}

void QuickDrawRoundSignal(void)
{
    gCurTask->updateState = 1;
    QuickDrawStartTimer();
    TaskSleepForever();
}

void QuickDrawRoundSignalUpdate(void)
{
    s32 r = QuickDrawCountPresses();

    if (r != 0)
        QuickDrawDecideRound(r);
    else if (QuickDrawIsTimeUp())
        gCurTask->state = 2;
    if (gCurTask->state != 1)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundTimeUp(void)
{
    gCurTask->updateState = 2;
    sub_080ba78c();
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundTimeUpUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundAllFalseStart(void)
{
    gCurTask->updateState = 3;
    sub_080ba7b0();
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundAllFalseStartUpdate(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundWin(void)
{
    gCurTask->updateState = 4;
    sub_080ba7fc(1);
    TaskYieldTrampoline(80);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundWinUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 4 && t->unk18 != 2)
    {
        if (QuickDrawFindMatchWinner())
            gCurTask->unk18 = 2;
        else
            TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
    }
}

void QuickDrawRoundTie(void)
{
    gCurTask->updateState = 5;
    sub_080ba860();
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawRoundTieUpdate(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void QuickDrawRoundNext(void)
{
    gCurTask->updateState = 6;
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    sub_080ba8cc();
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    gCurTask->state = 0;
    TaskSleepForever();
}

void QuickDrawRoundNextUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->state != 6 && t->unk18 != 2)
        TaskSetEntry(QuickDrawEnterState, gCurTaskIdx);
}

void sub_080bafc8(s32 a0)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->unk2C & bit)
        {
            QuickDrawSetPlayerState(i, 4);
            o->unk20 = 4;
            if (a0 != 1)
                o->unk20 = i;
        }
    }
    t = gCurTask;
    t->unk70 += a0;
    t->unk6E |= t->unk2C;
    if (gPlayerCount != 1 && (s16)t->unk70 == gPlayerCount)
    {
        t->state = 2;
        if (gTaskSlotTypes[62] != -1)
            TaskFree(62);
    }
}

void CreateQuickDrawOpponent(void)
{
    s32 i;
    struct Task *t;

    QuickDrawLoadOpponentGraphics(0);
    i = TaskCreateFrom(94, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        t->parent = gCurTaskIdx;
        t->variant = 5;
        t->unk74 = gSubGameLevel;
        t->unk76 = 0;
        gCurTask->unk46 = i;
    }
}

void sub_080bb0d8(s32 a0)
{
    struct Task *t = gCurTask;
    struct Task *o = &gTasks[t->unk46];

    if (t->unk20 == o->unk1C)
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

void sub_080bb120(void)
{
    if (gCurTask->unk1C != 0)
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x400;
        QuickDrawLoadOpponentGraphics(gUnk_0200B048);
    }
    QuickDrawSetOpponentState(gCurTask->unk46, gCurTask->unk30);
    sub_080ba8cc();
}

void sub_080bb174(void)
{
    QuickDrawSetOpponentState(gCurTask->unk46, 3);
    sub_080ba7fc(1);
    gCurTask->unk1C = 1;
}

void sub_080bb19c(void)
{
    QuickDrawSetOpponentState(gCurTask->unk46, 2);
    sub_080ba7fc(0);
    gCurTask->unk1C = 0;
}

void sub_080bb1c4(void)
{
    QuickDrawSetOpponentState(gCurTask->unk46, 4);
    sub_080ba860();
    gCurTask->unk1C = 0;
}

u8 QuickDrawFindMatchWinnerVsCpu(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gQuickDrawWins[i] > 4)
        {
            gUnk_02004B5C = i;
            found = 1;
            break;
        }
    }
    return found;
}

void QuickDrawEnterStateVsCpu(void)
{
    CallTableEntry(gCurTask->state, 7, gUnk_08756334);
}

void sub_080bb258(void)
{
    gCurTask->updateState = 0;
    PlayBgm(0x823);
    while (gCurTask->unk24 == 0)
        TaskYieldTrampoline(1);
    QuickDrawWaitForSignal();
    gCurTask->state = 1;
    TaskSleepForever();
}

void sub_080bb29c(void)
{
    if (gCurTask->unk24 != 0)
    {
        s32 r = QuickDrawCountPresses();
        if (r != 0)
            sub_080bafc8(r);
        if (gCurTask->state != 0)
            TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
    }
}

void sub_080bb2d8(void)
{
    gCurTask->updateState = 1;
    QuickDrawStartTimer();
    TaskSleepForever();
}

void sub_080bb2f4(void)
{
    sub_080bb0d8(QuickDrawCountPresses());
    if (gCurTask->state != 1)
    {
        QuickDrawFreeze();
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
    }
}

void sub_080bb328(void)
{
    gCurTask->updateState = 2;
    sub_080ba7b0();
    gCurTask->unk1C = 0;
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void sub_080bb358(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
}

void sub_080bb380(void)
{
    gCurTask->updateState = 3;
    sub_080bb174();
    TaskYieldTrampoline(80);
    gCurTask->state = 6;
    TaskSleepForever();
}

void sub_080bb3a8(void)
{
    struct Task *t = gCurTask;

    if (t->state != 3 && t->unk18 != 2)
    {
        if (QuickDrawFindMatchWinnerVsCpu())
            gCurTask->unk18 = 2;
        else
            TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
    }
}

void sub_080bb3e8(void)
{
    gCurTask->updateState = 4;
    sub_080bb19c();
    TaskYieldTrampoline(80);
    gCurTask->state = 6;
    TaskSleepForever();
}

void sub_080bb410(void)
{
    struct Task *t = gCurTask;

    if (t->state != 4 && t->unk18 != 2)
        t->unk18 = 2;
}

void sub_080bb42c(void)
{
    gCurTask->updateState = 5;
    sub_080bb1c4();
    TaskYieldTrampoline(8);
    gCurTask->state = 6;
    TaskSleepForever();
}

void sub_080bb454(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
}

void sub_080bb47c(void)
{
    gCurTask->updateState = 6;
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    sub_080bb120();
    TaskYieldTrampoline(1);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1400;
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    gCurTask->state = 0;
    TaskSleepForever();
}

void sub_080bb4fc(void)
{
    struct Task *t = gCurTask;

    if (t->state != 6 && t->unk18 != 2)
        TaskSetEntry(QuickDrawEnterStateVsCpu, gCurTaskIdx);
}
