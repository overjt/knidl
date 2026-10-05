#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

u32 RandomRange(u32 range);
s32 PlaySfx(s32 id);
void TaskSetEntry(void *a, u32 i);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);
void sub_08021c74(s32 a, s32 b);
void TaskInitWaterFlags(void);
s32 IsFullBlockAtPixel(u16 a, u16 b);
s32 IsTaskBelowPlayerBounds(struct Task *t);
void RequestScreenShake(u32 a);

/* gPlayerMotionYPresets is a table of 8-byte records; the three used halfwords are
   signed 8.8 velocities for Task.velY / unk60 / unk68, and 0x9999 is the
   "leave this axis alone" sentinel.  Field 0's sign bit additionally clears
   Task.onGround.  Same unpack as PlayerSetMotionXPreset's case 10 (which writes
   unk54/unk5C/unk64), except that block 1 needs its own sentinel local: the
   `u16 c` truncation keeps cse2 from folding the copy into the shift, and the
   separate result `s` lets the loaded value die at that copy so the shifted
   result can reuse its register. */
void PlayerSetMotionYPreset(s32 a0)
{
    u16 *e = (u16 *)gPlayerMotionYPresets + a0 * 4;
    s32 v = e[0];

    if (v != 0x9999)
    {
        struct Task *t = gCurTask;
        u16 c = v;
        s32 s = c << 8;

        if (c & 0x8000)
            s |= 0xFF000000;
        t->velY = s;
        if (e[0] & 0x8000)
            t->onGround = 0;
    }

    if (e[1] != 0x9999)
    {
        struct Task *t2 = gCurTask;
        s32 v2 = e[1] << 8;

        if (e[1] & 0x8000)
            v2 |= 0xFF000000;
        t2->accelY = v2;
    }

    if (e[2] != 0x9999)
    {
        struct Task *t3 = gCurTask;
        s32 v3 = e[2] << 8;

        if (e[2] & 0x8000)
            v3 |= 0xFF000000;
        t3->speedLimitY = v3;
    }
}

void MetaKnightActionStand(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 0;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_STAND;

    if (gCurTask->player->prevMode != 0)
    {
        struct Task *t;
        struct Task *t2;

        PlayerStopAxes(3);
        t = gCurTask;
        t->playerStandSavedClampedTopY = (u16)t->player->clampedTopY;
        t->playerPoseSlope = t->player->slope;
        if (t->player->wallSide != 0)
            t->player->savedWallSide = t->player->wallSide;
        gCurTask->player->running = 0;
        t2 = gCurTask;
        t2->player->actionFlags &= ~PLAYER_ACTION_FLAG_RUN_PENDING;
        t2->player->runTapTimer = 0;
        PlayerPlayBump();
    }

    {
        s16 *p = (s16 *)gMetaKnightStandFrames;

        TaskSetFrame(p[PlayerGetFacingSlope(gCurTask->player->playerIndex)]);
    }
    TaskSleepForever();
}

void MetaKnightActionWalk(void)
{
    struct PlayerState *p;
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 1;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_WALK;
    PlayerSetMotionXPreset(1, 72);
    p = gCurTask->player;
    if (p->prevMode != 1)
    {
        p->running = 0;
        gCurTask->player->savedWallSide = 0;
        gCurTask->player->runTapTimer = 0;
        gCurTask->playerWalkStepDelay = 0;
        PlayerPlayBump();
    }
    while (1)
    {
        TaskSetFrame(0x11D5);
        TaskYieldTrampoline(gCurTask->playerWalkStepDelay + 2);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 10);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 5);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 5);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 2);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 10);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 5);
        t = gCurTask; t->frame++; TaskYieldTrampoline(t->playerWalkStepDelay + 5);
    }
}

void MetaKnightActionWalkUpdate(void)
{
    while (PlayerCheckSkid() == 0 && PlayerCheckJump() == 0 && PlayerCheckFallOrWater() == 0
           && PlayerCheckEnterDoor() == 0 && PlayerCheckLadder() == 0 && PlayerCheckDuckOrSwallow() == 0
           && PlayerCheckBButton() == 0)
    {
        struct Task *t = gCurTask;

        if (t->velX == 0 && t->speedLimitX == 0)
        {
            t->player->requestedAction = META_KNIGHT_ACTION_STAND;
        }
        else if (gTerrainResult[0] != 0)
        {
            PlayerCheckBump();
            gCurTask->player->requestedAction = META_KNIGHT_ACTION_STAND;
        }
        else
        {
            struct Task *t2 = gCurTask;
            struct PlayerState *p = t2->player;
            s32 v = p->running;

            if (v != 0)
            {
                p->requestedAction = META_KNIGHT_ACTION_RUN;
            }
            else if ((gLatchedHeldKeys[p->playerIndex] & 48) == 0)
            {
                s32 d = t2->velX;

                if (d < 0)
                    d = -d;
                if ((u32)d <= 0xFFFF)
                    t2->playerWalkStepDelay = 2;
            }
            else
            {
                t2->playerWalkStepDelay = v;
            }
        }
        break;
    }
    PlayerSetMotionXPreset(2, 72);
}

void MetaKnightActionRun(void)
{
    struct PlayerState *p;
    u16 *q;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 2;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_RUN;
    gCurTask->playerCrossingGap = 0;
    PlayerSetMotionXPreset(3, 72);
    if (gCurTask->player->prevMode != 2)
    {
        ((u8 *)gCurTask->player)[70] = 0;
        q = gLatchedHeldKeys;
        p = gCurTask->player;
        if (q[p->playerIndex] & 48)
        {
            if (((u8 *)p)[62] == 2)
                ((u8 *)p)[62] = 0;
        }
        PlayerPlayBump();
        PlayerStartSfx(117, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_RUN_DUST, 0);
    }
    while (1)
    {
        TaskSetFrame(0x11DD);
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount++;
        } while ((s16)gCurTask->playerLoopCount <= 4);
    }
}

void MetaKnightActionRunUpdate(void)
{
    struct Task *t;
    struct Task *t3;
    struct PlayerState *p;
    struct PlayerState *p4;
    u16 *q;
    s32 x;
    s32 m;
    s32 m2;
    s32 y;
    s32 m3;

    t = gCurTask;
    if (t->playerCrossingGap == 0)
    {
        m2 = gTerrainResult[13];
        if (m2 != 0)
        {
            t->player->playerOverGapTimer = 5;
            t->playerCrossingGap = 1;
        }
        else
        {
            t->player->playerOverGapTimer = m2;
        }
    }
    else
    {
        p = t->player;
        if ((s16)p->playerOverGapTimer == 0)
        {
            if (IsFullBlockAtPixel(((u16 *)t)[36],
                             (y = ((u16 *)t)[37], m3 = -16, m3 &= y, m3 + 16)) != 0)
                gCurTask->onGround = 1;
        }
        else
        {
            p->playerOverGapTimer--;
        }
    }
    t = gCurTask;
    if ((t->onGround & 1) != 0 || (((u8 *)t->player)[72] & 3) != 0)
    {
        t->playerCrossingGap = 0;
        t->player->playerOverGapTimer = 0;
    }
    while (PlayerCheckSkid() == 0 && PlayerCheckJump() == 0)
    {
        if (gCurTask->playerCrossingGap == 0 && PlayerCheckFallOrWater() != 0)
            break;
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (PlayerCheckLadder() != 0)
            break;
        if (PlayerCheckDuckOrSwallow() != 0)
            goto end;
        if (PlayerCheckBButton() != 0)
            goto end;
        q = gLatchedHeldKeys;
        t3 = gCurTask;
        p4 = t3->player;
        m = q[p4->playerIndex] & 48;
        if (m == 0)
        {
            x = t3->velX;
            if (x < 0)
                x = -x;
            if ((u32)x <= 0x1CBFF)
            {
                p4->running = m;
                gCurTask->player->requestedAction = META_KNIGHT_ACTION_WALK;
                goto end;
            }
        }
        if (gTerrainResult[0] == 0)
            goto end;
        PlayerCheckBump();
        gCurTask->player->requestedAction = META_KNIGHT_ACTION_STAND;
        goto end;
    }
end:
    PlayerSetMotionXPreset(3, 72);
}

void MetaKnightActionSkid(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 3;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_SKID;
    PlayerSetMotionXPreset(4, 72);
    if (gCurTask->player->prevMode != 3)
    {
        PlaySfx(119);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SKID_DUST, 0);
    }
    TaskSetFrame(0x11E3);
    TaskSleepForever();
}

void MetaKnightActionJump(void)
{
    struct Task *t;
    struct PlayerState *p;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 4;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_5;
    t = gCurTask;
    if (t->player->prevMode != 4)
    {
        t->playerJumpFallAllowed = 1;
        p = t->player;
        if (p->prevMode == 9)
        {
            p->playerJumpPhaseTimer = 4;
        }
        else
        {
            t->variant = 0;
            TaskSetFrame(0x11ED);
            TaskYieldTrampoline(3);
            gCurTask->player->playerJumpPhaseTimer = 20;
        }
        PlayerSetMotionYPreset(4);
        PlaySfx(SE_META_KNIGHT_JUMP);
        gCurTask->variant = 1;
        ((s8 *)gCurTask->player)[16] = 10;
    }
    PlayerPlayBump();
    TaskSetFrame(0x11E4);
    TaskYieldTrampoline(8);
    gCurTask->playerJumpFallAllowed = 0;
    TaskSetFrame(0x11E5);
    TaskYieldTrampoline(3);
    gCurTask->playerLoopCount = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->playerLoopCount++;
    } while ((s16)gCurTask->playerLoopCount <= 5);
    gCurTask->playerJumpFallAllowed = 1;
    TaskSleepForever();
}

void MetaKnightActionJumpUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;
    struct PlayerState *p3;
    s32 k;

    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterWater() == 0 && PlayerCheckLadder() == 0)
    {
        if (PlayerCheckBButton() != 0)
            break;
        if (PlayerCheckEnterDoor() != 0)
            break;
        t = gCurTask;
        k = t->variant;
        if (k == 0)
            return;
        if (t->onGround & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        if (gTerrainResult[1] != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(2);
            gCurTask->player->requestedAction = META_KNIGHT_ACTION_FALL;
            break;
        }
        if (k == 1)
        {
            p = t->player;
            p->playerJumpPhaseTimer--;
            if (p->playerJumpPhaseTimer == 0 || (k &= gLatchedHeldKeys[t->player->playerIndex]) == 0)
            {
                t->variant = 2;
                PlayerSetMotionYPreset(5);
            }
        }
        else if (t->playerJumpFallAllowed != 0 && t->velY >= 0)
        {
            PlayerSetMotionYPreset(6);
            gCurTask->player->requestedAction = META_KNIGHT_ACTION_FALL;
        }
        p3 = gCurTask->player;
        if (((s8 *)p3)[16] == 0)
        {
            if (gLatchedPressedKeys[p3->playerIndex] & 1)
            {
                p3->requestedAction = META_KNIGHT_ACTION_FLOAT;
                break;
            }
        }
        if (gTerrainResult[0] == 0)
            break;
        PlayerCheckBump();
        if (((u8 *)gCurTask->player)[62] & 7)
            TaskSetEntry(MetaKnightActionJump, gCurTaskIdx);
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    PlayerStopAtWall();
}

void MetaKnightActionReleaseJump(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 4;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_6;
    if (gCurTask->player->prevMode != 4)
    {
        PlayerSetMotionYPreset(4);
        PlaySfx(SE_META_KNIGHT_JUMP);
        ((s8 *)gCurTask->player)[16] = 10;
    }
    gCurTask->variant = 1;
    TaskSetFrame(0x11E4);
    TaskSleepForever();
}

void MetaKnightActionFall(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 5;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_FALL;
    PlayerSetMotionYPreset(6);
    PlayerPlayBump();
    TaskSetFrame(0x11EC);
    TaskSleepForever();
}

void MetaKnightActionFallUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;

    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterWater() == 0 && PlayerCheckLadder() == 0)
    {
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (PlayerCheckBButton() != 0)
            break;
        t = gCurTask;
        if (t->onGround & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        p = t->player;
        if (((s8 *)p)[16] == 0)
        {
            if (gLatchedPressedKeys[p->playerIndex] & 1)
            {
                p->requestedAction = META_KNIGHT_ACTION_FLOAT;
                break;
            }
        }
        if (gTerrainResult[0] == 0)
            break;
        PlayerCheckBump();
        if (((u8 *)gCurTask->player)[62] & 7)
            TaskSetEntry(MetaKnightActionFall, gCurTaskIdx);
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    if (gCurTask->onGround & 1)
        PlayerLand(1);
    PlayerStopAtCeilingAndWall();
}

void MetaKnightActionFloat(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 14;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_FLOAT;
    gCurTask->player->running = 0;
    ((s8 *)gCurTask->player)[16] = 7;
    PlayerSetMotionYPreset(11);
    PlaySfx(0x109);
    TaskSetFrame(0x11EE);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E4);
    TaskSleepForever();
}

void MetaKnightActionFloatUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 v;

    PlayerTurnToHeldDirection();
    while (PlayerCheckLadder() == 0 && PlayerCheckBButton() == 0)
    {
        if ((v = PlayerCheckEnterDoor()) != 0)
            break;
        t = gCurTask;
        if (t->onGround & 1)
        {
            if (t->velY >= 0)
            {
                PlayerCheckBump();
                PlayerLand(0);
                PlayerRequestLocomotion();
                goto end;
            }
            t->onGround = v;
        }
        p = gCurTask->player;
        if (((s8 *)p)[16] == 0)
        {
            if (gLatchedPressedKeys[p->playerIndex] & 1)
            {
                TaskSetEntry(MetaKnightActionFloat, gCurTaskIdx);
                break;
            }
        }
        if (gCurTask->velY > 0x10000)
        {
            PlayerSetMotionYPreset(6);
            gCurTask->player->requestedAction = META_KNIGHT_ACTION_FALL;
        }
        break;
    }
end:
    PlayerSetMotionXPreset(6, 72);
    PlayerStopAtWall();
}

void MetaKnightActionDuck(void)
{
    struct Task *t;
    s16 *q;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 6;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_DUCK;
    t = gCurTask;
    if (t->player->prevMode != 6)
    {
        ((u32 **)t->player)[25] = gPlayerDuckBodyBox;
        ((u32 **)t->player)[26] = gPlayerDuckTerrainBox;
        t->playerPoseSlope = ((u8 *)t->player)[75];
        PlayerSetMotionXPreset(0, 72);
    }
    gCurTask->playerDuckDropTimer = 8;
    q = (s16 *)gUnk_0873D5C0;
    TaskSetFrame(q[PlayerGetFacingSlope(gCurTask->player->playerIndex)]);
    TaskSleepForever();
}

void MetaKnightActionSlide(void)
{
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 7;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_SLIDE;
    t = gCurTask;
    if (t->player->prevMode != 7)
    {
        t->playerActionDone28 = 0;
        t->variant = 0;
    }
    switch (gCurTask->variant)
    {
    case 0:
        PlayerSetMotionXPreset(11, 1);
        TaskSetFrame(0x1201);
        TaskYieldTrampoline(1);
        PlayerStopAxes(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
        /* fallthrough */
    case 1:
        gCurTask->u80.attackAbility = ABILITY_SWORD;
        gCurTask->player->playerSlideBrakeTimer = 10;
        PlayerSetMotionXPreset(11, 0);
        gCurTask->player->hitBoxSet = gUnk_0873D03C;
        PlayerStartSfx(118, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SLIDE_DUST, 0);
        TaskSetFrame(0x1203);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        while ((u32)abs(gCurTask->velX) > 0x7FFF)
            TaskYieldTrampoline(1);
        gCurTask->variant = 2;
        /* fallthrough */
    case 2:
        gCurTask->player->hitBoxSet = 0;
        gCurTask->u80.attackAbility = ABILITY_NORMAL;
        break;
    }
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void MetaKnightActionSlideUpdate(void)
{
    struct Task *t;
    struct Task *t2;
    struct PlayerState *p;
    s32 v;
    s32 x;
    s32 k;

    v = PlayerCheckFallOrWater();
    if (v == 0)
    {
        t = gCurTask;
        k = t->variant;
        switch (k)
        {
        case 0:
            break;
        case 1:
            if (gTerrainResult[0] != 0)
            {
                PlayerCheckBump();
                PlayerStopAxes(1);
                gCurTask->player->requestedAction = META_KNIGHT_ACTION_STAND;
                break;
            }
            p = t->player;
            if ((s16)p->playerSlideBrakeTimer != 0)
            {
                p->playerSlideBrakeTimer--;
                if (p->playerSlideBrakeTimer == 0)
                    PlayerSetMotionXPreset(5, 72);
            }
            t2 = gCurTask;
            x = t2->velX;
            if (x < 0)
                x = -x;
            if ((u32)x > 0xA000)
                RegisterCollider((u8)gCurTaskIdx, t2->pixelX, t2->pixelY, gUnk_0873CA68);
            break;
        case 2:
            if (t->playerActionDone28 != 0)
            {
                t->player->hitBoxSet = (void *)v;
                t->player->requestedAction = META_KNIGHT_ACTION_STAND;
            }
            break;
        }
    }
    else
    {
        if (gCurTask->accelX == 0)
            PlayerSetMotionXPreset(5, 72);
    }
}

/* MATCH (512 bytes).  `xa` is load-bearing, not cosmetic.  It is the FIRST x
   of loop 2 and is referenced only inside that loop`s entry block, so it adds
   a fourth block-local quantity there.  With three quantities (the unk28
   index; the combined i*2/+i/*2 chain; the &gUnk_0873D986 pool address)
   local-alloc takes gcc 2.9`s hand-rolled `case 3:` sort in block_alloc,
   whose comparisons use the literal qty numbers 0/1/2 while EXCHANGE permutes
   qty_order - it swaps twice and leaves the identity order, so the index is
   allocated first and takes r0 while the *6 chain takes r1.  The record
   pointer is then `(set r (plus <chain> <pool>))`, set_preference reads
   XEXP(src,0) = the chain = hard r1, so the pointer allocno prefers r1;
   prune_preferences copies that into regs_someone_prefers[v] (v conflicts
   with it and is higher priority), find_reg pass 0 refuses r1 for v, v takes
   r2 and the pointer takes r1 - the r1/r2 swap seen in BOTH loops.  A fourth
   quantity pushes block_alloc onto the qsort path, which sorts correctly:
   chain->r0, pool->r1, index->r1, xa->r0; the pointer`s preference becomes
   r0, which it conflicts with and is pruned, so v is free to take r1. */
void MetaKnightActionLadder(void)
{
    struct Task *h1;
    struct Task *h2;
    struct Task *h3;
    struct Task *d;
    struct Task *a1;
    struct Task *b1;
    struct Task *c2;
    struct Task *a2;
    struct Task *b2;
    u16 *q;
    u16 *r;
    s32 v;
    s32 x;
    s32 xa;
    s32 k;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 9;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_LADDER;
    if (gCurTask->player->prevMode != 9)
    {
        PlayerStopAxes(3);
        gCurTask->facing = 1;
        h1 = gCurTask;
        h1->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
        h1->player->running = 0;
        ((u8 *)gCurTask->player)[80] = 1;
        q = gLatchedHeldKeys;
        h2 = gCurTask;
        if (q[h2->player->playerIndex] & 64)
        {
            h2->variant = 1;
            gCurTask->playerLadderStep = 0;
        }
        else
        {
            h2->variant = 2;
            gCurTask->playerLadderStep = 10;
        }
        h3 = gCurTask;
        h3->playerLadderSavedFacing = h3->facing;
        h3->posX = ((h3->pixelX & 0xFFF0) | 8) << 16;
    }
    d = gCurTask;
    d->playerBaseFrame = 0x11F4;
    k = d->variant;
    switch (k)
    {
    case 1:
        d->playerLadderDir = k;
        for (;;)
        {
            if (gCurTask->playerLadderStep == 0 || gCurTask->playerLadderStep == 6)
                PlaySfx(123);
            a1 = gCurTask;
            r = &((u16 *)gUnk_0873D986)[a1->playerLadderStep * 3];
            x = -r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a1->velY = v;
            x = r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a1->speedLimitY = v;
            a1->frame = r[0] + ((u16 *)a1)[35];
            TaskYieldTrampoline(r[1]);
            b1 = gCurTask;
            b1->playerLadderStep++;
            if (b1->playerLadderStep > 9)
                b1->playerLadderStep = 0;
        }
    case 2:
        c2 = gCurTask;
        c2->playerLadderDir = c2->variant;
        c2->player->playerLadderSfxTimer = 0;
        PlaySfx(124);
        for (;;)
        {
            a2 = gCurTask;
            r = &((u16 *)gUnk_0873D986)[a2->playerLadderStep * 3];
            xa = r[2];
            v = xa << 8;
            if (xa & 0x8000)
                v |= 0xFF000000;
            a2->velY = v;
            x = r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a2->speedLimitY = v;
            a2->frame = r[0] + ((u16 *)a2)[35];
            TaskYieldTrampoline(r[1]);
            b2 = gCurTask;
            b2->playerLadderStep++;
            if (b2->playerLadderStep > 13)
                b2->playerLadderStep = 10;
        }
    case 0:
        PlayerStopAxes(2);
        break;
    }
    TaskSleepForever();
}

/* MATCH (600 bytes).  The two `unk01 = 5` / `unk01 = 1` arms are NOT written
   as an if/else-if chain in the tail: they are labelled statements INSIDE
   case 0, sitting between the `else if ((...&64) != 0)` arm and the third
   arm (which is therefore reached by `goto arm3`).  The tail branches to them
   with the un-inverted conditions (`beq set5` / `bne set1`, both backward),
   and each returns.  gcc emits basic blocks in source order, so that source
   position is what puts the 11 instructions in the hole at 0x080424AE and
   makes the four literal pools land where the ROM has them - the previous
   4-byte residue was pool alignment padding caused purely by block order.
   Corroboration: at 0x08042562 the `movs r3,#1` for the `unk7A & 1` mask is
   still live inside set1 (`strb r3,[r0,#1]`), i.e. the constant 1 is CSEd
   across the goto, which only happens if set1 is a jump target. */
void MetaKnightActionLadderUpdate(void)
{
    struct Task *t;
    struct Task *ta;
    struct Task *tb;
    struct Task *tc;
    struct Task *td;
    struct Task *te;
    struct Task *tf;
    struct PlayerState *p;
    u16 *qa;
    u16 *qb;
    struct Task *tg;
    u16 *qd;
    s32 k;
    s32 n;

    t = gCurTask;
    k = t->variant;
    switch (k)
    {
    case 1:
        if (gTerrainResult[1] != 0 || (((u8 *)t->player)[72] & 4) != 0)
            t->velY = 0;
        qa = gLatchedHeldKeys;
        ta = gCurTask;
        if ((qa[ta->player->playerIndex] & 192) == 0)
            ta->variant = 0;
        else if ((qa[ta->player->playerIndex] & 128) != 0)
            ta->variant = 2;
        if (gCurTask->variant == 1)
            break;
        TaskSetEntry(MetaKnightActionLadder, gCurTaskIdx);
        break;
    case 2:
        p = t->player;
        k &= p->playerLadderSfxTimer;
        if (k != 0)
        {
            PlaySfx(124);
            gCurTask->player->playerLadderSfxTimer = 0;
        }
        else
        {
            p->playerLadderSfxTimer++;
        }
        qb = gLatchedHeldKeys;
        tb = gCurTask;
        if ((qb[tb->player->playerIndex] & 192) == 0)
            tb->variant = 0;
        else if ((qb[tb->player->playerIndex] & 64) != 0)
            tb->variant = 1;
        if (gCurTask->variant == 2)
            break;
        TaskSetEntry(MetaKnightActionLadder, gCurTaskIdx);
        break;
    case 0:
        if ((gLatchedPressedKeys[t->player->playerIndex] & 192) == 0)
            break;
        n = t->playerLadderDir;
        if (n == 1)
        {
            if ((gLatchedPressedKeys[t->player->playerIndex] & 64) != 0)
            {
                t->variant = n;
                tc = gCurTask;
                tc->playerLadderStep++;
                if (tc->playerLadderStep > 9)
                    tc->playerLadderStep = k;
            }
            else
            {
                t->variant = 2;
                gCurTask->playerLadderStep = 10;
            }
        }
        else if ((gLatchedPressedKeys[t->player->playerIndex] & 64) != 0)
        {
            t->variant = 1;
            gCurTask->playerLadderStep = k;
        }
        else
            goto arm3;
        goto callit;
    set5:
        tg->player->requestedAction = META_KNIGHT_ACTION_JUMP;
        return;
    set1:
        tg->player->requestedAction = META_KNIGHT_ACTION_STAND;
        return;
    arm3:
        t->variant = 2;
        td = gCurTask;
        td->playerLadderStep++;
        if (td->playerLadderStep > 13)
            td->playerLadderStep = 10;
    callit:
        TaskSetEntry(MetaKnightActionLadder, gCurTaskIdx);
        break;
    }
    qd = gLatchedPressedKeys;
    te = gCurTask;
    if ((qd[te->player->playerIndex] & 48) != 0)
    {
        te->facing = te->playerLadderSavedFacing;
        tf = gCurTask;
        tf->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
        if (tf->velY != 0)
            PlayerStopAxes(2);
        PlayerRequestLocomotion();
    }
    else if ((gTerrainResult[6] & 3) == 0)
    {
        te->facing = te->playerLadderSavedFacing;
        tg = gCurTask;
        if (tg->variant == 1)
            goto set5;
        if (tg->onGround & 1)
            goto set1;
        tg->player->requestedAction = META_KNIGHT_ACTION_FALL;
    }
}

void MetaKnightActionHurt(void)
{
    struct Task *t;
    struct Task *t5;
    struct Task *tj;
    struct Task *tt;
    struct PlayerState *p;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 17;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_HURT;
    t = gCurTask;
    p = t->player;
    if (p->prevMode != 17)
    {
        p->statusFlags &= ~PLAYER_STATUS_PALETTE_LOCKED;
        t->variant = 5;
    }
    while (1)
    {
        switch (gCurTask->variant)
        {
        case 5:
            gCurTask->player->running = 0;
            RequestScreenShake(2);
            t5 = gCurTask;
            if (t5->hitEffect & HIT_EFFECT_TERRAIN_DAMAGE)
                t5->variant = 4;
            else
                t5->variant = t5->hitEffect & 15;
            ((u8 *)gCurTask->player)[63] = 1;
            gCurTask->player->invulnerabilityTimer = 0x8000;
            PlayerStopAxes(3);
            continue;
        case 6:
            if (gCurTask->waterFlags & 1)
                PlayerSetWaterMotionY();
            SetPlayerInvulnerability(1, 96, gCurTask->player->playerIndex);
            TaskSleepForever();
            /* fallthrough */
        case 0:
            PlaySfx(0x107);
            if ((s8)gCurTask->hitDirection == 0)
                PlayerSetMotionXPreset(10, 32);
            else
                PlayerSetMotionXPreset(10, 33);
            TaskSetFrame(0x123B);
            TaskYieldTrampoline(16);
            break;
        case 1:
            PlaySfx(0x107);
            PlayerStartOffsetScript(16);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_HURT_FLAMES, 0);
            gCurTask->playerLoopCount = 0;
            do
            {
                TaskSetFrame(0x1244);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount++;
            } while ((s16)gCurTask->playerLoopCount <= 3);
            TaskSetFrame(0x1244);
            TaskYieldTrampoline(1);
            tj = gCurTask;
            goto spawn25;
        case 2:
            PlaySfx(0x107);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_HURT_SPARKS, 0);
            gCurTask->playerLoopCount = 0;
            do
            {
                PlayerStartOffsetScript(17);
                TaskSetFrame(0x123B);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x1249);
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount++;
            } while ((s16)gCurTask->playerLoopCount <= 7);
            TaskSetFrame(0x123B);
            TaskYieldTrampoline(1);
            tj = gCurTask;
        spawn25:
            CreatePlayerEffect(tj->player->playerIndex, 25, 0);
            break;
        case 3:
            PlaySfx(0x107);
            CreatePlayerEffect(gCurTask->player->playerIndex, 24, 0);
            PlayerStartOffsetScript(18);
            TaskSetFrame(0x124B);
            TaskYieldTrampoline(44);
            TaskSetFrame(0x123B);
            TaskYieldTrampoline(1);
            break;
        case 4:
            PlaySfx(0x107);
            gCurTask->onGround = 0;
            PlayerSetMotionYPreset(30);
            TaskSetFrame(0x123C);
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount++;
            } while ((s16)gCurTask->playerLoopCount <= 5);
            TaskSetFrame(0x11EC);
            TaskYieldTrampoline(2);
            goto again;
        }
        if ((s8)gCurTask->hitDirection == 0)
            PlayerSetMotionXPreset(10, 36);
        else
            PlayerSetMotionXPreset(10, 37);
        PlayerStartOffsetScript(15);
        TaskSetFrame(0x123C);
        TaskYieldTrampoline(3);
        gCurTask->playerLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount++;
        } while ((s16)gCurTask->playerLoopCount <= 4);
        if ((s8)gCurTask->hitDirection == 0)
            PlayerSetMotionXPreset(10, 34);
        else
            PlayerSetMotionXPreset(10, 35);
        PlayerStartOffsetScript(15);
        tt = gCurTask;
        if (tt->onGround & 1)
        {
            TaskSetFrame(0x11EC);
            TaskYieldTrampoline(3);
        }
        else
        {
            tt->frame++;
            TaskYieldTrampoline(3);
        }
        PlayerStopAxes(3);
    again:
        gCurTask->variant = 6;
    }
}

void MetaKnightActionHurtUpdate(void)
{
    switch (gCurTask->variant)
    {
    case 6:
        PlayerRequestStandOrFall();
        break;
    case 5:
        break;
    case 4:
        PlayerSetMotionXPreset(7, 72);
    case 0:
    case 1:
    case 2:
    case 3:
        if (PlayerHasCrossedWaterSurface(0))
        {
            gCurTask->variant = 6;
            TaskSetEntry(MetaKnightActionHurt, gCurTaskIdx);
        }
        break;
    }
    PlayerStopAtCeilingAndWall();
}

/* Stage entry (issue #85).  Clears the player's bit in gActivePlayerMask,
   decrements gActivePlayerCount, installs MetaKnightActionDieUpdate as Task.updateCallback, spawns five
   sub-tasks through CreatePlayerEffect (id 12 four times, then id 13) and picks a
   random signed 8.8 value into Task.velX from the camera x (gSpriteCameraX)
   and gFrameCount.

   Four shapes were load-bearing in the +/-1 chain:
     * `k` is a REAL LOCAL holding the 1 stored into PlayerState.paletteFlashMode, not a
       literal.  A literal store leaves the RTL as `(set (mem:QI) (const_int 1))`
       and reload invents the register only after cse, so there is no pseudo for
       cse to reuse; with a local, cse follows the TAKEN side of the `beq` into
       this else-arm (that label has LABEL_NUSES == 1) and still knows k == 1 at
       the `& 1`, rewriting `movs r0,#1` into `adds r0,r5,#0` (lesson 3.327).
     * The store must be the FIELD spelling `->unk22 = k = 1`.  `((u8 *)p)[34]`
       or a separate `k = 1;` statement emits the `movs` before the address
       computation; the field form emits address-then-`movs` (the offset 34
       exceeds strb's #31 immediate, so it still becomes `adds r0,#34`).
     * `m` caches `gFrameCount & 1` BEFORE `sign = 1`, which is what puts the
       `movs r5,#1` between the `ands` and the `cmp`.
     * `dx` (the camera delta) must be its own local: the ROM keeps it in r0 and
       the later `x << 8` in r2, and two registers mean two variables (3.322).
       Reusing one `d` for both puts the delta in r2.
   Sharing `k` with `sign` instead lets cse delete `sign = 1` in the first arm.

   `PlayerState.invulnerabilityTimer` (u16 at 0x12) had to exist as a real field: the ROM's
   `movs r0,#128; lsls r0,r0,#8; strh r0,[r1,#18]` only comes out of a struct
   member - every `*(u16 *)((u8 *)p + 18)` spelling adds an `adds r0,rN,#0`
   copy (lesson 3.323).  include/task.h now carries it, so the throw-away
   stand-in this body used is gone. */
void MetaKnightActionDie(void)
{
    struct Task *t;
    u16 *q;
    s32 i;
    s32 sign;
    s32 k;
    s32 m;
    s32 dx;
    s32 r;
    s32 d;
    s32 x;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 18;
    gCurTask->updateCallback = (u32)MetaKnightActionDieUpdate;
    gCurTask->lateUpdateCallback = 0;
    gActivePlayerCount--;
    gActivePlayerMask &= ~(1 << gCurTask->player->playerIndex);
    if (gCurTask->hitEffect == HIT_EFFECT_MID_BOSS)
        sub_08027548();
    gCurTask->player->actionFlags |= 8;
    gCurTask->player->statusFlags |= PLAYER_STATUS_NO_DRIFT;
    gCurTask->variant = 0;
    gCurTask->player->statusFlags &= ~PLAYER_STATUS_PALETTE_LOCKED;
    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    ((u8 *)gCurTask->player)[23] = 0;
    ((u16 *)gCurTask->player)[12] = 0;
    q = (u16 *)gCurTask->player;
    q[14] = 0;
    q[13] = 0;
    PlayerStopAxes(3);
    gCurTask->layer = 4;
    HoldPlayerCamera(gCurTask->player->playerIndex);
    StopAllSfx();
    StopAllSound();
    gPauseDisabled = 1;
    TaskSetFrame(0x123B);
    FreezeOtherTasks((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE | TASK_SKIP_LATE_UPDATE));
    SetRoomUpdateFlags(2);
    if ((gDispCnt & 0x400) == 0)
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
    }
    RequestScreenShake(4);
    TaskYieldTrampoline(1);
    PlaySfx(158);
    TaskYieldTrampoline(59);
    gCurTask->variant = 1;
    for (i = 0; i < 4; i++)
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_DEATH_STAR_RING, i);
    CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_META_KNIGHT_DEATH_BLAST, 0);
    gCurTask->player->paletteFlashMode = k = 1;
    gCurTask->player->invulnerabilityTimer = 0x8000;
    PlayBgm(BGM_LOST_LIFE);
    if (IsTaskBelowPlayerBounds(gCurTask))
    {
        PlayerSetMotionYPreset(32);
    }
    else
    {
        dx = gCurTask->pixelX - gSpriteCameraX;
        if (dx <= 55)
            sign = 1;
        else if (dx > 160)
            sign = -1;
        else
        {
            m = gFrameCount & 1;
            sign = 1;
            if (m == 0)
                sign = -1;
        }
        r = RandomRange(3) << 8;
        r |= RandomRange(16) << 4;
        if (r <= 255)
            r |= 256;
        t = gCurTask;
        x = r * sign;
        d = x << 8;
        if (x & 0x8000)
            d |= 0xFF000000;
        t->velX = d;
        t->speedLimitX = 0x40000;
        PlayerSetMotionYPreset(33);
    }
    TaskSetFrame(0x124C);
    TaskSleepForever();
}

void MetaKnightActionDieUpdate(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        break;
    case 1:
        if (gCurTask->velY > 0)
        {
            gCurTask->speedLimitY = 0x40000;
            if (gCurTask->pixelY - gSpriteCameraY > 199)
            {
                PlayerStopAxes(2);
                gCurTask->player->playerDieTimer = 90;
                gCurTask->variant = 2;
            }
        }
        if ((gCurTask->player->statusFlags & PLAYER_STATUS_TIMERS_FROZEN) == 0)
            PlayerUpdatePaletteFlash();
        break;
    case 2:
        if (--gCurTask->player->playerDieTimer == 0)
        {
            gStageRequest = STAGE_REQUEST_LOST_LIFE;
            TaskFree(gCurTaskIdx);
        }
        break;
    }
}

void MetaKnightActionRecoil(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 5;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_RECOIL;
    PlayerSetMotionXPreset(11, 16);
    PlayerSetMotionYPreset(22);
    gCurTask->onGround = 0;
    TaskSleepForever();
}

void MetaKnightActionRecoilUpdate(void)
{
    gCurTask->player->requestedAction = META_KNIGHT_ACTION_FALL;
}

void MetaKnightActionEnterDoor(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateCallback = 0;
    gCurTask->lateUpdateCallback = 0;
    gCurTask->player->running = 0;
    gPauseDisabled = 1;
    PlayerStopAxes(3);
    gCurTask->player->statusFlags |= PLAYER_STATUS_NO_DRIFT;
    RequestScreenShake(0);
    if (gInHub == 0)
    {
        StopAllSfx();
        FreezeOtherTasks((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE | TASK_SKIP_LATE_UPDATE));
        if ((gDispCnt & 0x400) == 0)
        {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1D00;
        }
        TaskYieldTrampoline(1);
    }
    EnterDoor();
    if (gInHub != 0)
        ((void (*)(void))CreateEntryDoorOpening)();
    if (gInHub == 0)
        PlaySfx(SE_ENTER_DOOR);
    if ((gCurTask->waterFlags & 1) == 0)
    {
        TaskSetFrame(0x1208);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    else
    {
        TaskSetFrame(0x1208);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
    gCurTask->frame++;
    TaskSleepForever();
}

void MetaKnightActionExitDoor(void)
{
    s32 a;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_EXIT_DOOR;
    TaskInitWaterFlags();
    sub_08021c74((s32)gPlayerDefaultTerrainBox, gCurTaskIdx);
    gCurTask->playerActionDone28 = 0;
    gCurTask->frame = -1;
    if (gEntryDoorEvent == 1)
    {
        ((void (*)(void))CameraResumeFollowFocus)();
        a = ((s32 (*)(void))CreateEntryDoorStageClearFlag)();
        TaskYieldTrampoline(1);
        sub_08026704(a);
    }
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    a = ((s32 (*)(void))CreateEntryDoorOpening)();
    gCurTask->frame = -1;
    TaskYieldTrampoline(4);
    gCurTask->spriteFlags = 0x2000;
    PlayerSetMotionXPreset(10, 9);
    gCurTask->frame = 0x120B;
    TaskYieldTrampoline(3);
    gCurTask->frame--;
    TaskYieldTrampoline(3);
    CloseDoorOpening(a);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    PlayerStopAxes(1);
    TaskSetFrameByFacing(0x1208);
    TaskYieldTrampoline(2);
    {
        s16 *p = (s16 *)gMetaKnightStandFrames;
        TaskSetFrame(p[((s32 (*)(s32))PlayerGetFacingSlope)((s8)gCurTask->player->playerIndex)]);
    }
    if (gEntryDoorEvent == 1)
    {
        TaskSetSkipMask((TASK_SKIP_MOVE | TASK_SKIP_UPDATE | TASK_SKIP_LATE_UPDATE), gCurTaskIdx);
        CreateStageUnlockPan();
        while (gCameraPanDone == 0)
            TaskYieldTrampoline(1);
        ((void (*)(void))CameraResumeFollowFocus)();
        TaskSetSkipMask(0, gCurTaskIdx);
    }
    gCurTask->spriteFlags = 0x4000;
    ((void (*)(void))ApplyEntryDoorEvent)();
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void MetaKnightActionSwim(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 15;
    gCurTask->updateState = META_KNIGHT_ACTION_HANDLER_SWIM;
    if (gCurTask->player->prevMode != 15)
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x30)
            gCurTask->variant = 3;
        else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x41)
            gCurTask->variant = 1;
        else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x80)
            gCurTask->variant = 2;
        else
            gCurTask->variant = 0;
        gCurTask->playerSwimPrevState = gCurTask->variant;
        PlayerSetWaterMotionY();
        ((u8 *)gCurTask->player)[61] = 0;
    }
    switch (gCurTask->variant)
    {
    case 0:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        if (gCurTask->playerSwimPrevState == 2)
        {
            TaskSetFrame(0x1239);
            TaskYieldTrampoline(3);
        }
        TaskSetFrame(0x1238);
        break;
    case 1:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        while (1)
        {
        lab1:
            gCurTask->unk28 = 0;
            PlaySfx(120);
            if (((u8 *)gCurTask->player)[92] & 1)
            {
                PlayerSetMotionYPreset(15);
                TaskSetFrame(0x1234);
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                TaskSetFrame(0x1230);
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                gCurTask->playerLoopCount = 0;
                do
                {
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                } while ((s16)++gCurTask->playerLoopCount <= 4);
                gCurTask->speedLimitY = 0x10000;
            }
            TaskSetFrame(0x1233);
            gCurTask->playerLoopCount = 0;
            do
            {
                if (gCurTask->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 14);
            {
                if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x41)
                    goto lab1;
            }
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
        gCurTask->player->playerSwimStrokeTimer = 15;
        TaskSetFrame(0x1239);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        break;
    case 3:
        PlayerSetMotionXPreset(11, 3);
        gCurTask->unk28 = 0;
        if (((u8 *)gCurTask->player)[92] & 1)
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 0x41)
                PlayerSetMotionYPreset(12);
            else
                PlayerSetMotionYPreset(13);
        }
        else
        {
            PlayerSetMotionYPreset(13);
            gCurTask->unk28 = 10;
        }
        gCurTask->player->playerSwimStrokeTimer = 15;
        while (1)
        {
            PlaySfx(120);
            TaskSetFrame(0x1228);
            TaskYieldTrampoline(5);
            gCurTask->playerLoopCount = 0;
            do
            {
                gCurTask->frame++;
                TaskYieldTrampoline(5);
            } while ((s16)++gCurTask->playerLoopCount <= 6);
        }
    }
    TaskSleepForever();
}
