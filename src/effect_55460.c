#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_55460.c (0x08055460-0x08055B23, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 16-21 (entry 16, sub_0805569c, sits after entries 17 and 18 in
 * the ROM).  Variants 16, 17 and 19 are spawned by M16 (16 also by M17's
 * actor core), 18 and 20 by M10, 21 by M11. 16 (sub_0805569c), 17
 * (sub_08055460) and 18 (sub_08055520) are animations that stay put
 * (TaskUpdatePixelPos), from gUnk_0874C804, gUnk_0874C960 and gUnk_0874C980; 19
 * (sub_0805574c) has no draw hook and draws itself through its Task.unk04
 * callback sub_080557d4 (QueueSprite); 20 (sub_0805587c) is a
 * three-sub-state puff whose sub-states 0 and 1 (one differing store, then a
 * shared body) spawn its sub-state 2; 21 (sub_08055a40) rides on its
 * spawner, and sub_08055abc kills it (or, while gUnk_0300244C is set, hides
 * it) when the player is in mode 13, 16, 18 or 20. */

extern u32 gUnk_0874C960[];
extern u32 gUnk_0874C980[];
extern u32 gUnk_0874C804[];
extern u32 gUnk_0874C784[];
extern s16 gSpriteCameraX;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gSpriteCameraY;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_0874C600[];
extern u32 gUnk_0874C780[];
extern s16 gUnk_0300244C;

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskUpdatePixelPos(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_080557d4(void);
void sub_08055abc(void);

void sub_08055460(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskUpdatePixelPos;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C960;
    t->unk4C = ((struct Task *)t->unk8C)->unk48 << 16;
    t->unk50 = (((struct Task *)t->unk8C)->unk4A - 8) << 16;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_08055520(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskUpdatePixelPos;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 8;
    t = gCurTask;
    t->unk38 = gUnk_0874C980;
    t->unk4C = ((struct Task *)t->unk8C)->unk48 << 16;
    t->unk50 = ((struct Task *)t->unk8C)->unk4A << 16;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 6;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 7;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 7;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 11;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 8;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 11;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    TaskExitTrampoline();
}

void sub_0805569c(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskUpdatePixelPos;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 8;
    t = gCurTask;
    t->unk38 = gUnk_0874C804;
    t->unk3C = 0;
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
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0805574c(void)
{
    struct Task *t;

    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    gCurTask->unk04 = (u32)sub_080557d4;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk48 = ((struct Task *)t->unk8C)->unk48;
    t->unk4A = ((struct Task *)t->unk8C)->unk4A;
    t->unk3C = 0;
    TaskYieldTrampoline(4);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk88->unk40 |= 0x80;
    TaskExitTrampoline();
}

void sub_080557d4(void)
{
    QueueSprite(gCurTask->unk42, gUnk_0874C784[gCurTask->unk3C], 0, 0,
                 gCurTask->unk48 - 48 - gSpriteCameraX,
                 gCurTask->unk4A - gSpriteCameraY);
    QueueSprite(gCurTask->unk42, gUnk_0874C784[gCurTask->unk3C], 0, 0,
                 gCurTask->unk48 + 48 - gSpriteCameraX,
                 gCurTask->unk4A - gSpriteCameraY);
}

void sub_0805587c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *p;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C600;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk43 = 1;
        goto common;
    case 1:
        t->unk43 = -1;
    common:
        TaskStop();
        u = gCurTask;
        if (u->unk43 == 1)
            u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 - 6) << 16;
        else
            u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 + 6) << 16;
        u->unk50 = (((struct Task *)u->unk8C)->unk4A + 8) << 16;
        gCurTask->unk60 = -0x2000;
        TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 20, 2);
        TaskSetMotionXFacing(0x5A5A5A5A, 0);
        TaskYieldTrampoline(1);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        break;
    case 2:
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

void sub_08055a40(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMoveRelativeToParent;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk04 = (u32)sub_08055abc;
    gCurTask->unk42 = 5;
    t = gCurTask;
    t->unk38 = gUnk_0874C780;
    t->unk4C = 0;
    t->unk50 = -0xC0000;
    t->unk6C = 0;
    do
    {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(8);
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    TaskExitTrampoline();
}

void sub_08055abc(void)
{
    if (gUnk_0300244C == 0)
    {
        u8 a = gCurTask->unk88->unk04;

        if (a == 13 || a == 20 || a == 16 || a == 18)
            TaskFree(gCurTaskIdx);
    }
    else
    {
        u8 a = gCurTask->unk88->unk04;

        if (a == 13 || a == 20 || a == 16 || a == 18)
            gCurTask->unk3C = 0xFFFF;
    }
}
