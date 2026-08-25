/* game_code_and_rodata 0x080653EC-0x080673EC (issue #65, module M17 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080653EC 0x080673EC src/actor_653ec.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s16 gUnk_03002348;
extern s16 gUnk_03002158[];
extern s8 gUnk_020061D8[];
extern struct Actor gUnk_0200C320[];
extern s8 gUnk_02007FB8[];
extern u32 gUnk_030012B0[];
extern u32 gUnk_02005E10[];
extern u32 gUnk_0873DF24[];
extern u8 gUnk_0825088C[];
extern u8 gUnk_03001470[];
extern u32 gUnk_0873DF38[][4];
extern u8 gUnk_0873DF78[];
extern u8 gUnk_082530C8[];
extern u32 gUnk_0873DF7C[][3];
extern u8 gUnk_0873DFAC[];
extern u32 *gUnk_0873EF74[];
extern u32 *gUnk_0873F118[];
extern u32 gUnk_0873F01C[];
extern u8 gUnk_0825CA44[];
extern vs16 gUnk_03004CA0[];
extern void sub_080031b8(u32 a);
extern u16 gUnk_02007D60;
extern u16 gUnk_02007FF0;
extern u32 *gUnk_0873F0C4[];
extern u32 *gUnk_0873F138[];
extern u32 *gUnk_0873F0E4[];
extern u32 *gUnk_0873F15C[];
extern u32 gUnk_0200AEF4;
extern u32 gUnk_02004C90;
extern u32 gUnk_02007D00[];
extern u32 gUnk_02006190[];
extern u32 gUnk_02006040[];
extern u8 gUnk_03002350;
extern void sub_080055b0(u32 a, u32 b);
extern void sub_080662d8(void);
extern u32 sub_080b5628(u32 a);
extern u32 sub_080b55d8(u32 a, u32 b);
extern s16 gUnk_0873E184[];
extern u16 gUnk_0873E16C[];
extern s16 gUnk_02005588[];
extern u16 gUnk_030023AC;
extern u8 gUnk_03002340;
extern void sub_08066468(void);
extern void sub_080664cc(struct GfxHeader *h);
extern void sub_08068f68(void);
extern void sub_08069b44(void);
extern void sub_08066480(struct GfxHeader *h, u32 b, u32 c);
extern u16 gUnk_03001EA4;
extern u8 gUnk_0200AFF8;
extern void sub_0800a280(void);
extern void sub_0800a340(s16 a);
extern void sub_0800a294(s16 a);
extern void sub_08066754(void);
extern void sub_080666a4(void);
void sub_08066988(u32 i);
u8 sub_08066a0c(s16 dx, u16 dy);
void sub_08066c08(u32 def, u8 b);
void sub_08066dcc(void);
extern void sub_080640c8(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern void sub_080639b4(u32 v);
extern u8 gUnk_0200D080;
extern s8 gUnk_0873E1B4[];
extern u16 gUnk_030023D8;
extern u32 sub_08026a0c(void);
extern u32 sub_08026a80(void);
extern u32 sub_08026aec(void);
extern s32 sub_08064188(s32 n);
extern s32 sub_0806415c(s32 n);
extern s32 sub_08064e90(u32 sub, u32 type, u8 p2, s16 x, s16 y);
extern void sub_08066a94(u8 a);
extern void sub_0806a008(void);
extern void sub_08006338(s32 a);
extern void sub_0806ae94(void);
extern void sub_080262dc(void);
extern void sub_08065350(void);
extern void sub_0806be4c(u32 i);
extern void sub_08006148(void *fn, u32 i);
extern void sub_0806a344(void);
extern void sub_08063f24(s32 i);
extern s32 gUnk_0873E1B8[];
extern void sub_08066c74(void);
extern void sub_08063908(u32 def);
extern void sub_08063ff4(void);
extern void sub_08063e14(void);
extern void sub_08006304(void);
extern void sub_0806523c(void);
extern void sub_08066e88(u8 a);
extern void sub_080637cc(void);
extern u32 gUnk_08752BA8[];
extern s16 gUnk_0873E1E8[];
extern s16 gUnk_0873E220[];
extern u8 gUnk_02006178;
extern void sub_0806ee2c(void);
extern s32 sub_08064fc4(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 sub_08063b38(void);
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern s16 gUnk_030023E4;

extern u32 sub_08005acc(void);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern void sub_0800a554(void);
extern void sub_08063fe0(void);
extern u8 sub_08065160(void);
extern void sub_080059a0(void);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08003014(u32 a, u32 b, u32 c, u32 d, u32 e);
extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);

void sub_08065470(void);
void sub_0806555c(void);
void sub_08065848(u32 p0, s32 idx);
s16 sub_08065f74(u32 i);
u32 *sub_0806601c(void);
void sub_0806627c(void);
void sub_08066544(void);
void sub_08066564(void);
void sub_080665a0(void);
void sub_0806685c(void *src, u32 size, u8 force);
u8 sub_08066a6c(void);
u8 sub_08066a80(void);
void sub_080663f4(void *src, u32 size);
u16 sub_0806660c(u16 a);
u32 sub_08066718(void);
void sub_08066658(struct AnimCmd *p);
void sub_080666a4(void);
void sub_08066988(u32 i);
u8 sub_08066a0c(s16 dx, u16 dy);
void sub_08066c08(u32 def, u8 b);
void sub_08066dcc(void);

void sub_080653ec(void)
{
    struct Task *p;

    p = gUnk_03002490;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    if (sub_08066a6c() != 0)
    {
        if (sub_08005acc() == 0)
            return;
        sub_08065470();
    }
    else if (sub_08066a80() != 0)
    {
        sub_0800a554();
        sub_08063fe0();
    }
}

void sub_08065438(void)
{
    struct Task *p;

    p = gUnk_03002490;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    if (sub_08066a6c() == 0)
        return;
    if (sub_08005acc() == 0)
        return;
    sub_08065470();
}

/* Push the running task's tile stream into OBJ VRAM, then draw it. */
void sub_08065470(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u32 dst;
    u32 prio;

    if (gUnk_03002490->unk38 == NULL)
        return;
    if (gUnk_03002490->unk3C == -1)
        return;
    prio = gUnk_03002490->unk40;
    dst = ((prio & 0x7FF) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)gUnk_03002490->unk38[gUnk_03002490->unk3C];
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
    if ((*(u16 *)&gUnk_03002490->unk8C->unk0A & 0x101) == 0)
        sub_0806685c(g->unk04 + 1, *g->unk04, 0);
    t = gUnk_03002490;
    sub_08001a94(t->unk42, g->unk00, t->unk3E, 0x800 | t->unk40,
                 t->unk48 - gUnk_03002348,
                 (s16)(t->unk4A - gUnk_030023E4));
}

/* Draw the running task's main sprite plus its optional second part. */
void sub_0806555c(void)
{
    struct Task *t;
    struct Actor *a;
    u32 *tbl;

    t = gUnk_03002490;
    a = t->unk8C;
    tbl = t->unk38;
    sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40,
                 t->unk48 - gUnk_03002348,
                 (s16)(t->unk4A - gUnk_030023E4));
    if (a->unk1A != -1)
    {
        t = gUnk_03002490;
        sub_08001a94(a->unk03 + t->unk42, tbl[a->unk1A], t->unk3E, a->unk1E,
                     t->unk48 - gUnk_03002348 + a->unk16,
                     (s16)(t->unk4A - gUnk_030023E4 + a->unk18));
    }
}

void sub_08065640(void)
{
    struct Task *p;

    p = gUnk_03002490;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    if (sub_08065160() != 0)
    {
        if (sub_08005acc() == 0)
            return;
        sub_0806555c();
    }
    else
    {
        sub_08063fe0();
    }
}

void sub_0806567c(void)
{
    struct Task *p;

    p = gUnk_03002490;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    if (sub_08065160() == 0)
        return;
    if (sub_08005acc() == 0)
        return;
    sub_0806555c();
}

/* Integrate the running task's velocity, clamped to its per-axis maximum. */
void sub_080656b4(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 m;
    s32 n;

    t = gUnk_03002490;
    v = t->unk54 + t->unk5C;
    t->unk54 = v;
    t->unk58 = t->unk58 + t->unk60;
    m = t->unk64;
    if (m != 0x80000000)
    {
        if (v > 0)
        {
            if (v > m)
                t->unk54 = m;
        }
        else
        {
            if (v < -m)
                t->unk54 = -m;
        }
    }
    u = gUnk_03002490;
    n = u->unk68;
    if (n != 0x80000000)
    {
        if (u->unk58 > 0 && u->unk58 > n)
            u->unk58 = n;
    }
    w = gUnk_03002490;
    w->unk4C = w->unk4C + w->unk54;
    w->unk50 = w->unk50 + w->unk58;
    w->unk48 = w->unk4C >> 16;
    w->unk4A = w->unk50 >> 16;
}

void sub_0806572c(void)
{
    struct Task *t;

    sub_080059a0();
    t = gUnk_03002490;
    t->unk48 = (t->unk4C >> 16) + gUnk_03002158[0];
    t->unk4A = (t->unk50 >> 16) + gUnk_03002158[2];
}

void sub_08065760(u32 i, u32 p1, u8 p2)
{
    struct Task *t;
    struct Actor *a;
    s8 j;

    j = gUnk_020061D8[i];
    if (j != -1)
    {
        t = &gUnk_03002790[j];
        t->unk8C = a = &gUnk_0200C320[j];
        a->unk28 = p1;
        a->unk0C = p2;
    }
}

void sub_080657a4(void)
{
    struct Task *t;
    s32 j;

    j = gUnk_020061D8[2];
    if (j != -1)
    {
        t = &gUnk_03002790[j];
        t->unk1C = 2;
    }
}

void sub_080657cc(u32 v)
{
    struct Task *t;
    s32 j;

    j = gUnk_020061D8[2];
    if (j != -1)
    {
        t = &gUnk_03002790[j];
        t->unk18 = v;
        t->unk1C = 1;
    }
}

void sub_080657f8(u32 a, u32 b)
{
    struct Task *t;
    s32 j;

    sub_08065848(4, 2);
    j = gUnk_020061D8[2];
    if (j != -1)
    {
        t = &gUnk_03002790[j];
        t->unk18 = a;
        t->unk1C = 0;
        t->unk24 = b;
        CpuSet(gUnk_030012B0, gUnk_02005E10, 224);
    }
}

/* Reference-counted spawn of the slot-`idx` helper task. */
void sub_08065848(u32 p0, s32 idx)
{
    struct Task *t;
    s32 i;

    if (idx > 2)
        return;
    if (gUnk_02007FB8[idx] == 0)
    {
        i = sub_08064d34(174, 1);
        if (i != -1)
        {
            t = &gUnk_03002790[i];
            t->unk18 = gUnk_03002490->unk40 >> 12;
            t->unk73 = p0;
            t->unk74 = idx;
            gUnk_020061D8[idx] = i;
        }
    }
    gUnk_02007FB8[idx]++;
}

void sub_080658b8(void)
{
    sub_08002e98(gUnk_03002490->unk73, 5, gUnk_0873DF24);
}

/* Task body: cross-fade two palettes while the helper's refcount holds. */
/* Task body: cross-fade two palettes while the helper's refcount holds. */
/* Task body: cross-fade two palettes while the helper's refcount holds. */
void sub_080658d8(void)
{
    struct Task *w;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;

    gUnk_03002490->unk28 = 12;
    gUnk_03002490->unk2C = 2;
    gUnk_03002490->unk30 = 3;
    gUnk_03002490->unk34 = 0;
    while (gUnk_02007FB8[gUnk_03002490->unk74] != 0)
    {
        u = gUnk_03002490;
        u->unk28--;
        if (u->unk28 == 0)
        {
            u->unk28 = 12;
            u->unk2C++;
            if (u->unk2C > 3)
                u->unk2C = 0;
            v = gUnk_03002490;
            v->unk30++;
            if (v->unk30 > 3)
                v->unk30 = 0;
            gUnk_03002490->unk34 = 0;
        }
        w = gUnk_03002490;
        if (w->unk30 == 0)
            w->unk34 = w->unk34 + 128;
        else
            w->unk34 = w->unk34 + 64;
        x = gUnk_03002490;
        if (x->unk34 > 256)
            x->unk34 = 256;
        y = gUnk_03002490;
        sub_08003014((u32)(gUnk_0825088C + (y->unk2C << 5)),
                     (u32)(gUnk_0825088C + (y->unk30 << 5)), (u16)y->unk34,
                     16, (u32)(gUnk_03001470 + (y->unk18 << 5)));
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

/* Task body: cycle the actor's 32-byte palette out of gUnk_0873DF38. */
/* Task body: cycle the actor's 32-byte palette out of gUnk_0873DF38. */
void sub_080659b4(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *x;
    struct Actor *a;
    u32 v;

    t = gUnk_03002490;
    a = t->unk8C;
    t->unk18 = 10;
    t->unk1C = 0;
    while (gUnk_02007FB8[gUnk_03002490->unk74] != 0 && a->unk0C <= 3)
    {
        u = gUnk_03002490;
        if (u->unk18 <= 0)
        {
            v = gUnk_0873DF38[a->unk0C][u->unk1C];
            if (v == 0)
                v = a->unk28;
            sub_080017e4(2, v,
                         (u32)(gUnk_03001470 + ((u->unk40 >> 12) << 5)), 32);
            w = gUnk_03002490;
            w->unk18 = 10;
            w->unk1C = (w->unk1C + 1) & 3;
        }
        x = gUnk_03002490;
        x->unk18--;
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

/* Task body: flash one palette entry on and off. */
void sub_08065a68(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *x;
    u32 off;

    t = gUnk_03002490;
    t->unk18 = 2;
    t->unk1C = 0;
    off = (t->unk40 >> 12) << 5;
    off += 26;
    while (gUnk_02007FB8[gUnk_03002490->unk74] != 0)
    {
        u = gUnk_03002490;
        if (u->unk18 <= 0)
        {
            if (u->unk1C != 0)
                sub_080017e4(2, (u32)gUnk_0873DF78,
                             (u32)(gUnk_03001470 + off), 2);
            else
                sub_080017e4(2, (u32)gUnk_082530C8,
                             (u32)(gUnk_03001470 + off), 2);
            w = gUnk_03002490;
            w->unk18 = 2;
            w->unk1C ^= 1;
        }
        x = gUnk_03002490;
        x->unk18--;
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

/* Task body: cross-fade the actor's palette pair out of gUnk_0873DF7C. */
void sub_08065b14(void)
{
    struct Task *w;
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    struct Task *y;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    t->unk28 = 10;
    t->unk2C = 1;
    t->unk30 = 2;
    t->unk34 = 0;
    while (gUnk_02007FB8[gUnk_03002490->unk74] != 0)
    {
        u = gUnk_03002490;
        u->unk28--;
        if (u->unk28 == 0)
        {
            u->unk28 = 10;
            u->unk2C++;
            if (u->unk2C > 2)
            {
                u->unk2C = 0;
                u->unk28 = 2;
            }
            v = gUnk_03002490;
            v->unk30++;
            if (v->unk30 > 2)
                v->unk30 = 0;
            gUnk_03002490->unk34 = 0;
        }
        w = gUnk_03002490;
        if (w->unk30 == 0)
            w->unk34 = w->unk34 + 16;
        else
            w->unk34 = w->unk34 + 128;
        x = gUnk_03002490;
        if (x->unk34 > 256)
            x->unk34 = 256;
        y = gUnk_03002490;
        sub_08003014(gUnk_0873DF7C[a->unk0C][y->unk2C],
                     gUnk_0873DF7C[a->unk0C][y->unk30], (u16)y->unk34, 16,
                     (u32)(gUnk_03001470 + (y->unk18 << 5)));
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

/* Task body: fade the shared palette in, out, or straight to zero. */
void sub_08065c14(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk20 = 0;
    while (gUnk_02007FB8[gUnk_03002490->unk74] != 0)
    {
        t = gUnk_03002490;
        switch (t->unk1C)
        {
        case 0:
            t->unk20 += t->unk18;
            if (t->unk20 > t->unk24)
            {
                t->unk20 = t->unk24;
                t->unk1C = 3;
            }
            u = gUnk_03002490;
            sub_08003014((u32)gUnk_02005E10, (u32)gUnk_0873DFAC,
                         (u16)u->unk20, 224, (u32)gUnk_030012B0);
            break;
        case 1:
            t->unk20 -= t->unk18;
            if (t->unk20 < 0)
            {
                t->unk20 = 0;
                t->unk1C = 3;
            }
            u = gUnk_03002490;
            sub_08003014((u32)gUnk_02005E10, (u32)gUnk_0873DFAC,
                         (u16)u->unk20, 224, (u32)gUnk_030012B0);
            break;
        case 2:
            sub_08003014((u32)gUnk_02005E10, (u32)gUnk_0873DFAC, 0, 224,
                         (u32)gUnk_030012B0);
            gUnk_03002490->unk1C = 3;
            break;
        }
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

/* Point the actor at the palette its class/sub/level combination wants. */
void sub_08065ce0(u32 i)
{
    struct Task *t;
    struct Actor *a;
    u32 *tbl;
    u32 v;
    u32 k;
    u32 *q;

    t = &gUnk_03002790[i];
    a = t->unk8C;
    switch (t->unk72)
    {
    case 0:
        tbl = gUnk_0873EF74[t->unk76];
        break;
    case 1:
        tbl = gUnk_0873F118[t->unk76];
        break;
    default:
        tbl = NULL;
        break;
    }
    if (a->unk0C > 3 || tbl == NULL)
    {
        a->unk0C = 0;
    }
    else
    {
        k = a->unk0C - 1;
        q = tbl + k;
        v = *q;
        if (v != 0)
            a->unk28 = v;
    }
}

/* Copy one palette bank out of the class/sub/level palette tables.
   `sub` is reused as the table pointer, matching the ROM's register use. */
/* Copy one palette bank out of the class/sub/level palette tables.  `sub`
   doubles as the table pointer and `src` as the source address, matching
   the ROM's register use. */
/* Copy one palette bank out of the class/sub/level palette tables.
   `sub` is reused as the table pointer, matching the ROM's register use. */
void sub_08065d44(u32 slot, u32 sub, u32 level, u32 n, u8 kind, u32 src)
{
    u32 off;
    u32 k;
    u32 *q;
    u32 p;

    switch (kind)
    {
    case 0:
        sub = (u32)gUnk_0873EF74[sub];
        break;
    case 1:
        sub = (u32)gUnk_0873F118[sub];
        break;
    default:
        sub = 0;
        break;
    }
    if (level > 3)
        return;
    if (sub == 0)
        return;
    if (kind == 1 && level == 0)
    {
        p = src;
    }
    else
    {
        k = level - 1;
        q = (u32 *)sub + k;
        p = *q;
    }
    if (p == 0)
        return;
    off = ((u32 *)sub)[4] << 1;
    p += off;
    slot <<= 5;
    slot += off;
    if (kind == 0)
        n = ((u32 *)sub)[5];
    sub_080017e4(2, p, (u32)(gUnk_03001470 + slot), n << 1);
}

void sub_08065dbc(u32 slot, u32 sub, u32 level)
{
    sub_08065d44(slot, sub, level, 0, 0, 0);
}

void sub_08065dd0(u32 slot, u32 i)
{
    u32 p;

    p = gUnk_0873F01C[i];
    if (p != 0)
        sub_080017e4(2, p, (u32)(gUnk_03001470 + (slot << 5)), 32);
}

void sub_08065dfc(u32 slot)
{
    slot <<= 5;
    sub_080017e4(2, (u32)gUnk_0825CA44, (u32)(gUnk_03001470 + slot), 32);
}

void sub_08065e1c(u32 def, u32 which)
{
    struct Actor *a;
    s32 v;

    a = gUnk_03002490->unk8C;
    if (which != 0)
    {
        v = a->unk34;
        if (v == -2)
            return;
        if (v == -1)
            sub_080031b8(def);
        else
            sub_080031b8(v);
    }
    else
    {
        v = a->unk38;
        if (v == -2)
            return;
        if (v == -1)
            sub_080031b8(def);
        else
            sub_080031b8(v);
    }
}

/* Set bit 0 of every live enemy actor's two flag bytes. */
void sub_08065e6c(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;

    for (i = 32; i < 63; i++)
    {
        if (gUnk_03004CA0[i] != -1 && gUnk_03004CA0[i] != 4)
        {
            t = &gUnk_03002790[i];
            a = t->unk8C;
            if (a != NULL && (u8)(t->unk72 - 7) > 3)
            {
                a->unk0A |= 1;
                a->unk0B |= 1;
            }
        }
    }
}

void sub_08065ed0(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;

    for (i = 32; i < 63; i++)
    {
        if (gUnk_03004CA0[i] != -1 && gUnk_03004CA0[i] != 4)
        {
            t = &gUnk_03002790[i];
            a = t->unk8C;
            if (a != NULL && (u8)(t->unk72 - 7) > 3)
            {
                a->unk0A = 0;
                a->unk0B = 0;
            }
        }
    }
}

u8 sub_08065f2c(u32 i)
{
    struct Task *g;
    struct Task *a;
    struct Task *b;

    g = gUnk_03002790;
    a = &g[gCurTaskIdx];
    b = &g[i];
    if (a->unk16 == b->unk16)
        return 1;
    return 0;
}

s16 sub_08065f5c(void)
{
    return sub_08065f74(gCurTaskIdx);
}

/* Horizontal draw offset of task `i`'s sprite for the current view mode. */
/* Horizontal draw offset of task `i`'s sprite for the current view mode. */
s16 sub_08065f74(u32 i)
{
    struct Task *t;
    struct Actor *a;
    s16 w;
    s16 adj;
    s16 r;
    s32 m;

    t = &gUnk_03002790[i];
    a = t->unk8C;
    switch (gUnk_03002350)
    {
    case 2:
        w = a->unk44->unk02;
        break;
    case 3:
        w = a->unk44->unk04;
        break;
    case 4:
        w = a->unk44->unk06;
        break;
    case 1:
    default:
        w = a->unk44->unk00;
        break;
    }
    switch (t->unk72)
    {
    case 1:
        m = gUnk_02007D60 & 15;
        if (w > 30)
            adj = m << 2;
        else
            adj = m << 1;
        break;
    case 2:
        m = gUnk_02007FF0 & 15;
        if (w > 30)
            adj = m << 2;
        else
            adj = m << 1;
        break;
    default:
        adj = 0;
        break;
    }
    r = w - adj;
    return r + a->unk02;
}

u32 *sub_0806601c(void)
{
    struct Task *t;
    struct Actor *a;
    u32 *r;
    u32 *tbl;
    u32 v;
    u32 k;
    u32 *q;

    t = gUnk_03002490;
    a = t->unk8C;
    if (t->unk72 == 1)
    {
        r = gUnk_0873F0C4[t->unk76];
        if (a->unk0C != 0)
        {
            tbl = gUnk_0873F118[t->unk76];
            if (tbl != NULL)
            {
                k = a->unk0C - 1;
                q = tbl + k;
                v = *q;
                if (v != 0)
                    a->unk28 = v;
            }
        }
    }
    else
    {
        r = gUnk_0873F138[t->unk76];
    }
    return r;
}

u16 sub_08066088(u32 mode)
{
    struct Task *t;
    struct Actor *a;
    u32 *p;
    u32 *tbl;
    struct GfxHeader *h;
    u16 prio;
    u32 v;
    s32 sh;
    u32 lo;
    u32 w40;

    a = gUnk_03002490->unk8C;
    a->unk20 = gUnk_03002490->unk40;
    prio = gUnk_03002490->unk40;
    p = sub_0806601c();
    t = gUnk_03002490;
    if (t->unk72 == 1)
        a->unk64.unk00 = (struct GfxHeader *)gUnk_0873F0E4[t->unk76];
    else
        a->unk64.unk00 = (struct GfxHeader *)gUnk_0873F15C[t->unk76];
    a->unk64.unk04 = gUnk_03002490->unk40 & 0xFFF;
    a->unk64.unk08 = gUnk_03002490->unk40 >> 12;
    if (p != NULL)
    {
        if (mode == 1)
        {
            sh = sub_080b5628(p[0] << 4);
            lo = a->unk64.unk08;
        }
        else
        {
            sh = sub_080b55d8(p[0] << 4, p[1]);
            lo = sh & 0xFFFF;
            sh = sh >> 16;
        }
        w40 = (lo << 12) | ((sh << 1) + 16);
        gUnk_03002490->unk40 = w40;
    }
    else
    {
        sub_0806685c(a->unk64.unk00->unk08, a->unk64.unk00->unk00 << 5, 0);
    }
    return prio;
}

void sub_08066144(void)
{
    s32 i;

    for (i = 0; i < 10; i++)
        gUnk_02007D00[i] = 0;
    for (i = 0; i < 8; i++)
        gUnk_02006190[i] = 0;
    for (i = 0; i < 10; i++)
        gUnk_02006040[i] = 0;
    gUnk_0200AEF4 = gUnk_02004C90 = 0;
}

void sub_0806619c(u32 p0, u32 p1, u32 p2, u16 p3, u8 p4)
{
    struct Task *t;

    sub_080055b0(7, gCurTaskIdx);
    t = gUnk_03002490;
    t->unk08 = (u32)sub_080662d8;
    gUnk_02006190[0] = t->unk48;
    gUnk_02006190[1] = t->unk4A;
    gUnk_02006190[2] = t->unk3C;
    gUnk_02004C90 = p2;
    gUnk_02006190[4] = p3;
    gUnk_02006190[3] = p0;
    gUnk_0200AEF4 = p1;
    gUnk_02006190[5] = p4;
}

void sub_0806621c(void)
{
    struct Task *t;
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    sub_080055b0(0, gCurTaskIdx);
    t = gUnk_03002490;
    t->unk08 = 0;
    t->unk48 = gUnk_02006190[0];
    t->unk4A = gUnk_02006190[1];
    t->unk3C = gUnk_02006190[2];
    if (gUnk_02004C90 != 0)
    {
        if (gUnk_02006190[5] != 0)
            sub_08066468();
        else
            sub_080664cc(a->unk64.unk00);
    }
}

/* Walk one step of the queued knock-back path. */
void sub_0806627c(void)
{
    struct Task *t;
    s32 i;
    s16 j;

    i = gUnk_02006190[3];
    if (i < 0)
        return;
    j = gUnk_0873E184[i] * 2;
    t = gUnk_03002490;
    t->unk48 += gUnk_0873E16C[j];
    t->unk4A += gUnk_0873E16C[j + 1];
    gUnk_02006190[3]--;
}

void sub_080662d8(void)
{
    struct Actor *a;
    u32 v;

    a = gUnk_03002490->unk8C;
    sub_08068f68();
    sub_08069b44();
    if (gUnk_02004C90 != 0)
    {
        v = gUnk_02006190[5];
        switch (v)
        {
        case 1:
            sub_080663f4((void *)gUnk_02004C90, gUnk_02006190[4]);
            break;
        case 0:
            sub_08066480(a->unk64.unk00, gUnk_02004C90, gUnk_02006190[4]);
            break;
        }
    }
    sub_0806627c();
    if (gUnk_0200AEF4 != 0)
        ((void (*)(void))gUnk_0200AEF4)();
}

/* True when every active player is in state 1. */
u8 sub_08066338(void)
{
    s32 n;
    s32 m;
    s32 i;

    n = 0;
    m = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            n++;
            if (gUnk_03002790[i].unk7A == 1)
                m++;
        }
    }
    if (n != 0 && n == m)
        return 1;
    return 0;
}

s32 sub_08066394(void)
{
    s32 i;
    s32 v;

    i = 0;
    v = 0;
    for (; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            v = gUnk_02005588[i];
            if (v != 0)
                break;
        }
    }
    return v;
}

void sub_080663f4(void *src, u32 size)
{
    struct Task *t;
    struct Actor *a;
    struct TaskGfx *g;
    u32 *tbl;

    t = gUnk_03002490;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    a = t->unk8C;
    if (a->unk0B & 1)
        return;
    a->unk0A |= 1;
    t = gUnk_03002490;
    tbl = t->unk38;
    g = (struct TaskGfx *)tbl[t->unk3C];
    if ((gUnk_03001EA4 & 2) == 0)
        sub_0806685c(g->unk04 + 1, *g->unk04, 0);
    else
        sub_0806685c(src, size << 1, 1);
}

void sub_08066468(void)
{
    gUnk_03002490->unk8C->unk0A &= 254;
}

void sub_08066480(struct GfxHeader *h, u32 src, u32 size)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    if (a->unk0B & 1)
        return;
    if ((gUnk_03001EA4 & 2) == 0)
        sub_0806685c(h->unk08, h->unk00 << 5, 0);
    else
        sub_0806685c((void *)src, size << 1, 1);
}

void sub_080664cc(struct GfxHeader *h)
{
    sub_0806685c(h->unk08, h->unk00 << 5, 0);
}

/* Task body: hand the actor over to the "carried" routine at 0x08066754. */
void sub_080664e0(struct AnimCmd *p)
{
    struct Task *t;

    sub_08066544();
    sub_080055b0(8, gCurTaskIdx);
    t = gUnk_03002490;
    t->unk20 = t->unk04;
    t->unk04 = (u32)sub_08066754;
    sub_080640c8();
    if (p != NULL)
        gUnk_03002490->unk24 = sub_080640dc(p);
    if (gUnk_0200AFF8 == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_0200AFF8 == 0);
    }
    sub_08066564();
    sub_080666a4();
}

void sub_08066544(void)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    sub_080639b4(((u32 *)a->unk60)[1]);
    sub_080665a0();
}

void sub_08066564(void)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    sub_080639b4(a->unk44->unk14);
}

void sub_08066580(void)
{
    struct Actor *a;

    a = gUnk_03002490->unk8C;
    sub_080665a0();
    sub_080639b4(a->unk44->unk14);
}

void sub_080665a0(void)
{
    struct Task **g;
    struct Task *t;
    s16 v;
    s16 w;

    g = &gUnk_03002490;
    gUnk_0200AFF8 = 0;
    sub_0800a280();
    t = *g;
    if (t->unk72 == 1 || (t->unk72 == 2 && t->unk76 == 7))
    {
        v = t->unk78;
        sub_0800a340(v);
    }
    else
    {
        w = gUnk_03002490->unk78;
        sub_0800a294(w);
    }
}

u16 sub_080665fc(void)
{
    return sub_0806660c(0);
}

u16 sub_0806660c(u16 a)
{
    struct Actor *p;

    p = gUnk_03002490->unk8C;
    return ((a + p->unk64.unk08) << 12) | p->unk64.unk04;
}

u16 sub_08066630(u16 a)
{
    struct Task *t;
    u16 v;
    u16 w;

    t = gUnk_03002490;
    v = t->unk40;
    w = a + (v >> 12);
    return (w << 12) | (v & 0xFFF);
}

void sub_08066658(struct AnimCmd *p)
{
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    t->unk20 = t->unk04;
    t->unk04 = (u32)sub_08066754;
    sub_080640c8();
    if (p != NULL)
        gUnk_03002490->unk24 = sub_080640dc(p);
    sub_080055b0(8, gCurTaskIdx);
    sub_080639b4(((u32 *)a->unk60)[1]);
}

void sub_080666a4(void)
{
    struct Task *t;

    sub_080055b0(0, gCurTaskIdx);
    t = gUnk_03002490;
    t->unk04 = t->unk20;
    t->unk24 = 0;
    t->unk20 = 0;
}

void sub_080666cc(struct AnimCmd *p)
{
    sub_08066658(p);
    if (gUnk_0200D080 == 0)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_0200D080 == 0);
    }
    sub_080666a4();
}

void sub_080666f8(struct AnimCmd *p)
{
    sub_08066658(p);
    while (sub_08066718() == 0)
        TaskYieldTrampoline(1);
    sub_080666a4();
}

u32 sub_08066718(void)
{
    switch (gUnk_03002490->unk76)
    {
    case 5:
        return sub_08026a0c();
    case 6:
        return sub_08026a80();
    case 0:
        return sub_08026aec();
    }
    return 0;
}

void sub_08066754(void)
{
    struct Task **g;
    struct Task *t;
    s32 v;

    g = &gUnk_03002490;
    t = *g;
    if (t->unk72 == 2 && t->unk76 == 5)
        v = sub_08064188(t->unk24);
    else
        v = sub_0806415c((*g)->unk24);
    (*g)->unk24 = v;
    sub_08068f68();
    sub_08069b44();
}

void sub_08066798(void)
{
    if (gUnk_030023D8 == 19)
        sub_08064e90(2, 70, 0, 128, 0);
}

void sub_080667c0(u8 a, u16 b)
{
    struct Task *t;
    struct Actor *p;

    p = gUnk_03002490->unk8C;
    sub_0800a554();
    t = gUnk_03002490;
    if (t->unk0C == (u32)sub_08065438 || t->unk0C == (u32)sub_080653ec)
        t->unk0C = (u32)sub_080653ec;
    else
        t->unk0C = (u32)sub_08065350;
    gUnk_03002490->unk78 += gUnk_0873E1B4[gUnk_03002350 - 1];
    p->unk05 = 2;
    p->unk30 = 0;
    sub_08066a94(a);
    sub_0806a008();
    sub_08006338((s16)b);
    sub_0806ae94();
}

void sub_0806684c(void)
{
    sub_080262dc();
    sub_08066798();
}

void sub_0806685c(void *src, u32 size, u8 force)
{
    struct Task *t;
    struct Actor *a;
    u32 slot;

    t = gUnk_03002490;
    a = t->unk8C;
    slot = t->unk40 >> 12;
    if (force == 0 && t->unk72 == 1 && a->unk0C != 0 && a->unk28 != 0)
        sub_080017e4(2, a->unk28, (u32)(gUnk_03001470 + (slot << 5)), size);
    else
        sub_080017e4(2, (u32)src, (u32)(gUnk_03001470 + (slot << 5)), size);
    a->unk24 = size >> 1;
}

/* Retire every other live task the running one is allowed to clean up. */
void sub_080668c8(void)
{
    struct Task *t;
    struct Actor *a;
    s32 i;
    u32 cls;

    a = gUnk_03002490->unk8C;
    for (i = 32; i <= 62; i++)
    {
        if (gUnk_03004CA0[i] == -1)
            continue;
        if (i == gCurTaskIdx)
            continue;
        if (gUnk_03004CA0[i] == 4)
            continue;
        t = &gUnk_03002790[i];
        if (gUnk_03002490->unk72 == 2 && a->unk3C != 0)
        {
            if ((u8)((u8 (*)(s32))a->unk3C)(i) != 1)
                continue;
        }
        cls = t->unk72;
        if (cls == 6 && t->unk76 == 0)
            continue;
        if (cls == 10)
            continue;
        if (cls == 9 || cls == 7 || cls == 8)
        {
            if (gUnk_03004CA0[i] == 174)
                continue;
            sub_08063f24(i);
        }
        else
        {
            sub_08066988(i);
        }
    }
}

/* Park task `i`: reset it to the idle body or kill it outright. */
void sub_08066988(u32 i)
{
    struct Task *t;
    struct Actor *a;
    u8 *p;

    t = &gUnk_03002790[i];
    a = t->unk8C;
    if (a->unk04 != 0)
    {
        p = t->unk88;
        sub_0806be4c(i);
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        if (a->unk04 != 1)
        {
            if ((u8)(p[22] + 2) <= 1)
            {
                sub_08063f24(i);
                return;
            }
        }
    }
    t->unk08 = 0;
    t->unk04 = 0;
    t->unk82 = 0;
    sub_08006148(sub_0806a344, i);
    sub_080055b0(0, i);
}

/* Is the running task inside the camera window padded by (dx, dy)? */
u8 sub_08066a0c(s16 dx, u16 dy)
{
    struct Task *t;

    t = gUnk_03002490;
    if (gUnk_03002158[0] - dx < t->unk48
        && t->unk48 < gUnk_03002158[1] + dx
        && gUnk_03002158[2] - (s16)dy < t->unk4A
        && t->unk4A < gUnk_03002158[3] + (s16)dy)
        return 1;
    return 0;
}

u8 sub_08066a6c(void)
{
    return sub_08066a0c(80, 40);
}

u8 sub_08066a80(void)
{
    return sub_08066a0c(360, 240);
}

void sub_08066a94(u8 mode)
{
    struct Task *t;
    struct Actor *a;
    struct TaskGfx *g;
    struct GfxHeader *h;
    u32 *tbl;
    u32 n;

    t = gUnk_03002490;
    a = t->unk8C;
    if (a->unk28 != 0)
        return;
    if (mode == 1)
    {
        tbl = t->unk38;
        g = (struct TaskGfx *)tbl[t->unk3C];
        a->unk28 = (u32)(g->unk04 + 1);
        n = *g->unk04 >> 1;
    }
    else
    {
        h = a->unk64.unk00;
        a->unk28 = (u32)h->unk08;
        n = h->unk00 << 4;
    }
    a->unk24 = n;
}

void sub_08066ae0(void)
{
    struct Task *t;
    struct Actor *a;
    u8 v;

    t = gUnk_03002490;
    a = t->unk8C;
    if (t->unk73 != 0)
    {
        switch (gUnk_03002350)
        {
        case 2:
            v = 13;
            break;
        case 3:
            v = 16;
            break;
        case 4:
            v = 18;
            break;
        case 1:
        default:
            v = 10;
            break;
        }
        a->unk02 = v;
        gUnk_03002490->unk73 = 0;
    }
    sub_080637cc();
}

void sub_08066b34(u32 def)
{
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    if (a->unk1A != -1)
    {
        t->unk0C = (u32)sub_08066c74;
        sub_08063908(def);
        gUnk_03002490->unk8C->unk16 = 6;
    }
}

void sub_08066b70(void)
{
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    if (a->unk1A == -1)
    {
        sub_08063ff4();
    }
    else if (t->unk54 < 0)
    {
        t->unk1C = 9;
    }
    else
    {
        t->unk1C = 3;
    }
}

void sub_08066ba8(void)
{
    struct Task *t;

    sub_08063e14();
    sub_08006304();
    t = gUnk_03002490;
    t->unk58 = 0x8000;
    if (t->unk43 == 1)
        t->unk1C = 0;
    else
        t->unk1C = 6;
}

void sub_08066bdc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk1C = t->unk1C + 1;
    if (t->unk1C > 11)
        t->unk1C = 0;
    t = gUnk_03002490;
    t->unk54 = gUnk_0873E1B8[t->unk1C];
}

void sub_08066c08(u32 def, u8 b)
{
    gUnk_03002490->unk8C->unk1A = 0xFFFF;
    sub_08063908(def);
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    sub_08066e88(b);
}

void sub_08066c3c(u32 def)
{
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    if (a->unk1A != -1 && t->unk74 != 2)
        sub_08066c08(def, 1);
}

/* Draw the running task plus its trailing "sparkle" sprite. */
/* Draw the running task plus its trailing "sparkle" sprite. */
void sub_08066c74(void)
{
    struct Task *p;
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 *tbl;

    p = gUnk_03002490;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    if (sub_08065160() != 0)
    {
        if (sub_08005acc() == 0)
            return;
        t = gUnk_03002490;
        tbl = t->unk38;
        sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40,
                     t->unk48 - gUnk_03002348,
                     (s16)(t->unk4A - gUnk_030023E4));
        u = gUnk_03002490;
        if ((u->unk13 & 7) == 0)
        {
            if (u->unk3C == 3)
            {
                u->unk8C->unk1A = 3;
            }
            else
            {
                a = u->unk8C;
                if (a->unk16 <= 0)
                {
                    a->unk16 = 6;
                    u->unk8C->unk1A++;
                    a = u->unk8C;
                    if (a->unk1A > 7)
                        a->unk1A = 4;
                }
            }
        }
        sub_08066dcc();
        t = gUnk_03002490;
        sub_08001a94(t->unk42, gUnk_08752BA8[t->unk8C->unk1A], t->unk3E, 0,
                     (s16)(gUnk_030023B4 + (t->unk48 - gUnk_03002348)),
                     (s16)(gUnk_030023D4 + (t->unk4A - gUnk_030023E4)));
        a = gUnk_03002490->unk8C;
        a->unk16--;
    }
    else
    {
        sub_08063fe0();
    }
}

/* Offset of the trailing sprite for the running task's animation. */
/* Offset of the trailing sprite for the running task's animation. */
/* Offset of the trailing sprite for the running task's animation. */
/* Offset of the trailing sprite for the running task's animation. */
void sub_08066dcc(void)
{
    struct Task *t;
    s32 i;

    t = gUnk_03002490;
    i = t->unk3C;
    switch (t->unk76)
    {
    case 9:
        gUnk_030023B4 = gUnk_0873E220[i * 2] * t->unk43;
        gUnk_030023D4 = gUnk_0873E220[i * 2 + 1];
        break;
    case 0:
        gUnk_030023B4 = gUnk_0873E1E8[i * 2] * t->unk43;
        gUnk_030023D4 = gUnk_0873E1E8[i * 2 + 1];
        break;
    case 17:
        gUnk_030023B4 = 0;
        if (t->unk3C == 3)
            gUnk_030023D4 = 13;
        else
            gUnk_030023D4 = -13;
        break;
    default:
        sub_0806ee2c();
        gUnk_030023B4 = gUnk_030023D4 = 0;
        break;
    }
}

void sub_08066e88(u8 a)
{
    struct Task *t;
    struct Task *u;
    s32 i;
    u8 kind;
    s32 v;

    kind = 0;
    if (gUnk_03002490->unk74 == 1)
        kind = 1;
    sub_08066dcc();
    t = gUnk_03002490;
    i = sub_08064fc4(0, 38, kind, 0,
                     (s16)(t->unk48 + gUnk_030023B4),
                     (s16)(t->unk4A + gUnk_030023D4), 0);
    if (i != -1)
    {
        u = &gUnk_03002790[i];
        v = gUnk_03002490->unk7F;
        if (v == -1)
            v = sub_08063b38();
        u->unk1C = v;
        u->unk28 = a;
        if (gUnk_02006178 == 1)
        {
            gUnk_03002490->unk8C->unk0D = 1;
            u->unk73 = 3;
            u->unk0C = (u32)sub_0806523c;
            u->unk42 = 11;
            u->unk38 = gUnk_08752BA8;
            u->unk3C = 0;
        }
    }
}
