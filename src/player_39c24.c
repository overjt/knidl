#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "collision.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_39c24.c (0x08039C24-0x0803AA63, issue #91).
 *
 * Player action body, part 8: action 21 (PlayerActionExitDoor, 3612 bytes), the
 * walk through a door the player entered with action 20.  It plays the
 * ability's door animations (gPlayerDoorAnims[ability][1..6], or
 * PlayerGetWaterDoorAnim for the form with Task.waterFlags bit 0 set), moves the player
 * by the door side kept in Task.unk2C, drives the door's M08 stage
 * objects through M07's helpers (CreateEntryDoorOpening ... sub_08026704) and the
 * cameras through CameraStartFollowingPlayer/CameraStartPlayersAtAnchor/ArePlayerCamerasDoneGliding/CameraResumeFollowFocus;
 * the second mode of gEntryDoorEvent also uploads an OBJ graphic
 * (LZ77UnCompWram of gUnk_080D07C8 into gUnk_02020000, then 0x06014000).
 * PlayerActionExitDoorUpdate, per-frame handler 19, only releases the player once
 * Task.unk28 is set. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 PlaySfx(s32 id);
void sub_08021c74(s8 *box, s32 id);
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void LoadAbilityTiles(void);                     /* M13, src/player_49738.c */

void PlayerActionExitDoor(void)
{
    s32 i;
    s32 r;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_EXIT_DOOR;
    gCurTask->facing = 1;
    gCurTask->frame = 0xFFFF;
    TaskInitWaterFlags();
    sub_08021c74((s8 *)gPlayerDefaultTerrainBox, gCurTaskIdx);
    if (gPlayerOrderShuffleCount == 0)
    {
        ShufflePlayerOrder();
        if (gPlayerCount > 1 && gEntryDoorEvent == 1)
        {
            for (i = 0; i < gPlayerCount; i++)
            {
                if (gPlayerHealth[i] == 0)
                    TaskSetSkipMask(4, i);
            }
        }
    }
    {
        struct Task *t = gCurTask;

        t->playerActionDone28 = 0;
        if (t->unk2C == 1 || t->unk2C == 3)
        {
            t->facing = -1;
            gCurTask->posX = (gCurTask->pixelX - 10) << 16;
        }
    }
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    if (gCurTask->unk2C == 0)
        r = CreateEntryDoorOpening();
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        if (!(t->waterFlags & 1))
        {
            t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][1];
            switch (t->player->ability)
            {
            case ABILITY_NORMAL:
            default:
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                break;
            case ABILITY_FIRE:
            case ABILITY_SPARK:
            case ABILITY_BURNING:
            case ABILITY_TORNADO:
            case ABILITY_BACKDROP:
            case ABILITY_THROW:
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                break;
            }
        }
        else
        {
            gCurTask->playerBaseFrame = PlayerGetWaterDoorAnim(1);
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    }
    switch (gEntryDoorEvent)
    {
    case 0:
    default:
        switch (gCurTask->unk2C)
        {
        case 0:
        case 2:
        {
            struct Task *t;

            PlayerSetMotionXPreset(10, 9);
            t = gCurTask;
            t->velX += t->unk2C << 13;
            break;
        }
        case 1:
        case 3:
        {
            struct Task *t;

            PlayerSetMotionXPreset(10, 10);
            t = gCurTask;
            t->velX -= (t->unk2C - 1) << 13;
            break;
        }
        }
        {
            struct Task *t = gCurTask;

            if (!(t->waterFlags & 1))
                t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][2];
            else
                gCurTask->playerBaseFrame = PlayerGetWaterDoorAnim(2);
        }
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        if (gCurTask->unk2C == 0)
            CloseDoorOpening(r);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
        PlayerStopAxes(1);
        if (gCurTask->unk2C == 0)
        {
            sub_08026584();
            CreateLocalPlayerArrow(gCurTask->player->playerIndex);
        }
        break;
    case 1:
        for (i = 0; i < gPlayerCount; i++)
            gPlayerStates[i].unk42 |= 64;
        if (gCurTask->unk2C != 0)
        {
            switch (gCurTask->unk2C)
            {
            case 0:
            case 2:
            {
                struct Task *t;

                gCurTask->facing = 1;
                PlayerSetMotionXPreset(10, 9);
                t = gCurTask;
                t->velX += t->unk2C << 13;
                break;
            }
            case 1:
            case 3:
            {
                struct Task *t;

                gCurTask->facing = -1;
                PlayerSetMotionXPreset(10, 10);
                t = gCurTask;
                t->velX -= (t->unk2C - 1) << 13;
                break;
            }
            }
            {
                struct Task *t = gCurTask;

                if (!(t->waterFlags & 1))
                    t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][2];
                else
                    gCurTask->playerBaseFrame = PlayerGetWaterDoorAnim(2);
            }
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 5; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
            {
                struct Task *t = gCurTask;

                if (!(t->waterFlags & 1))
                {
                    TaskSetFrame((s16)gUnk_0873D0F8[t->player->ability][0]);
                    TaskYieldTrampoline(1);
                }
                else
                {
                    if (t->player->ability == ABILITY_SWORD)
                        t->playerBaseFrame = 0x4A0;
                    else if (t->player->ability == ABILITY_HAMMER)
                        t->playerBaseFrame = 0x7C1;
                    else if (t->player->ability == ABILITY_PARASOL)
                        t->playerBaseFrame = 0x8C1;
                    else
                        t->playerBaseFrame = 221;
                    TaskSetFrame(gCurTask->playerBaseFrame);
                    TaskYieldTrampoline(1);
                }
            }
            PlayerStopAxes(1);
            gCurTask->player->unk42 &= 0xFFEF;
            TaskSetSkipMask(15, gCurTaskIdx);
            break;
        }
        LZ77UnCompWram(gUnk_080D07C8, gUnk_02020000);
        RequestCopy(3, (u32)gUnk_080DCC68, OBJ_VRAM0 + 0x4000, 0x400);
        CameraStartFollowingPlayer(gCurTaskIdx);
        {
            struct Task *t = gCurTask;

            t->spriteFlags = 0x2000;
            if (!(t->waterFlags & 1))
            {
                t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][3];
                switch (t->player->ability)
                {
                case ABILITY_NORMAL:
                default:
                    PlayerSetMotionXPreset(10, 11);
                    gCurTask->frame = gCurTask->playerBaseFrame;
                    TaskYieldTrampoline(4);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    break;
                case ABILITY_FIRE:
                case ABILITY_SPARK:
                case ABILITY_BURNING:
                case ABILITY_TORNADO:
                case ABILITY_BACKDROP:
                case ABILITY_THROW:
                    PlayerSetMotionXPreset(10, 12);
                    gCurTask->frame = gCurTask->playerBaseFrame;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    break;
                }
            }
            else
            {
                PlayerSetMotionXPreset(10, 11);
                gCurTask->frame = gCurTask->playerBaseFrame = PlayerGetWaterDoorAnim(3);
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
        PlayerSetMotionXPreset(10, 13);
        if (!(gCurTask->waterFlags & 1))
        {
            struct Task *t;

            PlayerSetMotionYPreset(20);
            t = gCurTask;
            t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][4];
        }
        else
        {
            PlayerSetMotionYPreset(21);
            gCurTask->playerBaseFrame = PlayerGetWaterDoorAnim(4);
        }
        gCurTask->frame = gCurTask->playerBaseFrame;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        CloseDoorOpening(r);
        gCurTask->frame = gCurTask->playerBaseFrame;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        PlayerStopAxes(3);
        i = CreateEntryDoorStageClearFlag();
        RequestScreenShake(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 20, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 20, 1);
        PlayerStartOffsetScript(14);
        PlaySfx(238);
        {
            struct Task *t = gCurTask;

            if (!(t->waterFlags & 1))
            {
                t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][5];
                switch (t->player->ability)
                {
                case ABILITY_NORMAL:
                default:
                    gCurTask->frame = gCurTask->playerBaseFrame;
                    TaskYieldTrampoline(24);
                    break;
                case ABILITY_FIRE:
                case ABILITY_SPARK:
                case ABILITY_BURNING:
                case ABILITY_TORNADO:
                case ABILITY_BACKDROP:
                case ABILITY_THROW:
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 5; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame = gCurTask->playerBaseFrame;
                        TaskYieldTrampoline(2);
                        gCurTask->frame++;
                        TaskYieldTrampoline(2);
                    }
                    break;
                }
            }
            else
            {
                gCurTask->frame = PlayerGetWaterDoorAnim(5);
                TaskYieldTrampoline(24);
            }
        }
        gCurTask->player->unk42 &= 0xFFEF;
        TaskSetSkipMask(14, gCurTaskIdx);
        CreateStageUnlockPan();
        {
            struct Task *t = gCurTask;

            if (!(t->waterFlags & 1))
            {
                switch (t->player->ability)
                {
                case ABILITY_NORMAL:
                default:
                    while (gCameraPanDone == 0)
                        TaskYieldTrampoline(1);
                    break;
                case ABILITY_FIRE:
                case ABILITY_SPARK:
                case ABILITY_BURNING:
                case ABILITY_TORNADO:
                case ABILITY_BACKDROP:
                case ABILITY_THROW:
                    for (;;)
                    {
                        gCurTask->frame = gCurTask->playerBaseFrame;
                        TaskYieldTrampoline(1);
                        if (gCameraPanDone != 0)
                            break;
                        TaskYieldTrampoline(1);
                        if (gCameraPanDone != 0)
                            break;
                        gCurTask->frame++;
                        TaskYieldTrampoline(1);
                        if (gCameraPanDone != 0)
                            break;
                        TaskYieldTrampoline(1);
                        if (gCameraPanDone != 0)
                            break;
                    }
                    break;
                }
            }
            else
            {
                while (gCameraPanDone == 0)
                    TaskYieldTrampoline(1);
            }
        }
        TaskSetSkipMask(0, gCurTaskIdx);
        gCurTask->spriteFlags = 0x4000;
        sub_08026704(i);
        RequestCopy(3, (u32)gUnk_02020000, OBJ_VRAM0 + 0x4000, 0x400);
        CameraStartPlayersAtAnchor();
        PlayerSetMotionXPreset(10, 14);
        if (!(gCurTask->waterFlags & 1))
        {
            struct Task *t;

            PlayerSetMotionYPreset(20);
            t = gCurTask;
            t->playerBaseFrame = gUnk_0873D3B8[t->player->ability][1];
            switch (t->player->ability)
            {
            case ABILITY_NORMAL:
            case ABILITY_MIKE:
            case ABILITY_CRASH:
            case ABILITY_LIGHT:
            {
                struct Task *u;

                gCurTask->frame = gCurTask->playerBaseFrame;
                TaskYieldTrampoline(2);
                u = gCurTask;
                u->frame = gPlayerDoorAnims[u->player->ability][6];
                TaskYieldTrampoline(2);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame--;
                    TaskYieldTrampoline(2);
                }
                break;
            }
            case ABILITY_PARASOL:
                gCurTask->playerBaseFrame = 0x841;
                goto anim;
            case ABILITY_SPARK:
                PlayerLoadSparkTiles();
            default:
            anim:
                gCurTask->frame = gCurTask->playerBaseFrame;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                break;
            }
            PlayerSetMotionXPreset(10, 15);
            while (1)
            {
                gCurTask->frame = gCurTask->playerBaseFrame;
                TaskYieldTrampoline(1);
                if (gCurTask->onGround & 1)
                    break;
                TaskYieldTrampoline(1);
                if (gCurTask->onGround & 1)
                    break;
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                if (gCurTask->onGround & 1)
                    break;
                TaskYieldTrampoline(1);
                if (gCurTask->onGround & 1)
                    break;
            }
        }
        else
        {
            PlayerSetMotionYPreset(21);
            gCurTask->frame = PlayerGetWaterDoorAnim(6);
            TaskYieldTrampoline(14);
            PlayerSetMotionXPreset(10, 15);
            while (!(gCurTask->onGround & 1))
                TaskYieldTrampoline(1);
        }
        PlayerStopAxes(3);
        gCurTask->player->bumpKind = 2;
        if (!(gCurTask->waterFlags & 1))
            PlayerPlayBump();
        sub_08026584();
        while (ArePlayerCamerasDoneGliding() == 0)
            TaskYieldTrampoline(1);
        CameraResumeFollowFocus();
        for (i = 0; i < gPlayerCount; i++)
        {
            gPlayerStates[i].unk42 &= 0xFFBF;
            if (gPlayerHealth[i] != 0)
                CreateLocalPlayerArrow(i);
            if (i != gCurTaskIdx)
                TaskSetSkipMask(0, i);
        }
        LoadAbilityTiles();
        break;
    }
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionExitDoorUpdate(void)
{
    if (gCurTask->playerActionDone28 != 0)
    {
        PlayerRequestLocomotion();
        gPauseDisabled = 0;
    }
}
