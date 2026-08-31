/* game_code_and_rodata 0x0806FF24-0x08070EC0 (issue #64, module M18 batch 8).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806FF24 0x08070EC0 pending/batch8/actor_6ff24.c --newpb
 *
 * The tail of M18: the class-1 "player intro / transformation" task family
 * that the 26-entry anchor table at 0x0873FB08 dispatches, continuing the
 * batch-7 range.  Each state is a body + a per-frame helper pair:
 *
 *   sub_0807079c / sub_080708ec   state 1
 *   sub_08070930 / sub_08070a84   state 3
 *   sub_08070ac8 / sub_08070c0c   state 4
 *   sub_08070c54 / sub_08070d48   state 5
 *   sub_08070d90 / sub_08070e7c   state 6
 *
 * The bodies are all the same shape: set Task.unk15 (the state), unk24 (the
 * sub-step the helper advances on a unk7A bit), unk0C (the draw hook) and
 * unk42/unk43, kick a sound with sub_08006338, then walk Task.unk58 (a 16.16
 * vertical speed) through a table of steps with TaskYieldTrampoline, and
 * finally wait for the helper to reach unk24 == 2.
 *
 * Also here: sub_0806ff7c, the OAM draw routine for the family, and
 * sub_08070498 / sub_08070648, the entry points that re-seat the running
 * task from the player record (Task.unk88) and the id at 0x020055C0.
 *
 * NOTE for the coordinator - symbols.csv corrections this range needs:
 *   FALSE entries (pool-skip branches / mid-function): 0x0806FFF8,
 *   0x08070406, 0x080706A8.
 *   MISSING entries: 0x080702D8, 0x08070454.
 * See REPORT.md for the evidence.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s8 gUnk_02006094;
extern u16 gUnk_020055C0;
extern u16 gUnk_03001470[];
extern u8 gUnk_03001F30;
extern struct PlayerState gUnk_03002170[];
extern u16 gUnk_03002348;
extern u16 gUnk_03002360;

extern u16 gUnk_030023AC;
extern u16 gUnk_030023E4;
extern s16 gUnk_0300244C;

/* ROM tables */
extern s16 gUnk_0873D420[][3];
extern u8 gUnk_0873F5D4[];
extern s16 *gUnk_0873F950[];
extern u16 gUnk_0873FAB4[];
extern u8 gUnk_0873FAE8[];
extern u32 gUnk_0873FB44[];
extern u32 gUnk_0873FB60[];
extern s16 gUnk_0873FF98[];

/* The 0x0824A9E4 record sub_08070648 uploads from (two tile-count halfwords
   plus two source pointers).  Local to this file until it gets a header. */
struct GfxSrc
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02;
    /*0x04*/ u32 unk04;
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
};
extern struct GfxSrc gUnk_0824A9E4;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_080017e4(u32 mode, void *src, void *dst, u32 size);
extern void sub_08001a94(u32 a, s32 b, u32 c, u32 d, s16 e, s16 f);
extern s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080031b8(s32 a);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(u32 fn, u32 a);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern s32 sub_080063f0(s16 x, s16 y);
extern void sub_0801bcac(u8 *a);
extern u32 sub_08025e88(u32 i);
extern void sub_080261d4(s32 a);
extern void sub_08026264(s32 x, s32 y);
extern s32 sub_0803d55c(s32 a);
extern void sub_0803d7c4(void);
extern void sub_0803db74(void);
extern void sub_0803ddc0(void);
extern void sub_08040808(u32 a);
extern void sub_08068a8c(u32 a, u8 flag);
extern void sub_08068b88(s32 i, u16 b, u8 c, u8 d);
extern void sub_0806d4e4(s32 a, s32 b);
extern void sub_0806ee30(void);
extern void sub_08070ec0(void);
extern void sub_08070ffc(void);

/* Defined below */
void sub_080700e8(void);
void sub_0807029c(void);
void sub_080702d8(void);
void sub_0807042c(void);
void sub_08070454(void);
void sub_0807073c(void);
void sub_08070334(void);
void sub_08070614(u32 a);

void sub_0806ff24(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk88->unk00 == gUnk_03002360)
        sub_08026264(t->unk48, t->unk4A);
    switch (gUnk_03002490->unk24)
    {
    case 1:
    case 2:
        sub_08070334();
        break;
    case 3:
        sub_080702d8();
        break;
    }
}

void sub_0806ff7c(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gUnk_03002490;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    dx = t->unk48 - gUnk_03002348;
    dy = t->unk4A - gUnk_030023E4;
    v = gUnk_03002790[t->unk44].unk18;
    t->unk18 = v;
    if (v <= -2)
        return;
    if (v == -1)
        t->unk40 |= 0xC00;
    else
        t->unk40 &= 0xF3FF;
    u = gUnk_03002490;
    if (u->unk18 > 0)
    {
        sign = (u->unk3E & 0x8000) ? -1 : 1;
        u->unk3E &= 0x7FFF;
        gfx = sub_08001cc8(sub_0803d55c(0),
                           (u16)gUnk_0873FF98[((s16 *)gUnk_03002490)[13]] * sign,
                           gUnk_0873FF98[((s16 *)gUnk_03002490)[13]], 0);
        if (sign < 0)
            gUnk_03002490->unk3E |= 0x8000;
    }
    else
    {
        gfx = sub_0803d55c(0);
    }
    if (gUnk_030023AC > 1)
        sub_0803d7c4();
    sub_0803db74();
    x = dx;
    y = dy;
    if (sub_080063f0(x, y) == 0)
        return;
    sub_08001a94(gUnk_03002490->unk42, gfx, gUnk_03002490->unk3E,
                 gUnk_03002490->unk40 | 0x800, x, y);
}

void sub_080700e8(void)
{
    struct PlayerState *p;
    struct Task *t;
    s16 *tbl;
    s32 k;
    s32 pri;

    tbl = gUnk_0873F950[(p = gUnk_03002490->unk88)->unk0D];
    k = p->unk00;
    if (k == 1 || k == 2)
        if (gUnk_030023AC == 4)
            k += 3;
    sub_08006338(tbl[k]);
    t = gUnk_03002490;
    t->unk28 = t->unk3C;
    switch (gUnk_03002490->unk88->unk00)
    {
    case 0:
        pri = 7;
        break;
    case 1:
        pri = 7;
        break;
    case 2:
        pri = 6;
        break;
    case 3:
        pri = 6;
        break;
    }
    gUnk_03002490->unk42 = pri;
}

void sub_08070174(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk3E &= 0x7FFF;
    if (t->unk34 != 0)
    {
        sub_080061c0(0x10000, 0x5A5A5A5A);
    }
    else
    {
        switch (t->unk88->unk00)
        {
        case 0:
            sub_080061c0(0x10000, 0x5A5A5A5A);
            break;
        case 1:
            sub_080061c0(0x12000, 0x5A5A5A5A);
            break;
        case 2:
            sub_080061c0(0xE000, 0x5A5A5A5A);
            break;
        case 3:
            sub_080061c0(0x14000, 0x5A5A5A5A);
            break;
        }
    }
    sub_0800622c(0xFFFC0000, 0x3000, 0x40000);
    sub_0807029c();
}

void sub_08070208(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08070454;
    t->unk12 = 4;
    gUnk_03002490->unk08 = 0;
    sub_080700e8();
}

void sub_0807022c(void)
{
    while (gUnk_03002490->unk24 != 3)
        TaskYieldTrampoline(1);
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_08070264(void)
{
    struct Task *t;
    s16 *row;

    row = gUnk_0873D420[gUnk_03002490->unk88->unk0D];
    sub_08006338(row[1]);
    t = gUnk_03002490;
    t->unk2C = 1;
    t->unk28 = 2;
}

void sub_0807029c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk2C = -t->unk43;
    t->unk28 = 1;
    t->unk3C = gUnk_0873FAB4[t->unk88->unk0D] + 13;
    t->unk30 = 13;
}

void sub_080702d8(void)
{
    struct Task *t;
    u8 k;
    s32 v;

    k = gUnk_03002490->unk88->unk0D;
    if (k == 1 || k == 2 || (s8)k == 5 || (s8)k == 15 || (s8)k == 16
        || (s8)k == 17 || (s8)k == 19 || (s8)k == 22 || (s8)k == 23)
    {
        t = gUnk_03002490;
        if (t->unk28 <= 0)
        {
            v = t->unk2C;
            t->unk3C += v;
            t->unk28 = 2;
            t->unk2C = -v;
        }
        gUnk_03002490->unk28--;
    }
}

void sub_08070334(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 d;
    s32 v;

    t = gUnk_03002490;
    if (t->unk28 <= 0)
    {
        d = t->unk2C;
        t->unk3C += d;
        v = t->unk30 + d;
        t->unk30 = v;
        t->unk28 = 1;
        if (v > 15)
        {
            t->unk3C = gUnk_0873FAB4[t->unk88->unk0D];
            t->unk30 = 0;
        }
        u = gUnk_03002490;
        if (u->unk30 < 0)
        {
            u->unk3C = gUnk_0873FAB4[u->unk88->unk0D] + 15;
            u->unk30 = 15;
        }
    }
    w = gUnk_03002490;
    w->unk28--;
}

void sub_080703a8(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        switch (u->unk24)
        {
        case 1:
            if (u->unk34 == 1)
            {
                if (gUnk_02006094 == -1)
                    sub_080031b8(153);
                gUnk_02006094 = 0;
            }
            sub_080261d4(4);
            sub_0806d4e4(0, 0);
            sub_08070174();
            break;
        case 2:
            sub_0800622c(0xFFFD0000, 0x3000, 0x40000);
            sub_08070264();
            break;
        }
    }
    sub_0807042c();
}

void sub_0807042c(void)
{
    switch (gUnk_03002490->unk24)
    {
    case 1:
        sub_08070334();
        break;
    case 2:
        sub_080702d8();
        break;
    }
}

void sub_08070454(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk48 = gUnk_03002790[t->unk44].unk48;
    t->unk4A = gUnk_03002790[t->unk44].unk4A;
}

void sub_08070498(u32 a, s32 b)
{
    struct PlayerState *p;
    struct Task *e;
    struct Task *s;
    struct Task *t;
    s16 *tbl;
    s32 k;

    p = &gUnk_03002170[a];
    e = &gUnk_03002790[a];
    s = &gUnk_03002790[b];
    sub_08068a8c(a, 1);
    e->unk48 = s->unk48;
    e->unk4A = s->unk4A;
    e->unk4C = e->unk48 << 16;
    e->unk50 = e->unk4A << 16;
    e->unk12 = 4;
    e->unk44 = b;
    gUnk_020055C0 = b;
    if (gUnk_0300244C != 0)
    {
        if (gUnk_03001F30 == 0)
        {
            tbl = gUnk_0873F950[p->unk0D];
            k = p->unk00;
            if (k == 1 || k == 2)
                if (gUnk_030023AC == 4)
                    k += 3;
            e->unk3C = tbl[k];
            p->unk37 = 0;
            t = gUnk_03002490;
            if (t->unk74 == 0)
            {
                if ((s8)gUnk_0873FAE8[sub_08025e88(gCurTaskIdx)] == 1)
                    e->unk3E &= 0x7FFF;
                else
                    e->unk3E |= 0x8000;
            }
            else
            {
                if ((s8)gUnk_0873FAE8[t->unk74] == 1)
                    e->unk3E &= 0x7FFF;
                else
                    e->unk3E |= 0x8000;
            }
        }
    }
    else
    {
        tbl = gUnk_0873F950[p->unk0D];
        k = p->unk00;
        if (k == 1 || k == 2)
            if (gUnk_030023AC == 4)
                k += 3;
        e->unk3C = tbl[k];
        p->unk37 = 0;
        e->unk3E &= 0x7FFF;
    }
    sub_08006148((u32)sub_0806ee30, a);
}

void sub_08070614(u32 a)
{
    struct PlayerState *p;

    sub_08068b88(a, 0, 1, 0);
    p = gUnk_03002490->unk88;
    if (p->unk0D == 25)
        p->unk37 = 3;
    sub_08040808(a);
}

void sub_08070648(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_08070ec0;
    t->unk04 = (u32)sub_0807073c;
    t->unk08 = 0;
    t->unk12 = 4;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk3E &= 0x7FFF;
    sub_080017e4(2, gUnk_0824A9E4.unk08,
                 &gUnk_03001470[u->unk40 >> 12], gUnk_0824A9E4.unk00 << 5);
    sub_080017e4(3, gUnk_0824A9E4.unk0C,
                 (u16 *)(0x06010000 + ((gUnk_03002490->unk40 & 0xFFF) << 5)),
                 gUnk_0824A9E4.unk02 << 5);
    v = gUnk_03002490;
    v->unk44 = gUnk_020055C0;
    if (gUnk_03002790[i = v->unk44].unk74 == 0)
        gUnk_03002490->unk43 = gUnk_0873FAE8[sub_08025e88(i)];
    else
        v->unk43 = gUnk_0873FAE8[gUnk_03002790[i].unk74];
    w = gUnk_03002490;
    w->unk14 = 2;
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_0873FB44);
}

void sub_0807073c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 7, gUnk_0873FB60);
}

void sub_08070758(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    sub_08002e98(t->unk14, 7, gUnk_0873FB44);
}

void sub_0807077c(void)
{
    gUnk_03002490->unk15 = 2;
    sub_08070ffc();
    sub_08006138();
}

void sub_08070798(void)
{
}

void sub_0807079c(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    t = gUnk_03002490;
    t->unk24 = 1;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk43 = 1;
    sub_08006338(0x11E4);
    sub_080061c0(0x6000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    sub_08006338(0x11E5);
    TaskYieldTrampoline(2);
    sub_08006338(0x11E6);
    TaskYieldTrampoline(2);
    sub_08006338(0x11E7);
    TaskYieldTrampoline(2);
    sub_08006338(0x11E8);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x8000;
    sub_08006338(0x11E9);
    TaskYieldTrampoline(2);
    sub_08006338(0x11EA);
    TaskYieldTrampoline(2);
    sub_08006338(0x11EB);
    TaskYieldTrampoline(2);
    sub_08006338(0x11EC);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_080708ec(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 1)
        {
            sub_080261d4(4);
            sub_0806d4e4(0, 0);
        }
    }
}

void sub_08070930(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 3;
    t = gUnk_03002490;
    t->unk24 = 1;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk42 = 7;
    sub_08006338(0x11E4);
    sub_080061c0(0xC000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    sub_08006338(0x11E5);
    TaskYieldTrampoline(2);
    sub_08006338(0x11E6);
    TaskYieldTrampoline(2);
    sub_08006338(0x11E7);
    TaskYieldTrampoline(2);
    sub_08006338(0x11E8);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x8000;
    sub_08006338(0x11E9);
    TaskYieldTrampoline(2);
    sub_08006338(0x11EA);
    TaskYieldTrampoline(2);
    sub_08006338(0x11EB);
    TaskYieldTrampoline(2);
    sub_08006338(0x11EC);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_08070a84(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 1)
        {
            sub_080261d4(4);
            sub_0806d4e4(0, 0);
        }
    }
}

void sub_08070ac8(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 4;
    t = gUnk_03002490;
    t->unk24 = 0;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk42 = 7;
    gUnk_03002490->unk43 = 1;
    sub_08006338(0x11EC);
    TaskYieldTrampoline(2);
    sub_080061c0(0xFFFF4000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFEC000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF4000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xC000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x14000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_08070c0c(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 1)
        {
            sub_080261d4(4);
            sub_0806d4e4(0, 0);
        }
    }
    sub_0807042c();
}

void sub_08070c54(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 5;
    t = gUnk_03002490;
    t->unk24 = 0;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk42 = 7;
    gUnk_03002490->unk43 = 1;
    sub_08006338(0x11EC);
    sub_080061c0(0xFFFEF000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFD0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_08070d48(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    sub_08026264(t->unk48, t->unk4A);
    sub_0801bcac(gUnk_0873F5D4);
    u = gUnk_03002490;
    if (u->unk7A & 1)
    {
        u->unk7A = 0;
        v = gUnk_03002490;
        v->unk24++;
    }
}

void sub_08070d90(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 6;
    t = gUnk_03002490;
    t->unk24 = 1;
    t->unk34 = 0;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk42 = 7;
    u = gUnk_03002490;
    u->unk3E &= 0x7FFF;
    sub_08006338(0x11E4);
    sub_080061c0(0x9000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x40000;
    while (gUnk_03002490->unk24 != 2)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_08070e7c(void)
{
    struct Task *t;
    struct Task *u;

    sub_0801bcac(gUnk_0873F5D4);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        t->unk7A = 0;
        u = gUnk_03002490;
        u->unk24++;
        if (u->unk24 == 2)
        {
            sub_080261d4(4);
            sub_0806d4e4(0, 0);
        }
    }
}
