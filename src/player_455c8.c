#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_455c8.c (0x080455C8-0x08045D33, issue #87).
 *
 * Player action bodies, part 13: actions 36-37 and per-frame handlers
 * 33-34.  sub_080455c8 (action 36, mode 13) is a four-state machine over
 * Task.unk73: state 0 starts the move (velocity preset 34, effect 32,
 * animation 0x5CB) and installs the attack box gUnk_0873CCA4 in
 * PlayerState.hitBoxSet, state 1 swaps in the scripts gUnk_0873CB2C /
 * gUnk_0873BD3C (PlayerState.terrainBox/unk64) and cycles the hit-box row
 * Task.unk2C through 0-2, state 2 restores the default scripts
 * gPlayerDefaultTerrainBox / gPlayerDefaultBodyBox and lands (preset 2), and state 3 is the
 * bounce-off (sound 153, RequestScreenShake(4), preset 23).  Its handler
 * sub_08045a50 drives it from the collision block gTerrainResult -
 * re-binding state 3 on a hit, fading the palette of gUnk_0873B510[]
 * row Task.unk2C and registering the box gUnk_0873BF00.
 * sub_08045c40 (action 37) is a linear script (animations
 * 0x658/0x65A, M14's CreatePlayerObject, sound 172); its handler sub_08045d18
 * waits for it to finish. */

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

/* gUnk_0873B510[]: a palette fade, src/dst palettes and the blend step */
struct M12Fade
{
    /*0x00*/ u16 *unk0;
    /*0x04*/ u16 *unk4;
    /*0x08*/ s32 unk8;
};

extern u32 gPlayerDefaultTerrainBox[];
extern u32 gPlayerDefaultBodyBox[];             /* stored to PlayerState.bodyBox as (u32)gPlayerDefaultBodyBox */
extern u32 gUnk_0873CCA4[];
extern u32 gUnk_0873CB2C[];
extern u32 gUnk_0873BD3C[];
extern struct Unk03005550 gTerrainResult;
extern struct M12Fade gUnk_0873B510[];
extern u8 gObjPalette[];              /* OBJ palette buffer (M11 spelling) */
extern u32 gUnk_0873BF00[];

void TaskYieldTrampoline(s32 frames);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void RequestScreenShake(u16 a);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerStartSfx(s32 a0, u16 a1);
void PlayerStopSfx(void);
s32 sub_0803e55c(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_080455c8(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 33;
    {
        struct Task *t = gCurTask;
        if (t->player->prevMode != 13) {
            t->unk73 = 0;
            gCurTask->unk80 = 5;
            gCurTask->unk28 = 0;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        gCurTask->onGround = 0;
        PlayerSetMotionXPreset(11, 38);
        PlayerSetMotionYPreset(34);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 0);
        TaskSetFrame(0x5CB);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        PlayerStopAxes(2);
        TaskYieldTrampoline(3);
        gCurTask->player->hitBoxSet = gUnk_0873CCA4;
        PlayerSetMotionXPreset(11, 39);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerSetMotionXPreset(11, 40);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 1;
        /* fallthrough */
    case 1:
        PlayerStartSfx(137, gCurTask->player->playerIndex);
        gCurTask->onGround = 0;
        gCurTask->player->terrainBox = (u32)gUnk_0873CB2C;
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        gCurTask->player->bodyBox = (u32)gUnk_0873BD3C;
        gCurTask->player->unk42 |= 16;
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 0;
        TaskSetFrame(0x5CF);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 3);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 4);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 1;
        TaskSetFrame(0x5D3);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 1;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 2;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 0;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        gCurTask->unk2C = 1;
        TaskSetFrame(0x5DF);
        TaskYieldTrampoline(2);
        gCurTask->unk2C = -1;
        gCurTask->player->unk42 &= 0xFFEF;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStopSfx();
        gCurTask->unk73 = 2;
        /* fallthrough */
    case 2:
        gCurTask->player->hitBoxSet = 0;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
        gCurTask->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        gCurTask->player->bodyBox = (u32)gPlayerDefaultBodyBox;
        PlayerSetMotionXPreset(11, 41);
        PlayerSetMotionYPreset(2);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 5);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 6);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 7);
        CreatePlayerEffect(gCurTask->player->playerIndex, 32, 8);
        TaskSetFrame(0x53C);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x544);
        gCurTask->unk73 = 4;
        break;
    case 3:
        gCurTask->player->hitBoxSet = 0;
        SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
        gCurTask->player->unk42 &= 0xFFEF;
        PlayerStopSfx();
        gCurTask->onGround = 0;
        PlaySfxIfLocalPlayer(153, gCurTask->player->playerIndex);
        RequestScreenShake(4);
        PlayerSetMotionXPreset(11, 17);
        PlayerSetMotionYPreset(23);
        TaskSetFrame(0x544);
        break;
    }
    TaskSleepForever();
}

void sub_08045a50(void)
{
    switch (gCurTask->unk73) {
    case 0:
        gCurTask->onGround = 0;
        {
            struct Task *t = gCurTask;
            if (t->velY < 0 && gTerrainResult.unk1 != 0)
                t->velY = 0;
        }
        if (gTerrainResult.unk0 == 0)
            break;
        PlayerStopAxes(1);
        {
            s8 d = gCurTask->facing;
            if ((d == 1 && gTerrainResult.unk0 == 1)
                || (d == -1 && gTerrainResult.unk0 == 2))
                gCurTask->unk28 = 1;
        }
        break;
    case 1:
        {
            struct Task *t = gCurTask;
            if (t->onGround & 1) {
                if (gTerrainResult.unk4 != 0) {
                    t->player->requestedAction = 2;
                    PlayerSetMotionXPreset(11, 41);
                    SetPlayerInvulnerability(255, 0, gCurTask->player->playerIndex);
                }
            } else if (*(u16 *)&gTerrainResult != 0 || t->unk28 != 0
                       || (t->player->unk48 & 11) != 0) {
                gCurTask->unk73 = 3;
                TaskSetEntry(sub_080455c8, gCurTaskIdx);
            }
        }
        {
            struct Task *t = gCurTask;
            if (t->unk2C != -1) {
                struct M12Fade *f = &gUnk_0873B510[t->unk2C];
                t->unk6E += f->unk8;
                if (t->unk6E > 255)
                    t->unk6E = 256;
                BlendColors(f->unk0, f->unk4, (u16)gCurTask->unk6E, 16,
                             (u16 *)(gObjPalette + ((gCurTask->tileWord >> 12) << 5)));
            }
        }
        RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                     gUnk_0873BF00);
        break;
    case 2:
        sub_0803e55c();
        break;
    case 3:
    case 4:
        PlayerRequestLocomotion();
        break;
    }
    if (PlayerHasCrossedWaterSurface(0) != 0)
        gCurTask->player->requestedAction = 23;
    {
        struct PlayerState *p = gCurTask->player;
        if (p->requestedAction != 0)
            p->unk42 &= 0xFFEF;
    }
}

void sub_08045c40(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 34;
    gCurTask->unk28 = 0;
    gCurTask->unk80 = 0;
    TaskSetFrame(0x658);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(0x65A);
    TaskYieldTrampoline(2);
    CreatePlayerObject(gCurTask->player->playerIndex, 6, 0);
    PlaySfxIfLocalPlayer(172, gCurTask->player->playerIndex);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 15);
    TaskSetFrame(0x658);
    TaskYieldTrampoline(2);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08045d18(void)
{
    if (gCurTask->unk28 != 0)
        PlayerRequestLocomotion();
    sub_0803e55c();
}
