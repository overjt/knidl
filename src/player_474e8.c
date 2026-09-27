#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_474e8.c (0x080474E8-0x08047FE7, issue #87).
 *
 * Player action bodies, part 17: actions 41-43 and per-frame handlers
 * 38-40, the last entries M12 holds.  sub_080474e8 (action 41, mode 13)
 * plays one of two animation scripts (0x8D2... or 0x8DF..., picked by
 * Task.waterFlags bit 0 and remembered in Task.unk2C) with sound 151 and
 * effect 36; its handler sub_080477cc calls PlayerRequestLocomotion once the script
 * has finished (Task.unk28 != 0) and requests action 23 when
 * PlayerHasCrossedWaterSurface(0) fires while Task.velY > 0.  sub_08047844 (action 42)
 * is a long scripted sequence (animations 0x8E5-0x8F7, sounds 180 and 274, effect
 * 38 steps 0-3) that holds PlayerState.unk42 bit 1 and releases it with
 * the HUD call SetPlayerAbility(0, -1, player); its handler sub_08047bd8 does
 * the same release when the ability PlayerState.ability is 11.
 * sub_08047c30 (action 43) is a re-entrant three-state machine: state 0
 * installs the hit boxes gUnk_0873C1B0 and gUnk_0873CEEC in the player
 * records gPlayerBodyBoxes[]/gPlayerHitBoxSets[] and steps the frame index
 * Task.unk46, state 1 plays frames 5-10 and waits while B is held,
 * state 2 plays the release.  Its handler sub_08047e74 shows frame
 * gUnk_0873DADE[Task.unk46][Task.unk28] (Task.unk28 = the ground flag
 * Task.onGround of the previous frame), registers the matching boxes and
 * re-binds the coroutine when the ground flag changes. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

extern struct M11R8 gPlayerHitBoxSets[];
extern struct M11R20 gPlayerBodyBoxes[];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873C1B0[];
extern u32 gUnk_0873CEEC[];
extern u16 gUnk_0873DADE[][2];
extern u32 gUnk_0873C1C4[];
extern u32 gUnk_0873C1EC[];
extern u32 gUnk_0873CEF4[];
extern u32 gUnk_0873CF1C[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 SetPlayerAbility(s32 a, s32 b, u32 c);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerLand(s32 a0);
s32 sub_0803e55c(void);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_080474e8(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 38;
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        if (t->waterFlags & 1)
            t->unk2C = 1;
        else
            t->unk2C = 0;
    }
    gCurTask->unk80 = 10;
    PlayerSetMotionXPreset(0, 72);
    PlaySfxIfLocalPlayer(151, gCurTask->player->playerIndex);
    if (gCurTask->unk2C == 0) {
        TaskSetFrame(0x8D2);
        TaskYieldTrampoline(4);
        if (gCurTask->onGround & 1)
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
        gCurTask->unk6C = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 4);
        CreatePlayerEffect(gCurTask->player->playerIndex, 36, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 36, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 36, 2);
        gCurTask->unk6C = 0;
        do {
            TaskSetFrame(0x8D8);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 2);
        TaskSetFrame(0x8DA);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
    } else {
        TaskSetFrame(0x8DF);
        TaskYieldTrampoline(4);
        if (gCurTask->onGround & 1)
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 3);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x8D5);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 36, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 36, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 36, 2);
        gCurTask->unk6C = 0;
        do {
            TaskSetFrame(0x8D8);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 2);
        TaskSetFrame(0x8DA);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x8E2);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_080477cc(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->unk28 != 0) {
        PlayerRequestLocomotion();
    } else if (t->velY > 0) {
        if (PlayerHasCrossedWaterSurface(0) != 0)
            gCurTask->player->requestedAction = 23;
    }
    u = gCurTask;
    if ((u->onGround & 1) == 0) {
        if ((u->waterFlags & 1) == 0)
            PlayerSetMotionYPreset(2);
        else
            PlayerSetMotionYPreset(13);
    } else {
        PlayerLand(1);
    }
    PlayerStopAtCeilingAndWall();
}

void sub_08047844(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 39;
    gCurTask->unk28 = 0;
    gCurTask->player->unk42 |= 2;
    PlayerSetMotionXPreset(0, 72);
    TaskSetFrame(0x8E5);
    TaskYieldTrampoline(16);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(16);
    TaskSetFrame(0x8EF);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x8E6);
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(8);
    TaskSetFrame(0x8EF);
    TaskYieldTrampoline(4);
    TaskSetFrame(0x8E6);
    TaskYieldTrampoline(12);
    PlaySfxIfLocalPlayer(180, gCurTask->player->playerIndex);
    TaskSetFrame(0x8F0);
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskSetFrame(0x8E7);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    CreatePlayerEffect(gCurTask->player->playerIndex, 38, 0);
    TaskSetFrame(0x8EA);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0x8E9);
        TaskYieldTrampoline(12);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        gCurTask->frame--;
        TaskYieldTrampoline(12);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
        PlaySfxIfLocalPlayer(180, gCurTask->player->playerIndex);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(12);
        CreatePlayerEffect(gCurTask->player->playerIndex, 38, (s16)gCurTask->unk6C + 1);
        gCurTask->frame++;
        TaskYieldTrampoline(12);
    } while ((s16)++gCurTask->unk6C <= 1);
    PlaySfxIfLocalPlayer(274, gCurTask->player->playerIndex);
    TaskSetFrame(0x8E9);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(0x8EB);
    TaskYieldTrampoline(4);
    CreatePlayerEffect(gCurTask->player->playerIndex, 38, 3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(16);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->player->unk42 &= 0xFFFD;
    SetPlayerAbility(0, -1, gCurTask->player->playerIndex);
    gCurTask->frame--;
    TaskYieldTrampoline(32);
    TaskSetFrame(0x8F6);
    TaskYieldTrampoline(6);
    if (gCurTask->onGround & 1) {
        PlayerSetMotionYPreset(37);
        TaskSetFrame(0x8F7);
        TaskYieldTrampoline(2);
    }
    TaskSetFrame(244);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08047bd8(void)
{
    if (sub_0803e55c() != 0) {
        struct Task *t = gCurTask;
        if (t->player->ability == 11) {
            t->player->unk42 &= 0xFFFD;
            SetPlayerAbility(0, -1, t->player->playerIndex);
        }
    } else if (gCurTask->unk28 != 0) {
        PlayerRequestLocomotion();
    }
}

void sub_08047c30(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 40;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            t->unk73 = 0;
            gCurTask->unk80 = 12;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        {
            struct Task *t = gCurTask;
            t->unk2C = -1;
            gPlayerBodyBoxes[t->player->playerIndex] = *(struct M11R20 *)gUnk_0873C1B0;
        }
        gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CEEC;
        gCurTask->unk28 = gCurTask->onGround;
        gCurTask->unk46 = 0;
        TaskYieldTrampoline(8);
        PlaySfxIfLocalPlayer(138, gCurTask->player->playerIndex);
        gCurTask->unk46 = 1;
        TaskYieldTrampoline(4);
        gCurTask->unk2C++;
        gCurTask->unk46 = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk2C++;
        gCurTask->unk46 = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk2C++;
        gCurTask->unk46 = 4;
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 1;
        if (gCurTask->onGround & 1) {
            CreatePlayerEffect(gCurTask->player->playerIndex, 39, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 39, 1);
        }
        /* fallthrough */
    case 1:
        gCurTask->unk2C = 3;
        gCurTask->unk46 = 5;
        TaskYieldTrampoline(2);
        gCurTask->unk2C = 4;
        gCurTask->unk46 = 6;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 7;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 8;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 9;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 10;
        while (gLatchedHeldKeys[gCurTask->player->playerIndex] & 2)
            TaskYieldTrampoline(1);
        gCurTask->unk73 = 2;
        /* fallthrough */
    case 2:
        gCurTask->unk2C = 1;
        gCurTask->unk46 = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 3;
        break;
    }
    TaskSleepForever();
}

void sub_08047e74(void)
{
    u16 *e = gUnk_0873DADE[gCurTask->unk46];

    sub_0803e55c();
    if (gCurTask->player->requestedAction != 0)
        return;
    switch (gCurTask->unk73) {
    case 1:
        if (gCurTask->unk28 != gCurTask->onGround)
            TaskSetEntry(sub_08047c30, gCurTaskIdx);
        /* fallthrough */
    case 0:
    case 2:
        gCurTask->frame = e[gCurTask->unk28];
        if (gCurTask->unk2C >= 0) {
            if (gCurTask->unk28 != 0) {
                LoadPlayerBodyBoxRect(gCurTask->player->playerIndex,
                             (u8 *)gUnk_0873C1C4 + gCurTask->unk2C * 8);
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex,
                             (s32)((u8 *)gUnk_0873CEF4 + gCurTask->unk2C * 8));
            } else {
                LoadPlayerBodyBoxRect(gCurTask->player->playerIndex,
                             (u8 *)gUnk_0873C1EC + gCurTask->unk2C * 8);
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex,
                             (s32)((u8 *)gUnk_0873CF1C + gCurTask->unk2C * 8));
            }
            RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                         (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->player->playerIndex],
                         gCurTask->player->playerIndex);
        }
        gCurTask->unk28 = gCurTask->onGround;
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
}
