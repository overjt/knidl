#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4ffdc.c (0x0804FFDC-0x080507BB, issue #90).
 *
 * Action 58's sub-actions 18-21 (gUnk_0873B6AC) and sub-handlers 22-25
 * (gUnk_0873B6BC), interleaved in the ROM as 18, 22, 19, 23, 20, 24, 21,
 * 25, and their two helpers.  sub_0804ffdc is a long yield script driven by
 * the 8.8 velocity pairs gUnk_0873B6CC; sub_08050418 spawns task type #6's
 * variant 12 through sub_08053a44; sub_08050508 switches the player to mode
 * 17 with camera presets PlayerSetMotionXPreset(10, 24-27).  sub_08050664 re-binds
 * sub-action 2 when a direction is pressed or held for ten frames, and
 * sub_080506dc steers the player with the held direction (velocity pairs
 * gUnk_0873B724[Task.unk6E], PlayerState.unk10 counting the glide
 * frames). */

extern u8 gUnk_03001F34;
extern u16 gGameState;
extern s16 gPlayerHealth[];             /* per-player health (M02's HUD) */
extern s16 gMaxHealth;
extern u8 gUnk_020055C4;
extern u16 gUnk_0873B6CC[][2];
extern u32 gPlayerDefaultBodyBox[];             /* stored to PlayerState.bodyBox as (u32)gPlayerDefaultBodyBox */
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u8 gUnk_0873B6DC[];
extern u16 gUnk_0873B6E8[][6];
extern u16 gFrameCount;
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern u16 gUnk_0873B724[][4];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
s32 AddPlayerHealth(s32 a, s32 b);
void PlayerUpdate(void);
void sub_0803332c(void);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void sub_0803f6e0(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerActionStarRodFlight(void);
void sub_0804fee8(void);
s32 sub_08053a44(s8 player, u8 variant, s32 arg);
void sub_080502f0(void);
s32 sub_08050664(void);

void sub_0804ffdc(void)
{
    gUnk_03001F34 = 1;
    {
        struct Task *t = gCurTask;
        t->updateCallback = (u32)sub_080502f0;
        t->unk70 = 0;
        t->frame = 0xFFFF;
        if (gGameState != 20)
        {
            gPlayerHealth[t->player->playerIndex] = 0;
            AddPlayerHealth(gMaxHealth, t->player->playerIndex);
        }
    }
    {
        struct Task *t = gCurTask;
        t->posX = 0x80000;
        t->posY = 0xB00000;
    }
    TaskYieldTrampoline(65);
    {
        struct Task *t = gCurTask;
        t->frame = 0x1056;
        t->speedLimitX = 0x80000000;
        t->speedLimitY = 0x80000000;
        t->velX = 0x10000;
        t->velY = 0xFFFE0000;
    }
    TaskYieldTrampoline(58);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(20);
    {
        struct Task *t = gCurTask;
        t->velY = 0xFFFF8000;
        t->frame--;
    }
    TaskYieldTrampoline(8);
    gCurTask->frame--;
    TaskYieldTrampoline(12);
    {
        struct Task *t = gCurTask;
        t->velX = 0x8000;
        t->velY = 0x8000;
        t->frame = 0x1051;
    }
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(10);
    gCurTask->velX = 0x2000;
    TaskYieldTrampoline(10);
    gCurTask->velX = 0xFFFFE000;
    TaskYieldTrampoline(6);
    gCurTask->frame = 0x1040;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1042;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1045;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1048;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->velX = 0;
        t->velY = 0;
        t->frame = 0x104B;
    }
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104E;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->frame = 0x105D;
        t->velX = 0xFFFF0000;
    }
    TaskYieldTrampoline(16);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(16);
    if (gUnk_020055C4 == 0)
        sub_0803f6e0();
    {
        struct Task *t = gCurTask;
        u16 *e = gUnk_0873B6CC[t->unk2C];
        s32 v;

        t->velX = 0xFFFF8000;
        v = e[0] << 8;
        if (e[0] & 0x8000)
            v |= 0xFF000000;
        t->velY = v;
        TaskYieldTrampoline(16);
        t = gCurTask;
        v = e[1] << 8;
        if (e[1] & 0x8000)
            v |= 0xFF000000;
        t->velY = v;
    }
    TaskYieldTrampoline(16);
    gCurTask->frame = 0x105E;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1041;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1044;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1045;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x1048;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->velX = 0xFFFFE000;
        t->velY = 0x8000;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame = 0x1049;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104C;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104D;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0x104F;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->velX = 0;
        t->frame = 0x103F;
        t->velY = 0x2000;
    }
    TaskYieldTrampoline(6);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(14);
    gCurTask->velY = 0;
    TaskYieldTrampoline(14);
    {
        struct Task *t = gCurTask;
        t->unk70++;
        t->player->bodyBox = (u32)gPlayerDefaultBodyBox;
    }
    TaskSleepForever();
}

void sub_080502f0(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        t->unk73 = 1;
        gCurTask->updateCallback = (u32)PlayerUpdate;
        gCurTask->lateUpdateCallback = (u32)sub_0803332c;
        TaskSetEntry(sub_0804fee8, gCurTaskIdx);
        gUnk_03001F34 = 0;
    }
}

void sub_08050340(void)
{
    struct Task *t = gCurTask;

    t->unk46 = 0;
    t->unk28 = 0;
    TaskSleepForever();
}

void sub_0805035c(void)
{
    if (sub_08050664() == 0)
    {
        u8 k = gUnk_0873B6DC[(gLatchedHeldKeys[gCurTask->player->playerIndex] & 0xF0) >> 4];
        struct Task *t = gCurTask;
        u16 *row = gUnk_0873B6E8[t->unk28];

        if (t->unk28 == 0)
        {
            if (t->unk46 == 0)
                t->unk28 = k;
            else
                t->unk46--;
        }
        else if (t->unk28 == k)
        {
            if (row[t->unk46 + 1] != 0xFFFF)
                t->unk46++;
        }
        else
        {
            if (t->unk46 != 0)
                t->unk46--;
            else
                t->unk28 = k;
        }
        gCurTask->frame = row[gCurTask->unk46];
    }
}

void sub_08050418(void)
{
    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        PlaySfxIfLocalPlayer(155, (u16)t->player->playerIndex);
    }
    gCurTask->frame = 0x1040;
    TaskYieldTrampoline(4);
    gCurTask->frame = 0x1041;
    TaskYieldTrampoline(1);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
    sub_08053a44(gCurTask->player->playerIndex, 12, 0);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 9; gCurTask->unk6C++)
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk70++;
    TaskSleepForever();
}

void sub_080504d4(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        t->unk73 = 1;
        TaskSetEntry(PlayerActionStarRodFlight, gCurTaskIdx);
    }
}

void sub_08050508(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 17;
    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->player->invulnerability = 1;
    }
    {
        struct Task *t = gCurTask;
        t->player->invulnerabilityTimer = 0x8000;
        if (gFrameCount & 1)
            PlaySfxIfLocalPlayer(111, (u16)t->player->playerIndex);
        else
            PlaySfxIfLocalPlayer(112, (u16)t->player->playerIndex);
    }
    PlayerStopAxes(3);
    if ((s8)gCurTask->hitDirection == 0)
        PlayerSetMotionXPreset(10, 24);
    else
        PlayerSetMotionXPreset(10, 26);
    gCurTask->frame = 0x105D;
    TaskYieldTrampoline(4);
    if ((s8)gCurTask->hitDirection == 0)
        PlayerSetMotionXPreset(10, 25);
    else
        PlayerSetMotionXPreset(10, 27);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlayerStopAxes(1);
    SetPlayerInvulnerability(1, 96, gCurTask->player->playerIndex);
    gCurTask->unk70++;
    TaskSleepForever();
}

void sub_08050630(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        t->unk73 = 1;
        TaskSetEntry(sub_0804fee8, gCurTaskIdx);
    }
}

s32 sub_08050664(void)
{
    if (!(gLatchedPressedKeys[gCurTask->player->playerIndex] & 3)
        && (!(gLatchedHeldKeys[gCurTask->player->playerIndex] & 3)
            || (s16)++gCurTask->player->unk14 != 10))
        return 0;
    gCurTask->player->unk14 = 0;
    gCurTask->unk73 = 2;
    TaskSetEntry(sub_0804fee8, gCurTaskIdx);
    return 1;
}

void sub_080506dc(void)
{
    struct Task *t = gCurTask;

    if (t->player->mode == 13)
    {
        u16 k = gLatchedHeldKeys[t->player->playerIndex] & 0xF0;

        if (k != 0)
        {
            u16 *e;
            s32 v;

            t->unk6E = k >> 4;
            e = gUnk_0873B724[t->unk6E];
            v = e[0] << 8;
            if (e[0] & 0x8000)
                v |= 0xFF000000;
            t->velX = v;
            t->speedLimitX = 0x20000;
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->velY = v;
            t->speedLimitY = 0x20000;
            t->player->unk10 = 8;
        }
        else if (t->player->unk10 != 0)
        {
            u16 *e;
            s32 v;

            e = gUnk_0873B724[t->unk6E];
            v = e[2] << 8;
            if (e[2] & 0x8000)
                v |= 0xFF000000;
            t->velX = v;
            v = e[3] << 8;
            if (e[3] & 0x8000)
                v |= 0xFF000000;
            t->velY = v;
            if (--t->player->unk10 == 0)
            {
                gCurTask->velX = 0;
                gCurTask->velY = 0;
            }
        }
    }
}
