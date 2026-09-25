#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4cc7c.c (0x0804CC7C-0x0804DC07, issue #90).
 *
 * Player action bodies, part 23: action 55 and per-frame handler 52.
 * sub_0804cc7c (action 55, mode 13) is an eleven-state `switch
 * (Task.unk73)` (jump table at 0x0804CCE4) over one stance: states 7-10
 * turn between the stance's four postures, each through a nested `switch
 * (Task.unk24)` on the posture it comes from (animations 0xF88-0xF9E);
 * states 2-6 are its attacks (animations 0xFA0-0xFD3, sounds 208-210 and
 * 240, effect 48), which spawn task type #6's variant 10 with sub-states
 * 0-5 through sub_08053940; state 1 ends the action.  The states that
 * leave the stance end in a long `bl` to the function's own exit at
 * 0x0804D6C6 (lesson 4.39; the census took it for a function).  Its
 * handler sub_0804d6d0 (jump table at 0x0804D6F0) reads the keys every
 * frame, re-binds action 55 with the next state (sounds 203-206 and 239,
 * camera preset sub_08040b40(13, 72)), steps the posture timer through
 * gUnk_0873DB34 and hands over to M11's transitions. */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */
extern s16 gUnk_0873DB34[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_080061c0(s32 a, s32 b);
void sub_08006338(s32 a);
void sub_0800634c(s32 a);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e448(void);
void sub_0803e650(s32 a0);
s32 sub_0803f884(void);
void sub_0803f9c0(void);
s32 sub_0803fa44(void);
s32 sub_08040298(void);
s32 sub_08040340(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

s32 sub_08053940(s8 player, u8 variant, s32 arg);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804cc7c(void)
{
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 52;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 13)
    {
        t->unk70 = 0;
        t->unk34 = 0;
        t->unk74 = 0;
        gUnk_03002490->unk73 = 0;
        gUnk_03002490->unk80 = 24;
    }
    switch (gUnk_03002490->unk73)
    {
    case 0:
        {
            struct Task *u = gUnk_03002490;

            u->unk46 = 0;
            u->unk73 = 7;
        }
        gUnk_03002490->unk24 = -1;
    case 7:
        sub_0803e050(3);
        {
            struct Task *u = gUnk_03002490;

            u->unk46 = 0xFFFF;
            switch (u->unk24)
            {
            case 8:
                u->unk3C = 0xF90;
                TaskYieldTrampoline(1);
                gUnk_03002490->unk3C = 0xF8C;
                TaskYieldTrampoline(1);
                break;
            case 9:
                u->unk3C = 0xF95;
                TaskYieldTrampoline(2);
                break;
            }
        }
        gUnk_03002490->unk24 = 7;
        while (1)
        {
            gUnk_03002490->unk46 = 0;
            TaskYieldTrampoline(5);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk46++;
                TaskYieldTrampoline(5);
            }
        }
    case 8:
        switch (gUnk_03002490->unk24)
        {
        case 7:
            sub_08006338(0xF8C);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF90;
            TaskYieldTrampoline(2);
            break;
        case 9:
            sub_08006338(0xF95);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF8C;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF90;
            TaskYieldTrampoline(1);
            break;
        case 10:
            sub_08006338(0xF9E);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF8C;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF90;
            TaskYieldTrampoline(2);
            break;
        }
        gUnk_03002490->unk24 = 8;
        while (1)
        {
            sub_08006338(0xF91);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 9:
        switch (gUnk_03002490->unk24)
        {
        case 7:
            sub_08006338(0xF88);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF95;
            TaskYieldTrampoline(2);
            break;
        case 8:
            sub_08006338(0xF90);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xF8C;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF95;
            TaskYieldTrampoline(1);
            break;
        case 10:
            sub_08006338(0xF9E);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF95;
            TaskYieldTrampoline(2);
            break;
        }
        gUnk_03002490->unk24 = 9;
        while (1)
        {
            sub_08006338(0xF96);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 10:
        switch (gUnk_03002490->unk24)
        {
        case 8:
            if (sub_0803f884() == 2)
                sub_0803e448();
            sub_08006338(0xF96);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            break;
        case 9:
            if (sub_0803f884() == 2)
                sub_0803e448();
            sub_08006338(0xF95);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            break;
        case 11:
            sub_0803e050(1);
            sub_08006338(0xF88);
            TaskYieldTrampoline(2);
            sub_08006338(0xFA0);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk43 = -gUnk_03002490->unk43;
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
            {
                if (!(gUnk_03002490->unk7B & 1))
                {
                    sub_080061c0(0x20000, 0x8000);
                    gUnk_03002490->unk64 = 0x20000;
                }
                else
                {
                    sub_080061c0(0x10000, 0x4000);
                    gUnk_03002490->unk64 = 0x10000;
                }
            }
            sub_08006338(0xFA0);
            TaskYieldTrampoline(1);
            sub_08006338(0xF88);
            TaskYieldTrampoline(2);
            break;
        }
        gUnk_03002490->unk24 = 10;
        while (1)
        {
            sub_08006338(0xF9A);
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
        }
    case 2:
        gUnk_03002490->unk2C = 0;
        gUnk_03002490->unk30 = 1;
        gUnk_03002490->unk70 = 0;
        sub_08006338(0xFD1);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_08006338(0xFB4);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        {
            struct Task *u = gUnk_03002490;

            u->unk2C++;
            u->unk88->unk14 = 0;
            if (!(gUnk_03002458[u->unk88->unk00] & 3))
                break;
            u->unk6C = 0;
        }
        do
        {
            gUnk_03002490->unk88->unk14++;
            sub_08006338(0xFB8);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        } while ((s16)++gUnk_03002490->unk6C <= 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 48, 0);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk88->unk14++;
            sub_08006338(0xFBC);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        sub_0803e34c(240, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 48, 0);
        gUnk_03002490->unk88->unk14++;
        while (1)
        {
            sub_08006338(0xFC0);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
    case 3:
        gUnk_03002490->unk70 = 0;
        sub_08006338(0xFD3);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_08053940(gUnk_03002490->unk88->unk00, 10, 0);
        sub_08053940(gUnk_03002490->unk88->unk00, 10, 1);
        sub_08053940(gUnk_03002490->unk88->unk00, 10, 2);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C = 0xFD5;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C = 0xFD7;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C = 0xFD9;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
        }
        gUnk_03002490->unk3C = 0xFD9;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFDB;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        {
            struct Task *u = gUnk_03002490;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk70++;
        break;
    case 4:
        gUnk_03002490->unk70 = 0;
        sub_0803e650(6);
        sub_08006338(0xFC4);
        TaskYieldTrampoline(2);
        sub_08053940(gUnk_03002490->unk88->unk00, 10, 3);
        sub_0803e34c(208, gUnk_03002490->unk88->unk00);
        {
            struct Task *u = gUnk_03002490;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk70++;
        break;
    case 5:
        gUnk_03002490->unk70 = 0;
        sub_08006338(0xFC6);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0803e650(7);
        gUnk_03002490->unk3C = 0xFC4;
        TaskYieldTrampoline(2);
        sub_08053940(gUnk_03002490->unk88->unk00, 10, 4);
        sub_0803e34c(209, gUnk_03002490->unk88->unk00);
        {
            struct Task *u = gUnk_03002490;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(2);
        gUnk_03002490->unk70++;
        break;
    case 6:
        gUnk_03002490->unk70 = 0;
        sub_08006338(0xFC6);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        sub_0803e650(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_08053940(gUnk_03002490->unk88->unk00, 10, 5);
        sub_0803e34c(210, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        {
            struct Task *u = gUnk_03002490;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 0xFB4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk70++;
        break;
    case 1:
        gUnk_03002490->unk70 = 0;
        sub_0803e050(3);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk70++;
        break;
    }
    sub_08006138();
}

void sub_0804d6d0(void)
{
    s32 old = gUnk_03002490->unk73;

    switch (old)
    {
    case 0:
        break;
    case 1:
        if ((s16)gUnk_03002490->unk70 != 0)
        {
            sub_0804042c();
            return;
        }
        break;
    case 7:
        {
            struct Task *t = gUnk_03002490;

            if (t->unk46 != -1)
            {
                if (t->unk43 == 1)
                    sub_08006338((s16)(t->unk46 + 0xF88));
                else
                    sub_08006338((s16)(t->unk46 + 0xFA8));
            }
        }
        {
            struct Task *t = gUnk_03002490;

            if (t->unk24 == 12)
            {
                if ((s16)t->unk88->unk14 == 3)
                    t->unk43 = -t->unk43;
                sub_0800634c(gUnk_0873DB34[gUnk_03002490->unk46]);
                {
                    struct Task *u = gUnk_03002490;

                    if (--u->unk88->unk14 == 0)
                        u->unk24 = 7;
                }
            }
            else if (sub_0803f884() == 2)
            {
                gUnk_03002490->unk24 = 12;
                gUnk_03002490->unk88->unk14 = 3;
                break;
            }
            else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
            {
                gUnk_03002490->unk73 = 10;
                gUnk_03002490->unk24 = 7;
                break;
            }
        }
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
        {
            gUnk_03002490->unk73 = 8;
            gUnk_03002490->unk24 = 7;
        }
        else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
        {
            gUnk_03002490->unk73 = 9;
            gUnk_03002490->unk24 = 7;
        }
        break;
    case 8:
        sub_08040b40(13, 72);
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
        {
            gUnk_03002490->unk73 = 9;
            break;
        }
        goto moving;
    case 9:
        sub_08040b40(13, 72);
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
        {
            gUnk_03002490->unk73 = 8;
            break;
        }
    moving:
        {
            struct Task *t = gUnk_03002490;

            if ((t->unk54 | t->unk5C | t->unk58 | t->unk60) == 0)
            {
                t->unk73 = 7;
                break;
            }
            if (gUnk_03002458[t->unk88->unk00] & 48)
                t->unk73 = 10;
        }
        break;
    case 10:
        if (gUnk_03002490->unk24 == 11)
            break;
        sub_08040b40(13, 72);
        {
            struct Task *t = gUnk_03002490;

            if ((t->unk54 | t->unk5C) == 0)
            {
                if (gUnk_03002458[t->unk88->unk00] & 128)
                {
                    t->unk73 = 9;
                    break;
                }
                if (gUnk_03002458[t->unk88->unk00] & 64)
                    t->unk73 = 8;
            }
        }
        {
            struct Task *t = gUnk_03002490;

            if ((t->unk54 | t->unk5C | t->unk58 | t->unk60) == 0)
            {
                t->unk73 = 7;
                break;
            }
        }
        if (sub_0803f884() == 2)
        {
            old = -1;
            gUnk_03002490->unk24 = 11;
        }
        break;
    case 2:
        {
            struct Task *t = gUnk_03002490;

            if (t->unk2C != 0)
            {
                if (!(gUnk_03002458[t->unk88->unk00] & 3))
                {
                    s16 n = t->unk88->unk14;

                    if (n == 0)
                        t->unk73 = 3;
                    else if (n <= 3)
                        t->unk73 = 4;
                    else if (n <= 8)
                        t->unk73 = 5;
                    else
                        t->unk73 = 6;
                }
                else if ((--t->unk30 & 0xFFFF) == 0 && (u32)(t->unk30 & 0xFFFF0000) <= 0xCFFFF)
                {
                    switch (t->unk30 >> 18)
                    {
                    case 0:
                        t->unk30 += 0x10008;
                        sub_0803e34c(203, t->unk88->unk00);
                        break;
                    case 1:
                        t->unk30 += 0x10008;
                        sub_0803e34c(204, t->unk88->unk00);
                        break;
                    case 2:
                        t->unk30 += 0x10008;
                        sub_0803e34c(205, t->unk88->unk00);
                        break;
                    default:
                        gUnk_03002490->unk30 |= 5;
                        sub_0803e34c(206, gUnk_03002490->unk88->unk00);
                        break;
                    }
                }
            }
        }
        sub_08040b40(13, 72);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        sub_08040b40(13, 72);
        {
            struct Task *t = gUnk_03002490;
            s16 m = t->unk70;

            if (m != 0)
            {
                if (gUnk_030023C0[t->unk88->unk00] & 3)
                    t->unk73 = 2;
                else if ((gUnk_03002458[t->unk88->unk00] & 240) || m == 2)
                {
                    t->unk24 = -1;
                    t->unk73 = 7;
                }
            }
        }
        break;
    }
    if (gUnk_03002490->unk34 == 0)
        sub_0803e34c(239, gUnk_03002490->unk88->unk00);
    {
        struct Task *t = gUnk_03002490;

        t->unk34 = (t->unk34 + 1) & 7;
        switch (t->unk73)
        {
        case 7:
        case 8:
        case 9:
        case 10:
            if (gUnk_030023C0[t->unk88->unk00] & 3)
                t->unk73 = 2;
            else if (sub_08040340() != 0)
                gUnk_03002490->unk73 = 1;
            break;
        }
    }
    if (gUnk_03002490->unk74 == 0 && sub_08040298() != 0)
    {
        gUnk_03002490->unk74++;
        gUnk_03002490->unk73 = 1;
    }
    sub_0803f9c0();
    sub_0803fa44();
    if (old != gUnk_03002490->unk73)
        sub_08006148(sub_0804cc7c, gCurTaskIdx);
}
