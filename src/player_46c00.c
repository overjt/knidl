#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_46c00.c (0x08046C00-0x080474E7, issue #87).
 *
 * Player action bodies, part 16: action 40 and per-frame handler 37.
 * sub_08046c00 (action 40, mode 13) is sub_08044d04's sibling: the same
 * opening, a ground form (Task.unk73 = 0) and an air form (1) in two
 * variants (Task.unk30 = Task.waterFlags bit 0), the collider record
 * gUnk_0873C060 and the block hit-box sets gUnk_0873CDB4 (ground) /
 * gUnk_0873CDF4 (air) whose rows gUnk_0873CDBC, gUnk_0873CDFC and
 * gUnk_0873CE64 it steps with LoadPlayerHitBoxSet.  On the ground it probes the
 * metatile 20 pixels ahead (sub_0802259c); a solid one (bits 0-1) gives
 * the impact: sound 241, the screen shake RequestScreenShake(2) and effect 35.
 * Its handler sub_08047270 is sub_08045398's twin with the collider rows
 * gUnk_0873C074, gUnk_0873C0C0 and gUnk_0873C128, and in the air it
 * records the held left/right direction in Task.unk34. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 offsetX; u8 offsetY; u8 *boxes; };

struct M11R20 { u32 w[5]; };

extern struct M11R20 gPlayerBodyBoxes[];
extern struct M11R8 gPlayerHitBoxSets[];
extern u32 gUnk_0873C060[];
extern u32 gUnk_0873CDB4[];
extern u32 gUnk_0873CDBC[];
extern u32 gUnk_0873CDF4[];
extern u32 gUnk_0873CDFC[];
extern u32 gUnk_0873CE64[];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873C074[];
extern u32 gUnk_0873C0C0[];
extern u32 gUnk_0873C128[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_0802259c(u16 x, u16 y);
void RequestScreenShake(u16 a);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerLand(s32 a0);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_08046c00(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 37;
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        t->unk2C = -1;
        if (t->waterFlags & 1)
            t->unk30 = 1;
        else
            t->unk30 = 0;
    }
    gCurTask->unk34 = gCurTask->facing;
    gCurTask->unk80 = 9;
    if (gCurTask->onGround & 1)
        gCurTask->unk73 = 0;
    else
        gCurTask->unk73 = 1;
    switch (gCurTask->unk73) {
    case 0:
        PlaySfxIfLocalPlayer(130, gCurTask->player->playerIndex);
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873C060;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CDB4;
        gCurTask->player->hitBoxSet = &gPlayerHitBoxSets[gCurTask->player->playerIndex];
        LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)gUnk_0873CDBC);
        if (gCurTask->unk30 == 0) {
            gCurTask->unk2C++;
            TaskSetFrame(0x7D2);
            TaskYieldTrampoline(8);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++) {
                gCurTask->unk2C++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->unk2C * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->unk2C * 8));
            {
                u16 x;
                s16 r;
                if (gCurTask->facing == 1)
                    x = gCurTask->pixelX + 20;
                else
                    x = gCurTask->pixelX - 20;
                r = sub_0802259c(x, gCurTask->pixelY + 13);
                if (r & 3) {
                    PlaySfxIfLocalPlayer(241, gCurTask->player->playerIndex);
                    RequestScreenShake(2);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 0);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame--;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(8);
                } else {
                    TaskSetFrame(0x7D9);
                    TaskYieldTrampoline(11);
                }
            }
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            TaskSetFrame(0x7DD);
            TaskYieldTrampoline(2);
            TaskSetFrame(0x7DA);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        } else {
            gCurTask->unk2C++;
            TaskSetFrame(0x7DE);
            TaskYieldTrampoline(10);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++) {
                gCurTask->unk2C++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->unk2C * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDBC + gCurTask->unk2C * 8));
            {
                u16 x;
                s16 r;
                if (gCurTask->facing == 1)
                    x = gCurTask->pixelX + 20;
                else
                    x = gCurTask->pixelX - 20;
                r = sub_0802259c(x, gCurTask->pixelY + 13);
                if (r & 3) {
                    PlaySfxIfLocalPlayer(241, gCurTask->player->playerIndex);
                    RequestScreenShake(2);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 0);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 35, 1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                    gCurTask->frame--;
                    TaskYieldTrampoline(1);
                    gCurTask->frame++;
                    TaskYieldTrampoline(10);
                } else {
                    TaskSetFrame(0x7E5);
                    TaskYieldTrampoline(13);
                }
            }
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            TaskSetFrame(0x7E9);
            TaskYieldTrampoline(4);
            TaskSetFrame(0x7E6);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        PlaySfxIfLocalPlayer(131, gCurTask->player->playerIndex);
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873C060;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CDF4;
        gCurTask->player->hitBoxSet = &gPlayerHitBoxSets[gCurTask->player->playerIndex];
        LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)gUnk_0873CDFC);
        if (gCurTask->unk30 == 0) {
            gCurTask->unk2C++;
            TaskSetFrame(0x7EA);
            TaskYieldTrampoline(1);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 10; gCurTask->unk6C++) {
                gCurTask->unk2C++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDFC + gCurTask->unk2C * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CDFC + gCurTask->unk2C * 8));
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x7F6);
            TaskYieldTrampoline(1);
        } else {
            gCurTask->unk2C++;
            TaskSetFrame(0x7F7);
            TaskYieldTrampoline(1);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 14; gCurTask->unk6C++) {
                gCurTask->unk2C++;
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CE64 + gCurTask->unk2C * 8));
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            }
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CE64 + gCurTask->unk2C * 8));
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x807);
            TaskYieldTrampoline(1);
        }
        break;
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08047270(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 0:
        if (t->unk28 != 0)
            PlayerRequestLocomotion();
        else if (t->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = 23;
        {
            struct Task *u = gCurTask;
            if (u->unk2C != -1) {
                LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C074 + u->unk2C * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
        }
        PlayerSetMotionXPreset(0, 72);
        break;
    case 1:
        if (gLatchedHeldKeys[t->player->playerIndex] & 48) {
            if (gLatchedHeldKeys[t->player->playerIndex] & 16)
                t->unk34 = 1;
            else
                t->unk34 = -1;
        }
        {
            struct Task *u = gCurTask;
            if ((u->onGround & 1) || u->unk28 != 0) {
                u->player->hitBoxSet = 0;
                PlayerRequestLocomotion();
            } else if (u->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0) {
                gCurTask->player->requestedAction = 23;
            }
        }
        {
            struct Task *u = gCurTask;
            if (u->unk2C != -1) {
                if (u->unk30 == 0)
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C0C0 + u->unk2C * 8);
                else
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873C128 + u->unk2C * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 52);
                else
                    PlayerSetMotionXPreset(10, 54);
            } else {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 53);
                else
                    PlayerSetMotionXPreset(10, 55);
            }
        } else {
            if (gCurTask->unk30 == 0)
                PlayerSetMotionXPreset(11, 2);
            else
                PlayerSetMotionXPreset(11, 4);
        }
        break;
    }
    {
        struct Task *u = gCurTask;
        if ((u->onGround & 1) == 0) {
            if ((u->waterFlags & 1) == 0)
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        } else {
            PlayerLand(1);
        }
    }
    PlayerStopAtCeilingAndWall();
}
