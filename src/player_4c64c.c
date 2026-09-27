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
 * and end the action (TaskSleepForever, which falls into state 0,
 * lesson 3.403).  Its handler sub_0804ca84 re-binds state 5 from states
 * 0-2 once PlayerState.unk16 >= 0 and unk08 == 0, re-reads the direction
 * every frame in state 4 and cycles the charge animation through
 * Task.unk46 (0xF7A/0xF82/0xF74 and 0xF7B/0xF83/0xF75), and requests
 * action 23 through sub_0803fce4. */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u16 gUnk_030023C0[];             /* newly-pressed keys, latched per player */

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
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
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 51;
    {
        struct Task *t = gCurTask;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 3;
            u = gCurTask;
            u->unk88->unk14 = 120;
            u->unk80 = 23;
        }
    }
    while (1) {
        switch (gCurTask->unk73) {
        case 3:
            sub_0803e1b8(3, 0, gCurTask->unk88->unk00);
            sub_0803e650(13);
            {
                struct Task *t = gCurTask;
                if (t->unk7A & 1)
                    sub_0805afac(t->unk88->unk00, 28, 3);
                else
                    sub_0803e050(2);
            }
            gCurTask->unk88->unk16 = 0;
            TaskSetFrame(0xF73);
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 1;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk88->unk16 = 2;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk88->unk16 = 1;
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk88->unk16 = 2;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            if (gUnk_03002458[gCurTask->unk88->unk00] & 64)
                gCurTask->unk28 = 0;
            else if (gUnk_03002458[gCurTask->unk88->unk00] & 128)
                gCurTask->unk28 = 2;
            else
                gCurTask->unk28 = 1;
            gCurTask->unk73 = 4;
            /* fallthrough */
        case 4:
            sub_0805afac(gCurTask->unk88->unk00, 47, 0);
            gCurTask->unk46 = 0;
            for (;;) {
                {
                    struct Task *t = gCurTask;
                    if ((s8)t->unk88->unk08 == 0)
                        t->unk73 = 5;
                }
                if ((u16)--gCurTask->unk88->unk14 == 0)
                    goto done;
                if (gUnk_030023C0[gCurTask->unk88->unk00] & 2)
                    goto done;
                TaskYieldTrampoline(1);
            }
        done:
            gCurTask->unk73 = gCurTask->unk28;
            break;
        case 5:
            gCurTask->unk88->unk42 &= 0xFDFF;
            do {
                sub_0803e1b8(255, 0, gCurTask->unk88->unk00);
            } while (0);
            TaskSleepForever();
            /* fallthrough */
        case 0:
            sub_0803e650(9);
            sub_0805afac(gCurTask->unk88->unk00, 47, 1);
            gCurTask->unk88->unk16 = 8;
            TaskSetFrame(0xF7C);
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 9;
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 10;
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            sub_0803e34c(236, gCurTask->unk88->unk00);
            gCurTask->unk88->unk16 = 253;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xF80);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk73 = 5;
            break;
        case 1:
            sub_0803e650(10);
            sub_0805afac(gCurTask->unk88->unk00, 47, 1);
            gCurTask->unk88->unk16 = 3;
            TaskSetFrame(0xF76);
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 4;
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 5;
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            sub_0803e34c(236, gCurTask->unk88->unk00);
            gCurTask->unk88->unk16 = 252;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xF80);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk73 = 5;
            break;
        case 2:
            sub_0803e650(11);
            sub_0805afac(gCurTask->unk88->unk00, 47, 1);
            gCurTask->unk88->unk16 = 13;
            TaskSetFrame(0xF84);
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 14;
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk88->unk16 = 15;
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            sub_0803e34c(236, gCurTask->unk88->unk00);
            gCurTask->unk88->unk16 = 251;
            gCurTask->unk3C++;
            TaskYieldTrampoline(7);
            TaskSetFrame(0xF80);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk73 = 5;
            break;
        }
    }
}

void sub_0804ca84(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 4:
        sub_0803f870();
        if (sub_0803fce4(0) != 0) {
            sub_0803e4a8();
            gCurTask->unk88->unk01 = 23;
            break;
        }
        if (gUnk_03002458[gCurTask->unk88->unk00] & 64)
            gCurTask->unk28 = 0;
        else if (gUnk_03002458[gCurTask->unk88->unk00] & 128)
            gCurTask->unk28 = 2;
        else
            gCurTask->unk28 = 1;
        {
            struct Task *v = gCurTask;
            switch (v->unk46) {
            case 0:
            case 1:
                if (v->unk28 == 0) {
                    v->unk88->unk16 = 6;
                    TaskSetFrame(0xF7A);
                } else if (v->unk28 == 2) {
                    v->unk88->unk16 = 11;
                    TaskSetFrame(0xF82);
                } else {
                    v->unk88->unk16 = 1;
                    TaskSetFrame(0xF74);
                }
                break;
            case 2:
            case 3:
                if (v->unk28 == 0) {
                    v->unk88->unk16 = 7;
                    TaskSetFrame(0xF7B);
                } else if (v->unk28 == 2) {
                    v->unk88->unk16 = 12;
                    TaskSetFrame(0xF83);
                } else {
                    v->unk88->unk16 = 2;
                    TaskSetFrame(0xF75);
                }
                break;
            }
        }
        gCurTask->unk46 = (gCurTask->unk46 + 1) & 3;
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
        TaskSetEntry(sub_0804c64c, gCurTaskIdx);
        break;
    case 5:
        sub_0804042c();
        return;
    }
    sub_0803f9c0();
    if (sub_0803fce4(0) != 0) {
        gCurTask->unk88->unk01 = 23;
        gCurTask->unk88->unk16 = 255;
        sub_0803e1b8(255, 0, gCurTask->unk88->unk00);
    } else if (!(gCurTask->unk7A & 1)) {
        sub_080413a4(2);
        sub_08040b40(11, 2);
    } else {
        sub_08040b40(0, 72);
        sub_0803e4ec(0);
    }
}
