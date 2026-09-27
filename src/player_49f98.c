#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_49f98.c (0x08049F98-0x0804A54B, issue #88).
 *
 * Player action bodies, part 17: action 46 and per-frame handler 43.
 * sub_08049f98 (action 46, mode 13) is a five-state move whose states
 * fall into each other; a fresh entry starts in state 0, or in state 4
 * when it comes from mode 5.  State 0 starts it (effect 42, animation
 * 0xB2D), state 1 points PlayerState.hitBoxSet at the block hit-box set
 * gUnk_0873CF5C with effects 42 x4 and sound 174 and runs velocity
 * presets 38, 0 and 1, and states 3-4 hand the player over to mode 5
 * with handler 7 (animation 0xAD2, then the ability's loop from
 * gUnk_0873D3B8[ability][1]).  Its handler sub_0804a258
 * flashes the palette gUnk_081F59F0 (the VRAM transfer queue
 * RequestCopy) in state 1, re-binds state 3 on a newly-pressed B after
 * the PlayerState.unk14 frames, registers the collider gUnk_0873C228,
 * steers with the held left/right keys, picks one of five animation
 * rows 0xB2E-0xB3E by |Task.velX| and cycles Task.unk46 through them,
 * ends the move on landing and re-binds state 4 on a ceiling hit. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gTerrainResult` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u32 gUnk_0873CF5C[];
extern s16 gUnk_0873D3B8[][2];
extern u16 gFrameCount;
extern u16 gUnk_081F59F0[];
extern u32 gUnk_0873C228[];
extern u8 gObjPalette[];              /* OBJ palette buffer (M11 spelling) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern struct Unk03005550 gTerrainResult;

void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void TaskSetFrameNoFlip(s32 a);
void TaskSetFrameFlip(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
void PlayerStartSfx(s32 a0, u16 a1);
void PlayerStopSfx(void);
s32 PlayerFaceHeldDirection(void);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerCheckBump(void);
s32 PlayerCheckLadder(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_08049f98(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 43;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            if (t->player->prevMode == 5)
                t->unk73 = 4;
            else
                t->unk73 = 0;
            gCurTask->unk80 = 15;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 5);
        PlayerStopAxes(2);
        PlayerStartOffsetScript(5);
        TaskSetFrame(0xB2D);
        TaskYieldTrampoline(4);
        gCurTask->unk73 = 1;
        gCurTask->player->unk14 = 4;
        /* fallthrough */
    case 1:
        PlayerSetMotionYPreset(38);
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        gCurTask->player->hitBoxSet = gUnk_0873CF5C;
        PlayerStartSfx(174, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 42, 4);
        gCurTask->unk46 = 0;
        gCurTask->unk28 = 2;
        TaskYieldTrampoline(23);
        PlayerSetMotionYPreset(0);
        TaskYieldTrampoline(10);
        gCurTask->unk73 = 2;
        /* fallthrough */
    case 2:
        {
            struct Task *t = gCurTask;
            t->player->unk42 &= 0xFFEF;
            SetPlayerInvulnerability(255, 0, t->player->playerIndex);
        }
        TaskYieldTrampoline(13);
        PlayerSetMotionYPreset(1);
        TaskYieldTrampoline(5);
        gCurTask->unk73 = 3;
        /* fallthrough */
    case 3:
        gCurTask->player->unk42 &= 0xFFEF;
        PlayerSetMotionYPreset(2);
        gCurTask->player->prevMode = gCurTask->player->mode;
        gCurTask->player->mode = 5;
        gCurTask->updateState = 7;
        gCurTask->player->hitBoxSet = 0;
        gCurTask->unk73 = 4;
        gCurTask->player->unk14 = 300;
        TaskSetFrame(0xAD2);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 4);
        /* fallthrough */
    case 4:
        {
            struct Task *t = gCurTask;
            struct PlayerState *p;
            t->player->unk42 &= 0xFFEF;
            p = t->player;
            if (p->mode != 5) {
                p->prevMode = p->mode;
                gCurTask->player->mode = 5;
                gCurTask->updateState = 7;
                gCurTask->player->hitBoxSet = 0;
                PlayerSetMotionYPreset(2);
            }
        }
        {
            struct Task *t = gCurTask;
            t->player->unk14 = 30;
            t->unk46 = gUnk_0873D3B8[t->player->ability][1];
        }
        while (1) {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
    }
}

void sub_0804a258(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73) {
    case 1:
        t->player->unk42 &= 0xFFEF;
        if ((gFrameCount & 7) <= 3) {
            RequestCopy(2, gUnk_081F59F0, gObjPalette + (t->tileWord >> 12) * 32, 64);
            gCurTask->player->unk42 |= 16;
        }
        /* fallthrough */
    case 2:
        {
            struct Task *u = gCurTask;
            struct PlayerState *p = u->player;
            if ((s16)p->unk14 == 0) {
                if (gLatchedPressedKeys[p->playerIndex] & 2) {
                    u->unk73 = 3;
                    TaskSetEntry(sub_08049f98, gCurTaskIdx);
                    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
                    PlayerStopSfx();
                    CreatePlayerEffect(gCurTask->player->playerIndex, 42, 3);
                    PlayerStopAxes(2);
                }
            } else {
                p->unk14--;
            }
        }
        /* fallthrough */
    case 3:
        {
            struct Task *u = gCurTask;
            if (u->unk28-- == 0) {
                u->unk46 = (u->unk46 + 1) & 3;
                u->unk28 = 2;
            }
        }
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873C228);
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) {
            PlayerFaceHeldDirection();
            PlayerSetMotionXPreset(11, 60);
        } else {
            PlayerSetMotionXPreset(11, 61);
        }
        if (gTerrainResult.unk0 != 0)
            PlayerStopAxes(1);
        {
            struct Task *u = gCurTask;
            if ((u8)(u->unk73 - 1) <= 1) {
                s32 a;
                if (abs(u->velX) <= 0x4000)
                    a = 0xB2E;
                else if (abs(u->velX) <= 0x10000)
                    a = 0xB32;
                else if (abs(u->velX) <= 0x14000)
                    a = 0xB36;
                else if (abs(u->velX) <= 0x1C000)
                    a = 0xB3A;
                else
                    a = 0xB3E;
                {
                    struct Task *w = gCurTask;
                    if (w->velX == 0)
                        TaskSetFrame((s16)(w->unk46 + a));
                    else if (w->velX < 0)
                        TaskSetFrameFlip(w->unk46 + a);
                    else
                        TaskSetFrameNoFlip((s16)(w->unk46 + a));
                }
            }
        }
        {
            struct Task *w = gCurTask;
            if (w->onGround & 1) {
                PlayerCheckBump();
                PlayerLand(1);
                PlayerRequestLocomotion();
                break;
            }
            if (w->velY < 0 && (gTerrainResult.unk1 != 0 || (w->player->boundsClamp & 4))) {
                w->velY = 0;
                w->unk73 = 4;
                TaskSetEntry(sub_08049f98, gCurTaskIdx);
                SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
                PlayerStopSfx();
                return;
            }
        }
        PlayerCheckLadder();
        break;
    }
    {
        struct PlayerState *p = gCurTask->player;
        if (p->requestedAction != 0)
            p->unk42 &= 0xFFEF;
    }
}
