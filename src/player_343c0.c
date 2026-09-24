#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_343c0.c (0x080343C0-0x08034F8B, issue #92).
 *
 * Player mode bodies, part 2.  The player task starts the "enter"
 * coroutine of the requested action PlayerState.unk02 from the anchor
 * table 0x0873A74C (sub_08002e98(unk02, 62, gUnk_0873A748), 1-based) and
 * runs the "per-frame" handler Task.unk15 from 0x0873A844
 * (sub_08002e98(unk15, 57, gUnk_0873A840)).  Here: actions 3-6 and 22.
 * sub_080343c0 enters mode 2 (handler 3, sub_0803469c), sub_08034874
 * mode 3 (handler 4, sub_080349b4), sub_08034a88 and sub_08034d34 mode 4
 * (handlers 5 and 6, sub_08034bec and sub_08034e60); sub_08034f70
 * (action 22) clears PlayerState.unk68 and runs sub_08034f8c, the
 * mode-5 coroutine of the next file.  The enter coroutines switch on the
 * ability PlayerState.unk0D for the animation (sub_08006338) and loop
 * on TaskYieldTrampoline; the handlers run M11's transition predicates
 * and write the next request into PlayerState.unk01. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
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

extern u16 gUnk_03002458[];             /* latched state mask per player (M11) */
extern u16 gUnk_0873D31C[];
extern u16 gUnk_0873D350[];
extern struct Unk03005550 gUnk_03005550;
extern u16 gUnk_0873D384[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
s32 sub_08022624(u16 x, u16 y);
void sub_08034f8c(void);
void sub_0803cd60(void);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e374(s32 a0, u16 a1);
s32 sub_0803e4ec(s32 a0);
void sub_0803f870(void);
void sub_0803f8e8(void);
s32 sub_0803f9e8(void);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fd90(void);
s32 sub_0803fdf4(void);
s32 sub_0803fe68(void);
s32 sub_0803fec4(void);
s32 sub_0803ff7c(void);
s32 sub_0803ffe0(void);
s32 sub_08040084(void);
s32 sub_080400c0(void);
s32 sub_08040298(void);
s32 sub_08040340(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 band, s32 id, s32 payload);

void sub_080343c0(void)
{
    struct Task *t;
    struct PlayerState *p;
    u16 *q;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 2;
    gUnk_03002490->unk15 = 3;
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk2C = -1;
    sub_08040b40(3, 72);
    if (gUnk_03002490->unk88->unk05 != 2)
    {
        gUnk_03002490->unk88->unk46 = 0;
        q = gUnk_03002458;
        p = gUnk_03002490->unk88;
        if (q[p->unk00] & 48)
        {
            if (p->unk3E == 2)
                p->unk3E = 0;
        }
        sub_0803cd60();
        sub_0803e374(117, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 7, 0);
        if (sub_0803fd20(gUnk_03002490->unk88->unk00) == 4)
            gUnk_03002490->unk73 = 1;
        else
            gUnk_03002490->unk73 = 0;
    }
    if (gUnk_03002490->unk73 == 0)
    {
        if (gUnk_03002490->unk88->unk06 == 1)
        {
            while (1)
            {
                sub_08006338(0x153);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C = 0;
                do
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk6C++;
                } while ((s16)gUnk_03002490->unk6C <= 3);
                sub_08006338(0x148);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C = 0;
                do
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk6C++;
                } while ((s16)gUnk_03002490->unk6C <= 9);
            }
        }
        else
        {
            gUnk_03002490->unk46 = gUnk_0873D31C[gUnk_03002490->unk88->unk0D];
            while (1)
            {
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C = 0;
                do
                {
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(3);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk6C++;
                } while ((s16)gUnk_03002490->unk6C <= 2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
    }
    sub_0805afac(gUnk_03002490->unk88->unk00, 6, 0x200);
    if (gUnk_03002490->unk88->unk06 == 1)
        gUnk_03002490->unk46 = 0x15D;
    else
        gUnk_03002490->unk46 = gUnk_0873D350[gUnk_03002490->unk88->unk0D];
    switch (gUnk_03002490->unk88->unk0D)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    }
}

void sub_0803469c(void)
{
    struct Task *t;
    struct Task *t3;
    struct PlayerState *p;
    struct PlayerState *p4;
    u16 *q;
    s32 x;
    s32 m;
    s32 m2;
    s32 m5;
    s32 y;
    s32 m3;

    t = gUnk_03002490;
    if (t->unk28 == 0)
    {
        m2 = gUnk_03005550.unkD;
        if (m2 != 0)
        {
            t->unk88->unk14 = 5;
            t->unk28 = 1;
        }
        else
        {
            t->unk88->unk14 = m2;
        }
    }
    else
    {
        p = t->unk88;
        if ((s16)p->unk14 == 0)
        {
            if (sub_08022624(((u16 *)t)[36],
                             (y = ((u16 *)t)[37], m3 = -16, m3 &= y, m3 + 16)) != 0)
                gUnk_03002490->unk7A = 1;
        }
        else
        {
            p->unk14--;
        }
    }
    t = gUnk_03002490;
    if ((t->unk7A & 1) != 0 || (t->unk88->unk48 & 3) != 0)
    {
        t->unk28 = 0;
        t->unk88->unk14 = 0;
    }
    while (sub_0803fd90() == 0 && sub_0803fdf4() == 0)
    {
        if (gUnk_03002490->unk28 == 0 && sub_0803fe68() != 0)
            break;
        if (sub_08040298() != 0)
            break;
        if (sub_0803ff7c() != 0)
            break;
        if (sub_0803fec4() != 0)
            goto end;
        if (sub_0803ffe0() != 0)
            goto end;
        if (sub_080400c0() != 0)
            goto end;
        if (sub_08040340() != 0)
            goto end;
        q = gUnk_03002458;
        t3 = gUnk_03002490;
        p4 = t3->unk88;
        m = q[p4->unk00] & 48;
        if (m == 0)
        {
            x = abs(t3->unk54);
            if ((u32)x <= 0x14BFF)
            {
                p4->unk3D = m;
                gUnk_03002490->unk88->unk01 = 2;
                goto end;
            }
        }
        m5 = gUnk_03005550.unk0;
        if (m5 != 0)
        {
            sub_0803f8e8();
            gUnk_03002490->unk88->unk01 = 1;
            goto end;
        }
        if (gUnk_03002490->unk73 == 0)
        {
            if (sub_0803fd20(gUnk_03002490->unk88->unk00) == 4)
            {
                gUnk_03002490->unk73 = 1;
                sub_08006148(sub_080343c0, gCurTaskIdx);
            }
        }
        else if (sub_0803fd20(gUnk_03002490->unk88->unk00) != 4)
        {
            gUnk_03002490->unk73 = m5;
            sub_08006148(sub_080343c0, gCurTaskIdx);
        }
        goto end;
    }
end:
    sub_08040b40(3, 72);
}

void sub_08034874(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 3;
    gUnk_03002490->unk15 = 4;
    sub_08040b40(4, 72);
    if (gUnk_03002490->unk88->unk05 != 3)
    {
        sub_0803e34c(119, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 6, 0);
    }
    if (gUnk_03002490->unk88->unk06 == 1)
        gUnk_03002490->unk46 = 0x15D;
    else
        gUnk_03002490->unk46 = gUnk_0873D350[gUnk_03002490->unk88->unk0D];
    switch (gUnk_03002490->unk88->unk0D)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    }
}

void sub_080349b4(void)
{
    while (sub_0803fdf4() == 0 && sub_0803fe68() == 0 && sub_080400c0() == 0 && sub_08040340() == 0)
    {
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
                {
                    gUnk_03002490->unk43 = 1;
                }
                gUnk_03002490->unk5C = 0;
                if (gUnk_03002490->unk88->unk3D == 0)
                    gUnk_03002490->unk88->unk01 = 2;
                else
                    gUnk_03002490->unk88->unk01 = 3;
            }
            else
            {
                gUnk_03002490->unk88->unk01 = 1;
            }
            if (gUnk_03002490->unk88->unk01 != 0)
                break;
        }
        if (gUnk_03005550.unk0 != 0)
        {
            sub_0803f8e8();
            gUnk_03002490->unk88->unk01 = 1;
        }
        break;
    }
}

void sub_08034a88(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 4;
    gUnk_03002490->unk15 = 5;
    if (gUnk_03002490->unk88->unk05 != 4)
    {
        if (gUnk_03002490->unk88->unk05 == 9)
            gUnk_03002490->unk88->unk14 = 4;
        else
            gUnk_03002490->unk88->unk14 = 23;
        sub_080413a4(0);
        sub_0803e34c(100, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk73 = 0;
    }
    sub_0803cd60();
    if (gUnk_03002490->unk88->unk06 == 1)
    {
        sub_08006338(0x158);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        sub_08006138();
    }
    gUnk_03002490->unk46 = gUnk_0873D384[gUnk_03002490->unk88->unk0D];
    switch (gUnk_03002490->unk88->unk0D)
    {
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    case 1:
    case 2:
    case 4:
    case 5:
    case 9:
    case 10:
    case 15:
    case 16:
    case 17:
    case 19:
    case 22:
    case 23:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08034bec(void)
{
    sub_0803f870();
    while (sub_0803ff7c() == 0 && sub_080400c0() == 0 && sub_08040340() == 0 && sub_08040298() == 0)
    {
        if (gUnk_03002490->unk7A & 1)
        {
            sub_0803f8e8();
            sub_0803e4ec(0);
            sub_0804042c();
            goto end;
        }
        if (gUnk_03005550.unk1 != 0)
        {
            sub_0803f8e8();
            sub_0803e050(2);
            gUnk_03002490->unk88->unk01 = 7;
            break;
        }
        switch (gUnk_03002490->unk73)
        {
        case 0:
            if (--gUnk_03002490->unk88->unk14 == 0
                || (gUnk_03002458[gUnk_03002490->unk88->unk00] & 1) == 0)
            {
                gUnk_03002490->unk73 = 1;
                sub_080413a4(1);
                gUnk_03002490->unk88->unk14 = 6;
            }
            break;
        case 1:
            if (sub_08040084() == 0 && --gUnk_03002490->unk88->unk14 == 0)
            {
                sub_0803e050(2);
                sub_080413a4(2);
                gUnk_03002490->unk88->unk01 = 7;
            }
            break;
        }
        if (gUnk_03005550.unk0 != 0)
        {
            sub_0803f8e8();
            if (gUnk_03002490->unk88->unk3E & 7)
                sub_08006148(sub_08034a88, gCurTaskIdx);
        }
        break;
    }
end:
    sub_08040b40(7, 72);
    sub_0803f9e8();
}

void sub_08034d34(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 4;
    gUnk_03002490->unk15 = 6;
    if (gUnk_03002490->unk88->unk05 != 4)
    {
        gUnk_03002490->unk73 = 0;
        sub_080413a4(0);
        sub_0803e34c(100, gUnk_03002490->unk88->unk00);
    }
    gUnk_03002490->unk46 = gUnk_0873D384[gUnk_03002490->unk88->unk0D];
    switch (gUnk_03002490->unk88->unk0D)
    {
    case 1:
    case 2:
    case 4:
    case 5:
    case 9:
    case 10:
    case 15:
    case 16:
    case 17:
    case 19:
    case 22:
    case 23:
        while (1)
        {
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        sub_08006338(gUnk_03002490->unk46);
        sub_08006138();
    }
}

void sub_08034e60(void)
{
    sub_0803f870();
    while (sub_080400c0() == 0 && sub_08040340() == 0)
    {
        if (gUnk_03002490->unk7A & 1)
        {
            sub_0803f8e8();
            sub_0803e4ec(0);
            sub_0804042c();
            break;
        }
        if (gUnk_03005550.unk1 != 0)
        {
            sub_0803f8e8();
            sub_0803e050(2);
            gUnk_03002490->unk88->unk01 = 7;
            break;
        }
        switch (gUnk_03002490->unk73)
        {
        case 0:
            if (--gUnk_03002490->unk88->unk14 == 0)
            {
                gUnk_03002490->unk73 = 1;
                sub_080413a4(1);
                gUnk_03002490->unk88->unk14 = 5;
            }
            break;
        case 1:
            if (--gUnk_03002490->unk88->unk14 == 0)
            {
                sub_0803e050(2);
                sub_080413a4(2);
                gUnk_03002490->unk88->unk01 = 7;
            }
            break;
        }
        if (gUnk_03005550.unk0 != 0)
        {
            sub_0803f8e8();
            if (gUnk_03002490->unk88->unk3E & 7)
                sub_08006148(sub_08034a88, gCurTaskIdx);
        }
        break;
    }
    sub_08040b40(7, 72);
    sub_0803f9e8();
}

void sub_08034f70(void)
{
    gUnk_03002490->unk88->unk68 = 0;
    sub_08034f8c();
}
