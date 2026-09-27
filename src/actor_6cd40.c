/* game_code_and_rodata 0x0806CD40-0x0806D22C (issue #64, module M18 batch 4c).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806CD40 0x0806D22C src/actor_6cd40.c --newpb
 *
 * The tail of the vehicle/ride block: sub_0806cd40's per-frame integrator
 * over the gTasks[] task table, the two sprite-list players
 * sub_0806ceb8 / sub_0806cf70, and the spawn/teardown helpers
 * sub_0806cffc / sub_0806d08c / sub_0806d148 / sub_0806d1e8.  Every literal
 * pool in this range ends exactly on the next function's entry, so any
 * symbols.csv boundary here is a valid carve point.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0874CB90[];
extern vs16 gTaskSlotTypes[];

extern void TaskYieldTrampoline(u32 a);
extern void TaskExitTrampoline(void);
extern void sub_080656b4(void);
extern void sub_0806523c(void);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetFrameByFacing(u32 a);
extern void TaskStop(void);
extern void sub_0806cd30(void);
extern u8 sub_08065f2c(s32 i);
extern void sub_08064a60(void);
extern s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);
extern u16 RandomSpread(s32 base, u8 scale, u8 amount);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void TaskSetFrame(s32 a);

void sub_0806cd40(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 j;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gCurTask;
    u->unk38 = gUnk_0874CB90;
    while (u->unk18 != 0 && gTaskSlotTypes[u->unk44] != -1)
    {
        if (sub_08065f2c(gCurTask->unk44) != 1)
            break;
        if ((s8)gTasks[j = gCurTask->unk44].unk7C == 3
         && gTasks[j].unk82 == 1)
            break;
        if ((s8)gTasks[j].unk7C == 4
         && (u16)(gTasks[j].unk82 - 2) <= 1)
            break;
        v = gCurTask;
        v->unk4C = (gTasks[j].unk48 + v->unk1C) << 16;
        v->unk50 = (gTasks[j].unk4A + v->unk20) << 16;
        v->unk60 = -0x2000;
        TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C += 2;
        TaskYieldTrampoline(1);
        sub_0806cd30();
        TaskYieldTrampoline(1);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C -= 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        TaskStop();
        u = gCurTask;
        u->unk18--;
    }
    TaskExitTrampoline();
}

void sub_0806ceb8(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gCurTask;
    u->unk38 = gUnk_0874CB90;
    sub_08064a60();
    gCurTask->unk4C = (RandomSpreadFacing(-8, 1, 8) + gCurTask->unk48) << 16;
    gCurTask->unk50 = (RandomSpread(-8, 1, 8) + gCurTask->unk4A) << 16;
    TaskSetMotionXFacing(0x5A5A5A5A, 0x4000);
    gCurTask->unk60 = -0x4000;
    TaskSetFrameByFacing(4);
    TaskYieldTrampoline(2);
    gCurTask->unk3C -= 2;
    TaskYieldTrampoline(2);
    gCurTask->unk3C -= 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0806cf70(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gCurTask;
    u->unk38 = gUnk_0874CB90;
    sub_08064a60();
    TaskSetMotionXFacing(0xFFFDC000, 0x1800);
    v = gCurTask;
    v->unk58 = -0x4000;
    v->unk60 = -0x2000;
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
void sub_0806cffc(s16 dx, s16 dy)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    i = (s16)sub_08064c1c(146, (s16)(dx + t->unk48), (s16)(dy + t->unk4A), 0);
    if (i != -1)
        gTasks[i].unk43 = 1;
    u = gCurTask;
    i = (s16)sub_08064c1c(146, (s16)(u->unk48 - dx), (s16)(dy + u->unk4A), 0);
    if (i != -1)
        gTasks[i].unk43 = 0xFF;
}

void sub_0806d08c(s16 a, s16 b, s16 c)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    i = (s16)sub_08064c1c(146, (s16)(t->unk48 + t->unk43 * a),
                          (s16)(c + t->unk4A), 0);
    if (i != -1)
        gTasks[i].unk43 = gCurTask->unk43;
    u = gCurTask;
    i = (s16)sub_08064c1c(146, (s16)(u->unk48 - b * u->unk43),
                          (s16)(c + u->unk4A), 0);
    if (i != -1)
        gTasks[i].unk43 = -gCurTask->unk43;
}

void sub_0806d148(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gCurTask;
    u->unk38 = gUnk_0874CB90;
    if (u->unk43 != 1 && u->unk43 != -1)
        u->unk43 = 1;
    TaskSetMotionXFacing(0x24000, 0xFFFFE800);
    v = gCurTask;
    v->unk58 = -0x4000;
    v->unk60 = -0x2000;
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(2);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

s32 sub_0806d1e8(s16 a, s16 b)
{
    struct Task *p;
    s32 i;

    i = sub_08064d6c(147, 0, 0, 0);
    if (i != -1)
    {
        p = &gTasks[i];
        p->unk24 = a;
        p->unk20 = b;
    }
    return i;
}
