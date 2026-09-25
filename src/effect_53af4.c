#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_53af4.c (0x08053AF4-0x0805432F, issue #89).
 *
 * Task type #7: body and variants 0-6. */

extern void (*gUnk_0873B928[])(void);   /* task type #7's 49 variants, indexed by Task.unk18 >> 24 */
extern u32 gUnk_08751C44[];
extern u32 gUnk_0874C600[];
extern u32 gUnk_08751CEC[];
/* sprite / animation tables stored to Task.unk38 */
extern u32 gUnk_0874C500[];
extern s16 gUnk_0873B9EC[];             /* [6][8]: s16 x, y offsets, 8.8 velocities */

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 sub_08002ee8(u32 range);                       /* RNG: 0 .. range-1 */
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005ca0(void);
void sub_08005d9c(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
void sub_08006338(s32 a);
void sub_08006384(u16 a);
void sub_080063ac(s16 a);
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
void sub_0803dfc8(void);
s32 sub_0803fd20(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */
void sub_08053be0(void);
void sub_08053c1c(void);
void sub_08053e34(void);
void sub_08054298(void);

void sub_08053af4(void)
{
    if (gUnk_03002490->unk8C == NULL)
    {
        gUnk_03002490->unk80 = 0;
        gUnk_03002490->unk8C = (struct Actor *)&gUnk_03002790[gUnk_03002490->unk44];
    }
    sub_08002e98(((u8 *)gUnk_03002490)[27], 49, gUnk_0873B928);
}

void sub_08053b40(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_08053c1c;
    gUnk_03002490->unk04 = (u32)sub_08053be0;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751C44;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 4;
    sub_08006384(4);
    gUnk_03002490->unk50 = 0x40000;
    for (;;)
    {
        gUnk_03002490->unk28 = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338((s16)gUnk_03002490->unk28++);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 11);
    }
}

void sub_08053be0(void)
{
    gUnk_03002490->unk13 = ((struct Task *)gUnk_03002490->unk8C)->unk13 & 0xFB;
    if (!(gUnk_03002490->unk88->unk40 & 4))
        sub_08005654(gCurTaskIdx);
}

void sub_08053c1c(void)
{
    if (gUnk_03002490->unk88->unk04 == 10)
        sub_0803dfc8();
    else
        sub_08005654(gCurTaskIdx);
}

void sub_08053c48(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C600;
    if ((t->unk18 & 15) == 0)
        t->unk43 = -1;
    else
        t->unk43 = 1;
    sub_080063ac(-12);
    gUnk_03002490->unk50 = (gUnk_03002490->unk4A + 6) << 16;
    sub_080061c0(-0x24000, 0x1800);
    gUnk_03002490->unk58 = -0x4000;
    gUnk_03002490->unk60 = -0x2000;
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

void sub_08053d08(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C600;
    if ((t->unk18 & 15) == 0)
        t->unk43 = -1;
    else
        t->unk43 = 1;
    sub_080063ac(-12);
    gUnk_03002490->unk50 = (gUnk_03002490->unk4A + 6) << 16;
    sub_080061c0(-0x60000, 0xC000);
    gUnk_03002490->unk58 = -0x20000;
    gUnk_03002490->unk60 = 0x4000;
    sub_0800617c(0);
    TaskYieldTrampoline(4);
    sub_0800617c(10);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_08053db8(void)
{
    struct Task *t = gUnk_03002490;
    s32 s;

    t->unk04 = (u32)sub_08053e34;
    s = t->unk18 & 15;
    if (s == 0)
    {
        t->unk00 = (u32)sub_080059fc;
        t->unk0C = (u32)sub_08005d9c;
        t->unk42 = 5;
        gUnk_03002490->unk38 = gUnk_08751CEC;
        gUnk_03002490->unk4C = sub_080064dc(-16, 1, 16) << 16;
        gUnk_03002490->unk50 = (sub_080064ac(-4, 1, 16) << 16) - 0x180000;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
    }
    TaskDispatchTrampoline();
}

void sub_08053e34(void)
{
}

void sub_08053e38(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 c;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_0874C500;
    n = sub_08002ee8(8);
    sub_080063ac(gUnk_0873B9EC[n]);
    t = gUnk_03002490;
    t->unk50 = (t->unk4A + (gUnk_0873B9EC + 8)[n] + 4) << 16;
    a = (gUnk_0873B9EC + 16)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk54 = b;
    a = (gUnk_0873B9EC + 24)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk58 = b;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    v = gUnk_03002490;
    c = (gUnk_0873B9EC + 32)[n];
    b = c << 8;
    if (c & 0x8000)
        b |= 0xFF000000;
    v->unk54 = b;
    a = (gUnk_0873B9EC + 40)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->unk58 = b;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    w = gUnk_03002490;
    w->unk54 = 0;
    w->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    TaskDispatchTrampoline();
}

void sub_08053f70(void)
{
    struct Task *u;
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
    s32 f;

    u = gUnk_03002490;
    u->unk00 = (u32)sub_080059d8;
    if (u->unk88->unk37 == 2)
        u->unk0C = (u32)sub_08005ca0;
    else
        u->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_0874C500;
    n = sub_08002ee8(8);
    sub_080063ac(gUnk_0873B9EC[n]);
    t = gUnk_03002490;
    t->unk50 = (t->unk4A + (gUnk_0873B9EC + 8)[n] + 4) << 16;
    a = (gUnk_0873B9EC + 16)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk54 = b;
    a = (gUnk_0873B9EC + 24)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk58 = b;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    v = gUnk_03002490;
    c = (gUnk_0873B9EC + 32)[n];
    d = c << 8;
    if (c & 0x8000)
        d |= 0xFF000000;
    v->unk54 = d;
    a = (gUnk_0873B9EC + 40)[n];
    d = a << 8;
    if (a & 0x8000)
        d |= 0xFF000000;
    v->unk58 = d;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    w = gUnk_03002490;
    w->unk54 = 0;
    w->unk58 = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    TaskDispatchTrampoline();
}

void sub_080540d0(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C600;
    switch (t->unk18 & 0xFF0000)
    {
    case 0:
        t->unk28 = 0;
        t->unk2C = t->unk88->unk04;
        t->unk30 = t->unk18 & 0xFF;
        t->unk04 = (u32)sub_08054298;
        do
        {
            struct Task *u = gUnk_03002490;
            struct Task *p;

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
            sub_0805afac(gUnk_03002490->unk88->unk00, 6, 0x10000);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            sub_080062c4();
        } while (gUnk_03002490->unk28 == 0);
        break;
    case 0x10000:
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

void sub_08054298(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 == 0)
    {
        u8 m = t->unk88->unk04;

        if (m != t->unk2C || m == 16)
        {
            t->unk28 = 1;
            return;
        }
        switch (t->unk18 & 0xFF00)
        {
        case 0:
            if (((struct Task *)t->unk8C)->unk7A == 0)
                t->unk28 = 1;
        case 0x100:
        {
            struct Task *u = gUnk_03002490;

            if (((u8 *)u)[24] != 0 && --u->unk30 == 0)
                u->unk28++;
            break;
        }
        case 0x200:
            if (sub_0803fd20(t->unk44) != 4)
                gUnk_03002490->unk28++;
            break;
        }
    }
}
