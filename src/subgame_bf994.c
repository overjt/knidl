/* game_code_and_rodata_080653ec_0806ef5c 0x080BF994-0x080C0DE8
 * (issue #66, module M36 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BF994 0x080C0DE8 src/subgame_bf994.c --newpb
 *
 * The middle of M36: the per-slot task that owns one player's marker, and
 * the geometry helpers the whole module places sprites with.
 *
 *   sub_080bf994 / sub_080bfb24 / sub_080bfb4c / sub_080bfb94
 *       the slot task's entry and its three short states.
 *   sub_080bfbf4   the "hit" animation: six frames off gUnk_0875676C driven
 *       by a countdown in Task.unk30/unk34, then the hand-off through
 *       sub_080bdf3c.
 *   sub_080bfde8 / sub_080bfe64   the eliminated / winner states.
 *   sub_080bff28   the free-running slot driver: waits out the intro, then
 *       loops forever re-reading the owning task's slot (Task.unk34) and,
 *       whenever it changes, re-arms the sprite and recomputes the hand-off
 *       speed as Div((gUnk_08756770[next] - gUnk_08756770[cur]) << 4, frames).
 *   sub_080c0074 / sub_080c0388 / sub_080c0540   the three placement bodies.
 *   sub_080c05f0 / sub_080c0704   set a slot's sprite frame / animation.
 *   sub_080c061c   position on the 16.16 parabola p0 + v*t + (a*t*t)/2:
 *       p0 from gUnk_08756798/gUnk_087567A0, v from gUnk_087567A8[c][b] and
 *       the two coefficient rows from gUnk_08756D3C[c][0..1].
 *   sub_080c072c / sub_080c0a10 / sub_080c0b18 / sub_080c0c58 / sub_080c0ca4
 *       the remaining table-driven placement and animation helpers.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s8 gUnk_02006168;
extern u8 gUnk_020061DC;
extern s8 gUnk_02006A10[];
extern u8 gUnk_0200AF10;
extern u8 gUnk_0200AFF0;
extern u8 gUnk_0200B044[];
extern u16 gUnk_03001E90;
extern u8 gUnk_03001470[];
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000FA8;
extern vu16 gUnk_03001EB8[];
extern u16 gUnk_03002150;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;

extern s8 gUnk_08756560[];
extern s8 gUnk_08756564[];
extern u32 gUnk_08755E00[];
extern u32 gUnk_08755E0C[];
extern u32 gUnk_08755E44[];
extern u32 gUnk_08755E7C[];
extern vu16 gUnk_03001270[];
extern vu16 gUnk_03001ED8;
extern s32 gUnk_08756540[];
extern s32 gUnk_08756550[];
extern s16 gUnk_08756798[];
extern s32 gUnk_087567A8[][3];
extern s32 *gUnk_08756D3C[][2];
extern u32 gUnk_08755EB4;
extern u32 gUnk_08755EB8[];
extern u32 gUnk_08755EC4[];
extern u32 gUnk_087571F8[];
extern s16 gUnk_08757014[];
extern s32 *gUnk_0875716C[];
extern s32 gUnk_08757178[][4];
extern s32 gUnk_087571B8[][4];
extern s32 gUnk_08756D74[];
extern s32 gUnk_08756DC8[];
extern s32 gUnk_08756E1C[];
extern s32 gUnk_08756E38[];
extern s32 gUnk_08756E54[][4];
extern s32 gUnk_08756EC4[][4];
extern s32 gUnk_08756F34[][4];
extern s32 gUnk_08756FA4[][4];
extern s16 gUnk_08756770[];
extern u16 gUnk_08756778[];
extern s8 gUnk_0875673C[];
extern s16 gUnk_087567A0[];
extern u8 gUnk_0875676C[];
extern u32 gUnk_08756780[];
extern u32 gUnk_0875678C[];
extern u32 *gUnk_0875671C[];
extern u32 gUnk_0875674C[];
extern u32 gUnk_0875675C[];
extern u32 gUnk_08756528[];
extern u8 gUnk_08756570[];
extern u8 gUnk_0875665C[];
extern u8 gUnk_0875665F[];
extern u8 gUnk_08756662[];
extern u32 gUnk_08756668[];
extern u32 gUnk_08756670[];
extern u32 gUnk_08756678[];
extern u32 gUnk_08756680[];
extern u32 gUnk_08756688[];
extern u32 gUnk_08755DC0;
extern u16 gUnk_08756538[];
extern u8 gUnk_087565E0[];
extern u8 *gUnk_08756650[];
extern u8 *gUnk_087565F4[];
extern u32 *gUnk_0875670C[];
extern s16 gUnk_0875672C[];
extern s16 gUnk_08756734[];
extern s8 gUnk_08756740[];
extern s8 gUnk_08756744[];
extern s8 gUnk_08756748[];
extern u32 gUnk_087566A4[];
extern u32 gUnk_087566D8[];

extern void TaskYieldTrampoline(u32 frames);

extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_0800214c(void);
extern void sub_080021dc(void);
extern s32 sub_080058e4(u32 type, s32 idx);
extern void sub_08006148(void *a, u32 i);
extern void sub_08005a90(void);
extern void sub_08005ca0(void);
extern void sub_080059d8(void);
extern void TaskDispatchTrampoline(void);
extern void sub_080bdf9c(u32 a);
extern void sub_080c0ca4(void);
extern void sub_08006338(s32 a);
extern void sub_080031b8(u32 a);
extern void sub_080060c0(void);
extern void sub_08009eb8(u32 a, u32 b);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
extern void sub_080062c4(void);
extern u32 sub_08002ee8(u32 range);
extern void sub_08003110(u32 a);
extern void sub_080034d0(void);
extern void sub_08006138(void);
extern void sub_080b9d0c(s32 a);
extern void sub_080b9d24(void);
extern void sub_080b9e30(void);

extern void sub_080bda78(void);
extern void sub_080bda98(void);
extern u32 sub_080bdd28(void);
extern u32 sub_080bdd70(void);
extern void sub_080bddb8(void);
extern void sub_080bde0c(void);
extern void sub_080bde78(u32 a);
extern void sub_080be04c(void);
extern void sub_080be1b0(void);
extern void sub_080be1d0(void);
extern void sub_080be4a4(void);
extern void sub_080be550(void);
extern void sub_080be5fc(void);
extern void sub_080be6b4(u32 a);
extern void sub_080be714(u32 a);
extern void sub_080be774(void);
extern void sub_080be850(void);
extern u32 sub_080bf0ac(void);
extern void sub_080bf154(void);
extern void sub_080be8a8(void);
extern void sub_080bfdb0(void);
extern void sub_080bfdcc(void);
extern void sub_080be010(void);
extern void sub_08006244(void);
extern s32 Div(s32 numerator, s32 denominator);
extern void sub_080c05f0(u32 a);
extern void sub_080c0704(u32 a);
extern void sub_080c0b18(u32 a);
extern s32 sub_080c1ebc(s32 a, s32 b);
extern void sub_080c061c(s32 a, s32 b, s32 c, s32 d);
extern void sub_080c072c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void sub_080c0a10(s32 a, s32 b, s32 c, s32 d, s16 e);
/* NOTE: sub_080bdebc's third parameter is `u16` at its definition
   (src/subgame_bda2c.c) but the ROM's call sites here sign-extend the
   argument, so the declaration visible here is the wider `s32` - the
   original source had the same prototype mismatch. */
extern void sub_080bdebc(s32 a, s32 b, s32 c, s32 d);
extern void sub_080bdf3c(s32 a, s32 b, u32 c, u32 d);
extern void sub_080be7c0(u32 a);


void sub_080bf994(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a90;
    t->unk15 = 10;
    v = gUnk_03002490;
    v->unk4C = gUnk_0875674C[v->unk1C] << 16;
    v->unk50 = gUnk_0875675C[v->unk1C] << 16;
    v->unk38 = gUnk_0875671C[v->unk1C];
    v->unk40 = 0;
    if (v->unk1C == 3)
        v->unk43 = -1;
    else
        v->unk43 = 1;
    sub_08006338(0);
    w = gUnk_03002490;
    u = &gUnk_03002790[w->unk44];
    w->unk20 = 0;
    if (u->unk34 == w->unk1C) {
        TaskYieldTrampoline(30);
        u->unk28 = -1;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        sub_080031b8(254);
        z = gUnk_03002490;
        sub_080bdf3c(z->unk4C + (gUnk_08756560[z->unk1C] << 16) * z->unk43,
                     z->unk50 + (gUnk_08756564[z->unk1C] << 16),
                     z->unk1C, z->unk43);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        u->unk28 = u->unk20;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
    } else {
        while (u->unk28 < 0)
            TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk14 = 11;
    sub_08006138();
}

void sub_080bfb24(void)
{
    if (gUnk_03002490->unk14 != 10)
        sub_08006148(sub_080be8a8, gCurTaskIdx);
}

void sub_080bfb4c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk15 = 11;
    while (1) {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(5);
    }
}

void sub_080bfb94(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    if ((u->unk34 == ((t->unk1C + 3) & 3) && u->unk28 <= 2)
     || (u->unk34 == ((t->unk1C + 1) & 3) && u->unk28 > 2)) {
        gUnk_03002490->unk14 = 12;
        sub_08006148(sub_080be8a8, gCurTaskIdx);
    }
}

void sub_080bfbf4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;
    s32 m;
    struct Task **gp;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    t->unk15 = 12;
    if (sub_08002ee8(8) == 0) {
        if (u->unk28 <= 2)
            u->unk20 = u->unk28 + 3;
        else
            u->unk20 = u->unk28;
    } else {
        if (u->unk28 > 2)
            u->unk20 = u->unk28 - 3;
        else
            u->unk20 = u->unk28;
    }
    v = gUnk_03002490;
    v->unk28 = u->unk20;
    m = u->unk24;
    v->unk30 = 6;
    v->unk34 = 0;
    gp = &gUnk_03002490;
    v->unk2C = m - 8;
    if (v->unk2C != 0) {
        do {
        w = gUnk_03002490;
        w->unk30--;
        if (w->unk30 == 0) {
            w->unk34++;
            if (w->unk34 == 4)
                w->unk34 = 0;
            x = gUnk_03002490;
            x->unk3C = gUnk_0875676C[x->unk34];
            if (x->unk34 & 1)
                x->unk30 = 6;
            else
                x->unk30 = 5;
        }
        TaskYieldTrampoline(1);
        y = *gp;
        y->unk2C--;
        } while (y->unk2C != 0);
    }
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080031b8(254);
    z = gUnk_03002490;
    sub_080bdf3c(z->unk4C + (gUnk_08756560[z->unk1C] << 16) * z->unk43,
                 z->unk50 + (gUnk_08756564[z->unk1C] << 16),
                 z->unk1C, z->unk43);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(7);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk14 = 11;
    sub_08006138();
}

void sub_080bfd58(void)
{
    if (gUnk_03002490->unk14 != 12)
        sub_08006148(sub_080be8a8, gCurTaskIdx);
}

void sub_080bfd80(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080bfdb0;
    t->unk14 = 0;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08756780);
    sub_08006138();
}

void sub_080bfdb0(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_0875678C);
}

void sub_080bfdcc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08756780);
}

void sub_080bfde8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    t->unk0C = 0;
    t->unk00 = (u32)sub_08005a90;
    t->unk15 = 0;
    sub_080be010();
    v = gUnk_03002490;
    v->unk30 = 0;
    v->unk24 = u->unk34;
    sub_080c05f0(v->unk24);
    sub_080c0704(gUnk_03002490->unk18);
    TaskYieldTrampoline(30);
    sub_080c0b18(gUnk_03002490->unk24);
    sub_080c0704(gUnk_03002490->unk24);
    w = gUnk_03002490;
    w->unk38 = 0;
    TaskYieldTrampoline(30);
    u->unk28 = -2;
    gUnk_03002490->unk14 = 1;
    sub_08006138();
}

void sub_080bfe64(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;
    s32 m;
    s32 k;

    t = gUnk_03002490;
    n = t->unk30 + 1;
    t->unk30 = n;
    if (t->unk48 >= -63 && t->unk48 <= 303 && t->unk4A > -64 && t->unk4A <= 223
     && (n & 8)) {
        m = n & 7;
        k = 0;
        if (m > 2) {
            k = 2;
            if (m <= 4)
                k = 1;
        }
        sub_08001a94(8, sub_08001cc8(gUnk_08755E00[k],
                                    gUnk_08756770[gUnk_03002490->unk24],
                                    gUnk_08756770[gUnk_03002490->unk24], 0),
                     gUnk_03002490->unk3E, gUnk_03002490->unk40,
                     gUnk_03002490->unk48, gUnk_03002490->unk4A);
    }
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080bfdcc, gCurTaskIdx);
}

void sub_080bff28(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *p;
    struct Task *q;
    s32 d;
    s32 m;
    s32 msk;
    s32 n;
    s16 *tb;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    t->unk0C = 0;
    t->unk00 = (u32)sub_08005a90;
    t->unk15 = 1;
    gUnk_03002490->unk42 = 8;
    v = gUnk_03002490;
    v->unk38 = gUnk_08755E00;
    v->unk28 = 0;
    v->unk2C = 0;
    v->unk30 = 0;
    v->unk34 = 0;
    v->unk24 = u->unk34;
    sub_080c0704(u->unk34);
    while (u->unk28 < -1)
        TaskYieldTrampoline(1);
    w = gUnk_03002490;
    w->unk3C = (u->unk34 * 3) << 1;
    sub_080c05f0(u->unk34);
    x = gUnk_03002490;
    x->unk58 = 0xFFFBC000;
    x->unk60 = 128 << 7;
    x->unk28 = 1;
    while (u->unk28 < 0) {
        tb = gUnk_087567A0;
        if (gUnk_03002490->unk50 > (tb[u->unk34] << 16)) {
            sub_08006244();
            sub_080c05f0(u->unk34);
        }
        TaskYieldTrampoline(1);
    }
    sub_08006244();
    y = gUnk_03002490;
    y->unk24 = u->unk34 - 1;
    while (1) {
        z = gUnk_03002490;
        tb = gUnk_08756770;
        u = &gUnk_03002790[z->unk44];
        if (z->unk24 != u->unk34) {
            sub_080c05f0(u->unk34);
            sub_080c0704(u->unk34);
            p = gUnk_03002490;
            m = u->unk34;
            p->unk24 = m;
            p->unk28 = 2;
            if (u->unk28 <= 2) {
                n = m + 1;
                msk = 3;
                d = n & msk;
            } else {
                n = m - 1;
                msk = 3;
                d = n & msk;
            }
            n = tb[d];
            n -= tb[m];
            gUnk_03002490->unk2C = Div(n << 4, u->unk24);
        }
        TaskYieldTrampoline(1);
    }
}

void sub_080c0074(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *p;
    struct Task *q;
    s32 n;
    s32 st;
    s32 k;
    s32 m;
    s32 c;
    s32 x;
    s32 y;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    n = t->unk30 + 1;
    t->unk30 = n;
    if (u->unk28 == 6) {
        t->unk14 = 2;
        sub_08006148(sub_080bfdcc, gCurTaskIdx);
        return;
    }
    st = t->unk28;
    switch (st) {
    case 0:
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        break;
    case 3:
        if (n & 1)
            t->unk4C += 0x40000;
        else
            t->unk4C -= 0x40000;
        /* fall through */
    case 1:
        v = gUnk_03002490;
        m = v->unk30 & 7;
        if (m <= 2)
            v->unk3C = 0;
        else if (m <= 4)
            v->unk3C = 1;
        else
            v->unk3C = 2;
        break;
    case 2:
        if (u->unk28 > 2) {
            k = u->unk28 - 3;
            sub_080c072c(u->unk34, k, u->unk2C, u->unk30, u->unk24, 1);
            sub_080c061c((u->unk34 + 3) & 3, k, u->unk2C, u->unk24 - u->unk30);
            sub_080c0a10((u->unk34 + 3) & 3, k, u->unk2C, u->unk24 - u->unk30,
                         (s16)(gUnk_08756778[gUnk_03002490->unk24]
                               + ((gUnk_03002490->unk2C * u->unk30) >> 5)));
            if (u->unk34 == 0 || u->unk34 == 3) {
                w = gUnk_03002490;
                w->unk34 -= 4 << (st - k);
                if (w->unk34 < 0)
                    w->unk34 += 512;
            } else {
                w = gUnk_03002490;
                w->unk34 += 4 << (st - k);
                if (w->unk34 > 511)
                    w->unk34 -= 512;
            }
        } else {
            k = u->unk28;
            sub_080c072c(u->unk34, k, u->unk2C, u->unk30, u->unk24, 0);
            sub_080c061c(u->unk34, k, u->unk2C, u->unk30);
            sub_080c0a10(u->unk34, k, u->unk2C, u->unk30,
                         (s16)(gUnk_08756778[gUnk_03002490->unk24]
                               + ((gUnk_03002490->unk2C * u->unk30) >> 5)));
            if (u->unk34 == 0 || u->unk34 == 3) {
                w = gUnk_03002490;
                w->unk34 += 4 << (st - k);
                if (w->unk34 > 511)
                    w->unk34 -= 512;
            } else {
                w = gUnk_03002490;
                w->unk34 -= 4 << (st - k);
                if (w->unk34 < 0)
                    w->unk34 += 512;
            }
        }
        p = gUnk_03002490;
        m = p->unk30 & 7;
        if (m <= 2)
            p->unk3C = 0;
        else if (m <= 4)
            p->unk3C = 1;
        else
            p->unk3C = 2;
        if (u->unk30 == u->unk24)
            gUnk_03002490->unk28 = 3;
        break;
    }
    q = gUnk_03002490;
    if ((s16)q->unk3C != -1) {
        if ((q->unk30 & 3) == 0)
            sub_080bdebc(q->unk4C, q->unk50,
                         (s16)(gUnk_08756770[q->unk24]
                               + ((q->unk2C * u->unk30) >> 4)),
                         q->unk34);
        x = (gUnk_03002490->unk4C - gUnk_03000B78) >> 16;
        y = (gUnk_03002490->unk50 - gUnk_03000FA8) >> 16;
        if (x >= -63 && x <= 303 && y > -64 && y <= 223)
            sub_08001a94(8,
                sub_08001cc8(gUnk_08755E00[(s16)gUnk_03002490->unk3C],
                    (s16)(gUnk_08756770[gUnk_03002490->unk24]
                          + ((gUnk_03002490->unk2C * u->unk30) >> 4)),
                    (s16)(gUnk_08756770[gUnk_03002490->unk24]
                          + ((gUnk_03002490->unk2C * u->unk30) >> 4)),
                    (s16)gUnk_03002490->unk34),
                gUnk_03002490->unk3E, gUnk_03002490->unk40, x, y);
    }
}

void sub_080c0388(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    t->unk0C = (u32)sub_08005ca0;
    t->unk15 = 2;
    gUnk_03001270[0] = 0x7FFF;
    v = gUnk_03002490;
    v->unk28 = u->unk34;
    v->unk2C = 0;
    v->unk30 = 0;
    sub_080c0704(v->unk28);
    gUnk_03002490->unk42 = 6;
    sub_080062c4();
    w = gUnk_03002490;
    w->unk4C = gUnk_0875672C[w->unk28] << 16;
    w->unk50 = (gUnk_08756734[w->unk28] + gUnk_0875673C[w->unk28]) << 16;
    sub_080031b8(255);
    x = gUnk_03002490;
    sub_080bdf3c(x->unk4C, x->unk50, x->unk28, x->unk43);
    y = gUnk_03002490;
    if (y->unk28 == 0)
        y->unk38 = gUnk_08755E0C;
    else if (y->unk28 == 2)
        y->unk38 = gUnk_08755E7C;
    else
        y->unk38 = gUnk_08755E44;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 6);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_08006138();
}

void sub_080c0540(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    n = sub_080c1ebc(t->unk28, t->unk2C);
    v = gUnk_03002490;
    v->unk2C = n;
    if (v->unk30 <= 7) {
        if (((v->unk30 >> 1) & 1) == 0) {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x80 << 5;
        } else {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0xC0 << 5;
        }
        gUnk_03002490->unk30++;
    }
    if (u->unk28 == -4) {
        gUnk_03002490->unk14 = 1;
        sub_08006148(sub_080bfdcc, gCurTaskIdx);
    }
}

void sub_080c05f0(u32 a)
{
    struct Task *t = gUnk_03002490;

    t->unk4C = gUnk_08756798[a] << 16;
    t->unk50 = gUnk_087567A0[a] << 16;
}

void sub_080c061c(s32 a, s32 b, s32 c, s32 d)
{
    struct Task *t;
    struct Task *u;
    s32 *p1;
    s32 *p2;

    p1 = gUnk_08756D3C[c][0];
    p2 = gUnk_08756D3C[c][1];
    if (a == 0 || a == 3)
        gUnk_03002490->unk4C = (gUnk_08756798[a] << 16) - gUnk_087567A8[c][b] * d;
    else
        gUnk_03002490->unk4C = (gUnk_08756798[a] << 16) + gUnk_087567A8[c][b] * d;
    gUnk_03002490->unk50 = (gUnk_087567A0[a] << 16) + p1[b * 4 + a] * d
             + ((p2[b * 4 + a] * d * d) >> 1);
}

void sub_080c0704(u32 a)
{
    gUnk_03000B78 = gUnk_08756540[a];
    gUnk_03000FA8 = gUnk_08756550[a];
}

void sub_080c072c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    if (f == 0) {
        if (a == 0 || a == 3)
            gUnk_03000B78 = gUnk_08756540[a] - gUnk_08756D74[c * 3 + b] * d
                          + ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        else
            gUnk_03000B78 = gUnk_08756540[a] + gUnk_08756D74[c * 3 + b] * d
                          - ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        switch (b) {
        case 2:
            gUnk_03000FA8 = gUnk_08756550[a] + gUnk_08756F34[c][a] * d
                          + ((gUnk_08756FA4[c][a] * d * d) >> 1);
            break;
        case 1:
            gUnk_03000FA8 = gUnk_08756550[a] + gUnk_08756E54[c][a] * d
                          + ((gUnk_08756EC4[c][a] * d * d) >> 1);
            break;
        case 0:
            if (a <= 1)
                gUnk_03000FA8 = gUnk_08756550[a] - gUnk_08756E1C[c] * d
                              + ((gUnk_08756E38[c] * d * d) >> 1);
            else
                gUnk_03000FA8 = gUnk_08756550[a] + gUnk_08756E1C[c] * d
                              - ((gUnk_08756E38[c] * d * d) >> 1);
            break;
        }
    } else {
        if (a <= 1)
            gUnk_03000B78 = gUnk_08756540[a] + gUnk_08756D74[c * 3 + b] * d
                          - ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        else
            gUnk_03000B78 = gUnk_08756540[a] - gUnk_08756D74[c * 3 + b] * d
                          + ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        switch (b) {
        case 2:
            gUnk_03000FA8 = gUnk_08756550[(a + 3) & 3]
                          + gUnk_08756F34[c][(a + 3) & 3] * (e - d)
                          + ((gUnk_08756FA4[c][(a + 3) & 3] * (e - d) * (e - d)) >> 1);
            break;
        case 1:
            gUnk_03000FA8 = gUnk_08756550[(a + 3) & 3]
                          + gUnk_08756E54[c][(a + 3) & 3] * (e - d)
                          + ((gUnk_08756EC4[c][(a + 3) & 3] * (e - d) * (e - d)) >> 1);
            break;
        case 0:
            if (a == 0 || a == 3)
                gUnk_03000FA8 = gUnk_08756550[a] - gUnk_08756E1C[c] * d
                              + ((gUnk_08756E38[c] * d * d) >> 1);
            else
                gUnk_03000FA8 = gUnk_08756550[a] + gUnk_08756E1C[c] * d
                              - ((gUnk_08756E38[c] * d * d) >> 1);
            break;
        }
    }
}

void sub_080c0a10(s32 a, s32 b, s32 c, s32 d, s16 e)
{
    s32 x;
    s32 y;
    s32 *p;

    if ((s16)gUnk_03002490->unk3C != -1) {
        p = gUnk_0875716C[b];
        if (a == 0 || a == 3)
            x = (gUnk_08756798[a] << 16) - gUnk_087567A8[c][b] * d;
        else
            x = (gUnk_08756798[a] << 16) + gUnk_087567A8[c][b] * d;
        y = (gUnk_08757014[a] << 16) + p[c * 4 + a] * d;
        sub_08001a94(10, sub_08001cc8(gUnk_08755EB4, (s16)e, (s16)e, 0), 0, 0,
                     (x >> 16) - (gUnk_03000B78 >> 16),
                     (s16)((y >> 16) - (gUnk_03000FA8 >> 16)));
    }
}

void sub_080c0b18(u32 a)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *z;

    t = gUnk_03002490;
    t->unk1C = -gUnk_08757178[t->unk18][a];
    t->unk20 = -gUnk_087571B8[t->unk18][a];
    t->unk6C = 0;
    do {
        u = gUnk_03002490;
        if ((s16)u->unk6C <= 16) {
            u->unk1C += gUnk_08757178[u->unk18][a];
            u->unk20 += gUnk_087571B8[u->unk18][a];
        } else {
            u->unk1C -= gUnk_08757178[u->unk18][a];
            u->unk20 -= gUnk_087571B8[u->unk18][a];
        }
        gUnk_03000B78 += gUnk_03002490->unk1C;
        gUnk_03000FA8 += gUnk_03002490->unk20;
        if ((gUnk_03002490->unk1C < 0 && gUnk_03000B78 < gUnk_08756540[a])
         || (gUnk_03002490->unk1C > 0 && gUnk_03000B78 > gUnk_08756540[a]))
            gUnk_03000B78 = gUnk_08756540[a];
        w = gUnk_03002490;
        if ((w->unk20 < 0 && gUnk_03000FA8 < gUnk_08756550[a])
         || (w->unk20 > 0 && gUnk_03000FA8 > gUnk_08756550[a]))
            gUnk_03000FA8 = gUnk_08756550[a];
        TaskYieldTrampoline(1);
        z = gUnk_03002490;
        z->unk6C++;
    } while ((s16)z->unk6C <= 31);
}

void sub_080c0c58(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk0C = 0;
    t->unk00 = (u32)sub_080059d8;
    t->unk04 = (u32)sub_080c0ca4;
    t->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    TaskDispatchTrampoline();
}

void sub_080c0ca4(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;
    s32 y;
    s32 n;

    t = gUnk_03002490;
    n = t->unk1C + 8;
    t->unk1C = n;
    if (n > 255)
        t->unk1C = n & 255;
    u = gUnk_03002490;
    x = (u->unk4C - gUnk_03000B78) >> 16;
    y = (u->unk50 - gUnk_03000FA8) >> 16;
    if (x >= -63 && x <= 303 && y > -64 && y <= 223)
        sub_08001a94(7, sub_08001cc8(gUnk_08755EB8[(s16)u->unk3C], u->unk18,
                                     u->unk18, (s16)u->unk1C),
                     0, 0, x, y);
}

void sub_080c0d30(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk0C = (u32)sub_08005ca0;
    t->unk00 = (u32)sub_08005a90;
    t->unk04 = 0;
    t->unk38 = gUnk_08755EC4;
    switch (t->unk74) {
    case 0:
        sub_080bdf9c(4);
        sub_080bdf9c(5);
        sub_080bdf9c(6);
        sub_080bdf9c(7);
        break;
    case 1:
    case 3:
        sub_080bdf9c(8);
        sub_080bdf9c(9);
        sub_080bdf9c(10);
        sub_080bdf9c(11);
        break;
    case 2:
        sub_080bdf9c(12);
        sub_080bdf9c(13);
        sub_080bdf9c(14);
        sub_080bdf9c(15);
        break;
    }
    u = gUnk_03002490;
    u->unk14 = u->unk74;
    sub_08002e98(gUnk_03002490->unk14, 16, gUnk_087571F8);
    sub_08006138();
}
