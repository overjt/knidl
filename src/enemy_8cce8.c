/* game_code_and_rodata 0x0808CCE8-0x0808E404 (issue #70, module M24 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808CCE8 0x0808E404 src/enemy_8cce8.c --newpb
 *
 * M24 is a bank of six enemy/object behaviour scripts built to the same
 * three-table pattern as M22/M25/M26 (rom-map section 9):
 *
 *   entry       -> installs the draw hook in Task.unk00 (TaskMove or
 *                  ActorMove) and the per-frame hook in Task.unk0C, points
 *                  Task.unk38 at a TaskGfx block, and hands Task.unk73 to
 *                  CallTableEntry with the script's entry table;
 *   unk14 table -> the coroutine BODIES: each installs its own resume function
 *                  in Task.unk04 and then runs a chain of TaskYieldTrampoline
 *                  waits;
 *   unk15 table -> the per-frame GUARDS that re-arm the body through
 *                  TaskSetEntry(fn, gCurTaskIdx) when the state changes.
 *
 * This batch holds:
 *   * the two stand-alone class-2 bodies `sub_0808cce8` (a two-variant intro
 *     that walks Task.posX/unk50 with RandomSpread and waits on the room byte
 *     gTaskSlotTypes[Task.unk44] through `sub_0808cfec`) and `sub_0808d014`,
 *     plus the smaller `sub_0808d148` and `sub_0808d218`;
 *   * script 1: entry `sub_0808d4e8` (Task.unk73 -> `0x08743188`, 3 rows) with
 *     the row bodies `sub_0808d558` / `sub_0808da00` / `sub_0808df58`, the
 *     body tables `0x08743194` / `0x087431AC` / `0x087431C4` and the guard
 *     tables `0x087431A0` / `0x087431B8` / `0x087431C8`;
 *   * its movement library: `sub_0808d364` / `sub_0808d388` snap Task.unk2C to
 *     the 16-pixel grid, `sub_0808d3e4` rolls a new mode out of the 8-entry
 *     table `gUnk_0874313C`, `sub_0808d460` flips the sprite through
 *     Task.unk3E and `sub_0808d494` / `sub_0808d4a8` / `sub_0808d4bc` /
 *     `sub_0808d4d0` set the animation id in Actor.unk1A;
 *   * script 2's entry `sub_0808e3a8` (Task.unk73 -> `0x087431E4`) and its
 *     aiming half: `sub_0808e070` / `sub_0808e0d0` / `sub_0808e174` turn the
 *     vector to the target into a heading with ArcTan2, `sub_0808e254` spawns
 *     actor 103, `sub_0808e2b4` is the GetDistSq proximity test and
 *     `sub_0808e33c` / `sub_0808e36c` are the per-frame step.  The script's
 *     rows continue in src/enemy_8e404.c.
 *
 * `sub_0808d388` is a pointer-referenced leaf the prologue scan could not
 * propose (no `push`, lesson 3.75); it and the module's three other census
 * fixes are curated in tools/symdb.py.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s8 gUnk_02007FB8[];
extern s8 gDigits[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern vu16 gTaskSlotTypes[];
extern u8 gTerrainResult[];

/* ROM tables */
extern u32 gUnk_0873F500[];
extern s16 gUnk_08742FAC[];
extern u8 gUnk_0874313C[];
extern u32 gUnk_08743144[];
extern u32 gUnk_08743158[];
extern u32 gUnk_08743188[];
extern u32 gUnk_08743194[];
extern u32 gUnk_087431A0[];
extern u32 gUnk_087431AC[];
extern u32 gUnk_087431B8[];
extern u32 gUnk_087431C4[];
extern u32 gUnk_087431C8[];
extern u32 gUnk_087431CC[];
extern u32 gUnk_087431D8[];
extern u32 gUnk_087431E4[];
extern u32 gUnk_087521D8[];
extern u32 gUnk_08752248[];
extern u32 gUnk_087522B4[];
extern u32 gUnk_087523E4[];
extern u32 gUnk_08752808[];
extern u32 gUnk_08752C18[];

/* Externals */
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 a);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void IntToDigits(s16 n);
extern void TaskMove(void);
extern void TaskDrawWorld(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetFrameByFacing(s16 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskUpdateFlip(void);
extern void TaskSetFrame(s32 a);
extern u16 RandomSpread(s32 base, u8 scale, u8 amount);
extern s32 sub_08021a40(s32 x, s32 y);
extern void ActorSetState(u16 v);
extern void ActorSetAttackBox(void *p);
extern s32 TaskFindNearestPlayer(void);
extern s32 GetDistSq(struct PointPair *p);
extern void TaskFaceNearestPlayer(void);
extern void ActorDestroy(void);
extern void TaskTurnAround(void);
extern void AngleToVector(s32 a, s32 b);
extern u8 TaskGetXDirBitToNearestPlayer(void);
extern void TaskAccelerateInDir(s32 step, s32 limit, u16 dir);
extern s8 TaskGetParentFacing(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_08065640(void);
extern void ActorMove(void);
extern void SetPaletteAnimSource(u32 i, u32 p1, u8 p2);
extern void AcquirePaletteAnim(u32 p0, s32 idx);
extern void ActorCheckHits(void);
extern u8 ActorCollideTerrain(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_080699a8(void);
extern u32 ActorReactToHit(void);
extern void sub_0806a0f0(s32 a);
extern void ActorDie(void);
extern void sub_0806ee2c(void);
extern void sub_0808e464(void);

/* Forward declarations */
void sub_0808cce8(void);
void sub_0808cfec(void);
void sub_0808d014(void);
void sub_0808d100(void);
void sub_0808d130(void);
void sub_0808d148(void);
void sub_0808d1d0(void);
void sub_0808d200(void);
void sub_0808d218(void);
void sub_0808d2a8(void);
s32 sub_0808d2b8(void);
s32 sub_0808d304(void);
s32 sub_0808d354(void);
s32 sub_0808d364(void);
s32 sub_0808d388(void);
void sub_0808d3e4(void);
void sub_0808d460(void);
void sub_0808d494(void);
void sub_0808d4a8(void);
void sub_0808d4bc(void);
void sub_0808d4d0(void);
void sub_0808d4e8(void);
void sub_0808d558(void);
void sub_0808d58c(void);
void sub_0808d624(void);
void sub_0808d640(void);
void sub_0808d764(void);
void sub_0808d790(void);
void sub_0808d938(void);
void sub_0808d964(void);
void sub_0808d9fc(void);
void sub_0808da00(void);
void sub_0808da34(void);
void sub_0808dacc(void);
void sub_0808dae8(void);
void sub_0808dc68(void);
void sub_0808dc94(void);
void sub_0808de90(void);
void sub_0808debc(void);
void sub_0808df54(void);
void sub_0808df58(void);
void sub_0808df9c(void);
void sub_0808dfc4(void);
void sub_0808e050(void);
void sub_0808e054(void);
void sub_0808e070(void);
void sub_0808e0d0(void);
void sub_0808e174(void);
void sub_0808e254(void);
void sub_0808e2b4(void);
void sub_0808e33c(void);
void sub_0808e36c(void);
void sub_0808e3a8(void);

void sub_0808cce8(void)
{

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->layer = 10;
    gCurTask->unk38 = gUnk_087522B4;
    switch (gCurTask->unk14)
    {
    case 0:
        sub_0808cfec();
        gCurTask->posX = (RandomSpread(-20, 1, 32) + gTasks[gCurTask->unk44].unk48) << 16;
        gCurTask->posY = (RandomSpread(20, 1, 8) + gTasks[gCurTask->unk44].unk4A) << 16;
        TaskSetMotionXFacing(0xFFFF4000, 128 << 5);
        gCurTask->unk58 = 0xFFFEC000;
        gCurTask->unk60 = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        sub_0808cfec();
        gCurTask->posX = (RandomSpread(-20, 1, 32) + gTasks[gCurTask->unk44].unk48) << 16;
        gCurTask->posY = (RandomSpread(4, 1, 8) + gTasks[gCurTask->unk44].unk4A) << 16;
        TaskSetMotionXFacing(0xFFFF4000, 128 << 5);
        gCurTask->unk58 = 0xFFFEC000;
        gCurTask->unk60 = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        break;
    case 1:
        sub_0808cfec();
        gCurTask->posX = (RandomSpread(-12, 1, 32) + gTasks[gCurTask->unk44].unk48) << 16;
        gCurTask->posY = (RandomSpread(16, 1, 8) + gTasks[gCurTask->unk44].unk4A) << 16;
        TaskSetMotionXFacing(192 << 8, 0xFFFFF000);
        gCurTask->unk58 = 0xFFFEC000;
        gCurTask->unk60 = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        sub_0808cfec();
        gCurTask->posY = (RandomSpread(0, 1, 8) + gTasks[gCurTask->unk44].unk4A) << 16;
        TaskSetMotionXFacing(192 << 8, 0xFFFFF000);
        gCurTask->unk58 = 0xFFFEC000;
        gCurTask->unk60 = 0xFFFFE000;
        TaskSetFrame(2);
        TaskYieldTrampoline(3);
        TaskSetFrame(3);
        TaskYieldTrampoline(3);
        TaskSetFrame(4);
        TaskYieldTrampoline(3);
        TaskSetFrame(5);
        TaskYieldTrampoline(3);
        TaskSetFrame(6);
        TaskYieldTrampoline(3);
        TaskSetFrame(7);
        TaskYieldTrampoline(3);
        break;
    }
    TaskExitTrampoline();
}

void sub_0808cfec(void)
{
    if (gTaskSlotTypes[gCurTask->unk44] != 104)
        TaskExitTrampoline();
}

void sub_0808d014(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *z;
    s32 v;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    u = gCurTask;
    u->unk38 = gUnk_087523E4;
    u->unk04 = (u32)sub_0808d100;
    u->unk7A = 0;
    w = gCurTask;
    w->unk28 = 0;
    v = gTasks[w->unk44].unk34;
    w->unk34 = v;
    if (w->unk74 <= 1)
        w->unk34 = v >> 1;
    gCurTask->facing = TaskGetParentFacing();
    AngleToVector(gUnk_08742FAC[gCurTask->unk34], 128 << 4);
    TaskSetMotionXFacing(gUnk_030023B4, 0x5A5A5A5A);
    gCurTask->unk58 = gUnk_030023D4;
    TaskSetFrame(0);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        z = gCurTask;
        z->unk6C++;
    } while ((s16)z->unk6C <= 2);
    ActorDestroy();
}

void sub_0808d100(void)
{
    if (sub_08069660() != 0) {
        TaskStop();
        TaskSetEntry(ActorDie, gCurTaskIdx);
    } else {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0808d130(void)
{
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_0808d148(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_0808d1d0;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->unk38 = gUnk_08752808;
    gCurTask->facing = TaskGetParentFacing();
    gCurTask->unk7A = 0;
    gCurTask->unk58 = 0xFFFA0000;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        TaskSetFrame(5);
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 3);
    ActorDestroy();
}

void sub_0808d1d0(void)
{
    if (sub_08069604() != 0)
    {
        TaskStop();
        TaskSetEntry(ActorDie, gCurTaskIdx);
    }
    else
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_0808d200(void)
{
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_0808d218(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)ActorMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    t = gCurTask;
    t->unk38 = gUnk_08752C18;
    t->unk04 = (u32)sub_0808d2a8;
    gCurTask->facing = TaskGetParentFacing();
    TaskSetMotionXFacing(192 << 9, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 168 << 5, 192 << 10);
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        TaskSetFrame(5);
        TaskYieldTrampoline(4);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
    }
}

void sub_0808d2a8(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_0808d2b8(void)
{
    switch (gCurTask->unk73)
    {
    case 0:
        ActorSetState(2);
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
        return 1;
    case 1:
        ActorSetState(2);
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
        return 1;
    }
    return 0;
}

s32 sub_0808d304(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk30 = 1;
    t->unk34 = 1;
    switch (t->unk73)
    {
    case 0:
        ActorSetState(0);
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
        return 1;
    case 1:
        ActorSetState(0);
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
        return 1;
    }
    return 0;
}

s32 sub_0808d354(void)
{
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0808d364(void)
{
    if (gCurTask->unk54 != 0 && sub_080699a8() != 0)
        TaskStop();
    return 0;
}

s32 sub_0808d388(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk54 >= 0)
        t->unk2C = ((t->unk48 - 16) & 0xFFF0) + 15;
    else
        t->unk2C = (t->unk48 + 16) & 0xFFF0;
    u = gCurTask;
    u->unk18 = u->unk2C - u->unk48;
    u->unk48 = u->unk2C + u->unk18;
    u->posX = u->unk48 << 16;
    return 0;
}

void sub_0808d3e4(void)
{
    struct Task *t;
    s8 i;
    u8 v;

    do
    {
        IntToDigits((s16)RandomRange(8));
        t = gCurTask;
        i = gDigits[0];
    } while (t->unk34 == gUnk_0874313C[i]);
    v = gUnk_0874313C[i];
    switch (v)
    {
    case 0:
        t->unk34 = v;
        ActorSetState(1);
        break;
    case 1:
        t->unk34 = v;
        ActorSetState(0);
        break;
    case 2:
        t->unk34 = v;
        t->facing = -1 * t->facing;
        ActorSetState(0);
        break;
    default:
        sub_0806ee2c();
        break;
    }
}

void sub_0808d460(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->facing == 1)
        t->unk3E |= 0x8000;
    else
        t->unk3E &= 0x7FFF;
}

void sub_0808d494(void)
{
    gCurTask->unk8C->unk1A = 9;
}

void sub_0808d4a8(void)
{
    gCurTask->unk8C->unk1A = 8;
}

void sub_0808d4bc(void)
{
    gCurTask->unk8C->unk1A = 10;
}

void sub_0808d4d0(void)
{
    gCurTask->unk8C->unk1A = -1;
}

void sub_0808d4e8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk8C->unk16 = 0;
    t->unk8C->unk18 = 0;
    t->unk8C->unk1E = (t->unk40 & 0xFFF) | (240 << 8);
    t->unk34 = 1;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)sub_08065640;
    t->layer = 11;
    u = gCurTask;
    u->unk38 = gUnk_087521D8;
    CallTableEntry(u->unk73, 3, gUnk_08743188);
}

void sub_0808d558(void)
{
    gCurTask->unk04 = (u32)sub_0808d58c;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_08743194);
}

void sub_0808d58c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u8 r;

    r = ActorCollideTerrain();
    t = gCurTask;
    if ((t->unk7A & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            t->unk24 = (u16)t->unk24 | 0x10000;
        if ((gCurTask->unk7A & 1) != 0)
            goto skip;
    }
    u = gCurTask;
    u->unk24 = (u16)u->unk24;
skip:
    if (r == 0)
    {
        sub_0808d364();
        CallTableEntry(gCurTask->unk15, 3, gUnk_087431A0);
    }
    v = gCurTask;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->unk4A;
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808d624(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_08743194);
}

void sub_0808d640(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->unk15 = 0;
    TaskFaceNearestPlayer();
    if (gCurTask->unk34 == 2)
        TaskTurnAround();
    t = gCurTask;
    t->unk28 = 0;
    t->unk6C = 0;
    do
    {
        sub_0808d460();
        TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(5);
        TaskUpdateFlip();
        TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(5);
        TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(5);
        TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(5);
        TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(gUnk_08743144[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        u = gCurTask;
        u->unk6C++;
    } while ((s16)u->unk6C <= 1);
    TaskStop();
    v = gCurTask;
    v->unk28 = 2;
    TaskSleepForever();
}

void sub_0808d764(void)
{
    if (gCurTask->unk28 == 2)
    {
        sub_0808d3e4();
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
    }
}

void sub_0808d790(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    sub_0808d460();
    TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
    sub_0808d4d0();
    gCurTask->frame = 6;
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        TaskUpdateFlip();
        TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(4);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        sub_0808d460();
        TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(7);
        TaskYieldTrampoline(7);
        TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 2);
    TaskStop();
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_0808d938(void)
{
    if (gCurTask->unk28 == 1)
    {
        sub_0808d3e4();
        TaskSetEntry(sub_0808d624, gCurTaskIdx);
    }
}

void sub_0808d964(void)
{
    struct Task *t;

    gCurTask->unk15 = 2;
    TaskFaceNearestPlayer();
    TaskStop();
    t = gCurTask;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(8);
        TaskUpdateFlip();
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808d9fc(void)
{
}

void sub_0808da00(void)
{
    gCurTask->unk04 = (u32)sub_0808da34;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 3, gUnk_087431AC);
}

void sub_0808da34(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u8 r;

    r = ActorCollideTerrain();
    t = gCurTask;
    if ((t->unk7A & 1) != 0)
    {
        if ((u8)(gTerrainResult[4] - 1) > 3)
            t->unk24 = (u16)t->unk24 | 0x10000;
        if ((gCurTask->unk7A & 1) != 0)
            goto skip;
    }
    u = gCurTask;
    u->unk24 = (u16)u->unk24;
skip:
    if (r == 0)
    {
        sub_0808d364();
        CallTableEntry(gCurTask->unk15, 3, gUnk_087431B8);
    }
    v = gCurTask;
    v->unk24 = (v->unk24 & 0xFFFF0000) | v->unk4A;
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808dacc(void)
{
    CallTableEntry(gCurTask->unk14, 3, gUnk_087431AC);
}

void sub_0808dae8(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk15 = 0;
    TaskFaceNearestPlayer();
    if (gCurTask->unk34 == 2)
        TaskTurnAround();
    t = gCurTask;
    t->unk28 = 0;
    t->unk6C = 0;
    do
    {
        sub_0808d460();
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(7);
        TaskUpdateFlip();
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[0], 0x5A5A5A5A);
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(3);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[1], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(6);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[2], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(2);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[3], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743144[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(2);
        u = gCurTask;
        u->unk6C++;
    } while ((s16)u->unk6C <= 1);
    TaskStop();
    gCurTask->unk28 = 2;
    TaskSleepForever();
}

void sub_0808dc68(void)
{
    if (gCurTask->unk28 == 2)
    {
        sub_0808d3e4();
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
    }
}

void sub_0808dc94(void)
{
    struct Task *t;

    gCurTask->unk15 = 1;
    gCurTask->unk28 = 0;
    TaskFaceNearestPlayer();
    sub_0808d460();
    if (gCurTask->unk30 != 0)
        TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
    sub_0808d4d0();
    gCurTask->frame = 6;
    TaskYieldTrampoline(8);
    gCurTask->unk6C = 0;
    do
    {
        TaskUpdateFlip();
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[0], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrame(4);
        TaskYieldTrampoline(7);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[1], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(4);
        TaskYieldTrampoline(1);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrame(7);
        TaskYieldTrampoline(7);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[2], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(1);
        sub_0808d460();
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 6;
        TaskYieldTrampoline(2);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[3], 0x5A5A5A5A);
        sub_0808d494();
        gCurTask->frame = 5;
        TaskYieldTrampoline(6);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4a8();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(7);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[4], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(1);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4bc();
        TaskSetFrameByFacing(7);
        TaskYieldTrampoline(7);
        if (gCurTask->unk30 != 0)
            TaskSetMotionXFacing(gUnk_08743158[5], 0x5A5A5A5A);
        sub_0808d4d0();
        TaskSetFrameByFacing(6);
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk6C++;
    } while ((s16)t->unk6C <= 2);
    TaskStop();
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_0808de90(void)
{
    if (gCurTask->unk28 == 1)
    {
        sub_0808d3e4();
        TaskSetEntry(sub_0808dacc, gCurTaskIdx);
    }
}

void sub_0808debc(void)
{
    struct Task *t;

    gCurTask->unk15 = 2;
    TaskFaceNearestPlayer();
    TaskStop();
    t = gCurTask;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(8);
        TaskUpdateFlip();
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808df54(void)
{
}

void sub_0808df58(void)
{
    gCurTask->unk04 = (u32)sub_0808df9c;
    TaskFaceNearestPlayer();
    ActorSetAttackBox(gUnk_0873F500);
    gCurTask->unk78 = 2;
    ActorSetState(0);
    CallTableEntry(gCurTask->unk14, 1, gUnk_087431C4);
}

void sub_0808df9c(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->unk15, 1, gUnk_087431C8);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808dfc4(void)
{
    gCurTask->unk15 = 0;
    TaskFaceNearestPlayer();
    gCurTask->unk7A = 1;
    TaskStop();
    while (1)
    {
        sub_0808d460();
        sub_0808d4d0();
        gCurTask->frame = 6;
        TaskYieldTrampoline(8);
        TaskUpdateFlip();
        sub_0808d4d0();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        sub_0808d4d0();
        TaskSetFrame(5);
        TaskYieldTrampoline(8);
        sub_0808d494();
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        sub_0808d4a8();
        TaskSetFrame(7);
        TaskYieldTrampoline(8);
        sub_0808d4bc();
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
    }
}

void sub_0808e050(void)
{
}

void sub_0808e054(void)
{
    if (--gUnk_02007FB8[1] < 0)
        sub_0806ee2c();
}

void sub_0808e070(void)
{
    switch (TaskGetXDirBitToNearestPlayer())
    {
    case 4:
        gCurTask->unk2C = (&gTasks[TaskFindNearestPlayer()])->unk48 - 64;
        break;
    case 8:
        gCurTask->unk2C = (&gTasks[TaskFindNearestPlayer()])->unk48 + 64;
        break;
    }
}

void sub_0808e0d0(void)
{
    struct Task *t;
    s32 dx;
    s32 dy;

    switch (TaskGetXDirBitToNearestPlayer())
    {
    case 4:
        gCurTask->unk2C = (&gTasks[TaskFindNearestPlayer()])->unk48 - 64;
        break;
    case 8:
        gCurTask->unk2C = (&gTasks[TaskFindNearestPlayer()])->unk48 + 64;
        break;
    }
    t = gCurTask;
    dx = (s16)(t->unk2C - (u16)t->unk48);
    dy = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->unk4A - (u16)gCurTask->unk4A);
    gCurTask->unk18 = (((u16)ArcTan2(dx, dy) >> 8) + 16) >> 5;
}

void sub_0808e174(void)
{
    struct Task *t;
    s32 dx;
    s32 dy;
    s32 i;

    t = gCurTask;
    dx = (s16)(t->unk2C - (u16)t->unk48);
    dy = (s16)((u16)(&gTasks[TaskFindNearestPlayer()])->unk4A - (u16)gCurTask->unk4A);
    i = (((u16)ArcTan2(dx, dy) >> 8) + 16) >> 5;
    switch (i)
    {
    case 0:
        gCurTask->unk34 = 0;
        break;
    case 1:
        gCurTask->unk34 = 1;
        break;
    case 2:
        gCurTask->unk34 = 2;
        break;
    case 3:
        gCurTask->unk34 = 3;
        break;
    case 4:
        gCurTask->unk34 = 4;
        break;
    case 5:
        gCurTask->unk34 = 5;
        break;
    case 6:
        gCurTask->unk34 = 6;
        break;
    case 7:
        gCurTask->unk34 = 7;
        break;
    }
}

void sub_0808e254(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    t = gCurTask;
    if (sub_08021a40(t->unk48 + (t->facing << 4), t->unk4A) == 0)
    {
        sp.unk00 = 1;
        sp.unk04 = 103;
        sp.unk08 = 0;
        sp.unk09 = gCurTask->unk34;
        sp.unk0C = 16;
        sp.unk0E = 0;
        sp.unk0A = 1;
        CreateActorFromDescAtOffsetFacing(&sp, 0);
    }
}

void sub_0808e2b4(void)
{
    struct PointPair p;
    struct Task *t;

    p.x0 = gCurTask->unk2C;
    p.y0 = (&gTasks[TaskFindNearestPlayer()])->unk4A;
    t = gCurTask;
    p.x1 = t->unk48;
    p.y1 = t->unk4A;
    if (GetDistSq(&p) <= 16)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0808e464, gCurTaskIdx);
    }
}

void sub_0808e33c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->unk30 == 0)
    {
        t->unk30 = 16;
        t->unk28 = t->unk18;
        sub_0808e0d0();
        u = gCurTask;
        if (u->unk28 != u->unk18)
            sub_0808e174();
    }
}

void sub_0808e36c(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk30 == 16 || t->unk30 == 8)
        TaskAccelerateInDir(gUnk_087431CC[t->unk74], gUnk_087431D8[t->unk74], (u16)t->unk34);
}

void sub_0808e3a8(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 11;
    gCurTask->unk38 = gUnk_08752248;
    AcquirePaletteAnim(3, 1);
    SetPaletteAnimSource(1, 0, gCurTask->unk8C->unk0C);
    CallTableEntry(gCurTask->unk73, 2, gUnk_087431E4);
}

