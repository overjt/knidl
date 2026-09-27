#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_44d04.c (0x08044D04-0x080455C7, issue #87).
 *
 * Player action bodies, part 12: action 35 and per-frame handler 32.
 * sub_08044d04 (action 35, mode 13) is an attack with a ground form
 * (Task.unk73 = 0) and an air form (1), each in two variants picked by
 * Task.waterFlags bit 0 (Task.unk30).  It installs the player's collider
 * record gPlayerBodyBoxes[] (registered with M05's RegisterCollider) and
 * block hit-box set gPlayerHitBoxSets[] (tested by M09's TaskBreakBlocks) from
 * gUnk_0873BF28/gUnk_0873CCAC or gUnk_0873BF84/gUnk_0873CCFC, points
 * PlayerState.hitBoxSet at the set while the swing is live and steps it
 * through the 8-byte rows of gUnk_0873CCB4 (ground) or gUnk_0873CD04 /
 * gUnk_0873CD44 (air, two passes, counters Task.unk6C/unk6E) with
 * LoadPlayerHitBoxSet, with effects 31 and 28 and sounds 147/148; the air form
 * restores the facing Task.facing it saved in Task.unk34.  Its handler
 * sub_08045398 copies the collider row Task.unk2C of gUnk_0873BF3C
 * (ground) or gUnk_0873BF98/gUnk_0873BFD8 (air) with LoadPlayerBodyBoxRect and
 * registers it every frame, requests action 23 through PlayerHasCrossedWaterSurface and
 * picks the PlayerSetMotionXPreset preset from the held left/right keys. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 offsetX; u8 offsetY; u8 *boxes; };

struct M11R20 { u32 w[5]; };

extern struct M11R20 gPlayerBodyBoxes[];
extern struct M11R8 gPlayerHitBoxSets[];
extern u32 gUnk_0873BF28[];
extern u32 gUnk_0873CCAC[];
extern u32 gUnk_0873CCB4[];
extern u32 gUnk_0873BF84[];
extern u32 gUnk_0873CCFC[];
extern u32 gUnk_0873CD04[];
extern u32 gUnk_0873CD44[];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873BF3C[];
extern u32 gUnk_0873BF98[];
extern u32 gUnk_0873BFD8[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
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

void sub_08044d04(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 32;
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
    gCurTask->unk80 = 4;
    if (gCurTask->onGround & 1)
        gCurTask->unk73 = 0;
    else
        gCurTask->unk73 = 1;
    switch (gCurTask->unk73) {
    case 0:
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873BF28;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CCAC;
        if (gCurTask->unk30 == 0) {
            TaskSetFrame(0x4BA);
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 31, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            PlaySfxIfLocalPlayer(147, gCurTask->player->playerIndex);
            {
                struct Task *t = gCurTask;
                t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                t->unk2C++;
                LoadPlayerHitBoxSet(t->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + t->unk2C * 8));
            }
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } else {
            TaskSetFrame(0x4CA);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            CreatePlayerEffect(gCurTask->player->playerIndex, 31, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
            PlaySfxIfLocalPlayer(147, gCurTask->player->playerIndex);
            {
                struct Task *t = gCurTask;
                t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                t->unk2C++;
                LoadPlayerHitBoxSet(t->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + t->unk2C * 8));
            }
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk2C++;
            LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CCB4 + gCurTask->unk2C * 8));
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk2C++;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->unk2C = -1;
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        }
        break;
    case 1:
        gPlayerBodyBoxes[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873BF84;
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CCFC;
        gCurTask->player->hitBoxSet = 0;
        if (gCurTask->unk30 == 0) {
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++) {
                PlaySfxIfLocalPlayer(148, gCurTask->player->playerIndex);
                {
                    struct Task *t = gCurTask;
                    t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                    t->unk2C = 0;
                    LoadPlayerHitBoxSet(t->player->playerIndex, (s32)gUnk_0873CD04);
                }
                TaskSetFrame(0x4DA);
                TaskYieldTrampoline(1);
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 6; gCurTask->unk6E++) {
                    gCurTask->unk2C++;
                    LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CD04 + gCurTask->unk2C * 8));
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                }
            }
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x4E2);
            TaskYieldTrampoline(1);
        } else {
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++) {
                PlaySfxIfLocalPlayer(148, gCurTask->player->playerIndex);
                {
                    struct Task *t = gCurTask;
                    t->player->hitBoxSet = &gPlayerHitBoxSets[t->player->playerIndex];
                    t->unk2C = 0;
                    LoadPlayerHitBoxSet(t->player->playerIndex, (s32)gUnk_0873CD44);
                }
                TaskSetFrame(0x4E3);
                TaskYieldTrampoline(1);
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 10; gCurTask->unk6E++) {
                    gCurTask->unk2C++;
                    LoadPlayerHitBoxSet(gCurTask->player->playerIndex, (s32)((u8 *)gUnk_0873CD44 + gCurTask->unk2C * 8));
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                }
            }
            gCurTask->unk2C = -1;
            gCurTask->player->hitBoxSet = 0;
            gCurTask->facing = gCurTask->unk34;
            TaskSetFrame(0x4EF);
            TaskYieldTrampoline(1);
        }
        break;
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08045398(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 0:
        if (t->unk28 != 0) {
            PlayerRequestLocomotion();
        } else {
            if (t->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
                gCurTask->player->requestedAction = 23;
            {
                struct Task *u = gCurTask;
                if (u->unk2C != -1) {
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873BF3C + u->unk2C * 8);
                    RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                                 (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
                }
            }
        }
        PlayerSetMotionXPreset(0, 72);
        break;
    case 1:
        if ((t->onGround & 1) || t->unk28 != 0)
            PlayerRequestLocomotion();
        else if (t->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = 23;
        {
            struct Task *u = gCurTask;
            if (u->unk2C != -1) {
                if (u->unk30 == 0)
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873BF98 + u->unk2C * 8);
                else
                    LoadPlayerBodyBoxRect(u->player->playerIndex, (u8 *)gUnk_0873BFD8 + u->unk2C * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
        }
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 56);
                else
                    PlayerSetMotionXPreset(10, 58);
            } else {
                if (gCurTask->unk30 == 0)
                    PlayerSetMotionXPreset(10, 57);
                else
                    PlayerSetMotionXPreset(10, 59);
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
