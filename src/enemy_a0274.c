
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02006190[];
extern s32 gUnk_02007D00[];
extern s16 gUnk_03002158[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern vu16 gUnk_03004CA0[];
extern s16 gUnk_03005628[];
extern u32 gUnk_082D8638[];
extern u16 * gUnk_087482A8[];
extern s16 gUnk_08748374[];
extern struct AnimCmd gUnk_08748384[];
extern struct AnimCmd gUnk_08748398[];
extern s32 gUnk_087483B8[];
extern s32 gUnk_08748410[];
extern u16 gUnk_08748420[];
extern s16 gUnk_08748430[];
extern u32 gUnk_0874844C[];
extern u32 gUnk_08748478[];
extern u32 gUnk_087484C4[];
extern u32 gUnk_087484CC[];
extern u32 gUnk_08748820[];
extern u32 gUnk_087538E0[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern s32 sub_080031b8(s32 id);
extern void sub_080034f0(s32 player, s32 songId);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006214(void);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_080063ac(s16 a);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_08063a00(u32 v);
extern s32 sub_08063cd0(void);
extern void sub_08063d7c(void);
extern void sub_08063e14(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_0806415c(s32 n);
extern u8 sub_08064358(void);
extern u8 sub_08064398(void);
extern s32 sub_0806493c(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern void sub_08065438(void);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_080664e0(struct AnimCmd *p);
extern void sub_080666f8(struct AnimCmd *p);
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern u8 sub_080692fc(void);
extern void sub_0806d08c(s16 a, s16 b, s16 c);
extern s32 sub_0806d1e8(s16 a, s16 b);
extern void sub_0809fca4(void);
extern void sub_0809fd20(void);
extern void sub_080a0028(void);
extern void sub_080a0098(void);
extern void sub_080a00ec(void);

/* Defined below */
void sub_080a0a38(void);
void sub_080a0b10(void);
void sub_080a1550(void);

void sub_080a0274(void)
{
    sub_08064cdc(178, -24, -8, 0);
}

void sub_080a028c(void)
{
    struct ActorSpawn sp;
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    sp.unk00 = 11;
    sp.unk04 = 113;
    sp.unk08 = 0;
    sp.unk09 = 0;
    sp.unk0C = 32;
    sp.unk0E = 16;
    sp.unk10 = a->unk20;
    sp.unk0A = 0;
    gUnk_03002490->unk46 = sub_08064b5c(&sp, 1);
}

void sub_080a02d4(u8 a)
{
    struct Task *t;
    u16 x;
    u16 y;
    s16 yy;
    s32 d;

    t = gUnk_03002490;
    y = t->unk4A + 24;
    if (a == 1)
    {
        x = t->unk48 + 32;
        d = 1;
    }
    else
    {
        x = t->unk48 + (t->unk43 << 5);
        d = 0;
    }
    yy = y;
    sub_08067120((s16)x, yy, d, 0);
    if (a == 1)
        sub_08067120(gUnk_03002490->unk48 - 32, yy, -1, 0);
}

void sub_080a0358(void)
{
    register struct Task *t asm("r1");
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;
    s32 r;
    s32 lo;
    s32 hi;
    s32 lo2;
    s32 hi2;

    t = gUnk_03002490;
    switch (t->unk1C)
    {
    case 3:
        n = 46;
        break;
    case 7:
        r = sub_08064d34(182, 0);
        t = gUnk_03002490;
        t->unk46 = r;
        n = 52;
        break;
    case 8:
        sub_08063d7c();
        v = gUnk_03002490;
        v->unk20 = gUnk_030023B4;
        lo = gUnk_03005628[0] - 87;
        if (gUnk_030023B4 < lo)
            v->unk20 = lo;
        v = gUnk_03002490;
        hi = gUnk_03005628[1] + 87;
        if (v->unk20 > hi)
            v->unk20 = hi;
        if (gUnk_03002490->unk48 == gUnk_030023B4)
            goto tail;
        if ((u8)sub_08064398() == 4)
        {
            x = gUnk_03002490;
            x->unk20 -= 16;
        }
        else
        {
            x = gUnk_03002490;
            x->unk20 += 16;
        }
        t = gUnk_03002490;
        n = 8;
        break;
    default:
        t = gUnk_03002490;
        n = 0;
        break;
    }
    t->unk34 = n;
tail:
    y = gUnk_03002490;
    lo2 = gUnk_03005628[0] - 87;
    if (y->unk20 < lo2)
        y->unk20 = lo2;
    z = gUnk_03002490;
    hi2 = gUnk_03005628[1] + 87;
    if (z->unk20 > hi2)
        z->unk20 = hi2;
}

void sub_080a043c(void)
{
    struct Task *t;
    s32 n;
    s32 one;

    sub_0806493c();
    t = gUnk_03002490;
    one = 1;
    t->unk20 = one;
    if (gUnk_030023D4 <= 111)
    {
        t->unk24 = 2;
    }
    else
    {
        n = (t->unk30 + 1) & 3;
        t->unk30 = n;
        if (n == 0)
        {
            t->unk24 = one;
            t->unk20 = 4;
        }
        else
        {
            t->unk24 = 0;
        }
    }
}

void sub_080a0480(u8 a)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u16 f;

    if (a == 0)
    {
        if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 21)
            gUnk_03002490->unk24 = 0;
        else
            gUnk_03002490->unk24 = 1;
    }
    else
    {
        t = gUnk_03002490;
        f = t->unk43;
        sub_08063e14();
        u = gUnk_03002490;
        if ((s16)f != u->unk43)
            u->unk24 = 0;
        else
            u->unk24 = 1;
    }
    v = gUnk_03002490;
    if (v->unk24 == 1)
    {
        if (v->unk78 < (s16)v->unk70)
            sub_080061c0(136 << 10, 0x5A5A5A5A);
        else
            sub_080061c0(160 << 9, 0x5A5A5A5A);
    }
}

u8 sub_080a0538(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    switch (t->unk14)
    {
    case 3:
        sub_08006244();
        break;
    case 4:
        t->unk24 = 4;
        t->unk7A = 0;
        sub_0806d08c(24, 8, 32);
        sub_08006338(23);
        u = gUnk_03002490;
        sub_0800622c(-u->unk58, -u->unk60, u->unk68);
        break;
    }
    return 0;
}

void sub_080a0588(void)
{
    gUnk_03002490->unk54 = 0;
}

u8 sub_080a0598(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk14 == 4)
    {
        t->unk24 = 4;
        sub_08006338(23);
        u = gUnk_03002490;
        sub_0800622c(-u->unk58, -u->unk60, u->unk68);
    }
    return 0;
}

void sub_080a05c8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->unk46 = 0xFFFF;
    gUnk_02007D00[8] = -1;
    t->unk46 = -1;
    sub_08063e14();
    sub_0806395c(0);
}

u8 sub_080a060c(void)
{
    struct Task *t;
    s16 dx;
    u16 dy;
    s16 y;

    t = gUnk_03002490;
    dx = t->unk48 - gUnk_03002158[0];
    dy = t->unk4A - gUnk_03002158[2];
    if ((u16)(dx + 19) <= 278)
    {
        y = dy;
        if (y > -20)
        {
            if (y <= 179)
                return 1;
        }
    }
    return 0;
}

void sub_080a0658(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 i;
    s32 n;
    s32 n2;
    s32 n3;

    if (sub_080a060c() != 0)
    {
        i = sub_08002ee8(3);
        n = sub_080640dc(gUnk_08748384);
        t = gUnk_03002490;
        t->unk24 = n;
        if (t->unk78 < (s16)t->unk70)
        {
            i += 3;
            n2 = sub_080640dc(gUnk_08748398);
            gUnk_03002490->unk24 = n2;
        }
        u = gUnk_03002490;
        if (u->unk2C != 0)
            u->unk20 = 0;
        else
            u->unk20 = gUnk_08748374[i];
        gUnk_03002490->unk2C = 0;
    }
    else
    {
        n3 = sub_080640dc(gUnk_08748398);
        v = gUnk_03002490;
        v->unk24 = n3;
        v->unk20 = 1;
    }
}

void sub_080a06f0(void)
{
    struct Task *t;
    struct Actor *a;
    u16 *tab;
    u16 *p;
    s32 q;
    s32 i;
    s32 n;

    t = gUnk_03002490;
    a = t->unk8C;
    n = t->unk18 + 1;
    t->unk18 = n;
    if (n == 6)
    {
        t->unk18 = 0;
        sub_0806395c(4);
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
    else
    {
        tab = gUnk_087482A8[a->unk1C];
        i = sub_08002ee8(8);
        q = i * 4 + (s32)tab;
        sub_0806395c(*(u16 *)q);
        gUnk_03002490->unk1C = *(u16 *)(q + 2);
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0768(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    sub_08006338(37);
    TaskYieldTrampoline(24);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk3C++;
    sub_080031b8(0x21E);
    sub_0800622c(0xFFFB0000, 160 << 6, 160 << 11);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(136 << 2);
    sub_08006338(36);
    TaskYieldTrampoline(4);
    u = gUnk_03002490;
    u->unk3C--;
    TaskYieldTrampoline(1);
    v = gUnk_03002490;
    v->unk3C--;
    v->unk60 = 160 << 6;
    v->unk68 = 160 << 11;
    while ((gUnk_03002490->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(0x1F7);
    sub_080a02d4(0);
    sub_080261d4(4);
    TaskYieldTrampoline(20);
}

void sub_080a0844(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s32 z;

    i = (u16)(gUnk_03002490->unk24 * 3);
    a = gUnk_08748420[i];
    b = gUnk_08748420[i + 1];
    c = gUnk_08748420[i + 2];
    d = gUnk_08748420[i + 3];
    sub_08006338(39);
    TaskYieldTrampoline(24);
    sub_080031b8(136 << 2);
    sub_08006338(36);
    TaskYieldTrampoline(a);
    t = gUnk_03002490;
    t->unk3C--;
    z = 0;
    TaskYieldTrampoline(b);
    sub_080261d4(d);
    sub_080a02d4(0);
    u = gUnk_03002490;
    u->unk3C--;
    TaskYieldTrampoline(c);
    v = gUnk_03002490;
    v->unk20--;
    v->unk6C = z;
    while ((s16)gUnk_03002490->unk6C < gUnk_03002490->unk20)
    {
        w = gUnk_03002490;
        w->unk3C--;
        TaskYieldTrampoline(6);
        sub_080031b8(136 << 2);
        gUnk_03002490->unk3C = 36;
        TaskYieldTrampoline(a);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(b);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(c);
        gUnk_03002490->unk6C++;
    }
}

void sub_080a094c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 anim;

    t = gUnk_03002490;
    anim = (t->unk1C == 7) ? 30 : 13;
    while (1)
    {
        gUnk_03002490->unk7A = 0;
        TaskYieldTrampoline(1);
        sub_080061c0(128 << 9, 0x5A5A5A5A);
        gUnk_03002490->unk58 = 0;
        sub_080031b8(0x201);
        sub_08006338((s16)anim);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk7A = 0;
        u = gUnk_03002490;
        u->unk58 = 0xFFFF8000;
        u->unk3C++;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 128 << 8;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0;
        sub_080031b8(0x201);
        v = gUnk_03002490;
        v->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk7A = 0;
        w = gUnk_03002490;
        w->unk58 = 0xFFFF8000;
        w->unk3C++;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 128 << 8;
        TaskYieldTrampoline(10);
    }
}

void sub_080a0a0c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080a0a38;
    sub_080a05c8();
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0874844C);
}

void sub_080a0a38(void)
{
    if (sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 11, gUnk_08748478);
    if ((s16)gUnk_03004CA0[gCurTaskIdx] != -1)
        sub_0809fca4();
}

void sub_080a0a84(void)
{
    s32 n;
    s32 m;

    sub_0809fca4();
    sub_080663f4(gUnk_082D8638, 16);
    if (gUnk_02006190[5] <= 0)
    {
        n = gUnk_02006190[4];
        if (n <= 11)
        {
            gUnk_02006190[4] = n + 2;
            gUnk_02006190[5] = gUnk_08748430[n + 3];
        }
        else
        {
            gUnk_02006190[4] = 0;
            m = gUnk_02006190[3] - 1;
            gUnk_02006190[3] = m;
            gUnk_02006190[5] = gUnk_08748430[1];
            if (m <= 0)
            {
                sub_0809fd20();
                return;
            }
        }
    }
    gUnk_02006190[5]--;
    gUnk_03002490->unk48 += gUnk_08748430[gUnk_02006190[4]];
}

void sub_080a0b10(void)
{
    sub_08063e14();
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_0874844C);
}

void sub_080a0b30(void)
{
    struct Task *t;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = z;
    sub_080062c4();
    sub_080692fc();
    sub_080666f8(gUnk_08748384);
    sub_080664e0(gUnk_08748384);
    sub_08063a00((u32)gUnk_08748820);
    gUnk_03002490->unk2C = z;
    sub_080a0658();
    sub_08006138();
}

void sub_080a0b74(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gUnk_03002490;
    n = sub_0806415c(t->unk24);
    u = gUnk_03002490;
    u->unk24 = n;
    n = u->unk20 - 1;
    u->unk20 = n;
    if (n == 0)
    {
        u->unk1C = 3;
        sub_0806395c(2);
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0bb4(void)
{
    struct Task *t;
    s32 one;

    t = gUnk_03002490;
    one = 1;
    t->unk15 = one;
    sub_080062c4();
    gUnk_03002490->unk7A = one;
    sub_080a0658();
    sub_08006138();
}

void sub_080a0bdc(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gUnk_03002490;
    n = sub_0806415c(t->unk24);
    u = gUnk_03002490;
    u->unk24 = n;
    if (u->unk20 == 0)
        sub_080a06f0();
    else
        u->unk20--;
}

void sub_080a0c08(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    sub_080a0358();
    sub_080a094c();
}

void sub_080a0c28(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 v2;
    s32 w;
    s32 f;
    s32 f2;

    t = gUnk_03002490;
    if (t->unk1C == 8)
    {
        v = t->unk20 - t->unk48;
        f = t->unk43;
        if (v < 0)
        {
            if (f != -1)
                goto zero;
        }
        else if (f != 1)
        {
        zero:
            v = 0;
        }
        w = v;
    }
    else
    {
        v2 = sub_08063cd0();
        f2 = gUnk_03002490->unk43;
        if (v2 < 0)
        {
            if (f2 != -1)
                goto zero2;
        }
        else if (f2 != 1)
        {
        zero2:
            v2 = 0;
        }
        w = v2;
    }
    w = abs(w);
    u = gUnk_03002490;
    if (w <= u->unk34 && u->unk1C != 11)
    {
        u->unk7A = 1;
        sub_0806395c(gUnk_03002490->unk1C);
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0ccc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 z;
    s32 k;
    s32 r;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 3;
    sub_080062c4();
    gUnk_03002490->unk7A = z;
    sub_080061c0(128 << 10, 0x5A5A5A5A);
    sub_0800622c(0xFFFC0000, 128 << 8, 128 << 11);
    sub_08006338(17);
    while ((gUnk_03002490->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    sub_080031b8(0x1F7);
    sub_080261d4(4);
    r = sub_0806d1e8(24, 32);
    u = gUnk_03002490;
    u->unk46 = r;
    sub_080a0274();
    sub_080061c0(160 << 9, k = 0x5A5A5A5A);
    v = gUnk_03002490;
    v->unk3C++;
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 9, k);
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 8, k);
    w = gUnk_03002490;
    w->unk3C++;
    TaskYieldTrampoline(10);
    sub_080061c0(128 << 6, k);
    TaskYieldTrampoline(10);
    sub_080062c4();
    sub_0806395c(1);
    sub_08006138();
}

void sub_080a0dbc(void)
{
    struct Task *t;
    s16 *p;

    t = gUnk_03002490;
    if (t->unk14 != 3)
    {
        p = &t->unk46;
        if (*p != -1)
        {
            sub_08005654(*p);
            gUnk_03002490->unk46 = 0xFFFF;
        }
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a0e08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 4;
    sub_080062c4();
    u = gUnk_03002490;
    u->unk20 = -2;
    u->unk24 = -2;
    sub_08006338(25);
    TaskYieldTrampoline(20);
    gUnk_03002490->unk6C = z;
    do
    {
        sub_08006338(26);
        TaskYieldTrampoline(10);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(10);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    sub_080031b8(0x21F);
    gUnk_03002490->unk7A = 0;
    v = gUnk_03002490;
    v->unk3C++;
    for (i = 0; i <= 10; i += 2)
    {
        gUnk_03002490->unk58 = gUnk_087483B8[i];
        TaskYieldTrampoline(gUnk_087483B8[i + 1]);
    }
    sub_080062c4();
    sub_08006338(22);
    w = gUnk_03002490;
    w->unk68 = 192 << 9;
    w->unk64 = 192 << 9;
    w->unk20 = 150 << 1;
    while (1)
    {
        TaskYieldTrampoline(12);
        sub_08063e14();
    }
}

void sub_080a0ec8(void)
{
    struct Task *t;
    s32 m2;
    s32 n;
    s32 n2;

    t = gUnk_03002490;
    n = t->unk20;
    m2 = -2;
    if (n == m2)
        return;
    if (n == 0)
    {
        sub_0806395c(5);
        sub_08006148(sub_080a0b10, gCurTaskIdx);
        return;
    }
    t->unk20 = n - 1;
    n2 = t->unk24;
    if (n2 != m2)
    {
        t->unk24 = n2 - 1;
        if (n2 <= 0)
        {
            sub_08006338(22);
            gUnk_03002490->unk24 = m2;
        }
    }
    if ((u8)sub_08064358() == 2)
        gUnk_03002490->unk60 = 0xFFFFFB00;
    else
        gUnk_03002490->unk60 = 160 << 3;
    if ((u8)sub_08064398() == 8)
        gUnk_03002490->unk5C = 0xFFFFF800;
    else
        gUnk_03002490->unk5C = 128 << 4;
    sub_08006338(gUnk_03002490->unk3C);
}

void sub_080a0f7c(void)
{
    struct Task *t;
    s32 z;
    s32 i;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 5;
    sub_080062c4();
    gUnk_03002490->unk7A = z;
    sub_08006338(22);
    sub_08006338(29);
    sub_080031b8(0x21A);
    sub_080a028c();
    for (i = 0; i <= 3; i++)
    {
        sub_080061c0(gUnk_08748410[i], 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    }
    sub_08006214();
    sub_08006338(43);
    sub_0800622c(0, 148 << 6, 192 << 10);
    while ((gUnk_03002490->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(41);
    TaskYieldTrampoline(10);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080a1030(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080a0b10, gCurTaskIdx);
}

void sub_080a1058(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 6;
    sub_080062c4();
    sub_08006338(41);
    TaskYieldTrampoline(24);
    gUnk_03002490->unk7A = z;
    sub_080a0480(0);
    u = gUnk_03002490;
    u->unk3C++;
    sub_080031b8(0x21E);
    sub_0800622c(0xFFFB0000, 160 << 6, 160 << 11);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    v = gUnk_03002490;
    v->unk3C++;
    v->unk60 = 160 << 6;
    v->unk68 = 160 << 11;
    if (v->unk24 != 0)
        sub_080a0480(1);
    while ((gUnk_03002490->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_080031b8(252 << 1);
    sub_080261d4(2);
    sub_080a02d4(1);
    sub_08006338(41);
    TaskYieldTrampoline(34);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080a1140(void)
{
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_080a0b10, gCurTaskIdx);
}

void sub_080a1168(void)
{
    gUnk_03002490->unk15 = 7;
    sub_080062c4();
    sub_080a043c();
    if (gUnk_03002490->unk24 != 2)
        sub_080a0844();
    else
        sub_080a0768();
    sub_0806395c(1);
    sub_08006138();
}

void sub_080a11a0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk14 != 7)
    {
        sub_08005654(t->unk46);
        gUnk_03002490->unk46 = 0xFFFF;
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a11e0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s16 *p;
    s32 z;
    s32 z2;

    t = gUnk_03002490;
    z = 0;
    t->unk15 = 8;
    sub_080062c4();
    gUnk_03002490->unk20 = z;
    sub_08006338(24);
    TaskYieldTrampoline(8);
    u = gUnk_03002490;
    u->unk3C++;
    TaskYieldTrampoline(20);
    sub_080a0098();
    while (gUnk_02007D00[0] != 1)
    {
        sub_08006338(26);
        TaskYieldTrampoline(4);
        v = gUnk_03002490;
        v->unk3C++;
        TaskYieldTrampoline(6);
    }
    w = gUnk_03002490;
    z2 = 0;
    w->unk20 = z2;
    if (gUnk_02007D00[9] != -1)
    {
        sub_080034f0(gUnk_02007D00[9], 0x21B);
        gUnk_02007D00[9] = -1;
    }
    x = gUnk_03002490;
    p = &x->unk46;
    if (*p != -1)
    {
        sub_08005654(*p);
        gUnk_03002490->unk46 = 0xFFFF;
    }
    sub_080031b8(135 << 2);
    sub_08006338(28);
    gUnk_03002490->unk6C = z2;
    do
    {
        sub_080063ac(-2);
        TaskYieldTrampoline(2);
        sub_080063ac(2);
        TaskYieldTrampoline(2);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    sub_080063ac(0);
    TaskYieldTrampoline(14);
    sub_0806395c(9);
    sub_08006138();
}

void sub_080a12e0(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    if (gUnk_02006190[7] == 1)
    {
        sub_080663f4(gUnk_082D8638, 16);
        if (gUnk_02006190[5] <= 0)
        {
            gUnk_02006190[7] = 0;
            sub_08066468();
        }
        else
        {
            gUnk_02006190[5]--;
        }
    }
    t = gUnk_03002490;
    if (t->unk20 != 0)
    {
        if (gUnk_02007D00[8] != -1)
        {
            if (t->unk46 != -1)
            {
                sub_08005654(t->unk46);
                gUnk_03002490->unk46 = 0xFFFF;
            }
        }
        sub_080a00ec();
        n = gUnk_02007D00[0];
        if (n == -1)
        {
            u = gUnk_03002490;
            if (u->unk34 <= 0)
            {
                if (u->unk46 != -1)
                {
                    sub_08005654(u->unk46);
                    gUnk_03002490->unk46 = 0xFFFF;
                }
                if (gUnk_02007D00[9] != n)
                {
                    sub_080034f0(gUnk_02007D00[9], 0x21B);
                    gUnk_02007D00[9] = n;
                }
                sub_0806395c(1);
                sub_08006148(sub_080a0b10, gCurTaskIdx);
            }
            else
            {
                u->unk34--;
            }
        }
    }
    if (gUnk_03002490->unk14 != 8)
    {
        sub_08066468();
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a13d4(void)
{
    gUnk_03002490->unk15 = 9;
    sub_080062c4();
    sub_080a0028();
    TaskYieldTrampoline(51);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080a1400(void)
{
    if (gUnk_02006190[7] == 1)
    {
        sub_080663f4(gUnk_082D8638, 16);
        if (gUnk_02006190[5] <= 0)
        {
            gUnk_02006190[7] = 0;
            sub_08066468();
        }
        else
        {
            gUnk_02006190[5]--;
        }
    }
    if (gUnk_03002490->unk14 != 9)
    {
        if (gUnk_02006190[5] > 0)
        {
            gUnk_02006190[7] = 0;
            sub_08066468();
        }
        sub_08006148(sub_080a0b10, gCurTaskIdx);
    }
}

void sub_080a146c(void)
{
    struct Task *t;
    struct Actor *a;
    u8 f;

    t = gUnk_03002490;
    a = t->unk8C;
    t->unk15 = 10;
    sub_080062c4();
    sub_08006338(43);
    sub_0800622c(0, 148 << 6, 192 << 10);
    while ((gUnk_03002490->unk7A & 1) == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(41);
    TaskYieldTrampoline(10);
    f = a->unk1C;
    sub_0806395c(1);
    a->unk1C = (s8)f;
    sub_08006138();
}

void sub_080a14e4(void)
{
    if (gUnk_03002490->unk14 != 10)
        sub_08006148(sub_080a0b10, gCurTaskIdx);
}

void sub_080a150c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_08065438;
    t->unk04 = (u32)sub_080a1550;
    t->unk38 = gUnk_087538E0;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087484C4);
}

void sub_080a1550(void)
{
    sub_080692fc();
    sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087484CC);
}

void sub_080a1570(void)
{
    sub_08063e14();
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087484C4);
}
