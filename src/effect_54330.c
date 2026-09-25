#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_54330.c (0x08054330-0x08054A7F, issue #89).
 *
 * Task type #7: variants 7-11. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gUnk_03005550` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_0874C600[];
extern u32 gUnk_0874C520[];
extern u32 gUnk_0874C648[];
extern s8 gUnk_0873CB74[];              /* collision box passed to sub_0801c3a4 */
extern struct Unk03005550 gUnk_03005550;

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_08005d9c(void);
void sub_08005f30(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
u16 sub_0801c3a4(const s8 *p);
s32 sub_08022624(u16 x, u16 y);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_08054504(void);
void sub_08054838(void);
void sub_08054a44(void);

void sub_08054330(void)
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
        t->unk04 = (u32)sub_08054504;
        t->unk28 = 0;
        t->unk6C = 0;
        while (t->unk28 == 0)
        {
            u = gUnk_03002490;
            if (u->unk43 == 1)
                u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 - 6) << 16;
            else
                u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 + 6) << 16;
            u->unk50 = (((struct Task *)u->unk8C)->unk4A + 8) << 16;
            sub_080062c4();
            gUnk_03002490->unk60 = -0x2000;
            sub_080061c0(-0x30000, 0x5A5A5A5A);
            sub_0800617c(0);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            sub_0805afac(gUnk_03002490->unk88->unk00, 7, 1);
            sub_080061c0(0x5A5A5A5A, 0);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            t = gUnk_03002490;
            t->unk6C++;
            if ((s16)t->unk6C > 2)
                break;
        }
        break;
    case 1:
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

void sub_08054504(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 == 0 && (t->unk88->unk04 != 2 || ((struct Task *)t->unk8C)->unk7A == 0))
        t->unk28 = 1;
}

void sub_08054538(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *p;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C600;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk04 = (u32)sub_08054838;
        if (t->unk43 == 1)
            t->unk4C = ((p = (struct Task *)t->unk8C)->unk48 - 6) << 16;
        else
            t->unk4C = ((p = (struct Task *)t->unk8C)->unk48 + 6) << 16;
        t->unk50 = (((struct Task *)t->unk8C)->unk4A + 8) << 16;
        sub_080061c0(-0x30000, 0x5A5A5A5A);
        gUnk_03002490->unk60 = -0x2000;
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 8, 1);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(1);
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        u = gUnk_03002490;
        if (u->unk43 == 1)
            u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 - 6) << 16;
        else
            u->unk4C = ((p = (struct Task *)u->unk8C)->unk48 + 6) << 16;
        u->unk50 = (((struct Task *)u->unk8C)->unk4A + 8) << 16;
        sub_080061c0(-0x24000, 0x1000);
        gUnk_03002490->unk58 = -0x4000;
        gUnk_03002490->unk60 = -0x2000;
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(1);
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        v = gUnk_03002490;
        if (v->unk43 == 1)
            v->unk4C = ((p = (struct Task *)v->unk8C)->unk48 - 6) << 16;
        else
            v->unk4C = ((p = (struct Task *)v->unk8C)->unk48 + 6) << 16;
        v->unk50 = (((struct Task *)v->unk8C)->unk4A + 8) << 16;
        sub_080061c0(-0x12000, 0x1800);
        gUnk_03002490->unk58 = -0x4000;
        gUnk_03002490->unk60 = -0x2000;
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        sub_0800617c(6);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        break;
    case 1:
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

void sub_08054838(void)
{
    if (gUnk_03002490->unk88->unk04 != 7)
        sub_08005654(gCurTaskIdx);
}

void sub_0805485c(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C520;
    if (((struct Task *)t->unk8C)->unk58 > 0)
        sub_0803e34c(134, t->unk44);
    u = gUnk_03002490;
    u->unk4C = ((struct Task *)u->unk8C)->unk48 << 16;
    u->unk50 = (u16)u->unk18 << 16;
    u->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    TaskDispatchTrampoline();
}

void sub_080548f0(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C520;
    t->unk4C = ((struct Task *)t->unk8C)->unk48 << 16;
    t->unk50 = (u16)t->unk18 << 16;
    t->unk3C = 11;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 13;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080549a4(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005f30;
    gUnk_03002490->unk04 = (u32)sub_08054a44;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C648;
    if (t->unk43 == 1)
        t->unk4C = (t->unk48 + 4) << 16;
    if (sub_08022624(gUnk_03002490->unk4C >> 16, gUnk_03002490->unk50 >> 10) == 0)
    {
        gUnk_03002490->unk58 = -0x10000;
        for (;;)
        {
            gUnk_03002490->unk54 = -0x10000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk54 = 0x10000;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(6);
        }
    }
    TaskDispatchTrampoline();
}

void sub_08054a44(void)
{
    sub_0801c3a4(gUnk_0873CB74);
    if (gUnk_03002490->unk7B == 0 || gUnk_03005550.unk1 != 0)
        sub_08005654(gCurTaskIdx);
}
