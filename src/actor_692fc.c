/* game_code_and_rodata 0x080692FC-0x0806A344 (issue #64, module M18 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080692FC 0x0806A344 pending/batch1/actor_692fc.c --newpb
 *
 * The player-input dispatch layer of the actor core.  Six probe wrappers
 * (0x080694E0-0x080696A0) snapshot the current task's directional state into a
 * 6-byte stack record (sub_08069a64) and hand it to one of the input decoders
 * at 0x0801BCAC..0x0801C3A4; the three big dispatchers (sub_080692fc,
 * sub_080696a0, sub_08069888) then walk the actor's seven-entry handler table
 * at Actor.unk54, calling the first handler that claims the frame.  The tail
 * of the module is the class-1 "carried" task body: state machine entry
 * points (sub_08069ae4/sub_08069bbc), the sub_08069c8c sound dispatcher and
 * the sub_08069dc4/sub_08069e48 push/pop of the actor's transform.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* The 6-byte directional record sub_08069a64 fills on the stack: three raw
   bytes copied from Actor.unk50 plus three that are negated when the task
   faces left (Task.unk43 == -1). */
struct InputState
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 unk05;
};

/* Seven-entry handler table hanging off Actor.unk54; every entry is a
   u8 (*)(void) that returns 1 when it consumed the frame. */
struct ActorHandlers
{
    /*0x00*/ u32 unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ u32 unk0C;
    /*0x10*/ u32 unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ u32 unk18;
};

/* Block Actor.unk5C points at: two s8 mode bytes and two u8 (*)(void)
   hooks.  Compare struct ActorAux, which is the Actor.unk60 block. */
struct ActorVt
{
    /*0x00*/ s8 unk00;
    /*0x01*/ s8 unk01;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
};

extern vs16 gUnk_03004CA0[];
extern u8 gUnk_03005550[];
extern u16 gUnk_03002360;
extern u16 gUnk_030023D8;
extern u16 gUnk_0873E58C[];
extern s16 gUnk_0873E5A4[];
extern u32 gUnk_0873F910[];

extern u32 sub_08002ee8(u32 range);
extern void sub_080031b8(s32 id);
extern void sub_080055b0(s32 a, s32 b);
extern void sub_08006148(void *fn, s32 i);
extern void sub_08006338(s32 a);
extern void sub_08009e60(s32 a, s32 b);
extern void sub_0800a42c(void);
extern void sub_0801bcac(struct InputState *p);
extern void sub_0801bde0(struct InputState *p);
extern void sub_0801bf1c(struct InputState *p);
extern void sub_0801c030(struct InputState *p);
extern void sub_0801c12c(struct InputState *p);
extern void sub_0801c230(struct InputState *p);
extern void sub_0801c30c(struct InputState *p);
extern u32 sub_0801c3a4(struct InputState *p);
extern u32 sub_0802205c(struct InputState *p);
extern void sub_080224f8(s32 i);
extern void sub_0804087c(s32 a);
extern void sub_0806395c(u8 v);
extern void sub_08063990(u32 v);
extern s32 sub_08063dac(u32 i);
extern void sub_08063fe0(void);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern void sub_080649b4(s32 a, s32 b);
extern void sub_08065e1c(u32 def, u32 which);
extern void sub_08068e04(void);
extern u32 sub_0806a344(void);
extern void sub_0806b26c(void);
extern u32 sub_0806bb7c(void);
extern void sub_0806d65c(void);
extern void sub_0806d77c(void);
extern void sub_0806df28(s32 a, s32 b);
extern void sub_0806ee2c(void);
extern void sub_080b4240(void);
extern void sub_080b460c(void);
extern void sub_080b54d0(s32 i);

void sub_08069a64(struct InputState *out);
u32 sub_08069ae4(s8 a);
u32 sub_08069b44(void);
s8 sub_08069c48(void);
void sub_08069c8c(void);
void sub_08069d78(void);
void sub_08069dc4(void);
void sub_08069e48(void);
u32 sub_08069ea0(void);
void sub_08069f0c(void);
void sub_08069f70(void);
void sub_08069fb0(void);
void sub_0806a158(void);
u32 sub_0806a25c(void);

u32 sub_080692fc(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    t = gUnk_03002490;
    a = t->unk8C;
    r = 0;
    i = 4;
    k = t->unk7A;
    f = t->unk7B;
    sub_08069a64(&v);
    sub_0801bcac(&v);
    u = gUnk_03002490;
    if ((u->unk7B & 0x80) != 0)
        sub_08064d6c(140, u->unk48, ((s16 *)gUnk_03005550)[i], 0);
    if ((f & 1) != 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gUnk_03002490->unk7B & 1) == 0 && (gUnk_03002490->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk0C;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if ((f & 1) == 0)
        goto b2;
    if ((f & 0x40) == 0)
        goto s2;
b2:
    if ((gUnk_03002490->unk7B & 1) != 0 && (gUnk_03002490->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk08;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s2:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gUnk_03002490->unk7A & 1) != 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk04;
    }
    else
    {
        if ((gUnk_03002490->unk7A & 1) == 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk00;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s3:
    if (r == 1)
        return 1;
    if ((gUnk_03005550[0] & 3) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk10;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gUnk_03002490->unk7A & 1) != 0 && (gUnk_03005550[3] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk14;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gUnk_03002490->unk7A & 1) == 0 && (gUnk_03005550[1] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk18;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    return r;
}

u32 sub_080694e0(void)
{
    struct InputState v;

    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    sub_08069a64(&v);
    sub_0802205c(&v);
}

u32 sub_0806951c(void)
{
    struct InputState v;
    u32 r;

    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08069a64(&v);
        sub_0801c230(&v);
        if ((*(u32 *)gUnk_03005550 & 0x00FFFFFF) != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 sub_0806956c(void)
{
    struct InputState v;
    u32 r;

    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08069a64(&v);
        sub_0801c030(&v);
        if ((*(u32 *)gUnk_03005550 & 0x00FFFF00) != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 sub_080695bc(void)
{
    struct InputState v;
    u32 r;

    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08069a64(&v);
        sub_0801bf1c(&v);
        if (gUnk_03005550[0] != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 sub_08069604(void)
{
    struct InputState v;
    u32 r;

    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    r = 0;
    sub_08069a64(&v);
    sub_0801c30c(&v);
    if (gUnk_03005550[0] != 0 || gUnk_03005550[4] != 0 || gUnk_03005550[1] != 0)
        r = 1;
    return r;
}

u32 sub_08069660(void)
{
    struct InputState v;
    u32 r;

    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        sub_08069a64(&v);
        r = (u8)sub_0801c3a4(&v);
    }
    else
    {
        r = 0;
    }
    return r;
}

u32 sub_080696a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    t = gUnk_03002490;
    a = t->unk8C;
    r = 0;
    i = 4;
    k = t->unk7A;
    f = t->unk7B;
    sub_08069a64(&v);
    sub_0801bde0(&v);
    sub_080b460c();
    u = gUnk_03002490;
    if ((u->unk7B & 0x80) != 0)
        sub_08064d6c(140, u->unk48, ((s16 *)gUnk_03005550)[i], 0);
    if ((f & 1) != 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gUnk_03002490->unk7B & 1) == 0 && (gUnk_03002490->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk0C;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if ((f & 1) == 0)
        goto b2;
    if ((f & 0x40) == 0)
        goto s2;
b2:
    if ((gUnk_03002490->unk7B & 1) != 0 && (gUnk_03002490->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk08;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s2:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gUnk_03002490->unk7A & 1) != 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk04;
    }
    else
    {
        if ((gUnk_03002490->unk7A & 1) == 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->unk54)->unk00;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s3:
    if (r == 1)
        return 1;
    if ((gUnk_03005550[0] & 3) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk10;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gUnk_03002490->unk7A & 1) != 0 && (gUnk_03005550[3] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk14;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gUnk_03002490->unk7A & 1) == 0 && (gUnk_03005550[1] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk18;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    return r;
}

u32 sub_08069888(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    t = gUnk_03002490;
    a = t->unk8C;
    r = 0;
    i = 4;
    k = t->unk7A;
    f = t->unk7B;
    sub_08069a64(&v);
    sub_0801c12c(&v);
    u = gUnk_03002490;
    if ((u->unk7B & 0x80) != 0)
        sub_08064d6c(140, u->unk48, ((s16 *)gUnk_03005550)[i], 0);
    if ((f & 1) == 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gUnk_03002490->unk7B & 1) != 0 && (gUnk_03002490->unk7B & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->unk54)->unk08;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gUnk_03002490->unk7A & 1) != 0)
            goto s2;
        fn = ((struct ActorHandlers *)a->unk54)->unk04;
    }
    else
    {
        if ((gUnk_03002490->unk7A & 1) == 0)
            goto s2;
        fn = ((struct ActorHandlers *)a->unk54)->unk00;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s2:
    if (r == 1)
        return 1;
    return r;
}

u32 sub_080699a8(void)
{
    struct Task *t;
    struct Task *u;
    s16 y;
    s16 m;
    s16 d;

    if (gUnk_03004CA0[gCurTaskIdx] != -1)
    {
        t = gUnk_03002490;
        if ((t->unk24 & 0xFFFF0000) != 0
         && (t->unk7A & 1) != 0
         && (gUnk_03005550[4] == 1 || gUnk_03005550[4] == 2
          || gUnk_03005550[4] == 3 || gUnk_03005550[4] == 4))
        {
            if (t->unk54 >= 0)
            {
                m = t->unk48 - 16;
                y = m | 0xF;
            }
            else
            {
                m = t->unk48 + 16;
                y = m & 0xFFF0;
            }
            u = gUnk_03002490;
            d = y - u->unk48;
            u->unk48 = y + d;
            u->unk4A = u->unk24;
            u->unk4C = u->unk48 << 16;
            u->unk50 = u->unk4A << 16;
            return 1;
        }
    }
    return 0;
}

void sub_08069a64(struct InputState *out)
{
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    out->unk01 = ((struct InputState *)a->unk50)->unk01;
    out->unk02 = ((struct InputState *)a->unk50)->unk02;
    out->unk03 = ((struct InputState *)a->unk50)->unk03;
    if (t->unk43 == -1)
    {
        out->unk00 = -((struct InputState *)a->unk50)->unk00;
        out->unk04 = -((struct InputState *)a->unk50)->unk05;
        out->unk05 = -((struct InputState *)a->unk50)->unk04;
    }
    else
    {
        out->unk00 = ((struct InputState *)a->unk50)->unk00;
        out->unk04 = ((struct InputState *)a->unk50)->unk04;
        out->unk05 = ((struct InputState *)a->unk50)->unk05;
    }
}

void sub_08069ac4(s32 i)
{
    struct Task *t;

    t = &gUnk_03002790[i];
    sub_080224f8(i);
    t->unk7A = 1;
}

u32 sub_08069ae4(s8 a)
{
    u32 r;

    r = 0;
    switch (a)
    {
    case 1:
        r = sub_0806a25c();
        break;
    case 3:
        r = sub_0806bb7c();
        break;
    case 4:
        r = sub_0806bb7c();
        break;
    case 2:
    case 5:
        r = sub_08069ea0();
        break;
    case 6:
    case 7:
    case 8:
        break;
    }
    return r;
}

u32 sub_08069b44(void)
{
    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    return sub_08069ae4(gUnk_03002490->unk7C);
}

u32 sub_08069b84(void)
{
    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    return sub_08069ae4(sub_08069c48());
}

u32 sub_08069bbc(void)
{
    u32 r;

    if (gUnk_03004CA0[gCurTaskIdx] == -1)
        return 0;
    gUnk_03002490->unk43 = 1;
    r = 0;
    switch ((s8)gUnk_03002490->unk7C)
    {
    case 1:
    case 2:
    case 5:
    case 7:
        /* sub_0806a158 is void in the ROM but its result is consumed here:
           the original had no prototype in scope at this point, so the
           implicit `int ()` declaration was used.  The cast reproduces the
           direct `bl` without tripping -Wimplicit -Werror. */
        r = ((u32 (*)(void))sub_0806a158)();
        break;
    case 3:
    case 4:
        r = sub_0806bb7c();
        break;
    case 6:
    case 8:
        break;
    }
    return r;
}

s8 sub_08069c48(void)
{
    struct Task *t;
    u8 *g;
    u8 v;
    s32 c;

    t = gUnk_03002490;
    if ((s8)t->unk7C != 0)
    {
        v = t->unk7C;
    }
    else
    {
        g = gUnk_03005550;
        if (g[12] != 0)
        {
            v = g[12];
            c = ((s8 *)g)[12];
            t->unk7C = c;
            gUnk_03002490->unk82 = 0;
        }
        else
        {
            v = 0;
        }
    }
    return v;
}

void sub_08069c8c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk8C->unk06 == 16 || t->unk8C->unk06 == 32)
    {
        u = &gUnk_03002790[t->unk7E];
        switch (u->unk80)
        {
        case 3:
            sub_080031b8(145);
            break;
        case 4:
            sub_080031b8(146);
            break;
        case 9:
            sub_080031b8(132);
            break;
        case 12:
            sub_080031b8(139);
            break;
        case 13:
        case 14:
            sub_080031b8(142);
            break;
        case 0:
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
        case 8:
        case 10:
        case 11:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
            sub_08069d78();
            break;
        }
    }
    else
    {
        sub_08069d78();
    }
}

void sub_08069d78(void)
{
    switch (gUnk_03002490->unk72)
    {
    case 0:
    case 3:
    case 4:
    case 5:
        sub_080031b8(127);
        break;
    case 1:
    case 2:
        sub_080031b8(508);
        break;
    }
}

void sub_08069dc4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    sub_080055b0(7, gCurTaskIdx);
    u = gUnk_03002490;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    a->unk14 = u->unk3C;
    sub_08006338(0);
    gUnk_03002490->unk08 = (u32)sub_08069fb0;
    a->unk01 = 11;
    v = gUnk_03002490;
    v->unk8C->unk22 = v->unk40 & 0xF000;
    if (v->unk82 > 3)
        v->unk82 = 0;
    sub_0806df28(gUnk_03002490->unk82, 1);
}

void sub_08069e48(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    sub_080055b0(0, gCurTaskIdx);
    u = gUnk_03002490;
    u->unk08 = 0;
    u->unk48 = u->unk4C >> 16;
    u->unk4A = u->unk50 >> 16;
    u->unk3C = a->unk14;
    u->unk40 = (u->unk40 & 0xFFF) | u->unk8C->unk22;
}

u32 sub_08069ea0(void)
{
    struct Actor *a;
    struct ActorVt *p;
    u8 r;

    a = gUnk_03002490->unk8C;
    p = (struct ActorVt *)a->unk5C;
    r = 0;
    sub_08069c8c();
    if ((gUnk_03002490->unk72 == 1 || gUnk_03002490->unk72 == 2) && a->unk05 != 2)
        sub_0800a42c();
    if (p != NULL)
    {
        if (p->unk00 != -1)
        {
            sub_08069dc4();
            r = 0;
        }
        else if (p->unk04 != 0)
        {
            r = ((u8 (*)(void))p->unk04)();
        }
        else
        {
            sub_0806ee2c();
        }
    }
    else
    {
        sub_0806ee2c();
    }
    return r;
}

void sub_08069f0c(void)
{
    struct Task *t;
    struct Actor *a;
    s16 j;

    t = gUnk_03002490;
    a = t->unk8C;
    j = gUnk_0873E5A4[(s8)a->unk01] * 2;
    t->unk48 += gUnk_0873E58C[j];
    t->unk4A += gUnk_0873E58C[j + 1];
    if ((s8)--a->unk01 < 0)
        sub_08069e48();
}

void sub_08069f70(void)
{
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    if ((a->unk01 & 1) == 0)
        t->unk40 = (t->unk40 & 0xFFF) | 0xF000;
    else
        t->unk40 = (t->unk40 & 0xFFF) | a->unk22;
}

void sub_08069fb0(void)
{
    sub_08068e04();
    sub_08069b44();
    sub_08069f70();
    sub_08069f0c();
}

void sub_08069fc8(void)
{
    if (gUnk_03004CA0[gCurTaskIdx] == 107 || gUnk_03004CA0[gCurTaskIdx] == 109
     || gUnk_03004CA0[gCurTaskIdx] == 137)
        sub_0806d77c();
    else
        sub_0806d65c();
}

void sub_0806a008(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk72 == 1 || t->unk72 == 2)
        gUnk_03002490->unk43 = sub_08063dac(t->unk7F);
}

s16 sub_0806a03c(void)
{
    s16 r;

    r = 0;
    switch ((s8)gUnk_03002490->unk7D)
    {
    case 7:
        r += 64;
    case 6:
        r += 64;
    case 5:
        r += 64;
    case 4:
        r += 64;
    case 3:
        r += 64;
    case 2:
        r += 64;
    case 1:
        r += 64;
    case 0:
        break;
    }
    return r;
}

void sub_0806a0cc(void)
{
    u32 v;

    switch (sub_08002ee8(3))
    {
    default:
        v = 167;
        break;
    case 0:
        v = 109;
        break;
    case 1:
        v = 166;
        break;
    }
    sub_08065e1c(v, 0);
}

void sub_0806a0f0(s32 a)
{
    struct Task *t;
    struct Actor *b;

    t = gUnk_03002490;
    b = t->unk8C;
    if (a == -2)
        t->unk18 = 0;
    else
        t->unk18 = a;
    b->unk05 = 2;
    sub_08063990((u32)gUnk_0873F910);
    if (gUnk_03002490->unk7A & 1)
        sub_0806395c(1);
    else
        sub_0806395c(0);
    sub_08006148(sub_0806b26c, gCurTaskIdx);
}

/* No return value: the ROM's epilogue is `pop {r0}; bx r0`.  Its caller
   sub_08069bbc nevertheless propagates whatever r0 holds - see the comment
   there. */
void sub_0806a158(void)
{
    struct Task *t;

    if (gUnk_03002490->unk76 != 0)
        sub_080b54d0(gCurTaskIdx);
    t = gUnk_03002490;
    switch (t->unk76)
    {
    case 1:
        if (gUnk_03002360 == t->unk7E)
            sub_080031b8(220);
        sub_08009e60(1, gUnk_03002490->unk7E);
        sub_08063fe0();
        break;
    case 3:
        if (gUnk_03002360 == t->unk7E)
            sub_080031b8(198);
        sub_0804087c(gUnk_03002490->unk7E);
        sub_08063fe0();
        break;
    case 2:
        if (gUnk_03002360 == t->unk7E)
            sub_080031b8(198);
        sub_0806395c(0);
        sub_08006148(sub_080b4240, gCurTaskIdx);
        break;
    case 4:
        if (gUnk_03002360 == t->unk7E)
            sub_080031b8(198);
        sub_0806395c(1);
        sub_08006148(sub_080b4240, gCurTaskIdx);
        break;
    default:
        sub_08063fe0();
        break;
    }
}

u32 sub_0806a25c(void)
{
    struct Actor *a;
    struct ActorVt *p;
    struct Task *t;
    u8 r;

    a = gUnk_03002490->unk8C;
    p = (struct ActorVt *)a->unk5C;
    r = 0;
    sub_08069c8c();
    if (gUnk_03002490->unk72 == 1 || gUnk_03002490->unk72 == 2)
        sub_0800a42c();
    t = gUnk_03002490;
    if (t->unk72 == 1)
        sub_080649b4(t->unk7F, 1);
    else
        sub_080649b4(t->unk7F, 2);
    if (p != NULL)
    {
        if (p->unk01 != -1)
        {
            sub_08006148(sub_0806a344, gCurTaskIdx);
            r = 1;
        }
        else
        {
            if (gUnk_03002490->unk72 == 1)
            {
                if (gUnk_030023D8 != 19)
                    sub_080031b8(510);
                else
                    sub_080031b8(514);
            }
            if (p->unk08 != 0)
                r = ((u8 (*)(void))p->unk08)();
            else
                sub_0806ee2c();
        }
        a->unk05 = 2;
    }
    else
    {
        sub_0806ee2c();
    }
    if (a->unk0D == 0)
        a->unk1A = 0xFFFF;
    return r;
}
