#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_57494.c (0x08057494-0x08057CDF, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 32-34.  Variant 32 (sub_08057494, M12/M13) is a nine-way jump
 * table over its sub-state (cases 5-8 share one arm) with the per-sub-state
 * rows gUnk_0873BAB0[][3] (8.8 x velocity, 8.8 y acceleration, frame); its
 * sub-states respawn variant 32 and install sub_08057a10, which kills the
 * task once the player leaves mode 13 or the spawner's Task.unk7B bit 0 is
 * set.  Variant 33 (sub_08057a48, spawned by M14's task type #6) is a short
 * animation from gUnk_08751BF4.  Variant 34 (sub_08057ad4, M12) has two
 * sub-states with the draw hooks TaskDrawWorldInViewOrFree and sub_0805af80 (shared with
 * variant 48) and the kill test sub_08057c98 (player mode 13). */

extern u32 gUnk_08751CF0[];
extern u16 gUnk_0873BAB0[][3];   /* per sub-state: 8.8 x velocity, 8.8 y acceleration, frame */
extern u32 gUnk_0874C600[];
extern u32 gUnk_0874C718[];
extern u32 gUnk_08751BF4[];
extern u16 gUnk_0873BAE6[];
extern u32 gUnk_08751D80[];
extern u32 gUnk_08751D50[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawWorld(void);
void TaskDrawWorldInViewOrFree(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
void TaskSetFrame(s32 a);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_0805af80(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_08057a10(void);
void sub_08057c98(void);

void sub_08057494(void)
{
    struct Task *t;
    u16 *row;
    s32 s;

    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_08751CF0;
    s = t->unk18 & 15;
    row = gUnk_0873BAB0[s];
    switch (s)
    {
    case 0:
        {
            struct Task *u = gCurTask;

            u->unk00 = (u32)TaskMove;
            u->unk38 = gUnk_0874C600;
            u->unk04 = (u32)sub_08057a10;
            u->unk6C = 0;
        }
        do
        {
            {
                struct Task *v = gCurTask;
                struct Task *p;

                if (v->unk43 == 1)
                    v->unk4C = ((p = (struct Task *)v->unk8C)->unk48 - 6) << 16;
                else
                    v->unk4C = ((p = (struct Task *)v->unk8C)->unk48 + 6) << 16;
                v->unk50 = (((struct Task *)v->unk8C)->unk4A + 8) << 16;
            }
            {
                s32 a = row[0];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                TaskSetMotionXFacing(b, 0x5A5A5A5A);
            }
            {
                struct Task *w = gCurTask;

                {
                    s32 a = row[1];
                    s32 b = a << 8;

                    if (a & 0x8000)
                        b |= 0xFF000000;
                    w->unk60 = b;
                }
            }
            TaskSetFrameByFacing(row[2]);
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 32, 1);
            TaskYieldTrampoline(1);
            gCurTask->unk3C -= 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C -= 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            TaskStop();
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        break;
    case 1:
        {
            struct Task *u = gCurTask;

            u->unk00 = (u32)TaskMove;
            u->unk38 = gUnk_0874C600;
        }
        gCurTask->unk4C = (gCurTask->unk48 + RandomSpreadFacing(-8, 1, 8)) << 16;
        gCurTask->unk50 = (gCurTask->unk4A + RandomSpread(-8, 1, 8)) << 16;
        {
            s32 a = row[0];
            s32 b = a << 8;

            if (a & 0x8000)
                b |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, b);
        }
        {
            struct Task *w = gCurTask;

            {
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->unk60 = b;
            }
        }
        TaskSetFrameByFacing(row[2]);
        TaskYieldTrampoline(2);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(1);
        break;
    case 2:
        {
            struct Task *u = gCurTask;

            u->unk00 = (u32)TaskMove;
            u->unk38 = gUnk_0874C718;
        }
        gCurTask->unk4C = (RandomSpreadFacing(-8, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
        gCurTask->unk50 = (RandomSpread(-4, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A - 8) << 16;
        TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
        gCurTask->unk60 = -0x6000;
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(1);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 24;
        TaskYieldTrampoline(2);
        break;
    case 3:
        {
            struct Task *u = gCurTask;

            u->unk00 = (u32)TaskMove;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0x1808;
            u->unk04 = (u32)sub_08057a10;
            u->unk6C = 0;
        }
        do
        {
            TaskSetMotionXFacing(-0x10000, -0x2000);
            gCurTask->unk4C = (RandomSpreadFacing(-16, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetFrameByFacing(16);
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(2);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        break;
    case 4:
        {
            struct Task *u = gCurTask;

            u->unk00 = (u32)TaskMove;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0x1808;
            u->unk04 = (u32)sub_08057a10;
            u->unk6C = 0;
        }
        do
        {
            TaskSetMotionXFacing(-0x20000, -0x2000);
            gCurTask->unk4C = (RandomSpreadFacing(-16, 1, 24) + ((struct Task *)gCurTask->unk8C)->unk48) << 16;
            gCurTask->unk50 = (RandomSpread(-4, 1, 16) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
            TaskSetFrameByFacing(16);
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C += 2;
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        {
            struct Task *u = gCurTask;

            u->unk00 = (u32)TaskMoveRelativeToParent;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0xF008;
        }
        {
            s32 a = row[0];
            s32 b = a << 8;

            if (a & 0x8000)
                b |= 0xFF000000;
            TaskSetMotionXFacing(b, 0x5A5A5A5A);
        }
        {
            struct Task *w = gCurTask;

            {
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->unk58 = b;
            }
        }
        gCurTask->unk4C = RandomSpreadFacing(-4, 1, 8) << 16;
        gCurTask->unk50 = RandomSpread(-4, 1, 8) << 16;
        gCurTask->unk3C = row[2];
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0;
        gCurTask->unk58 = 0;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void sub_08057a10(void)
{
    if (gCurTask->unk88->unk04 != 13 || (((struct Task *)gCurTask->unk8C)->unk7B & 1))
        TaskFree(gCurTaskIdx);
}

void sub_08057a48(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 8;
    t = gCurTask;
    t->unk38 = gUnk_08751BF4;
    t->unk40 = gTasks[t->unk44].unk40;
    t->unk3C = gUnk_0873BAE6[t->unk18 & 15];
    TaskYieldTrampoline(1);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08057ad4(void)
{
    struct Task *t = gCurTask;

    t->unk00 = (u32)TaskMove;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk0C = (u32)sub_0805af80;
        t->unk04 = (u32)sub_08057c98;
        t->unk42 = 8;
        {
            struct Task *u = gCurTask;

            u->unk38 = gUnk_08751D80;
            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 4;
            u->unk3C = 0xFFFF;
        }
        while (((struct Task *)gCurTask->unk8C)->unk73 == 0)
            TaskYieldTrampoline(1);
        for (;;)
        {
            {
                struct Task *v = gCurTask;

                v->unk4C = ((struct Task *)v->unk8C)->unk48 << 16;
                v->unk50 = ((struct Task *)v->unk8C)->unk4A << 16;
            }
            TaskSetFrame(0);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            {
                struct Task *w = gCurTask;

                if (((struct Task *)w->unk8C)->unk73 == 2)
                {
                    w->unk54 = 0;
                    w->unk3C = 0xFFFF;
                    while (((struct Task *)gCurTask->unk8C)->unk73 == 2)
                        TaskYieldTrampoline(1);
                }
            }
        }
    case 1:
        gCurTask->unk0C = (u32)TaskDrawWorldInViewOrFree;
        gCurTask->unk42 = 5;
        {
            struct Task *u = gCurTask;

            u->unk38 = gUnk_08751D50;
            if (u->unk43 == 1)
                u->unk4C = (((struct Task *)u->unk8C)->unk48 - 8) << 16;
            else
                u->unk4C = (((struct Task *)u->unk8C)->unk48 + 8) << 16;
        }
        {
            struct Task *v = gCurTask;

            v->unk50 = ((struct Task *)v->unk8C)->unk4A << 16;
            if (((struct Task *)v->unk8C)->unk28 == 0)
            {
                v->unk40 = (((struct Task *)v->unk8C)->unk40 + 0x1800) | 8;
                TaskSetFrameByFacing(0);
            }
            else
            {
                v->unk40 = 0;
                TaskSetFrameByFacing(6);
            }
        }
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(4);
        break;
    }
    TaskExitTrampoline();
}

void sub_08057c98(void)
{
    struct Task *t = gCurTask;
    u8 s;

    if (t->unk88->unk04 != 13 || (s = ((struct Task *)t->unk8C)->unk73) == 3 || s == 4)
        TaskFree(gCurTaskIdx);
    else
        t->unk43 = ((struct Task *)t->unk8C)->unk43;
}
