#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_55b24.c (0x08055B24-0x08056447, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 22-25, spawned by M10's action 16 (sub_08037ed8) and by M11.
 * Each is a loop of short animations around its spawner (22-24 at random
 * offsets from RandomSpread/RandomSpreadFacing; tables gUnk_0874C718,
 * gUnk_0874C7A4, gUnk_0874C7B4, gUnk_0874C7CC) that ends once its companion
 * sets Task.unk28: sub_08055d24, sub_080560fc, sub_08056300 and sub_08056428
 * do so when the player leaves mode 17 (in sub-state 0 of 22 and 23 also
 * when the spawner's Task.unk7A is set).  Variant 24 (sub_0805614c) sets its
 * velocities with TaskSetMotion and alternates two directions; variant 25
 * (sub_08056320) stays on the spawner's position, with Task.unk43 = 1 when
 * gFrameCount bit 0 is set and the inherited facing flipped otherwise;
 * variant 23 (sub_08055d74, 904 bytes) is the longest. */

extern u32 gUnk_0874C718[];
extern u32 gUnk_0874C7A4[];
extern u32 gUnk_0874C7B4[];
extern u32 gUnk_0874C7CC[];
extern u16 gFrameCount;

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void TaskMove(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_08055d24(void);
void sub_080560fc(void);
void sub_08056300(void);
void sub_08056428(void);

void sub_08055b24(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_08055d24;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C718;
    TaskStop();
    gCurTask->unk28 = 0;
    if ((gCurTask->unk18 & 15) == 0)
    {
        do
        {
            TaskSetMotionXFacing(0, -0x2000);
            gCurTask->unk58 = 0;
            gCurTask->unk60 = -0x2000;
            gCurTask->unk4C = (RandomSpreadFacing(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(3);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
    }
    else
    {
        do
        {
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk58 = -0x8000;
            TaskSetFrameByFacing(18);
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            TaskSetFrameByFacing(14);
            TaskYieldTrampoline(2);
            gCurTask->unk58 = -0x10000;
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk58 = -0x20000;
            TaskSetFrameByFacing(24);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
        } while (gCurTask->unk28 == 0);
    }
    TaskExitTrampoline();
}

void sub_08055d24(void)
{
    struct Task *t = gCurTask;

    if ((t->unk18 & 15) == 0)
    {
        if (t->unk28 == 0 && (t->unk88->unk04 != 17 || ((struct Task *)t->unk8C)->unk7A != 0))
            t->unk28 = 1;
    }
    else
    {
        if (t->unk28 == 0 && t->unk88->unk04 != 17)
            t->unk28 = 1;
    }
}

void sub_08055d74(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_080560fc;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C7A4;
    t->unk28 = 0;
    TaskStop();
    if ((gCurTask->unk18 & 15) == 0)
    {
        while (gCurTask->unk28 == 0)
        {
            gCurTask->unk4C = (RandomSpreadFacing(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 3;
            TaskYieldTrampoline(2);
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 2;
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gCurTask)->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
            gCurTask->unk4C = (RandomSpreadFacing(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-16, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(2);
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gCurTask)->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
        }
    }
    else
    {
        while (gCurTask->unk28 == 0)
        {
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gCurTask)->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
            gCurTask->unk4C = (RandomSpreadFacing(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            gCurTask->unk3C = 3;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gCurTask)->unk3C = 0xFFFF;
            TaskYieldTrampoline(2);
        }
    }
    TaskExitTrampoline();
}

void sub_080560fc(void)
{
    struct Task *t = gCurTask;

    if ((t->unk18 & 15) == 0)
    {
        if (t->unk28 == 0 && (t->unk88->unk04 != 17 || ((struct Task *)t->unk8C)->unk7A != 0))
            t->unk28 = 1;
    }
    else
    {
        if (t->unk28 == 0 && t->unk88->unk04 != 17)
            t->unk28 = 1;
    }
}

void sub_0805614c(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_08056300;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C7B4;
    t->unk28 = 0;
    TaskStop();
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1 && gCurTask->unk28 == 0; gCurTask->unk6C++)
    {
        gCurTask->unk4C = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
        gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
        TaskSetMotion(0x4000, -0x700, 0x5A5A5A5A, -0x4000, -0x1000, 0x5A5A5A5A);
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk6E++;
        } while (gCurTask->unk6E <= 4);
        if (gCurTask->unk28 != 0)
            break;
        gCurTask->unk4C = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
        gCurTask->unk50 = (RandomSpread(-12, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
        TaskSetMotion(-0x4000, 0x700, 0x5A5A5A5A, -0x4000, -0x1000, 0x5A5A5A5A);
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk6E++;
        } while (gCurTask->unk6E <= 4);
    }
    TaskExitTrampoline();
}

void sub_08056300(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && t->unk88->unk04 != 17)
        t->unk28 = 1;
}

void sub_08056320(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_08056428;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C7CC;
    t->unk28 = 0;
    TaskStop();
    if (gFrameCount & 1)
        gCurTask->unk43 = 1;
    else
        gCurTask->unk43 = -gCurTask->unk43;
    gCurTask->unk58 = -0x8000;
    while (gCurTask->unk28 == 0)
    {
        struct Task *u = gCurTask;

        u->unk4C = ((struct Task *)u->unk8C)->unk48 << 16;
        u->unk50 = ((struct Task *)u->unk8C)->unk4A << 16;
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(3);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(3);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(3);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_08056428(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && t->unk88->unk04 != 17)
        t->unk28 = 1;
}
