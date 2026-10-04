#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* Not from room.h or player.h: this file's view of gBrokenBlockX and
   gUnk_0873CC54 differs (lesson 3.517). */
extern s16 gBrokenBlockY[];
extern s16 gBrokenBlockX[];
extern u16 gLatchedHeldKeys[];
extern u8 gUnk_0873BEEC[];
extern u8 gUnk_0873CC54[];

void TaskSetEntry(void *func, u32 arg);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);
u16 TaskBreakFirstBlock(void *table, s32 id);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a0);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void PlayerSetMotionYPreset(s32 a0);
void PlayerActionThrow(void);
s32 CreateBlockStar(s32 x, s32 y, u32 p2, u8 p3, u8 p4);

void PlayerActionThrowUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct PlayerState *p;
    struct PlayerState *r;

    while (1)
    {
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            PlayerSetWaterMotionY();
            if ((s8)gCurTask->player->attachedCount != 0)
                gCurTask->player->unk16 = 0xFF;
            gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
            break;
        }
        t = gCurTask;
        switch (t->variant)
        {
        case 1:
            p = t->player;
            if (p->catchKind == 0)
            {
                if (t->unk28 == 0)
                {
                    if (TaskBreakFirstBlock(gUnk_0873CC54, p->playerIndex) != 0)
                    {
                        CreateBlockStar(gBrokenBlockX[0] + 8, gBrokenBlockY[0] + 8, gCurTaskIdx, HIT_KIND_GRAB, HIT_EFFECT_THROW);
                        gCurTask->player->catchKind = 2;
                    }
                }
                else
                {
                    t->unk28--;
                }
                u = gCurTask;
                if (u->player->catchKind == 0)
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BEEC);
            }
            v = gCurTask;
            if ((s8)v->player->attachedCount == 0)
            {
                if (v->unk2C == 0)
                {
                    if (!(gLatchedHeldKeys[v->player->playerIndex] & 2))
                    {
                        v->variant = 2;
                        TaskSetEntry(PlayerActionThrow, gCurTaskIdx);
                    }
                }
                else
                {
                    v->unk2C--;
                }
            }
            else if (v->unk30 == 0)
            {
                PlayerStartOffsetScript(1);
                gCurTask->unk30++;
            }
            break;
        case 0:
        case 2:
            break;
        case 3:
            r = t->player;
            if ((s8)r->attachedCount != 0)
                r->requestedAction = PLAYER_ACTION_THROW_HOLD;
            else if (t->onGround & 1)
                r->requestedAction = PLAYER_ACTION_STAND;
            else
                r->requestedAction = PLAYER_ACTION_FALL;
            break;
        }
        break;
    }
    w = gCurTask;
    if (w->onGround & 1)
    {
        if (w->velX != 0)
        {
            if (w->waterFlags & 1)
                PlayerSetMotionXPreset(8, 72);
            else
                PlayerSetMotionXPreset(0, 72);
        }
        PlayerLand(1);
    }
    else if (!(w->waterFlags & 1))
    {
        PlayerSetMotionYPreset(2);
        PlayerSetMotionXPreset(11, 2);
    }
    else
    {
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
    }
    PlayerStopAtCeilingAndWall();
}
