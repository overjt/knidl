/* game_code_and_rodata_080653ec_0806ef5c 0x080BA774-0x080BB528
 * (issue #95, module M35, file 2 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BA774 0x080BB528 src/subgame_ba774.c --newpb
 *
 * The reaction duel's round controller (sub-game 0 of src/subgame_b9d0c.c).
 * Task.unk14 is the requested state and Task.unk15 the running one; each of
 * the seven states is an <entry, per-frame check> pair dispatched through
 * CallTableEntry: 0x087562FC / 0x08756318 in link play (sub_080bace4 /
 * sub_080bab68) and 0x08756334 / 0x08756350 against the computer
 * (sub_080bb23c).  A check re-dispatches as soon as Task.unk14 changes.
 *
 *   state 0  wait for the players, then sub_080ba6b4 waits a random delay
 *            (range from 0x087562F6 / 0x087562F0, row gUnk_02006168)
 *            before the signal; the check sends anyone pressing too early
 *            to sub_080babb0, which moves to state 3 once all have
 *   state 1  the signal is up: sub_080ba708 collects the players that
 *            pressed into the mask Task.unk2C; sub_080bacbc sends a single
 *            presser to state 4 and several to state 5, and sub_080ba774
 *            ends the wait (state 2) once the frame counter Task.unk20
 *            passes 98
 *   2..5     pose the player tasks through sub_080bc740, score, wait
 *   state 6  fade out and back to state 0
 *
 * gTasks[i] is player i's task (task type #94, src/subgame_bc0cc.c),
 * gUnk_0200B03C[] the per-player win counts, gUnk_0200B07C[4] the rank
 * order kept by sub_080ba9a4, and gUnk_02006184 the best reaction time
 * (reset to 99, lowered to Task.unk20 by each winner).  sub_080bac5c and
 * sub_080bb1ec (single player: first to 5) report the match winner in
 * gUnk_02004B5C.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s32 gCurTaskIdx;
extern u8 gUnk_02004B5C;
extern s8 gUnk_02006168;
extern u8 gUnk_02006184;
extern u8 gUnk_0200B03C[];
extern u8 gUnk_0200B048;
extern u8 gUnk_0200B07C[];
extern vu16 gFadeSteps;
extern vu16 gDispCnt;
extern u16 gPlayerCount;
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern vs16 gTaskSlotTypes[];
extern u32 gUnk_087562FC[];
extern u32 gUnk_08756318[];
extern u32 gUnk_08756334[];
extern u32 gUnk_08756350[];

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
void sub_080ba50c(void);
void sub_080ba61c(void);
void sub_080ba688(void);
void sub_080ba6b4(void);
s32 sub_080ba708(void);
void sub_080bc740(s32 a0, u16 a1);
void sub_080bc79c(u16 a0);
void sub_080bd188(s32 a0);
void sub_080bd1d0(s32 a0, u16 a1);

void sub_080ba900(void);
void sub_080baa38(void);
void sub_080ba94c(void);
void sub_080ba978(void);
void sub_080baabc(void);
void sub_080bab68(void);
void sub_080bb074(void);

u8 sub_080ba774(void)
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
    while (gCurTask->unk75 == 0)
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
    o->unk3C = 0;
    o->unk24 = 0;
    t->unk20 = 0;
}

void sub_080ba900(void)
{
    s32 i = TaskCreateFrom(94, 32);

    if (i != -1)
    {
        struct Task *t = &gTasks[i];
        t->unk73 = 1;
        t->unk18 = 0;
        t->unk1C = 1;
        t->unk20 = 120;
        t->unk24 = -1;
        t->unk48 = 88;
        t->unk4A = 24;
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
        t->unk73 = 3;
    }
}

void sub_080ba978(void)
{
    s32 i = TaskCreateFrom(94, 32);
    struct Task *t;

    if (i != -1)
    {
        t = &gTasks[i];
        t->unk73 = 4;
    }
}

void sub_080ba9a4(s32 a0)
{
    u8 buf[4];
    u8 i = 0;
    u8 j = 0;
    s32 done = 0;

    while (j <= 3)
    {
        if (!done && gUnk_0200B03C[gUnk_0200B07C[i]] < gUnk_0200B03C[a0])
        {
            buf[j] = a0;
            done = 1;
            j++;
        }
        else
        {
            if (gUnk_0200B07C[i] != a0 || i == 3)
            {
                buf[j] = gUnk_0200B07C[i];
                j++;
            }
            i++;
        }
    }
    for (j = 0; j <= 3; j++)
        gUnk_0200B07C[j] = buf[j];
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
            sub_080bc740(i, 2);
            o->unk20 = 4;
            gUnk_0200B03C[o->unk18]++;
            sub_080ba9a4(o->unk18);
            if (gUnk_02006184 > gCurTask->unk20)
                gUnk_02006184 = gCurTask->unk20;
        }
        else
        {
            sub_080bc740(i, 3);
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
            sub_080bc740(i, 2);
            o->unk20 = i;
        }
    }
}

void QuickDrawRound(void)
{
    gCurTask->unk04 = (u32)sub_080bab68;
    sub_080ba61c();
    if (gPlayerCount != 1)
    {
        gCurTask->unk14 = 0;
        CallTableEntry(gCurTask->unk14, 7, gUnk_087562FC);
    }
    else
    {
        sub_080bb074();
        gCurTask->unk14 = 0;
        CallTableEntry(gCurTask->unk14, 7, gUnk_08756334);
    }
    TaskSleepForever();
}

void sub_080bab68(void)
{
    if (gPlayerCount != 1)
        CallTableEntry(gCurTask->unk15, 7, gUnk_08756318);
    else
        CallTableEntry(gCurTask->unk15, 7, gUnk_08756350);
    SubGameCheckEnd();
}

void sub_080babb0(s32 a0)
{
    s32 i;
    struct Task *t;

    for (i = 0; i < gPlayerCount; i++)
    {
        struct Task *o = &gTasks[i];
        s32 bit = 1 << i;
        if (gCurTask->unk2C & bit)
        {
            sub_080bc740(i, 4);
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
        t->unk14 = 3;
        if (gTaskSlotTypes[62] != -1)
            TaskFree(62);
    }
}

u8 sub_080bac5c(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gUnk_0200B03C[i] >= gCurTask->unk34)
        {
            gUnk_02004B5C = i;
            found = 1;
            break;
        }
    }
    return found;
}

void sub_080bacbc(s32 a0)
{
    if (a0 == 1)
        gCurTask->unk14 = 4;
    else
        gCurTask->unk14 = 5;
    sub_080ba50c();
}

void sub_080bace4(void)
{
    CallTableEntry(gCurTask->unk14, 7, gUnk_087562FC);
}

void sub_080bad00(void)
{
    gCurTask->unk15 = 0;
    PlayBgm(0x823);
    while (gCurTask->unk24 == 0)
        TaskYieldTrampoline(1);
    sub_080ba6b4();
    gCurTask->unk14 = 1;
    TaskSleepForever();
}

void sub_080bad44(void)
{
    if (gCurTask->unk24 != 0)
    {
        s32 r = sub_080ba708();
        if (r != 0)
            sub_080babb0(r);
        if (gCurTask->unk14 != 0)
            TaskSetEntry(sub_080bace4, gCurTaskIdx);
    }
}

void sub_080bad80(void)
{
    gCurTask->unk15 = 1;
    sub_080ba688();
    TaskSleepForever();
}

void sub_080bad9c(void)
{
    s32 r = sub_080ba708();

    if (r != 0)
        sub_080bacbc(r);
    else if (sub_080ba774())
        gCurTask->unk14 = 2;
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080bace4, gCurTaskIdx);
}

void sub_080bade4(void)
{
    gCurTask->unk15 = 2;
    sub_080ba78c();
    TaskYieldTrampoline(8);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080bae0c(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080bace4, gCurTaskIdx);
}

void sub_080bae34(void)
{
    gCurTask->unk15 = 3;
    sub_080ba7b0();
    TaskYieldTrampoline(8);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080bae5c(void)
{
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_080bace4, gCurTaskIdx);
}

void sub_080bae84(void)
{
    gCurTask->unk15 = 4;
    sub_080ba7fc(1);
    TaskYieldTrampoline(80);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080baeb0(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 4 && t->unk18 != 2)
    {
        if (sub_080bac5c())
            gCurTask->unk18 = 2;
        else
            TaskSetEntry(sub_080bace4, gCurTaskIdx);
    }
}

void sub_080baef0(void)
{
    gCurTask->unk15 = 5;
    sub_080ba860();
    TaskYieldTrampoline(8);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080baf18(void)
{
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_080bace4, gCurTaskIdx);
}

void sub_080baf40(void)
{
    gCurTask->unk15 = 6;
    BeginFastFadeOutToWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    sub_080ba8cc();
    BeginFastFadeInFromWhite();
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    gCurTask->unk14 = 0;
    TaskSleepForever();
}

void sub_080baf9c(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 6 && t->unk18 != 2)
        TaskSetEntry(sub_080bace4, gCurTaskIdx);
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
            sub_080bc740(i, 4);
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
        t->unk14 = 2;
        if (gTaskSlotTypes[62] != -1)
            TaskFree(62);
    }
}

void sub_080bb074(void)
{
    s32 i;
    struct Task *t;

    sub_080bd188(0);
    i = TaskCreateFrom(94, 32);
    if (i != -1)
    {
        t = &gTasks[i];
        t->unk44 = gCurTaskIdx;
        t->unk73 = 5;
        t->unk74 = gUnk_02006168;
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
            t->unk14 = 4;
        else
            t->unk14 = 5;
    }
    else if (a0 == 1)
    {
        t->unk14 = 3;
    }
}

void sub_080bb120(void)
{
    if (gCurTask->unk1C != 0)
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x400;
        sub_080bd188(gUnk_0200B048);
    }
    sub_080bd1d0(gCurTask->unk46, gCurTask->unk30);
    sub_080ba8cc();
}

void sub_080bb174(void)
{
    sub_080bd1d0(gCurTask->unk46, 3);
    sub_080ba7fc(1);
    gCurTask->unk1C = 1;
}

void sub_080bb19c(void)
{
    sub_080bd1d0(gCurTask->unk46, 2);
    sub_080ba7fc(0);
    gCurTask->unk1C = 0;
}

void sub_080bb1c4(void)
{
    sub_080bd1d0(gCurTask->unk46, 4);
    sub_080ba860();
    gCurTask->unk1C = 0;
}

u8 sub_080bb1ec(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gUnk_0200B03C[i] > 4)
        {
            gUnk_02004B5C = i;
            found = 1;
            break;
        }
    }
    return found;
}

void sub_080bb23c(void)
{
    CallTableEntry(gCurTask->unk14, 7, gUnk_08756334);
}

void sub_080bb258(void)
{
    gCurTask->unk15 = 0;
    PlayBgm(0x823);
    while (gCurTask->unk24 == 0)
        TaskYieldTrampoline(1);
    sub_080ba6b4();
    gCurTask->unk14 = 1;
    TaskSleepForever();
}

void sub_080bb29c(void)
{
    if (gCurTask->unk24 != 0)
    {
        s32 r = sub_080ba708();
        if (r != 0)
            sub_080bafc8(r);
        if (gCurTask->unk14 != 0)
            TaskSetEntry(sub_080bb23c, gCurTaskIdx);
    }
}

void sub_080bb2d8(void)
{
    gCurTask->unk15 = 1;
    sub_080ba688();
    TaskSleepForever();
}

void sub_080bb2f4(void)
{
    sub_080bb0d8(sub_080ba708());
    if (gCurTask->unk14 != 1)
    {
        sub_080ba50c();
        TaskSetEntry(sub_080bb23c, gCurTaskIdx);
    }
}

void sub_080bb328(void)
{
    gCurTask->unk15 = 2;
    sub_080ba7b0();
    gCurTask->unk1C = 0;
    TaskYieldTrampoline(8);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080bb358(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080bb23c, gCurTaskIdx);
}

void sub_080bb380(void)
{
    gCurTask->unk15 = 3;
    sub_080bb174();
    TaskYieldTrampoline(80);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080bb3a8(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 3 && t->unk18 != 2)
    {
        if (sub_080bb1ec())
            gCurTask->unk18 = 2;
        else
            TaskSetEntry(sub_080bb23c, gCurTaskIdx);
    }
}

void sub_080bb3e8(void)
{
    gCurTask->unk15 = 4;
    sub_080bb19c();
    TaskYieldTrampoline(80);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080bb410(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 4 && t->unk18 != 2)
        t->unk18 = 2;
}

void sub_080bb42c(void)
{
    gCurTask->unk15 = 5;
    sub_080bb1c4();
    TaskYieldTrampoline(8);
    gCurTask->unk14 = 6;
    TaskSleepForever();
}

void sub_080bb454(void)
{
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_080bb23c, gCurTaskIdx);
}

void sub_080bb47c(void)
{
    gCurTask->unk15 = 6;
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
    gCurTask->unk14 = 0;
    TaskSleepForever();
}

void sub_080bb4fc(void)
{
    struct Task *t = gCurTask;

    if (t->unk14 != 6 && t->unk18 != 2)
        TaskSetEntry(sub_080bb23c, gCurTaskIdx);
}
