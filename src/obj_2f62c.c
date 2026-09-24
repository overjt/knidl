#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* obj_2f62c.c (0x0802F62C-0x08030237, issue #86).
 *
 * Task types #229-#235 (class 3) and the spawner of #236, in the same
 * spawner / body / callback layout as obj_2eac8.c.  The draw callbacks
 * (sub_0802f718, sub_0802f93c, sub_0802fd98, sub_0802fea4, sub_0802ff70;
 * several types share the last two) draw the frame Task.unk3C of the
 * Task.unk38 table through sub_080306b4, and a second sprite from
 * Task.unk34 at Task.unk54/unk58 where the type has one.  #231
 * (sub_0802faa8) flies a fixed path, eight velocity changes per lap.
 * sub_0802ffe8 is also called from M33 (src/hud_b5024.c), and
 * sub_08030100 spawns up to two #235 objects from the table
 * gUnk_087328C0. */

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[6];
};

extern u32 gUnk_08755948[];
extern u32 gUnk_087558DC[];
extern u32 gUnk_087558C4[];
extern struct Unk02004B90 gUnk_02004B90[];
extern u32 gUnk_08755978[];
extern u8 gUnk_085A2DF8[][32];
extern s16 gUnk_020055D4;
extern u32 gUnk_087558D0[];
extern u8 gUnk_02007FC4;
extern u32 gUnk_0875597C[];
extern u32 gUnk_0875599C[];
extern u32 gUnk_087559A4[];
extern u32 gUnk_087559C0[];
extern s16 gUnk_087328C0[][2];
extern u32 gUnk_087559DC[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08005904(u32 type, s32 start, s32 end);
void sub_080059d8(void);
void sub_08005a74(void);
void sub_08005d9c(void);
void sub_08006138(void);
void sub_08008f10(s32 a0);
s32 sub_080306b4(u8 a, s32 b, u16 c, u16 d, s16 x, s16 y);
void sub_0802f684(void);
void sub_0802f6c0(void);
void sub_0802f6f4(void);
void sub_0802f718(void);
void sub_0802f8c8(void);
void sub_0802f93c(void);
void sub_0802fd98(void);
void sub_0802fea4(void);
s32 sub_08030140(u8 a, s32 b);

void sub_0802f62c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_0802f718;
    t->unk38 = gUnk_08755948;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    if (u->unk20 == 0)
        sub_0802f6f4();
    else if (u->unk1C == 0)
        sub_0802f6c0();
    else
        sub_0802f684();
    TaskDispatchTrampoline();
}

void sub_0802f684(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk34 = (s32)gUnk_087558DC;
    for (;;)
    {
        u = gUnk_03002490;
        u->unk28 = 0;
        u->unk3C = u->unk18 << 1;
        TaskYieldTrampoline(2);
        v = gUnk_03002490;
        v->unk28 = 1;
        v->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_0802f6c0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk34 = (s32)gUnk_087558DC;
    t->unk3C = t->unk18 << 1;
    for (;;)
    {
        gUnk_03002490->unk28 = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk28 = 1;
        TaskYieldTrampoline(2);
    }
}

void sub_0802f6f4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk34 = (s32)gUnk_087558C4;
    t->unk28 = 0;
    t->unk3C = t->unk18 << 1;
    sub_08006138();
}

void sub_0802f718(void)
{
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    t = gUnk_03002490;
    if (t->unk3C != -1)
    {
        tbl = t->unk38;
        sub_080306b4(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
    u = gUnk_03002490;
    tbl = (u32 *)u->unk34;
    if (gUnk_02004B90[u->unk76].unk1 != 0)
        sub_080306b4(u->unk42, tbl[u->unk28], u->unk3E, u->unk40, u->unk54, u->unk58);
    else
        sub_080306b4(u->unk42, tbl[2], u->unk3E, u->unk40, u->unk54, u->unk58);
}

s32 sub_0802f7dc(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(230, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 28;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk18 = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f84c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_0802f93c;
    t->unk38 = gUnk_08755978;
    t->unk42 = 14;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk12 = 2;
    v = gUnk_03002490;
    v->unk04 = (u32)sub_0802f8c8;
    v->unk3C = 0;
    v->unk34 = 0;
    for (;;)
    {
        gUnk_03002490->unk34 = 0;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk34 = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk34 = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk34 = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802f8c8(void)
{
    u8 *src;

    if (gUnk_03002490->unk34 != -1)
    {
        sub_080017e4(4, (u32)gUnk_085A2DF8[gUnk_03002490->unk34 * 9], 0x06013980, 96);
        src = (u8 *)gUnk_085A2DF8;
        sub_080017e4(4, (u32)(src + (gUnk_03002490->unk34 * 9 + 3) * 32), 0x06013D80, 96);
        sub_080017e4(4, (u32)(src + (gUnk_03002490->unk34 * 9 + 6) * 32), 0x06014180, 96);
        gUnk_03002490->unk34 = -1;
    }
}

void sub_0802f93c(void)
{
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    t = gUnk_03002490;
    if (t->unk18 != 0)
        sub_080306b4(t->unk42 + 1, gUnk_087558D0[1], t->unk3E, t->unk40, t->unk54, t->unk58);
    else if (gUnk_02004B90[t->unk76].unk1 != 0)
        sub_080306b4(t->unk42 + 1, gUnk_087558D0[0], t->unk3E, t->unk40, t->unk54, t->unk58);
    else
        sub_080306b4(t->unk42 + 1, gUnk_087558D0[2], t->unk3E, t->unk40, t->unk54, t->unk58);
    u = gUnk_03002490;
    if (u->unk3C != -1)
    {
        tbl = u->unk38;
        sub_080306b4(u->unk42, tbl[u->unk3C], gUnk_020055D4, u->unk40, u->unk48, u->unk4A);
    }
}

s32 sub_0802fa3c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(231, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 28;
        t->unk50 = t->unk4A << 16;
        t->unk24 = a;
        t->unk20 = b;
        gUnk_02007FC4 = 0;
    }
    return id;
}

void sub_0802faa8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0802fd98;
    t->unk38 = gUnk_0875597C;
    t->unk42 = 14;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk12 = 1;
    v = gUnk_03002490;
    v->unk3C = -1;
    if (v->unk20 != 0)
        while (gUnk_02007FC4 == 0)
            TaskYieldTrampoline(1);
    if (gUnk_03002490->unk24 != 0)
    {
        gUnk_03002490->unk12 = 3;
        gUnk_03002490->unk54 = -0x10000;
        gUnk_03002490->unk58 = 0x10000;
        gUnk_03002490->unk3C = -1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = 0x8000;
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk12 = 1;
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0x8000;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk58 = -0x10000;
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(4);
    }
    for (;;)
    {
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk58 = -0x10000;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0x8000;
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk12 = 3;
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = 0x8000;
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x10000;
        gUnk_03002490->unk58 = 0x10000;
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = -1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x10000;
        gUnk_03002490->unk58 = 0x10000;
        gUnk_03002490->unk3C = -1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = 0x8000;
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0;
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk12 = 1;
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0x8000;
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk58 = -0x10000;
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802fd98(void)
{
    struct Task *t;
    u32 *tbl;

    t = gUnk_03002490;
    if (t->unk3C != -1)
    {
        tbl = t->unk38;
        sub_080306b4(t->unk42, tbl[t->unk3C], gUnk_020055D4, t->unk40, t->unk48, t->unk4A);
    }
}

s32 sub_0802fdf4(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(232, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 28;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk18 = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802fe64(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_0802fea4;
    t->unk38 = gUnk_0875599C;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk3C = u->unk18;
    sub_08006138();
}

void sub_0802fea4(void)
{
    struct Task *u;
    u32 *tbl;

    if (gUnk_02004B90[gUnk_03002490->unk76].unk1 != 0)
        sub_080306b4(gUnk_03002490->unk42, gUnk_087558D0[0], gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk54, gUnk_03002490->unk58);
    else
        sub_080306b4(gUnk_03002490->unk42, gUnk_087558D0[2], gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk54, gUnk_03002490->unk58);
    u = gUnk_03002490;
    if (u->unk3C != -1)
    {
        tbl = u->unk38;
        sub_080306b4(u->unk42, tbl[u->unk3C], u->unk3E, u->unk40, u->unk48, u->unk4A);
    }
}

void sub_0802ff70(void)
{
    struct Task *t;
    u32 *tbl;

    t = gUnk_03002490;
    if (t->unk3C != -1)
    {
        tbl = t->unk38;
        sub_080306b4(15, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
    sub_080306b4(15, gUnk_087558D0[1], gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk54, gUnk_03002490->unk58);
}

s32 sub_0802ffe8(s32 a, s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(233, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4A = y - 28;
        t->unk4C = x << 16;
        t->unk50 = (y - 28) << 16;
        t->unk18 = a;
    }
    return id;
}

void sub_08030034(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_087559A4;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0xD800;
    u->unk3C = u->unk18;
    sub_08006138();
}

s32 sub_08030074(s32 a)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(234, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = 128;
        t->unk4A = 420;
        t->unk4C = 128 << 16;
        t->unk50 = 420 << 16;
        t->unk18 = a;
    }
    return id;
}

void sub_080300c0(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_087559C0;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0xD800;
    u->unk3C = u->unk18;
    sub_08006138();
}

s32 sub_08030100(u8 a)
{
    s32 i;

    if (a <= 5)
    {
        sub_08008f10(a);
        for (i = 0; i <= 1; i++)
        {
            if (gUnk_087328C0[a][i] != -1)
                sub_08030140(a, i);
        }
    }
}

s32 sub_08030140(u8 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(235, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = gUnk_087328C0[a][b];
        t->unk4A = 96;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        t->unk18 = b;
    }
    return id;
}

void sub_080301a4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_087559DC;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0xD3C0;
    u->unk3C = u->unk18;
    sub_08006138();
}

s32 sub_080301e8(s32 a, s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(236, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4A = y;
        t->unk4C = x << 16;
        t->unk50 = y << 16;
        t->unk14 = a;
        t->unk72 = 9;
    }
    return id;
}
