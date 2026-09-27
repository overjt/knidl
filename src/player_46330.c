#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_46330.c (0x08046330-0x08046BFF, issue #87).
 *
 * Player action bodies, part 15: action 39 and per-frame handler 36.
 * sub_08046330 (action 39, mode 13) is a re-entrant five-state machine
 * over Task.unk73: state 0 winds up (animation 0x701, effect 6, sound
 * 154), state 1 installs the script gUnk_0873CB34 and the block hit-box
 * set gUnk_0873CDAC and spins in an endless yield loop, state 2 turns
 * round (it negates the facing Task.unk43) and goes back to state 1,
 * state 3 finishes the move and state 4 bounces off (sound 153, the
 * screen shake sub_080261d4(4), velocity preset 36).  Its handler
 * sub_0804676c is what leaves the spin: every frame of state 1 it
 * re-binds the coroutine to state 3 on a newly-pressed B, to state 2
 * when the held direction opposes the facing, and to state 4 when the
 * collision block gTerrainResult reports a hit; it keeps the player on
 * slopes and ledges with M06's terrain probes IsFullBlockAtPixel and
 * IsWaterAtPixel and registers the collider gUnk_0873BF14. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (good/sub_08045a50.c). */
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

extern u32 gPlayerDefaultTerrainBox[];
extern u32 gUnk_0873CB34[];
extern u32 gUnk_0873CDAC[];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gTerrainResult;
extern u32 gUnk_0873BF14[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u8 IsWaterAtPixel(s16 x, s16 y);
s32 IsFullBlockAtPixel(u16 x, u16 y);
void sub_080261d4(u16 a);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerStartSfx(s32 a0, u16 a1);
void PlayerStopSfx(void);
s32 PlayerLand(s32 a0);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_08046330(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 36;
    {
        struct Task *t = gCurTask;
        if (t->unk88->unk05 != 13) {
            t->unk28 = 0;
            if (t->unk7A & 1)
                t->unk2C = 1;
            else
                t->unk2C = 0;
            gCurTask->unk30 = 0;
            CreatePlayerEffect(gCurTask->unk88->unk00, 34, 0);
            gCurTask->unk80 = 8;
            gCurTask->unk73 = 0;
        }
    }
again:
    {
        struct Task *t = gCurTask;
        t->unk88->unk6C = 0;
        switch (t->unk73) {
        case 0:
            PlayerSetMotionXPreset(11, 44);
            TaskSetFrame(0x701);
            TaskYieldTrampoline(4);
            CreatePlayerEffect(gCurTask->unk88->unk00, 6, 30);
            PlayerSetMotionXPreset(11, 45);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 7);
            PlayerStartSfx(154, gCurTask->unk88->unk00);
            gCurTask->unk73 = 1;
            PlayerSetMotionXPreset(11, 46);
            /* fallthrough */
        case 1:
            {
                struct Task *u = gCurTask;
                u->unk88->unk68 = (u32)gUnk_0873CB34;
                u->unk88->unk6C = gUnk_0873CDAC;
                SetPlayerInvulnerability(3, 0, u->unk88->unk00);
            }
            while (1) {
                if (gCurTask->unk7A & 1 || gCurTask->unk28 != 0)
                    CreatePlayerEffect(gCurTask->unk88->unk00, 34, 1);
                TaskSetFrame(0x70A);
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            }
        case 2:
            SetPlayerInvulnerability(255, 0, gCurTask->unk88->unk00);
            PlaySfxIfLocalPlayer(245, gCurTask->unk88->unk00);
            if (gCurTask->unk7A & 1 || gCurTask->unk28 != 0) {
                CreatePlayerEffect(gCurTask->unk88->unk00, 6, 4);
                TaskSetFrame(0x70E);
                TaskYieldTrampoline(4);
                PlayerSetMotionXPreset(11, 47);
                gCurTask->unk43 = -gCurTask->unk43;
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                PlayerSetMotionXPreset(11, 48);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                PlayerSetMotionXPreset(11, 49);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                PlayerStopAxes(1);
            } else {
                PlayerSetMotionXPreset(11, 50);
                TaskSetFrame(0x70E);
                TaskYieldTrampoline(4);
                gCurTask->unk43 = -gCurTask->unk43;
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            {
                struct Task *u = gCurTask;
                if (u->unk7A & 1 || u->unk28 != 0) {
                    CreatePlayerEffect(u->unk88->unk00, 6, 4);
                    PlayerSetMotionXPreset(11, 46);
                }
            }
            gCurTask->unk73 = 1;
            goto again;
        case 3:
            PlayerStopSfx();
            {
                struct Task *u = gCurTask;
                u->unk88->unk68 = (u32)gPlayerDefaultTerrainBox;
                SetPlayerInvulnerability(255, 0, u->unk88->unk00);
            }
            PlayerSetMotionXPreset(11, 51);
            TaskSetFrame(0x702);
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 6);
            TaskSetFrame(0x701);
            gCurTask->unk73 = 5;
            break;
        case 4:
            {
                struct Task *u = gCurTask;
                u->unk88->unk68 = (u32)gPlayerDefaultTerrainBox;
                SetPlayerInvulnerability(255, 0, u->unk88->unk00);
            }
            PlayerStopSfx();
            PlaySfxIfLocalPlayer(153, gCurTask->unk88->unk00);
            sub_080261d4(4);
            gCurTask->unk7A = 0;
            PlayerSetMotionXPreset(11, 17);
            PlayerSetMotionYPreset(36);
            while (1) {
                TaskSetFrame(0x702);
                TaskYieldTrampoline(1);
                gCurTask->unk6C = 0;
                do {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(1);
                } while ((s16)++gCurTask->unk6C <= 6);
            }
        }
    }
    TaskSleepForever();
}

void sub_0804676c(void)
{
    {
        struct Task *t = gCurTask;
        if (t->unk30 == 0) {
            if (gTerrainResult.unkD != 0) {
                t->unk88->unk14 = 5;
                t->unk30 = 1;
            } else {
                t->unk88->unk14 = 0;
            }
        } else {
            struct PlayerState *p = t->unk88;
            if ((s16)p->unk14 == 0) {
                if (IsFullBlockAtPixel(t->unk48, (t->unk4A & ~15) + 16) != 0)
                    gCurTask->unk7A = 1;
            } else {
                p->unk14--;
            }
        }
    }
    {
        struct Task *t = gCurTask;
        if (t->unk7A & 1) {
            t->unk30 = 0;
            t->unk88->unk14 = 0;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
    case 3:
        if (gCurTask->unk7A & 1) {
            PlayerLand(1);
            PlayerSetMotionXPreset(0, 72);
        } else {
            PlayerSetMotionYPreset(2);
            PlayerSetMotionXPreset(11, 2);
        }
        if (gTerrainResult.unk0 == 0)
            break;
        {
            struct Task *t = gCurTask;
            if (t->unk73 == 0)
                t->unk54 = 0;
            else
                PlayerStopAxes(1);
        }
        break;
    case 1:
        {
            u16 k = gLatchedPressedKeys[gCurTask->unk88->unk00] & 2;
            struct Task *t = gCurTask;
            if (k) {
                t->unk73 = 3;
                TaskSetEntry(sub_08046330, gCurTaskIdx);
            } else {
                if (t->unk7A & 1) {
                    t->unk28 = 0;
                    PlayerSetMotionXPreset(11, 46);
                } else if (t->unk28 == 0) {
                    if (IsWaterAtPixel(t->unk48, t->unk4A + 15) != 0) {
                        {
                            struct Task *u = gCurTask;
                            if (u->unk7B & 1)
                                u->unk4A -= 8;
                        }
                        PlayerStopAxes(2);
                        {
                            struct Task *u = gCurTask;
                            u->unk50 = ((u->unk4A & 0xFFF0) + 5) << 16;
                            u->unk4A = u->unk50 >> 16;
                            u->unk28 = 1;
                        }
                    } else {
                        if (gCurTask->unk88->unk48 & 3)
                            PlayerStopAxes(1);
                        PlayerSetMotionXPreset(11, 50);
                    }
                } else {
                    PlayerSetMotionXPreset(11, 46);
                }
                if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 16 && gCurTask->unk43 == -1)
                    || (gLatchedHeldKeys[gCurTask->unk88->unk00] & 32 && gCurTask->unk43 == 1)) {
                    gCurTask->unk73 = 2;
                    TaskSetEntry(sub_08046330, gCurTaskIdx);
                } else if (*(u16 *)&gTerrainResult != 0
                           || ((gCurTask->unk88->unk48 & 3)
                               && ((gCurTask->unk7A & 1) || gCurTask->unk28 != 0))) {
                    if (gTerrainResult.unk1 != 0)
                        gCurTask->unk58 = 0;
                    else
                        PlayerStopAxes(1);
                    gCurTask->unk73 = 4;
                    TaskSetEntry(sub_08046330, gCurTaskIdx);
                }
            }
        }
        {
            struct Task *t = gCurTask;
            if ((t->unk7A & 1) || t->unk28 != 0) {
                PlayerStopAxes(2);
            } else {
                if (t->unk2C != 0)
                    PlayerStopAxes(2);
                PlayerSetMotionYPreset(2);
            }
        }
        RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A, gUnk_0873BF14);
        break;
    case 2:
        {
            u16 k = gLatchedPressedKeys[gCurTask->unk88->unk00] & 2;
            if (k) {
                gCurTask->unk73 = 3;
                TaskSetEntry(sub_08046330, gCurTaskIdx);
                break;
            }
            if (*(u16 *)&gTerrainResult != 0 || (gCurTask->unk88->unk48 != 0 && (gCurTask->unk7A & 1))) {
                if (gTerrainResult.unk1 != 0)
                    gCurTask->unk58 = 0;
                else
                    PlayerStopAxes(1);
                gCurTask->unk73 = 4;
                TaskSetEntry(sub_08046330, gCurTaskIdx);
            }
        }
        if (gCurTask->unk7A & 1) {
            gCurTask->unk2C = 1;
            if (gCurTask->unk58 != 0)
                PlayerStopAxes(2);
        } else {
            gCurTask->unk2C = 0;
            PlayerSetMotionYPreset(2);
        }
        break;
    case 4:
        {
            struct Task *t = gCurTask;
            if (t->unk58 < 0) {
                if (gTerrainResult.unk1 != 0)
                    t->unk58 = 0;
                break;
            }
        }
        /* fallthrough */
    case 5:
        PlayerRequestLocomotion();
        {
            struct PlayerState *p = gCurTask->unk88;
            if (p->unk01 == 2)
                p->unk01 = 4;
        }
        return;
    }
    if (gCurTask->unk7B & 1) {
        PlayerStopSfx();
        gCurTask->unk88->unk01 = 23;
    }
    {
        struct Task *t = gCurTask;
        if (t->unk7A & 1)
            t->unk2C = 1;
        else
            t->unk2C = 0;
    }
}
