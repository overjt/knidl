/* game_code_and_rodata 0x0806CD40-0x0806D22C (issue #64, module M18 batch 4c).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806CD40 0x0806D22C src/actor_6cd40.c --newpb
 *
 * The tail of the vehicle/ride block: sub_0806cd40's per-frame integrator
 * over the gUnk_03002790[] task table, the two sprite-list players
 * sub_0806ceb8 / sub_0806cf70, and the spawn/teardown helpers
 * sub_0806cffc / sub_0806d08c / sub_0806d148 / sub_0806d1e8.  Every literal
 * pool in this range ends exactly on the next function's entry, so any
 * symbols.csv boundary here is a valid carve point.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_0874CB90[];
extern vs16 gUnk_03004CA0[];

extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);
extern void sub_080656b4(void);
extern void sub_0806523c(void);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800617c(u32 a);
extern void sub_080062c4(void);
extern void sub_0806cd30(void);
extern u8 sub_08065f2c(s32 i);
extern void sub_08064a60(void);
extern s16 sub_080064dc(s32 base, u8 scale, u8 amount);
extern u16 sub_080064ac(s32 base, u8 scale, u8 amount);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void sub_08006338(s32 a);

void sub_0806cd40(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 j;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CB90;
    while (u->unk18 != 0 && gUnk_03004CA0[u->unk44] != -1)
    {
        if (sub_08065f2c(gUnk_03002490->unk44) != 1)
            break;
        if ((s8)gUnk_03002790[j = gUnk_03002490->unk44].unk7C == 3
         && gUnk_03002790[j].unk82 == 1)
            break;
        if ((s8)gUnk_03002790[j].unk7C == 4
         && (u16)(gUnk_03002790[j].unk82 - 2) <= 1)
            break;
        v = gUnk_03002490;
        v->unk4C = (gUnk_03002790[j].unk48 + v->unk1C) << 16;
        v->unk50 = (gUnk_03002790[j].unk4A + v->unk20) << 16;
        v->unk60 = -0x2000;
        sub_080061c0(0xFFFD0000, 0x5A5A5A5A);
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        sub_0806cd30();
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        sub_080062c4();
        u = gUnk_03002490;
        u->unk18--;
    }
    TaskDispatchTrampoline();
}

void sub_0806ceb8(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CB90;
    sub_08064a60();
    gUnk_03002490->unk4C = (sub_080064dc(-8, 1, 8) + gUnk_03002490->unk48) << 16;
    gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 8) + gUnk_03002490->unk4A) << 16;
    sub_080061c0(0x5A5A5A5A, 0x4000);
    gUnk_03002490->unk60 = -0x4000;
    sub_0800617c(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C -= 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0806cf70(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CB90;
    sub_08064a60();
    sub_080061c0(0xFFFDC000, 0x1800);
    v = gUnk_03002490;
    v->unk58 = -0x4000;
    v->unk60 = -0x2000;
    sub_0800617c(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    sub_0800617c(6);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}
void sub_0806cffc(s16 dx, s16 dy)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gUnk_03002490;
    i = (s16)sub_08064c1c(146, (s16)(dx + t->unk48), (s16)(dy + t->unk4A), 0);
    if (i != -1)
        gUnk_03002790[i].unk43 = 1;
    u = gUnk_03002490;
    i = (s16)sub_08064c1c(146, (s16)(u->unk48 - dx), (s16)(dy + u->unk4A), 0);
    if (i != -1)
        gUnk_03002790[i].unk43 = 0xFF;
}

void sub_0806d08c(s16 a, s16 b, s16 c)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gUnk_03002490;
    i = (s16)sub_08064c1c(146, (s16)(t->unk48 + t->unk43 * a),
                          (s16)(c + t->unk4A), 0);
    if (i != -1)
        gUnk_03002790[i].unk43 = gUnk_03002490->unk43;
    u = gUnk_03002490;
    i = (s16)sub_08064c1c(146, (s16)(u->unk48 - b * u->unk43),
                          (s16)(c + u->unk4A), 0);
    if (i != -1)
        gUnk_03002790[i].unk43 = -gUnk_03002490->unk43;
}

void sub_0806d148(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CB90;
    if (u->unk43 != 1 && u->unk43 != -1)
        u->unk43 = 1;
    sub_080061c0(0x24000, 0xFFFFE800);
    v = gUnk_03002490;
    v->unk58 = -0x4000;
    v->unk60 = -0x2000;
    sub_08006338(1);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    sub_08006338(7);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

s32 sub_0806d1e8(s16 a, s16 b)
{
    struct Task *p;
    s32 i;

    i = sub_08064d6c(147, 0, 0, 0);
    if (i != -1)
    {
        p = &gUnk_03002790[i];
        p->unk24 = a;
        p->unk20 = b;
    }
    return i;
}
