#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* obj_2eac8.c (0x0802EAC8-0x0802F62B, issue #86).
 *
 * Task types #221-#228 (class 3), the stage objects M07 places: for each
 * type a spawner (s32 f(x, y, ...) -> sub_08005904(type, 32, 63), which
 * stores the position and the object's parameters and returns the task
 * id or -1), the type's body (sets the update/draw callbacks, the
 * graphics table Task.unk38 and the palette, then runs its animation as
 * a TaskYieldTrampoline coroutine) and its callbacks.  The animated ones
 * keep the frame to upload in Task.unk28 and a Task.unk04 callback
 * (sub_0802eba4, sub_0802ed20, sub_0802ee88) that DMAs that frame's tiles
 * into OBJ VRAM with sub_080017e4 and resets unk28 to -1.  The last three
 * spawners are the three variants of type #229 (obj_2f62c.c). */

extern u32 gUnk_087558BC[];
extern u8 gUnk_085A0638[];
extern u32 gUnk_087558C0[];
extern u32 gUnk_087558DC[];
extern u8 gUnk_085A0C38[][32];
extern u32 gUnk_087558E8[];
extern u8 gUnk_085A12F8[];
extern u8 gUnk_085A1BF8[];
extern u8 gUnk_085A24F8[];
extern u32 gUnk_087558EC[];
extern u32 gUnk_087558FC[];
extern u32 gUnk_08755930[];
extern u32 gUnk_0875593C[];
extern u32 gUnk_08755944[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_080031b8(u32 a);
s32 sub_08005904(u32 type, s32 start, s32 end);
void sub_08005a74(void);
void sub_08005d9c(void);
void sub_08006138(void);
void sub_0802f718(void);
void sub_0802fea4(void);
void sub_0802ff70(void);
void sub_0802eba4(void);
void sub_0802ed20(void);
void sub_0802ee88(void);
void sub_0802f110(void);
void sub_0802f1dc(void);
void sub_0802f2b0(void);
void sub_0802f2fc(void);
void sub_0802f3d0(void);
void sub_0802f400(void);

s32 sub_0802eac8(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(221, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 28;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk20 = 1;
        t->unk76 = a;
    }
    return id;
}

void sub_0802eb28(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_0802fea4;
    t->unk38 = gUnk_087558BC;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk28 = -1;
    u->unk3C = 0;
    u->unk04 = (u32)sub_0802eba4;
    for (;;)
    {
        gUnk_03002490->unk28 = 0;
        TaskYieldTrampoline(60);
        gUnk_03002490->unk28 = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk28 = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk28 = 3;
        TaskYieldTrampoline(4);
    }
}

void sub_0802eba4(void)
{
    u8 *src;

    if (gUnk_03002490->unk28 != -1)
    {
        sub_080017e4(4, (u32)&gUnk_085A0638[gUnk_03002490->unk28 * 384], 0x06012180, 128);
        src = gUnk_085A0638;
        sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 384 + 128)), 0x06012580, 128);
        sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 384 + 256)), 0x06012980, 128);
        gUnk_03002490->unk28 = -1;
    }
}

s32 sub_0802ec1c(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(222, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 28;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk20 = 2;
        t->unk76 = a;
    }
    return id;
}

void sub_0802ec7c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_0802f718;
    t->unk38 = gUnk_087558C0;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk2C = -1;
    u->unk34 = (s32)gUnk_087558DC;
    u->unk28 = 0;
    u->unk3C = 0;
    u->unk04 = (u32)sub_0802ed20;
    for (;;)
    {
        gUnk_03002490->unk2C = 0;
        TaskYieldTrampoline(30);
        gUnk_03002490->unk2C = 1;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk2C = 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk2C = 3;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk2C = 4;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk2C = 0;
        TaskYieldTrampoline(30);
        gUnk_03002490->unk2C = 5;
        TaskYieldTrampoline(4);
    }
}

void sub_0802ed20(void)
{
    u8 *src;

    if (gUnk_03002490->unk2C != -1)
    {
        sub_080017e4(4, (u32)gUnk_085A0C38[gUnk_03002490->unk2C * 9], 0x06012100, 96);
        src = (u8 *)gUnk_085A0C38;
        sub_080017e4(4, (u32)(src + (gUnk_03002490->unk2C * 9 + 3) * 32), 0x06012500, 96);
        sub_080017e4(4, (u32)(src + (gUnk_03002490->unk2C * 9 + 6) * 32), 0x06012900, 96);
        gUnk_03002490->unk2C = -1;
    }
}

s32 sub_0802ed94(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(223, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4A = y;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        t->unk18 = a;
    }
    return id;
}

void sub_0802ede4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_087558E8;
    t->unk3C = 0;
    t->unk42 = 14;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk12 = 4;
    v = gUnk_03002490;
    v->unk28 = -1;
    v->unk04 = (u32)sub_0802ee88;
    sub_080031b8(222);
    gUnk_03002490->unk28 = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk28 = 1;
    TaskYieldTrampoline(3);
    w = gUnk_03002490;
    w->unk28 = 2;
    w->unk1C = 0;
    do
        TaskYieldTrampoline(1);
    while (gUnk_03002490->unk1C == 0);
    sub_080031b8(223);
    gUnk_03002490->unk28 = 2;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk28 = 1;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0802ee88(void)
{
    struct Task *t;
    s32 idx;
    u8 *src;

    t = gUnk_03002490;
    idx = t->unk28;
    if (idx != -1)
    {
        switch (t->unk18)
        {
        case 0:
            sub_080017e4(4, (u32)&gUnk_085A12F8[idx * 768], 0x06011100, 256);
            src = gUnk_085A12F8;
            sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 768 + 256)), 0x06011500, 256);
            sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 768 + 512)), 0x06011900, 256);
            break;
        case 1:
            sub_080017e4(4, (u32)&gUnk_085A1BF8[idx * 768], 0x06011100, 256);
            src = gUnk_085A1BF8;
            sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 768 + 256)), 0x06011500, 256);
            sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 768 + 512)), 0x06011900, 256);
            break;
        case 2:
            sub_080017e4(4, (u32)&gUnk_085A24F8[idx * 768], 0x06011100, 256);
            src = gUnk_085A24F8;
            sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 768 + 256)), 0x06011500, 256);
            sub_080017e4(4, (u32)(src + (gUnk_03002490->unk28 * 768 + 512)), 0x06011900, 256);
            break;
        }
        gUnk_03002490->unk28 = -1;
    }
}

s32 sub_0802ef90(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(224, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x + 26;
        t->unk4A = y;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        t->unk42 = a;
        t->unk3E = b;
    }
    return id;
}

void sub_0802eff8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_087558EC;
    t->unk40 = 0x8800;
    for (;;)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

s32 sub_0802f05c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(225, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 25;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk1C = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f0cc(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk38 = gUnk_087558FC;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    if (u->unk1C == 0)
        sub_0802f1dc();
    else
        sub_0802f110();
    TaskDispatchTrampoline();
}

void sub_0802f110(void)
{
    gUnk_03002490->unk0C = (u32)sub_0802fea4;
    for (;;)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(64);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
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
    }
}

void sub_0802f1dc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0802ff70;
    t->unk3C = 0;
    sub_08006138();
}

s32 sub_0802f1fc(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(226, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 25;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk1C = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f26c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk38 = gUnk_08755930;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    if (u->unk1C == 0)
        sub_0802f2fc();
    else
        sub_0802f2b0();
    TaskDispatchTrampoline();
}

void sub_0802f2b0(void)
{
    gUnk_03002490->unk0C = (u32)sub_0802fea4;
    for (;;)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(62);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(30);
    }
}

void sub_0802f2fc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0802ff70;
    t->unk3C = 0;
    sub_08006138();
}

s32 sub_0802f31c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(227, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 25;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk1C = a;
        t->unk20 = 1;
        t->unk76 = b;
    }
    return id;
}

void sub_0802f38c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk38 = gUnk_0875593C;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    if (u->unk1C == 0)
        sub_0802f400();
    else
        sub_0802f3d0();
    TaskDispatchTrampoline();
}

void sub_0802f3d0(void)
{
    gUnk_03002490->unk0C = (u32)sub_0802fea4;
    for (;;)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(20);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(20);
    }
}

void sub_0802f400(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0802ff70;
    t->unk3C = 0;
    sub_08006138();
}

s32 sub_0802f420(s32 x, s32 y, s32 a)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(228, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 28;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk20 = 1;
        t->unk76 = a;
    }
    return id;
}

void sub_0802f480(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_0802fea4;
    t->unk38 = gUnk_08755944;
    t->unk42 = 15;
    u = gUnk_03002490;
    u->unk40 = 0x8800;
    u->unk3C = 0;
    TaskYieldTrampoline(20);
    sub_08006138();
}

s32 sub_0802f4c8(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(229, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 24;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk18 = a;
        t->unk1C = 1;
        t->unk20 = 2;
        t->unk76 = b;
    }
    return id;
}

s32 sub_0802f53c(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(229, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 24;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk18 = a;
        t->unk1C = 0;
        t->unk20 = 2;
        t->unk76 = b;
    }
    return id;
}

s32 sub_0802f5b4(s32 x, s32 y, s32 a, s32 b)
{
    s32 id;
    struct Task *t;

    id = sub_08005904(229, 32, 63);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk48 = x;
        t->unk4C = t->unk48 << 16;
        t->unk4A = y - 24;
        t->unk50 = t->unk4A << 16;
        t->unk54 = x;
        t->unk58 = y;
        t->unk18 = a;
        t->unk1C = 0;
        t->unk20 = 0;
        t->unk76 = b;
    }
    return id;
}
