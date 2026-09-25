#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_55b24.c (0x08055B24-0x08056447, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 22-25, spawned by M10's action 16 (sub_08037ed8) and by M11.
 * Each is a loop of short animations around its spawner (22-24 at random
 * offsets from sub_080064ac/sub_080064dc; tables gUnk_0874C718,
 * gUnk_0874C7A4, gUnk_0874C7B4, gUnk_0874C7CC) that ends once its companion
 * sets Task.unk28: sub_08055d24, sub_080560fc, sub_08056300 and sub_08056428
 * do so when the player leaves mode 17 (in sub-state 0 of 22 and 23 also
 * when the spawner's Task.unk7A is set).  Variant 24 (sub_0805614c) sets its
 * velocities with sub_0800625c and alternates two directions; variant 25
 * (sub_08056320) stays on the spawner's position, with Task.unk43 = 1 when
 * gUnk_03001EA4 bit 0 is set and the inherited facing flipped otherwise;
 * variant 23 (sub_08055d74, 904 bytes) is the longest. */

extern u32 gUnk_0874C718[];
extern u32 gUnk_0874C7A4[];
extern u32 gUnk_0874C7B4[];
extern u32 gUnk_0874C7CC[];
extern u16 gUnk_03001EA4;

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080059d8(void);
void sub_08005d9c(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_08055d24(void);
void sub_080560fc(void);
void sub_08056300(void);
void sub_08056428(void);

void sub_08055b24(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_08055d24;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C718;
    sub_080062c4();
    gUnk_03002490->unk28 = 0;
    if ((gUnk_03002490->unk18 & 15) == 0)
    {
        do
        {
            sub_080061c0(0, -0x2000);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = -0x2000;
            gUnk_03002490->unk4C = (sub_080064dc(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_0800617c(0);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 == 0);
    }
    else
    {
        do
        {
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk58 = -0x8000;
            sub_0800617c(18);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            sub_0800617c(14);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk58 = -0x10000;
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk58 = -0x20000;
            sub_0800617c(24);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
        } while (gUnk_03002490->unk28 == 0);
    }
    TaskDispatchTrampoline();
}

void sub_08055d24(void)
{
    struct Task *t = gUnk_03002490;

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

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_080560fc;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C7A4;
    t->unk28 = 0;
    sub_080062c4();
    if ((gUnk_03002490->unk18 & 15) == 0)
    {
        while (gUnk_03002490->unk28 == 0)
        {
            gUnk_03002490->unk4C = (sub_080064dc(-16, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-16, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 3;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk4C = (sub_080064dc(-16, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-16, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 1;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
        }
    }
    else
    {
        while (gUnk_03002490->unk28 == 0)
        {
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            gUnk_03002490->unk3C = 3;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(2);
        }
    }
    TaskDispatchTrampoline();
}

void sub_080560fc(void)
{
    struct Task *t = gUnk_03002490;

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

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_08056300;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C7B4;
    t->unk28 = 0;
    sub_080062c4();
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1 && gUnk_03002490->unk28 == 0; gUnk_03002490->unk6C++)
    {
        gUnk_03002490->unk4C = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
        sub_0800625c(0x4000, -0x700, 0x5A5A5A5A, -0x4000, -0x1000, 0x5A5A5A5A);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6E = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6E++;
        } while (gUnk_03002490->unk6E <= 4);
        if (gUnk_03002490->unk28 != 0)
            break;
        gUnk_03002490->unk4C = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
        sub_0800625c(-0x4000, 0x700, 0x5A5A5A5A, -0x4000, -0x1000, 0x5A5A5A5A);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6E = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6E++;
        } while (gUnk_03002490->unk6E <= 4);
    }
    TaskDispatchTrampoline();
}

void sub_08056300(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 == 0 && t->unk88->unk04 != 17)
        t->unk28 = 1;
}

void sub_08056320(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_08056428;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C7CC;
    t->unk28 = 0;
    sub_080062c4();
    if (gUnk_03001EA4 & 1)
        gUnk_03002490->unk43 = 1;
    else
        gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    gUnk_03002490->unk58 = -0x8000;
    while (gUnk_03002490->unk28 == 0)
    {
        struct Task *u = gUnk_03002490;

        u->unk4C = ((struct Task *)u->unk8C)->unk48 << 16;
        u->unk50 = ((struct Task *)u->unk8C)->unk4A << 16;
        sub_0800617c(0);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_08056428(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 == 0 && t->unk88->unk04 != 17)
        t->unk28 = 1;
}
