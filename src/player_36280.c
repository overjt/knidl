#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_36280.c (0x08036280-0x08036C93, issue #91).
 *
 * Player action bodies, part 4: per-frame handler 9 and actions 10 and
 * 11.  PlayerActionFloatUpdate (handler 9) is the other half of M09's mode-14
 * coroutine PlayerActionFloat: a seven-state switch over Task.unk73 that
 * re-binds the coroutine with the next state (1 on a held A or up, 4 on
 * a newly-pressed B, 2/3/5 from the ground flags Task.onGround/unk7B).
 * PlayerActionDuck (action 10, mode 6) installs the scripts
 * gUnk_0873BD28/gUnk_0873CB24 in PlayerState.bodyBox/unk68 and plays
 * gUnk_0873D4BC[ability][column]; its handler PlayerActionDuckUpdate requests
 * action 11 on a newly-pressed A or B and 7 on the collision flag
 * gTerrainResult.unk5.  PlayerActionSlide (action 11, mode 7) installs the
 * attack hit-box set gUnk_0873CC84 in PlayerState.hitBoxSet; its handler
 * PlayerActionSlideUpdate registers the box gUnk_0873BE9C with M09's collision
 * registry RegisterCollider while the player moves faster than 0xE000 and
 * drops to state 1 below 0x8000. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gTerrainResult;
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873CB24[];
extern u16 gUnk_0873D4BC[][5];
extern u8 gUnk_03001F30;
extern u32 gUnk_0873CC84[];
extern s16 gUnk_0873D5CA[][2];
extern u8 gUnk_0873BE9C[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);
void PlayerActionFloat(void);
void PlayerStopAxes(s32 a0);
void PlayerStartSfx(s32 a0, u16 a1);
s32 PlayerFaceHeldDirection(void);
s32 PlayerLand(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerCheckBump(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerStopAtWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fe68(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void sub_08041e8c(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void PlayerActionFloatUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    if (gCurTask->unk73 != 6 && PlayerCheckDropAbility() != 0)
    {
        if (gCurTask->unk73 == 5)
            gCurTask->unk73 = 2;
        return;
    }
    PlayerTurnToHeldDirection();
    switch (gCurTask->unk73)
    {
    case 0:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
        {
            PlayerSetMotionYPreset(8);
        }
        else if (gCurTask->onGround & 1)
        {
            PlayerStopAxes(2);
        }
        else if (gCurTask->waterFlags & 1)
        {
            gCurTask->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        else
        {
            PlayerSetMotionYPreset(7);
        }
        gCurTask->onGround = 0;
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        break;
    case 1:
        if (PlayerCheckEnterDoor() != 0)
            break;
        gCurTask->onGround = 0;
        PlayerSetMotionYPreset(8);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        if (gCurTask->onGround & 1)
            PlayerStopAxes(2);
        if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
        {
            gCurTask->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        break;
    case 2:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (!(gCurTask->onGround & 1))
            PlayerSetMotionYPreset(7);
        else
            PlayerStopAxes(2);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        t = gCurTask;
        if (t->onGround & 1)
        {
            t->unk73 = 3;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            PlayerStopAxes(2);
            break;
        }
        if (gLatchedHeldKeys[t->player->playerIndex] & 65)
        {
            t->unk73 = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedPressedKeys[t->player->playerIndex] & 2)
        {
            t->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (!(t->waterFlags & 1))
            break;
        PlayerStopAxes(2);
        gCurTask->unk28 = -1;
        gCurTask->unk73 = 5;
        TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        break;
    case 3:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (!(gCurTask->onGround & 1))
            PlayerSetMotionYPreset(7);
        else
            PlayerStopAxes(2);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        u = gCurTask;
        if (!(u->onGround & 1))
        {
            u->unk73 = 2;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedHeldKeys[u->player->playerIndex] & 65)
        {
            u->unk73 = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedPressedKeys[u->player->playerIndex] & 2)
        {
            u->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        break;
    case 4:
        v = gCurTask;
        if (!(v->onGround & 1))
        {
            if (!(v->waterFlags & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        }
        PlayerSetMotionXPreset(7, 72);
        w = gCurTask;
        if (w->velY < 0)
        {
            if (gTerrainResult.unk1 != 0)
                w->velY = 0;
        }
        else if (w->onGround & 1)
        {
            if ((u32)w->velY > 0xC000)
                PlayerLand(1);
            else
                PlayerCheckLanding();
        }
        PlayerStopAtWall();
        break;
    case 5:
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtWall();
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
        {
            PlayerStopAxes(2);
            gCurTask->unk73 = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        }
        else if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 2)
        {
            PlayerStopAxes(2);
            gCurTask->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        }
        break;
    case 6:
        PlayerRequestLocomotion();
        x = gCurTask;
        if (x->onGround & 1)
        {
            if ((u32)x->velY > 0xC000)
                PlayerLand(1);
            else
                PlayerCheckLanding();
        }
        break;
    }
    if (PlayerHasCrossedWaterSurface(0) != 0)
    {
        gCurTask->player->mouthState = 0;
        gCurTask->player->requestedAction = 23;
    }
}

void PlayerActionDuck(void)
{
    struct Task *t;
    struct PlayerState *p;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 6;
    gCurTask->updateState = 10;
    t = gCurTask;
    if (t->player->prevMode != 6)
    {
        t->player->bodyBox = (u32)gUnk_0873BD28;
        t->player->terrainBox = (u32)gUnk_0873CB24;
        t->unk2C = t->player->slope;
        PlayerSetMotionXPreset(0, 72);
    }
    gCurTask->unk28 = 8;
    gCurTask->player->unk33 = sub_0803fd20(gCurTask->player->playerIndex);
    p = gCurTask->player;
    p->unk35 = 0;
    p->unk34 = 0;
    gCurTask->unk46 = gUnk_0873D4BC[gCurTask->player->ability][sub_0803fd20(gCurTask->player->playerIndex)];
    switch (gCurTask->player->ability)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
    case 15:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(6);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(6);
            }
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void PlayerActionDuckUpdate(void)
{
    s32 dir = gCurTask->facing;

    PlayerFaceHeldDirection();
    while (sub_0803fe68() == 0 && PlayerCheckDropAbility() == 0)
    {
        u16 *q = gLatchedPressedKeys;
        struct Task *t = gCurTask;

        if (q[t->player->playerIndex] & 3)
        {
            t->player->requestedAction = 11;
            break;
        }
        if (!(gLatchedHeldKeys[t->player->playerIndex] & 128))
        {
            PlayerRequestLocomotion();
            break;
        }
        if (gTerrainResult.unk5 != 0)
        {
            if (t->unk28 == 0)
            {
                t->onGround = 0;
                gCurTask->player->requestedAction = 7;
                gCurTask->unk84 = 0;
                gCurTask->posY += 0x10000;
                break;
            }
            t->unk28--;
        }
        {
            struct Task *w = gCurTask;

            if (w->player->slope != w->unk2C || dir != w->facing)
            {
                if (gUnk_03001F30 == 0)
                    TaskSetEntry(PlayerActionDuck, gCurTaskIdx);
                else
                    TaskSetEntry(sub_08041e8c, gCurTaskIdx);
            }
        }
        break;
    }
    gCurTask->unk2C = gCurTask->player->slope;
    if (gTerrainResult.unk0 != 0)
        PlayerStopAxes(1);
}

void PlayerActionSlide(void)
{
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 7;
    gCurTask->updateState = 11;
    t = gCurTask;
    if (t->player->prevMode != 7)
    {
        t->unk28 = 0;
        t->unk73 = 0;
        gCurTask->player->unk14 = 10;
        PlayerStartSfx(118, gCurTask->player->playerIndex);
        gCurTask->player->hitBoxSet = gUnk_0873CC84;
        PlayerSetMotionXPreset(11, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 8, 0);
    }
    switch (gCurTask->unk73)
    {
    case 0:
        gCurTask->unk46 = gUnk_0873D5CA[gCurTask->player->ability][0];
        switch (gCurTask->player->ability)
        {
        case 0:
        default:
            TaskSetFrame(gCurTask->unk46);
            TaskSleepForever();
        case 1:
        case 2:
        case 5:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
            }
        case 4:
        case 15:
        case 16:
        case 17:
        case 19:
        case 22:
        case 23:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
    case 1:
        TaskSetFrame(gUnk_0873D5CA[gCurTask->player->ability][1]);
        break;
    }
    gCurTask->player->hitBoxSet = 0;
    gCurTask->unk28++;
    TaskSleepForever();
}

void PlayerActionSlideUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;

    while (1)
    {
        if (sub_0803fe68() != 0)
        {
            if (gCurTask->accelX == 0)
                PlayerSetMotionXPreset(5, 72);
            break;
        }
        if (PlayerCheckDropAbility() != 0)
            break;
        t = gCurTask;
        if (t->unk28 != 0)
        {
            t->player->requestedAction = 1;
        }
        else if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(1);
            gCurTask->player->requestedAction = 1;
        }
        else
        {
            if (abs(t->velX) <= 0x7FFF)
            {
                t->unk73 = 1;
                TaskSetEntry(PlayerActionSlide, gCurTaskIdx);
            }
            if (abs(gCurTask->velX) > 0xE000)
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873BE9C);
        }
        break;
    }
    p = gCurTask->player;
    if ((s16)p->unk14 == 0)
    {
        PlayerSetMotionXPreset(5, 72);
        gCurTask->player->unk14--;
    }
    else if ((s16)p->unk14 > 0)
    {
        p->unk14--;
    }
}
