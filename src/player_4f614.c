#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4f614.c (0x0804F614-0x0804F947, issue #90).
 *
 * Helpers of action 49's sub-actions.  sub_0804f614 plays the reaction
 * PlayerState.unk3E asks for (effect 4 through sub_0805afac, then one of
 * hit poses 0xCEA/0xCEB, mirrored through Task.unk3E bit 15), sub_0804f76c
 * picks a speed class 2-4 from |Task.unk54|, sub_0804f79c steps the
 * 16-step direction Task.unk46 with the sign of Task.unk54, sub_0804f7f8 is the
 * four key probes the sub-handlers share (mode 0-3 -> next sub-action 4, 6
 * or 8) and sub_0804f8ec the landing check (M11's sub_0803fa44, effect 4
 * on a fast landing). */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */

void TaskYieldTrampoline(s32 frames);
void sub_0800634c(s32 a);
void sub_08006364(s32 a);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803fa44(void);
s32 sub_08040298(void);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

s32 sub_0804f614(void)
{
    u8 k = gUnk_03002490->unk88->unk3E;

    if (k != 0)
    {
        sub_0805afac(gUnk_03002490->unk88->unk00, 4, 0);
        gUnk_03002490->unk88->unk3E = 0;
        gUnk_03002490->unk24 = 1;
        switch (k)
        {
        case 1:
            gUnk_03002490->unk3C = 0xCEB;
            TaskYieldTrampoline(2);
            {
                struct Task *t = gUnk_03002490;
                t->unk58 = -t->unk20;
                t->unk68 = 0x50000;
            }
            break;
        case 2:
            break;
        case 3:
        case 4:
            sub_0803e34c(107, (u16)gUnk_03002490->unk88->unk00);
            if (gUnk_03002490->unk40 & 0x8000)
            {
                sub_0800634c(0xCEA);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3E |= 0x8000;
            }
            else
            {
                gUnk_03002490->unk3C = 0xCEA;
                TaskYieldTrampoline(2);
            }
            break;
        case 5:
        case 6:
            sub_0803e34c(107, (u16)gUnk_03002490->unk88->unk00);
            if (!(gUnk_03002490->unk40 & 0x8000))
            {
                sub_08006364(0xCEA);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3E &= 0x7FFF;
            }
            else
            {
                gUnk_03002490->unk3C = 0xCEA;
                TaskYieldTrampoline(2);
            }
            break;
        }
        gUnk_03002490->unk24 = 0;
    }
}

s32 sub_0804f76c(void)
{
    s32 v = abs(gUnk_03002490->unk54);
    s32 r;

    if (v <= 0xFFFF)
        r = 4;
    else if (v <= 0x20000)
        r = 3;
    else
        r = 2;
    return r;
}

void sub_0804f79c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk6E == 1)
    {
        if (t->unk54 < 0)
        {
            if (--t->unk46 < 0)
                t->unk46 = 15;
            return;
        }
    }
    else if (t->unk54 > 0)
    {
        if (--t->unk46 < 0)
            t->unk46 = 15;
        return;
    }
    if (++t->unk46 > 15)
        t->unk46 = 0;
}

s32 sub_0804f7f8(s32 a)
{
    s32 r = 0;

    switch (a)
    {
    case 0:
    {
        struct Task *t = gUnk_03002490;
        if ((t->unk7A & 1) && !(gUnk_03002458[t->unk88->unk00] & 0x80)
            && (gUnk_030023C0[t->unk88->unk00] & 1))
        {
            t->unk73 = 4;
            r = 4;
        }
        break;
    }
    case 1:
    {
        struct Task *t = gUnk_03002490;
        if (!(t->unk7A & 1))
        {
            t->unk73 = 6;
            r = 6;
        }
        break;
    }
    case 2:
        if (gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2)
        {
            gUnk_03002490->unk73 = 8;
            r = 8;
        }
        break;
    case 3:
        if (sub_08040298() != 0)
        {
            gUnk_03002490->unk88->unk01 = 0;
            gUnk_03002490->unk74 = 1;
            gUnk_03002490->unk73 = 8;
            r = 8;
        }
        break;
    }
    return r;
}

s32 sub_0804f8ec(s32 a0)
{
    if (sub_0803fa44())
    {
        if ((gUnk_03002490->unk7B & 1) == 0
            && (gUnk_03002490->unk58 & 0xFFFF0000) != 0 && a0 != 0)
            sub_0805afac(gUnk_03002490->unk88->unk00, 4, 0);
        sub_0803e050(2);
        return 1;
    }
    return 0;
}
