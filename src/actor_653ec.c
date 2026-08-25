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

void sub_08065470(void);
void sub_0806555c(void);

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
