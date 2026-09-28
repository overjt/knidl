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
 * sub_0803bde8 (action 19, mode 23; its per-frame handler 18 is
 * sub_0803c990) hands something over to the partner player in
 * Task.unk18: the two face each other, play the rows of gUnk_0873DA62 by
 * ability, and PlayerState.unk3A says what passes - 1 refills the
 * partner's health gPlayerHealth[] up to gMaxHealth step by step
 * through sub_080b4204 (src/hud_b2fe8.c), 2 gives one or two steps, 3
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
   gUnk_0873DACA[k][0]) and, by PlayerState.unk3A, raises the target's
   health gPlayerHealth[] through sub_080b4204 while it is below the maximum
   gMaxHealth (1: until full, 2: at most 1 or 2 steps by gExtraMode)
   or copies its own unk17/unk18 to the target (3); then it restores both
   tasks' Task.layer/unk43 (saved on the stack), clears PlayerState.unk42
   bit 8 on both players and sets the target's bit in PlayerState.unk3B. */
void sub_0803bde8(void)
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
    gCurTask->updateState = 18;
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
            t->unk46 = gUnk_0873DA62[t->player->ability][1];
            switch (t->player->ability)
            {
            case 0:
                if (gCurTask->player->mouthState == 1)
                    gCurTask->unk46 = 324;
            default:
                TaskSetFrame(gCurTask->unk46);
            case 24:
                TaskSleepForever();
            case 1:
            case 2:
            case 5:
            case 15:
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
            case 4:
                TaskSetFrame(gUnk_0873DACA[1][1]);
                break;
            case 9:
                TaskSetFrame(gUnk_0873DACA[2][1]);
                break;
            case 10:
                TaskSetFrame(gUnk_0873DACA[3][1]);
                break;
            case 24:
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
        t->unk46 = gUnk_0873DA62[t->player->ability][0];
        switch (t->player->ability)
        {
        default:
        case 0:
            if (gCurTask->player->mouthState == 1)
            {
                TaskSetFrame(325);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
            }
            switch (gCurTask->player->unk3A)
            {
            case 1:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    do
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
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
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < n; gCurTask->unk6C++)
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        TaskYieldTrampoline(8);
                        if (gCurTask->unk28 != 0)
                            break;
                    }
                }
                TaskYieldTrampoline(16);
                break;
            case 3:
                if ((s8)q->unk22 != 0)
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
            switch (gCurTask->player->unk3A)
            {
            case 1:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    do
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
                        {
                            TaskSetFrame((s16)(gCurTask->unk46 + 3));
                            TaskYieldTrampoline(2);
                            TaskSetFrame((s16)(gCurTask->unk46 + 15));
                            TaskYieldTrampoline(2);
                        }
                    } while (gCurTask->unk28 == 0);
                }
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
                {
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 15));
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
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < n; gCurTask->unk6C++)
                    {
                        if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++)
                        {
                            TaskSetFrame((s16)(gCurTask->unk46 + 3));
                            TaskYieldTrampoline(2);
                            TaskSetFrame((s16)(gCurTask->unk46 + 15));
                            TaskYieldTrampoline(2);
                        }
                        if (gCurTask->unk28 != 0)
                            break;
                    }
                }
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 3; gCurTask->unk6E++)
                {
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 3:
                if ((s8)q->unk22 != 0)
                    q->unk1E = q->unk20 = 0;
                q->invincible = gCurTask->player->invincible;
                q->invincibleTimer = gCurTask->player->invincibleTimer;
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++)
                {
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            }
            TaskSetFrame((s16)(gCurTask->unk46 + 3));
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
            gCurTask->unk46 = gUnk_0873DACA[0][0];
            break;
        case 4:
            t->unk46 = gUnk_0873DACA[1][0];
            break;
        case 9:
            t->unk46 = gUnk_0873DACA[2][0];
            break;
        case 10:
            t->unk46 = gUnk_0873DACA[3][0];
            break;
        case 24:
            t->unk46 = gUnk_0873DACA[4][0];
            break;
        }
        TaskSetFrame(gCurTask->unk46);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        switch (gCurTask->player->unk3A)
        {
        case 1:
            if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
            {
                do
                {
                    if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                        PlaySfx(221);
                    gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
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
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < n; gCurTask->unk6C++)
                {
                    if (gLocalPlayer == gCurTask->player->playerIndex || gLocalPlayer == gCurTask->unk18)
                        PlaySfx(221);
                    gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                    TaskYieldTrampoline(8);
                    if (gCurTask->unk28 != 0)
                        break;
                }
            }
            TaskYieldTrampoline(16);
            break;
        case 3:
            if ((s8)q->unk22 != 0)
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
    gCurTask->player->unk3B |= 1 << gCurTask->unk18;
    TaskSleepForever();
}

void sub_0803c990(void)
{
    struct PlayerState *p = gCurTask->player;

    if (p->ability != 24)
        sub_08040710();
    else
        p->requestedAction = 55;
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
            d = p->unk0D;
            if (a == 0 && d == 0)
                d = (gCurTask->facing == 1) ? 2 : 3;
        }
        else
        {
            if (p->unk00 == 0)
            {
                p->unk08 = 0;
                n = RandomRange(16);
                if (gCurTask->facing == 1)
                    p->unk00 = gUnk_0873A924[0][n] << 16;
                else
                    p->unk00 = -(gUnk_0873A924[0][n] << 16);
                p->unk04 = gUnk_0873A924[1][n] << 16;
            }
            if (abs(p->unk00) <= 0xF0000)
            {
                p->unk00 = 0;
                p->unk0C = 1;
                p->unk0D = 0;
                continue;
            }
            if (p->unk00 > 0)
                p->unk08 -= 0x6000;
            else
                p->unk08 += 0x6000;
            /* The two volatile reads are the twin's placeholders: the ROM
               re-reads unk00 here and unk04 below, and a plain read is
               folded into the earlier one by gcse's PRE. */
            p->unk00 = *(volatile s32 *)&p->unk00 + p->unk08;
            /* the shift count reuses n: its own local takes r1 (sh/u swap) */
            n = (abs(p->unk00) >> 20) + 1;
            u = *(volatile s32 *)&p->unk04;
            t = (abs(u) & 0xFFFF0000) >> n;
            if (p->unk04 > 0)
                t = -t;
            p->unk04 += t;
            d = a;
            k = 0;
            if (a == 0)
            {
                if (--p->unk0C == 0)
                {
                    if (p->unk0D == 0)
                    {
                        if (gCurTask->facing == 1)
                            p->unk0D = 2;
                        else
                            p->unk0D = 3;
                    }
                    else if (p->unk0D <= 9)
                        p->unk0D += 2;
                    p->unk0C = 1;
                }
                d = p->unk0D;
                k = gCurTask->tileWord + 0x180C;
            }
        }
        x = gCurTask->pixelX + ((s16 *)&p->unk00)[1];
        y = gCurTask->pixelY + ((s16 *)&p->unk04)[1] + 4;
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
    if (e->unk4 != 0)
    {
        if (e->unk4 & 128)
            TaskSetSkipMask(3, gCurTaskIdx);
        if (e->unk4 & 64)
        {
            if (gCurTask->facing == 1)
                gCurTask->player->pixelOffsetX = e->unk0;
            else
                gCurTask->player->pixelOffsetX = -e->unk0;
        }
        else
        {
            gCurTask->player->pixelOffsetX = e->unk0;
        }
        gCurTask->player->pixelOffsetY = e->unk2;
        gCurTask->player->offsetScriptDelay = e->unk4 & 15;
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
