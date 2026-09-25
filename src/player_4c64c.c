#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4c64c.c (0x0804C64C-0x0804CC7B, issue #88).
 *
 * Player action bodies, part 22: action 54 and per-frame handler 51.
 * sub_0804c64c (action 54, mode 13) is a charged three-way move, a
 * `while (1) switch (Task.unk73)` state machine.  A fresh entry starts
 * in state 3 with the 120-frame timer PlayerState.unk14 and
 * Task.unk80 = 23: state 3 winds up (animation 0xF73, the frame index
 * PlayerState.unk16 stepping 0-1-2-1-2, effect 28 on the ground) and
 * picks the direction Task.unk28 from the held keys (up 0, down 2,
 * otherwise 1); state 4 (effect 47) holds until the timer runs out or
 * B is pressed, switching to state 5 once PlayerState.unk08 is 0, and
 * then jumps to state unk28.  States 0-2 are the three releases
 * (animations 0xF7C/0xF76/0xF84, sound 236, then 0xF80), which end in
 * state 5: clear PlayerState.unk42 bit 9, sub_0803e1b8(255, 0, player)
 * and end the action (sub_08006138, which falls into state 0,
 * lesson 3.403).  Its handler sub_0804ca84 re-binds state 5 from states
 * 0-2 once PlayerState.unk16 >= 0 and unk08 == 0, re-reads the direction
 * every frame in state 4 and cycles the charge animation through
 * Task.unk46 (0xF7A/0xF82/0xF74 and 0xF7B/0xF83/0xF75), and requests
 * action 23 through sub_0803fce4. */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803e34c(s32 a0, u16 a1);
void sub_0803e4a8(void);
s32 sub_0803e4ec(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f870(void);
void sub_0803f9c0(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_0804c64c(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 51;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 3;
            u = gUnk_03002490;
            u->unk88->unk14 = 120;
            u->unk80 = 23;
        }
    }
    while (1) {
        switch (gUnk_03002490->unk73) {
        case 3:
            sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
            sub_0803e650(13);
            {
                struct Task *t = gUnk_03002490;
                if (t->unk7A & 1)
                    sub_0805afac(t->unk88->unk00, 28, 3);
                else
                    sub_0803e050(2);
            }
            gUnk_03002490->unk88->unk16 = 0;
            sub_08006338(0xF73);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 1;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk88->unk16 = 2;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk88->unk16 = 1;
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk88->unk16 = 2;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
                gUnk_03002490->unk28 = 0;
            else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
                gUnk_03002490->unk28 = 2;
            else
                gUnk_03002490->unk28 = 1;
            gUnk_03002490->unk73 = 4;
            /* fallthrough */
        case 4:
            sub_0805afac(gUnk_03002490->unk88->unk00, 47, 0);
            gUnk_03002490->unk46 = 0;
            for (;;) {
                {
                    struct Task *t = gUnk_03002490;
                    if ((s8)t->unk88->unk08 == 0)
                        t->unk73 = 5;
                }
                if ((u16)--gUnk_03002490->unk88->unk14 == 0)
                    goto done;
                if (gUnk_030023C0[gUnk_03002490->unk88->unk00] & 2)
                    goto done;
                TaskYieldTrampoline(1);
            }
        done:
            gUnk_03002490->unk73 = gUnk_03002490->unk28;
            break;
        case 5:
            gUnk_03002490->unk88->unk42 &= 0xFDFF;
            do {
                sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
            } while (0);
            sub_08006138();
            /* fallthrough */
        case 0:
            sub_0803e650(9);
            sub_0805afac(gUnk_03002490->unk88->unk00, 47, 1);
            gUnk_03002490->unk88->unk16 = 8;
            sub_08006338(0xF7C);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 9;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 10;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            sub_0803e34c(236, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk16 = 253;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            sub_08006338(0xF80);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk73 = 5;
            break;
        case 1:
            sub_0803e650(10);
            sub_0805afac(gUnk_03002490->unk88->unk00, 47, 1);
            gUnk_03002490->unk88->unk16 = 3;
            sub_08006338(0xF76);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 4;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 5;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            sub_0803e34c(236, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk16 = 252;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            sub_08006338(0xF80);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk73 = 5;
            break;
        case 2:
            sub_0803e650(11);
            sub_0805afac(gUnk_03002490->unk88->unk00, 47, 1);
            gUnk_03002490->unk88->unk16 = 13;
            sub_08006338(0xF84);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 14;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk88->unk16 = 15;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            sub_0803e34c(236, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk16 = 251;
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(7);
            sub_08006338(0xF80);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk73 = 5;
            break;
        }
    }
}

void sub_0804ca84(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 4:
        sub_0803f870();
        if (sub_0803fce4(0) != 0) {
            sub_0803e4a8();
            gUnk_03002490->unk88->unk01 = 23;
            break;
        }
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
            gUnk_03002490->unk28 = 0;
        else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 128)
            gUnk_03002490->unk28 = 2;
        else
            gUnk_03002490->unk28 = 1;
        {
            struct Task *v = gUnk_03002490;
            switch (v->unk46) {
            case 0:
            case 1:
                if (v->unk28 == 0) {
                    v->unk88->unk16 = 6;
                    sub_08006338(0xF7A);
                } else if (v->unk28 == 2) {
                    v->unk88->unk16 = 11;
                    sub_08006338(0xF82);
                } else {
                    v->unk88->unk16 = 1;
                    sub_08006338(0xF74);
                }
                break;
            case 2:
            case 3:
                if (v->unk28 == 0) {
                    v->unk88->unk16 = 7;
                    sub_08006338(0xF7B);
                } else if (v->unk28 == 2) {
                    v->unk88->unk16 = 12;
                    sub_08006338(0xF83);
                } else {
                    v->unk88->unk16 = 2;
                    sub_08006338(0xF75);
                }
                break;
            }
        }
        gUnk_03002490->unk46 = (gUnk_03002490->unk46 + 1) & 3;
        break;
    case 0:
    case 1:
    case 2:
        {
            struct PlayerState *p = t->unk88;
            if ((s8)p->unk16 >= 0 && (s8)p->unk08 == 0)
                goto rebind;
        }
        break;
    rebind:
        do {
            do {
                t->unk73 = 5;
            } while (0);
        } while (0);
        sub_08006148(sub_0804c64c, gCurTaskIdx);
        break;
    case 5:
        sub_0804042c();
        return;
    }
    sub_0803f9c0();
    if (sub_0803fce4(0) != 0) {
        gUnk_03002490->unk88->unk01 = 23;
        gUnk_03002490->unk88->unk16 = 255;
        sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    } else if (!(gUnk_03002490->unk7A & 1)) {
        sub_080413a4(2);
        sub_08040b40(11, 2);
    } else {
        sub_08040b40(0, 72);
        sub_0803e4ec(0);
    }
}
