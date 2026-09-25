#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4e78c.c (0x0804E78C-0x0804EE07, issue #90).
 *
 * Action 49's sub-actions 0-3 (gUnk_0873B664) and sub-handlers 9-12
 * (gUnk_0873B688), each sub-action followed by its sub-handler.  The
 * sub-actions are yield scripts (sub_0804e78c: sound 171, animations
 * 0xCCA-0xCDA and effect 44 four times; sub_0804eca4: sound 119,
 * effect 6 and camera preset sub_08040b40(11, 62)); the sub-handlers read
 * the keys and the ground flag Task.unk7A, pick the next sub-action
 * (Task.unk73) and re-bind it through sub_0804e600, most of them through
 * the helper sub_0804f7f8's four key probes. */

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

extern u32 gUnk_0873BD50[];
extern u32 gUnk_0873CB3C[];
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern struct Unk03005550 gUnk_03005550;

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e414(void);
s32 sub_0803e448(void);
void sub_0803f8e8(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);

void sub_08049738(void);
void sub_0804e600(void);
s32 sub_0804f614(void);
s32 sub_0804f76c(void);
void sub_0804f79c(void);
s32 sub_0804f7f8(s32 a);
s32 sub_0804f8ec(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804e78c(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk70 = 0;
        t->unk46 = 0xFFFF;
        t->unk88->unk64 = (u32)gUnk_0873BD50;
        t->unk88->unk68 = (u32)gUnk_0873CB3C;
        sub_0803e34c(171, (u16)t->unk88->unk00);
    }
    if (gUnk_03002490->unk7A & 1)
    {
        sub_080413a4(47);
        sub_08006338(0xCCA);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
    sub_08006338(0xCCC);
    TaskYieldTrampoline(2);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
    gUnk_03002490->unk3C = 0xCDA;
    TaskYieldTrampoline(2);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 5; gUnk_03002490->unk6C++)
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk6E = t->unk43;
        t->unk70++;
        sub_0805afac(t->unk88->unk00, 44, 0x100);
    }
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x200);
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x300);
    sub_0805afac(gUnk_03002490->unk88->unk00, 44, 0x400);
    sub_08049738();
    sub_08006138();
}

void sub_0804e8f4(void)
{
    struct Task *t = gUnk_03002490;

    if ((s16)t->unk70 != 0)
    {
        if (t->unk7A & 1)
        {
            if (gUnk_03002458[t->unk88->unk00] & 1)
                t->unk73 = 4;
            else
                t->unk73 = 1;
        }
        else
            t->unk73 = 6;
        gUnk_03002490->unk46 = 0;
        sub_08006148(sub_0804e600, gCurTaskIdx);
    }
    sub_0804f8ec(0);
}

void sub_0804e97c(void)
{
    sub_0803e050(3);
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = t->unk88->unk4A;
    }
    sub_0804f614();
    {
        struct Task *t = gUnk_03002490;
        if (t->unk46 == 0)
            t->unk2C = 0;
        else if (t->unk46 <= 7)
            t->unk2C = -1;
        else
            t->unk2C = 1;
    }
    if (gUnk_03002490->unk46 != 0)
    {
        do
        {
            struct Task *t = gUnk_03002490;
            t->unk46 += t->unk2C;
            if (t->unk46 > 15)
                t->unk46 = 0;
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk46 != 0);
    }
    while (1)
    {
        gUnk_03002490->unk46 = 16;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk46--;
        TaskYieldTrampoline(3);
    }
}

void sub_0804ea7c(void)
{
    sub_0803e448();
    while (!sub_0804f7f8(0) && !sub_0804f7f8(1) && !sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
        {
            if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 16) && gUnk_03002490->unk28 == 1)
                break;
            if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 32) && gUnk_03002490->unk28 == 2)
                break;
            gUnk_03002490->unk73 = 2;
        }
        break;
    }
    if (gUnk_03002490->unk73 != 1)
    {
        struct Task *t = gUnk_03002490;
        t->unk24 = 0;
        sub_08006148(sub_0804e600, gCurTaskIdx);
    }
}

void sub_0804eb28(void)
{
    sub_0804f614();
    sub_08040b40(12, 1);
    gUnk_03002490->unk28 = sub_0804f76c();
    gUnk_03002490->unk2C = gUnk_03002490->unk88->unk4B;
    while (1)
    {
        sub_0804f79c();
        TaskYieldTrampoline(gUnk_03002490->unk28);
    }
}

void sub_0804eb60(void)
{
    while (!sub_0804f7f8(0) && !sub_0804f7f8(1) && !sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gUnk_03005550.unk0 != 0)
        {
            struct Task *t;
            sub_0803f8e8();
            t = gUnk_03002490;
            t->unk54 = -t->unk54;
            t->unk5C = -t->unk5C;
            t->unk43 = -t->unk43;
            gUnk_03002490->unk24 = 0;
            sub_08006148(sub_0804e600, gCurTaskIdx);
            break;
        }
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16)
        {
            if (gUnk_03002490->unk43 == -1)
            {
                gUnk_03002490->unk73 = 3;
                break;
            }
        }
        else if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 32) && gUnk_03002490->unk43 == 1)
        {
            gUnk_03002490->unk73 = 3;
            break;
        }
        if (gUnk_03002490->unk54 == 0 && gUnk_03002490->unk64 == 0)
        {
            gUnk_03002490->unk73 = 1;
            break;
        }
        if (gUnk_03002490->unk2C != gUnk_03002490->unk88->unk4B)
        {
            gUnk_03002490->unk24 = 0;
            sub_08006148(sub_0804e600, gCurTaskIdx);
        }
        break;
    }
    if (gUnk_03002490->unk73 != 2)
    {
        struct Task *t = gUnk_03002490;
        t->unk24 = 0;
        sub_08006148(sub_0804e600, gCurTaskIdx);
    }
    sub_08040b40(12, 1);
    gUnk_03002490->unk28 = sub_0804f76c();
}

void sub_0804eca4(void)
{
    sub_08040b40(11, 62);
    sub_0803e34c(119, (u16)gUnk_03002490->unk88->unk00);
    gUnk_03002490->unk34 = sub_0805afac(gUnk_03002490->unk88->unk00, 6, 60);
    sub_08006138();
}

void sub_0804ecec(void)
{
    while (!sub_0804f7f8(0) && !sub_0804f7f8(1) && !sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gUnk_03005550.unk0 != 0)
        {
            struct Task *t;
            sub_0803f8e8();
            t = gUnk_03002490;
            t->unk54 = -t->unk54;
            t->unk5C = -t->unk5C;
            t->unk43 = -t->unk43;
            gUnk_03002490->unk73 = 2;
            break;
        }
        if (gUnk_03002490->unk54 == 0)
        {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
            {
                if (gUnk_03002490->unk43 == 1)
                {
                    if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 32)
                        gUnk_03002490->unk43 = -1;
                }
                else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16)
                    gUnk_03002490->unk43 = 1;
                sub_0803e414();
                gUnk_03002490->unk5C = 0;
                gUnk_03002490->unk73 = 2;
            }
            else
                gUnk_03002490->unk73 = 1;
        }
        break;
    }
    if (gUnk_03002490->unk73 != 3)
    {
        sub_08006148(sub_0804e600, gCurTaskIdx);
        gUnk_03002790[gUnk_03002490->unk34].unk28 = -1;
    }
}
