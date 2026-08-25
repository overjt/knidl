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
extern s16 gUnk_030023E4;

extern u32 sub_08005acc(void);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern u8 sub_08066a6c(void);
extern u8 sub_08066a80(void);
extern void sub_0800a554(void);
extern void sub_08063fe0(void);
extern void sub_0806685c(u16 *src, u32 size, u32 c);
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
    struct Task *t;
    struct Task *u;
    struct Task *w;
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
    struct Task *u;
    struct Task *v;
    struct Task *w;
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
    struct Task *t;
    struct Task *u;
    struct Task *w;
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
    struct Task *t;
    struct Task *u;
    struct Task *w;
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
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
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
