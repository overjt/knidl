#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_56dd4.c (0x08056DD4-0x08057493, issue #89).
 *
 * Task type #7: variants 29-31. */

extern u32 gUnk_08751CA4[];
extern u32 gUnk_08751CBC[];
extern s16 gUnk_0873BA8C[][2][3];   /* {base, scale, amount} rows for sub_080064dc */
extern u32 gUnk_0873C038[];
extern u32 gUnk_0873CC94[];
extern u32 gUnk_08751CEC[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 sub_08002ee8(u32 range);                       /* RNG: 0 .. range-1 */
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_08005fc8(void);
void sub_08006040(void);
void sub_080061c0(s32 a, s32 b);
void sub_08006338(s32 a);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_08030804(struct HitBoxSet *p, s32 x, s32 y, s32 e);
void sub_0805707c(void);
void sub_080573a4(void);

void sub_08056dd4(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_0805707c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751CA4;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 12;
    t->unk28 = 0;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(12);
        do
        {
            gUnk_03002490->unk4C = (sub_080064dc(16, 1, 32) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-8, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0x18000, -0x800);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = -0x2000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
        } while (gUnk_03002490->unk28 == 0);
        break;
    case 1:
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(8);
        do
        {
            gUnk_03002490->unk4C = (sub_080064dc(32, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
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
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        do
        {
            gUnk_03002490->unk4C = (sub_080064dc(20, 1, 12) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
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

void sub_0805707c(void)
{
    {
        struct Task *t = gUnk_03002490;

        if (t->unk28 == 0 && (t->unk88->unk04 != 13 || t->unk43 != ((struct Task *)t->unk8C)->unk43))
            t->unk28 = 1;
    }
    {
        struct Task *t = gUnk_03002490;

        if (!(t->unk88->unk40 & 0x100) && (((struct Task *)t->unk8C)->unk7B & 1))
            sub_08005654(gCurTaskIdx);
    }
    if (gUnk_03002490->unk88->unk0D != 1)
        sub_08005654(gCurTaskIdx);
}

void sub_0805710c(void)
{
    struct Task *t;
    s16 *x;
    s16 *y;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk04 = (u32)sub_080573a4;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751CBC;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x800) | 12;
    if ((t->unk28 = t->unk18 & 15) == 0)
        t->unk0C = (u32)sub_08005fc8;
    else
        t->unk0C = (u32)sub_08006040;
    x = gUnk_0873BA8C[gUnk_03002490->unk28][0];
    y = gUnk_0873BA8C[gUnk_03002490->unk28][1];
    for (;;)
    {
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        sub_08006338(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        sub_08006338(8);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        sub_08006338(1);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = sub_080064dc(x[0], x[1], x[2]) << 16;
        gUnk_03002490->unk50 = sub_080064dc(y[0], y[1], y[2]) << 16;
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
    }
}

void sub_080573a4(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *p;

    if (t->unk88->unk04 != 13 || (p = (struct Task *)t->unk8C)->unk73 != 1)
    {
        sub_08005654(gCurTaskIdx);
    }
    else if (t->unk28 == 0)
    {
        sub_0801a828(gCurTaskIdx, p->unk48, p->unk4A, gUnk_0873C038);
        sub_08030804((struct HitBoxSet *)gUnk_0873CC94, ((struct Task *)gUnk_03002490->unk8C)->unk48,
                     ((struct Task *)gUnk_03002490->unk8C)->unk4A, gUnk_03002490->unk44);
    }
}

void sub_08057430(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_08751CEC;
    gUnk_03002490->unk4C = sub_080064dc(-32, 1, 16) << 16;
    gUnk_03002490->unk50 = sub_080064ac(-4, 1, 16) << 16;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    TaskDispatchTrampoline();
}
