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
 * 0-5 through CreatePlayerObject; state 1 ends the action.  The states that
 * leave the stance end in a long `bl` to the function's own exit at
 * 0x0804D6C6 (lesson 4.39; the census took it for a function).  Its
 * handler sub_0804d6d0 (jump table at 0x0804D6F0) reads the keys every
 * frame, re-binds action 55 with the next state (sounds 203-206 and 239,
 * camera preset PlayerSetMotionXPreset(13, 72)), steps the posture timer through
 * gUnk_0873DB34 and hands over to M11's transitions. */

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern s16 gUnk_0873DB34[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetFrame(s32 a);
void TaskSetFrameNoFlip(s32 a);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerFaceHeldDirection(void);
void PlayerStartOffsetScript(s32 a0);
s32 sub_0803f884(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

s32 CreatePlayerObject(s8 player, u8 variant, s32 arg);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_0804cc7c(void)
{
    struct Task *t;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 52;
    t = gCurTask;
    if (t->unk88->unk05 != 13)
    {
        t->unk70 = 0;
        t->unk34 = 0;
        t->unk74 = 0;
        gCurTask->unk73 = 0;
        gCurTask->unk80 = 24;
    }
    switch (gCurTask->unk73)
    {
    case 0:
        {
            struct Task *u = gCurTask;

            u->unk46 = 0;
            u->unk73 = 7;
        }
        gCurTask->unk24 = -1;
    case 7:
        PlayerStopAxes(3);
        {
            struct Task *u = gCurTask;

            u->unk46 = 0xFFFF;
            switch (u->unk24)
            {
            case 8:
                u->unk3C = 0xF90;
                TaskYieldTrampoline(1);
                gCurTask->unk3C = 0xF8C;
                TaskYieldTrampoline(1);
                break;
            case 9:
                u->unk3C = 0xF95;
                TaskYieldTrampoline(2);
                break;
            }
        }
        gCurTask->unk24 = 7;
        while (1)
        {
            gCurTask->unk46 = 0;
            TaskYieldTrampoline(5);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
            {
                gCurTask->unk46++;
                TaskYieldTrampoline(5);
            }
        }
    case 8:
        switch (gCurTask->unk24)
        {
        case 7:
            TaskSetFrame(0xF8C);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF90;
            TaskYieldTrampoline(2);
            break;
        case 9:
            TaskSetFrame(0xF95);
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF8C;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF90;
            TaskYieldTrampoline(1);
            break;
        case 10:
            TaskSetFrame(0xF9E);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF8C;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF90;
            TaskYieldTrampoline(2);
            break;
        }
        gCurTask->unk24 = 8;
        while (1)
        {
            TaskSetFrame(0xF91);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 9:
        switch (gCurTask->unk24)
        {
        case 7:
            TaskSetFrame(0xF88);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF95;
            TaskYieldTrampoline(2);
            break;
        case 8:
            TaskSetFrame(0xF90);
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xF8C;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF95;
            TaskYieldTrampoline(1);
            break;
        case 10:
            TaskSetFrame(0xF9E);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF95;
            TaskYieldTrampoline(2);
            break;
        }
        gCurTask->unk24 = 9;
        while (1)
        {
            TaskSetFrame(0xF96);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 10:
        switch (gCurTask->unk24)
        {
        case 8:
            if (sub_0803f884() == 2)
                PlayerFaceHeldDirection();
            TaskSetFrame(0xF96);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            break;
        case 9:
            if (sub_0803f884() == 2)
                PlayerFaceHeldDirection();
            TaskSetFrame(0xF95);
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xF9E;
            TaskYieldTrampoline(2);
            break;
        case 11:
            PlayerStopAxes(1);
            TaskSetFrame(0xF88);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xFA0);
            TaskYieldTrampoline(1);
            gCurTask->unk43 = -gCurTask->unk43;
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
            {
                if (!(gCurTask->unk7B & 1))
                {
                    TaskSetMotionXFacing(0x20000, 0x8000);
                    gCurTask->unk64 = 0x20000;
                }
                else
                {
                    TaskSetMotionXFacing(0x10000, 0x4000);
                    gCurTask->unk64 = 0x10000;
                }
            }
            TaskSetFrame(0xFA0);
            TaskYieldTrampoline(1);
            TaskSetFrame(0xF88);
            TaskYieldTrampoline(2);
            break;
        }
        gCurTask->unk24 = 10;
        while (1)
        {
            TaskSetFrame(0xF9A);
            TaskYieldTrampoline(5);
            gCurTask->unk3C++;
            TaskYieldTrampoline(5);
            gCurTask->unk3C++;
            TaskYieldTrampoline(5);
            gCurTask->unk3C++;
            TaskYieldTrampoline(5);
        }
    case 2:
        gCurTask->unk2C = 0;
        gCurTask->unk30 = 1;
        gCurTask->unk70 = 0;
        TaskSetFrame(0xFD1);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0xFB4);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->unk2C++;
            u->unk88->unk14 = 0;
            if (!(gLatchedHeldKeys[u->unk88->unk00] & 3))
                break;
            u->unk6C = 0;
        }
        do
        {
            gCurTask->unk88->unk14++;
            TaskSetFrame(0xFB8);
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        } while ((s16)++gCurTask->unk6C <= 2);
        CreatePlayerEffect(gCurTask->unk88->unk00, 48, 0);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
        {
            gCurTask->unk88->unk14++;
            TaskSetFrame(0xFBC);
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        PlaySfxIfLocalPlayer(240, gCurTask->unk88->unk00);
        CreatePlayerEffect(gCurTask->unk88->unk00, 48, 0);
        gCurTask->unk88->unk14++;
        while (1)
        {
            TaskSetFrame(0xFC0);
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
    case 3:
        gCurTask->unk70 = 0;
        TaskSetFrame(0xFD3);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->unk88->unk00, 10, 0);
        CreatePlayerObject(gCurTask->unk88->unk00, 10, 1);
        CreatePlayerObject(gCurTask->unk88->unk00, 10, 2);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
        {
            gCurTask->unk3C = 0xFD5;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
        {
            gCurTask->unk3C = 0xFD7;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
        {
            gCurTask->unk3C = 0xFD9;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        gCurTask->unk3C = 0xFD9;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 0xFDB;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        {
            struct Task *u = gCurTask;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk70++;
        break;
    case 4:
        gCurTask->unk70 = 0;
        PlayerStartOffsetScript(6);
        TaskSetFrame(0xFC4);
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->unk88->unk00, 10, 3);
        PlaySfxIfLocalPlayer(208, gCurTask->unk88->unk00);
        {
            struct Task *u = gCurTask;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk70++;
        break;
    case 5:
        gCurTask->unk70 = 0;
        TaskSetFrame(0xFC6);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerStartOffsetScript(7);
        gCurTask->unk3C = 0xFC4;
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->unk88->unk00, 10, 4);
        PlaySfxIfLocalPlayer(209, gCurTask->unk88->unk00);
        {
            struct Task *u = gCurTask;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk70++;
        break;
    case 6:
        gCurTask->unk70 = 0;
        TaskSetFrame(0xFC6);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerStartOffsetScript(8);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->unk88->unk00, 10, 5);
        PlaySfxIfLocalPlayer(210, gCurTask->unk88->unk00);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        {
            struct Task *u = gCurTask;

            u->unk70++;
            u->unk3C++;
        }
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 0xFB4;
        TaskYieldTrampoline(1);
        gCurTask->unk70++;
        break;
    case 1:
        gCurTask->unk70 = 0;
        PlayerStopAxes(3);
        TaskYieldTrampoline(8);
        gCurTask->unk70++;
        break;
    }
    TaskSleepForever();
}

void sub_0804d6d0(void)
{
    s32 old = gCurTask->unk73;

    switch (old)
    {
    case 0:
        break;
    case 1:
        if ((s16)gCurTask->unk70 != 0)
        {
            PlayerRequestLocomotion();
            return;
        }
        break;
    case 7:
        {
            struct Task *t = gCurTask;

            if (t->unk46 != -1)
            {
                if (t->unk43 == 1)
                    TaskSetFrame((s16)(t->unk46 + 0xF88));
                else
                    TaskSetFrame((s16)(t->unk46 + 0xFA8));
            }
        }
        {
            struct Task *t = gCurTask;

            if (t->unk24 == 12)
            {
                if ((s16)t->unk88->unk14 == 3)
                    t->unk43 = -t->unk43;
                TaskSetFrameNoFlip(gUnk_0873DB34[gCurTask->unk46]);
                {
                    struct Task *u = gCurTask;

                    if (--u->unk88->unk14 == 0)
                        u->unk24 = 7;
                }
            }
            else if (sub_0803f884() == 2)
            {
                gCurTask->unk24 = 12;
                gCurTask->unk88->unk14 = 3;
                break;
            }
            else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
            {
                gCurTask->unk73 = 10;
                gCurTask->unk24 = 7;
                break;
            }
        }
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 64)
        {
            gCurTask->unk73 = 8;
            gCurTask->unk24 = 7;
        }
        else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128)
        {
            gCurTask->unk73 = 9;
            gCurTask->unk24 = 7;
        }
        break;
    case 8:
        PlayerSetMotionXPreset(13, 72);
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128)
        {
            gCurTask->unk73 = 9;
            break;
        }
        goto moving;
    case 9:
        PlayerSetMotionXPreset(13, 72);
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 64)
        {
            gCurTask->unk73 = 8;
            break;
        }
    moving:
        {
            struct Task *t = gCurTask;

            if ((t->unk54 | t->unk5C | t->unk58 | t->unk60) == 0)
            {
                t->unk73 = 7;
                break;
            }
            if (gLatchedHeldKeys[t->unk88->unk00] & 48)
                t->unk73 = 10;
        }
        break;
    case 10:
        if (gCurTask->unk24 == 11)
            break;
        PlayerSetMotionXPreset(13, 72);
        {
            struct Task *t = gCurTask;

            if ((t->unk54 | t->unk5C) == 0)
            {
                if (gLatchedHeldKeys[t->unk88->unk00] & 128)
                {
                    t->unk73 = 9;
                    break;
                }
                if (gLatchedHeldKeys[t->unk88->unk00] & 64)
                    t->unk73 = 8;
            }
        }
        {
            struct Task *t = gCurTask;

            if ((t->unk54 | t->unk5C | t->unk58 | t->unk60) == 0)
            {
                t->unk73 = 7;
                break;
            }
        }
        if (sub_0803f884() == 2)
        {
            old = -1;
            gCurTask->unk24 = 11;
        }
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            if (t->unk2C != 0)
            {
                if (!(gLatchedHeldKeys[t->unk88->unk00] & 3))
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
                        PlaySfxIfLocalPlayer(203, t->unk88->unk00);
                        break;
                    case 1:
                        t->unk30 += 0x10008;
                        PlaySfxIfLocalPlayer(204, t->unk88->unk00);
                        break;
                    case 2:
                        t->unk30 += 0x10008;
                        PlaySfxIfLocalPlayer(205, t->unk88->unk00);
                        break;
                    default:
                        gCurTask->unk30 |= 5;
                        PlaySfxIfLocalPlayer(206, gCurTask->unk88->unk00);
                        break;
                    }
                }
            }
        }
        PlayerSetMotionXPreset(13, 72);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        PlayerSetMotionXPreset(13, 72);
        {
            struct Task *t = gCurTask;
            s16 m = t->unk70;

            if (m != 0)
            {
                if (gLatchedPressedKeys[t->unk88->unk00] & 3)
                    t->unk73 = 2;
                else if ((gLatchedHeldKeys[t->unk88->unk00] & 240) || m == 2)
                {
                    t->unk24 = -1;
                    t->unk73 = 7;
                }
            }
        }
        break;
    }
    if (gCurTask->unk34 == 0)
        PlaySfxIfLocalPlayer(239, gCurTask->unk88->unk00);
    {
        struct Task *t = gCurTask;

        t->unk34 = (t->unk34 + 1) & 7;
        switch (t->unk73)
        {
        case 7:
        case 8:
        case 9:
        case 10:
            if (gLatchedPressedKeys[t->unk88->unk00] & 3)
                t->unk73 = 2;
            else if (PlayerCheckDropAbility() != 0)
                gCurTask->unk73 = 1;
            break;
        }
    }
    if (gCurTask->unk74 == 0 && PlayerCheckEnterDoor() != 0)
    {
        gCurTask->unk74++;
        gCurTask->unk73 = 1;
    }
    PlayerStopAtCeilingAndWall();
    PlayerCheckLanding();
    if (old != gCurTask->unk73)
        TaskSetEntry(sub_0804cc7c, gCurTaskIdx);
}
