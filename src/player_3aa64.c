#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"

/* player_3aa64.c (0x0803AA64-0x0803BDE7, issue #91).
 *
 * Player action bodies, part 9: actions 18 and 23-28, per-frame handlers
 * 17 and 20-25.  gLatchedHeldKeys[player] is the held-keys mask the bodies
 * test (0x30 left/right, 0x41 A or up, 0x80 down; gLatchedPressedKeys[] is the
 * newly-pressed one).  PlayerActionSwim (action 23) is the twin of M11's
 * MetaKnightActionSwim: a four-state machine over Task.variant picked from the keys
 * (left/right = 3, A/up = 1, down = 2, else 0) that plays a row of
 * gUnk_0873D9DA[4][4] chosen by the ability; its per-frame handler 20,
 * PlayerActionSwimUpdate, re-picks the state and re-binds the coroutine when the
 * keys change.  Actions 24-28 (PlayerActionStandInWater, PlayerActionWalkInWater, PlayerActionSwallowInWater,
 * PlayerActionSpitInWater, PlayerActionWaterShot) are short animation scripts, the last a
 * four-way directional pick; handlers 21-25 run M11's predicates and
 * request the next action through PlayerState.requestedAction.  PlayerActionRecoil
 * (action 18) installs handler 17, the leaf PlayerActionRecoilUpdate. */

void TaskSetEntry(void *a, u32 i);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionSwim(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s16 *anim;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 15;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SWIM;
    t = gCurTask;
    if (t->player->mouthState != 0)
    {
        if (t->player->prevMode != 15)
        {
            t->variant = 0;
            u = gCurTask;
            u->player->unk14 = 1;
            if (gLatchedHeldKeys[u->player->playerIndex] & 65)
                u->unk28 = 2;
            else
                u->unk28 = 5;
            PlayerSetWaterMotionY();
        }
        while (1)
        {
            TaskSetFrame(364);
            TaskYieldTrampoline(gCurTask->unk28);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(gCurTask->unk28);
            }
            TaskSetFrame(0x161);
            TaskYieldTrampoline(gCurTask->unk28);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 9; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(gCurTask->unk28);
                if (gCurTask->frame == 356)
                    PlaySfxIfLocalPlayer(135, gCurTask->player->playerIndex);
            }
            PlaySfxIfLocalPlayer(135, gCurTask->player->playerIndex);
        }
    }
    v = gCurTask;
    if (v->player->prevMode != 15)
    {
        if (gLatchedHeldKeys[v->player->playerIndex] & 48)
            v->variant = 3;
        else if (gLatchedHeldKeys[v->player->playerIndex] & 65)
            v->variant = 1;
        else if (gLatchedHeldKeys[v->player->playerIndex] & 128)
            v->variant = 2;
        else
            v->variant = 0;
        PlayerSetWaterMotionY();
        gCurTask->unk2C = 0;
    }
    gCurTask->player->running = 0;
    switch (gCurTask->player->ability)
    {
    default:
        anim = gUnk_0873D9DA[0];
        break;
    case ABILITY_SWORD:
        anim = gUnk_0873D9DA[1];
        break;
    case ABILITY_HAMMER:
        anim = gUnk_0873D9DA[2];
        break;
    case ABILITY_PARASOL:
        anim = gUnk_0873D9DA[3];
        break;
    }
    switch (gCurTask->variant)
    {
    case 0:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        if (gCurTask->unk2C != 0)
        {
            gCurTask->unk2C = 0;
            TaskSetFrame(anim[3]);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame--;
            TaskYieldTrampoline(3);
        }
        TaskSetFrame(anim[0]);
        break;
    case 1:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        gCurTask->playerBaseFrame = anim[1];
        while (1)
        {
        lab1:
            PlaySfxIfLocalPlayer(120, gCurTask->player->playerIndex);
            gCurTask->unk28 = 0;
            if (gCurTask->player->prevWaterFlags & 1)
            {
                PlayerSetMotionYPreset(15);
                TaskSetFrame((s16)(gCurTask->playerBaseFrame + 1));
                TaskYieldTrampoline(2);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->playerLoopCount = 0;
                do
                {
                    gCurTask->frame++;
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                } while ((s16)++gCurTask->playerLoopCount <= 4);
                gCurTask->speedLimitY = 0x10000;
            }
            TaskSetFrame(gCurTask->playerBaseFrame);
            gCurTask->playerLoopCount = 0;
            do
            {
                if (gCurTask->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 14);
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
                goto lab1;
            gCurTask->playerLoopCount = 0;
            do
            {
                if (gCurTask->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 44);
            gCurTask->unk28 = -1;
            TaskSleepForever();
        }
    case 2:
        PlayerSetMotionYPreset(14);
        PlayerSetMotionXPreset(11, 4);
        gCurTask->player->unk14 = 15;
        TaskSetFrame(anim[3]);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        break;
    case 3:
        PlayerSetMotionXPreset(11, 3);
        gCurTask->unk28 = 0;
        if (gCurTask->player->prevWaterFlags & 1)
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
                PlayerSetMotionYPreset(12);
            else
                PlayerSetMotionYPreset(13);
        }
        else
        {
            PlayerSetMotionYPreset(13);
            gCurTask->unk28 = 10;
        }
        gCurTask->player->unk14 = 15;
        gCurTask->playerBaseFrame = anim[2];
        while (1)
        {
            PlaySfxIfLocalPlayer(120, gCurTask->player->playerIndex);
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
        }
    }
    TaskSleepForever();
}

void PlayerActionSwimUpdate(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PlayerState *q;
    u8 *st;

    if (PlayerCheckDropAbility() != 0)
        return;
    PlayerTurnToHeldDirection();
    t = gCurTask;
    if (t->player->mouthState != 0)
    {
        if (t->velY < 0)
        {
            t->unk28 = 2;
        }
        else
        {
            t->unk28 = 5;
            t->player->unk14 = 1;
        }
        if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 65) && --gCurTask->player->unk14 == 0)
        {
            gCurTask->player->unk14 = 15;
            PlayerSetMotionYPreset(18);
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
            PlayerSetMotionYPreset(17);
        else
            PlayerSetMotionYPreset(16);
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            PlayerSetMotionXPreset(11, 3);
        else
            PlayerSetMotionXPreset(11, 4);
    }
    else
    {
        st = &t->variant;
        switch (*st)
        {
        case 0:
            if (gLatchedHeldKeys[t->player->playerIndex] & 48)
                goto set3a;
            if (gLatchedHeldKeys[t->player->playerIndex] & 65)
                goto set1a;
            if (!(gLatchedHeldKeys[t->player->playerIndex] & 128))
                break;
            *st = 2;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        case 1:
            /* The do { } while (0) changes no code: it counts case 1's
               references one loop level deeper (lessons 3.383, 3.412),
               which lets the key-mask value win its register ahead of
               the switch value, as in the ROM. */
            do
            {
                if ((gLatchedHeldKeys[t->player->playerIndex] & 193) == 128)
                    goto set2b;
                if (gLatchedPressedKeys[t->player->playerIndex] & 48)
                    goto set3b;
                if (t->unk28 == -1)
                    goto set0b;
                if (gLatchedPressedKeys[t->player->playerIndex] & 65)
                    t->unk28 = 1;
            } while (0);
            break;
        case 2:
            t->unk2C = 0;
            if (gLatchedHeldKeys[t->player->playerIndex] & 65)
                goto set1c;
            if (gLatchedHeldKeys[t->player->playerIndex] & 48)
                goto set3c;
            if (!(gLatchedHeldKeys[t->player->playerIndex] & 240))
                goto set0c;
            if ((s16)t->player->unk14 != 0)
                t->player->unk14--;
            break;
        set1d:
            *st = 1;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            goto keys;
        set2d:
            *st = 2;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            goto keys;
        dec2:
            t->player->unk14--;
            goto keys;
        case 3:
            if (gLatchedHeldKeys[t->player->playerIndex] & 48)
                goto keys3;
            if (gLatchedHeldKeys[t->player->playerIndex] & 65)
                goto set1d;
            if (gLatchedHeldKeys[t->player->playerIndex] & 128)
                goto set2d;
            if ((s16)t->player->unk14 != 0)
                goto dec2;
            if (gLatchedHeldKeys[t->player->playerIndex] & 241)
                goto keys;
            if (t->velY < 0)
                goto keys;
            *st = 0;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
        keys:
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            {
            keys3:
                PlayerSetMotionXPreset(11, 3);
            }
            else
                PlayerSetMotionXPreset(11, 4);
            v = gCurTask;
            if (v->unk28 != 0)
                goto dec;
            if (gLatchedHeldKeys[v->player->playerIndex] & 65)
                PlayerSetMotionYPreset(12);
            else
                PlayerSetMotionYPreset(13);
            break;
        set3a:
            *st = 3;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set1a:
            *st = 1;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set2b:
            *st = 2;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set3b:
            *st = 3;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set0b:
            *st = 0;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set1c:
            *st = 1;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set3c:
            *st = 3;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set0c:
            *st = 0;
            gCurTask->unk2C = 1;
            TaskSetEntry(PlayerActionSwim, gCurTaskIdx);
            break;
        set25:
            w->player->requestedAction = PLAYER_ACTION_WALK_IN_WATER;
            goto out;
        dec:
            v->unk28--;
            PlayerSetMotionYPreset(13);
        }
    }
    if (PlayerCheckBButton() == 0 && PlayerCheckEnterDoor() == 0)
    {
        x = gCurTask;
        if (!(x->waterFlags & 1))
        {
            if ((gLatchedHeldKeys[(q = x->player)->playerIndex] & 64) && q->mouthState != 1)
                q->requestedAction = PLAYER_ACTION_FLOAT;
            else
                q->requestedAction = PLAYER_ACTION_JUMP;
            gCurTask->player->running = 0;
            gCurTask->player->runTapTimer = 0;
        }
        else
        {
            q = x->player;
            if (q->mouthState != 0 || x->variant != 1 || !(gLatchedHeldKeys[q->playerIndex] & 65))
            {
                w = gCurTask;
                if (w->velY != 0 && (w->onGround & 1))
                {
                    if (w->velX != 0)
                        goto set25;
                    w->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
                }
            }
        }
    }
out:
    PlayerStopAtCeilingAndWall();
    if (gCurTask->onGround & 1)
        PlayerLand(0);
}

void PlayerActionStandInWater(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 0;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_STAND_IN_WATER;
    if (gCurTask->player->prevMode != 0)
    {
        PlayerStopAxes(3);
        gCurTask->playerStandInWaterSavedWallSide = gCurTask->player->wallSide;
    }
    if (gCurTask->player->mouthState == 1)
    {
        gCurTask->playerBaseFrame = 352;
    }
    else
    {
        switch (gCurTask->player->ability)
        {
        default:
            gCurTask->playerBaseFrame = 221;
            break;
        case ABILITY_SWORD:
            gCurTask->playerBaseFrame = 1184;
            break;
        case ABILITY_HAMMER:
            gCurTask->playerBaseFrame = 0x7C1;
            break;
        case ABILITY_PARASOL:
            gCurTask->playerBaseFrame = 0x8C1;
            break;
        }
    }
    TaskSetFrame(gCurTask->playerBaseFrame);
    TaskSleepForever();
}

void PlayerActionStandInWaterUpdate(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterDoor() == 0 && PlayerCheckStartSwim() == 0 && PlayerCheckDuckOrSwallow() == 0
           && PlayerCheckBButton() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) && gCurTask->playerStandInWaterSavedWallSide == 1)
                break;
            if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->playerStandInWaterSavedWallSide == 2)
                break;
            gCurTask->player->requestedAction = PLAYER_ACTION_WALK_IN_WATER;
        }
        break;
    }
}

void PlayerActionWalkInWater(void)
{
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 1;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_WALK_IN_WATER;
    t = gCurTask;
    if ((u32)abs(t->velX) > 0x9900)
    {
        if (t->velX < 0)
            t->velX = -0x9900;
        else
            t->velX = 0x9900;
    }
    PlayerSetMotionXPreset(9, 72);
    if (gCurTask->player->mouthState == 1)
    {
        while (1)
        {
            TaskSetFrame(364);
            TaskYieldTrampoline(5);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(5);
            }
            TaskSetFrame(0x161);
            TaskYieldTrampoline(5);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 9; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(5);
            }
        }
    }
    switch (gCurTask->player->ability)
    {
    default:
        gCurTask->playerBaseFrame = 207;
        break;
    case ABILITY_SWORD:
        gCurTask->playerBaseFrame = 0x492;
        break;
    case ABILITY_HAMMER:
        gCurTask->playerBaseFrame = 0x7B3;
        break;
    case ABILITY_PARASOL:
        gCurTask->playerBaseFrame = 0x8B3;
        break;
    }
    while (1)
    {
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskSetFrame((s16)(gCurTask->playerBaseFrame - 6));
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
    }
}

void PlayerActionWalkInWaterUpdate(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterDoor() == 0 && PlayerCheckStartSwim() == 0 && PlayerCheckDuckOrSwallow() == 0
           && PlayerCheckBButton() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gCurTask->velX == 0 && gCurTask->speedLimitX == 0)
        {
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
            break;
        }
        if (!(gCurTask->onGround & 1))
            gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        break;
    }
    PlayerSetMotionXPreset(9, 72);
}

void PlayerActionSwallowInWater(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 12;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SWALLOW_IN_WATER;
    gCurTask->playerActionDone28 = 0;
    gCurTask->player->mouthState = 0;
    TaskSetFrame(225);
    TaskYieldTrampoline(2);
    PlaySfxIfLocalPlayer(113, gCurTask->player->playerIndex);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(227);
    TaskYieldTrampoline(2);
    TaskSetFrame(229);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionSwallowInWaterUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->playerActionDone28 != 0)
    {
        gLatchedHeldKeys[t->player->playerIndex] = gLatchedPressedKeys[t->player->playerIndex] = 0;
        if (t->onGround & 1)
            t->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
        else
            t->player->requestedAction = PLAYER_ACTION_SWIM;
    }
}

void PlayerActionSpitInWater(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 11;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SPIT_IN_WATER;
    gCurTask->playerActionDone28 = 0;
    gCurTask->player->mouthState = 0;
    TaskSetFrame(224);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    if ((s8)gCurTask->player->attachedCount > 1)
        CreatePlayerObject(gCurTask->player->playerIndex, 2, 0);
    else
        CreatePlayerObject(gCurTask->player->playerIndex, 1, 0);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionSpitInWaterUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->playerActionDone28 != 0)
    {
        if (!(t->onGround & 1) || (gLatchedHeldKeys[t->player->playerIndex] & 65))
            t->player->requestedAction = PLAYER_ACTION_SWIM;
        else
            t->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
    }
    if (gCurTask->onGround & 1)
    {
        if (gCurTask->velX != 0)
            PlayerSetMotionXPreset(8, 72);
        PlayerLand(1);
    }
    else
    {
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
    }
    PlayerStopAtCeilingAndWall();
}

void PlayerActionWaterShot(void)
{
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 11;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_WATER_SHOT;
    t = gCurTask;
    if (t->player->prevMode != 11)
    {
        if (gLatchedHeldKeys[t->player->playerIndex] & 240)
        {
            if (gLatchedHeldKeys[t->player->playerIndex] & 64)
                t->playerWaterShotDir = 3;
            else if (gLatchedHeldKeys[t->player->playerIndex] & 128)
                t->playerWaterShotDir = 1;
            else if (gLatchedHeldKeys[t->player->playerIndex] & 16)
                t->playerWaterShotDir = 0;
            else if (gLatchedHeldKeys[t->player->playerIndex] & 32)
                t->playerWaterShotDir = 2;
        }
        else if (t->facing == 1)
        {
            t->playerWaterShotDir = 0;
        }
        else
        {
            t->playerWaterShotDir = 2;
        }
        gCurTask->variant = 0;
        {
            struct Task *u = gCurTask;

            u->playerWaterShotCount = 0;
            u->player->unk14 = 15;
            if ((u32)abs(u->velX) > 0x10C00)
                TaskSetMotionXFacing(0x10C00, 0x5A5A5A5A);
        }
        if (gUnk_0300244C != 0)
        {
            struct Task *v = gCurTask;

            if ((u32)abs(v->velY) > 0xE000)
            {
                v->velY = 0xE000;
                PlayerSetMotionYPreset(13);
            }
        }
    }
    switch (gCurTask->playerWaterShotDir)
    {
    case 3:
        if (gCurTask->variant == 0)
        {
            CreatePlayerObject(gCurTask->player->playerIndex, 3, 0);
            while (1)
            {
                TaskSetFrame(239);
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->playerWaterShotDir = -1;
        TaskSetFrame(241);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->playerWaterShotCount++;
        break;
    case 1:
        if (gCurTask->variant == 0)
        {
            CreatePlayerObject(gCurTask->player->playerIndex, 3, 0);
            while (1)
            {
                TaskSetFrame(235);
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->playerWaterShotDir = -1;
        TaskSetFrame(237);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->playerWaterShotCount++;
        break;
    case 0:
        if (gCurTask->variant == 0)
        {
            gCurTask->facing = 1;
            CreatePlayerObject(gCurTask->player->playerIndex, 3, 0);
            while (1)
            {
                TaskSetFrame(231);
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->playerWaterShotDir = -1;
        TaskSetFrame(233);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->playerWaterShotCount++;
        break;
    case 2:
        if (gCurTask->variant == 0)
        {
            gCurTask->facing = -1;
            CreatePlayerObject(gCurTask->player->playerIndex, 3, 0);
            while (1)
            {
                TaskSetFrame(231);
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->playerWaterShotDir = -1;
        TaskSetFrame(233);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->playerWaterShotCount++;
        break;
    }
    TaskSleepForever();
}

void PlayerActionWaterShotUpdate(void)
{
    struct Task *t = gCurTask;
    u8 *st = &t->variant;

    if (*st == 0)
    {
        if (!(gLatchedHeldKeys[t->player->playerIndex] & 2) && (s16)t->player->unk14 == 0)
        {
            *st = 1;
            TaskSetEntry(PlayerActionWaterShot, gCurTaskIdx);
        }
        else
        {
            s32 d = gCurTask->playerWaterShotDir;

            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 240)
            {
                if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
                    d = 3;
                else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
                    d = 1;
                else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
                    d = 0;
                else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 32)
                    d = 2;
            }
            if (d != gCurTask->playerWaterShotDir)
            {
                gCurTask->playerWaterShotDir = d;
                TaskSetEntry(PlayerActionWaterShot, gCurTaskIdx);
            }
        }
        if ((s16)gCurTask->player->unk14 != 0)
            gCurTask->player->unk14--;
    }
    else if (t->playerWaterShotCount != 0 && PlayerCheckStartSwim() == 0)
    {
        if (!(gCurTask->onGround & 1) || (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65))
            gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
        else
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
    }
    if (!(gCurTask->waterFlags & 1))
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
            gCurTask->player->requestedAction = PLAYER_ACTION_FLOAT;
        else
            gCurTask->player->requestedAction = PLAYER_ACTION_JUMP;
    }
    if (gCurTask->onGround & 1)
    {
        PlayerLand(1);
        PlayerSetMotionXPreset(8, 72);
    }
    else
    {
        PlayerSetMotionXPreset(11, 4);
        PlayerSetMotionYPreset(13);
    }
    PlayerStopAtCeilingAndWall();
}

void PlayerActionRecoil(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 5;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_RECOIL;
    PlayerSetMotionXPreset(11, 16);
    PlayerSetMotionYPreset(22);
    gCurTask->onGround = 0;
    TaskSleepForever();
}

void PlayerActionRecoilUpdate(void)
{
    gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
}
