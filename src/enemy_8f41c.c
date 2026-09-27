/* game_code_and_rodata 0x0808F41C-0x0809000C (issue #70, module M24 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808F41C 0x0809000C src/enemy_8f41c.c --newpb
 *
 * See src/enemy_8cce8.c for the three-table pattern all of M24 is built on.
 *
 * This batch holds:
 *   * script 4's bodies and guards (tables `0x0874329C`/`0x087432A8`,
 *     `0x087432B4`/`0x087432C0`, `0x087432CC`/`0x087432D8` and the
 *     single-row `0x087432E4`/`0x087432E8`); the bodies walk the `s16[][4]`
 *     aim table `gUnk_0874325A` one row per Task.unk34 / Task.unk73 through
 *     `sub_0808eec4`;
 *   * the class-3 hook row `0x08743518` — `sub_0808f9b8`, `sub_0808f978`,
 *     `sub_0808f9d8` and `sub_0808f9f8`;
 *   * script 5: entry `sub_0808fa10` (`0x087432F4`, 2 rows), rows
 *     `sub_0808fa50` / `sub_0808fbac`, bodies `0x087432FC` (3) and
 *     `0x08743308` (1);
 *   * script 6: entry `sub_0808fc40` (`0x08743600`, 1 row), row
 *     `sub_0808fc90`, bodies `0x08743604` (2), guards `0x0874360C` (2);
 *   * script 7: entry `sub_0808fdb8` (`0x0874362C`, 4 identical rows), row
 *     `sub_0808fdf8`, body `0x0874363C` (`sub_0808fe88`, the class-2 wanderer
 *     that picks its heading from gTasks[Task.unk44].unk34) and guard
 *     `0x08743640` (`sub_0808ffe0`).
 *
 * `sub_0808fa04` and `sub_0808fe6c` are dead exports (twins of
 * `sub_0808f9f8` and of `sub_0808fdf8`'s cue call) that no ROM word points at;
 * both are curated in tools/symdb.py.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u8 gUnk_08743248[];
extern s16 gUnk_0874325A[][4];
extern u32 gUnk_087432E4[];
extern u32 gUnk_087432E8[];
extern u8 gUnk_087432EC[][4];
extern u32 gUnk_087432F4[];
extern u32 gUnk_087432FC[];
extern u32 gUnk_08743308[];
extern u32 gUnk_08743600[];
extern u32 gUnk_08743604[];
extern u32 gUnk_0874360C[];
extern u8 gUnk_08743614[];
extern s16 gUnk_0874361A[];
extern u32 gUnk_0874362C[];
extern u32 gUnk_0874363C[];
extern u32 gUnk_08743640[];
extern u32 gUnk_0875227C[];
extern u32 gUnk_08752520[];
extern u32 gUnk_08752A70[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern s32 PlaySfx(s32 id);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void TaskSetFrameNoFlip(s32 a);
extern void TaskSetFrameFlip(s32 a);
extern void sub_080224b0(void);
extern void sub_0806395c(u16 v);
extern void sub_080639b4(void *p);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_080640c8(void);
extern void sub_0806421c(s32 a, s32 b);
extern void sub_08064a60(void);
extern void sub_0806523c(void);
extern void sub_08066ba8(void);
extern void sub_08066bdc(void);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_08069888(void);
extern u32 sub_08069b44(void);
extern void sub_0806a344(void);
extern void sub_0806ee2c(void);
extern void sub_0808eec4(s32 a);
extern void sub_0808efdc(void);
extern void sub_0808f058(void);
extern void sub_0808f0d0(void);
extern void sub_0808f1b4(u16 a, void *b);
extern void sub_0808f380(void);
extern void sub_0808f39c(void);
extern void sub_0808f3b8(void);

/* Forward declarations */
void sub_0808f41c(void);
void sub_0808f4b4(void);
void sub_0808f4f8(void);
void sub_0808f51c(void);
void sub_0808f528(void);
void sub_0808f578(void);
void sub_0808f5cc(void);
void sub_0808f678(void);
void sub_0808f6c0(void);
void sub_0808f71c(void);
void sub_0808f728(void);
void sub_0808f75c(void);
void sub_0808f7ac(void);
void sub_0808f844(void);
void sub_0808f888(void);
void sub_0808f8dc(void);
void sub_0808f8e8(void);
void sub_0808f930(void);
void sub_0808f954(void);
void sub_0808f974(void);
s32 sub_0808f978(void);
s32 sub_0808f9b8(void);
s32 sub_0808f9d8(void);
s32 sub_0808f9f8(void);
s32 sub_0808fa04(void);
void sub_0808fa10(void);
void sub_0808fa50(void);
void sub_0808fa84(void);
void sub_0808fa98(void);
void sub_0808fab4(void);
void sub_0808fb50(void);
void sub_0808fb80(void);
void sub_0808fbac(void);
void sub_0808fbf0(void);
void sub_0808fc00(void);
void sub_0808fc40(void);
void sub_0808fc90(void);
void sub_0808fcd4(void);
void sub_0808fd1c(void);
void sub_0808fd38(void);
void sub_0808fd5c(void);
void sub_0808fd60(void);
void sub_0808fdb4(void);
void sub_0808fdb8(void);
void sub_0808fdf8(void);
void sub_0808fe28(void);
void sub_0808fe6c(void);
void sub_0808fe88(void);
void sub_0808ffe0(void);

void sub_0808f41c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->unk15 = 2;
    sub_08063e14();
    t = gCurTask;
    t->unk28 = 0;
    t->unk70 = 0;
    PlaySfx(190);
    sub_0808efdc();
    u = gCurTask;
    u->unk6E = 0;
    u->unk6C = 0;
    do
    {
        v = gCurTask;
        sub_0808eec4(gUnk_0874325A[v->unk34][v->unk6E]);
        w = gCurTask;
        w->unk6E++;
        TaskYieldTrampoline(2);
        TaskStop();
        x = gCurTask;
        x->unk6C++;
    } while ((s16)x->unk6C <= 3);
    y = gCurTask;
    y->unk70 = 1;
    y->unk28 = 1;
    y->unk2C = 0;
    TaskSleepForever();
}

void sub_0808f4b4(void)
{
    if ((s16)gCurTask->unk70 != 0 && sub_08069888() == 0 && gCurTask->unk28 != 0)
    {
        sub_0806395c(0);
        TaskSetEntry(sub_0808f380, gCurTaskIdx);
    }
}

void sub_0808f4f8(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    t = gCurTask;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    TaskSleepForever();
}

void sub_0808f51c(void)
{
    sub_08069888();
}

void sub_0808f528(void)
{
    gCurTask->unk15 = 0;
    TaskStopY();
    switch (gCurTask->unk73)
    {
    case 1:
        TaskSetFrameNoFlip(7);
        break;
    case 2:
        TaskSetFrameNoFlip(5);
        break;
    case 3:
        TaskSetFrameFlip(5);
        break;
    }
    TaskSleepForever();
}

void sub_0808f578(void)
{
    struct Task *t;

    if (sub_08069888() == 0)
    {
        t = gCurTask;
        switch (t->unk73)
        {
        case 1:
            t->unk34 = 2;
            break;
        case 2:
            t->unk34 = 1;
            break;
        case 3:
            t->unk34 = 3;
            break;
        }
        sub_0806395c(2);
        TaskSetEntry(sub_0808f39c, gCurTaskIdx);
    }
}

void sub_0808f5cc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->unk15 = 2;
    t = gCurTask;
    t->unk28 = 0;
    t->unk70 = 1;
    while (1)
    {
        TaskYieldTrampoline(100);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk70 = 0;
            PlaySfx(190);
            sub_0808f058();
            u = gCurTask;
            u->unk2C = 0;
            u->unk6E = 0;
            do
            {
                v = gCurTask;
                sub_0808eec4(gUnk_0874325A[v->unk73][v->unk2C]);
                w = gCurTask;
                w->unk2C++;
                TaskYieldTrampoline(2);
                TaskStop();
                x = gCurTask;
                x->unk6E++;
            } while ((s16)x->unk6E <= 3);
            gCurTask->unk70 = 1;
            TaskYieldTrampoline(7);
            y = gCurTask;
            y->unk6C++;
        } while ((s16)y->unk6C <= 2);
        gCurTask->unk28 = 1;
    }
}

void sub_0808f678(void)
{
    if ((s16)gCurTask->unk70 != 0 && sub_08069888() == 0 && gCurTask->unk28 != 0)
    {
        TaskStop();
        sub_0806395c(2);
        TaskSetEntry(sub_0808f39c, gCurTaskIdx);
    }
}

void sub_0808f6c0(void)
{
    struct Task *t;

    switch (gCurTask->unk73)
    {
    case 1:
        TaskSetFrameNoFlip(7);
        break;
    case 2:
        TaskSetFrameNoFlip(5);
        break;
    case 3:
        TaskSetFrameFlip(5);
        break;
    }
    gCurTask->unk15 = 1;
    t = gCurTask;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    TaskSleepForever();
}

void sub_0808f71c(void)
{
    sub_08069888();
}

void sub_0808f728(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    gCurTask->unk74 = 1;
    t = gCurTask;
    t->unk28 = gUnk_08743248[t->unk74];
    TaskStop();
    while (1)
        sub_0808f0d0();
}

void sub_0808f75c(void)
{
    if (gCurTask->unk8C->unk1A == -1)
    {
        if (sub_08069888() == 0)
            sub_0808f1b4(1, sub_0808f3b8);
    }
    else if (sub_080692fc() == 0)
        sub_0808f1b4(1, sub_0808f3b8);
}

void sub_0808f7ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gCurTask->unk15 = 1;
    sub_08063e14();
    t = gCurTask;
    t->unk28 = 0;
    t->unk70 = 0;
    PlaySfx(190);
    sub_0808efdc();
    u = gCurTask;
    u->unk6E = 0;
    u->unk6C = 0;
    do
    {
        v = gCurTask;
        sub_0808eec4(gUnk_0874325A[v->unk34][v->unk6E]);
        w = gCurTask;
        w->unk6E++;
        TaskYieldTrampoline(2);
        TaskStop();
        x = gCurTask;
        x->unk6C++;
    } while ((s16)x->unk6C <= 3);
    y = gCurTask;
    y->unk70 = 1;
    y->unk28 = 1;
    y->unk2C = 0;
    TaskSleepForever();
}

void sub_0808f844(void)
{
    if ((s16)gCurTask->unk70 != 0 && sub_08069888() == 0 && gCurTask->unk28 != 0)
    {
        sub_0806395c(0);
        TaskSetEntry(sub_0808f3b8, gCurTaskIdx);
    }
}

void sub_0808f888(void)
{
    struct Task *t;

    gCurTask->unk15 = 2;
    if (gCurTask->unk8C->unk1A == -1)
    {
        sub_080640c8();
        TaskStop();
        t = gCurTask;
        t->unk60 = 168 << 5;
        t->unk68 = 192 << 10;
        TaskSleepForever();
    }
    else
    {
        sub_08066ba8();
        while (1)
        {
            sub_08066bdc();
            TaskYieldTrampoline(8);
        }
    }
}

void sub_0808f8dc(void)
{
    sub_080692fc();
}

void sub_0808f8e8(void)
{
    gCurTask->unk04 = (u32)sub_0808f930;
    sub_080639b4(gUnk_0873F500);
    gCurTask->unk78 = 2;
    sub_0806395c(0);
    gCurTask->unk43 = 255;
    CallTableEntry(gCurTask->unk14, 1, gUnk_087432E4);
}

void sub_0808f930(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_087432E8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808f954(void)
{
    gCurTask->unk15 = 0;
    TaskStop();
    TaskSetFrameNoFlip(6);
    TaskSleepForever();
}

void sub_0808f974(void)
{
}

s32 sub_0808f978(void)
{
    sub_080224b0();
    if (gCurTask->unk7B == 3)
    {
        sub_0806395c(2);
        TaskSetEntry(sub_0808fa98, gCurTaskIdx);
        return 1;
    }
    else
    {
        sub_0806395c(1);
        TaskSetEntry(sub_0808fa98, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808f9b8(void)
{
    sub_0806395c(0);
    TaskSetEntry(sub_0808fa98, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9d8(void)
{
    sub_0806395c(2);
    TaskSetEntry(sub_0808fa98, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9f8(void)
{
    sub_08063ff4();
    return 0;
}

s32 sub_0808fa04(void)
{
    sub_08063ff4();
    return 0;
}

void sub_0808fa10(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gCurTask;
    u->unk38 = gUnk_08752A70;
    CallTableEntry(u->unk73, 2, gUnk_087432F4);
}

void sub_0808fa50(void)
{
    gCurTask->unk04 = (u32)sub_0808fa84;
    sub_08063e14();
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087432FC);
}

void sub_0808fa84(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fa98(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087432FC);
}

void sub_0808fab4(void)
{
    TaskStop();
    while (1)
    {
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][0]);
        TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][1]);
        TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][2]);
        gCurTask->unk54 = 0;
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_087432EC[gCurTask->unk74][3]);
    }
}

void sub_0808fb50(void)
{
    sub_080224b0();
    gCurTask->unk60 = 128 << 5;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_0808fb80(void)
{
    TaskStopY();
    gCurTask->unk58 = 128 << 7;
    TaskSetMotionXFacing(128 << 7, 0x5A5A5A5A);
    TaskSetFrame(6);
    TaskSleepForever();
}

void sub_0808fbac(void)
{
    gCurTask->unk04 = (u32)sub_0808fbf0;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gCurTask->unk78 = 2;
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08743308);
}

void sub_0808fbf0(void)
{
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fc00(void)
{
    TaskStop();
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(gUnk_087432EC[0][0]);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[0][1]);
        TaskSetFrame(6);
        TaskYieldTrampoline(gUnk_087432EC[0][2]);
        TaskSetFrame(5);
        TaskYieldTrampoline(gUnk_087432EC[0][3]);
    }
}

void sub_0808fc40(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gCurTask->unk38 = gUnk_0875227C;
    sub_08064a60();
    gCurTask->unk7A = 0;
    u = gCurTask;
    u->unk28 = 0;
    CallTableEntry(u->unk73, 1, gUnk_08743600);
}

void sub_0808fc90(void)
{
    gCurTask->unk04 = (u32)sub_0808fcd4;
    PlaySfx(165);
    TaskSetMotionXFacing(128 << 12, 0x5A5A5A5A);
    sub_0806395c(1);
    CallTableEntry(gCurTask->unk14, 2, gUnk_08743604);
}

void sub_0808fcd4(void)
{
    if (sub_08069604() == 0)
        CallTableEntry(gCurTask->unk15, 2, gUnk_0874360C);
    else
    {
        sub_0806395c(0);
        TaskSetEntry(sub_0808fd1c, gCurTaskIdx);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fd1c(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08743604);
}

void sub_0808fd38(void)
{
    gCurTask->unk15 = 1;
    gCurTask->unk7A = 0;
    gCurTask->unk3C = 0;
    TaskSleepForever();
}

void sub_0808fd5c(void)
{
}

void sub_0808fd60(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    gCurTask->unk04 = 0;
    TaskStop();
    t = gCurTask;
    t->unk48 += t->unk43 * 16;
    t->unk4C = t->unk48 << 16;
    t->unk3C = 1;
    TaskYieldTrampoline(2);
    sub_08063fe0();
    TaskSleepForever();
}

void sub_0808fdb4(void)
{
}

void sub_0808fdb8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gCurTask;
    u->unk38 = gUnk_08752520;
    CallTableEntry(u->unk73, 4, gUnk_0874362C);
}

void sub_0808fdf8(void)
{
    gCurTask->unk04 = (u32)sub_0808fe28;
    sub_0806395c(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_0874363C);
}

void sub_0808fe28(void)
{
    if (sub_08069660() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_08743640);
    else
        TaskSetEntry(sub_0806a344, gCurTaskIdx);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fe6c(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_0874363C);
}

void sub_0808fe88(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    t = gCurTask;
    t->unk28 = gUnk_08743614[t->unk74];
    switch (t->unk30 = (&gTasks[t->unk44])->unk34)
    {
    case 0:
        sub_0806421c(0, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 1:
        sub_0806421c(224 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 2:
        sub_0806421c(192 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 3:
        sub_0806421c(160 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    case 4:
        sub_0806421c(128 << 1, gUnk_0874361A[gCurTask->unk74]);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    u = gCurTask;
    u->unk54 = gUnk_030023B4;
    u->unk58 = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
    }
}

void sub_0808ffe0(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->unk28 < 0)
        TaskSetEntry(sub_0806a344, gCurTaskIdx);
}

