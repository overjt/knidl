#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_509ec.c (0x080509EC-0x080514F7, issue #90).
 *
 * Task type #6, variants 0-2, each variant body followed by the callbacks
 * only it installs.  A variant body installs the sprite (sub_080059d8 /
 * sub_08005f30, Task.unk42 = 5, an animation table in Task.unk38) and runs
 * `switch (Task.unk18 & 15)` over its sub-states, each a yield script that
 * ends in TaskDispatchTrampoline; its per-frame callback (Task.unk04) runs
 * the hit test sub_08030848 and the terrain checks and re-binds the body
 * in another sub-state, or the shared exit sub_08050814, on contact.
 * Variant 0 (sub_080509ec, callback sub_08050c48) spawns a copy of itself
 * in sub-state 1 while it moves; variants 1 and 2 (sub_08050d00,
 * sub_08051124) have the collision callbacks sub_08050e84/sub_080512f8
 * (sound 125 on contact) and the Task.unk08 callbacks sub_08050f80/
 * sub_080513d4, which draw a six-step trail behind the object
 * (sub_08050f80 is also installed by variants 11 and 12). */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gUnk_03005550` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_0874C568[];
extern s8 gUnk_0873CB44[];              /* collision box passed to sub_0802205c / sub_0801c230 */
extern u32 gUnk_0873BD64[];             /* collider row passed to sub_0801a828 (4th arg) */
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873CB84[];
extern u32 gUnk_0874C44C[];
extern s8 gUnk_0873CB4C[];
extern u32 gUnk_0873CB94[];
extern u32 gUnk_0873BD78[];
extern u8 gUnk_02000020;
extern s16 gUnk_03002348;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_0874C478[];
extern u32 gUnk_0873CBA4[];
extern u32 gUnk_0873BD8C[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void sub_080059d8(void);
void sub_08005f30(void);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_08006214(void);
void sub_080062c4(void);
u32 sub_0800641c(s16 a, s16 b);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0801c230(const s8 *p);
void sub_0802205c(s8 *box);
s32 sub_08030848(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/sub_08050c48.c); landed M09/M12/M13 files spell it u16 */
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_08050814(void);

s32 sub_08053940(s8 player, u8 variant, s32 arg);
void sub_08050c48(void);
void sub_08050e84(void);
void sub_08050f80(void);
void sub_080512f8(void);
void sub_080513d4(void);

void sub_080509ec(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_08005f30;
        t->unk42 = 5;
    }
    gUnk_03002490->unk38 = gUnk_0874C568;
    sub_0802205c(gUnk_0873CB44);
    switch (gUnk_03002490->unk18 & 15)
    {
    case 0:
    {
        struct Task *t = gUnk_03002490;
        t->unk04 = (u32)sub_08050c48;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 8) << 16;
        else
            t->unk4C = (t->unk48 - 8) << 16;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk50 = (t->unk4A + 2) << 16;
        sub_0803e34c(114, t->unk44);
    }
        sub_080061c0(0x3C000, -0x2000);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
        {
            sub_0800617c(0);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            sub_08053940(gUnk_03002490->unk88->unk00, 0, 1);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(2);
            sub_0800617c(4);
            TaskYieldTrampoline(2);
        }
        sub_08006214();
        gUnk_03002490->unk04 = 0;
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(2);
        sub_0800617c(4);
        TaskYieldTrampoline(1);
        sub_0800617c(10);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        break;
    case 1:
        sub_080061c0(-0x10000, 0x5A5A5A5A);
        sub_0800617c(6);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        break;
    case 2:
        sub_080062c4();
        {
            struct Task *t = gUnk_03002490;
            t->unk04 = 0;
            sub_0801a828(gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873BD64);
        }
        gUnk_03002490->unk3C = 26;
        TaskYieldTrampoline(1);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 10; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        }
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08050c48(void)
{
    if (sub_08030848((struct HitBoxSet *)gUnk_0873CB84, gUnk_03002490->unk44))
        gUnk_03002490->unk7C = 1;
    else
        sub_0801c230(gUnk_0873CB44);
    if (gUnk_03005550.unk1 != 0 || (gUnk_03002490->unk7A & 1) || gUnk_03005550.unk0 != 0
        || gUnk_03002490->unk7C != 0)
    {
        struct Task *t = gUnk_03002490;
        t->unk18 = (t->unk18 & ~15) | 2;
        sub_08006148(sub_080509ec, gCurTaskIdx);
    }
    sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BD64);
}

void sub_08050d00(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_08005f30;
        t->unk04 = (u32)sub_08050e84;
        t->unk42 = 5;
    }
    gUnk_03002490->unk38 = gUnk_0874C44C;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gUnk_03002490;
        t->unk50 = (t->unk4A + 4) << 16;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 8) << 16;
        else
            t->unk4C = (t->unk48 - 8) << 16;
    }
    sub_0803e34c(105, gUnk_03002490->unk44);
    {
        struct Task *t = gUnk_03002490;
        if (!(t->unk7B & 1))
        {
            t->unk28 = 0;
            if (t->unk43 == 1)
                t->unk2C = -0x40000;
            else
                t->unk2C = 0x40000;
            gUnk_03002490->unk30 = 0;
            sub_080061c0(0x40000, 0x5A5A5A5A);
            while (1)
            {
                gUnk_03002490->unk3C = 4;
                TaskYieldTrampoline(3);
                gUnk_03002490->unk08 = (u32)sub_08050f80;
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
    }
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk60 = 0x400;
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
    {
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08050e84(void)
{
    s32 hit;

    if (sub_08030848((struct HitBoxSet *)gUnk_0873CB94, gUnk_03002490->unk44))
        gUnk_03002490->unk7C = 1;
    else
        sub_0801c230(gUnk_0873CB4C);
    hit = 0;
    {
        struct Task *t = gUnk_03002490;
        if ((t->unk7A & 1) || *(u16 *)&gUnk_03005550 != 0 || t->unk7C != 0)
            hit++;
        else if ((t->unk7B & 1) && t->unk28 != 0)
            hit = 1;
    }
    if (hit)
    {
        struct Task *t;
        sub_08006148(sub_08050814, gCurTaskIdx);
        t = gUnk_03002490;
        t->unk24 = (s32)gUnk_0873BD78;
        if (gUnk_03005550.unk1 != 0 || (t->unk7A & 1) || gUnk_03005550.unk0 != 0)
            sub_0803e34c(125, gUnk_03002490->unk44);
    }
    sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BD78);
}

void sub_08050f80(void)
{
    s32 dx;
    s32 x, y;
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk43 == 1)
        dx = -8;
    else
        dx = 8;
    switch (t->unk28)
    {
    case 0:
    case 1:
        if (gUnk_02000020 == 2 || gUnk_02000020 == 3)
            gUnk_03002490->unk34 = 4;
        else
            gUnk_03002490->unk34 = 8;
        break;
    case 2:
    case 3:
        if (gUnk_02000020 == 2 || gUnk_02000020 == 3)
            gUnk_03002490->unk34 = 5;
        else
            gUnk_03002490->unk34 = 9;
        break;
    case 4:
    case 5:
        if (gUnk_02000020 == 2 || gUnk_02000020 == 3)
            gUnk_03002490->unk34 = 6;
        else
            gUnk_03002490->unk34 = 10;
        break;
    }
    t = gUnk_03002490;
    if (t->unk28++ > 5)
    {
        t->unk28 = 0;
        if (t->unk43 == 1)
            t->unk2C = -0x40000;
        else
            t->unk2C = 0x40000;
        gUnk_03002490->unk30 = 0;
    }
    else
    {
        if (t->unk43 == 1)
            t->unk30 += -0x10000;
        else
            t->unk30 += 0x10000;
        gUnk_03002490->unk2C += gUnk_03002490->unk30;
    }
    t = gUnk_03002490;
    if (t->unk88->unk37 != 2)
    {
        x = t->unk48 + dx + ((s16 *)&t->unk2C)[1];
        y = t->unk4A;
        if (!sub_0800641c(x, y))
            return;
        x -= gUnk_03002348;
        y -= gUnk_030023E4;
    }
    else
    {
        x = t->unk48 + dx + ((s16 *)&t->unk2C)[1];
        y = t->unk4A;
    }
    {
        u32 *tbl = gUnk_03002490->unk38;
        sub_08001a94(gUnk_03002490->unk42, tbl[gUnk_03002490->unk34], 0, 0, x, y);
    }
}

void sub_08051124(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005f30;
    gUnk_03002490->unk04 = (u32)sub_080512f8;
    gUnk_03002490->unk08 = (u32)sub_080513d4;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_0874C478;
    sub_0802205c(gUnk_0873CB4C);
    {
        struct Task *t = gUnk_03002490;
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 8) << 16;
        else
            t->unk4C = (t->unk48 - 8) << 16;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk50 = (t->unk4A + 4) << 16;
        t->unk28 = 6;
        t->unk34 = 0;
        sub_0803e34c(106, t->unk44);
    }
    sub_080061c0(0x40000, 0x5A5A5A5A);
    while (1)
    {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 18;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 19;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 22;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 23;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(2);
    }
}

void sub_080512f8(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk78 = 127;
        sub_08030848((struct HitBoxSet *)gUnk_0873CBA4, t->unk44);
    }
    sub_0801c230(gUnk_0873CB4C);
    {
        struct Task *t = gUnk_03002490;
        if ((t->unk7A & 1) || *(u16 *)&gUnk_03005550 != 0)
        {
            sub_0803e34c(125, t->unk44);
            sub_08006148(sub_08050814, gCurTaskIdx);
            gUnk_03002490->unk24 = (s32)gUnk_0873BD8C;
        }
        else if (t->unk7B & 1)
        {
            if (t->unk54 & 0xFFFF0000)
                sub_080061c0(0x5A5A5A5A, -0x1800);
            gUnk_03002490->unk60 = 0x1000;
        }
    }
    sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BD8C);
}

void sub_080513d4(void)
{
    struct Task *t;

    switch (gUnk_03002490->unk28)
    {
    case 0:
        {
            struct Task *u = gUnk_03002490;
            if (u->unk43 == 1)
                u->unk30 = u->unk48 - 8;
            else
                u->unk30 = u->unk48 + 8;
        }
    case 1:
        gUnk_03002490->unk2C = 24;
        break;
    case 2:
    case 3:
        gUnk_03002490->unk2C = 25;
        break;
    case 4:
    case 5:
        gUnk_03002490->unk2C = 26;
        break;
    case 6:
    case 7:
        {
            struct Task *v = gUnk_03002490;
            v->unk2C = -1;
            v->unk34 = 0;
        }
        break;
    }
    t = gUnk_03002490;
    t->unk28 = (t->unk28 + 1) & 7;
    if (t->unk2C != -1)
    {
        if (t->unk43 == 1)
            t->unk34--;
        else
            t->unk34++;
        if (sub_0800641c(gUnk_03002490->unk30 + gUnk_03002490->unk34, gUnk_03002490->unk4A))
        {
            u32 *tbl = gUnk_03002490->unk38;
            sub_08001a94(gUnk_03002490->unk42, tbl[gUnk_03002490->unk2C], 0, 0,
                         gUnk_03002490->unk30 + gUnk_03002490->unk34 - gUnk_03002348,
                         gUnk_03002490->unk4A - gUnk_030023E4);
        }
    }
}
