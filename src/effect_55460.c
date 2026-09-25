#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_55460.c (0x08055460-0x08055B23, issue #89).
 *
 * Task type #7: variants 16-21. */

extern u32 gUnk_0874C960[];
extern u32 gUnk_0874C980[];
extern u32 gUnk_0874C804[];
extern u32 gUnk_0874C784[];
extern s16 gUnk_03002348;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_0874C600[];
extern u32 gUnk_0874C780[];
extern s16 gUnk_0300244C;

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005a74(void);
void sub_08005d9c(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_080557d4(void);
void sub_08055abc(void);

void sub_08055460(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_08005a74;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C960;
    t->unk4C = ((struct Task *)t->unk8C)->unk48 << 16;
    t->unk50 = (((struct Task *)t->unk8C)->unk4A - 8) << 16;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_08055520(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_08005a74;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C980;
    t->unk4C = ((struct Task *)t->unk8C)->unk48 << 16;
    t->unk50 = ((struct Task *)t->unk8C)->unk4A << 16;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 11;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 11;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    TaskDispatchTrampoline();
}

void sub_0805569c(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_08005a74;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C804;
    t->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0805574c(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk04 = (u32)sub_080557d4;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk48 = ((struct Task *)t->unk8C)->unk48;
    t->unk4A = ((struct Task *)t->unk8C)->unk4A;
    t->unk3C = 0;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 6);
    gUnk_03002490->unk88->unk40 |= 0x80;
    TaskDispatchTrampoline();
}

void sub_080557d4(void)
{
    sub_08001a94(gUnk_03002490->unk42, gUnk_0874C784[gUnk_03002490->unk3C], 0, 0,
                 gUnk_03002490->unk48 - 48 - gUnk_03002348,
                 gUnk_03002490->unk4A - gUnk_030023E4);
    sub_08001a94(gUnk_03002490->unk42, gUnk_0874C784[gUnk_03002490->unk3C], 0, 0,
                 gUnk_03002490->unk48 + 48 - gUnk_03002348,
                 gUnk_03002490->unk4A - gUnk_030023E4);
}

void sub_0805587c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *p;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C600;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk43 = 1;
        goto common;
    case 1:
        t->unk43 = -1;
    common:
        sub_080062c4();
        u = gUnk_03002490;
        if (u->unk43 == 1)
            u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 - 6) << 16;
        else
            u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 + 6) << 16;
        u->unk50 = (((struct Task *)u->unk8C)->unk4A + 8) << 16;
        gUnk_03002490->unk60 = -0x2000;
        sub_080061c0(-0x30000, 0x5A5A5A5A);
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 20, 2);
        sub_080061c0(0x5A5A5A5A, 0);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        break;
    case 2:
        gUnk_03002490->unk4C = (gUnk_03002490->unk48 + sub_080064dc(-8, 1, 8)) << 16;
        gUnk_03002490->unk50 = (gUnk_03002490->unk4A + sub_080064ac(-8, 1, 8)) << 16;
        sub_080061c0(0x5A5A5A5A, 0x4000);
        gUnk_03002490->unk60 = -0x4000;
        sub_0800617c(4);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(1);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08055a40(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_08055abc;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C780;
    t->unk4C = 0;
    t->unk50 = -0xC0000;
    t->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    TaskDispatchTrampoline();
}

void sub_08055abc(void)
{
    if (gUnk_0300244C == 0)
    {
        u8 a = gUnk_03002490->unk88->unk04;

        if (a == 13 || a == 20 || a == 16 || a == 18)
            sub_08005654(gCurTaskIdx);
    }
    else
    {
        u8 a = gUnk_03002490->unk88->unk04;

        if (a == 13 || a == 20 || a == 16 || a == 18)
            gUnk_03002490->unk3C = 0xFFFF;
    }
}
