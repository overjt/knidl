/* game_code_and_rodata 0x08070EC0-0x08072D8C (issue #79, module M19 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08070EC0 0x08072D8C src/actor_70ec0.c --newpb
 *
 * M19 batch 1: the class-3 entries for task types #74 (sub_08071030) and the
 * warp-star/intro coroutines they install, plus the shared sprite-frame and
 * palette helpers the rest of the bank calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* RAM cells and ROM tables */
extern s16 gUnk_02005588[];
extern s16 gUnk_03002158[];
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s16 gUnk_0300244C;
extern s16 gUnk_0873FF98[];
extern s32 gUnk_030023D4;
extern s32 gUnk_0873FB94[];
extern s8 gUnk_02006160;
extern s8 gUnk_030023B8;
extern struct PlayerState gUnk_03002170[];
extern struct Task * gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u16 gUnk_020055C0;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern u32 gUnk_02004B4C;
extern u32 gUnk_02005584;
extern u32 gUnk_02007D00[];
extern u32 gUnk_0824A9CC[];
extern u32 gUnk_0825D2C8[];
extern u32 gUnk_0873F554[];
extern u32 gUnk_0873F5CC[];
extern u32 gUnk_0873FB7C[];
extern u32 gUnk_0873FBAC[];
extern u32 gUnk_0873FBB8[];
extern u32 gUnk_0873FBC4[];
extern u32 gUnk_0873FC2C[];
extern u32 gUnk_08752D50[];
extern u32 gUnk_08752D8C[];
extern u8 gUnk_020061E0;
extern u8 gUnk_02007CF0;
extern u8 gUnk_03001F30;
extern u8 gUnk_03002340;
extern u8 gUnk_03002350;
extern u8 gUnk_0873FAE8[];
extern vs32 gCurTaskIdx;

/* callees */
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
extern s32 sub_080031b8(u32 a);
extern s32 sub_080063f0(s16 a, s16 b);
extern s32 sub_08025f00();
extern s32 sub_08027750();
extern s32 sub_080277f0();
extern s32 sub_08040934();
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 sub_08064fc4(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern u32 sub_08002ee8(u32 range);
extern u32 sub_08005acc(void);
extern u32 sub_08025e88(s32 i);
extern u32 sub_08068e04(void);
extern u8 sub_08065160(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080034f0(s32 player, s32 songId);
extern void sub_08005654(s32 id);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0801bcac(u32 *p);
extern void sub_080261d4(u32 a);
extern void sub_0806395c(u8 v);
extern void sub_080639b4(u32 v);
extern void sub_08063fe0(void);
extern void sub_080651b4(void);
extern void sub_080656b4(void);
extern void sub_0806572c(void);
extern void sub_08067108(void);
extern void sub_0806d4e4(u32 a, s32 b);
extern void sub_0806ef38(void);
extern void sub_08070454(void);
extern void sub_08070498(u32 a, s32 b);
extern void sub_08070758(void);
extern void sub_08074bb0(int a, int b, int c);

/* defined below */
void sub_08071098(void);
void sub_080711d0(void);
u16 sub_08071360(s32 idx);
void sub_08071778(void);
void sub_080719a0(void);
void sub_08071830(void);

void sub_08070ec0(void)
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
        gfx = sub_08001cc8((s32)gUnk_0824A9CC,
                           (u16)gUnk_0873FF98[u->unk18 >> 16] * sign,
                           gUnk_0873FF98[u->unk18 >> 16], 0);
        if (sign < 0)
            gUnk_03002490->unk3E |= 0x8000;
    }
    else
    {
        gfx = (s32)gUnk_0824A9CC;
    }
    x = dx;
    y = dy;
    if (sub_080063f0(x, y) == 0)
        return;
    sub_08001a94(gUnk_03002490->unk42, gfx, gUnk_03002490->unk3E,
                 gUnk_03002490->unk40 | 0x800, x, y);
}

void sub_08070ffc(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_08070454;
        t->unk12 = 4;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk08 = 0;
        t->unk50 = 0;
        t->unk4C = 0;
        t->unk42 = 7;
    }
    sub_08006338(-1);
}

void sub_08071030(void)
{
    {
        struct Task *t = gUnk_03002490;

        if (t->unk74 != 23)
            t->unk00 = (u32)sub_080656b4;
        else
            t->unk00 = (u32)sub_0806572c;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk0C = (u32)sub_080651b4;
        t->unk42 = 11;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752D50;
        t->unk04 = (u32)sub_08071098;
    }
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0873FBAC);
}

void sub_08071098(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_0873FBB8);
    if (gUnk_02006160 == -1 || gUnk_02006160 == gCurTaskIdx)
    {
        if (sub_08068e04())
            sub_080711d0();
    }
}

void sub_080710e0(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0873FBAC);
}

void sub_080710fc(void)
{
    u32 off;

    {
        struct Task *t = gUnk_03002490;

        if (t->unk38 != gUnk_08752D50)
            return;
        t->unk6E++;
        if (t->unk6E > 5)
            t->unk6E = 0;
    }
    {
        struct Task *t = gUnk_03002490;

        u32 dst;

        t->unk3C = 13;
        off = (t->unk40 & 0x7FF) << 5;
        dst = 0x06010000 + off;
        sub_080017e4(1, gUnk_0873FB7C[t->unk6E], dst, 128);
    }
    sub_080017e4(1, gUnk_0873FB7C[gUnk_03002490->unk6E] + 128, 0x06010400 + off, 128);
    sub_080017e4(1, gUnk_0873FB7C[gUnk_03002490->unk6E] + 256, 0x06010800 + off, 128);
    sub_080017e4(1, gUnk_0873FB7C[gUnk_03002490->unk6E] + 384, 0x06010C00 + off, 128);
}

void sub_080711d0(void)
{
    if (gUnk_0300244C != 0 && gUnk_02005588[gUnk_03002490->unk7E] <= 0)
        return;
    if (gUnk_020061E0 == 0)
    {
        struct Task *t = gUnk_03002490;
        u16 f;

        t->unk3C = 13;
        if (t->unk74 != 23)
        {
            t->unk4A -= 16;
            t->unk4C = t->unk48 << 16;
            t->unk50 = t->unk4A << 16;
        }
        else
        {
            t->unk4A -= 8;
            t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
            t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        }
        f = sub_08071360(gUnk_03002490->unk7E);
        {
            struct Task *t2 = gUnk_03002490;

            t2->unk40 = f;
            gUnk_02006160 = gCurTaskIdx;
            gUnk_02007CF0 = 2;
            if (t2->unk46 != -1)
            {
                sub_08005654(t2->unk46);
                gUnk_03002490->unk46 = 0xFFFF;
            }
        }
        sub_080639b4((u32)gUnk_0873F554);
    }
    gUnk_03002170[gUnk_03002490->unk7E].unk04 = 16;
    sub_08040934(gUnk_03002490->unk7E);
    gUnk_03002490->unk28++;
    sub_08070498(gUnk_03002490->unk7E, gCurTaskIdx);
    sub_0806d4e4(0, 0);
    if (gUnk_03002360 == gUnk_03002490->unk7E)
        sub_080031b8(219);
    if (gUnk_030023B8 == 7)
        gUnk_02007D00[9] = 1;
    sub_0806395c(1);
    sub_08006148(sub_080710e0, gCurTaskIdx);
}

u16 sub_08071360(s32 idx)
{
    u16 v = gUnk_03002790[idx].unk40 & 0x7FF;
    u32 off = v << 5;

    sub_080017e4(1, (u32)gUnk_0825D2C8, 0x06010180 + off, 128);
    sub_080017e4(1, (u32)gUnk_0825D2C8 + 128, 0x06010580 + off, 128);
    sub_080017e4(1, (u32)gUnk_0825D2C8 + 256, 0x06010980 + off, 128);
    sub_080017e4(1, (u32)gUnk_0825D2C8 + 384, 0x06010D80 + off, 128);
    gUnk_020061E0++;
    return v + 0xF00C;
}

void sub_080713f8(int x, int y, int c)
{
    sub_08064fc4(5, 0, 0, c, x, y, 0);
}

void sub_08071418(void)
{
    gUnk_03002490->unk15 = 0;
    {
        struct Task *t = gUnk_03002490;

        t->unk28 = 0;
        t->unk2C = 16;
        t->unk30 = 0;
    }
    gUnk_03002490->unk46 = sub_08064d34(164, 0);
    while (1)
    {
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(3);
    }
}

void sub_0807156c(void)
{
    if (gUnk_02007CF0 != 0 && gUnk_03002350 != 1)
    {
        struct Task *t = gUnk_03002490;

        if (t->unk46 != -1)
        {
            sub_08005654(t->unk46);
            gUnk_03002490->unk46 = 0xFFFF;
        }
        sub_0806395c(2);
        sub_08006148(sub_080710e0, gCurTaskIdx);
    }
    else
    {
        {
            struct Task *t = gUnk_03002490;

            if (t->unk2C <= 0)
            {
                t->unk58 = gUnk_0873FB94[t->unk30];
                t->unk30++;
                if (t->unk30 > 5)
                    t->unk30 = 0;
                gUnk_03002490->unk2C = 16;
            }
        }
        gUnk_03002490->unk2C--;
    }
}

void sub_0807160c(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    while (1)
    {
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(18);
    }
}

void sub_08071640(void)
{
    sub_080710fc();
    if (gUnk_03002490->unk28 == gUnk_03002350 && sub_08027750())
    {
        struct Task *t = gUnk_03002490;
        u32 v = t->unk74;

        if (v == 0)
        {
            v = sub_08025e88(gCurTaskIdx);
            t = gUnk_03002490;
        }
        t->unk14 = v;
        sub_08006148(sub_08071778, gCurTaskIdx);
    }
}

void sub_08071694(void)
{
    gUnk_03002490->unk04 = 0;
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752D8C;
        t->unk3C = 0;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(1);
    sub_08063fe0();
}

void sub_08071774(void)
{
}

void sub_08071778(void)
{
    struct Task *t;

    {
        struct Task *u = gUnk_03002490;

        u->unk0C = (u32)sub_080719a0;
        u->unk42 = 10;
    }
    t = gUnk_03002490;
    t->unk38 = gUnk_08752D50;
    t->unk04 = (u32)sub_08071830;
    t->unk18 = 0;
    t->unk28 = 0;
    t->unk78 = 0xFFFF;
    t->unk70 = 0;
    if (t->unk74 == 0)
        gUnk_03002490->unk43 = gUnk_0873FAE8[sub_08025e88(gCurTaskIdx)];
    else
        t->unk43 = gUnk_0873FAE8[t->unk74];
    sub_08064d6c(97, gUnk_03002158[0] + 120, gUnk_03002158[2] + 80, 0);
    sub_08002e98(gUnk_03002490->unk14, 26, gUnk_0873FBC4);
}

void sub_08071830(void)
{
    sub_080710fc();
    sub_08002e98(gUnk_03002490->unk15, 26, gUnk_0873FC2C);
}

void sub_08071850(void)
{
    sub_08002e98(gUnk_03002490->unk14, 26, gUnk_0873FBC4);
}

void sub_0807186c(int a, int b, int c, int d)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk78 = (s8)a;
        t->unk74 = b;
    }
    gUnk_03002490->unk75 = c;
    gUnk_03002490->unk34 = (s16)d;
}

void sub_08071898(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk78 = 0xFFFF;
        t->unk74 = 0;
    }
    gUnk_03002490->unk75 = 0;
    gUnk_03002490->unk34 = 0;
}

void sub_080718c0(void)
{
    struct Task *t = gUnk_03002490;
    u32 v;

    if (t->unk78 == -1)
        return;
    t->unk70--;
    if ((s16)t->unk70 > 0)
        return;
    if ((s8)t->unk75 == -1)
    {
        if (t->unk78 == 2 || t->unk78 == 0)
        {
            v = (u8)(u16)t->unk78;
        }
        else
        {
            s32 r = sub_08002ee8(3);

            v = 1;
            if (r != 0)
                v = 0;
        }
        sub_08074bb0(gUnk_03002490->unk74 << 5, (s16)gUnk_03002490->unk34, v);
        gUnk_03002490->unk78 = 0xFFFF;
    }
    else
    {
        gUnk_030023D4 = sub_08002ee8(32) - 16;
        gUnk_030023D4 += gUnk_03002490->unk74 << 5;
        if (gUnk_030023D4 < 0)
            gUnk_030023D4 += 512;
        {
            u16 a1 = gUnk_030023D4;
            s32 a2 = (s16)gUnk_03002490->unk34;
            register s32 r asm("r2");
            register s32 c asm("r0");

            r = sub_08002ee8(3);
            c = (u8)gUnk_03002490->unk78;
            r &= c;
            sub_08074bb0(a1, a2, r);
        }
        gUnk_03002490->unk70 = (s8)gUnk_03002490->unk75;
    }
}

void sub_080719a0(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u32 *g;

    v = t->unk18;
    if (v <= -2)
    {
        if (gUnk_03001F30 != 0)
            return;
        g = t->unk38;
        sub_08001a94(t->unk42, g[t->unk3C], t->unk3E, t->unk40,
                     t->unk48 - gUnk_03002348, t->unk4A - gUnk_030023E4);
        return;
    }
    if (v == -1)
        t->unk40 |= 0xC00;
    else
        t->unk40 &= 0xF3FF;
    {
        struct Task *w = gUnk_03002490;

        if (w->unk18 > 0)
        {
            w->unk18 += w->unk28;
            if (w->unk18 < 0)
                w->unk18 = 0;
            if (gUnk_03002490->unk18 > 0x3F0000)
                gUnk_03002490->unk18 = 0x3F0000;
        }
    }
    if (gUnk_03001F30 != 0)
        return;
    if (gUnk_03002490->unk38 == NULL)
        return;
    if (gUnk_03002490->unk3C == -1)
        return;
    if (sub_08065160() == 0)
        return;
    if (sub_08005acc() == 0)
        return;
    u = gUnk_03002490;
    g = u->unk38;
    if (u->unk18 > 0)
    {
        sign = (u->unk3E & 0x8000) ? -1 : 1;
        u->unk3E &= 0x7FFF;
        gfx = sub_08001cc8(g[u->unk3C],
                           (u16)gUnk_0873FF98[u->unk18 >> 16] * sign,
                           gUnk_0873FF98[u->unk18 >> 16], 0);
        {
            struct Task *x = gUnk_03002490;

            sub_08001a94(x->unk42, gfx, x->unk3E, x->unk40,
                         x->unk48 - gUnk_03002348, x->unk4A - gUnk_030023E4);
        }
        if (sign < 0)
            gUnk_03002490->unk3E |= 0x8000;
    }
    else
    {
        sub_08001a94(u->unk42, g[u->unk3C], u->unk3E, u->unk40,
                     u->unk48 - gUnk_03002348, u->unk4A - gUnk_030023E4);
    }
}

void sub_08071bb0(u16 a)
{
    s32 i;

    if (gUnk_03001F30 != 0)
        while (1)
            ;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            struct Task *t = &gUnk_03002790[i];

            t->unk43 = gUnk_03002490->unk43;
            t->unk4C = t->unk48 << 16;
            t->unk50 = t->unk4A << 16;
            t->unk14 = a;
            sub_08006148(sub_0806ef38, i);
        }
    }
}

void sub_08071c38(u16 a)
{
    s32 i;

    if (gUnk_03001F30 != 1)
        while (1)
            ;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            struct Task *t = &gUnk_03002790[i];

            t->unk43 = gUnk_03002490->unk43;
            t->unk4C = t->unk48 << 16;
            t->unk50 = t->unk4A << 16;
            t->unk14 = a;
            sub_08006148(sub_08070758, i);
        }
    }
}

void sub_08071cc0(int x, int y, int c)
{
    s32 id = sub_08064fc4(5, 0, 0, c, x >> 16, y >> 16, 0);

    if (id != -1)
    {
        struct Task *t = &gUnk_03002790[id];

        t->unk14 = c;
        sub_08006148(sub_08071778, id);
        t->unk40 = sub_08071360(gCurTaskIdx);
    }
    gUnk_020055C0 = id;
    sub_08067108();
}

void sub_08071d2c(void)
{
    sub_080031b8(219);
    sub_0800622c(0x30000, -0x4000, 0x30000);
    TaskYieldTrampoline(4);
    sub_080031b8(216);
    gUnk_03002490->unk24 = 0;
}

void sub_08071d60(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    {
        t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
        t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
        t->unk00 = (u32)sub_0806572c;
        t->unk24 = 0;
    }
    {
        s32 r;

        gUnk_02005584 = 216;
        r = sub_080031b8(216);
        gUnk_02004B4C = r;
    }
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = Div(0x780000 - gUnk_03002490->unk50, 32);
    TaskYieldTrampoline(32);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    sub_08064d6c(148, gUnk_03002490->unk48, gUnk_03002490->unk4A + 16, 0);
    sub_080261d4(2);
    sub_080031b8(272);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    sub_0807186c(1, 4, 4, 0x400);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x14000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x28000;
    TaskYieldTrampoline(40);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_08071e74(void)
{
    sub_080718c0();
}

void sub_08071e80(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    sub_080062c4();
    sub_080061c0(0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0x20000;
    sub_08006138();
}

void sub_08071ebc(void)
{
    struct Task *t;

    sub_080718c0();
    sub_0801bcac(gUnk_0873F5CC);
    t = gUnk_03002490;
    if (t->unk7A & 1)
    {
        sub_080277f0(t->unk48, t->unk4A);
        sub_080261d4(4);
        sub_080034f0(gUnk_02004B4C, gUnk_02005584);
        sub_080031b8(219);
        sub_0806d4e4(0, 0);
        sub_080031b8(272);
        if (gUnk_03001F30 == 0)
            sub_08071bb0(3);
        else
            sub_08071c38(3);
        gUnk_020061E0 = 0;
        sub_08063fe0();
    }
}

void sub_08071f54(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 5;
    t = gUnk_03002490;
    t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
    t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
    t->unk00 = (u32)sub_0806572c;
    t->unk24 = 0;
    sub_080062c4();
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    {
        s32 r;

        gUnk_02005584 = 126;
        r = sub_080031b8(126);
        gUnk_02004B4C = r;
    }
    gUnk_03002490->unk54 = -0x8000;
    sub_0807186c(1, 4, 4, 0x600);
    gUnk_03002490->unk58 = -0x80000;
    TaskYieldTrampoline(6);
    sub_0807186c(1, 5, 4, 0x600);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(6);
    sub_0807186c(1, 6, 4, 0x600);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(6);
    sub_0807186c(1, 7, 6, 0x600);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(6);
    sub_0807186c(-1, 0, 0, 0);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(6);
    sub_080031b8(272);
    sub_0807186c(1, 4, 4, 0x600);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x20000;
        u->unk58 = -0x80000;
    }
    TaskYieldTrampoline(12);
    sub_080062c4();
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_080720dc(void)
{
    sub_080718c0();
}

void sub_080720e8(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 6;
    t = gUnk_03002490;
    t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
    t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
    t->unk00 = (u32)sub_0806572c;
    t->unk24 = 0;
    sub_0807186c(1, 4, 4, 0x300);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x40000;
        u->unk58 = -0x80000;
    }
    TaskYieldTrampoline(4);
    sub_0807186c(1, 2, 4, 0x300);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(3);
    sub_0807186c(-1, 0, 0, 0);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(4);
    sub_080031b8(272);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(2);
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x10000;
        u->unk58 = -0x80000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(3);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x40000;
        u->unk58 = -0x60000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    sub_0807186c(-1, 0, 0, 0);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(8);
    sub_080031b8(272);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0;
        u->unk58 = -0x60000;
    }
    TaskYieldTrampoline(2);
    sub_0807186c(1, 1, 4, 0x300);
    gUnk_03002490->unk54 = -0x60000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(3);
    sub_0807186c(-1, 0, 0, 0);
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk58 = 0x80000;
    TaskYieldTrampoline(3);
    sub_080031b8(272);
    sub_080062c4();
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(2);
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x40000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(14);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(10);
    sub_0807186c(1, 6, 8, 0x300);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    sub_0807186c(1, 6, 10, 0x300);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_0807237c(void)
{
    sub_080718c0();
}

void sub_08072388(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 8;
    t = gUnk_03002490;
    t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
    t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
    t->unk00 = (u32)sub_0806572c;
    t->unk24 = 0;
    {
        s32 r;

        gUnk_02005584 = 126;
        r = sub_080031b8(126);
        gUnk_02004B4C = r;
    }
    gUnk_03002490->unk18 = 0x3F0000;
    sub_08071898();
    sub_080062c4();
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x60000;
        u->unk58 = -0x30000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x20000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x10000;
        u->unk58 = -0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x10000;
        u->unk58 = -0x8000;
    }
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = -0x20000;
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x40000;
        u->unk58 = 0x2000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x60000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x40000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x20000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x10000;
        u->unk58 = 0x30000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x8000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x10000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x20000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(12);
    sub_08064d6c(148, gUnk_03002490->unk48, gUnk_03002490->unk4A + 16, 0);
    sub_080261d4(2);
    sub_080031b8(272);
    gUnk_03002490->unk28 = -0x2600;
    sub_0807186c(1, 4, 3, 0x400);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(8);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x8000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    sub_080062c4();
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(16);
    sub_0807186c(-1, 0, 0, 0);
    sub_080062c4();
    gUnk_03002490->unk54 = 0xC000;
    TaskYieldTrampoline(48);
    {
        struct Task *u = gUnk_03002490;

        u->unk18 = 0x100000;
        u->unk28 = -0xC00;
    }
    sub_080034f0(gUnk_02004B4C, gUnk_02005584);
    {
        s32 r;

        gUnk_02005584 = 217;
        r = sub_080031b8(217);
        gUnk_02004B4C = r;
    }
    sub_080062c4();
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(40);
    gUnk_03002490->unk58 = 0x6000;
    TaskYieldTrampoline(40);
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(40);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(40);
    gUnk_03002490->unk58 = 0x1000;
    TaskYieldTrampoline(40);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_08072678(void)
{
    sub_080718c0();
}

void sub_08072684(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 10;
    t = gUnk_03002490;
    t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
    t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
    t->unk00 = (u32)sub_0806572c;
    t->unk24 = 0;
    {
        s32 r;

        gUnk_02005584 = 250;
        r = sub_080031b8(250);
        gUnk_02004B4C = r;
    }
    sub_08071898();
    sub_080062c4();
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(6);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x8000;
        u->unk58 = -0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x10000;
        u->unk58 = -0x10000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x30000;
        u->unk58 = -0x40000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = -0x20000;
    TaskYieldTrampoline(8);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x10000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x8000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(8);
    sub_0807186c(1, 6, 4, 0x300);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = 0x20000;
            u->unk58 = -0x20000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x20000;
            u->unk58 = 0x20000;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    sub_08064d6c(148, gUnk_03002490->unk48, gUnk_03002490->unk4A + 16, 0);
    sub_080261d4(2);
    sub_080031b8(272);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x20000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(2);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x20000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(2);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x80000;
        u->unk58 = -0x60000;
    }
    TaskYieldTrampoline(12);
    gUnk_03002490->unk54 = 0x60000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = 0x30000;
    TaskYieldTrampoline(4);
    sub_080062c4();
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_080728a4(void)
{
    sub_080718c0();
}

void sub_080728b0(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 11;
    t = gUnk_03002490;
    t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
    t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
    t->unk00 = (u32)sub_0806572c;
    t->unk24 = 0;
    sub_08071898();
    sub_080062c4();
    gUnk_03002490->unk54 = 0x80000;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk6C = 0;
    while (1)
    {
        sub_0807186c(1, 7, 4, 0x300);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = 0x80000;
            u->unk58 = -0x20000;
        }
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = 0x60000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = 0x40000;
        TaskYieldTrampoline(6);
        sub_0807186c(1, 5, -1, 0x300);
        gUnk_03002490->unk54 = 0x20000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x10000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk18 = -1;
        sub_0807186c(1, 1, 4, 0x300);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x10000;
            u->unk58 = -0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x20000;
            u->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x40000;
            u->unk58 = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x80000;
            u->unk58 = 0x30000;
        }
        TaskYieldTrampoline(18);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x40000;
            u->unk58 = 0x10000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x20000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk18 = 0;
        sub_0807186c(1, 7, 4, 0x300);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = 0x20000;
            u->unk58 = -0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = 0x40000;
            u->unk58 = -0x10000;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
        if ((s16)gUnk_03002490->unk6C > 2)
            break;
    }
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x80000;
        u->unk58 = -0x20000;
    }
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x30000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 5, -1, 0x300);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 4, -1, 0x300);
    gUnk_03002490->unk58 = -0x80000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskYieldTrampoline(4);
    sub_08025f00();
    gUnk_03002490->unk24 = 1;
    sub_08006138();
}

void sub_08072af4(void)
{
    sub_080718c0();
}

void sub_08072b00(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 12;
    t = gUnk_03002490;
    t->unk4C = (t->unk48 - gUnk_03002158[0]) << 16;
    t->unk50 = (t->unk4A - gUnk_03002158[2]) << 16;
    t->unk00 = (u32)sub_0806572c;
    {
        s32 r;

        gUnk_02005584 = 218;
        r = sub_080031b8(218);
        gUnk_02004B4C = r;
    }
    sub_080062c4();
    sub_0807186c(1, 7, 4, 0x300);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = 0x60000;
        u->unk58 = -0x30000;
    }
    TaskYieldTrampoline(14);
    sub_0807186c(1, 5, 4, 0x300);
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x60000;
    TaskYieldTrampoline(4);
    sub_0807186c(1, 4, 4, 0x300);
    gUnk_03002490->unk6C = 0;
    do
    {
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = 0x60000;
            u->unk58 = -0x60000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gUnk_03002490;

            u->unk54 = -0x60000;
            u->unk58 = 0x60000;
        }
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = -0x60000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = 0x60000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk58 = 0x20000;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_08071898();
    sub_08006244();
    TaskYieldTrampoline(8);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x800;
        u->unk58 = 0x800;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x2000;
        u->unk58 = 0x2000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x8000;
        u->unk58 = 0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x10000;
        u->unk58 = 0x10000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x20000;
        u->unk58 = 0x20000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = -0x40000;
        u->unk58 = 0x40000;
    }
    TaskYieldTrampoline(6);
    gUnk_03002490->unk54 = -0x80000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk43 = 255;
    sub_080277f0(gUnk_03002490->unk48, gUnk_03002490->unk4A);
    sub_080261d4(4);
    sub_080034f0(gUnk_02004B4C, gUnk_02005584);
    sub_080031b8(219);
    sub_0806d4e4(0, 0);
    sub_080031b8(272);
    if (gUnk_03001F30 == 0)
        sub_08071bb0(4);
    else
        sub_08071c38(4);
    gUnk_020061E0 = 0;
    sub_08063fe0();
}

void sub_08072d80(void)
{
    sub_080718c0();
}
