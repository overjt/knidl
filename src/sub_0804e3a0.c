#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s16 gUnk_02004B6C[];
extern s16 gUnk_02007FA0[];
extern u16 gLatchedHeldKeys[];
extern u8 gUnk_0873BEEC[];
extern u8 gUnk_0873CC54[];

void TaskSetEntry(void *func, u32 arg);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);
u16 sub_08030898(void *table, s32 id);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a0);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void PlayerSetMotionYPreset(s32 a0);
void sub_0804e0e0(void);
s32 sub_08065100(s32 x, s32 y, u32 p2, u8 p3, u8 p4);

void sub_0804e3a0(void)
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
            if ((s8)gCurTask->player->unk07 != 0)
                gCurTask->player->unk16 = 0xFF;
            gCurTask->player->requestedAction = 23;
            break;
        }
        t = gCurTask;
        switch (t->unk73)
        {
        case 1:
            p = t->player;
            if (p->unk09 == 0)
            {
                if (t->unk28 == 0)
                {
                    if (sub_08030898(gUnk_0873CC54, p->playerIndex) != 0)
                    {
                        sub_08065100(gUnk_02007FA0[0] + 8, gUnk_02004B6C[0] + 8, gCurTaskIdx, 4, 2);
                        gCurTask->player->unk09 = 2;
                    }
                }
                else
                {
                    t->unk28--;
                }
                u = gCurTask;
                if (u->player->unk09 == 0)
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BEEC);
            }
            v = gCurTask;
            if ((s8)v->player->unk07 == 0)
            {
                if (v->unk2C == 0)
                {
                    if (!(gLatchedHeldKeys[v->player->playerIndex] & 2))
                    {
                        v->unk73 = 2;
                        TaskSetEntry(sub_0804e0e0, gCurTaskIdx);
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
            if ((s8)r->unk07 != 0)
                r->requestedAction = 54;
            else if (t->onGround & 1)
                r->requestedAction = 1;
            else
                r->requestedAction = 7;
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
