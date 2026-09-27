#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4e78c.c (0x0804E78C-0x0804EE07, issue #90).
 *
 * Action 49's sub-actions 0-3 (gUnk_0873B664) and sub-handlers 9-12
 * (gUnk_0873B688), each sub-action followed by its sub-handler.  The
 * sub-actions are yield scripts (sub_0804e78c: sound 171, animations
 * 0xCCA-0xCDA and effect 44 four times; sub_0804eca4: sound 119,
 * effect 6 and camera preset PlayerSetMotionXPreset(11, 62)); the sub-handlers read
 * the keys and the ground flag Task.onGround, pick the next sub-action
 * (Task.unk73) and re-bind it through sub_0804e600, most of them through
 * the helper sub_0804f7f8's four key probes. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_0873BD50[];
extern u32 gUnk_0873CB3C[];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern struct Unk03005550 gTerrainResult;

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerUpdateFlip(void);
s32 PlayerFaceHeldDirection(void);
void PlayerCheckBump(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);

void LoadAbilityTiles(void);
void sub_0804e600(void);
s32 sub_0804f614(void);
s32 sub_0804f76c(void);
void sub_0804f79c(void);
s32 sub_0804f7f8(s32 a);
s32 sub_0804f8ec(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_0804e78c(void)
{
    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->unk46 = 0xFFFF;
        t->player->bodyBox = (u32)gUnk_0873BD50;
        t->player->terrainBox = (u32)gUnk_0873CB3C;
        PlaySfxIfLocalPlayer(171, (u16)t->player->playerIndex);
    }
    if (gCurTask->onGround & 1)
    {
        PlayerSetMotionYPreset(47);
        TaskSetFrame(0xCCA);
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    TaskSetFrame(0xCCC);
    TaskYieldTrampoline(2);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    gCurTask->frame = 0xCDA;
    TaskYieldTrampoline(2);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    {
        struct Task *t = gCurTask;
        t->unk6E = t->facing;
        t->unk70++;
        CreatePlayerEffect(t->player->playerIndex, 44, 0x100);
    }
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x200);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x300);
    CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x400);
    LoadAbilityTiles();
    TaskSleepForever();
}

void sub_0804e8f4(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        if (t->onGround & 1)
        {
            if (gLatchedHeldKeys[t->player->playerIndex] & 1)
                t->unk73 = 4;
            else
                t->unk73 = 1;
        }
        else
            t->unk73 = 6;
        gCurTask->unk46 = 0;
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    }
    sub_0804f8ec(0);
}

void sub_0804e97c(void)
{
    PlayerStopAxes(3);
    {
        struct Task *t = gCurTask;
        t->unk28 = t->player->unk4A;
    }
    sub_0804f614();
    {
        struct Task *t = gCurTask;
        if (t->unk46 == 0)
            t->unk2C = 0;
        else if (t->unk46 <= 7)
            t->unk2C = -1;
        else
            t->unk2C = 1;
    }
    if (gCurTask->unk46 != 0)
    {
        do
        {
            struct Task *t = gCurTask;
            t->unk46 += t->unk2C;
            if (t->unk46 > 15)
                t->unk46 = 0;
            TaskYieldTrampoline(1);
        } while (gCurTask->unk46 != 0);
    }
    while (1)
    {
        gCurTask->unk46 = 16;
        TaskYieldTrampoline(3);
        gCurTask->unk46++;
        TaskYieldTrampoline(3);
        gCurTask->unk46++;
        TaskYieldTrampoline(3);
        gCurTask->unk46++;
        TaskYieldTrampoline(3);
        gCurTask->unk46++;
        TaskYieldTrampoline(3);
        gCurTask->unk46--;
        TaskYieldTrampoline(3);
        gCurTask->unk46--;
        TaskYieldTrampoline(3);
        gCurTask->unk46--;
        TaskYieldTrampoline(3);
    }
}

void sub_0804ea7c(void)
{
    PlayerFaceHeldDirection();
    while (!sub_0804f7f8(0) && !sub_0804f7f8(1) && !sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) && gCurTask->unk28 == 1)
                break;
            if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->unk28 == 2)
                break;
            gCurTask->unk73 = 2;
        }
        break;
    }
    if (gCurTask->unk73 != 1)
    {
        struct Task *t = gCurTask;
        t->unk24 = 0;
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    }
}

void sub_0804eb28(void)
{
    sub_0804f614();
    PlayerSetMotionXPreset(12, 1);
    gCurTask->unk28 = sub_0804f76c();
    gCurTask->unk2C = gCurTask->player->unk4B;
    while (1)
    {
        sub_0804f79c();
        TaskYieldTrampoline(gCurTask->unk28);
    }
}

void sub_0804eb60(void)
{
    while (!sub_0804f7f8(0) && !sub_0804f7f8(1) && !sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gTerrainResult.unk0 != 0)
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            t->velX = -t->velX;
            t->accelX = -t->accelX;
            t->facing = -t->facing;
            gCurTask->unk24 = 0;
            TaskSetEntry(sub_0804e600, gCurTaskIdx);
            break;
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
        {
            if (gCurTask->facing == -1)
            {
                gCurTask->unk73 = 3;
                break;
            }
        }
        else if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->facing == 1)
        {
            gCurTask->unk73 = 3;
            break;
        }
        if (gCurTask->velX == 0 && gCurTask->speedLimitX == 0)
        {
            gCurTask->unk73 = 1;
            break;
        }
        if (gCurTask->unk2C != gCurTask->player->unk4B)
        {
            gCurTask->unk24 = 0;
            TaskSetEntry(sub_0804e600, gCurTaskIdx);
        }
        break;
    }
    if (gCurTask->unk73 != 2)
    {
        struct Task *t = gCurTask;
        t->unk24 = 0;
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
    }
    PlayerSetMotionXPreset(12, 1);
    gCurTask->unk28 = sub_0804f76c();
}

void sub_0804eca4(void)
{
    PlayerSetMotionXPreset(11, 62);
    PlaySfxIfLocalPlayer(119, (u16)gCurTask->player->playerIndex);
    gCurTask->unk34 = CreatePlayerEffect(gCurTask->player->playerIndex, 6, 60);
    TaskSleepForever();
}

void sub_0804ecec(void)
{
    while (!sub_0804f7f8(0) && !sub_0804f7f8(1) && !sub_0804f7f8(3) && !sub_0804f7f8(2))
    {
        if (gTerrainResult.unk0 != 0)
        {
            struct Task *t;
            PlayerCheckBump();
            t = gCurTask;
            t->velX = -t->velX;
            t->accelX = -t->accelX;
            t->facing = -t->facing;
            gCurTask->unk73 = 2;
            break;
        }
        if (gCurTask->velX == 0)
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            {
                if (gCurTask->facing == 1)
                {
                    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 32)
                        gCurTask->facing = -1;
                }
                else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
                    gCurTask->facing = 1;
                PlayerUpdateFlip();
                gCurTask->accelX = 0;
                gCurTask->unk73 = 2;
            }
            else
                gCurTask->unk73 = 1;
        }
        break;
    }
    if (gCurTask->unk73 != 3)
    {
        TaskSetEntry(sub_0804e600, gCurTaskIdx);
        gTasks[gCurTask->unk34].unk28 = -1;
    }
}
