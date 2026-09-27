#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"
#include "player.h"

/* player_337f4.c (0x080337F4-0x080343BF, issue #92).
 *
 * Player mode bodies, part 1.  The six empty handlers sub_080337f4 ...
 * sub_08033808 are entries 59-61 of the action table gPlayerActions and
 * entry 56 of the per-frame table gPlayerActionHandlers (two of them, sub_080337f8
 * and sub_08033800, are dead exports nothing points at).  Then actions 1
 * and 2: PlayerActionStand enters mode 0 (per-frame handler 1, PlayerActionStandUpdate)
 * and PlayerActionWalk mode 1 (handler 2, PlayerActionWalkUpdate).  Their animations
 * come from gUnk_0873D0F8[ability][5] (column picked by M11's
 * sub_0803fd20, row 26 when PlayerState.mouthState == 1) and gUnk_0873D2E8. */

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 IsFullBlockAtPixel(u16 x, u16 y);
s32 sub_08022788(s32 y, s32 i);
void PlayerPlayBump(void);
void PlayerStopAxes(s32 a0);
void PlayerUpdateFlip(void);
s32 PlayerFaceHeldDirection(void);
void PlayerCheckBump(void);
s32 sub_0803fd20(s32 a0);
s32 PlayerCheckSkid(void);
s32 PlayerCheckJump(void);
s32 sub_0803fe68(void);
s32 PlayerCheckDuckOrSwallow(void);
s32 PlayerCheckLadder(void);
s32 PlayerCheckFloat(void);
s32 PlayerCheckBButton(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_08041438(void);
s32 CreatePlayerEffect(s32 band, s32 id, s32 payload);

void sub_080337f4(void)
{
}

void sub_080337f8(void)
{
}

void sub_080337fc(void)
{
}

void sub_08033800(void)
{
}

void sub_08033804(void)
{
}

void sub_08033808(void)
{
}

void PlayerActionStand(void)
{
    struct PlayerState *p;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 0;
    gCurTask->updateState = 1;

    if (gCurTask->player->prevMode != 0)
    {
        struct Task *t;
        struct Task *t2;

        PlayerStopAxes(3);
        t = gCurTask;
        t->unk28 = (u16)t->player->unk4E;
        t->unk2C = t->player->slope;
        if (t->player->wallSide != 0)
            t->player->unk46 = t->player->wallSide;
        gCurTask->player->running = 0;
        t2 = gCurTask;
        t2->player->unk40 &= 0xFFEF;
        t2->player->unk0F = 0;
        PlayerPlayBump();
    }
    gCurTask->player->unk33 = sub_0803fd20(gCurTask->player->playerIndex);
    p = gCurTask->player;
    p->unk35 = 0;
    p->unk34 = 0;
    if (gCurTask->player->mouthState == 1)
        gCurTask->unk46 = gUnk_0873D0F8[26][sub_0803fd20(gCurTask->player->playerIndex)];
    else
        gCurTask->unk46 = gUnk_0873D0F8[gCurTask->player->ability][sub_0803fd20(gCurTask->player->playerIndex)];
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
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case 15:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(6);
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void PlayerActionStandUpdate(void)
{
    s32 dir = gCurTask->facing;
    s32 turn = 0;

    if (PlayerFaceHeldDirection() != 0)
        PlayerUpdateFlip();
    while (PlayerCheckJump() == 0 && sub_0803fe68() == 0 && PlayerCheckEnterDoor() == 0
           && PlayerCheckLadder() == 0 && PlayerCheckDuckOrSwallow() == 0 && PlayerCheckFloat() == 0
           && PlayerCheckBButton() == 0)
    {
        if (PlayerCheckDropAbility() != 0)
            goto end;
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            if (gCurTask->player->unk46 != 0)
            {
                if (dir == gCurTask->facing)
                {
                    if (gCurTask->facing == 1)
                    {
                        if (IsFullBlockAtPixel(gCurTask->pixelX + 7, gCurTask->pixelY) == 0)
                            turn = 1;
                    }
                    else
                    {
                        if (IsFullBlockAtPixel(gCurTask->pixelX - 7, gCurTask->pixelY) == 0)
                            turn = 1;
                    }
                    if (turn == 0)
                    {
                        struct PlayerState *q = gCurTask->player;
                        u8 v = q->unk46;

                        if (v == 1 && (gLatchedHeldKeys[q->playerIndex] & 32))
                            turn = 1;
                        else if (v == 2 && (gLatchedHeldKeys[q->playerIndex] & 16))
                            turn = 1;
                    }
                }
                else
                {
                    turn = 1;
                }
            }
            else
            {
                turn = 1;
            }
        }
        if (turn != 0)
        {
            struct Task *u = gCurTask;
            s32 x = u->unk28;

            if (x != -1 && dir == u->facing && sub_08022788(x, u->player->playerIndex) != 0)
                turn = 0;
        }
        if (turn != 0)
        {
            gCurTask->player->requestedAction = 2;
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
                gCurTask->posX = (gCurTask->posX & 0xFFFF0000) | 0xF000;
            else
                gCurTask->posX = (gCurTask->posX & 0xFFFF0000) | 0x1000;
            if (gUnk_03001F30 == 0)
            {
                if (gCurTask->player->mouthState == 1)
                    gCurTask->unk46 = gUnk_0873D0F8[26][sub_0803fd20(gCurTask->player->playerIndex)];
                else
                    gCurTask->unk46 = gUnk_0873D0F8[gCurTask->player->ability][sub_0803fd20(gCurTask->player->playerIndex)];
            }
            else
            {
                gCurTask->unk46 = gUnk_0873D206[sub_0803fd20(gCurTask->player->playerIndex)];
            }
            TaskSetFrame(gCurTask->unk46);
            gCurTask->player->unk46 = 0;
            break;
        }
        {
            struct Task *w = gCurTask;

            if (w->player->slope != w->unk2C || dir != w->facing)
            {
                if (gUnk_03001F30 == 0)
                    TaskSetEntry(PlayerActionStand, gCurTaskIdx);
                else
                    TaskSetEntry(sub_08041438, gCurTaskIdx);
            }
        }
        break;
    }
end:
    gCurTask->unk2C = gCurTask->player->slope;
}

void PlayerActionWalk(void)
{
    struct PlayerState *p;
    struct Task *t;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 1;
    gCurTask->updateState = 2;
    gCurTask->unk2C = -1;
    PlayerSetMotionXPreset(1, 72);
    p = gCurTask->player;
    if (p->prevMode != 1)
    {
        p->running = 0;
        gCurTask->player->unk0F = 0;
        gCurTask->player->unk46 = 0;
        gCurTask->unk28 = 0;
        PlayerPlayBump();
        if (sub_0803fd20(gCurTask->player->playerIndex) == 4)
            gCurTask->variant = 1;
        else
            gCurTask->variant = 0;
    }
    if (gCurTask->variant == 0)
    {
        if (gCurTask->player->mouthState == 1)
        {
            while (1)
            {
                TaskSetFrame(0x153);
                TaskYieldTrampoline(gCurTask->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 3);
                TaskSetFrame(0x148);
                TaskYieldTrampoline(gCurTask->unk28 + 5);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 5);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 3);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
            }
        }
        gCurTask->unk46 = gUnk_0873D2E8[gCurTask->player->ability];
        switch (gCurTask->player->ability)
        {
        case 0:
        default:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(gCurTask->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 8);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                TaskSetFrame((s16)(gCurTask->unk46 - 4));
                TaskYieldTrampoline(gCurTask->unk28 + 8);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
            }
        case 1:
        case 2:
        case 5:
        case 19:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(gCurTask->unk28 + 2);
                gCurTask->unk6C = 0;
                do
                {
                    t = gCurTask;
                    t->frame++;
                    TaskYieldTrampoline(t->unk28 + 2);
                    gCurTask->unk6C++;
                } while ((s16)gCurTask->unk6C <= 10);
                TaskSetFrame((s16)(gCurTask->unk46 - 8));
                TaskYieldTrampoline(gCurTask->unk28 + 2);
                gCurTask->unk6C = 0;
                do
                {
                    t = gCurTask;
                    t->frame++;
                    TaskYieldTrampoline(t->unk28 + 2);
                    gCurTask->unk6C++;
                } while ((s16)gCurTask->unk6C <= 6);
            }
        case 4:
        case 15:
        case 16:
        case 17:
        case 22:
        case 23:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(gCurTask->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                TaskSetFrame((s16)(gCurTask->unk46 - 5));
                TaskYieldTrampoline(gCurTask->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 4);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
                t = gCurTask; t->frame++; TaskYieldTrampoline(t->unk28 + 2);
            }
        }
    }
    CreatePlayerEffect(gCurTask->player->playerIndex, 6, 0x200);
    if (gCurTask->player->mouthState == 1)
        gCurTask->unk46 = 0x15D;
    else
        gCurTask->unk46 = gUnk_0873D350[gCurTask->player->ability];
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
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void PlayerActionWalkUpdate(void)
{
    while (PlayerCheckSkid() == 0 && PlayerCheckJump() == 0 && sub_0803fe68() == 0
           && PlayerCheckEnterDoor() == 0 && PlayerCheckLadder() == 0 && PlayerCheckDuckOrSwallow() == 0
           && PlayerCheckFloat() == 0)
    {
        struct Task *t;

        if (PlayerCheckBButton() != 0)
            goto end;
        if (PlayerCheckDropAbility() != 0)
            goto end;
        t = gCurTask;
        if (t->velX == 0 && t->speedLimitX == 0)
        {
            t->player->requestedAction = 1;
        }
        else if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            gCurTask->player->requestedAction = 1;
        }
        else
        {
            struct Task *t2 = gCurTask;
            struct PlayerState *p = t2->player;
            s32 v = p->running;

            if (v != 0)
            {
                p->requestedAction = 3;
            }
            else
            {
                if ((gLatchedHeldKeys[p->playerIndex] & 48) == 0)
                {
                    s32 d = abs(t2->velX);

                    if ((u32)d <= 0xFFFF)
                        t2->unk28 = 2;
                }
                else
                {
                    t2->unk28 = v;
                }
                if (gCurTask->variant == 0)
                {
                    if (sub_0803fd20(gCurTask->player->playerIndex) == 4)
                    {
                        gCurTask->variant = 1;
                        TaskSetEntry(PlayerActionWalk, gCurTaskIdx);
                    }
                }
                else if (sub_0803fd20(gCurTask->player->playerIndex) != 4)
                {
                    gCurTask->variant = 0;
                    TaskSetEntry(PlayerActionWalk, gCurTaskIdx);
                }
            }
        }
        break;
    }
end:
    PlayerSetMotionXPreset(2, 72);
}
