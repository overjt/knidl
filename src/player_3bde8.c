#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "actor.h"

/* player_3bde8.c (0x0803BDE8-0x0803CD5F, issue #91).
 *
 * Player action body, part 10: action 19 and three helpers M09 calls.
 * PlayerActionShareItem (action 19, mode 23; its per-frame handler 18 is
 * PlayerActionShareItemUpdate) hands something over to the partner player in
 * Task.unk18: the two face each other, play the rows of gUnk_0873DA62 by
 * ability, and PlayerState.shareItem says what passes - 1 refills the
 * partner's health gPlayerHealth[] up to gMaxHealth step by step
 * through HealPlayerStep (src/hud_b2fe8.c), 2 gives one or two steps, 3
 * copies PlayerState.invincible/unk18.  sub_0803c9b4 (from M09's
 * sub_08033414, the twin of M04's sub_080109c8) steps and draws the
 * three spark records gUnk_02007E90[player][]; sub_0803cbd8 (M09's
 * sub_0803332c) steps the knock-back script
 * gUnk_0873A994[PlayerState.offsetScript][PlayerState.offsetScriptStep] into the 8.8
 * offsets PlayerState.pixelOffsetX/unk26; sub_0803ccd8 (M09's PlayerActionFall)
 * applies step n of the 8.8 motion table gUnk_0873AEBC. */

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 RandomRange(u32 range);
s32 PlaySfx(s32 id);
u32 IsWorldPosOnScreen(s16 a, s16 b);        /* u8 in early_5d9c.c; u32 as in player_109c8.c (u8 costs 78 bytes) */

/* Action 19 enter (mode 23, per-frame handler 18), aimed at the player
   Task.unk18 names.  With itself as the target it only plays the ability's
   animation (gUnk_0873DA62[ability][1], or gUnk_0873DACA[k][1] when
   Task.waterFlags bit 0 is set) and ends; otherwise it turns to face the target
   task gTasks[unk18], plays gUnk_0873DA62[ability][0] (or
   gUnk_0873DACA[k][0]) and, by PlayerState.shareItem, raises the target's
   health gPlayerHealth[] through HealPlayerStep while it is below the maximum
   gMaxHealth (1: until full, 2: at most 1 or 2 steps by gExtraMode)
   or copies its own unk17/unk18 to the target (3); then it restores both
   tasks' Task.layer/unk43 (saved on the stack), clears PlayerState.unk42
   bit 8 on both players and sets the target's bit in PlayerState.sharedMask. */
void PlayerActionShareItem(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *q;
    s16 n;
    u16 a43;
    u16 b43;
    u8 a42;
    u8 b42;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 23;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SHARE_ITEM;
    if (gCurTask->player->mouthState == 2)
        gCurTask->player->mouthState = 0;
    PlaySfxIfLocalPlayer(292, gCurTask->player->playerIndex);
    if (gCurTask->unk18 == gCurTask->player->playerIndex)
    {
        gCurTask->layer = 5;
        TaskSetSkipMask(14, gCurTaskIdx);
        t = gCurTask;
        t->player->unk42 |= 0x100;
        t->player->unk42 &= 0xFFEF;
        if (!(t->waterFlags & 1))
        {
            t->playerBaseFrame = gUnk_0873DA62[t->player->ability][1];
            switch (t->player->ability)
            {
            case ABILITY_NORMAL:
                if (gCurTask->player->mouthState == 1)
                    gCurTask->playerBaseFrame = 324;
            default:
                TaskSetFrame(gCurTask->playerBaseFrame);
            case ABILITY_UFO:
                TaskSleepForever();
            case ABILITY_FIRE:
            case ABILITY_SPARK:
            case ABILITY_BURNING:
            case ABILITY_HI_JUMP:
            case ABILITY_TORNADO:
                while (1)
                {
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
        }
        else
        {
            switch (gCurTask->player->ability)
            {
            default:
                if (gCurTask->player->mouthState == 1)
                    TaskSetFrame(326);
                else
                    TaskSetFrame(gUnk_0873DACA[0][1]);
                break;
            case ABILITY_SWORD:
                TaskSetFrame(gUnk_0873DACA[1][1]);
                break;
            case ABILITY_HAMMER:
                TaskSetFrame(gUnk_0873DACA[2][1]);
                break;
            case ABILITY_PARASOL:
                TaskSetFrame(gUnk_0873DACA[3][1]);
                break;
            case ABILITY_UFO:
                break;
            }
            TaskSleepForever();
        }
    }
    q = &gPlayerStates[gCurTask->unk18];
    u = &gTasks[gCurTask->unk18];
    FreezeOtherTasks(15);
    TaskSetSkipMask(12, gCurTaskIdx);
    TaskSetSkipMask(0, gCurTask->unk18);
    a43 = gCurTask->facing;
    a42 = gCurTask->layer;
    PlayerStopAxes(3);
    t = gCurTask;
    t->player->unk42 |= 0x100;
    t->player->unk42 &= 0xFFEF;
    if (u->pixelX - t->pixelX > 0)
        t->facing = 1;
    else
        t->facing = -1;
    gCurTask->layer = 4;
    b43 = u->facing;
    b42 = u->layer;
    u->velX = u->accelX = u->speedLimitX = 0;
    u->velY = u->accelY = u->speedLimitY = 0;
    if ((u->facing = -gCurTask->facing) == 1)
        u->spriteFlags &= 0x7FFF;
    else
        u->spriteFlags |= 0x8000;
    t = gCurTask;
    if (!(t->waterFlags & 1))
    {
        t->playerBaseFrame = gUnk_0873DA62[t->player->ability][0];
        switch (t->player->ability)
        {
        default:
        case ABILITY_NORMAL:
            if (gCurTask->player->mouthState == 1)
            {
                TaskSetFrame(325);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(gCurTask->playerBaseFrame);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
            }
            switch (gCurTask->player->shareItem)
            {
            case 1:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    do
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = HealPlayerStep(gCurTask->unk18);
                        TaskYieldTrampoline(8);
                    } while (gCurTask->unk28 == 0);
                }
                TaskYieldTrampoline(16);
                break;
            case 2:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    if (gExtraMode == 0)
                        n = 2;
                    else
                        n = 1;
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount < n; gCurTask->playerLoopCount++)
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = HealPlayerStep(gCurTask->unk18);
                        TaskYieldTrampoline(8);
                        if (gCurTask->unk28 != 0)
                            break;
                    }
                }
                TaskYieldTrampoline(16);
                break;
            case 3:
                if ((s8)q->paletteFlashMode != 0)
                    q->unk1E = q->unk20 = 0;
                q->invincible = gCurTask->player->invincible;
                q->invincibleTimer = gCurTask->player->invincibleTimer;
                TaskYieldTrampoline(32);
                break;
            }
            if (gCurTask->player->mouthState == 1)
            {
                TaskSetFrame(319);
                TaskYieldTrampoline(2);
            }
            else
            {
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
            }
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
            switch (gCurTask->player->shareItem)
            {
            case 1:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    do
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = HealPlayerStep(gCurTask->unk18);
                        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++)
                        {
                            TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                            TaskYieldTrampoline(2);
                            TaskSetFrame((s16)(gCurTask->playerBaseFrame + 15));
                            TaskYieldTrampoline(2);
                        }
                    } while (gCurTask->unk28 == 0);
                }
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 3; gCurTask->playerLoopCount++)
                {
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 2:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    if (gExtraMode == 0)
                        n = 2;
                    else
                        n = 1;
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount < n; gCurTask->playerLoopCount++)
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = HealPlayerStep(gCurTask->unk18);
                        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++)
                        {
                            TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                            TaskYieldTrampoline(2);
                            TaskSetFrame((s16)(gCurTask->playerBaseFrame + 15));
                            TaskYieldTrampoline(2);
                        }
                        if (gCurTask->unk28 != 0)
                            break;
                    }
                }
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 3; gCurTask->unk6E++)
                {
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 3:
                if ((s8)q->paletteFlashMode != 0)
                    q->unk1E = q->unk20 = 0;
                q->invincible = gCurTask->player->invincible;
                q->invincibleTimer = gCurTask->player->invincibleTimer;
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 7; gCurTask->playerLoopCount++)
                {
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->playerBaseFrame + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            }
            TaskSetFrame((s16)(gCurTask->playerBaseFrame + 3));
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            break;
        }
    }
    else
    {
        switch (t->player->ability)
        {
        default:
            gCurTask->playerBaseFrame = gUnk_0873DACA[0][0];
            break;
        case ABILITY_SWORD:
            t->playerBaseFrame = gUnk_0873DACA[1][0];
            break;
        case ABILITY_HAMMER:
            t->playerBaseFrame = gUnk_0873DACA[2][0];
            break;
        case ABILITY_PARASOL:
            t->playerBaseFrame = gUnk_0873DACA[3][0];
            break;
        case ABILITY_UFO:
            t->playerBaseFrame = gUnk_0873DACA[4][0];
            break;
        }
        TaskSetFrame(gCurTask->playerBaseFrame);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        switch (gCurTask->player->shareItem)
        {
        case 1:
            if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
            {
                do
                {
                    if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                        PlaySfx(221);
                    gCurTask->unk28 = HealPlayerStep(gCurTask->unk18);
                    TaskYieldTrampoline(8);
                } while (gCurTask->unk28 == 0);
            }
            TaskYieldTrampoline(16);
            break;
        case 2:
            if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
            {
                if (gExtraMode == 0)
                    n = 2;
                else
                    n = 1;
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount < n; gCurTask->playerLoopCount++)
                {
                    if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                        PlaySfx(221);
                    gCurTask->unk28 = HealPlayerStep(gCurTask->unk18);
                    TaskYieldTrampoline(8);
                    if (gCurTask->unk28 != 0)
                        break;
                }
            }
            TaskYieldTrampoline(16);
            break;
        case 3:
            if ((s8)q->paletteFlashMode != 0)
                q->unk1E = q->unk20 = 0;
            q->invincible = gCurTask->player->invincible;
            q->invincibleTimer = gCurTask->player->invincibleTimer;
            TaskYieldTrampoline(32);
            break;
        }
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
    gCurTask->facing = a43;
    gCurTask->layer = a42;
    gCurTask->player->unk42 &= 0xFEFF;
    u->facing = b43;
    u->layer = b42;
    gPlayerStates[gCurTask->unk18].unk42 &= 0xFEFF;
    u->taskClass--;
    FreezeOtherTasks(0);
    TaskSetSkipMask(0, gCurTaskIdx);
    gCurTask->player->sharedMask |= 1 << gCurTask->unk18;
    TaskSleepForever();
}

void PlayerActionShareItemUpdate(void)
{
    struct PlayerState *p = gCurTask->player;

    if (p->ability != ABILITY_UFO)
        PlayerRequestStandOrFall();
    else
        p->requestedAction = PLAYER_ACTION_UFO;
}

/* M09's sub_08033414: step and draw the player's three sparks (twin of M04's
   sub_080109c8, src/player_109c8.c).  a != 0 freezes the frame counter and
   draws frame `a` with no palette offset; Task.skipMask & 1 skips the motion. */
void sub_0803c9b4(s32 a)
{
    struct M04Spark *p;
    s32 i;
    s32 n;
    s32 t;
    s32 u;
    s32 d;
    s32 k;
    s32 x;
    s32 y;

    for (i = 0; i <= 2; i++)
    {
        p = &gUnk_02007E90[gCurTask->player->playerIndex][i];
        if (gCurTask->skipMask & 1)
        {
            k = 0;
            if (a == 0)
                k = gCurTask->tileWord + 0x180C;
            d = p->frame;
            if (a == 0 && d == 0)
                d = (gCurTask->facing == 1) ? 2 : 3;
        }
        else
        {
            if (p->offsetX == 0)
            {
                p->velX = 0;
                n = RandomRange(16);
                if (gCurTask->facing == 1)
                    p->offsetX = gUnk_0873A924[0][n] << 16;
                else
                    p->offsetX = -(gUnk_0873A924[0][n] << 16);
                p->offsetY = gUnk_0873A924[1][n] << 16;
            }
            if (abs(p->offsetX) <= 0xF0000)
            {
                p->offsetX = 0;
                p->frameTimer = 1;
                p->frame = 0;
                continue;
            }
            if (p->offsetX > 0)
                p->velX -= 0x6000;
            else
                p->velX += 0x6000;
            /* The two volatile reads are the twin's placeholders: the ROM
               re-reads unk00 here and unk04 below, and a plain read is
               folded into the earlier one by gcse's PRE. */
            p->offsetX = *(volatile s32 *)&p->offsetX + p->velX;
            /* the shift count reuses n: its own local takes r1 (sh/u swap) */
            n = (abs(p->offsetX) >> 20) + 1;
            u = *(volatile s32 *)&p->offsetY;
            t = (abs(u) & 0xFFFF0000) >> n;
            if (p->offsetY > 0)
                t = -t;
            p->offsetY += t;
            d = a;
            k = 0;
            if (a == 0)
            {
                if (--p->frameTimer == 0)
                {
                    if (p->frame == 0)
                    {
                        if (gCurTask->facing == 1)
                            p->frame = 2;
                        else
                            p->frame = 3;
                    }
                    else if (p->frame <= 9)
                        p->frame += 2;
                    p->frameTimer = 1;
                }
                d = p->frame;
                k = gCurTask->tileWord + 0x180C;
            }
        }
        x = gCurTask->pixelX + ((s16 *)&p->offsetX)[1];
        y = gCurTask->pixelY + ((s16 *)&p->offsetY)[1] + 4;
        if (IsWorldPosOnScreen(x, y))
        {
            x -= gSpriteCameraX;
            y -= gSpriteCameraY;
            QueueSprite(gCurTask->layer, gUnk_0873A964[d], 0, k, x, y);
        }
    }
}

void sub_0803cbd8(void)
{
    struct Unk0873A994 *e;
    if ((s8)--gCurTask->player->offsetScriptDelay > 0)
    {
        gCurTask->player->offsetScriptDelay--;
        return;
    }
    e = gUnk_0873A994[(s8)gCurTask->player->offsetScript];
    e += (s8)gCurTask->player->offsetScriptStep;
    if (e->flags != 0)
    {
        if (e->flags & 128)
            TaskSetSkipMask(3, gCurTaskIdx);
        if (e->flags & 64)
        {
            if (gCurTask->facing == 1)
                gCurTask->player->pixelOffsetX = e->offsetX;
            else
                gCurTask->player->pixelOffsetX = -e->offsetX;
        }
        else
        {
            gCurTask->player->pixelOffsetX = e->offsetX;
        }
        gCurTask->player->pixelOffsetY = e->offsetY;
        gCurTask->player->offsetScriptDelay = e->flags & 15;
        gCurTask->player->offsetScriptStep++;
    }
    else
    {
        gCurTask->player->pixelOffsetX = gCurTask->player->pixelOffsetY = 0;
        gCurTask->player->unk40 &= 0xFFFE;
        TaskSetSkipMask(0, gCurTaskIdx);
    }
}

void sub_0803ccd8(s32 a)
{
    u16 *e = gUnk_0873AEBC[a];
    s32 x = e[1] << 8;
    s32 y;
    struct Task *t;

    if (e[1] & 0x8000)
        x |= 0xFF000000;
    y = e[1] << 8;
    if (e[1] & 0x8000)
        y |= 0xFF000000;
    TaskSetMotionY(x, 0, y);
    t = gCurTask;
    if (t->facing == 1)
    {
        s32 v = e[0] << 8;
        if (e[0] & 0x8000)
            v |= 0xFF000000;
        t->unk2C = v;
    }
    else
    {
        t->unk2C = -(e[0] & 0x8000 ? (e[0] << 8) | 0xFF000000 : e[0] << 8);
    }
}
