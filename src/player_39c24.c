#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"
#include "actor.h"

/* player_39c24.c (0x08039C24-0x0803AA63, issue #91).
 *
 * Player action body, part 8: action 21 (PlayerActionExitDoor, 3612 bytes), the
 * walk through a door the player entered with action 20.  It plays the
 * ability's door animations (gPlayerDoorAnims[ability][1..6], or
 * sub_0803f7e0 for the form with Task.waterFlags bit 0 set), moves the player
 * by the door side kept in Task.unk2C, drives the door's M08 stage
 * objects through M07's helpers (sub_080264b0 ... sub_08026704) and the
 * cameras through sub_08027850/sub_08027908/sub_08027a30/sub_08027a60;
 * the second mode of gUnk_0200AF00 also uploads an OBJ graphic
 * (LZ77UnCompWram of gUnk_080D07C8 into gUnk_02020000, then 0x06014000).
 * PlayerActionExitDoorUpdate, per-frame handler 19, only releases the player once
 * Task.unk28 is set. */

void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 PlaySfx(s32 id);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSleepForever(void);
void TaskSetFrame(s32 a);
void sub_08021c74(s8 *box, s32 id);
void TaskInitWaterFlags(void);
void RequestScreenShake(u16 a);
s32 sub_080264b0(void);
void sub_0802651c(s32 i);
s32 sub_0802653c(void);
s32 sub_08026584(void);
void sub_08026704(s32 i);
s32 sub_0802672c(void);
void sub_08027850(s32 a);
void sub_08027908(void);
s32 sub_08027a30(void);
void sub_08027a60(void);
void PlayerPlayBump(void);
void PlayerStopAxes(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void sub_0803f6e0(void);
u16 sub_0803f7e0(u16 a0);
s32 PlayerRequestLocomotion(void);
void sub_08040808(s32 a0);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void LoadAbilityTiles(void);                     /* M13, src/player_49738.c */
void sub_08049a58(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void PlayerActionExitDoor(void)
{
    s32 i;
    s32 r;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateState = 19;
    gCurTask->facing = 1;
    gCurTask->frame = 0xFFFF;
    TaskInitWaterFlags();
    sub_08021c74((s8 *)gPlayerDefaultTerrainBox, gCurTaskIdx);
    if (gUnk_020055C4 == 0)
    {
        sub_0803f6e0();
        if (gPlayerCount > 1 && gUnk_0200AF00 == 1)
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

        t->unk28 = 0;
        if (t->unk2C == 1 || t->unk2C == 3)
        {
            t->facing = -1;
            gCurTask->posX = (gCurTask->pixelX - 10) << 16;
        }
    }
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    if (gCurTask->unk2C == 0)
        r = sub_080264b0();
    TaskYieldTrampoline(4);
    {
        struct Task *t = gCurTask;

        if (!(t->waterFlags & 1))
        {
            t->unk46 = gPlayerDoorAnims[t->player->ability][1];
            switch (t->player->ability)
            {
            case 0:
            default:
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                break;
            case 1:
            case 2:
            case 5:
            case 19:
            case 22:
            case 23:
                TaskSetFrame(gCurTask->unk46);
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
            gCurTask->unk46 = sub_0803f7e0(1);
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    }
    switch (gUnk_0200AF00)
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
                t->unk46 = gPlayerDoorAnims[t->player->ability][2];
            else
                gCurTask->unk46 = sub_0803f7e0(2);
        }
        TaskSetFrame(gCurTask->unk46);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        if (gCurTask->unk2C == 0)
            sub_0802651c(r);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
        PlayerStopAxes(1);
        if (gCurTask->unk2C == 0)
        {
            sub_08026584();
            sub_08040808(gCurTask->player->playerIndex);
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
                    t->unk46 = gPlayerDoorAnims[t->player->ability][2];
                else
                    gCurTask->unk46 = sub_0803f7e0(2);
            }
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
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
                    if (t->player->ability == 4)
                        t->unk46 = 0x4A0;
                    else if (t->player->ability == 9)
                        t->unk46 = 0x7C1;
                    else if (t->player->ability == 10)
                        t->unk46 = 0x8C1;
                    else
                        t->unk46 = 221;
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(1);
                }
            }
            PlayerStopAxes(1);
            gCurTask->player->unk42 &= 0xFFEF;
            TaskSetSkipMask(15, gCurTaskIdx);
            break;
        }
        LZ77UnCompWram(gUnk_080D07C8, gUnk_02020000);
        RequestCopy(3, (u32)gUnk_080DCC68, 0x06014000, 0x400);
        sub_08027850(gCurTaskIdx);
        {
            struct Task *t = gCurTask;

            t->spriteFlags = 0x2000;
            if (!(t->waterFlags & 1))
            {
                t->unk46 = gPlayerDoorAnims[t->player->ability][3];
                switch (t->player->ability)
                {
                case 0:
                default:
                    PlayerSetMotionXPreset(10, 11);
                    gCurTask->frame = gCurTask->unk46;
                    TaskYieldTrampoline(4);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    break;
                case 1:
                case 2:
                case 5:
                case 19:
                case 22:
                case 23:
                    PlayerSetMotionXPreset(10, 12);
                    gCurTask->frame = gCurTask->unk46;
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
                gCurTask->frame = gCurTask->unk46 = sub_0803f7e0(3);
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
            t->unk46 = gPlayerDoorAnims[t->player->ability][4];
        }
        else
        {
            PlayerSetMotionYPreset(21);
            gCurTask->unk46 = sub_0803f7e0(4);
        }
        gCurTask->frame = gCurTask->unk46;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        sub_0802651c(r);
        gCurTask->frame = gCurTask->unk46;
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
        i = sub_0802653c();
        RequestScreenShake(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 20, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 20, 1);
        PlayerStartOffsetScript(14);
        PlaySfx(238);
        {
            struct Task *t = gCurTask;

            if (!(t->waterFlags & 1))
            {
                t->unk46 = gPlayerDoorAnims[t->player->ability][5];
                switch (t->player->ability)
                {
                case 0:
                default:
                    gCurTask->frame = gCurTask->unk46;
                    TaskYieldTrampoline(24);
                    break;
                case 1:
                case 2:
                case 5:
                case 19:
                case 22:
                case 23:
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
                    {
                        gCurTask->frame = gCurTask->unk46;
                        TaskYieldTrampoline(2);
                        gCurTask->frame++;
                        TaskYieldTrampoline(2);
                    }
                    break;
                }
            }
            else
            {
                gCurTask->frame = sub_0803f7e0(5);
                TaskYieldTrampoline(24);
            }
        }
        gCurTask->player->unk42 &= 0xFFEF;
        TaskSetSkipMask(14, gCurTaskIdx);
        sub_0802672c();
        {
            struct Task *t = gCurTask;

            if (!(t->waterFlags & 1))
            {
                switch (t->player->ability)
                {
                case 0:
                default:
                    while (gUnk_020055E8 == 0)
                        TaskYieldTrampoline(1);
                    break;
                case 1:
                case 2:
                case 5:
                case 19:
                case 22:
                case 23:
                    for (;;)
                    {
                        gCurTask->frame = gCurTask->unk46;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                        gCurTask->frame++;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                    }
                    break;
                }
            }
            else
            {
                while (gUnk_020055E8 == 0)
                    TaskYieldTrampoline(1);
            }
        }
        TaskSetSkipMask(0, gCurTaskIdx);
        gCurTask->spriteFlags = 0x4000;
        sub_08026704(i);
        RequestCopy(3, (u32)gUnk_02020000, 0x06014000, 0x400);
        sub_08027908();
        PlayerSetMotionXPreset(10, 14);
        if (!(gCurTask->waterFlags & 1))
        {
            struct Task *t;

            PlayerSetMotionYPreset(20);
            t = gCurTask;
            t->unk46 = gUnk_0873D3B8[t->player->ability][1];
            switch (t->player->ability)
            {
            case 0:
            case 7:
            case 20:
            case 21:
            {
                struct Task *u;

                gCurTask->frame = gCurTask->unk46;
                TaskYieldTrampoline(2);
                u = gCurTask;
                u->frame = gPlayerDoorAnims[u->player->ability][6];
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->frame--;
                    TaskYieldTrampoline(2);
                }
                break;
            }
            case 10:
                gCurTask->unk46 = 0x841;
                goto anim;
            case 2:
                sub_08049a58();
            default:
            anim:
                gCurTask->frame = gCurTask->unk46;
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
                gCurTask->frame = gCurTask->unk46;
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
            gCurTask->frame = sub_0803f7e0(6);
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
        while (sub_08027a30() == 0)
            TaskYieldTrampoline(1);
        sub_08027a60();
        for (i = 0; i < gPlayerCount; i++)
        {
            gPlayerStates[i].unk42 &= 0xFFBF;
            if (gPlayerHealth[i] != 0)
                sub_08040808(i);
            if (i != gCurTaskIdx)
                TaskSetSkipMask(0, i);
        }
        LoadAbilityTiles();
        break;
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void PlayerActionExitDoorUpdate(void)
{
    if (gCurTask->unk28 != 0)
    {
        PlayerRequestLocomotion();
        gUnk_03001F34 = 0;
    }
}
