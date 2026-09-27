#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_53af4.c (0x08053AF4-0x0805432F, issue #89).
 *
 * Task type #7, the player's effect objects: the body and variants 0-6.
 * Task type #7 (class 1) is started by M16's sub_0805afac/sub_0805b088
 * (src/effect_5afac.c), and by M14's sub_08053940 when its type-6 bands are
 * full, with Task.unk18 = variant << 24 | arg and the spawner's position,
 * facing Task.unk43 and PlayerState Task.unk88 copied in.  The body
 * Task_PlayerEffect links the task to its spawner the first time (Task.unk8C =
 * &gTasks[Task.unk44], read back as a struct Task) and dispatches the
 * variant, the top byte of Task.unk18, through the 49 entries of
 * gPlayerEffectVariants, which fill this file and the eleven effect_*.c files after
 * it.  A variant installs a motion hook in Task.unk00 (TaskMove moves in
 * world space, TaskMoveRelativeToParent keeps the position relative to the spawner's
 * task, TaskUpdatePixelPos stays put), a draw hook in Task.unk0C, often a
 * per-frame callback in Task.unk04 and an animation table in Task.unk38,
 * then runs a TaskYieldTrampoline script and ends in TaskExitTrampoline;
 * the functions after a body are the callbacks only it installs.  Variant 0
 * (sub_08053b40, spawned by M10) rides on its spawner and cycles frames
 * 0-11, hidden every other frame; sub_08053be0 copies the spawner's
 * Task.unk13 with bit 2 cleared and kills it once PlayerState.unk40 bit 2
 * clears, and its draw hook sub_08053c1c draws through M11's sub_0803dfc8 in
 * player mode 10 and kills it otherwise.  Variants 1 and 2 (M10) are short
 * puffs launched from 12 pixels behind the point they face, the sub-state
 * picking the facing; 3 (M13's ability get) shows for three frames at a
 * random offset from its spawner (sub_08053e34 is its empty Task.unk04
 * stub); 4 and 5 fly one of eight random trajectories of the s16 rows
 * gUnk_0873B9EC[6][8] (offsets and 8.8 velocities), 5 with TaskDrawScreen's
 * draw when PlayerState.unk37 == 2.  Variant 6 has two forms picked by the
 * third byte of Task.unk18: a loop that places a puff 6 pixels behind the
 * spawner and 8 below it and spawns the other form (a rising puff) each
 * round until sub_08054298 sets Task.unk28 - when the player's mode differs
 * from the one saved at the start or is 16, or, by the second byte of
 * Task.unk18, when the spawner's Task.unk7A clears, a countdown in
 * Task.unk30 runs out or M11's sub_0803fd20 no longer returns 4. */

extern void (*gPlayerEffectVariants[])(void);   /* task type #7's 49 variants, indexed by Task.unk18 >> 24 */
extern u32 gUnk_08751C44[];
extern u32 gUnk_0874C600[];
extern u32 gUnk_08751CEC[];
extern u32 gUnk_0874C500[];
extern s16 gUnk_0873B9EC[];             /* [6][8]: s16 x, y offsets, 8.8 velocities */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawScreen(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
void TaskSetFrame(s32 a);
void sub_08006384(u16 a);
void TaskStepForward(s16 a);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_0803dfc8(void);
s32 sub_0803fd20(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_08053be0(void);
void sub_08053c1c(void);
void sub_08053e34(void);
void sub_08054298(void);

void Task_PlayerEffect(void)
{
    if (gCurTask->unk8C == NULL)
    {
        gCurTask->unk80 = 0;
        gCurTask->unk8C = (struct Actor *)&gTasks[gCurTask->unk44];
    }
    CallTableEntry(((u8 *)gCurTask)[27], 49, gPlayerEffectVariants);
}

void sub_08053b40(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMoveRelativeToParent;
    gCurTask->unk0C = (u32)sub_08053c1c;
    gCurTask->unk04 = (u32)sub_08053be0;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751C44;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 4;
    sub_08006384(4);
    gCurTask->unk50 = 0x40000;
    for (;;)
    {
        gCurTask->unk28 = 0;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame((s16)gCurTask->unk28++);
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 11);
    }
}

void sub_08053be0(void)
{
    gCurTask->unk13 = ((struct Task *)gCurTask->unk8C)->unk13 & 0xFB;
    if (!(gCurTask->unk88->unk40 & 4))
        TaskFree(gCurTaskIdx);
}

void sub_08053c1c(void)
{
    if (gCurTask->unk88->unk04 == 10)
        sub_0803dfc8();
    else
        TaskFree(gCurTaskIdx);
}

void sub_08053c48(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C600;
    if ((t->unk18 & 15) == 0)
        t->unk43 = -1;
    else
        t->unk43 = 1;
    TaskStepForward(-12);
    gCurTask->unk50 = (gCurTask->unk4A + 6) << 16;
    TaskSetMotionXFacing(-0x24000, 0x1800);
    gCurTask->unk58 = -0x4000;
    gCurTask->unk60 = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(2);
    TaskSetFrameByFacing(6);
    TaskYieldTrampoline(2);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08053d08(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C600;
    if ((t->unk18 & 15) == 0)
        t->unk43 = -1;
    else
        t->unk43 = 1;
    TaskStepForward(-12);
    gCurTask->unk50 = (gCurTask->unk4A + 6) << 16;
    TaskSetMotionXFacing(-0x60000, 0xC000);
    gCurTask->unk58 = -0x20000;
    gCurTask->unk60 = 0x4000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(4);
    TaskSetFrameByFacing(10);
    TaskYieldTrampoline(2);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_08053db8(void)
{
    struct Task *t = gCurTask;
    s32 s;

    t->unk04 = (u32)sub_08053e34;
    s = t->unk18 & 15;
    if (s == 0)
    {
        t->unk00 = (u32)TaskMoveRelativeToParent;
        t->unk0C = (u32)TaskDrawWorld;
        t->unk42 = 5;
        gCurTask->unk38 = gUnk_08751CEC;
        gCurTask->unk4C = RandomSpreadFacing(-16, 1, 16) << 16;
        gCurTask->unk50 = (RandomSpread(-4, 1, 16) << 16) - 0x180000;
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(3);
    }
    TaskExitTrampoline();
}

void sub_08053e34(void)
{
}

void sub_08053e38(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 c;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    gCurTask->unk38 = gUnk_0874C500;
    n = RandomRange(8);
    TaskStepForward(gUnk_0873B9EC[n]);
    t = gCurTask;
    t->unk50 = (t->unk4A + (gUnk_0873B9EC + 8)[n] + 4) << 16;
    a = (gUnk_0873B9EC + 16)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk54 = b;
    a = (gUnk_0873B9EC + 24)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk58 = b;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    v = gCurTask;
    c = (gUnk_0873B9EC + 32)[n];
    b = c << 8;
    if (c & 0x8000)
        b |= 0xFF000000;
    v->unk54 = b;
    a = (gUnk_0873B9EC + 40)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->unk58 = b;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->unk54 = 0;
    w->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void sub_08053f70(void)
{
    struct Task *u;
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
    s32 f;

    u = gCurTask;
    u->unk00 = (u32)TaskMove;
    if (u->unk88->unk37 == 2)
        u->unk0C = (u32)TaskDrawScreen;
    else
        u->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 8;
    gCurTask->unk38 = gUnk_0874C500;
    n = RandomRange(8);
    TaskStepForward(gUnk_0873B9EC[n]);
    t = gCurTask;
    t->unk50 = (t->unk4A + (gUnk_0873B9EC + 8)[n] + 4) << 16;
    a = (gUnk_0873B9EC + 16)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk54 = b;
    a = (gUnk_0873B9EC + 24)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk58 = b;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    v = gCurTask;
    c = (gUnk_0873B9EC + 32)[n];
    d = c << 8;
    if (c & 0x8000)
        d |= 0xFF000000;
    v->unk54 = d;
    a = (gUnk_0873B9EC + 40)[n];
    d = a << 8;
    if (a & 0x8000)
        d |= 0xFF000000;
    v->unk58 = d;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->unk54 = 0;
    w->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void sub_080540d0(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C600;
    switch (t->unk18 & 0xFF0000)
    {
    case 0:
        t->unk28 = 0;
        t->unk2C = t->unk88->unk04;
        t->unk30 = t->unk18 & 0xFF;
        t->unk04 = (u32)sub_08054298;
        do
        {
            struct Task *u = gCurTask;
            struct Task *p;

            if (u->unk43 == 1)
                u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 - 6) << 16;
            else
                u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 + 6) << 16;
            u->unk50 = (((struct Task *)u->unk8C)->unk4A + 8) << 16;
            TaskStop();
            gCurTask->unk60 = -0x2000;
            TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            sub_0805afac(gCurTask->unk88->unk00, 6, 0x10000);
            TaskYieldTrampoline(1);
            gCurTask->unk3C -= 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C -= 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            TaskStop();
        } while (gCurTask->unk28 == 0);
        break;
    case 0x10000:
        gCurTask->unk4C = (gCurTask->unk48 + RandomSpreadFacing(-8, 1, 8)) << 16;
        gCurTask->unk50 = (gCurTask->unk4A + RandomSpread(-8, 1, 8)) << 16;
        TaskSetMotionXFacing(0x5A5A5A5A, 0x4000);
        gCurTask->unk60 = -0x4000;
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(2);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void sub_08054298(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0)
    {
        u8 m = t->unk88->unk04;

        if (m != t->unk2C || m == 16)
        {
            t->unk28 = 1;
            return;
        }
        switch (t->unk18 & 0xFF00)
        {
        case 0:
            if (((struct Task *)t->unk8C)->unk7A == 0)
                t->unk28 = 1;
        case 0x100:
        {
            struct Task *u = gCurTask;

            if (((u8 *)u)[24] != 0 && --u->unk30 == 0)
                u->unk28++;
            break;
        }
        case 0x200:
            if (sub_0803fd20(t->unk44) != 4)
                gCurTask->unk28++;
            break;
        }
    }
}
