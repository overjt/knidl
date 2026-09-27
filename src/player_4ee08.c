#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4ee08.c (0x0804EE08-0x0804F613, issue #90).
 *
 * Action 49's sub-actions 4-8 (gUnk_0873B664) and sub-handlers 13-17
 * (gUnk_0873B688), each sub-action followed by its sub-handler: more
 * attacks of the same move set (sounds 168-170, 183, 184, 202 and 246,
 * M11's PlayerSetMotionYPreset steering, effect 44), the last one (sub_0804f450)
 * installing the hit boxes gPlayerDefaultBodyBox/gPlayerDefaultTerrainBox in PlayerState.
 * Sub-handler 17 (sub_0804f5bc, a push-less leaf) requests action 7, 20
 * or 23 from the ground flag and the key state. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (M12's PlayerActionBurningUpdate). */
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
extern struct Unk03005550 gTerrainResult;
extern u32 gPlayerDefaultBodyBox[];             /* stored to PlayerState.bodyBox as (u32)gPlayerDefaultBodyBox */
extern u32 gPlayerDefaultTerrainBox[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void RequestScreenShake(u16 a);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void FreezeOtherTasks(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerCheckBump(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void sub_0804e600(void);
s32 sub_0804f614(void);
void sub_0804f79c(void);
s32 sub_0804f7f8(s32 a);
s32 sub_0804f8ec(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_0804ee08(void)
{
    s32 i;

    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->player->unk14 = 0xFFFF;
        t->player->running = 1;
    }
    {
        struct Task *t = gCurTask;
        t->unk28 = t->unk46;
        t->unk46 = -1;
        t->frame = 0xCE9;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xCE7;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;
        t->unk46 = t->unk28;
        PlaySfxIfLocalPlayer(170, (u16)t->player->playerIndex);
    }
    PlayerSetMotionYPreset(43);
    gCurTask->player->unk14 = 23;
    while (1)
    {
        if (--gCurTask->player->unk14 == 0 || !(gLatchedHeldKeys[gCurTask->player->playerIndex] & 1))
            break;
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
    PlayerSetMotionYPreset(44);
    for (i = 4; i >= 0; i--)
    {
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
    gCurTask->unk70++;
    PlayerStopAxes(2);
    TaskSleepForever();
}

void sub_0804ef00(void)
{
    if ((s16)gCurTask->player->unk14 == -1)
        return;
    PlayerTurnToHeldDirection();
    while (!sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gTerrainResult.unk1 != 0 || (gCurTask->player->boundsClamp & 4))
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            if (t->player->bumpKind == 1)
            {
                t->unk20 = t->velY;
                t->velY = 0;
            }
            else
            {
                t->velY = -t->velY;
                t->speedLimitY = 0x50000;
            }
            gCurTask->variant = 6;
        }
        else
        {
            if (gTerrainResult.unk0 != 0)
            {
                struct Task *t = gCurTask;
                t->velX = -t->velX;
                t->accelX = -t->accelX;
                t->facing = -t->facing;
            }
            if ((s16)gCurTask->unk70 != 0)
                gCurTask->variant = 6;
        }
        break;
    }
    if (gCurTask->variant != 4)
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    PlayerSetMotionXPreset(12, 0);
}

void sub_0804efec(void)
{
    {
        struct Task *t = gCurTask;
        t->unk28 = t->velY;
        t->player->running = 1;
    }
    PlayerStopAxes(2);
    {
        struct Task *t = gCurTask;
        t->unk2C = t->unk46;
        t->unk46 = 0xFFFF;
        t->frame = 0xCE9;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xCE7;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gCurTask;
        t->unk46 = t->unk2C;
        t->onGround = 0;
    }
    gCurTask->velY = gCurTask->unk28;
    PlayerSetMotionYPreset(46);
    {
        struct Task *t = gCurTask;
        if (abs(t->unk28) <= 0x1FFFF)
            PlaySfxIfLocalPlayer(184, (u16)t->player->playerIndex);
        else if (abs(t->unk28) <= 0x2FFFF)
            PlaySfxIfLocalPlayer(183, (u16)t->player->playerIndex);
        else if (abs(t->unk28) <= 0x3FFFF)
            PlaySfxIfLocalPlayer(170, (u16)t->player->playerIndex);
        else if (abs(t->unk28) <= 0x57FFF)
            PlaySfxIfLocalPlayer(169, (u16)t->player->playerIndex);
        else
            PlaySfxIfLocalPlayer(168, (u16)t->player->playerIndex);
    }
    sub_0804f614();
    while (1)
    {
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
}

void sub_0804f124(void)
{
    if (gCurTask->velY == 0)
        return;
    PlayerTurnToHeldDirection();
    while (!sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gTerrainResult.unk1 != 0 || (gCurTask->player->boundsClamp & 4))
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            if (t->player->bumpKind == 1)
            {
                if (-t->velY > 0x57FFF)
                    PlaySfxIfLocalPlayer(246, (u16)t->player->playerIndex);
                gCurTask->unk20 = gCurTask->velY;
                gCurTask->velY = 0;
            }
            else
            {
                t->velY = -t->velY;
                t->speedLimitY = 0x50000;
            }
            gCurTask->variant = 6;
        }
        else
        {
            if (gTerrainResult.unk0 != 0)
            {
                struct Task *t;
                PlayerCheckBump();
                t = gCurTask;
                t->velX = -t->velX;
                t->accelX = -t->accelX;
                t->facing = -t->facing;
            }
            if (gCurTask->velY >= 0)
                gCurTask->variant = 6;
        }
        break;
    }
    if (gCurTask->variant != 5)
    {
        struct Task *t = gCurTask;
        t->unk24 = 0;
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    }
    PlayerSetMotionXPreset(12, 0);
}

void sub_0804f22c(void)
{
    gCurTask->player->running = 1;
    PlayerSetMotionYPreset(45);
    sub_0804f614();
    while (1)
    {
        sub_0804f79c();
        TaskYieldTrampoline(1);
    }
}

void sub_0804f258(void)
{
    PlayerTurnToHeldDirection();
    if (!sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = 23;
        else if (gCurTask->onGround & 1)
            gCurTask->variant = 7;
        else if (gTerrainResult.unk0 != 0)
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            t->velX = -t->velX;
            t->accelX = -t->accelX;
            t->facing = -t->facing;
        }
    }
    if (gCurTask->variant != 6)
    {
        struct Task *t = gCurTask;
        t->unk24 = 0;
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    }
    PlayerSetMotionXPreset(12, 0);
}

void sub_0804f30c(void)
{
    s32 flag;
    s32 m;

    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->unk28 = t->velY;
    }
    sub_0804f8ec(1);
    flag = 0;
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 1)
    {
        flag = 1;
        m = 358;
    }
    else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
    {
        flag = 1;
        m = 76;
    }
    else
        m = 179;
    {
        struct Task *t = gCurTask;
        t->unk28 = m = m * (t->unk28 >> 8);
        if (m <= 0x3FFF)
            flag = 0;
        if (m > 0x57FFF)
            PlaySfxIfLocalPlayer(246, (u16)t->player->playerIndex);
    }
    if (flag)
    {
        struct Task *t = gCurTask;
        t->unk24 = 1;
        t->frame = 0xCE8;
        TaskYieldTrampoline(2);
    }
    else
    {
        struct Task *t = gCurTask;
        if (t->unk28 <= 0x17FFF)
            t->unk28 = 0;
    }
    gCurTask->unk70++;
    TaskSleepForever();
}

void sub_0804f3e4(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        t->unk24 = 0;
        if (t->unk28 != 0)
        {
            t->variant = 5;
            if ((u32)gCurTask->unk28 > 0x80000)
                gCurTask->unk28 = 0x80000;
            gCurTask->velY = -gCurTask->unk28;
            gCurTask->speedLimitY = 0x80000;
        }
        else if (t->velX != 0)
            t->variant = 2;
        else
            t->variant = 1;
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    }
}

void sub_0804f450(void)
{
    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->unk46 = 0xFFFF;
        if (t->unk74 != 0)
        {
            PlayerStopAxes(3);
            RequestScreenShake(0);
            FreezeOtherTasks(15);
        }
    }
    if (gCurTask->onGround & 1)
    {
        gCurTask->frame = 0xCE9;
        TaskYieldTrampoline(2);
        if (gCurTask->unk74 == 0)
            PlayerSetMotionYPreset(48);
        gCurTask->frame = 0xCE7;
        TaskYieldTrampoline(3);
    }
    gCurTask->frame = 0xCD2;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x500);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x501);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x502);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x503);
    PlaySfxIfLocalPlayer(202, (u16)gCurTask->player->playerIndex);
    {
        struct Task *t = gCurTask;
        t->facing = t->unk6E;
    }
    {
        struct Task *t = gCurTask;
        t->player->bodyBox = (u32)gPlayerDefaultBodyBox;
        t->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        t->unk70++;
    }
    TaskSleepForever();
}

void sub_0804f5bc(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        if (t->unk74 != 0)
            t->player->requestedAction = 20;
        else
            t->player->requestedAction = 7;
    }
    if (gCurTask->player->requestedAction == 0 && (gCurTask->waterFlags & 1))
        gCurTask->player->requestedAction = 23;
}
