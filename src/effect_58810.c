#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_58810.c (0x08058810-0x0805956F, issue #89).
 *
 * Task type #7: variants 40-41. */

extern u32 gUnk_08751E5C[];
extern u32 gUnk_08751E7C[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 sub_08002ee8(u32 range);                       /* RNG: 0 .. range-1 */
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_08058e80(void);
void sub_080594e0(void);

void sub_08058810(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_08005d9c;
    t->unk04 = (u32)sub_08058e80;
    t->unk38 = gUnk_08751E5C;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 8;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk42 = 5;
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(16);
        while (1)
        {
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-12, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x800;
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x400;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = -0x400;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x1000);
            gUnk_03002490->unk60 = -0x800;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-12, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x800;
            gUnk_03002490->unk3C = 5;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x400;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = -0x400;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x1000);
            gUnk_03002490->unk60 = -0x800;
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-12, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x800;
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x400;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = -0x400;
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x1000);
            gUnk_03002490->unk60 = -0x800;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-12, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x800;
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(-4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = 0x400;
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(12, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 8) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x2000);
            gUnk_03002490->unk60 = -0x400;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = sub_080064dc(16, 1, 32) << 16;
            gUnk_03002490->unk50 = sub_080064ac(4, 1, 16) << 16;
            sub_080062c4();
            sub_080061c0(0x5A5A5A5A, 0x1000);
            gUnk_03002490->unk60 = -0x800;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(16);
            if (gUnk_03002490->unk28 != 0)
                break;
        }
        break;
    case 1:
        gUnk_03002490->unk42 = 8;
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(12);
        do
        {
            gUnk_03002490->unk4C = sub_080064dc(32, 1, 8) << 16;
            gUnk_03002490->unk50 = sub_080064dc(4, 1, 8) << 16;
            sub_080061c0(0x10000, 0x4000);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = (sub_08002ee8(32) - 16) << 8;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 == 0);
        break;
    case 2:
        gUnk_03002490->unk42 = 8;
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        do
        {
            gUnk_03002490->unk4C = sub_080064dc(20, 1, 12) << 16;
            gUnk_03002490->unk50 = sub_080064dc(0, 1, 8) << 16;
            sub_080061c0(0x8000, 0x2000);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = (sub_08002ee8(32) - 16) << 8;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 == 0);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08058e80(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 == 0 && (t->unk88->unk04 != 13 || t->unk43 != ((struct Task *)t->unk8C)->unk43))
        t->unk28 = 1;
    if (!(gUnk_03002490->unk88->unk40 & 0x100) && (((struct Task *)gUnk_03002490->unk8C)->unk7B & 1))
        sub_08005654(gCurTaskIdx);
    if (gUnk_03002490->unk88->unk0D != 13)
        sub_08005654(gCurTaskIdx);
}

void sub_08058f10(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_080594e0;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751E7C;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 12;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        while (1)
        {
            gUnk_03002490->unk4C = (sub_080064ac(-24, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(4, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(-0x18000, 0x2000);
            gUnk_03002490->unk58 = -0xC000;
            gUnk_03002490->unk60 = -0x1800;
            gUnk_03002490->unk3C = 8;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = (sub_080064ac(-24, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(-0x18000, 0x2000);
            gUnk_03002490->unk58 = -0xC000;
            gUnk_03002490->unk60 = -0x1800;
            gUnk_03002490->unk3C = 8;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            if (gUnk_03002490->unk28 != 0)
                break;
        }
        break;
    case 1:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(5);
        while (gUnk_03002490->unk28 == 0)
        {
            gUnk_03002490->unk4C = (sub_080064ac(-20, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(16, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(-0xC000, 0x1000);
            gUnk_03002490->unk58 = -0x14000;
            gUnk_03002490->unk60 = -0x2000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = (sub_080064ac(-20, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(-0xC000, 0x1000);
            gUnk_03002490->unk58 = -0x14000;
            gUnk_03002490->unk60 = -0x2000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        break;
    case 2:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(10);
        while (gUnk_03002490->unk28 == 0)
        {
            gUnk_03002490->unk4C = (sub_080064ac(-12, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(20, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0xC000, -0x1000);
            gUnk_03002490->unk58 = -0x14000;
            gUnk_03002490->unk60 = -0x2000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk50 = (sub_080064ac(4, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0xC000, -0x1000);
            gUnk_03002490->unk58 = -0x14000;
            gUnk_03002490->unk60 = -0x2000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        break;
    case 3:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(15);
        while (gUnk_03002490->unk28 == 0)
        {
            gUnk_03002490->unk4C = (sub_080064ac(-8, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(4, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0x18000, -0x2000);
            gUnk_03002490->unk58 = -0xC000;
            gUnk_03002490->unk60 = -0x1800;
            gUnk_03002490->unk3C = 14;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = (sub_080064ac(-8, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0x18000, -0x2000);
            gUnk_03002490->unk58 = -0xC000;
            gUnk_03002490->unk60 = -0x1800;
            gUnk_03002490->unk3C = 14;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        }
        break;
    }
    TaskDispatchTrampoline();
}

void sub_080594e0(void)
{
    struct Task *t = gUnk_03002490;
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
    if (!(gUnk_03002490->unk88->unk40 & 0x100) && (((struct Task *)gUnk_03002490->unk8C)->unk7B & 1))
        sub_08005654(gCurTaskIdx);
}
