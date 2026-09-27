#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "room.h"

/* player_4f614.c (0x0804F614-0x0804F947, issue #90).
 *
 * Helpers of action 49's sub-actions.  sub_0804f614 plays the reaction
 * PlayerState.bumpKind asks for (effect 4 through CreatePlayerEffect, then one of
 * hit poses 0xCEA/0xCEB, mirrored through Task.spriteFlags bit 15), sub_0804f76c
 * picks a speed class 2-4 from |Task.velX|, sub_0804f79c steps the
 * 16-step direction Task.unk46 with the sign of Task.velX, sub_0804f7f8 is the
 * four key probes the sub-handlers share (mode 0-3 -> next sub-action 4, 6
 * or 8) and sub_0804f8ec the landing check (M11's PlayerCheckLanding, effect 4
 * on a fast landing). */

void TaskYieldTrampoline(s32 frames);
void TaskSetFrameNoFlip(s32 a);
void TaskSetFrameFlip(s32 a);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerCheckLanding(void);
s32 PlayerCheckEnterDoor(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

s32 sub_0804f614(void)
{
    u8 k = gCurTask->player->bumpKind;

    if (k != 0)
    {
        CreatePlayerEffect(gCurTask->player->playerIndex, 4, 0);
        gCurTask->player->bumpKind = 0;
        gCurTask->unk24 = 1;
        switch (k)
        {
        case 1:
            gCurTask->frame = 0xCEB;
            TaskYieldTrampoline(2);
            {
                struct Task *t = gCurTask;
                t->velY = -t->unk20;
                t->speedLimitY = 0x50000;
            }
            break;
        case 2:
            break;
        case 3:
        case 4:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            if (gCurTask->tileWord & 0x8000)
            {
                TaskSetFrameNoFlip(0xCEA);
                TaskYieldTrampoline(2);
                gCurTask->spriteFlags |= 0x8000;
            }
            else
            {
                gCurTask->frame = 0xCEA;
                TaskYieldTrampoline(2);
            }
            break;
        case 5:
        case 6:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            if (!(gCurTask->tileWord & 0x8000))
            {
                TaskSetFrameFlip(0xCEA);
                TaskYieldTrampoline(2);
                gCurTask->spriteFlags &= 0x7FFF;
            }
            else
            {
                gCurTask->frame = 0xCEA;
                TaskYieldTrampoline(2);
            }
            break;
        }
        gCurTask->unk24 = 0;
    }
}

s32 sub_0804f76c(void)
{
    s32 v = abs(gCurTask->velX);
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
    struct Task *t = gCurTask;

    if (t->unk6E == 1)
    {
        if (t->velX < 0)
        {
            if (--t->unk46 < 0)
                t->unk46 = 15;
            return;
        }
    }
    else if (t->velX > 0)
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
        struct Task *t = gCurTask;
        if ((t->onGround & 1) && !(gLatchedHeldKeys[t->player->playerIndex] & 0x80)
            && (gLatchedPressedKeys[t->player->playerIndex] & 1))
        {
            t->variant = 4;
            r = 4;
        }
        break;
    }
    case 1:
    {
        struct Task *t = gCurTask;
        if (!(t->onGround & 1))
        {
            t->variant = 6;
            r = 6;
        }
        break;
    }
    case 2:
        if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
        {
            gCurTask->variant = 8;
            r = 8;
        }
        break;
    case 3:
        if (PlayerCheckEnterDoor() != 0)
        {
            gCurTask->player->requestedAction = 0;
            gCurTask->unk74 = 1;
            gCurTask->variant = 8;
            r = 8;
        }
        break;
    }
    return r;
}

s32 sub_0804f8ec(s32 a0)
{
    if (PlayerCheckLanding())
    {
        if ((gCurTask->waterFlags & 1) == 0
            && (gCurTask->velY & 0xFFFF0000) != 0 && a0 != 0)
            CreatePlayerEffect(gCurTask->player->playerIndex, 4, 0);
        PlayerStopAxes(2);
        return 1;
    }
    return 0;
}
