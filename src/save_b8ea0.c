#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern vu16 gUnk_03001EB8[];    /* per-player keys pressed */
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern struct Task *gUnk_03002490;
extern void TaskDispatchTrampoline(void);
extern void sub_08009cb0(s32 a);
extern void sub_08005654(s32 a);
extern void sub_08009e60(s32 a, s32 b);
extern void sub_0800b318(u8 *s, s32 a, s32 b, s32 c);
extern void sub_0800b34c(s32 a, s32 b, s32 c);
extern void TaskYieldTrampoline(u32 frames);
extern s32 gCurTaskIdx;
extern u16 gUnk_02000010[];
extern struct Unk02005E00 gUnk_02005E00;
extern s16 gUnk_02007D48[];
extern u32 gUnk_03001270[];
extern s8 gUnk_03001F08[];
extern u8 gUnk_03002340;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern u8 gUnk_085ADD1C[];
extern u8 gUnk_085B09BC[];
extern u8 gUnk_085B09DC[];
extern u8 gUnk_085B0A10[];
extern u8 gUnk_085B0A30[];
extern u8 gUnk_085B0A64[];
extern u8 gUnk_085B0AB0[];
extern u8 gUnk_085B0AD0[];
extern u8 gUnk_085B0AD4[];
extern u8 gUnk_085B0AFC[];
extern u8 gUnk_085B0B00[];
extern u8 gUnk_085B0B08[];
extern u8 gUnk_085B0B10[];
extern u8 gUnk_085B0B28[];
extern u8 gUnk_085B0B5C[];
extern u8 gUnk_085B0BB4[];
extern u8 gUnk_085B0BD4[];
extern u8 gUnk_085B0C2C[];
extern u32 gUnk_0875625C[];
extern u16 gUnk_08756268[];
extern u32 gUnk_08756270[];
extern u32 gUnk_0875628C[];
extern void sub_08006148(void *fn, s32 i);
extern s32 sub_08002f14(s16 n);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern void sub_08006138(void);
extern void sub_0800b3a8(void);

void sub_080b8ea0(void);
void sub_080b8ef4(void);
void sub_080b8f8c(s32 a);
void sub_080b8ff0(void);
void sub_080b902c(void);
void sub_080b9064(void);
void sub_080b9090(void);
void sub_080b90c8(void);
void sub_080b90f8(void);
void sub_080b9108(void);
void sub_080b9118(void);
void sub_080b9140(void);
void sub_080b9198(void);
void sub_080b91fc(void);
void sub_080b927c(void);
void sub_080b9344(void);
void sub_080b938c(void);
void sub_080b93d8(void);
s32 sub_080b9424(void);
void sub_080b94b4(s32 a, s32 b);
void sub_080b9578(void);
void sub_080b95ac(void);
void sub_080b95ec(void);
void sub_080b963c(void);
void sub_080b9658(void);
void sub_080b97fc(s32 a);
void sub_080b9878(void);
void sub_080b98c0(void);
void sub_080b9968(s32 a, s32 b);
void sub_080b99e8(s32 a, s32 b, s32 c);
void sub_080b9a88(s32 a);
void sub_080b9b08(s32 a);
void sub_080b9b98(s32 a);
void sub_080b9c28(void);
void sub_080b9c74(void);
void sub_080b9cc0(void);

void sub_080b8ea0(void)
{
    struct Unk02005E00 *s;
    struct Task *t;
    u8 *f;
    u8 *e;
    s32 z;

    s = &gUnk_02005E00;
    t = gUnk_03002490;
    f = (u8 *)s + 4;
    e = &f[t->unk1C];
    z = 0;
    *e = z;
    s->unk00 = z;
}
void sub_080b8ebc(void)
{
    struct Unk02005E00 *s;
    u8 *f;
    u8 *e;
    s32 i;
    s32 v;
    s32 z;

    i = 0;
    s = &gUnk_02005E00;
    f = (u8 *)s + 4;
    z = 0;
    do
    {
        e = (u8 *)(i + (u32)f);
        v = *e << 24;
        if (v != 0 && (u32)v >> 28 == gCurTaskIdx)
        {
            *e = z;
            s->unk00 = z;
        }
        i++;
    } while (i <= 3);
}
void sub_080b8ef4(void)
{
    register vs32 *ip asm("r5");
    struct Unk02005E00 *s;
    struct Task *t;
    u8 *f;
    u8 *g;
    u8 *e;
    s32 i;
    s32 n;
    s32 v;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    s = &gUnk_02005E00;
    ip = &gCurTaskIdx;
    g = (u8 *)s + 8;
    if (g[*ip] != 0)
    {
        t->unk14 = 6;
    }
    else
    {
        i = 0;
        n = 0;
        f = (u8 *)s + 4;
        do
        {
            e = (u8 *)(i + (u32)f);
            v = *e << 24;
            if (v != 0 && (u32)v >> 28 == *ip)
            {
                gUnk_03002490->unk1C = i;
                n++;
            }
            i++;
        } while (i <= 3);
        if (n != 0)
        {
            sub_080b8f8c(1);
            if (gUnk_02007D48[gUnk_03002490->unk1C] > 0)
                gUnk_03002490->unk14 = 2;
            else
                gUnk_03002490->unk14 = 4;
        }
        else
        {
            gUnk_03002490->unk14 = n;
        }
    }
}
void sub_080b8f8c(s32 a)
{
    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_080017e4(2, (u32)gUnk_085ADD1C, (u32)gUnk_03001270, 64);
        if (a <= 2)
            sub_080017e4(1, gUnk_0875625C[a], 192 << 19, gUnk_08756268[a] << 5);
    }
}
void sub_080b8ff0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk18 = 0;
    if (gUnk_03002360 == t->unk88->unk00)
    {
        sub_080b8f8c(0);
        sub_0800b3a8();
        sub_080b97fc(gUnk_03002490->unk18);
    }
}
void sub_080b902c(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gUnk_02000010[i] |= 0xFFFF;
    gUnk_03002490->unk18 = 0;
    gUnk_03002490->unk24 = 0;
    sub_080b98c0();
}
void sub_080b9064(void)
{
    struct Unk02005E00 *s;

    s = &gUnk_02005E00;
    if (s->unk00 == 0)
        s->unk00 = 1200;
    sub_080b8f8c(1);
    sub_080b9b08(gUnk_03002490->unk1C);
}
void sub_080b9090(void)
{
    sub_08009e60(-1, gUnk_03002490->unk1C);
    sub_08009e60(1, gUnk_03002490->unk88->unk00);
    sub_080b8ea0();
    sub_080b9b98(gUnk_03002490->unk1C);
}
void sub_080b90c8(void)
{
    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
        sub_08009cb0(gUnk_03002360);
    TaskDispatchTrampoline();
}
void sub_080b90f8(void)
{
    sub_080b8ea0();
    sub_080b9c28();
}
void sub_080b9108(void)
{
    sub_080b8f8c(2);
    sub_080b9c74();
}
void sub_080b9118(void)
{
    struct Unk02005E00 *s;
    vs32 *ip;
    u8 *g;

    sub_080b8f8c(2);
    sub_080b9cc0();
    s = &gUnk_02005E00;
    ip = &gCurTaskIdx;
    g = (u8 *)s + 8;
    g[*ip] = 1;
}
void sub_080b9140(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;

    k = gUnk_03001EB8;
    t = gUnk_03002490;
    e = &k[t->unk88->unk00];
    if ((*e & 0xC0) != 0)
    {
        if ((*e & 0x40) != 0)
        {
            if (t->unk18 == 1)
            {
                t->unk18 = 0;
                sub_080b97fc(0);
            }
        }
        else if (t->unk18 == 0)
        {
            t->unk18 = 1;
            sub_080b97fc(1);
        }
    }
}
void sub_080b9198(void)
{
    vu16 *k;
    struct Task *t;

    k = gUnk_03001EB8;
    t = gUnk_03002490;
    if ((k[t->unk88->unk00] & 1) != 0)
    {
        if (t->unk18 == 0)
        {
            if (sub_080b9424() != 0)
                gUnk_03002490->unk14 = 1;
            else
                gUnk_03002490->unk14 = 5;
        }
        else
        {
            t->unk14 = 6;
        }
        sub_08006148(sub_080b9658, gCurTaskIdx);
    }
}
void sub_080b91fc(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;

    if (gUnk_03002490->unk18 > gUnk_03002490->unk20)
    {
        do
        {
            gUnk_03002490->unk18--;
            sub_080b9a88(gUnk_03002490->unk18);
        } while (gUnk_03002490->unk18 > gUnk_03002490->unk20);
    }
    k = gUnk_03001EB8;
    t = gUnk_03002490;
    e = &k[t->unk88->unk00];
    if ((*e & 0xC0) != 0)
    {
        if ((*e & 0x40) != 0)
        {
            if (t->unk18 != 0)
            {
                t->unk18--;
                sub_080b9a88(t->unk18);
            }
        }
        else if (t->unk18 < t->unk20)
        {
            t->unk18++;
            sub_080b9a88(t->unk18);
        }
    }
}
void sub_080b927c(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;
    s32 i;
    s32 j;

    k = gUnk_03001EB8;
    t = gUnk_03002490;
    e = &k[t->unk88->unk00];
    if ((*e & 11) != 0)
    {
        if ((*e & 1) != 0)
        {
            for (i = 0, j = 0; i < gUnk_030023AC; i++)
            {
                if (i == gCurTaskIdx)
                    continue;
                if ((1 & (gUnk_03002490->unk24 >> i)) == 0)
                    continue;
                if (j == gUnk_03002490->unk18)
                {
                    gUnk_03002490->unk1C = i;
                    break;
                }
                j++;
            }
            gUnk_02005E00.unk04[gUnk_03002490->unk1C] = (gCurTaskIdx << 4) | 1;
            gUnk_03002490->unk14 = 2;
        }
        else
        {
            t->unk14 = 0;
        }
    }
}
void sub_080b9344(void)
{
    vu16 *k;
    s32 p;

    k = gUnk_03001EB8;
    p = gUnk_03002490->unk88->unk00;
    if ((k[p] & 1) != 0)
    {
        if (gUnk_03002360 == p)
            sub_08009cb0(p);
        sub_08005654(gCurTaskIdx);
    }
}
void sub_080b938c(void)
{
    vu16 *k;
    struct Task *t;

    k = gUnk_03001EB8;
    t = gUnk_03002490;
    if ((k[t->unk88->unk00] & 1) != 0)
        t->unk14 = 0;
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_080b9658, gCurTaskIdx);
}
void sub_080b93d8(void)
{
    vu16 *k;
    struct Task *t;

    k = gUnk_03001EB8;
    t = gUnk_03002490;
    if ((k[t->unk88->unk00] & 1) != 0)
        t->unk14 = 0;
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080b9658, gCurTaskIdx);
}
s32 sub_080b9424(void)
{
    s32 i;
    s32 n;
    u8 *f;

    gUnk_03002490->unk24 = 0;
    for (i = 0, n = 0; i < gUnk_030023AC; i++)
    {
        if ((1 & (gUnk_03002340 >> i)) != 0 && gUnk_02007D48[i] > 0 && gUnk_02005E00.unk04[i] == 0)
        {
            n++;
            gUnk_03002490->unk24 |= 1 << i;
        }
    }
    gUnk_03002490->unk20 = n - 1;
    return n;
}
void sub_080b94b4(s32 a, s32 b)
{
    struct Task *t;
    s32 i;
    s32 j;
    s32 v;
    u16 u;
    u8 *f;
    s16 *e;
    s16 *q;

    t = gUnk_03002490;
    if (a != t->unk20 || b != t->unk24)
    {
        sub_080b98c0();
    }
    else
    {
        for (i = 0, j = 0; i < gUnk_030023AC; i++)
        {
            if ((1 & (gUnk_03002340 >> i)) != 0 && gUnk_02005E00.unk04[i] == 0)
            {
                e = &gUnk_02007D48[i];
                u = *e;
                v = *e;
                if (v > 0)
                {
                    if (((gUnk_03002490->unk24 >> i) & 1) != 0)
                    {
                        q = (s16 *)&gUnk_02000010[i];
                        if (*q != v)
                            *q = u;
                    }
                    sub_080b99e8(i, j, gUnk_02007D48[i]);
                    j++;
                }
            }
        }
    }
}
void sub_080b9578(void)
{
    struct Task *t;
    u8 v;
    u8 *p;

    p = (u8 *)&gUnk_02005E00;
    t = gUnk_03002490;
    p += 4;
    v = p[t->unk1C];
    if ((v & 2) != 0)
    {
        if ((v >> 4) == gCurTaskIdx)
            t->unk14 = 3;
    }
}
void sub_080b95ac(void)
{
    if (gUnk_02005E00.unk00 <= 0)
        gUnk_03002490->unk14 = 4;
    gUnk_02005E00.unk00--;
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080b9658, gCurTaskIdx);
}
void sub_080b95ec(void)
{
    struct Task *t;
    s16 *p;

    p = gUnk_02007D48;
    t = gUnk_03002490;
    if (p[t->unk1C] <= 0)
        t->unk14 = 4;
}
void sub_080b9610(void)
{
    gUnk_03002490->unk04 = (u32)sub_080b963c;
    sub_080b8ef4();
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08756270);
}
void sub_080b963c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 7, gUnk_0875628C);
}
void sub_080b9658(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08756270);
}
void sub_080b9674(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080b8ff0();
    sub_08006138();
}
void sub_080b9690(void)
{
    sub_080b9140();
    sub_080b9198();
}
void sub_080b96a0(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080b902c();
    sub_08006138();
}
void sub_080b96bc(void)
{
    s32 a;
    s32 b;

    a = gUnk_03002490->unk20;
    b = gUnk_03002490->unk24;
    if (sub_080b9424() != 0)
    {
        sub_080b94b4(a, b);
        sub_080b91fc();
        sub_080b927c();
    }
    else
    {
        gUnk_03002490->unk14 = 5;
    }
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080b9658, gCurTaskIdx);
}
void sub_080b9710(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080b9064();
    while (1)
    {
        TaskYieldTrampoline(4);
        sub_080b95ec();
    }
}
void sub_080b9730(void)
{
    sub_080b9578();
    sub_080b95ac();
}
void sub_080b9740(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080b9090();
    TaskYieldTrampoline(180);
    sub_080b90c8();
    sub_08006138();
}
void sub_080b9764(void)
{
    sub_080b9344();
}
void sub_080b9770(void)
{
    gUnk_03002490->unk15 = 4;
    sub_080b90f8();
    TaskYieldTrampoline(180);
    gUnk_03002490->unk14 = 0;
    sub_08006138();
}
void sub_080b9798(void)
{
    sub_080b938c();
}
void sub_080b97a4(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080b9108();
    TaskYieldTrampoline(300);
    gUnk_03002490->unk14 = 0;
    sub_08006138();
}
void sub_080b97d0(void)
{
    sub_080b93d8();
}
void sub_080b97dc(void)
{
    gUnk_03002490->unk15 = 6;
    sub_080b9118();
    sub_08006138();
}
void sub_080b97f8(void)
{
}
void sub_080b97fc(s32 a)
{
    u8 *p;
    u8 *q;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        if (a == 0)
        {
            p = gUnk_085B09BC;
            q = gUnk_085B09DC;
        }
        else
        {
            p = gUnk_085B0A10;
            q = gUnk_085B0A30;
        }
        sub_0800b318(p, 1, 2, 8);
        sub_0800b318(p + 16, 1, 3, 8);
        sub_0800b318(q, 1, 4, 13);
        sub_0800b318(q + 26, 1, 5, 13);
    }
}
void sub_080b9878(void)
{
    u8 *p;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        p = gUnk_085B0A64;
        sub_0800b318(p, 1, 2, 19);
        p += 38;
        sub_0800b318(p, 1, 3, 19);
    }
}
void sub_080b98c0(void)
{
    s32 i;
    s32 j;
    s16 *e;
    s16 *pe;
    u8 *f;
    u16 v;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_0800b3a8();
        for (i = 0, j = 0; i < gUnk_030023AC; i++)
        {
            f = gUnk_02005E00.unk04;
            if (((gUnk_03002340 >> i) & 1) != 0)
            {
                pe = gUnk_02007D48;
                e = &pe[i];
                v = *e;
                if (*e > 0 && f[i] == 0)
                {
                    gUnk_02000010[i] = v;
                    sub_080b9968(i, j);
                    sub_080b99e8(i, j, *e);
                    j++;
                }
            }
        }
        sub_080b9878();
        sub_080b9a88(gUnk_03002490->unk18);
    }
}
void sub_080b9968(s32 a, s32 b)
{
    u8 *p;
    u8 *q;

    s32 off;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        off = a * 4;
        p = gUnk_085B0AB0;
        sub_0800b318(p + off, 3, b * 2 + 4, 2);
        p += 16;
        sub_0800b318(p + off, 3, b * 2 + 5, 2);
        q = gUnk_085B0AD0;
        sub_0800b318(q, 5, b * 2 + 4, 1);
        q += 2;
        sub_0800b318(q, 5, b * 2 + 5, 1);
    }
}
void sub_080b99e8(s32 a, s32 b, s32 c)
{
    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_08002f14(c);
        sub_0800b318(&gUnk_085B0AD4[gUnk_03001F08[1] * 2], 6, b * 2 + 4, 1);
        sub_0800b318(&gUnk_085B0AD4[20 + gUnk_03001F08[1] * 2], 6, b * 2 + 5, 1);
        sub_0800b318(&gUnk_085B0AD4[gUnk_03001F08[0] * 2], 7, b * 2 + 4, 1);
        sub_0800b318(&gUnk_085B0AD4[20 + gUnk_03001F08[0] * 2], 7, b * 2 + 5, 1);
    }
}
void sub_080b9a88(s32 a)
{
    s32 i;
    u8 *p;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        for (i = 0; i < gUnk_030023AC; i++)
        {
            sub_0800b34c(2, i * 2 + 4, 1);
            sub_0800b34c(2, i * 2 + 5, 1);
        }
        p = gUnk_085B0AFC;
        sub_0800b318(p, 2, a * 2 + 4, 1);
        p += 2;
        sub_0800b318(p, 2, a * 2 + 5, 1);
    }
}
void sub_080b9b08(s32 a)
{
    u8 *p;
    u8 *q;
    u8 *r;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_0800b3a8();
        p = gUnk_085B0B00;
        sub_0800b318(p, 13, 6, 2);
        p += 4;
        sub_0800b318(p, 13, 7, 2);
        a *= 4;
        q = gUnk_085B0BB4;
        sub_0800b318(q + a, 16, 8, 2);
        q += 16;
        sub_0800b318(q + a, 16, 9, 2);
        r = gUnk_085B0B10;
        sub_0800b318(r, 10, 8, 6);
        r += 12;
        sub_0800b318(r, 10, 9, 6);
    }
}
void sub_080b9b98(s32 a)
{
    u8 *p;
    u8 *q;
    u8 *r;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_0800b3a8();
        p = gUnk_085B0B08;
        sub_0800b318(p, 13, 6, 2);
        p += 4;
        sub_0800b318(p, 13, 7, 2);
        a *= 4;
        q = gUnk_085B0BB4;
        sub_0800b318(q + a, 19, 8, 2);
        q += 16;
        sub_0800b318(q + a, 19, 9, 2);
        r = gUnk_085B0B28;
        sub_0800b318(r, 6, 8, 13);
        r += 26;
        sub_0800b318(r, 6, 9, 13);
    }
}
void sub_080b9c28(void)
{
    u8 *p;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_0800b3a8();
        p = gUnk_085B0B5C;
        sub_0800b318(p, 4, 8, 22);
        p += 44;
        sub_0800b318(p, 4, 9, 22);
    }
}
void sub_080b9c74(void)
{
    u8 *p;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_0800b3a8();
        p = gUnk_085B0BD4;
        sub_0800b318(p, 4, 8, 22);
        p += 44;
        sub_0800b318(p, 4, 9, 22);
    }
}
void sub_080b9cc0(void)
{
    u8 *p;

    if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
    {
        sub_0800b3a8();
        p = gUnk_085B0C2C;
        sub_0800b318(p, 8, 8, 14);
        p += 28;
        sub_0800b318(p, 8, 9, 14);
    }
}
