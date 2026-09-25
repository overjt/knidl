#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4f948.c (0x0804F948-0x0804FFDB, issue #90).
 *
 * Player action bodies, part 26: actions 56-58 and per-frame handlers
 * 53-55.  sub_0804f948 (action 56, mode 13) installs the collider row
 * gUnk_0873C2C8, plays sound 155 and animations 0x1026/0x102A and spawns
 * task type #6's variant 11 through sub_08053a44; its handler sub_0804fab0
 * re-binds it and requests action 2 once the animation ends.
 * sub_0804fba4 (action 57, mode 13) plays animation 0xFE5 with the collider
 * row gUnk_0873C304; its handler sub_0804fc98 is a `switch (Task.unk73)`
 * with M11's steering and camera presets.  Action 58 is a second move set
 * one level down: sub_0804fe68 (mode 13) dispatches Task.unk73 through its
 * four sub-actions gUnk_0873B6AC and sub_0804fee8 is the re-entry callback;
 * its handler sub_0804ff1c runs the sub-handler Task.unk73 of
 * gUnk_0873B6BC, the steering helper sub_080506dc, clamps the player to
 * 16-224 x 18-132 and, once gUnk_030023E4 passes 888, subtracts the
 * player's whole health (sub_08009ee8) and requests action 17. */

struct M11R20 { u32 w[5]; };

extern struct M11R20 gUnk_020060E0[];
extern u32 gUnk_0873C2C8[];
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern u32 gUnk_0873C2DC[];
extern u32 gUnk_0873C304[];
extern u32 gUnk_0873C318[];
extern void (*gUnk_0873B6AC[])(void);   /* enter 58's sub-actions [4] */
extern void (*gUnk_0873B6BC[])(void);   /* handler 55's per-frame sub-handlers [4] */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern s16 gUnk_02005588[];             /* per-player health (M02's HUD) */

void TaskYieldTrampoline(s32 frames);
/* task / sprite services (landed prototypes) */
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
s32 sub_08009ee8(s32 a, s32 b);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e4ec(s32 a0);
s32 sub_0803e55c(void);
s32 sub_0803e5c0(s32 playerIdx, u8 *src6);
void sub_0803f870(void);
void sub_0803f8e8(void);
void sub_0803f9c0(void);
s32 sub_0803ffe0(void);
s32 sub_080400c0(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_080506dc(void);
s32 sub_08053a44(s8 player, u8 variant, s32 arg);

void sub_0804f948(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 53;
    gUnk_03002490->unk80 = 25;
    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        t->unk2C = -1;
        gUnk_020060E0[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873C2C8;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = 0;
        sub_0803e34c(155, (u16)t->unk88->unk00);
    }
    sub_08006338(0x1026);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    sub_08006338(0x102A);
    TaskYieldTrampoline(1);
    sub_08053a44(gUnk_03002490->unk88->unk00, 11, 0);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
    {
        struct Task *t = gUnk_03002490;
        t->unk2C++;
        t->unk3C++;
        TaskYieldTrampoline(1);
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk2C = -1;
        t->unk3C++;
    }
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;
        t->unk28++;
        t->unk3C++;
    }
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk70++;
    sub_08006138();
}

void sub_0804fab0(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        sub_0804042c();
    }
    else
    {
        if (t->unk28 != 0)
        {
            if (gUnk_030023C0[t->unk88->unk00] & 2)
            {
                sub_08006148(sub_0804f948, gCurTaskIdx);
            }
            else if ((t->unk7A & 1) && (gUnk_03002458[t->unk88->unk00] & 0x30))
            {
                sub_0803f870();
                gUnk_03002490->unk88->unk01 = 2;
            }
        }
        {
            struct Task *u = gUnk_03002490;
            if (u->unk2C != -1)
            {
                sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873C2DC + u->unk2C * 8);
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                             &gUnk_020060E0[gUnk_03002490->unk88->unk00]);
            }
        }
    }
    sub_0803e55c();
}

void sub_0804fba4(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 54;
    gUnk_03002490->unk73 = 0;
    gUnk_03002490->unk88->unk14 = 23;
    sub_080413a4(0);
    sub_0803e34c(100, (u16)gUnk_03002490->unk88->unk00);
    {
        struct Task *t = gUnk_03002490;
        t->unk2C = t->unk43;
        t->unk80 = 25;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = -1;
        gUnk_020060E0[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873C304;
    }
    while (1)
    {
        {
            struct Task *t = gUnk_03002490;
            t->unk28++;
            sub_0803e34c(0x11D, (u16)t->unk88->unk00);
        }
        sub_08006338(0xFE5);
        TaskYieldTrampoline(1);
        for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 6; gUnk_03002490->unk6E++)
        {
            struct Task *t = gUnk_03002490;
            t->unk28++;
            t->unk3C++;
            TaskYieldTrampoline(1);
        }
        gUnk_03002490->unk28 = -1;
    }
}

void sub_0804fc98(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73)
    {
    case 0:
        if (--t->unk88->unk14 == 0 || !(gUnk_03002458[t->unk88->unk00] & 1))
        {
            t->unk73 = 1;
            sub_080413a4(1);
            gUnk_03002490->unk88->unk14 = 5;
        }
        break;
    case 1:
        if (--t->unk88->unk14 == 0)
        {
            sub_0803e050(2);
            sub_080413a4(2);
            gUnk_03002490->unk73 = 2;
        }
        break;
    case 2:
        break;
    }
    if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x30)
    {
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x10)
            gUnk_03002490->unk2C = 1;
        else
            gUnk_03002490->unk2C = -1;
    }
    if (sub_0803ffe0() == 0)
    {
        if (sub_080400c0() == 0)
        {
            if (gUnk_03002490->unk7A & 1)
            {
                sub_0803f8e8();
                sub_0804042c();
                gUnk_03002490->unk43 = gUnk_03002490->unk2C;
            }
        }
        else
        {
            gUnk_03002490->unk43 = gUnk_03002490->unk2C;
        }
    }
    {
        struct Task *u = gUnk_03002490;
        if (u->unk28 != -1)
        {
            sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873C318 + u->unk28 * 8);
            sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                         &gUnk_020060E0[gUnk_03002490->unk88->unk00]);
        }
    }
    if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x30)
    {
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x10)
            sub_08040b40(10, 70);
        else
            sub_08040b40(10, 71);
    }
    else
    {
        sub_08040b40(11, 2);
    }
    if (!(gUnk_03002490->unk7A & 1))
        sub_080413a4(2);
    else
        sub_0803e4ec(1);
    sub_0803f9c0();
}

void sub_0804fe68(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 55;
    if (gUnk_03002490->unk88->unk05 != 13)
    {
        gUnk_03002490->unk88->unk3D = 0;
        gUnk_03002490->unk88->unk14 = 0;
        gUnk_03002490->unk88->unk10 = 0;
        sub_0803e050(3);
        gUnk_03002490->unk73 = 0;
        gUnk_03002490->unk80 = 0;
        gUnk_03002490->unk08 = 0;
        gUnk_03002490->unk88->unk64 = 0;
        gUnk_03002490->unk88->unk68 = 0;
        gUnk_03002490->unk88->unk6C = 0;
    }
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_0873B6AC);
}

void sub_0804fee8(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_0873B6AC);
}

void sub_0804ff1c(void)
{
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_0873B6BC);
    sub_080506dc();
    {
        struct Task *t = gUnk_03002490;
        if (t->unk48 < 16)
        {
            t->unk48 = 16;
            t->unk4C = 16 << 16;
        }
        else if (t->unk48 > 224)
        {
            t->unk48 = 224;
            t->unk4C = 224 << 16;
        }
    }
    {
        struct Task *t = gUnk_03002490;
        if (t->unk4A < 18)
        {
            t->unk4A = 18;
            t->unk50 = 18 << 16;
        }
        else if (t->unk4A > 132)
        {
            t->unk4A = 132;
            t->unk50 = 132 << 16;
        }
    }
    if (gUnk_030023E4 > 888)
    {
        gUnk_03002490->unk7C = 1;
        sub_08009ee8(-gUnk_02005588[gUnk_03002490->unk88->unk00], gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk01 = 17;
    }
}
