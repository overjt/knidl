
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern u32 gUnk_02006040[];
extern s32 gUnk_02006190[];
extern s32 gUnk_02007D00[];
extern u8 gUnk_03001F30;
extern struct PlayerState gUnk_03002170[];
extern u8 gUnk_03002340;
extern s32 gUnk_030023B4;
extern u32 gUnk_030023E8[];
extern s16 gUnk_0300244C;
extern u32 gUnk_08748264[];
extern u32 gUnk_08748974[];
extern u32 gUnk_087538E0[];

/* Externals */
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern s32 sub_080031b8(s32 id);
extern void sub_080034f0(s32 player, s32 songId);
extern void sub_080055b0(u32 a, u32 b);
extern void sub_08005654(s32 id);
extern void sub_080059d8(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_08006338(s32 a);
extern void sub_0800a698(void);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(u32 *p);
extern void sub_08064970(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern void sub_08065438(void);
extern s16 sub_08065f5c(void);
extern u16 sub_08066088(u32 mode);
extern void sub_08066468(void);
extern void sub_080689c8(s32 i, s32 d);
extern void sub_08068f68(void);
extern u32 sub_08069b44(void);
extern void sub_0806a344(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern void sub_080a0a84(void);
extern void sub_080a0b10(void);
extern void sub_080b7c00(u32 a);

void sub_0809fbd0(void)
{
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        if (((gUnk_03002340 >> i) & 1) != 0)
            gUnk_03002170[i].unk42 |= 64;
    }
}

void sub_0809fc08(void)
{
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        if (((gUnk_03002340 >> i) & 1) != 0)
            gUnk_03002170[i].unk42 &= 0xFFBF;
    }
}

void sub_0809fc44(void)
{
    struct Task *t;
    struct Task *u;
    register s32 n asm("r2");
    register s32 v asm("r2");
    s32 w;
    t = gUnk_03002490;
    sub_08066088(0);
    n = sub_08065f5c();
    w = n * 5;
    v = (w + (w << 4)) >> 8;
    t = gUnk_03002490;
    t->unk70 = v;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08065438;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087538E0;
    sub_08002e98(u->unk73, 1, gUnk_08748264);
}

void sub_0809fca4(void)
{
    sub_08068f68();
    sub_08069b44();
}

void sub_0809fcb4(void)
{
    struct Task *t;
    s32 one;

    sub_080261d4(4);
    sub_080055b0(7, gCurTaskIdx);
    t = gUnk_03002490;
    t->unk08 = (u32)sub_080a0a84;
    gUnk_02006190[0] = t->unk48;
    gUnk_02006190[1] = t->unk4A;
    gUnk_02006190[2] = t->unk3C;
    one = 1;
    gUnk_02006190[7] = one;
    sub_08006338(8);
    gUnk_02006190[3] = one;
    gUnk_02006190[4] = -2;
    gUnk_02006190[5] = 0;
    sub_0806caa0(1, 0, 0);
}

void sub_0809fd20(void)
{
    struct Task *t;
    s32 z;

    sub_080055b0(0, gCurTaskIdx);
    t = gUnk_03002490;
    z = 0;
    t->unk08 = z;
    t->unk48 = gUnk_02006190[0];
    t->unk4A = gUnk_02006190[1];
    t->unk3C = gUnk_02006190[2];
    gUnk_02006190[7] = z;
    sub_08066468();
}

u8 sub_0809fd64(void)
{
    struct Task *t;
    s16 *p;

    sub_080639a4(gUnk_08748974);
    sub_08006338(9);
    t = gUnk_03002490;
    p = &t->unk46;
    if (*p != -1)
    {
        sub_08005654(*p);
        gUnk_03002490->unk46 = 0xFFFF;
    }
    if (gUnk_02007D00[8] != -1 && gUnk_02007D00[1] != -1)
    {
        sub_080689c8(gUnk_02007D00[8], -gUnk_03002490->unk43);
        gUnk_02007D00[8] = -1;
    }
    if (gUnk_02007D00[9] != -1)
    {
        sub_080034f0(gUnk_02007D00[9], 0x21B);
        gUnk_02007D00[9] = -1;
    }
    if (gUnk_03001F30 == 1)
    {
        sub_0800a698();
        sub_080b7c00(gUnk_030023E8[0]);
    }
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

u8 sub_0809fe10(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Actor *a;
    u8 s0;
    u8 res;

    t = gUnk_03002490;
    a = t->unk8C;
    s0 = t->unk14;
    sub_0809fcb4();
    switch (gUnk_03002490->unk14)
    {
    case 3:
        u = gUnk_03002490;
        if ((u->unk7A & 1) == 0)
        {
            gUnk_02006190[2] = 43;
            sub_0806395c(10);
            BLOCK_CROSS_JUMP
            goto install;
        }
        else
        {
            u->unk2C = 1;
            gUnk_02006190[2] = 4;
            sub_0806395c(1);
        }
        BLOCK_CROSS_JUMP
        goto install;
    case 4:
        v = gUnk_03002490;
        if ((v->unk7A & 1) == 0)
        {
            if (gUnk_0300244C != 0)
                gUnk_02006190[2] = 43;
            sub_0806395c(10);
            BLOCK_CROSS_JUMP
            goto install;
        }
        else
        {
            v->unk2C = 1;
            gUnk_02006190[2] = 4;
            sub_0806395c(1);
        }
        BLOCK_CROSS_JUMP
        goto install;
    case 7:
        w = gUnk_03002490;
        if (w->unk24 == 2)
        {
            if (gUnk_0300244C != 0)
                gUnk_02006190[2] = 43;
            sub_0806395c(10);
            BLOCK_CROSS_JUMP
            goto install;
        }
        else
        {
            w->unk2C = 1;
            gUnk_02006190[2] = 4;
            sub_0806395c(1);
        }
        BLOCK_CROSS_JUMP
        goto install;
    case 2:
        x = gUnk_03002490;
        x->unk2C = 1;
        gUnk_02006190[2] = 4;
        sub_0806395c(1);
        a->unk1C = gUnk_03002490->unk1C;
    install:
        sub_08006148(sub_080a0b10, gCurTaskIdx);
        break;
    case 8:
    case 9:
        sub_080055b0(0, gCurTaskIdx);
        y = gUnk_03002490;
        y->unk08 = 0;
        y->unk3C = gUnk_02006190[2];
        gUnk_02006190[5] = 14;
        break;
    case 0:
        gUnk_03002490->unk14 = 1;
    case 1:
        z = gUnk_03002490;
        z->unk2C = 1;
        gUnk_02006190[2] = 4;
        sub_0806395c(1);
        sub_08006148(sub_080a0b10, gCurTaskIdx);
        break;
    case 5:
    case 6:
        break;
    }
    if (s0 != gUnk_03002490->unk14)
    {
        res = 1;
        BLOCK_CROSS_JUMP
    }
    else
    {
        res = 0;
    }
    return res;
}

void sub_0809ffec(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk7C == 6 && t->unk82 == 4)
        gUnk_03002170[t->unk7E].unk01 = 18;
}

void sub_080a0028(void)
{
    struct ActorSpawn sp;

    sub_08064970();
    if (gUnk_030023B4 <= 127)
        gUnk_03002490->unk43 = 1;
    else
        gUnk_03002490->unk43 = -1;
    sub_08006338(29);
    sp.unk00 = 10;
    sp.unk04 = 112;
    sp.unk08 = 0;
    sp.unk09 = 0;
    sp.unk10 = 0;
    sp.unk0C = 32;
    sp.unk0E = 16;
    sp.unk0A = 0;
    sub_08064b5c(&sp, 1);
    sub_080031b8(0x21D);
}

void sub_080a0098(void)
{
    struct Task **tp;
    s32 *p;
    s32 *q;
    s32 *r;
    s32 z;

    tp = &gUnk_03002490;
    r = gUnk_02007D00;
    q = gUnk_02006040;
    z = 0;
    p = q + 9;
    do
    {
        *p = z;
        p--;
    } while ((s32)p >= (s32)q);
    (*tp)->unk20 = 1;
    (*tp)->unk34 = 90;
    r[0] = -1;
    (*tp)->unk46 = sub_08064cdc(183, 38, 10, 0);
    r[9] = sub_080031b8(0x21B);
}
