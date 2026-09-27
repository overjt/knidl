#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_58810.c (0x08058810-0x0805956F, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 40 and 41, spawned by M13's ability get and actions.  Variant 40
 * (sub_08058810, 1648 bytes) rides on its spawner through three sub-states,
 * each a long yield script whose every step stops once Task.unk28 is set;
 * its callback sub_08058e80 sets it when the player leaves mode 13 or its
 * facing no longer matches the spawner's, and kills the task when the
 * ability is no longer 13 or PlayerState.unk40 bit 8 is clear while the
 * spawner's Task.unk7B bit 0 is set.  Variant 41 (sub_08058f10, 1488 bytes)
 * is four sub-states in world space (gUnk_08751E7C); its callback
 * sub_080594e0 sets Task.unk28 when the player leaves mode 13 (or, while
 * PlayerState.unk40 bit 8 is clear, when the spawner's Task.unk73 is not 1)
 * and kills it on the same unk40/unk7B test. */

extern u32 gUnk_08751E5C[];
extern u32 gUnk_08751E7C[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawWorld(void);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_08058e80(void);
void sub_080594e0(void);

void sub_08058810(void)
{
    struct Task *t = gCurTask;

    t->unk00 = (u32)TaskMoveRelativeToParent;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk04 = (u32)sub_08058e80;
    t->unk38 = gUnk_08751E5C;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 8;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk42 = 5;
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(16);
        while (1)
        {
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x800;
            gCurTask->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x400;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = -0x400;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->unk60 = -0x800;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x800;
            gCurTask->unk3C = 5;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x400;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = -0x400;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->unk60 = -0x800;
            gCurTask->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x800;
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x400;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = -0x400;
            gCurTask->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->unk60 = -0x800;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-12, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x800;
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(-4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = 0x400;
            gCurTask->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(12, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 8) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x2000);
            gCurTask->unk60 = -0x400;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = RandomSpreadFacing(16, 1, 32) << 16;
            gCurTask->unk50 = RandomSpread(4, 1, 16) << 16;
            TaskStop();
            TaskSetMotionXFacing(0x5A5A5A5A, 0x1000);
            gCurTask->unk60 = -0x800;
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk28 != 0)
                break;
        }
        break;
    case 1:
        gCurTask->unk42 = 8;
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(12);
        do
        {
            gCurTask->unk4C = RandomSpreadFacing(32, 1, 8) << 16;
            gCurTask->unk50 = RandomSpreadFacing(4, 1, 8) << 16;
            TaskSetMotionXFacing(0x10000, 0x4000);
            gCurTask->unk58 = 0;
            gCurTask->unk60 = (RandomRange(32) - 16) << 8;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    case 2:
        gCurTask->unk42 = 8;
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        do
        {
            gCurTask->unk4C = RandomSpreadFacing(20, 1, 12) << 16;
            gCurTask->unk50 = RandomSpreadFacing(0, 1, 8) << 16;
            TaskSetMotionXFacing(0x8000, 0x2000);
            gCurTask->unk58 = 0;
            gCurTask->unk60 = (RandomRange(32) - 16) << 8;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 == 0);
        break;
    }
    TaskExitTrampoline();
}

void sub_08058e80(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0 && (t->unk88->unk04 != 13 || t->unk43 != ((struct Task *)t->unk8C)->unk43))
        t->unk28 = 1;
    if (!(gCurTask->unk88->unk40 & 0x100) && (((struct Task *)gCurTask->unk8C)->unk7B & 1))
        TaskFree(gCurTaskIdx);
    if (gCurTask->unk88->unk0D != 13)
        TaskFree(gCurTaskIdx);
}

void sub_08058f10(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_080594e0;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751E7C;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 12;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        while (1)
        {
            gCurTask->unk4C = (RandomSpread(-24, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(4, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(-0x18000, 0x2000);
            gCurTask->unk58 = -0xC000;
            gCurTask->unk60 = -0x1800;
            gCurTask->unk3C = 8;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = (RandomSpread(-24, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(-0x18000, 0x2000);
            gCurTask->unk58 = -0xC000;
            gCurTask->unk60 = -0x1800;
            gCurTask->unk3C = 8;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            if (gCurTask->unk28 != 0)
                break;
        }
        break;
    case 1:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(5);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->unk4C = (RandomSpread(-20, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(16, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(-0xC000, 0x1000);
            gCurTask->unk58 = -0x14000;
            gCurTask->unk60 = -0x2000;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = (RandomSpread(-20, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(-0xC000, 0x1000);
            gCurTask->unk58 = -0x14000;
            gCurTask->unk60 = -0x2000;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        break;
    case 2:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(10);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->unk4C = (RandomSpread(-12, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(20, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0xC000, -0x1000);
            gCurTask->unk58 = -0x14000;
            gCurTask->unk60 = -0x2000;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk50 = (RandomSpread(4, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0xC000, -0x1000);
            gCurTask->unk58 = -0x14000;
            gCurTask->unk60 = -0x2000;
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        break;
    case 3:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(15);
        while (gCurTask->unk28 == 0)
        {
            gCurTask->unk4C = (RandomSpread(-8, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(4, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0x18000, -0x2000);
            gCurTask->unk58 = -0xC000;
            gCurTask->unk60 = -0x1800;
            gCurTask->unk3C = 14;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            if (gCurTask->unk28 != 0)
                break;
            gCurTask->unk4C = (RandomSpread(-8, 1, 32) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-12, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetMotionXFacing(0x18000, -0x2000);
            gCurTask->unk58 = -0xC000;
            gCurTask->unk60 = -0x1800;
            gCurTask->unk3C = 14;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        }
        break;
    }
    TaskExitTrampoline();
}

void sub_080594e0(void)
{
    struct Task *t = gCurTask;
    struct PlayerState *p = t->unk88;

    if (!(p->unk40 & 0x100))
    {
        if (t->unk28 == 0 && (p->unk04 != 13 || ((struct Task *)t->unk8C)->unk73 != 1))
            t->unk28 = 1;
    }
    else
    {
        if (t->unk28 == 0 && p->unk04 != 13)
            t->unk28 = 1;
    }
    if (!(gCurTask->unk88->unk40 & 0x100) && (((struct Task *)gCurTask->unk8C)->unk7B & 1))
        TaskFree(gCurTaskIdx);
}
