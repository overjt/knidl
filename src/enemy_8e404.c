/* game_code_and_rodata 0x0808E404-0x0808F41C (issue #70, module M24 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808E404 0x0808F41C src/enemy_8e404.c --newpb
 *
 * See src/enemy_8cce8.c for the three-table pattern all of M24 is built on.
 *
 * This batch holds:
 *   * script 2's rows `sub_0808e404` / `sub_0808e804` (bodies `0x087431EC`
 *     (4) and `0x0874320C` (1), guards `0x087431FC` (4) and `0x08743210`),
 *     with `sub_0808e480` / `sub_0808e54c` / `sub_0808e610` / `sub_0808e730`
 *     as the bodies and `sub_0808e510` / `sub_0808e5cc` / `sub_0808e704` /
 *     `sub_0808e800` as their guards;
 *   * script 3: entry `sub_0808e8d4` (Task.unk73 -> `0x08743224`, 3 rows),
 *     rows `sub_0808e914` / `sub_0808eb24`, bodies `0x08743230` (3) and
 *     `0x08743240`, guards `0x0874323C` and `0x08743244`;
 *   * the class-3 hook row `0x087434FC` — `sub_0808ec34`, `sub_0808ebe0`,
 *     `sub_0808ecb4` and `sub_0808ec90`, every one returning s32;
 *   * the module's shared aiming library: `sub_0808ed38` classifies the
 *     direction to the target into 16 sectors ((u16)ArcTan2 >> 12) and stores
 *     it in Task.unk30 with the parity in Task.unk20, `sub_0808ee60` turns
 *     Task.unk34 one notch towards it, `sub_0808ee9c` reports arrival in
 *     Task.unk1C, `sub_0808eec4` converts the heading into an aim angle for
 *     AngleToVector, `sub_0808efdc` / `sub_0808f058` fire actor 109 through
 *     CreateActorFromDescAtOffsetFacing + CreateChildTaskAtOffsetFacing, and `sub_0808f0d0` / `sub_0808f1b4` are the
 *     per-frame drivers;
 *   * script 4's entry `sub_0808f224` (Task.unk73 -> `0x08743284`, 6 rows) and
 *     its three <body, guard> table pairs `0x0874329C`/`0x087432A8`,
 *     `0x087432B4`/`0x087432C0` and `0x087432CC`/`0x087432D8`.  Its bodies
 *     continue in src/enemy_8f41c.c.
 *
 * `sub_0808ed0c` is a dead export: a byte-for-byte twin of `sub_0808ece0`
 * that no ROM word points at (lesson 4.30, curated in tools/symdb.py).
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u32 gUnk_087431EC[];
extern u32 gUnk_087431FC[];
extern u32 gUnk_0874320C[];
extern u32 gUnk_08743210[];
extern u32 gUnk_08743214[];
extern u32 gUnk_0874321C[];
extern u32 gUnk_08743224[];
extern u32 gUnk_08743230[];
extern u32 gUnk_0874323C[];
extern u32 gUnk_08743240[];
extern u32 gUnk_08743244[];
extern u8 gUnk_08743248[];
extern s8 gUnk_0874324C[];
extern s8 gUnk_08743251[];
extern s8 gUnk_08743256[];
extern u32 gUnk_08743284[];
extern u32 gUnk_0874329C[];
extern u32 gUnk_087432A8[];
extern u32 gUnk_087432B4[];
extern u32 gUnk_087432C0[];
extern u32 gUnk_087432CC[];
extern u32 gUnk_087432D8[];
extern u32 gUnk_08743390[];
extern u32 gUnk_087433E8[];
extern u32 gUnk_08743558[];
extern u32 gUnk_087524A4[];
extern u32 gUnk_087524E4[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void TaskMove(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskStopY(void);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void TaskSetFrameNoFlip(s32 a);
extern void TaskSetFrameFlip(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(void *p);
extern void ActorSetAttackBox(void *p);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetNearestPlayerDx(void);
extern void TaskFaceNearestPlayer(void);
extern void TaskTurnAround(void);
extern void AngleToVector(s32 a, s32 b);
extern u8 TaskGetXDirBitToNearestPlayer(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_08066b34(u32 *p);
extern void sub_08066b70(void);
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066c3c(u32 *p);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069888(void);
extern u32 ActorReactToHit(void);
extern void sub_0806a0f0(s32 a);
extern void ActorDie(void);
extern void sub_0806ee2c(void);
extern void sub_0808e070(void);
extern void sub_0808e254(void);
extern void sub_0808e2b4(void);
extern void sub_0808e33c(void);
extern void sub_0808e36c(void);

/* Forward declarations */
void sub_0808e404(void);
void sub_0808e440(void);
void sub_0808e464(void);
void sub_0808e480(void);
void sub_0808e510(void);
void sub_0808e54c(void);
void sub_0808e5cc(void);
void sub_0808e610(void);
void sub_0808e704(void);
void sub_0808e730(void);
void sub_0808e800(void);
void sub_0808e804(void);
void sub_0808e848(void);
void sub_0808e870(void);
void sub_0808e8a0(void);
s32 sub_0808e8a4(void);
s32 sub_0808e8c4(void);
void sub_0808e8d4(void);
void sub_0808e914(void);
void sub_0808e964(void);
void sub_0808e994(void);
void sub_0808e9b0(void);
void sub_0808e9d4(void);
void sub_0808ea00(void);
void sub_0808eb10(void);
void sub_0808eb24(void);
void sub_0808eb64(void);
void sub_0808eb94(void);
void sub_0808ebdc(void);
s32 sub_0808ebe0(void);
s32 sub_0808ec34(void);
s32 sub_0808ec90(void);
s32 sub_0808ecb4(void);
s32 sub_0808ece0(void);
s32 sub_0808ed0c(void);
void sub_0808ed38(void);
void sub_0808ee60(void);
void sub_0808ee9c(void);
void sub_0808eec4(s32 a);
void sub_0808ef88(void);
void sub_0808efdc(void);
void sub_0808f058(void);
void sub_0808f0d0(void);
void sub_0808f1b4(u16 a, void *b);
void sub_0808f224(void);
void sub_0808f26c(void);
void sub_0808f2a0(void);
void sub_0808f2c4(void);
void sub_0808f2fc(void);
void sub_0808f320(void);
void sub_0808f35c(void);
void sub_0808f380(void);
void sub_0808f39c(void);
void sub_0808f3b8(void);
void sub_0808f3d4(void);
void sub_0808f400(void);

void sub_0808e404(void)
{
    gCurTask->unk7A = 0;
    gCurTask->unk04 = (u32)sub_0808e440;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 4, gUnk_087431EC);
}

void sub_0808e440(void)
{
    CallTableEntry(gCurTask->unk15, 4, gUnk_087431FC);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808e464(void)
{
    CallTableEntry(gCurTask->unk14, 4, gUnk_087431EC);
}

void sub_0808e480(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk30 = 16;
    TaskStop();
    while (1)
    {
        switch (gCurTask->unk20 = TaskGetXDirBitToNearestPlayer())
        {
        case 8:
            if (gCurTask->unk1C != 0)
            {
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->unk1C = 0;
            }
            TaskSetFrameFlip(4);
            TaskYieldTrampoline(8);
            break;
        case 4:
            if (gCurTask->unk1C != 0)
            {
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->unk1C = 0;
            }
            TaskSetFrameNoFlip(4);
            TaskYieldTrampoline(8);
            break;
        default:
            TaskYieldTrampoline(1);
            break;
        }
    }
}

void sub_0808e510(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk20 != TaskGetXDirBitToNearestPlayer())
        gCurTask->unk1C = 1;
    sub_0808e070();
    sub_0808e2b4();
    u = gCurTask;
    u->unk30--;
    sub_0808e33c();
    sub_0808e36c();
}

void sub_0808e54c(void)
{
    gCurTask->unk15 = 2;
    gCurTask->unk30 = 0;
    TaskStop();
    while (1)
    {
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->unk58 = 0xFFFFF000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0xFFFFF800;
        TaskYieldTrampoline(16);
        gCurTask->unk58 = 128 << 4;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 128 << 5;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 128 << 4;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0xFFFFF800;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 0xFFFFF000;
        TaskYieldTrampoline(8);
    }
}

void sub_0808e5cc(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk30++;
    t->unk48 = t->unk2C;
    if (t->unk30 > 5 && RandomRange(2) != 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0808e464, gCurTaskIdx);
    }
}

void sub_0808e610(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gCurTask->unk15 = 1;
    TaskFaceNearestPlayer();
    gCurTask->unk30 = 0;
    TaskStop();
    t = gCurTask;
    switch (t->facing)
    {
    case 1:
        t->unk34 = 0;
        break;
    case -1:
        t->unk34 = 1;
        break;
    }
    gCurTask->unk6E = 0;
    do
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        u = gCurTask;
        u->unk6E++;
    } while ((s16)u->unk6E <= 7);
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < RandomRange(3) + 1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        sub_0808e254();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        w = gCurTask;
        w->unk6C++;
    }
    gCurTask->unk30 = 1;
    TaskSleepForever();
}

void sub_0808e704(void)
{
    if (gCurTask->unk30 != 0)
    {
        ActorSetState(3);
        TaskSetEntry(sub_0808e464, gCurTaskIdx);
    }
}

void sub_0808e730(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->unk15 = 3;
    t = gCurTask;
    t->unk1C = 1;
    t->unk60 = 0xFFFFF000;
    while (1)
    {
        switch (TaskGetXDirBitToNearestPlayer())
        {
        case 4:
            AngleToVector(160 << 1, 102);
            u = gCurTask;
            u->unk54 = gUnk_030023B4;
            if (u->unk1C != 0)
            {
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->unk1C = 0;
            }
            TaskSetFrameFlip(4);
            TaskYieldTrampoline(8);
            break;
        case 8:
            AngleToVector(224 << 1, 102);
            v = gCurTask;
            v->unk54 = gUnk_030023B4;
            if (v->unk1C != 0)
            {
                TaskSetFrameFlip(5);
                TaskYieldTrampoline(8);
                TaskSetFrameNoFlip(5);
                TaskYieldTrampoline(8);
                gCurTask->unk1C = 0;
            }
            TaskSetFrameNoFlip(4);
            TaskYieldTrampoline(8);
            break;
        default:
            TaskYieldTrampoline(1);
            break;
        }
    }
}

void sub_0808e800(void)
{
}

void sub_0808e804(void)
{
    gCurTask->unk04 = (u32)sub_0808e848;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_0874320C);
}

void sub_0808e848(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->unk15, 1, gUnk_08743210);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808e870(void)
{
    gCurTask->unk15 = 0;
    TaskFaceNearestPlayer();
    gCurTask->unk7A = 1;
    TaskStop();
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_0808e8a0(void)
{
}

s32 sub_0808e8a4(void)
{
    ActorSetState(2);
    TaskSetEntry(sub_0808e994, gCurTaskIdx);
    return 1;
}

s32 sub_0808e8c4(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0808e8d4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->unk38 = gUnk_087524A4;
    CallTableEntry(u->unk73, 3, gUnk_08743224);
}

void sub_0808e914(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_0808e964;
    switch (t->unk73)
    {
    case 0:
        t->facing = 1;
        break;
    case 1:
        t->facing = 255;
        break;
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08743230);
}

void sub_0808e964(void)
{
    if (sub_08069888() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_0874323C);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808e994(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_08743230);
}

void sub_0808e9b0(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk7A = 0;
    TaskSetFrame(5);
    TaskSleepForever();
}

void sub_0808e9d4(void)
{
    s32 v;

    v = TaskGetNearestPlayerDx();
    if (abs(v) <= 7)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0808e994, gCurTaskIdx);
    }
}

void sub_0808ea00(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk60 = gUnk_08743214[t->unk74];
    t->unk68 = gUnk_0874321C[t->unk74];
    while (1)
    {
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        gCurTask->frame += 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        TaskTurnAround();
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 1;
        TaskYieldTrampoline(2);
    }
}

void sub_0808eb10(void)
{
    ActorSetHitReactions(gUnk_08743558);
    ActorDie();
}

void sub_0808eb24(void)
{
    gCurTask->unk04 = (u32)sub_0808eb64;
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_08743240);
}

void sub_0808eb64(void)
{
    if (sub_08069888() == 0)
        CallTableEntry(gCurTask->unk15, 1, gUnk_08743244);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808eb94(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk7A = 1;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
    }
}

void sub_0808ebdc(void)
{
}

s32 sub_0808ebe0(void)
{
    switch (gCurTask->unk73)
    {
    default:
        ActorSetState(1);
        TaskSetEntry(sub_0808f39c, gCurTaskIdx);
        return 1;
    case 0:
        ActorSetState(1);
        TaskSetEntry(sub_0808f380, gCurTaskIdx);
        return 1;
    case 4:
        ActorSetState(2);
        TaskSetEntry(sub_0808f3b8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808ec34(void)
{
    switch (gCurTask->unk73)
    {
    default:
        ActorSetState(0);
        TaskSetEntry(sub_0808f39c, gCurTaskIdx);
        return 1;
    case 0:
        ActorSetState(0);
        TaskSetEntry(sub_0808f380, gCurTaskIdx);
        return 1;
    case 4:
        sub_08066c3c(gUnk_08743390);
        ActorSetState(0);
        TaskSetEntry(sub_0808f3b8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808ec90(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk73 == 4 && t->unk14 == 2)
        sub_08066b70();
    return 0;
}

s32 sub_0808ecb4(void)
{
    if (gCurTask->unk73 == 4)
        sub_08066c08(gUnk_08743390, 0);
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0808ece0(void)
{
    sub_08066c08(gUnk_08743390, 0);
    ActorSetState(2);
    TaskSetEntry(sub_0808f3b8, gCurTaskIdx);
    return 1;
}

s32 sub_0808ed0c(void)
{
    sub_08066c08(gUnk_08743390, 0);
    ActorSetState(2);
    TaskSetEntry(sub_0808f3b8, gCurTaskIdx);
    return 1;
}

void sub_0808ed38(void)
{
    s32 dx;
    s32 dy;

    dx = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->unk48 - (u16)gCurTask->unk48);
    dy = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->unk4A - (u16)gCurTask->unk4A);
    switch (gCurTask->unk30 = (u16)ArcTan2(dx, dy) >> 12)
    {
    case 0:
    case 15:
        gCurTask->unk30 = 0;
        gCurTask->unk20 = 1;
        break;
    case 1:
    case 2:
        gCurTask->unk30 = 0;
        gCurTask->unk20 = 0;
        break;
    case 3:
    case 4:
        gCurTask->unk30 = 2;
        gCurTask->unk20 = 0;
        break;
    case 5:
    case 6:
        gCurTask->unk30 = 4;
        gCurTask->unk20 = 0;
        break;
    case 7:
    case 8:
        gCurTask->unk30 = 4;
        gCurTask->unk20 = 1;
        break;
    case 9:
    case 10:
        gCurTask->unk30 = 3;
        gCurTask->unk20 = 1;
        break;
    case 11:
    case 12:
        gCurTask->unk30 = 2;
        gCurTask->unk20 = 1;
        break;
    case 13:
    case 14:
        gCurTask->unk30 = 1;
        gCurTask->unk20 = 1;
        break;
    }
}

void sub_0808ee60(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk34 > t->unk30)
    {
        if (t->unk34 > 0)
        {
            t->unk18 = t->unk34;
            t->unk34--;
        }
        else
        {
            t->unk18 = t->unk34;
        }
    }
    else if (t->unk34 < t->unk30)
    {
        if (t->unk34 <= 3)
        {
            t->unk18 = t->unk34;
            t->unk34++;
        }
        else
        {
            t->unk18 = t->unk34;
        }
    }
    else if (t->unk34 == t->unk30)
    {
        t->unk18 = t->unk34;
        t->unk1C = 1;
    }
}

void sub_0808ee9c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk34 > t->unk30)
        t->unk1C = 0;
    else if (t->unk34 < t->unk30)
        t->unk1C = 0;
    else if (t->unk34 == t->unk30)
        t->unk1C = 1;
}

void sub_0808eec4(s32 a)
{
    struct Task *t;
    u16 b;

    b = a;

    switch (gCurTask->unk73)
    {
    case 0:
    case 4:
        switch (gCurTask->unk34)
        {
        case 0:
            AngleToVector(0, (s16)b);
            break;
        case 1:
            AngleToVector(224 << 1, (s16)b);
            break;
        case 2:
            AngleToVector(192 << 1, (s16)b);
            break;
        case 3:
            AngleToVector(160 << 1, (s16)b);
            break;
        case 4:
            AngleToVector(128 << 1, (s16)b);
            break;
        }
        break;
    case 1:
        AngleToVector(192 << 1, (s16)b);
        break;
    case 2:
        AngleToVector(224 << 1, (s16)b);
        break;
    case 3:
        AngleToVector(160 << 1, (s16)b);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    t = gCurTask;
    t->unk54 = gUnk_030023B4;
    t->unk58 = gUnk_030023D4;
}

void sub_0808ef88(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    switch (TaskGetXDirBitToNearestPlayer())
    {
    default:
        t = gCurTask;
        t->unk30 = 0;
        t->unk34 = 0;
        break;
    case 4:
        u = gCurTask;
        u->unk30 = 0;
        u->unk34 = 0;
        TaskSetFrameNoFlip(4);
        break;
    case 8:
        v = gCurTask;
        v->unk30 = 4;
        v->unk34 = 4;
        TaskSetFrameFlip(4);
        break;
    }
}

void sub_0808efdc(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 7;
    sp.unk04 = 109;
    sp.unk08 = 0;
    sp.unk09 = gCurTask->unk74;
    sp.unk0C = gUnk_0874324C[gCurTask->unk30];
    sp.unk0E = gUnk_08743251[gCurTask->unk30];
    sp.unk0A = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    gCurTask->unk46 = CreateChildTaskAtOffsetFacing(172, gUnk_0874324C[gCurTask->unk30], gUnk_08743251[gCurTask->unk30], 0);
}

void sub_0808f058(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 7;
    sp.unk04 = 109;
    sp.unk08 = 0;
    sp.unk09 = 4;
    sp.unk0C = gUnk_08743256[gCurTask->unk34];
    sp.unk0E = gUnk_08743251[gCurTask->unk34];
    sp.unk0A = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    gCurTask->unk46 = CreateChildTaskAtOffsetFacing(172, gUnk_08743256[gCurTask->unk34], gUnk_08743251[gCurTask->unk34], 0);
}

void sub_0808f0d0(void)
{
    sub_0808ed38();
    sub_0808ee60();
    switch (gCurTask->unk34)
    {
    case 0:
        TaskSetFrameNoFlip(4);
        break;
    case 1:
        if (gCurTask->unk18 == 2)
        {
            TaskSetFrameNoFlip(6);
            TaskYieldTrampoline(4);
        }
        TaskSetFrameNoFlip(5);
        break;
    case 2:
        switch (gCurTask->unk18)
        {
        case 1:
            TaskSetFrameNoFlip(6);
            TaskYieldTrampoline(4);
            break;
        case 3:
            TaskSetFrameFlip(6);
            TaskYieldTrampoline(4);
            break;
        }
        TaskSetFrameNoFlip(7);
        break;
    case 3:
        if (gCurTask->unk18 == 2)
        {
            TaskSetFrameFlip(6);
            TaskYieldTrampoline(4);
        }
        TaskSetFrameFlip(5);
        break;
    case 4:
        TaskSetFrameFlip(4);
        break;
    }
    TaskYieldTrampoline(gUnk_08743248[gCurTask->unk74]);
}

void sub_0808f1b4(u16 a, void *b)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (--t->unk28 <= 0)
    {
        t->unk28 = gUnk_08743248[t->unk74];
        sub_0808ed38();
        sub_0808ee9c();
        u = gCurTask;
        if (u->unk1C != 0)
        {
            u->unk1C = 0;
            if (u->unk2C != 0)
            {
                if (u->unk20 != 0)
                {
                    ActorSetState(a);
                    TaskSetEntry(b, gCurTaskIdx);
                }
            }
            else
            {
                u->unk2C = 1;
            }
        }
    }
}

void sub_0808f224(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->unk38 = gUnk_087524E4;
    u->unk8C->unk1A = 4;
    CallTableEntry(u->unk73, 6, gUnk_08743284);
}

void sub_0808f26c(void)
{
    gCurTask->unk04 = (u32)sub_0808f2a0;
    ActorSetState(0);
    sub_0808ef88();
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874329C);
}

void sub_0808f2a0(void)
{
    CallTableEntry(gCurTask->unk15, 3, gUnk_087432A8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808f2c4(void)
{
    gCurTask->unk04 = (u32)sub_0808f2fc;
    ActorSetState(0);
    gCurTask->facing = 255;
    CallTableEntry(gCurTask->unk14, 3, gUnk_087432B4);
}

void sub_0808f2fc(void)
{
    CallTableEntry(gCurTask->unk15, 3, gUnk_087432C0);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808f320(void)
{
    gCurTask->unk04 = (u32)sub_0808f35c;
    ActorSetState(0);
    sub_0808ef88();
    sub_08066b34(gUnk_087433E8);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087432CC);
}

void sub_0808f35c(void)
{
    CallTableEntry(gCurTask->unk15, 3, gUnk_087432D8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808f380(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_0874329C);
}

void sub_0808f39c(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087432B4);
}

void sub_0808f3b8(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087432CC);
}

void sub_0808f3d4(void)
{
    struct Task *t;

    gCurTask->unk15 = 0;
    t = gCurTask;
    t->unk28 = gUnk_08743248[t->unk74];
    TaskStopY();
    while (1)
        sub_0808f0d0();
}

void sub_0808f400(void)
{
    if (sub_08069888() == 0)
        sub_0808f1b4(2, sub_0808f380);
}

