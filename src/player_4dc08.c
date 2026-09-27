#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4dc08.c (0x0804DC08-0x0804E39F, issue #90).
 *
 * Player action bodies, part 24: actions 30 and 31 and per-frame handler
 * 27 (handler 28, the pair of action 31, is PR #133's src/sub_0804e3a0.c).
 * PlayerActionBackdrop (action 30, mode 10) is a three-state `switch (Task.variant)`
 * whose states fall into each other: state 0 winds up (animation 0xE79,
 * camera preset PlayerSetMotionXPreset(11, 5)), state 1 plays sound 200 and effect 6
 * and loops animations 0xE7C/0xE85 four times, and state 2 either swings
 * (animation 0xE7D, M11's PlayerSetMotionYPreset steering, effect 28 when it lands)
 * or, once PlayerState.attachedCount is set, plays sound 177, sets
 * PlayerState.unk42 bit 9 and stops; every pass counts Task.unk28.  Its
 * handler PlayerActionBackdropUpdate re-binds state 2 when PlayerState.attachedCount is set, runs
 * the hit test sub_08030898(gUnk_0873CC64) in state 1 (which spawns
 * sub_08065100's object and marks PlayerState.unk09) and, in state 2,
 * requests action 53, 8 or 1 once the swing is over.  PlayerActionThrow
 * (action 31, mode 10; the twin of M10's PlayerActionInhale) clears the three
 * records gUnk_02007E90[player][] (and gUnk_02007CF4[player] in link
 * play), plays sound 103 and holds animation 0xF71 with PlayerState.unk40
 * bit 2 set until PlayerState.attachedCount is non-zero and equal to unk08, then
 * recovers or releases (sound 201, PlayerState.unk42 bit 9). */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (M12's PlayerActionBurningUpdate). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 onSlipperyFloor;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

/* gUnk_02007E90[4][3]: M04's per-player spark records (src/player_10358.c) */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
};

extern u32 gUnk_0873CC64[];             /* hit-box set, passed as (struct HitBoxSet *) */
extern s16 gUnk_02007FA0[];
extern s16 gUnk_02004B6C[];
extern struct Unk03005550 gTerrainResult;
extern u32 gUnk_0873BED8[];             /* collider row passed to RegisterCollider (4th arg) */
extern s16 gUnk_0300244C;
extern u8 gUnk_02007CF4[];
extern s32 gUnk_03001F2C;               /* boot_091ac.c spelling */
extern struct M04Spark gUnk_02007E90[][3];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
u16 sub_08030898(struct HitBoxSet *p, s32 e);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerStartSfx(s32 a0, u16 a1);
void PlayerStopSfx(void);
void PlayerSetWaterMotionY(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 sub_08065100(s32 x, s32 y, u32 p2, u8 p3, u8 p4);   /* this caller passes x and y unnarrowed (ldrsh; adds #8) */

void PlayerActionBackdrop(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 10;
    gCurTask->updateState = 27;
    if (gCurTask->player->prevMode != 10)
    {
        gCurTask->unk28 = 0;
        gCurTask->variant = 0;
        gCurTask->player->unk16 = 0;
        {
            struct PlayerState *p = gCurTask->player;

            p->unk09 = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
    }
    switch (gCurTask->variant)
    {
    case 0:
        PlayerStopAxes(2);
        PlayerSetMotionXPreset(11, 5);
        TaskSetFrame(0xE79);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        PlayerStopAxes(1);
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        PlaySfxIfLocalPlayer(200, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, 6, 260);
        PlayerSetMotionXPreset(11, 6);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
        {
            TaskSetFrame(0xE7C);
            TaskYieldTrampoline(2);
            TaskSetFrame(0xE85);
            TaskYieldTrampoline(2);
        }
        gCurTask->variant = 2;
    case 2:
        if ((s8)gCurTask->player->attachedCount == 0)
        {
            if (gCurTask->onGround & 1)
                CreatePlayerEffect(gCurTask->player->playerIndex, 4, 0);
            else if (!(gCurTask->waterFlags & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
            PlayerSetMotionXPreset(11, 7);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            TaskSetFrame(0xE7D);
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            gCurTask->frame++;
            TaskYieldTrampoline(5);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            TaskSetFrame(0xE7D);
            TaskYieldTrampoline(5);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            if (gCurTask->onGround & 1)
                PlayerSetMotionYPreset(19);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            if (gCurTask->onGround & 1)
            {
                PlayerStopAxes(2);
                CreatePlayerEffect(gCurTask->player->playerIndex, 28, 5);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                PlayerSetMotionXPreset(11, 8);
                gCurTask->frame++;
                TaskYieldTrampoline(8);
            }
        }
        else
        {
            PlaySfxIfLocalPlayer(177, gCurTask->player->playerIndex);
            SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
            gCurTask->player->unk42 |= 0x200;
            PlayerStopAxes(1);
            TaskSetFrame(0xE83);
            TaskYieldTrampoline(1);
        }
        gCurTask->unk28++;
    }
    TaskSleepForever();
}

void PlayerActionBackdropUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;

    t = gCurTask;
    switch (t->variant)
    {
    case 0:
        break;
    case 1:
        if ((s8)t->player->attachedCount != 0)
        {
            t->variant = 2;
            TaskSetEntry(PlayerActionBackdrop, gCurTaskIdx);
        }
        else if (t->player->unk09 == 0)
        {
            if (sub_08030898((struct HitBoxSet *)gUnk_0873CC64, t->player->playerIndex) != 0)
            {
                sub_08065100(gUnk_02007FA0[0] + 8, gUnk_02004B6C[0] + 8, gCurTaskIdx, 4, 3);
                gCurTask->player->unk09 = 2;
            }
            u = gCurTask;
            if (u->player->unk09 == 0)
            {
                if (gTerrainResult.unk0 != 0)
                {
                    RequestScreenShake(1);
                    gCurTask->player->requestedAction = 18;
                }
                else
                {
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BED8);
                }
            }
        }
        break;
    case 2:
        if (t->unk28 != 0)
        {
            p = t->player;
            if ((s8)p->heldCount != 0)
                p->requestedAction = 53;
            else if (!(t->onGround & 1))
                p->requestedAction = 8;
            else
                p->requestedAction = 1;
        }
        if (gTerrainResult.unk0 != 0)
            PlayerStopAxes(1);
        u = gCurTask;
        if (u->velY != 0)
        {
            if (PlayerHasCrossedWaterSurface(0) != 0)
            {
                PlayerSetWaterMotionY();
                if ((s8)gCurTask->player->attachedCount == 0)
                    gCurTask->player->requestedAction = 23;
            }
            else if (PlayerCheckLanding() != 0)
            {
                struct Task *v = gCurTask;

                if (!(v->waterFlags & 1) && (v->velY & 0xFFFF0000))
                    CreatePlayerEffect(v->player->playerIndex, 4, 0);
                PlayerStopAxes(2);
            }
        }
        else if (!(u->onGround & 1))
        {
            if (!(u->waterFlags & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        }
        break;
    }
}

void PlayerActionThrow(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 10;
    gCurTask->updateState = 28;
    t = gCurTask;
    if (t->player->prevMode != 10)
    {
        t->unk30 = 0;
        t->variant = 0;
    }
    u = gCurTask;
    switch (u->variant)
    {
    case 0:
        u->unk28 = 1;
        u->unk2C = 30;
        u->player->unk16 = 0;
        {
            struct PlayerState *p = gCurTask->player;

            p->unk09 = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
        if (gUnk_0300244C != 0)
            gUnk_02007CF4[gCurTask->player->playerIndex] = 0;
        gUnk_03001F2C = 0;
        do
        {
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk00 = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk04 = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk08 = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk0C = 1;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk0D = 0;
            gUnk_03001F2C++;
        } while (gUnk_03001F2C <= 2);
        gCurTask->variant = 1;
        TaskSetFrame(0xF6E);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStartSfx(103, gCurTask->player->playerIndex);
        gCurTask->player->unk40 |= 4;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        while (1)
        {
            TaskSetFrame(0xF71);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
            gCurTask->frame++;
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
        }
    hit:
        gCurTask->variant = 2;
    case 2:
        gCurTask->player->unk40 &= 0xFFFB;
        PlayerStopSfx();
        if ((s8)gCurTask->player->heldCount == 0)
        {
            TaskSetFrame(0xF6E);
            TaskYieldTrampoline(2);
        }
        else
        {
            PlaySfxIfLocalPlayer(201, gCurTask->player->playerIndex);
            SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
            gCurTask->player->unk42 |= 0x200;
            TaskSetFrame(0xF73);
            TaskYieldTrampoline(1);
        }
        gCurTask->variant = 3;
    }
    TaskSleepForever();
}
