#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_54a80.c (0x08054A80-0x0805545F, issue #89).
 *
 * Task type #7: variants 12-15. */

extern u32 gUnk_0874C6F4[];
extern u16 gUnk_0873BA4C[][4];          /* [8 + 4][4]: 8.8 velocity x, y, acceleration x, y */
extern s16 gUnk_03002348;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_0874CA1C[];
extern u16 gUnk_0873E640[];
extern u32 gUnk_0874C67C[];
extern u32 gUnk_0874C718[];
extern u32 gUnk_08751C74[];
extern u8 gUnk_02007CF0;
extern u8 gUnk_02000020;

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
u32 sub_08002ee8(u32 range);                       /* RNG: 0 .. range-1 */
void sub_080055b0(u8 val, s32 idx);
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
s32 sub_080063f0(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell s32) */
u32 sub_0800641c(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
u16 sub_080064ac(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 sub_080064dc(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.unk43 != 1 */
s32 sub_0803eaf8(s32 a0);
void sub_08054b98(void);
void sub_080552fc(void);
void sub_080553d4(void);

void sub_08054a80(void)
{
    struct Task *t;
    s32 a;
    s32 b;

    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk08 = (u32)sub_08054b98;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C6F4;
    t->unk48 = ((struct Task *)t->unk8C)->unk48;
    t->unk4A = ((struct Task *)t->unk8C)->unk4A;
    a = gUnk_0873BA4C[t->unk18 & 7][0];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk28 = b;
    a = gUnk_0873BA4C[t->unk18 & 7][1];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk2C = b;
    t->unk4C = t->unk48;
    t->unk50 = t->unk4A;
    a = gUnk_0873BA4C[(t->unk18 & 7) + 4][0];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk30 = b;
    a = gUnk_0873BA4C[(t->unk18 & 7) + 4][1];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->unk34 = b;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    TaskDispatchTrampoline();
}

void sub_08054b98(void)
{
    struct Task *t;
    u32 *tbl;
    s32 x;
    s32 y;
    s32 i;
    s32 j;

    t = gUnk_03002490;
    tbl = t->unk38;
    if (t->unk88->unk37 == 2)
    {
        x = t->unk48;
        y = t->unk4A;
    }
    else
    {
        x = t->unk48 - gUnk_03002348;
        y = t->unk4A - gUnk_030023E4;
    }
    if (sub_080063f0(x, y) != 0)
        sub_08001a94(gUnk_03002490->unk42, tbl[gUnk_03002490->unk3C], 0, 0, x, y);
    t = gUnk_03002490;
    if (t->unk88->unk37 == 2)
    {
        x = t->unk4C;
        y = t->unk50;
    }
    else
    {
        x = t->unk4C - gUnk_03002348;
        y = t->unk50 - gUnk_030023E4;
    }
    if (sub_080063f0(x, y) != 0)
        sub_08001a94(gUnk_03002490->unk42, tbl[gUnk_03002490->unk3C], 0, 0, x, y);
    i = gUnk_03002490->unk18 & 7;
    gUnk_03002490->unk28 += (gUnk_0873BA4C[i][2] & 0x8000) ? (gUnk_0873BA4C[i][2] << 8) | 0xFF000000 : gUnk_0873BA4C[i][2] << 8;
    gUnk_03002490->unk2C += (gUnk_0873BA4C[i][3] & 0x8000) ? (gUnk_0873BA4C[i][3] << 8) | 0xFF000000 : gUnk_0873BA4C[i][3] << 8;
    gUnk_03002490->unk48 += gUnk_03002490->unk28 >> 16;
    gUnk_03002490->unk4A += gUnk_03002490->unk2C >> 16;
    j = (gUnk_03002490->unk18 & 7) + 4;
    gUnk_03002490->unk30 += (gUnk_0873BA4C[j][2] & 0x8000) ? (gUnk_0873BA4C[j][2] << 8) | 0xFF000000 : gUnk_0873BA4C[j][2] << 8;
    gUnk_03002490->unk34 += (gUnk_0873BA4C[j][3] & 0x8000) ? (gUnk_0873BA4C[j][3] << 8) | 0xFF000000 : gUnk_0873BA4C[j][3] << 8;
    gUnk_03002490->unk4C += ((s16 *)&gUnk_03002490->unk30)[1];
    gUnk_03002490->unk50 += ((s16 *)&gUnk_03002490->unk34)[1];
}

void sub_08054d94(void)
{
    struct Task *t;
    s32 i;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 8;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874CA1C;
    for (i = 0; i < 23; i++)
    {
        gUnk_03002490->unk3C = gUnk_0873E640[i];
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_08054de8(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C67C;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk4C = -0xA0000;
        t->unk50 = -0x40000;
        t->unk3C = 12;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = 0xA0000;
        gUnk_03002490->unk50 = -0xA0000;
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = 0xA0000;
        gUnk_03002490->unk50 = -0x40000;
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk4C = -0xA0000;
        gUnk_03002490->unk50 = -0xA0000;
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        break;
    case 1:
        t->unk4C = 0;
        t->unk50 = 0x80000;
        t->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 10);
        break;
    case 2:
        t->unk4C = 0;
        t->unk50 = 0x80000;
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(14);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 5);
        break;
    case 3:
        t->unk42 = 8;
        gUnk_03002490->unk4C = 0;
        gUnk_03002490->unk50 = -0x80000;
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(20);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 7);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08054fe4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080552fc;
    t->unk08 = (u32)sub_080553d4;
    t->unk42 = ((struct Task *)t->unk8C)->unk42;
    u = gUnk_03002490;
    u->unk28 = u->unk88->unk0D;
    switch (u->unk88->unk0D)
    {
    case 1:
        u->unk43 = -1;
        gUnk_03002490->unk38 = gUnk_0874C718;
        for (;;)
        {
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk43 = -gUnk_03002490->unk43;
                gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
                gUnk_03002490->unk50 = (sub_080064ac(-12, 1, 12) + ((struct Task *)gUnk_03002490->unk8C)->unk4A - 4) << 16;
                sub_080061c0(0xC000, -0x1000);
                gUnk_03002490->unk58 = -0x8000;
                gUnk_03002490->unk60 = 0;
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
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 1);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(4);
        }
    case 2:
        v = gUnk_03002490;
        v->unk38 = gUnk_08751C74;
        v->unk40 = (((struct Task *)v->unk8C)->unk40 + 0x1800) | 8;
        for (;;)
        {
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
                gUnk_03002490->unk50 = (sub_080064ac(-16, 1, 12) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
                gUnk_03002490->unk3C = sub_08002ee8(12);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 2);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
                gUnk_03002490->unk50 = (sub_080064ac(-16, 1, 12) + ((struct Task *)gUnk_03002490->unk8C)->unk4A - 10) << 16;
                gUnk_03002490->unk3C = sub_08002ee8(12);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 2);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(16);
            gUnk_03002490->unk4C = (sub_080064dc(-12, 1, 24) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(-16, 1, 12) + ((struct Task *)gUnk_03002490->unk8C)->unk4A - 10) << 16;
            gUnk_03002490->unk3C = sub_08002ee8(12);
            TaskYieldTrampoline(2);
            ((volatile struct Task *)gUnk_03002490)->unk3C = 0xFFFF;
            TaskYieldTrampoline(8);
        }
    }
    TaskDispatchTrampoline();
}

void sub_080552fc(void)
{
    struct Task *t;
    struct PlayerState *ps;
    s32 ok;
    s32 r;

    if (gUnk_02007CF0 == 1
     || (t = gUnk_03002490, ps = t->unk88, t->unk28 != ps->unk0D)
     || ps->unk04 == 13 || ps->unk04 == 22
     || gUnk_02000020 == 1)
    {
        sub_08005654(gCurTaskIdx);
        return;
    }
    ok = 1;
    if (((struct Task *)t->unk8C)->unk7B & 1)
    {
        ok = 0;
    }
    else
    {
        r = sub_0803eaf8(ps->unk00);
        if (r == 0x5A5A5A5A)
            ok = 0;
        gUnk_03002490->unk48 = (gUnk_03002490->unk4C >> 16) + ((u32)r >> 16);
        gUnk_03002490->unk4A = (gUnk_03002490->unk50 >> 16) + r;
    }
    if (gUnk_03002490->unk88->unk04 == 20 && ((struct Task *)gUnk_03002490->unk8C)->unk18 != 0)
        ok = 0;
    if (ok == 0)
        sub_080055b0(8, gCurTaskIdx);
    else
        sub_080055b0(0, gCurTaskIdx);
}

void sub_080553d4(void)
{
    struct Task *t;
    u32 *tbl;
    s32 x, y;

    if (gUnk_03002490->unk38 == 0)
        return;
    if (gUnk_03002490->unk3C == -1)
        return;
    if (sub_0800641c(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 0)
        return;
    t = gUnk_03002490;
    tbl = t->unk38;
    x = t->unk48 - gUnk_03002348;
    y = t->unk4A - gUnk_030023E4;
    sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, x, (s16)y);
}
