#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_36280.c (0x08036280-0x08036C93, issue #91).
 *
 * Player action bodies, part 4: per-frame handler 9 and actions 10 and
 * 11.  PlayerActionFloatUpdate (handler 9) is the other half of M09's mode-14
 * coroutine PlayerActionFloat: a seven-state switch over Task.unk73 that
 * re-binds the coroutine with the next state (1 on a held A or up, 4 on
 * a newly-pressed B, 2/3/5 from the ground flags Task.unk7A/unk7B).
 * PlayerActionDuck (action 10, mode 6) installs the scripts
 * gUnk_0873BD28/gUnk_0873CB24 in PlayerState.unk64/unk68 and plays
 * gUnk_0873D4BC[ability][column]; its handler PlayerActionDuckUpdate requests
 * action 11 on a newly-pressed A or B and 7 on the collision flag
 * gTerrainResult.unk5.  PlayerActionSlide (action 11, mode 7) installs the
 * attack hit-box set gUnk_0873CC84 in PlayerState.unk6C; its handler
 * PlayerActionSlideUpdate registers the box gUnk_0873BE9C with M09's collision
 * registry RegisterCollider while the player moves faster than 0xE000 and
 * drops to state 1 below 0x8000. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gTerrainResult;
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873CB24[];
extern u16 gUnk_0873D4BC[][5];
extern u8 gUnk_03001F30;
extern u32 gUnk_0873CC84[];
extern s16 gUnk_0873D5CA[][2];
extern u8 gUnk_0873BE9C[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);
void PlayerActionFloat(void);
void PlayerStopAxes(s32 a0);
void PlayerStartSfx(s32 a0, u16 a1);
s32 PlayerFaceHeldDirection(void);
s32 PlayerLand(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerCheckBump(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerStopAtWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fe68(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void sub_08041e8c(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void PlayerActionFloatUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    if (gCurTask->unk73 != 6 && PlayerCheckDropAbility() != 0)
    {
        if (gCurTask->unk73 == 5)
            gCurTask->unk73 = 2;
        return;
    }
    PlayerTurnToHeldDirection();
    switch (gCurTask->unk73)
    {
    case 0:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 65)
        {
            PlayerSetMotionYPreset(8);
        }
        else if (gCurTask->unk7A & 1)
        {
            PlayerStopAxes(2);
        }
        else if (gCurTask->unk7B & 1)
        {
            gCurTask->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        else
        {
            PlayerSetMotionYPreset(7);
        }
        gCurTask->unk7A = 0;
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        break;
    case 1:
        if (PlayerCheckEnterDoor() != 0)
            break;
        gCurTask->unk7A = 0;
        PlayerSetMotionYPreset(8);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        if (gCurTask->unk7A & 1)
            PlayerStopAxes(2);
        if (gLatchedPressedKeys[gCurTask->unk88->unk00] & 2)
        {
            gCurTask->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        break;
    case 2:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (!(gCurTask->unk7A & 1))
            PlayerSetMotionYPreset(7);
        else
            PlayerStopAxes(2);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        t = gCurTask;
        if (t->unk7A & 1)
        {
            t->unk73 = 3;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            PlayerStopAxes(2);
            break;
        }
        if (gLatchedHeldKeys[t->unk88->unk00] & 65)
        {
            t->unk73 = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedPressedKeys[t->unk88->unk00] & 2)
        {
            t->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (!(t->unk7B & 1))
            break;
        PlayerStopAxes(2);
        gCurTask->unk28 = -1;
        gCurTask->unk73 = 5;
        TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        break;
    case 3:
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (!(gCurTask->unk7A & 1))
            PlayerSetMotionYPreset(7);
        else
            PlayerStopAxes(2);
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtCeilingAndWall();
        u = gCurTask;
        if (!(u->unk7A & 1))
        {
            u->unk73 = 2;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedHeldKeys[u->unk88->unk00] & 65)
        {
            u->unk73 = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        if (gLatchedPressedKeys[u->unk88->unk00] & 2)
        {
            u->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
            break;
        }
        break;
    case 4:
        v = gCurTask;
        if (!(v->unk7A & 1))
        {
            if (!(v->unk7B & 1))
                PlayerSetMotionYPreset(2);
            else
                PlayerSetMotionYPreset(13);
        }
        PlayerSetMotionXPreset(7, 72);
        w = gCurTask;
        if (w->unk58 < 0)
        {
            if (gTerrainResult.unk1 != 0)
                w->unk58 = 0;
        }
        else if (w->unk7A & 1)
        {
            if ((u32)w->unk58 > 0xC000)
                PlayerLand(1);
            else
                PlayerCheckLanding();
        }
        PlayerStopAtWall();
        break;
    case 5:
        PlayerSetMotionXPreset(6, 72);
        PlayerStopAtWall();
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 65)
        {
            PlayerStopAxes(2);
            gCurTask->unk73 = 1;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        }
        else if (gLatchedPressedKeys[gCurTask->unk88->unk00] & 2)
        {
            PlayerStopAxes(2);
            gCurTask->unk73 = 4;
            TaskSetEntry(PlayerActionFloat, gCurTaskIdx);
        }
        break;
    case 6:
        PlayerRequestLocomotion();
        x = gCurTask;
        if (x->unk7A & 1)
        {
            if ((u32)x->unk58 > 0xC000)
                PlayerLand(1);
            else
                PlayerCheckLanding();
        }
        break;
    }
    if (PlayerHasCrossedWaterSurface(0) != 0)
    {
        gCurTask->unk88->unk06 = 0;
        gCurTask->unk88->unk01 = 23;
    }
}

void PlayerActionDuck(void)
{
    struct Task *t;
    struct PlayerState *p;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 6;
    gCurTask->unk15 = 10;
    t = gCurTask;
    if (t->unk88->unk05 != 6)
    {
        t->unk88->unk64 = (u32)gUnk_0873BD28;
        t->unk88->unk68 = (u32)gUnk_0873CB24;
        t->unk2C = t->unk88->unk4B;
        PlayerSetMotionXPreset(0, 72);
    }
    gCurTask->unk28 = 8;
    gCurTask->unk88->unk33 = sub_0803fd20(gCurTask->unk88->unk00);
    p = gCurTask->unk88;
    p->unk35 = 0;
    p->unk34 = 0;
    gCurTask->unk46 = gUnk_0873D4BC[gCurTask->unk88->unk0D][sub_0803fd20(gCurTask->unk88->unk00)];
    switch (gCurTask->unk88->unk0D)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
    case 15:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(6);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(6);
            }
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void PlayerActionDuckUpdate(void)
{
    s32 dir = gCurTask->facing;

    PlayerFaceHeldDirection();
    while (sub_0803fe68() == 0 && PlayerCheckDropAbility() == 0)
    {
        u16 *q = gLatchedPressedKeys;
        struct Task *t = gCurTask;

        if (q[t->unk88->unk00] & 3)
        {
            t->unk88->unk01 = 11;
            break;
        }
        if (!(gLatchedHeldKeys[t->unk88->unk00] & 128))
        {
            PlayerRequestLocomotion();
            break;
        }
        if (gTerrainResult.unk5 != 0)
        {
            if (t->unk28 == 0)
            {
                t->unk7A = 0;
                gCurTask->unk88->unk01 = 7;
                gCurTask->unk84 = 0;
                gCurTask->posY += 0x10000;
                break;
            }
            t->unk28--;
        }
        {
            struct Task *w = gCurTask;

            if (w->unk88->unk4B != w->unk2C || dir != w->facing)
            {
                if (gUnk_03001F30 == 0)
                    TaskSetEntry(PlayerActionDuck, gCurTaskIdx);
                else
                    TaskSetEntry(sub_08041e8c, gCurTaskIdx);
            }
        }
        break;
    }
    gCurTask->unk2C = gCurTask->unk88->unk4B;
    if (gTerrainResult.unk0 != 0)
        PlayerStopAxes(1);
}

void PlayerActionSlide(void)
{
    struct Task *t;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 7;
    gCurTask->unk15 = 11;
    t = gCurTask;
    if (t->unk88->unk05 != 7)
    {
        t->unk28 = 0;
        t->unk73 = 0;
        gCurTask->unk88->unk14 = 10;
        PlayerStartSfx(118, gCurTask->unk88->unk00);
        gCurTask->unk88->unk6C = gUnk_0873CC84;
        PlayerSetMotionXPreset(11, 0);
        CreatePlayerEffect(gCurTask->unk88->unk00, 8, 0);
    }
    switch (gCurTask->unk73)
    {
    case 0:
        gCurTask->unk46 = gUnk_0873D5CA[gCurTask->unk88->unk0D][0];
        switch (gCurTask->unk88->unk0D)
        {
        case 0:
        default:
            TaskSetFrame(gCurTask->unk46);
            TaskSleepForever();
        case 1:
        case 2:
        case 5:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
            }
        case 4:
        case 15:
        case 16:
        case 17:
        case 19:
        case 22:
        case 23:
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
    case 1:
        TaskSetFrame(gUnk_0873D5CA[gCurTask->unk88->unk0D][1]);
        break;
    }
    gCurTask->unk88->unk6C = 0;
    gCurTask->unk28++;
    TaskSleepForever();
}

void PlayerActionSlideUpdate(void)
{
    struct Task *t;
    struct PlayerState *p;

    while (1)
    {
        if (sub_0803fe68() != 0)
        {
            if (gCurTask->unk5C == 0)
                PlayerSetMotionXPreset(5, 72);
            break;
        }
        if (PlayerCheckDropAbility() != 0)
            break;
        t = gCurTask;
        if (t->unk28 != 0)
        {
            t->unk88->unk01 = 1;
        }
        else if (gTerrainResult.unk0 != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(1);
            gCurTask->unk88->unk01 = 1;
        }
        else
        {
            if (abs(t->unk54) <= 0x7FFF)
            {
                t->unk73 = 1;
                TaskSetEntry(PlayerActionSlide, gCurTaskIdx);
            }
            if (abs(gCurTask->unk54) > 0xE000)
                RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A, gUnk_0873BE9C);
        }
        break;
    }
    p = gCurTask->unk88;
    if ((s16)p->unk14 == 0)
    {
        PlayerSetMotionXPreset(5, 72);
        gCurTask->unk88->unk14--;
    }
    else if ((s16)p->unk14 > 0)
    {
        p->unk14--;
    }
}
