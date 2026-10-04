#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "save.h"

/* player_3919c.c (0x0803919C-0x08039C23, issue #91).
 *
 * Player action bodies, part 7: actions 17 and 20.  PlayerActionDie (action
 * 17) is the player's death: it installs sub_080396a4 as the task's
 * per-frame callback Task.updateCallback (state 1 falls until the player is below
 * the screen, state 2 waits PlayerState.unk14 frames, state 4 leaves for
 * the results screen), counts the players whose health gPlayerHealth[] is
 * not 0, plays the lost-life or game-over music, loops the fall animation
 * until the callback reaches state 3, and when gLivingPlayerCount drops to 0
 * raises M02's stage request gStageRequest = 6; Task.variant = 4 or 5 then
 * says whether the player has lives left (gPlayerLives[]).
 * PlayerActionEnterDoor (action 20) enters a door: it stops the player, plays the
 * landing or crouch animation picked by M11's sub_080404e4, calls M07's
 * door code EnterDoor and plays the ability's door animation
 * (gPlayerDoorAnims[ability][0]). */

s32 PlaySfx(s32 id);
void TaskSetEntry(void *a, u32 i);
void RequestScreenShake(u16 a);
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionDie(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    u16 *anim;
    u16 n;
    u16 i;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 18;
    gCurTask->updateCallback = (u32)sub_080396a4;
    gCurTask->lateUpdateCallback = 0;
    gCurTask->facing = 1;
    t = gCurTask;
    t->spriteFlags &= 0x7FFF;
    gActivePlayerCount--;
    gActivePlayerMask &= ~(1 << t->player->playerIndex);
    if (t->hitEffect == 0x200)
        sub_08027548();
    u = gCurTask;
    u->player->unk40 |= 8;
    u->player->unk42 |= 0x100;
    u->variant = 0;
    gCurTask->player->unk42 &= 0xFFEF;
    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
    PlayerStopAxes(3);
    gCurTask->player->mouthState = 0;
    SetPlayerAbility(0, -1, gCurTask->player->playerIndex);
    sub_080276ac(gCurTask->player->playerIndex);
    gCurTask->player->unk16 = 255;
    anim = gUnk_0873D9FA[gCurTask->player->ability];
    n = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerHealth[i] != 0)
            n++;
    }
    if (n == 0)
    {
        StopAllSound();
        StopAllSfx();
        gCurTask->layer = 4;
        gCurTask->player->invincible = n;
        gCurTask->player->invincibleTimer = n;
        gCurTask->player->unk1A = gCurTask->player->unk1C = n;
    }
    else
    {
        PlayerEndInvincibility();
    }
    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        gCurTask->frame = anim[0];
        if (gPlayerCount == 1)
        {
            StopAllSfx();
            StopAllSound();
            FreezeOtherTasks(15);
            SetRoomUpdateFlags(2);
            if (!(gDispCnt & 0x400))
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        else if (gActivePlayerMask == 0)
        {
            for (i = 4; i <= 63; i++)
            {
                if (gTaskSlotTypes[i] != -1 && i != 63)
                    TaskSetSkipMask(15, i);
            }
            StopAllSfx();
            PauseRoom();
            SetRoomUpdateFlags(2);
            if (!(gDispCnt & 0x400))
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        else
        {
            sub_08040934(gCurTask->player->playerIndex);
        }
        if (gCurTask->player->unk37 != 2 || gActivePlayerMask == 0)
            RequestScreenShake(4);
    }
    else
    {
        gCurTask->frame = anim[0];
        if (gActivePlayerMask == 0)
        {
            for (i = 4; i <= 63; i++)
            {
                if (gTaskSlotTypes[i] != -1 && i != 63)
                    TaskSetSkipMask(15, i);
            }
            StopAllSfx();
            PauseRoom();
            SetRoomUpdateFlags(2);
            RequestScreenShake(4);
            if (!(gDispCnt & 0x400))
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        else
        {
            sub_08040934(gCurTask->player->playerIndex);
        }
    }
    TaskYieldTrampoline(1);
    if (gActivePlayerMask == 0)
        PlaySfx(158);
    else
        PlaySfxIfLocalPlayer(158, gCurTask->player->playerIndex);
    TaskYieldTrampoline(59);
    gCurTask->variant = 1;
    gCurTask->player->unk22 = 1;
    gCurTask->player->invulnerabilityTimer = 0x8000;
    for (i = 0; i <= 3; i++)
        CreatePlayerEffect(gCurTask->player->playerIndex, 12, i);
    if (n == 0)
        PlayBgm(3);
    else if (gRoomExitKind != 1)
        PlaySfx(270);
    PlayerSetMotionYPreset(32);
    gCurTask->playerBaseFrame = anim[1];
    do
    {
        struct Task *w = gCurTask;
        struct Task *x;

        if (w->drawCallback != 0)
            CreatePlayerEffectHighSlot(w->player->playerIndex, 5, 0);
        gCurTask->frame = gCurTask->playerBaseFrame;
        TaskYieldTrampoline(1);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
        {
            gCurTask->frame--;
            TaskYieldTrampoline(1);
        }
        x = gCurTask;
        if (x->drawCallback != 0)
            CreatePlayerEffectHighSlot(x->player->playerIndex, 5, 0);
        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 7; gCurTask->playerLoopCount++)
        {
            gCurTask->frame--;
            TaskYieldTrampoline(1);
        }
    } while (gCurTask->variant != 3);
    gLivingPlayerCount--;
    t2 = gCurTask;
    t2->moveCallback = 0;
    t2->drawCallback = 0;
    t2->lateUpdateCallback = 0;
    if (gLivingPlayerCount == 0)
    {
        gStageRequest = 6;
        TaskExitTrampoline();
    }
    sub_080276cc(gCurTask->player->playerIndex);
    if (gPlayerLives[gCurTask->player->playerIndex] == 0)
        gCurTask->variant = 4;
    else
        gCurTask->variant = 5;
    gPauseDisabled = 0;
    TaskSleepForever();
}

void sub_080396a4(void)
{
    switch (gCurTask->variant)
    {
    case 4:
    {
        struct Task *t = gCurTask;

        t->updateCallback = 0;
        t->taskClass = 4;
        if (gGameState != 20)
            TaskSetEntry(PlayerLifeRequestInit, gCurTaskIdx);
        else
            sub_080b9118();
        break;
    }
    case 1:
    {
        struct Task *t = gCurTask;

        if (t->velY > 0)
        {
            struct PlayerState *p;
            u16 y;

            t->speedLimitY = 0x40000;
            p = t->player;
            if (p->unk37 != 2)
                y = t->pixelY - (u16)(gPlayerCameraPos[p->playerIndex].y - 80);
            else
                y = t->pixelY;
            if ((s16)y > 184)
            {
                struct Task *u;
                struct PlayerState *q;

                gCurTask->player->unk14 = 90;
                gCurTask->variant = 2;
                PlayerStopAxes(2);
                u = gCurTask;
                q = u->player;
                if (q->unk37 != 2 && u->pixelY - gSpriteCameraY <= 183)
                    CreatePlayerEffect(q->playerIndex, 18, 0);
                gCurTask->drawCallback = 0;
            }
        }
        if (!(gCurTask->player->unk42 & 32))
            sub_0803e080();
        break;
    }
    case 2:
    {
        struct Task *t = gCurTask;

        if (--t->player->unk14 == 0)
            t->variant = 3;
        break;
    }
    case 0:
    case 3:
    case 5:
        break;
    }
}

void PlayerActionEnterDoor(void)
{
    struct Task *t;
    s32 n;
    s32 i;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateCallback = 0;
    gCurTask->lateUpdateCallback = 0;
    gCurTask->player->running = 0;
    PlayerStopAxes(3);
    gCurTask->player->unk42 |= 0x100;
    RequestScreenShake(0);
    if (gInHub == 0)
    {
        FreezeOtherTasks(15);
        if (!(gDispCnt & 0x400))
        {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1D00;
        }
        n = 0;
        if (gPlayerCount > 1 && gLivingPlayerCount > 1)
        {
            for (i = 0; i < gPlayerCount; i++)
            {
                if (gPlayerHealth[i] == 0)
                {
                    n++;
                    TaskSetSkipMask(0, i);
                }
            }
        }
        if (n != 0)
            StopOtherSfx(158);
        else
            StopAllSfx();
    }
    switch (sub_080404e4())
    {
    case 1:
        gCurTask->player->mouthState = 0;
        CreatePlayerObject(gCurTask->player->playerIndex, 0, 0);
        TaskSetFrame(gUnk_0873D7E4[gCurTask->player->ability][2]);
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        break;
    case 2:
        gCurTask->player->mouthState = 0;
        if (!(gCurTask->waterFlags & 1))
        {
            TaskSetFrame(79);
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            if ((s8)gCurTask->player->attachedCount > 1)
                CreatePlayerObject(gCurTask->player->playerIndex, 2, 0);
            else
                CreatePlayerObject(gCurTask->player->playerIndex, 1, 0);
            if (gCurTask->onGround & 1)
            {
                CreatePlayerEffect(gCurTask->player->playerIndex, 2, 0);
                CreatePlayerEffect(gCurTask->player->playerIndex, 2, 1);
            }
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
    default:
            TaskYieldTrampoline(1);
            break;
        }
        TaskSetFrame(224);
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        if ((s8)gCurTask->player->attachedCount > 1)
            CreatePlayerObject(gCurTask->player->playerIndex, 2, 0);
        else
            CreatePlayerObject(gCurTask->player->playerIndex, 1, 0);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        break;
    }
    EnterDoor();
    if (gInHub != 0)
        CreateEntryDoorOpening();
    if (gInHub == 0)
        PlaySfx(181);
    t = gCurTask;
    if (!(t->waterFlags & 1))
    {
        t->playerBaseFrame = gPlayerDoorAnims[t->player->ability][0];
        switch (t->player->ability)
        {
        case 0:
        default:
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            break;
        case 1:
        case 2:
        case 5:
        case 19:
        case 22:
        case 23:
            TaskSetFrame(gCurTask->playerBaseFrame);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            while (1)
            {
                TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                TaskYieldTrampoline(2);
                TaskSetFrame((s16)(gCurTask->playerBaseFrame + 15));
                TaskYieldTrampoline(2);
            }
        }
    }
    else
    {
        gCurTask->playerBaseFrame = sub_0803f7e0(0);
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
    }
    TaskSleepForever();
}
