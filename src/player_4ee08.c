#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4ee08.c (0x0804EE08-0x0804F613, issue #90).
 *
 * Action 49's sub-actions 4-8 (gUnk_0873B664) and sub-handlers 13-17
 * (gUnk_0873B688), each sub-action followed by its sub-handler: more
 * attacks of the same move set (sounds 168-170, 183, 184, 202 and 246,
 * M11's sub_080413a4 steering, effect 44), the last one (sub_0804f450)
 * installing the hit boxes gUnk_0873BD00/gUnk_0873CB1C in PlayerState.
 * Sub-handler 17 (sub_0804f5bc, a push-less leaf) requests action 7, 20
 * or 23 from the ground flag and the key state. */

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

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern struct Unk03005550 gUnk_03005550;
extern u32 gUnk_0873BD00[];             /* stored to PlayerState.unk64 as (u32)gUnk_0873BD00 */
extern u32 gUnk_0873CB1C[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_080261d4(u16 a);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e3e4(s32 a0);
void sub_0803f870(void);
void sub_0803f8e8(void);
s32 sub_0803fce4(s32 a);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_0804e600(void);
s32 sub_0804f614(void);
void sub_0804f79c(void);
s32 sub_0804f7f8(s32 a);
s32 sub_0804f8ec(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804ee08(void)
{
    s32 i;

    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        t->unk88->unk14 = 0xFFFF;
        t->unk88->unk3D = 1;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = t->unk46;
        t->unk46 = -1;
        t->unk3C = 0xCE9;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0xCE7;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gUnk_03002490;
        t->unk46 = t->unk28;
        sub_0803e34c(170, (u16)t->unk88->unk00);
    }
    sub_080413a4(43);
    gUnk_03002490->unk88->unk14 = 23;
    while (1)
    {
        if (--gUnk_03002490->unk88->unk14 == 0 || !(gUnk_03002458[gUnk_03002490->unk88->unk00] & 1))
            break;
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
    sub_080413a4(44);
    for (i = 4; i >= 0; i--)
    {
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk70++;
    sub_0803e050(2);
    sub_08006138();
}

void sub_0804ef00(void)
{
    if ((s16)gUnk_03002490->unk88->unk14 == -1)
        return;
    sub_0803f870();
    while (!sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gUnk_03005550.unk1 != 0 || (gUnk_03002490->unk88->unk48 & 4))
        {
            struct Task *t;
            sub_0803f8e8();
            t = gUnk_03002490;
            if (t->unk88->unk3E == 1)
            {
                t->unk20 = t->unk58;
                t->unk58 = 0;
            }
            else
            {
                t->unk58 = -t->unk58;
                t->unk68 = 0x50000;
            }
            gUnk_03002490->unk73 = 6;
        }
        else
        {
            if (gUnk_03005550.unk0 != 0)
            {
                struct Task *t = gUnk_03002490;
                t->unk54 = -t->unk54;
                t->unk5C = -t->unk5C;
                t->unk43 = -t->unk43;
            }
            if ((s16)gUnk_03002490->unk70 != 0)
                gUnk_03002490->unk73 = 6;
        }
        break;
    }
    if (gUnk_03002490->unk73 != 4)
        sub_08006148(sub_0804e600, gCurTaskIdx);
    sub_08040b40(12, 0);
}

void sub_0804efec(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = t->unk58;
        t->unk88->unk3D = 1;
    }
    sub_0803e050(2);
    {
        struct Task *t = gUnk_03002490;
        t->unk2C = t->unk46;
        t->unk46 = 0xFFFF;
        t->unk3C = 0xCE9;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0xCE7;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gUnk_03002490;
        t->unk46 = t->unk2C;
        t->unk7A = 0;
    }
    gUnk_03002490->unk58 = gUnk_03002490->unk28;
    sub_080413a4(46);
    {
        struct Task *t = gUnk_03002490;
        if (abs(t->unk28) <= 0x1FFFF)
            sub_0803e34c(184, (u16)t->unk88->unk00);
        else if (abs(t->unk28) <= 0x2FFFF)
            sub_0803e34c(183, (u16)t->unk88->unk00);
        else if (abs(t->unk28) <= 0x3FFFF)
            sub_0803e34c(170, (u16)t->unk88->unk00);
        else if (abs(t->unk28) <= 0x57FFF)
            sub_0803e34c(169, (u16)t->unk88->unk00);
        else
            sub_0803e34c(168, (u16)t->unk88->unk00);
    }
    sub_0804f614();
    while (1)
    {
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
}

void sub_0804f124(void)
{
    if (gUnk_03002490->unk58 == 0)
        return;
    sub_0803f870();
    while (!sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gUnk_03005550.unk1 != 0 || (gUnk_03002490->unk88->unk48 & 4))
        {
            struct Task *t;
            sub_0803f8e8();
            t = gUnk_03002490;
            if (t->unk88->unk3E == 1)
            {
                if (-t->unk58 > 0x57FFF)
                    sub_0803e34c(246, (u16)t->unk88->unk00);
                gUnk_03002490->unk20 = gUnk_03002490->unk58;
                gUnk_03002490->unk58 = 0;
            }
            else
            {
                t->unk58 = -t->unk58;
                t->unk68 = 0x50000;
            }
            gUnk_03002490->unk73 = 6;
        }
        else
        {
            if (gUnk_03005550.unk0 != 0)
            {
                struct Task *t;
                sub_0803f8e8();
                t = gUnk_03002490;
                t->unk54 = -t->unk54;
                t->unk5C = -t->unk5C;
                t->unk43 = -t->unk43;
            }
            if (gUnk_03002490->unk58 >= 0)
                gUnk_03002490->unk73 = 6;
        }
        break;
    }
    if (gUnk_03002490->unk73 != 5)
    {
        struct Task *t = gUnk_03002490;
        t->unk24 = 0;
        sub_08006148(sub_0804e600, gCurTaskIdx);
    }
    sub_08040b40(12, 0);
}

void sub_0804f22c(void)
{
    gUnk_03002490->unk88->unk3D = 1;
    sub_080413a4(45);
    sub_0804f614();
    while (1)
    {
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
}

void sub_0804f258(void)
{
    sub_0803f870();
    if (!sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (sub_0803fce4(0) != 0)
            gUnk_03002490->unk88->unk01 = 23;
        else if (gUnk_03002490->unk7A & 1)
            gUnk_03002490->unk73 = 7;
        else if (gUnk_03005550.unk0 != 0)
        {
            struct Task *t;
            sub_0803f8e8();
            t = gUnk_03002490;
            t->unk54 = -t->unk54;
            t->unk5C = -t->unk5C;
            t->unk43 = -t->unk43;
        }
    }
    if (gUnk_03002490->unk73 != 6)
    {
        struct Task *t = gUnk_03002490;
        t->unk24 = 0;
        sub_08006148(sub_0804e600, gCurTaskIdx);
    }
    sub_08040b40(12, 0);
}

void sub_0804f30c(void)
{
    s32 flag;
    s32 m;

    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        t->unk28 = t->unk58;
    }
    sub_0804f8ec(1);
    flag = 0;
    if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 1)
    {
        flag = 1;
        m = 358;
    }
    else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
    {
        flag = 1;
        m = 76;
    }
    else
        m = 179;
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = m = m * (t->unk28 >> 8);
        if (m <= 0x3FFF)
            flag = 0;
        if (m > 0x57FFF)
            sub_0803e34c(246, (u16)t->unk88->unk00);
    }
    if (flag)
    {
        struct Task *t = gUnk_03002490;
        t->unk24 = 1;
        t->unk3C = 0xCE8;
        TaskYieldTrampoline(2);
    }
    else
    {
        struct Task *t = gUnk_03002490;
        if (t->unk28 <= 0x17FFF)
            t->unk28 = 0;
    }
    gUnk_03002490->unk70++;
    sub_08006138();
}

void sub_0804f3e4(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        t->unk24 = 0;
        if (t->unk28 != 0)
        {
            t->unk73 = 5;
            if ((u32)gUnk_03002490->unk28 > 0x80000)
                gUnk_03002490->unk28 = 0x80000;
            gUnk_03002490->unk58 = -gUnk_03002490->unk28;
            gUnk_03002490->unk68 = 0x80000;
        }
        else if (t->unk54 != 0)
            t->unk73 = 2;
        else
            t->unk73 = 1;
        sub_08006148(sub_0804e600, gCurTaskIdx);
    }
}

void sub_0804f450(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        t->unk46 = 0xFFFF;
        if (t->unk74 != 0)
        {
            sub_0803e050(3);
            sub_080261d4(0);
            sub_0803e3e4(15);
        }
    }
    if (gUnk_03002490->unk7A & 1)
    {
        gUnk_03002490->unk3C = 0xCE9;
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk74 == 0)
            sub_080413a4(48);
        gUnk_03002490->unk3C = 0xCE7;
        TaskYieldTrampoline(3);
    }
    gUnk_03002490->unk3C = 0xCD2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x500);
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x501);
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x502);
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x503);
    sub_0803e34c(202, (u16)gUnk_03002490->unk88->unk00);
    {
        struct Task *t = gUnk_03002490;
        t->unk43 = t->unk6E;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk88->unk64 = (u32)gUnk_0873BD00;
        t->unk88->unk68 = (u32)gUnk_0873CB1C;
        t->unk70++;
    }
    sub_08006138();
}

void sub_0804f5bc(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        if (t->unk74 != 0)
            t->unk88->unk01 = 20;
        else
            t->unk88->unk01 = 7;
    }
    if (gUnk_03002490->unk88->unk01 == 0 && (gUnk_03002490->unk7B & 1))
        gUnk_03002490->unk88->unk01 = 23;
}
