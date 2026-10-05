#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"
#include "effect.h"

/* player_4cc7c.c (0x0804CC7C-0x0804DC07, issue #90).
 *
 * Player action bodies, part 23: action 55 and per-frame handler 52.
 * PlayerActionUFO (action 55, mode 13) is an eleven-state `switch
 * (Task.variant)` (jump table at 0x0804CCE4) over one stance: states 7-10
 * turn between the stance's four postures, each through a nested `switch
 * (Task.unk24)` on the posture it comes from (animations 0xF88-0xF9E);
 * states 2-6 are its attacks (animations 0xFA0-0xFD3, sounds 208-210 and
 * 240, effect 48), which spawn task type #6's variant 10 with sub-states
 * 0-5 through CreatePlayerObject; state 1 ends the action.  The states that
 * leave the stance end in a long `bl` to the function's own exit at
 * 0x0804D6C6 (lesson 4.39; the census took it for a function).  Its
 * handler PlayerActionUFOUpdate (jump table at 0x0804D6F0) reads the keys every
 * frame, re-binds action 55 with the next state (sounds 203-206 and 239,
 * camera preset PlayerSetMotionXPreset(13, 72)), steps the posture timer through
 * gUnk_0873DB34 and hands over to M11's transitions. */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

s32 CreatePlayerObject(s8 player, u8 variant, s32 arg);

void PlayerActionUFO(void)
{
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_UFO;
    t = gCurTask;
    if (t->player->prevMode != 13)
    {
        t->playerUfoCutIn = 0;
        t->playerUfoSfxTimer = 0;
        t->playerUfoLeaving = 0;
        gCurTask->variant = 0;
        gCurTask->u80.attackAbility = ABILITY_UFO;
    }
    switch (gCurTask->variant)
    {
    case 0:
        {
            struct Task *u = gCurTask;

            u->playerUfoFrameIndex = 0;
            u->variant = 7;
        }
        gCurTask->playerUfoPosture = -1;
    case 7:
        PlayerStopAxes(3);
        {
            struct Task *u = gCurTask;

            u->playerUfoFrameIndex = 0xFFFF;
            switch (u->playerUfoPosture)
            {
            case 8:
                u->frame = 0xF90;
                TaskYieldTrampoline(1);
                gCurTask->frame = 0xF8C;
                TaskYieldTrampoline(1);
                break;
            case 9:
                u->frame = 0xF95;
                TaskYieldTrampoline(2);
                break;
            }
        }
        gCurTask->playerUfoPosture = 7;
        while (1)
        {
            gCurTask->playerUfoFrameIndex = 0;
            TaskYieldTrampoline(5);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
            {
                gCurTask->playerUfoFrameIndex++;
                TaskYieldTrampoline(5);
            }
        }
    case 8:
        switch (gCurTask->playerUfoPosture)
        {
        case 7:
            TaskSetFrame(0xF8C);
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF90;
            TaskYieldTrampoline(2);
            break;
        case 9:
            TaskSetFrame(0xF95);
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF9E;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF8C;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF90;
            TaskYieldTrampoline(1);
            break;
        case 10:
            TaskSetFrame(0xF9E);
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF8C;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF90;
            TaskYieldTrampoline(2);
            break;
        }
        gCurTask->playerUfoPosture = 8;
        while (1)
        {
            TaskSetFrame(0xF91);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case 9:
        switch (gCurTask->playerUfoPosture)
        {
        case 7:
            TaskSetFrame(0xF88);
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF95;
            TaskYieldTrampoline(2);
            break;
        case 8:
            TaskSetFrame(0xF90);
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xF8C;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF9E;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF95;
            TaskYieldTrampoline(1);
            break;
        case 10:
            TaskSetFrame(0xF9E);
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF95;
            TaskYieldTrampoline(2);
            break;
        }
        gCurTask->playerUfoPosture = 9;
        while (1)
        {
            TaskSetFrame(0xF96);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case 10:
        switch (gCurTask->playerUfoPosture)
        {
        case 8:
            if (PlayerGetHeldDirection() == 2)
                PlayerFaceHeldDirection();
            TaskSetFrame(0xF96);
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF9E;
            TaskYieldTrampoline(2);
            break;
        case 9:
            if (PlayerGetHeldDirection() == 2)
                PlayerFaceHeldDirection();
            TaskSetFrame(0xF95);
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF88;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0xF9E;
            TaskYieldTrampoline(2);
            break;
        case 11:
            PlayerStopAxes(1);
            TaskSetFrame(0xF88);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xFA0);
            TaskYieldTrampoline(1);
            gCurTask->facing = -gCurTask->facing;
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            {
                if (!(gCurTask->waterFlags & 1))
                {
                    TaskSetMotionXFacing(0x20000, 0x8000);
                    gCurTask->speedLimitX = 0x20000;
                }
                else
                {
                    TaskSetMotionXFacing(0x10000, 0x4000);
                    gCurTask->speedLimitX = 0x10000;
                }
            }
            TaskSetFrame(0xFA0);
            TaskYieldTrampoline(1);
            TaskSetFrame(0xF88);
            TaskYieldTrampoline(2);
            break;
        }
        gCurTask->playerUfoPosture = 10;
        while (1)
        {
            TaskSetFrame(0xF9A);
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
        }
    case 2:
        gCurTask->playerUfoCharging = 0;
        gCurTask->playerUfoChargeSfxTimer = 1;
        gCurTask->playerUfoCutIn = 0;
        TaskSetFrame(0xFD1);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0xFB4);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->playerUfoCharging++;
            u->player->unk14 = 0;
            if (!(gLatchedHeldKeys[u->player->playerIndex] & 3))
                break;
            u->playerLoopCount = 0;
        }
        do
        {
            gCurTask->player->unk14++;
            TaskSetFrame(0xFB8);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        } while ((s16)++gCurTask->playerLoopCount <= 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_UFO_CHARGE_SPARKLE, 0);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
        {
            gCurTask->player->unk14++;
            TaskSetFrame(0xFBC);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        PlaySfxIfLocalPlayer(240, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_UFO_CHARGE_SPARKLE, 0);
        gCurTask->player->unk14++;
        while (1)
        {
            TaskSetFrame(0xFC0);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
    case 3:
        gCurTask->playerUfoCutIn = 0;
        TaskSetFrame(0xFD3);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_UFO_SHOT, 0);
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_UFO_SHOT, 1);
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_UFO_SHOT, 2);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++)
        {
            gCurTask->frame = 0xFD5;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++)
        {
            gCurTask->frame = 0xFD7;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++)
        {
            gCurTask->frame = 0xFD9;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        gCurTask->frame = 0xFD9;
        TaskYieldTrampoline(2);
        gCurTask->frame = 0xFDB;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        {
            struct Task *u = gCurTask;

            u->playerUfoCutIn++;
            u->frame++;
        }
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->playerUfoCutIn++;
        break;
    case 4:
        gCurTask->playerUfoCutIn = 0;
        PlayerStartOffsetScript(6);
        TaskSetFrame(0xFC4);
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_UFO_SHOT, 3);
        PlaySfxIfLocalPlayer(208, gCurTask->player->playerIndex);
        {
            struct Task *u = gCurTask;

            u->playerUfoCutIn++;
            u->frame++;
        }
        TaskYieldTrampoline(2);
        gCurTask->playerUfoCutIn++;
        break;
    case 5:
        gCurTask->playerUfoCutIn = 0;
        TaskSetFrame(0xFC6);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerStartOffsetScript(7);
        gCurTask->frame = 0xFC4;
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_UFO_SHOT, 4);
        PlaySfxIfLocalPlayer(209, gCurTask->player->playerIndex);
        {
            struct Task *u = gCurTask;

            u->playerUfoCutIn++;
            u->frame++;
        }
        TaskYieldTrampoline(2);
        gCurTask->playerUfoCutIn++;
        break;
    case 6:
        gCurTask->playerUfoCutIn = 0;
        TaskSetFrame(0xFC6);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerStartOffsetScript(8);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_UFO_SHOT, 5);
        PlaySfxIfLocalPlayer(210, gCurTask->player->playerIndex);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        {
            struct Task *u = gCurTask;

            u->playerUfoCutIn++;
            u->frame++;
        }
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFB4;
        TaskYieldTrampoline(1);
        gCurTask->playerUfoCutIn++;
        break;
    case 1:
        gCurTask->playerUfoCutIn = 0;
        PlayerStopAxes(3);
        TaskYieldTrampoline(8);
        gCurTask->playerUfoCutIn++;
        break;
    }
    TaskSleepForever();
}

void PlayerActionUFOUpdate(void)
{
    s32 old = gCurTask->variant;

    switch (old)
    {
    case 0:
        break;
    case 1:
        if ((s16)gCurTask->playerUfoCutIn != 0)
        {
            PlayerRequestLocomotion();
            return;
        }
        break;
    case 7:
        {
            struct Task *t = gCurTask;

            if (t->playerUfoFrameIndex != -1)
            {
                if (t->facing == 1)
                    TaskSetFrame((s16)(t->playerUfoFrameIndex + 0xF88));
                else
                    TaskSetFrame((s16)(t->playerUfoFrameIndex + 0xFA8));
            }
        }
        {
            struct Task *t = gCurTask;

            if (t->playerUfoPosture == 12)
            {
                if ((s16)t->player->unk14 == 3)
                    t->facing = -t->facing;
                TaskSetFrameNoFlip(gUnk_0873DB34[gCurTask->playerUfoFrameIndex]);
                {
                    struct Task *u = gCurTask;

                    if (--u->player->unk14 == 0)
                        u->playerUfoPosture = 7;
                }
            }
            else if (PlayerGetHeldDirection() == 2)
            {
                gCurTask->playerUfoPosture = 12;
                gCurTask->player->unk14 = 3;
                break;
            }
            else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            {
                gCurTask->variant = 10;
                gCurTask->playerUfoPosture = 7;
                break;
            }
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
        {
            gCurTask->variant = 8;
            gCurTask->playerUfoPosture = 7;
        }
        else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
        {
            gCurTask->variant = 9;
            gCurTask->playerUfoPosture = 7;
        }
        break;
    case 8:
        PlayerSetMotionXPreset(13, 72);
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
        {
            gCurTask->variant = 9;
            break;
        }
        goto moving;
    case 9:
        PlayerSetMotionXPreset(13, 72);
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
        {
            gCurTask->variant = 8;
            break;
        }
    moving:
        {
            struct Task *t = gCurTask;

            if ((t->velX | t->accelX | t->velY | t->accelY) == 0)
            {
                t->variant = 7;
                break;
            }
            if (gLatchedHeldKeys[t->player->playerIndex] & 48)
                t->variant = 10;
        }
        break;
    case 10:
        if (gCurTask->playerUfoPosture == 11)
            break;
        PlayerSetMotionXPreset(13, 72);
        {
            struct Task *t = gCurTask;

            if ((t->velX | t->accelX) == 0)
            {
                if (gLatchedHeldKeys[t->player->playerIndex] & 128)
                {
                    t->variant = 9;
                    break;
                }
                if (gLatchedHeldKeys[t->player->playerIndex] & 64)
                    t->variant = 8;
            }
        }
        {
            struct Task *t = gCurTask;

            if ((t->velX | t->accelX | t->velY | t->accelY) == 0)
            {
                t->variant = 7;
                break;
            }
        }
        if (PlayerGetHeldDirection() == 2)
        {
            old = -1;
            gCurTask->playerUfoPosture = 11;
        }
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            if (t->playerUfoCharging != 0)
            {
                if (!(gLatchedHeldKeys[t->player->playerIndex] & 3))
                {
                    s16 n = t->player->unk14;

                    if (n == 0)
                        t->variant = 3;
                    else if (n <= 3)
                        t->variant = 4;
                    else if (n <= 8)
                        t->variant = 5;
                    else
                        t->variant = 6;
                }
                else if ((--t->playerUfoChargeSfxTimer & 0xFFFF) == 0 && (u32)(t->playerUfoChargeSfxTimer & 0xFFFF0000) <= 0xCFFFF)
                {
                    switch (t->playerUfoChargeSfxTimer >> 18)
                    {
                    case 0:
                        t->playerUfoChargeSfxTimer += 0x10008;
                        PlaySfxIfLocalPlayer(203, t->player->playerIndex);
                        break;
                    case 1:
                        t->playerUfoChargeSfxTimer += 0x10008;
                        PlaySfxIfLocalPlayer(204, t->player->playerIndex);
                        break;
                    case 2:
                        t->playerUfoChargeSfxTimer += 0x10008;
                        PlaySfxIfLocalPlayer(205, t->player->playerIndex);
                        break;
                    default:
                        gCurTask->playerUfoChargeSfxTimer |= 5;
                        PlaySfxIfLocalPlayer(206, gCurTask->player->playerIndex);
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
            s16 m = t->playerUfoCutIn;

            if (m != 0)
            {
                if (gLatchedPressedKeys[t->player->playerIndex] & 3)
                    t->variant = 2;
                else if ((gLatchedHeldKeys[t->player->playerIndex] & 240) || m == 2)
                {
                    t->playerUfoPosture = -1;
                    t->variant = 7;
                }
            }
        }
        break;
    }
    if (gCurTask->playerUfoSfxTimer == 0)
        PlaySfxIfLocalPlayer(239, gCurTask->player->playerIndex);
    {
        struct Task *t = gCurTask;

        t->playerUfoSfxTimer = (t->playerUfoSfxTimer + 1) & 7;
        switch (t->variant)
        {
        case 7:
        case 8:
        case 9:
        case 10:
            if (gLatchedPressedKeys[t->player->playerIndex] & 3)
                t->variant = 2;
            else if (PlayerCheckDropAbility() != 0)
                gCurTask->variant = 1;
            break;
        }
    }
    if (gCurTask->playerUfoLeaving == 0 && PlayerCheckEnterDoor() != 0)
    {
        gCurTask->playerUfoLeaving++;
        gCurTask->variant = 1;
    }
    PlayerStopAtCeilingAndWall();
    PlayerCheckLanding();
    if (old != gCurTask->variant)
        TaskSetEntry(PlayerActionUFO, gCurTaskIdx);
}
