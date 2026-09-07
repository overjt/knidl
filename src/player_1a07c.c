#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_03001470[];
extern u16 gUnk_03001EA4;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern struct Task *gUnk_03002490;
extern u32 gUnk_0874CE90[];
extern u8 gUnk_08757368[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s16 f);
void sub_08003014(s32 a, s32 b, s32 c, s32 d, void *e);
s32 sub_080031b8(s32 id);
void sub_080059fc(void);
void sub_08005ca0(void);
void sub_080062c4(void);
s32 sub_080063f0(s16 x, s16 y);
s32 sub_0803d55c(s32 mode);

void sub_0801a07c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059fc;
    t->unk0C = (u32)sub_08005ca0;
    t->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_0874CE90;
    gUnk_03002490->unk40 = 0;
    gUnk_03002490->unk4C = 0;
    gUnk_03002490->unk50 = 0;
    sub_080062c4();
    sub_080031b8(0x242);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 18;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 19;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    TaskDispatchTrampoline();
}

void sub_0801a1ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    struct Task *q;
    s32 mode;
    s32 anim;
    s32 n;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gUnk_03002490;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    mode = 0;
    if (t->unk3C > 49 && (gUnk_03001EA4 & 3) == 0)
        mode = 73;
    u = gUnk_03002490;
    dx = u->unk48;
    dy = u->unk4A;
    anim = sub_0803d55c(mode);
    v = gUnk_03002490;
    if (v->unk3C > 49)
    {
        n = v->unk34 + 1;
        v->unk34 = n;
        if (n < 0)
            v->unk34 = 0;
        w = gUnk_03002490;
        if (w->unk34 > 10)
            w->unk34 = 0;
        sub_08003014(0x0859A0B0, 0x0859A0D0, gUnk_08757368[gUnk_03002490->unk34], 16,
                     (void *)((u32)gUnk_03001470
                              + (((gUnk_03002490->unk40 >> 12) + 1) << 5)));
    }
    if (gUnk_030023AC > 1)
        sub_080017e4(2, (gUnk_03002360 << 5) + 0x080DC628,
                     (u32)gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) << 5), 22);
    x = dx;
    y = dy;
    if (sub_080063f0(x, y) == 0)
        return;
    q = gUnk_03002490;
    sub_08001a94(q->unk42, anim, q->unk3E, 0x800 | q->unk40, x, y);
}

void sub_0801a310(void)
{
    struct Task *t;
    struct Task *u;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u32 dst;
    u32 prio;

    t = gUnk_03002490;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    prio = t->unk40;
    dst = ((prio & 0x7FF) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)t->unk38[t->unk3C];
    p = g->unk08;
    if (*p != 0xFFFF)
    {
        do
        {
            q = p + 1;
            sub_080017e4(4, (u32)q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        } while (*p != 0xFFFF);
    }
    sub_080017e4(2, (u32)(g->unk04 + 1), (u32)gUnk_03001470 + ((prio >> 12) << 5),
                 *g->unk04);
    u = gUnk_03002490;
    sub_08001a94(u->unk42, g->unk00, u->unk3E, 0x800 | u->unk40, u->unk48, u->unk4A);
}
