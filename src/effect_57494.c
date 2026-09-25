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
 * sub-states with the draw hooks sub_08005f30 and sub_0805af80 (shared with
 * variant 48) and the kill test sub_08057c98 (player mode 13). */

extern u32 gUnk_08751CF0[];
extern u16 gUnk_0873BAB0[][3];   /* per sub-state: 8.8 x velocity, 8.8 y acceleration, frame */
extern u32 gUnk_0874C600[];
extern u32 gUnk_0874C718[];
extern u32 gUnk_08751BF4[];
extern u16 gUnk_0873BAE6[];
extern u32 gUnk_08751D80[];
extern u32 gUnk_08751D50[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_08005f30(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
void sub_08006338(s32 a);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_0805af80(void);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_08057a10(void);
void sub_08057c98(void);

void sub_08057494(void)
{
    struct Task *t;
    u16 *row;
    s32 s;

    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751CF0;
    s = t->unk18 & 15;
    row = gUnk_0873BAB0[s];
    switch (s)
    {
    case 0:
        {
            struct Task *u = gUnk_03002490;

            u->unk00 = (u32)sub_080059d8;
            u->unk38 = gUnk_0874C600;
            u->unk04 = (u32)sub_08057a10;
            u->unk6C = 0;
        }
        do
        {
            {
                struct Task *v = gUnk_03002490;
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
                sub_080061c0(b, 0x5A5A5A5A);
            }
            {
                struct Task *w = gUnk_03002490;

                {
                    s32 a = row[1];
                    s32 b = a << 8;

                    if (a & 0x8000)
                        b |= 0xFF000000;
                    w->unk60 = b;
                }
            }
            sub_0800617c(row[2]);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            sub_0805afac(gUnk_03002490->unk88->unk00, 32, 1);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            sub_080062c4();
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 1);
        break;
    case 1:
        {
            struct Task *u = gUnk_03002490;

            u->unk00 = (u32)sub_080059d8;
            u->unk38 = gUnk_0874C600;
        }
        gUnk_03002490->unk4C = (gUnk_03002490->unk48 + sub_080064dc(-8, 1, 8)) << 16;
        gUnk_03002490->unk50 = (gUnk_03002490->unk4A + sub_080064ac(-8, 1, 8)) << 16;
        {
            s32 a = row[0];
            s32 b = a << 8;

            if (a & 0x8000)
                b |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, b);
        }
        {
            struct Task *w = gUnk_03002490;

            {
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->unk60 = b;
            }
        }
        sub_0800617c(row[2]);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(1);
        break;
    case 2:
        {
            struct Task *u = gUnk_03002490;

            u->unk00 = (u32)sub_080059d8;
            u->unk38 = gUnk_0874C718;
        }
        gUnk_03002490->unk4C = (sub_080064dc(-8, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(-4, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A - 8) << 16;
        sub_080061c0(-0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk60 = -0x6000;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 24;
        TaskYieldTrampoline(2);
        break;
    case 3:
        {
            struct Task *u = gUnk_03002490;

            u->unk00 = (u32)sub_080059d8;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0x1808;
            u->unk04 = (u32)sub_08057a10;
            u->unk6C = 0;
        }
        do
        {
            sub_080061c0(-0x10000, -0x2000);
            gUnk_03002490->unk4C = (sub_080064dc(-16, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_0800617c(16);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        break;
    case 4:
        {
            struct Task *u = gUnk_03002490;

            u->unk00 = (u32)sub_080059d8;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0x1808;
            u->unk04 = (u32)sub_08057a10;
            u->unk6C = 0;
        }
        do
        {
            sub_080061c0(-0x20000, -0x2000);
            gUnk_03002490->unk4C = (sub_080064dc(-16, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-4, 1, 16) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_0800617c(16);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
        {
            struct Task *u = gUnk_03002490;

            u->unk00 = (u32)sub_080059fc;
            u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0xF008;
        }
        {
            s32 a = row[0];
            s32 b = a << 8;

            if (a & 0x8000)
                b |= 0xFF000000;
            sub_080061c0(b, 0x5A5A5A5A);
        }
        {
            struct Task *w = gUnk_03002490;

            {
                s32 a = row[1];
                s32 b = a << 8;

                if (a & 0x8000)
                    b |= 0xFF000000;
                w->unk58 = b;
            }
        }
        gUnk_03002490->unk4C = sub_080064dc(-4, 1, 8) << 16;
        gUnk_03002490->unk50 = sub_080064ac(-4, 1, 8) << 16;
        gUnk_03002490->unk3C = row[2];
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08057a10(void)
{
    if (gUnk_03002490->unk88->unk04 != 13 || (((struct Task *)gUnk_03002490->unk8C)->unk7B & 1))
        sub_08005654(gCurTaskIdx);
}

void sub_08057a48(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751BF4;
    t->unk40 = gUnk_03002790[t->unk44].unk40;
    t->unk3C = gUnk_0873BAE6[t->unk18 & 15];
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_08057ad4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059d8;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk0C = (u32)sub_0805af80;
        t->unk04 = (u32)sub_08057c98;
        t->unk42 = 8;
        {
            struct Task *u = gUnk_03002490;

            u->unk38 = gUnk_08751D80;
            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 4;
            u->unk3C = 0xFFFF;
        }
        while (((struct Task *)gUnk_03002490->unk8C)->unk73 == 0)
            TaskYieldTrampoline(1);
        for (;;)
        {
            {
                struct Task *v = gUnk_03002490;

                v->unk4C = ((struct Task *)v->unk8C)->unk48 << 16;
                v->unk50 = ((struct Task *)v->unk8C)->unk4A << 16;
            }
            sub_08006338(0);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            {
                struct Task *w = gUnk_03002490;

                if (((struct Task *)w->unk8C)->unk73 == 2)
                {
                    w->unk54 = 0;
                    w->unk3C = 0xFFFF;
                    while (((struct Task *)gUnk_03002490->unk8C)->unk73 == 2)
                        TaskYieldTrampoline(1);
                }
            }
        }
    case 1:
        gUnk_03002490->unk0C = (u32)sub_08005f30;
        gUnk_03002490->unk42 = 5;
        {
            struct Task *u = gUnk_03002490;

            u->unk38 = gUnk_08751D50;
            if (u->unk43 == 1)
                u->unk4C = (((struct Task *)u->unk8C)->unk48 - 8) << 16;
            else
                u->unk4C = (((struct Task *)u->unk8C)->unk48 + 8) << 16;
        }
        {
            struct Task *v = gUnk_03002490;

            v->unk50 = ((struct Task *)v->unk8C)->unk4A << 16;
            if (((struct Task *)v->unk8C)->unk28 == 0)
            {
                v->unk40 = (((struct Task *)v->unk8C)->unk40 + 0x1800) | 8;
                sub_0800617c(0);
            }
            else
            {
                v->unk40 = 0;
                sub_0800617c(6);
            }
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(4);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08057c98(void)
{
    struct Task *t = gUnk_03002490;
    u8 s;

    if (t->unk88->unk04 != 13 || (s = ((struct Task *)t->unk8C)->unk73) == 3 || s == 4)
        sub_08005654(gCurTaskIdx);
    else
        t->unk43 = ((struct Task *)t->unk8C)->unk43;
}
