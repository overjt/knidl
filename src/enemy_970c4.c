#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* This module's |x|: the ROM tests `< 0` first, unlike global.h's abs(). */
#define ABS(n) ((n) < 0 ? -(n) : (n))

struct Unk0200D120
{
    /*0x00*/ u8 filler00[0x20];
    /*0x20*/ u16 unk20;
    /*0x22*/ u8 filler22[0x26];
    /*0x48*/ s8 *unk48;
    /*0x4C*/ u8 filler4C[0x24];
};

/* RAM cells */
extern struct Unk0200D120 gUnk_0200D120[];

/* ROM tables */
extern u32 gUnk_087537E8[];

/* Externals */
extern void sub_080059d8(void);
extern void sub_08005d9c(void);
extern u32 sub_08002ee8(u32 range);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006338(s32 a);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_080974c8(void);

/* Defined below */
void sub_080970c4(void);

void sub_080970c4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    struct Task *q0;
    struct Task *q1;
    struct Task *r;
    s8 *p0;
    s8 *p1;
    s8 *p2;
    s8 *p3;
    s8 *p2b;
    s8 *p3b;
    s8 *pa;
    s8 *pb;
    u16 x;
    u16 y;
    s32 i0;
    s32 i1;
    s32 i2;
    s32 j2;
    s32 i3;
    s32 j3;
    s32 ia;
    s32 ib;
    s32 n;
    s32 k0;
    s32 k1;
    s32 k2;
    s32 k3;
    s32 c0_2;
    s32 c2_2;
    s32 c4_2;
    s32 u48_2;
    s32 c0_3;
    s32 c2_3;
    s32 c4_3;
    s32 u48_3;
    s32 h3;
    s16 *a0;
    s16 *a1;
    s16 *b2;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 10;
    u = gUnk_03002490;
    u->unk38 = gUnk_087537E8;
    u->unk40 = gUnk_0200D120[u->unk44 - 32].unk20;
    if (sub_08002ee8(2) != 0)
        gUnk_03002490->unk43 = 1;
    else
        gUnk_03002490->unk43 = -1;
    v = gUnk_03002490;
    v->unk28 = 0;
    v->unk04 = (u32)sub_080974c8;
    while (1) {
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(sub_08002ee8(8));
        w = gUnk_03002490;
        switch (w->unk73) {
        case 0:
            i0 = w->unk44;
            q0 = &gUnk_03002790[i0];
            a0 = &q0->unk48;
            k0 = q0->unk43;
            p0 = gUnk_0200D120[i0 - 32].unk48;
            x = k0 * (p0[2] + p0[0]) + *a0;
            y = (p0[3] + q0->unk4A) + p0[1];
            break;
        case 1:
            i1 = w->unk44;
            q1 = &gUnk_03002790[i1];
            a1 = &q1->unk48;
            k1 = q1->unk43;
            p1 = gUnk_0200D120[i1 - 32].unk48;
            x = k1 * (p1[2] + p1[0]) + *a1;
            y = (q1->unk4A + p1[5]) - (ABS(p1[5] - p1[3]) >> 1) + p1[1];
            break;
        case 2:
            i2 = w->unk44;
            p2 = gUnk_0200D120[i2 - 32].unk48;
            c4_2 = p2[4];
            c0_2 = p2[0];
            k2 = gUnk_03002790[i2].unk43;
            u48_2 = gUnk_03002790[i2].unk48;
            c2_2 = p2[2];
            x = u48_2 + (c4_2 - (ABS(c4_2 - c2_2) >> 1) + c0_2) * k2;
            j2 = gUnk_03002490->unk44;
            r = &gUnk_03002790[j2];
            b2 = &r->unk4A;
            p2b = gUnk_0200D120[j2 - 32].unk48;
            y = (p2b[3] + *b2) + p2b[1];
            break;
        case 3:
            i3 = w->unk44;
            p3 = gUnk_0200D120[i3 - 32].unk48;
            c4_3 = p3[4];
            c0_3 = p3[0];
            k3 = gUnk_03002790[i3].unk43;
            u48_3 = gUnk_03002790[i3].unk48;
            c2_3 = p3[2];
            x = u48_3 + (c4_3 - (ABS(c4_3 - c2_3) >> 1) + c0_3) * k3;
            j3 = gUnk_03002490->unk44;
            h3 = gUnk_03002790[j3].unk4A;
            p3b = gUnk_0200D120[j3 - 32].unk48;
            y = (h3 + p3b[5]) - (ABS(p3b[5] - p3b[3]) >> 1) + p3b[1];
            break;
        }
        ia = gUnk_03002490->unk44;
        pa = gUnk_0200D120[ia - 32].unk48;
        n = sub_08002ee8(ABS(pa[4] - pa[2]) >> 1);
        ib = gUnk_03002490->unk44;
        x = x + n * gUnk_03002790[ib].unk43;
        pb = gUnk_0200D120[ib - 32].unk48;
        y = y + sub_08002ee8(ABS(pb[5] - pb[3]) >> 1);
        z = gUnk_03002490;
        z->unk4C = x << 16;
        z->unk50 = y << 16;
        if ((z->unk28 & 1) != 0)
            z->unk43 = -z->unk43;
        gUnk_03002490->unk28++;
        sub_080061c0(0x14000, -0x1C00);
        gUnk_03002490->unk58 = -0x10000;
        sub_08006338(1);
        TaskYieldTrampoline(1);
        sub_08006338(2);
        TaskYieldTrampoline(2);
        sub_08006338(3);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x20000;
        sub_08006338(4);
        TaskYieldTrampoline(2);
        sub_08006338(1);
        TaskYieldTrampoline(1);
        sub_080061c0(-0x4000, 0x5A5A5A5A);
        gUnk_03002490->unk58 = -0x40000;
        sub_08006338(0);
        TaskYieldTrampoline(2);
        sub_080061c0(0x4000, 0x5A5A5A5A);
        sub_08006338(0);
        TaskYieldTrampoline(2);
    }
}
