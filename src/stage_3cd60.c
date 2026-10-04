#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "room.h"
#include "effect.h"
#include "actor.h"

struct PlayerHitBoxSet { u8 unk00; u8 unk01; u8 offsetX; u8 offsetY; u8 *boxes; };
struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
struct PlayerBodyBox { u32 w[5]; };

/* Not from collision.h or player.h: this file's view of gTerrainResult and
   gUnk_020055C4 differs (lesson 3.517). */
extern u16 gEndingLocalPlayer;
extern struct PlayerHitBoxSet gPlayerHitBoxSets[];
extern u8 gUnk_020055C4[];
extern struct PlayerBodyBox gPlayerBodyBoxes[];
extern struct M11Buf gPlayerHitBoxLists[];
extern u16 gUnk_02007F60[];
extern u16 gPlayerBubbleTimers[];
extern u16 gObjPaletteBank1[];
extern u8 gCreditsDemoSet;
extern u8 gTerrainResult[];
extern u16 gPlayerPalettes[][16];
extern u32 gUnk_080DC728[];
extern u8 gUnk_080DCA28[];
extern u32 gUnk_080DCC28[];
extern u32 gUnk_080DCC48[];
extern u8 gUnk_081BC050[];
extern u8 gUnk_081BE6BC[];
extern u16 gUnk_08226254[];
extern u16 gUnk_0873AF0C[][2];
extern s8 gUnk_0873AF20[][2];
extern u8 gUnk_0873AF30[][2];
extern u8 gUnk_0873AF3A[][2];
extern u8 gUnk_0873AF42[];
extern u32 gPlayerDefaultTerrainBox[];
extern u8 gAbilityBButtonActions[];
extern s16 gUnk_0873D210[];
extern s16 gUnk_0873D2E0[];
extern u16 gUnk_0873D79E[];
extern u16 gUnk_0873DB44[][2];
extern u32 gUnk_08751990[];
extern u32 gUnk_087519CC[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 RandomRange(u32 range);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
s32 IsOnScreen(s16 x, s16 y);
u32 IsWorldPosOnScreen(s16 a, s16 b);
s32 IsTaskBelowPlayerBounds(struct Task *t);
void sub_08033414(void);
s32 sub_0803d010(void);
void sub_0803d7c4(void);
s32 sub_0803d870(void);
void sub_0803db74(void);
void sub_0803e28c(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerUpdateInvincibility(void);
void sub_0803e8ec(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerStopAtWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
void sub_080409b8(s32 a0);
void sub_08040a44(s16 p0, s16 p1);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void PlayerSetMotionYPreset(s32 a0);

void PlayerPlayBump(void)
{
    u8 v;
    s16 *p;

    v = gCurTask->player->bumpKind;
    if (v == 0)
        return;
    gCurTask->player->bumpKind = 0;
    if (gMetaKnightmareMode == 0)
        p = &gUnk_0873D210[gCurTask->player->ability * 4];
    else
        p = gUnk_0873D2E0;
    CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_IMPACT_STAR, 0);
    if (gCurTask->player->mouthState == 1) {
        if (v == 2) {
            TaskSetFrame(0x15F);
            TaskYieldTrampoline(2);
        }
    } else {
        switch (v) {
        case 1:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            TaskSetFrame(p[0]);
            TaskYieldTrampoline(4);
            break;
        case 2:
            TaskSetFrame(p[3]);
            TaskYieldTrampoline(6);
            break;
        case 3:
        case 5:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            TaskSetFrame(p[1]);
            TaskYieldTrampoline(4);
            break;
        case 4:
        case 6:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            TaskSetFrame(p[2]);
            TaskYieldTrampoline(4);
            break;
        }
    }
}

void sub_0803ce98(void)
{
    struct PlayerState *ps;
    struct PlayerState *ps2;
    s32 v, c;

    ps = gCurTask->player;
    if (ps->mode != 0 && ps->mode != 6)
        return;
    if (ps->blinkTimer != 0) {
        ps->blinkTimer--;
    } else {
        if (gUnk_0873AF20[ps->blinkScriptPos][0] == 0)
            ps->blinkScriptPos = 0;
        gCurTask->player->blinkTimer = gUnk_0873AF20[gCurTask->player->blinkScriptPos][0];
        gCurTask->player->blinkShown = gUnk_0873AF20[gCurTask->player->blinkScriptPos++][1];
    }
    ps2 = gCurTask->player;
    if (ps2->blinkShown == 0)
        return;
    c = (ps2->mode != 0);
    v = gUnk_0873AF0C[ps2->facingSlope][c];
    if (ps2->mouthState == 1)
        v += 10;
    if (gLocalPlayer != ps2->playerIndex || gInHub != 0) {
        if (IsWorldPosOnScreen(gCurTask->pixelX, gCurTask->pixelY) == 0)
            return;
    }
    if (sub_0803d010() != 0)
        return;
    QueueSprite(gCurTask->layer, gUnk_08751990[v],
                 (u16)(gCurTask->spriteFlags & 0x8000),
                 gCurTask->tileWord & 0xF000,
                 gCurTask->pixelX - gSpriteCameraX,
                 gCurTask->pixelY - gSpriteCameraY);
}

/* True while the running task's unk3C (state id) is one of 1511..1515. */
s32 sub_0803d010(void)
{
    if ((u16)(gCurTask->frame - 1511) <= 4)
        return 1;
    return 0;
}

void CreatePlayer(s32 a0)
{
    struct Task *t;

    t = &gTasks[TaskCreateFrom(TASK_PLAYER, 0)];
    t->posX = gRoomEntryX << 16;
    t->posY = gRoomEntryY << 16;
    t->pixelX = t->posX >> 16;
    t->pixelY = t->posY >> 16;
    if ((u8)(gUnk_02000020 - 2) <= 1)
        SetPlayerAbilityNoHud(ABILITY_STAR_ROD, -1, a0);
}

/* Reset player record a0 to its start-of-stage state. */
void InitPlayerState(s32 a0)
{
    struct PlayerState *p;

    p = &gPlayerStates[a0];
    p->playerIndex = a0;
    p->prevAction = PLAYER_ACTION_NONE;
    p->action = PLAYER_ACTION_NONE;
    p->requestedAction = PLAYER_ACTION_NONE;
    p->prevMode = 255;
    p->mode = -1;
    p->mouthState = 0;
    p->heldCount = 0;
    p->attachedCount = 0;
    p->catchKind = 0;
    p->pendingAbility = ABILITY_NORMAL;
    p->abilitySwallowCount = 0;
    p->pendingAbilityUses = -1;
    p->ability = gPlayerAbilities[a0];
    p->abilityUses = gPlayerAbilityUses[a0];
    p->flightCoastTimer = 0;
    p->runTapTimer = 0;
    p->invulnerabilityTimer = 0;
    p->unk14 = 0;
    p->invincible = 0;
    p->invincibleTimer = 0;
    p->invincibleFlashStep = 0;
    p->invincibleFlashTimer = 0;
    p->paletteFlashMode = 0;
    p->unk20 = 0;
    p->unk1E = 0;
    p->pixelOffsetY = 0;
    p->pixelOffsetX = 0;
    p->blockBreakCooldown = 0;
    p->offsetScriptDelay = 0;
    p->offsetScriptStep = 0;
    p->sfxPlayer = 0xFFFF;
    p->sfxId = 0;
    p->ownStarSwallowCount = 0;
    p->ownStarInMouth = 0;
    p->unk36 = 0;
    p->unk37 = 0;
    p->shareTimer = 0;
    p->sharedMask = 0;
    p->shareItem = 0;
    p->unk3C = 0;
    p->boundsClamp = 0;
    p->running = 0;
    p->bumpKind = 0;
    p->invulnerability = 0;
    p->unk40 = 0;
    p->unk42 = 0;
    p->blocksBroken = 0;
    p->hitsThisFrame = 0;
    p->unk50 = 0;
    p->savedWallSide = 0;
    p->hiJumpsLeft = 1;
    p->unk50 = 0;
    p->atDoor = 0;
    p->wallSide = 0;
    p->slope = 0;
    p->onSlipperyFloor = 0;
    p->clampedTopY = -1;
    p->driftVelY = 0;
    p->driftVelX = 0;
    p->prevPixelY = 0;
    p->prevPixelX = 0;
    p->bodyBox = 0;
    p->terrainBox = 0;
    p->hitBoxSet = 0;
    p->prevTerrainBox = gPlayerDefaultTerrainBox;
    gPlayerBubbleTimers[a0] = 45;
}

/* Reset player record a0 to its start-of-stage state. */
void InitPlayerStateKeepInvincibility(s32 a0)
{
    struct PlayerState *p;

    p = &gPlayerStates[a0];
    p->playerIndex = a0;
    p->prevAction = PLAYER_ACTION_NONE;
    p->action = PLAYER_ACTION_NONE;
    p->requestedAction = PLAYER_ACTION_NONE;
    p->prevMode = 255;
    p->mode = -1;
    p->mouthState = 0;
    p->heldCount = 0;
    p->attachedCount = 0;
    p->catchKind = 0;
    p->pendingAbility = ABILITY_NORMAL;
    p->abilitySwallowCount = 0;
    p->pendingAbilityUses = -1;
    p->ability = gPlayerAbilities[a0];
    p->abilityUses = gPlayerAbilityUses[a0];
    p->flightCoastTimer = 0;
    p->runTapTimer = 0;
    p->invulnerabilityTimer = 0;
    p->unk14 = 0;
    p->paletteFlashMode = 0;
    p->unk20 = 0;
    p->unk1E = 0;
    p->pixelOffsetY = 0;
    p->pixelOffsetX = 0;
    p->blockBreakCooldown = 0;
    p->offsetScriptDelay = 0;
    p->offsetScriptStep = 0;
    p->sfxPlayer = 0xFFFF;
    p->sfxId = 0;
    p->ownStarSwallowCount = 0;
    p->ownStarInMouth = 0;
    p->unk36 = 0;
    p->shareTimer = 0;
    p->sharedMask = 0;
    p->shareItem = 0;
    p->unk3C = 0;
    p->boundsClamp = 0;
    p->running = 0;
    p->bumpKind = 0;
    p->invulnerability = 0;
    p->unk40 = 0;
    p->unk42 = 0;
    p->blocksBroken = 0;
    p->hitsThisFrame = 0;
    p->unk50 = 0;
    p->savedWallSide = 0;
    p->hiJumpsLeft = 1;
    p->unk50 = 0;
    p->atDoor = 0;
    p->wallSide = 0;
    p->slope = 0;
    p->onSlipperyFloor = 0;
    p->clampedTopY = -1;
    p->driftVelY = 0;
    p->driftVelX = 0;
    p->bodyBox = 0;
    p->terrainBox = 0;
    p->hitBoxSet = 0;
    p->prevTerrainBox = gPlayerDefaultTerrainBox;
    gPlayerBubbleTimers[a0] = 45;
}

/* Reset player record a0 to its start-of-stage state. */
void InitPlayerStateKeepMouth(s32 a0)
{
    struct PlayerState *p;

    p = &gPlayerStates[a0];
    p->playerIndex = a0;
    p->prevAction = PLAYER_ACTION_NONE;
    p->action = PLAYER_ACTION_NONE;
    p->requestedAction = PLAYER_ACTION_NONE;
    p->prevMode = 255;
    p->mode = -1;
    p->ability = gPlayerAbilities[a0];
    p->abilityUses = gPlayerAbilityUses[a0];
    p->flightCoastTimer = 0;
    p->runTapTimer = 0;
    p->invulnerabilityTimer = 0;
    p->unk14 = 0;
    p->invincible = 0;
    p->invincibleTimer = 0;
    p->invincibleFlashStep = 0;
    p->invincibleFlashTimer = 0;
    p->paletteFlashMode = 0;
    p->unk20 = 0;
    p->unk1E = 0;
    p->pixelOffsetY = 0;
    p->pixelOffsetX = 0;
    p->blockBreakCooldown = 0;
    p->offsetScriptDelay = 0;
    p->offsetScriptStep = 0;
    p->sfxPlayer = 0xFFFF;
    p->sfxId = 0;
    p->unk36 = 0;
    p->shareTimer = 0;
    p->sharedMask = 0;
    p->shareItem = 0;
    p->boundsClamp = 0;
    p->running = 0;
    p->bumpKind = 0;
    p->invulnerability = 0;
    p->unk40 = 0;
    p->unk42 = 0;
    p->blocksBroken = 0;
    p->hitsThisFrame = 0;
    p->unk50 = 0;
    p->savedWallSide = 0;
    p->hiJumpsLeft = 1;
    p->unk50 = 0;
    p->atDoor = 0;
    p->wallSide = 0;
    p->slope = 0;
    p->onSlipperyFloor = 0;
    p->clampedTopY = -1;
    p->driftVelY = 0;
    p->driftVelX = 0;
    p->bodyBox = 0;
    p->terrainBox = 0;
    p->hitBoxSet = 0;
    p->prevTerrainBox = gPlayerDefaultTerrainBox;
    gPlayerBubbleTimers[a0] = 45;
}

/* Set the camera/scroll target (a0 = dx, a1 = dy, a2 = limit). */
void PlayerAccelerateAxis(s32 a0, s32 a1, s32 a2)
{
    s8 sign;
    s32 ax, ay;

    gUnk_03001F2C = a0;
    gUnk_03002448 = a1;
    gUnk_03002344 = a2;
    if (a2 == 0x80000000)
        return;
    if (a1 != 0) {
        sign = (a1 >= 0);
    } else if (a0 != 0) {
        sign = (a0 >= 0);
    } else {
        if (a2 != 0)
            gUnk_03002344 = a1;
        return;
    }
    if (sign) {
        if (a0 < 0)
            return;
    } else {
        if (a0 > 0)
            return;
    }
    gUnk_03002160 = ax = (a0 < 0) ? -a0 : a0;
    gUnk_03001F10 = ay = (a1 < 0) ? -a1 : a1;
    if (ax != a2) {
        if (ax > a2) {
            if (ax - ay < a2) {
                if (sign)
                    gUnk_03001F2C = a2;
                else
                    gUnk_03001F2C = -a2;
            } else {
                if (a1 != 0) {
                    gUnk_03002448 = -a1;
                    return;
                }
                if (sign)
                    gUnk_03001F2C = a2;
                else
                    gUnk_03001F2C = -a2;
                return;
            }
        } else {
            if (ax + ay <= a2)
                return;
            if (sign)
                gUnk_03001F2C = a2;
            else
                gUnk_03001F2C = -a2;
        }
    }
    gUnk_03002448 = 0;
}

void PlayerMove(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    PlayerAccelerateAxis(t->velX, t->accelX, t->speedLimitX);
    t = gCurTask;
    t->velX = gUnk_03001F2C;
    t->speedLimitX = gUnk_03002344;
    t->velX += gUnk_03002448;
    PlayerAccelerateAxis(t->velY, t->accelY, t->speedLimitY);
    t = gCurTask;
    t->velY = gUnk_03001F2C;
    t->speedLimitY = gUnk_03002344;
    t->velY += gUnk_03002448;
    if ((t->player->unk42 & 0x100) == 0) {
        t->posX += t->player->driftVelX;
        if ((t->onGround & 1) == 0) {
            t->posY += t->player->driftVelY;
            if (t->player->driftVelY < 0)
                t->onGround = 0;
        }
    }
    u = gCurTask;
    u->posX += u->velX;
    u->posY += u->velY;
    u->pixelX = u->posX >> 16;
    u->pixelY = u->posY >> 16;
}

/* Upload the frame's graphics for gfx script entry unk3C+a0 and return its
   OAM/anim word (bit 0 is the "extended record" tag). */
s32 PlayerLoadFrameTilesAndPalette(s32 a0)
{
    struct Task *t;
    struct TaskGfx *g;
    u16 **p;
    u16 *s;
    u16 *q;
    u32 dst;
    u32 prio;
    u32 *tbl;

    t = gCurTask;
    prio = t->tileWord;
    tbl = t->frameTable;
    g = (struct TaskGfx *)tbl[t->frame + a0];
    if ((g->oamTemplate & 1) != 0) {
        p = &g->palette;
        if ((t->player->unk42 & 16) == 0 && g->palette != NULL)
            RequestCopy(2, (u32)(g->palette + 1),
                         (u32)gObjPalette + ((prio >> 12) << 5), *g->palette);
        p++;
        s = *p;
        dst = ((prio & 0x7FF) << 5) + OBJ_VRAM0;
        if (*s != 0xFFFF) {
            do {
                q = s + 1;
                RequestCopy(3, (u32)q, dst, *s);
                s = (u16 *)((u8 *)q + *s);
                dst += 0x400;
            } while (*s != 0xFFFF);
        }
        p++;
        if ((gCurTask->player->unk42 & 16) == 0 && *p != NULL)
            RequestCopy(2, (u32)(*p + 1),
                         (u32)gObjPaletteBank1 + ((prio >> 12) << 5), **p);
        q = p[1];
        if (q != NULL) {
            s = q;
            dst = ((prio & 0x7FF) << 5) + (OBJ_VRAM0 + 0x800);
            if (*s != 0xFFFF) {
                do {
                    q = s + 1;
                    RequestCopy(3, (u32)q, dst, *s);
                    s = (u16 *)((u8 *)q + *s);
                    dst += 0x400;
                } while (*s != 0xFFFF);
            }
        }
    } else {
        if ((t->player->unk42 & 16) == 0 && *g->palette != 0)
            RequestCopy(2, (u32)(g->palette + 1),
                         (u32)gObjPalette + ((prio >> 12) << 5), *g->palette);
        s = g->tiles;
        dst = ((prio & 0x7FF) << 5) + OBJ_VRAM0;
        if (*s != 0xFFFF) {
            do {
                q = s + 1;
                RequestCopy(3, (u32)q, dst, *s);
                s = (u16 *)((u8 *)q + *s);
                dst += 0x400;
            } while (*s != 0xFFFF);
        }
    }
    return g->oamTemplate & ~1;
}

void sub_0803d710(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u32 *tbl;
    u16 **p;
    u16 *w;

    t = gCurTask;
    tbl = t->frameTable;
    p = (u16 **)tbl[t->frame];
    g = (struct TaskGfx *)p;
    p++;
    w = g->palette;
    if (((u32)w & 1) != 0) {
        RequestCopy(2, (u32)(p[0] + 1),
                     (u32)gObjPalette + ((t->tileWord & 0xF000) >> 7), *p[0]);
        if (p[2] != NULL)
            RequestCopy(2, (u32)(p[2] + 1),
                         (u32)gObjPalette + 32
                             + ((gCurTask->tileWord & 0xF000) >> 7), *p[2]);
    } else {
        RequestCopy(2, (u32)(w + 1),
                     (u32)gObjPalette + ((t->tileWord & 0xF000) >> 7), *w);
    }
    if (gMetaKnightmareMode == 0) {
        if (gPlayerCount > 1)
            sub_0803d7c4();
        sub_0803db74();
    }
}

void sub_0803d7c4(void)
{
    s32 idx;

    if (gCurTask->player->playerIndex == 0)
        return;
    idx = sub_0803d870();
    if (idx == -1)
        return;
    RequestCopy(2, (idx << 1) + (u32)gPlayerPalettes
                    + (gCurTask->player->playerIndex << 5),
                 (u32)gObjPalette + ((gCurTask->tileWord >> 12) << 5), 32);
}

void sub_0803d824(void)
{
    s32 idx;

    idx = sub_0803d870();
    if (idx == -1)
        return;
    RequestCopy(2, (idx << 1) + (u32)gPlayerPalettes + (gEndingLocalPlayer << 5),
                 (u32)gObjPalette + ((gCurTask->tileWord >> 12) << 5), 32);
}

/* Pick the HUD/status graphics slot for the running task's state, or -1 for
   "nothing to upload". */
s32 sub_0803d870(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *x;
    struct Task *y;
    struct PlayerState *ps;
    struct PlayerState *q;
    u16 v;
    u16 w;

    t = gCurTask;
    v = t->frame;
    if ((v >= 273 && v <= 284) || (v >= 389 && v <= 403))
        return 192;
    if (v >= 252 && v <= 272)
        return -1;
    if (v >= 372 && v <= 388)
        return -1;
    if (v >= 4551 && v <= 4554)
        return 64;
    ps = t->player;
    if (ps->ability == ABILITY_NEEDLE || ps->ability == ABILITY_ICE) {
        if ((t->waterFlags & 1) == 0)
            return -1;
        if ((ps->unk40 & 0x100) != 0)
            return -1;
        if (ps->mouthState == 2)
            return -1;
        if (v >= 173 && v <= 242)
            return 0;
        if (v >= 285 && v <= 293)
            return 0;
        return -1;
    }
    if ((ps->unk42 & 16) == 0) {
        switch (ps->ability) {
        case ABILITY_HAMMER:
            return 320;
        case ABILITY_BALL:
            return 640;
        case ABILITY_BACKDROP:
            return 448;
        case ABILITY_THROW:
            return 512;
        case ABILITY_STONE:
            u = gCurTask;
            ps = u->player;
            if (ps->mode == 13)
                goto second;
            if ((u->waterFlags & 1) == 0)
                return -1;
            if ((ps->unk40 & 0x100) != 0)
                return -1;
            if (ps->mouthState == 2)
                return -1;
            w = u->frame;
            if (w >= 173 && w <= 242)
                return 0;
            if (w >= 285 && w <= 293)
                return 0;
            return -1;
        }
    }
    q = gCurTask->player;
    if (q->mode == 13) {
    second:
        switch (gCurTask->player->ability) {
        case ABILITY_BURNING:
            x = gCurTask;
            w = x->frame;
            if (w >= 1487 && w <= 1504)
                return -1;
            break;
        case ABILITY_TORNADO:
            x = gCurTask;
            w = x->frame;
            if (w >= 3549 && w <= 3556)
                return -1;
            break;
        case ABILITY_WHEEL:
            x = gCurTask;
            w = x->frame;
            if (w >= 1794 && w <= 1810)
                return 256;
            break;
        case ABILITY_FREEZE:
            if ((gCurTask->player->unk42 & 16) != 0)
                return -1;
            return 704;
        case ABILITY_STONE:
            x = gCurTask;
            w = x->frame;
            if (w >= 3144 && w <= 3148)
                return 384;
            return -1;
        case ABILITY_UFO:
            y = gCurTask;
            if ((y->player->unk42 & 16) != 0)
                return -1;
            w = y->frame;
            if (w >= 3976 && w <= 4068)
                return 576;
            break;
        }
    } else {
        if (q->ability == ABILITY_UFO) {
            if ((q->unk42 & 16) != 0)
                goto ret_m1;
            return 576;
        }
    }
    if ((gCurTask->player->unk42 & 16) != 0)
        goto ret_m1;
    return 0;
ret_m1:
    return -1;
}

void sub_0803db74(void)
{
    struct Task *t;
    struct Task *u3;
    struct Task *u;
    struct PlayerState *ps;
    struct PlayerState *ps2;

    t = gCurTask;
    ps = t->player;
    if ((u8)(ps->unk37 - 2) <= 1) {
        switch (ps->unk3C++) {
        case 0:
        case 1:
            u = gCurTask;
            if ((u->player->unk42 & 16) == 0)
                RequestCopy(2, (u32)gUnk_08226254,
                             (u32)gObjPalette + (((u->tileWord >> 12) + 1) << 5),
                             32);
            break;
        case 2:
            u = gCurTask;
            if ((u->player->unk42 & 16) == 0)
                BlendColors(gUnk_08226254, gUnk_08226254 - 16, 128, 16,
                             (u16 *)((u32)gObjPalette
                                     + (((u->tileWord >> 12) + 1) << 5)));
            break;
        case 3:
            u3 = gCurTask;
            if ((u3->player->unk42 & 16) == 0)
                BlendColors(gUnk_08226254, gUnk_08226254 - 16, 256, 16,
                             (u16 *)((u32)gObjPalette
                                     + (((u3->tileWord >> 12) + 1) << 5)));
            break;
        case 9:
            gCurTask->player->unk3C = 0;
            break;
        }
        return;
    }
    if ((u16)(t->frame - 655) <= 218 || (u16)(t->frame - 4369) <= 23) {
        if ((ps->unk42 & 16) == 0)
            RequestCopy(2, (u32)gUnk_081BC050,
                         (u32)gObjPalette + (((t->tileWord >> 12) + 1) << 5), 32);
        return;
    }
    if ((u16)(t->frame - 874) <= 8) {
        if ((ps->unk42 & 16) == 0)
            RequestCopy(2, (u32)&gUnk_081BE6BC[ps->playerIndex * 128],
                         (u32)gObjPalette + ((t->tileWord >> 12) << 5), 32);
    }
    u = gCurTask;
    if ((u16)(u->frame - 3821) <= 128 || (u16)(u->frame - 4525) <= 5) {
        ps2 = u->player;
        if ((ps2->unk42 & 16) == 0) {
            if (gCreditsDemoSet == 0)
                RequestCopy(2, (u32)&gUnk_080DCA28[ps2->playerIndex * 32],
                             (u32)gObjPalette + ((u->tileWord >> 12) << 5), 32);
            else
                RequestCopy(2, (u32)&gUnk_080DCA28[gEndingLocalPlayer * 32],
                             (u32)gObjPalette + ((u->tileWord >> 12) << 5), 32);
        }
    }
}

/* BLOCKED ON fwd.h: byte-exact (520/520, MATCH) as soon as fwd.h line 79
   reads `void sub_08040a44(s16 a0, s16 a1);` instead of
   `void sub_08040a44(u16 a0, u16 a1);`.  With the u16 prototype gcc folds
   `(u16)(s16)x` back to `x`, so the a44 call is fed the RAW x/y, x/y stay
   live past the sign-extends, and the sign-extended values need two extra
   callee-saved registers (26 differing bytes, size exact).  With s16 the
   `(s16)x` CSE reaches the a44 call, x/y die at the sign-extend and the
   allocator coalesces them into r4/r5 exactly as the ROM does.
   NB sub_08040a44's own body zero-extends r0/r1 (0x08040A4C
   `lsls r0,r0,#16; lsrs r5,r0,#16`), so its DEFINITION takes u16 params:
   this call site was compiled without a narrowing prototype in scope.
   Also needs `void sub_08033414(void);` in hdr.c (already present). */
void sub_0803ddc0(void)
{
    s32 speed;
    u16 x;
    u16 y;
    s32 pal;

    if (gCurTask->lateUpdateCallback != 0 && (gCurTask->skipMask & 8) == 0)
        sub_08033414();
    if ((gLocalPlayer != gCurTask->player->playerIndex
         || gInHub != 0)
     && gCurTask->player->unk37 != 2
     && IsWorldPosOnScreen(gCurTask->pixelX, gCurTask->pixelY) == 0)
    {
        if (gCurTask->frameTable == 0)
            return;
        if (gCurTask->frame == -1)
            return;
        sub_0803d710();
        return;
    }
    if (gCurTask->frameTable == 0)
        return;
    if (gCurTask->frame == -1)
        return;
    speed = 0;
    if ((gCurTask->player->unk37 == 2
      || gCurTask->player->unk37 == 3)
     && gCurTask->frame > 0xFEC && (gFrameCount & 3) == 0)
        speed = 131;
    if (gCurTask->player->unk37 == 2)
    {
        x = gCurTask->pixelX;
        y = gCurTask->pixelY;
    }
    else
    {
        x = gCurTask->pixelX - gSpriteCameraX;
        y = gCurTask->pixelY - gSpriteCameraY;
    }
    pal = PlayerLoadFrameTilesAndPalette(speed);
    if (gMetaKnightmareMode == 0)
    {
        if (gCreditsDemoSet == 0)
        {
            if (gPlayerCount > 1)
                sub_0803d7c4();
        }
        else
        {
            sub_0803d824();
        }
        sub_0803db74();
    }
    if (IsOnScreen((s16)x, (s16)y) == 0)
        return;
    sub_08040a44((s16)x, (s16)y);
    QueueSprite(gCurTask->layer, pal, gCurTask->spriteFlags,
                 2048 | gCurTask->tileWord, (s16)x, (s16)y);
    if ((gCurTask->player->unk40 & 128) == 0)
        return;
    QueueSprite(gCurTask->layer, pal, gCurTask->spriteFlags,
                 2048 | gCurTask->tileWord, (s16)x - 48, (s16)y);
    QueueSprite(gCurTask->layer, pal, gCurTask->spriteFlags,
                 2048 | gCurTask->tileWord, (s16)x + 48, (s16)y);
}

void PlayerDrawWorldLoadTilesAndPalette(void)
{
    if (gCurTask->frameTable == 0)
        return;
    if (gCurTask->frame == -1)
        return;
    if (IsWorldPosOnScreen(gCurTask->pixelX, gCurTask->pixelY) == 0)
        return;
    QueueSprite(gCurTask->layer, PlayerLoadFrameTilesAndPalette(0), gCurTask->spriteFlags,
                 gCurTask->tileWord,
                 gCurTask->pixelX - gSpriteCameraX,
                 gCurTask->pixelY - gSpriteCameraY);
}

void PlayerStopAxes(s32 axes)
{
    if (axes & 1)
    {
        gCurTask->speedLimitX = 0;
        gCurTask->accelX = 0;
        gCurTask->velX = 0;
    }
    if (axes & 2)
    {
        gCurTask->speedLimitY = 0;
        gCurTask->accelY = 0;
        gCurTask->velY = 0;
    }
}

void sub_0803e080(void)
{
    struct Task *t = gCurTask;
    struct PlayerState *p = t->player;

    switch ((s8)p->paletteFlashMode)
    {
    case 1:
        p->unk42 &= ~0x10;
        {
            struct PlayerState *q = t->player;

            if ((s16)q->invulnerabilityTimer == 0)
            {
                struct PlayerState *r;

                q->paletteFlashMode = 0;
                r = gCurTask->player;
                r->unk20 = 0;
                r->unk1E = 0;
                break;
            }
            if ((gFrameCount & 7) > 3)
                break;
            RequestCopy(2, (u32)&gUnk_080DC728[q->playerIndex * 8],
                (u32)&gObjPalette[(t->tileWord >> 12) << 5], 32);
        }
        gCurTask->player->unk42 |= 16;
        break;
    case 2:
        p->unk42 &= ~0x10;
        {
            struct PlayerState *q = t->player;
            u32 src;

            if (q->invincible != 0)
                break;
            if (q->ability != ABILITY_MIKE && q->ability != ABILITY_CRASH && q->ability != ABILITY_LIGHT)
            {
                q->paletteFlashMode = 0;
                return;
            }
            if ((gFrameCount & 15) == 9)
            {
                src = (u32)gUnk_080DCC48;
            }
            else if ((gFrameCount & 15) == 10 || (gFrameCount & 15) == 11)
            {
                src = (u32)gUnk_080DCC28;
            }
            else
            {
                break;
            }
            RequestCopy(2, src,
                (u32)&gObjPalette[(gCurTask->tileWord >> 12) << 5], 24);
        }
        gCurTask->player->unk42 |= 16;
        break;
    }
    sub_0803e8ec();
}

/* CENSUS: this is ONE function, 0x0803E1B8-0x0803E28C (212 bytes).  The
   7-entry jump table at 0x0803E1E0 runs to 0x0803E1F8, so bounds.txt's
   `sub_0803e1f4` is the table's last two words plus this body's case arms -
   0x0803E1F4 carries `rom-pointer` evidence in symbols.csv, not a prologue. */
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2)
{
    struct PlayerState *p = &gPlayerStates[a2];

    switch (a0)
    {
    default:
        sub_0803e28c((s32)p);
        p->invulnerability = 0;
        p->invulnerabilityTimer = 0;
        p->paletteFlashMode = 0;
        break;
    case 0:
        sub_0803e28c((s32)p);
        p->invulnerabilityTimer = a1;
        break;
    case 1:
        p->invulnerability = 1;
        p->invulnerabilityTimer = a1;
        p->paletteFlashMode = 1;
        break;
    case 2:
        sub_0803e28c((s32)p);
        p->invulnerability = 2;
        p->invulnerabilityTimer = 0x8000;
        p->paletteFlashMode = 0;
        break;
    case 3:
        sub_0803e28c((s32)p);
        p->invulnerability = 3;
        p->invulnerabilityTimer = 0x8000;
        p->paletteFlashMode = 0;
        break;
    case 4:
        p->invulnerability = 3;
        p->invulnerabilityTimer = a1;
        break;
    case 5:
        sub_0803e28c((s32)p);
        p->invincible = 1;
        p->invincibleTimer = 960;
        break;
    case 6:
        p->invulnerability = 6;
        p->invulnerabilityTimer = 0x8000;
        p->paletteFlashMode = 3;
        break;
    }
}

void sub_0803e28c(s32 a0)
{
    if ((s8)((struct PlayerState *)a0)->paletteFlashMode == 1)
    {
        u16 mask = 16;
        struct Task *t = gCurTask;
        struct PlayerState *p;

        if (t->player->unk42 & 0x200)
            mask |= 0x200;
        t->player->unk42 &= ~mask;
        p = t->player;
        /* task.h has no fields at PlayerState+0x1E / +0x20 yet */
        ((u16 *)p)[16] = 0;
        ((u16 *)p)[15] = 0;
    }
}

void PlayerUpdateInvulnerability(void)
{
    struct PlayerState *p = gCurTask->player;

    if ((p->unk42 & 32) == 0)
    {
        /* PlayerState+0x12 is invulnerabilityTimer, read here through an s16 view */
        if (((s16 *)p)[9] != -32768)
        {
            if (((s16 *)p)[9] != 0)
                ((s16 *)p)[9]--;
            else if (p->invulnerability != 0)
            {
                if (p->invulnerability == 1 && (p->unk42 & 0x200))
                    p->unk42 &= ~0x200;
                gCurTask->player->invulnerability = 0;
            }
        }
    }
    PlayerUpdateInvincibility();
}

s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1)
{
    u16 songArea = a1;

    if (gLocalPlayer == songArea)
        return PlaySfx(a0);
    return -1;
}

void PlayerStartSfx(s32 a0, u16 a1)
{
    if (gLocalPlayer == a1)
    {
        gCurTask->player->sfxId = a0;
        gCurTask->player->sfxPlayer = PlaySfx((s16)a0);
    }
}

void PlayerStopSfx(void)
{
    if (gCurTask->player->sfxPlayer != -1)
    {
        StopSfxOnPlayer(gCurTask->player->sfxPlayer, gCurTask->player->sfxId);
        gCurTask->player->sfxPlayer = -1;
    }
}

void FreezeOtherTasks(s32 a0)
{
    TaskFreezeOrThawOthers((u16)a0, gCurTaskIdx);
    if (a0 != 0)
    {
        TaskRestoreSkipMask(63);
        PauseRoom();
    }
    else
    {
        ResumeRoom();
    }
}

void PlayerUpdateFlip(void)
{
    if (gCurTask->facing == 1)
        gCurTask->spriteFlags &= 0x7FFF;
    else
        gCurTask->spriteFlags |= 0x8000;
}

s32 PlayerFaceHeldDirection(void)
{
    u16 *tbl = gLatchedHeldKeys;
    s32 i = (s8)gCurTask->player->playerIndex;

    if (tbl[i] & 48)
    {
        if (tbl[i] & 16)
        {
            if (gCurTask->facing == -1)
            {
                gCurTask->facing = 1;
                return 1;
            }
        }
        else
        {
            if (gCurTask->facing == 1)
            {
                gCurTask->facing = -1;
                return 1;
            }
        }
    }
    return 0;
}

void PlayerSetWaterMotionY(void)
{
    if (gCurTask->player->mouthState == 0)
    {
        if ((u32)gCurTask->velY > 0xE000)
            gCurTask->velY = 0xE000;
        PlayerSetMotionYPreset(13);
    }
    else
    {
        if ((u32)gCurTask->velY > 0x18000)
            gCurTask->velY = 0x18000;
        PlayerSetMotionYPreset(16);
    }
}

s32 PlayerLand(s32 a0)
{
    if (PlayerCheckLanding())
    {
        if ((gCurTask->waterFlags & 1) == 0)
        {
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->player->playerIndex);
            if ((gCurTask->velY & 0xFFFF0000) != 0 && a0 != 0)
                CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_IMPACT_STAR, 0);
        }
        PlayerStopAxes(2);
        return 1;
    }
    return 0;
}

s32 sub_0803e55c(void)
{
    PlayerStopAtCeilingAndWall();
    if (PlayerHasCrossedWaterSurface(0) == 0)
    {
        if (gCurTask->onGround & 1)
        {
            PlayerLand(1);
            PlayerSetMotionXPreset(0, 72);
        }
        else
        {
            PlayerSetMotionYPreset(2);
            PlayerSetMotionXPreset(11, 2);
        }
    }
    else
    {
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    }
    return gCurTask->player->requestedAction;
}

s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6)
{
    vu8 *src = (vu8 *)src6;

    if (src[0] == 128)
        return 0;
    {
        struct PlayerBodyBox *tbl = (struct PlayerBodyBox *)gPlayerBodyBoxes;
        u8 *dst = (u8 *)&tbl[playerIdx];

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
    }
    return 1;
}

/* NEEDS a byte-level view of the module's 8-byte rows in hdr.c (see report):
       struct PlayerHitBoxSet  { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
       struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
       extern struct M11Buf gPlayerHitBoxLists[];
   gPlayerHitBoxSets keeps its `struct PlayerHitBoxSet[]` spelling - only PlayerHitBoxSet's members
   change, and an 8-byte struct still copies with ldmia/stmia whatever its
   members are.  Only an ARRAY-typed extern with a SCALAR member at +4 puts the
   field offset on the SYMBOL (`adds r2,#4; adds r2,r3,r2`); a `(T *)` cast or a
   pointer local folds it into the access and costs 4 bytes.
   Byte-matched (88 bytes at 0x0803E5F8) with those lines in place. */
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1)
{
    vu8 *src = (vu8 *)a1;
    u8 *q;

    if (src[0] == 128)
        return 0;
    gPlayerHitBoxSets[a0].offsetX = src[0];
    gPlayerHitBoxSets[a0].offsetY = src[1];
    gPlayerHitBoxSets[a0].boxes = gPlayerHitBoxLists[a0].unk00;
    gPlayerHitBoxLists[a0].unk00[0] = src[2];
    gPlayerHitBoxLists[a0].unk00[1] = src[3];
    gPlayerHitBoxLists[a0].unk00[2] = src[4];
    gPlayerHitBoxLists[a0].unk00[3] = src[5];
    q = gPlayerHitBoxLists[a0].unk04;
    q[0] = 127;
    q[1] = q[2] = q[3] = 0;
    return 1;
}

void PlayerStartOffsetScript(s32 a0)
{
    gCurTask->player->offsetScript = a0;
    gCurTask->player->offsetScriptStep = 0;
    gCurTask->player->offsetScriptDelay = 1;
    gCurTask->player->unk40 |= 1;
}

void sub_0803e68c(s32 a0)
{
    struct PlayerState *p = &gPlayerStates[a0];
    struct Task *t = &gTasks[a0];
    s32 v;

    if (gLocalPlayer == p->playerIndex)
        t->layer = 6;
    else
        t->layer = 7;
    if (t->onGround != 0)
    {
        t->speedLimitX = 0;
        if (t->facing == 1)
            t->accelX = 3584;
        else
            t->accelX = -3584;
        t->speedLimitY = 0;
        t->accelY = 0;
        t->velY = 0;
    }
    else
    {
        t->speedLimitX = 0;
        if (t->facing == 1)
            t->accelX = 2048;
        else
            t->accelX = -2048;
        t->accelY = 9728;
        t->speedLimitY = 163840;
    }
    p->unk42 = 64;
    v = 0;
    if (p->mouthState == 1)
    {
        v = 14;
    }
    else if (p->mode == 14)
    {
        t->variant = 4;
        v = 9;
    }
    else if (p->mode == 10)
    {
        if ((s8)p->attachedCount != 0 && (s8)p->attachedCount == (s8)p->heldCount)
        {
            p->heldCount = p->attachedCount;
            v = 14;
        }
        else
        {
            p->heldCount = 0;
            p->attachedCount = 0;
            v = t->onGround != 0 ? 1 : 7;
        }
    }
    else if (p->mode == 13)
    {
        if (t->onGround != 0)
        {
            if (t->velX != 0)
            {
                s32 d = t->velX;

                if (d < 0)
                    d = -d;
                if ((u32)d > 0x0001CBFF)
                {
                    p->running = 1;
                    v = 3;
                }
                else
                {
                    p->running = 0;
                    v = 2;
                }
            }
            else
            {
                PlayerStopAxes(1);
                v = 1;
            }
        }
        else
        {
            v = 7;
        }
    }
    p->requestedAction = v;
}

s32 PlayerUpdateInvincibility(void)
{
    struct PlayerState *p = gCurTask->player;
    s32 i;

    if (p->unk42 & 32)
        return;
    if ((s16)p->invincibleTimer == 0)
    {
        p->invincible = 0;
        return;
    }
    p->invincibleTimer--;
    if (gPlayerCount == 1)
    {
        if ((s16)gCurTask->player->invincibleTimer == 240)
            sub_080270d0();
        return;
    }
    if ((s16)gCurTask->player->invincibleTimer != 240)
        return;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (i != gCurTask->player->playerIndex
         && gPlayerStates[i].invincible != 0
         && (s16)gPlayerStates[i].invincibleTimer > 240)
            return;
    }
    sub_080270d0();
}

void PlayerEndInvincibility(void)
{
    s32 i;
    s32 ok;

    if ((s16)gCurTask->player->invincibleTimer == 0)
        return;
    ok = 1;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (i != gCurTask->player->playerIndex
         && gPlayerStates[i].invincible != 0
         && (s16)gPlayerStates[i].invincibleTimer > 240)
            ok = 0;
    }
    if (ok != 0 && gCurrentBgm == 19)
        sub_080270d0();
    gCurTask->player->invincible = 0;
    gCurTask->player->invincibleTimer = 0;
    {
        struct PlayerState *r = gCurTask->player;

        r->invincibleFlashStep = 0;
        r->invincibleFlashTimer = 0;
    }
}

void sub_0803e8ec(void)
{
    struct PlayerState *p = gCurTask->player;
    s32 needBig;
    s32 needSmall;

    if (p->invincible == 0)
        return;
    if (p->mode == 13)
    {
        if (p->ability == ABILITY_SPARK)
            return;
        if (p->ability == ABILITY_CRASH && (p->unk42 & 16))
            return;
    }
    gCurTask->player->unk42 &= ~0x10;
    {
        struct PlayerState *q = gCurTask->player;

        if ((s16)q->invincibleTimer == 0)
        {
            q->invincibleFlashStep = 0;
            q->invincibleFlashTimer = 0;
            return;
        }
        needBig = 0;
        needSmall = 0;
        if (q->ability != ABILITY_NORMAL)
            needBig = q->mode != 13;
        if (!(q->mode == 13 && q->ability == ABILITY_SPARK))
            needSmall = 1;
    }
    {
        struct Task *t = gCurTask;
        struct PlayerState *q = t->player;

        if ((s16)q->invincibleTimer <= 239)
        {
            if (needSmall == 0)
                return;
            if ((gFrameCount & 7) > 3)
                return;
            RequestCopy(2, (u32)&gUnk_080DC728[q->playerIndex * 8],
                (u32)&gObjPalette[(t->tileWord >> 12) << 5], 24);
            gCurTask->player->unk42 |= 16;
            return;
        }
        if (needSmall != 0 && (gFrameCount & 15) == 0)
            CreatePlayerEffectHighSlot(q->playerIndex, PLAYER_EFFECT_VARIANT_IMPACT_STAR, 0);
    }
    {
        struct PlayerState *r = gCurTask->player;

        if ((s16)r->invincibleFlashTimer != 0)
        {
            r->invincibleFlashTimer--;
            gCurTask->player->unk42 |= 16;
            return;
        }
        switch ((s16)r->invincibleFlashStep)
        {
        case 0:
            sub_0803d710();
            if (needSmall)
            {
                RequestCopy(2, (u32)gUnk_080DCC48,
                    (u32)&gObjPalette[(gCurTask->tileWord >> 12) << 5], 24);
                gCurTask->player->unk42 |= 16;
            }
            if (needBig)
            {
                RequestCopy(2, (u32)gUnk_080DCC48,
                    (u32)&gObjPalette[((gCurTask->tileWord >> 12) + 1) << 5], 32);
                gCurTask->player->unk42 |= 16;
            }
            gCurTask->player->invincibleFlashTimer = 1;
            gCurTask->player->invincibleFlashStep++;
            break;
        case 1:
            sub_0803d710();
            if (needSmall)
            {
                RequestCopy(2, (u32)gUnk_080DCC28,
                    (u32)&gObjPalette[(gCurTask->tileWord >> 12) << 5], 24);
                gCurTask->player->unk42 |= 16;
            }
            gCurTask->player->invincibleFlashTimer = 2;
            gCurTask->player->invincibleFlashStep++;
            break;
        case 2:
            r->invincibleFlashTimer = 4;
            gCurTask->player->invincibleFlashStep = 0;
            break;
        }
    }
}

/* CENSUS: one function, 0x0803EAF8-0x0803F5FC (2820 bytes).  Returns the
   packed pair (hi << 16) | lo; `pop {r1}` at 0x0803F5E8 is lesson 3.94's
   not-void marker.  The body is three near-identical if/else-if chains over
   Task.frame (one per PlayerState.ability value 1/2/5) whose arms are all
   cross-jumped into one shared set of tails at 0x0803F16C-0x0803F5C2. */
s32 sub_0803eaf8(s32 a0)
{
    struct Task *t;
    u16 lo;
    u16 hi;

    lo = 0;
    hi = 0;
    switch (gPlayerStates[a0].ability)
    {
    case ABILITY_FIRE:
        t = &gTasks[a0];
        if ((u16)t->frame >= 434 && (u16)t->frame <= 453)
        {
            hi = 0;
            lo = 6;
        }
        else if ((u16)t->frame >= 486 && (u16)t->frame <= 487)
        {
            if ((t->spriteFlags & 0x8000) == 0)
                hi = -12;
            else
                hi = 12;
            lo = 4;
        }
        else if ((u16)t->frame >= 488 && (u16)t->frame <= 509)
        {
            switch (t->frame)
            {
            case 490:
            case 496:
                hi = 0;
                lo = 8;
                break;
            case 491:
            case 492:
            case 497:
            case 500:
            case 501:
                hi = 0;
                lo = 16;
                break;
            case 493:
            case 502:
            case 503:
            case 504:
            case 505:
                hi = 0;
                lo = 20;
                break;
            case 494:
            case 506:
            case 507:
            case 508:
                lo = 0x5A5A;
                hi = lo;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 510 && (u16)t->frame <= 525)
        {
            switch (t->frame)
            {
            default:
                hi = 0;
                lo = -4;
                break;
            case 522:
                hi = 0;
                lo = 6;
                break;
            }
        }
        else if ((u16)t->frame >= 526 && (u16)t->frame <= 577)
        {
            switch (t->frame)
            {
            case 536:
            case 537:
            case 538:
            case 549:
            case 550:
            case 551:
            case 562:
            case 563:
            case 564:
            case 575:
            case 576:
            case 577:
                hi = 0;
                lo = 8;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 578 && (u16)t->frame <= 582)
        {
            if ((u16)t->frame == 582)
                lo = 4;
            else
                lo = 8;
            if ((t->spriteFlags & 0x8000) == 0)
                hi = -8;
            else
                hi = 8;
        }
        else if ((u16)t->frame >= 583 && (u16)t->frame <= 585)
        {
            switch (t->frame)
            {
            default:
                hi = 0;
                break;
            case 584:
                if ((t->spriteFlags & 0x8000) == 0)
                    hi = 6;
                else
                    hi = -6;
                lo = 0;
                break;
            case 585:
                if ((t->spriteFlags & 0x8000) == 0)
                    hi = -8;
                else
                    hi = 8;
                lo = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 617 && (u16)t->frame <= 632)
        {
            switch (t->frame)
            {
            case 617:
                hi = -16;
                lo = 4;
                break;
            case 618:
                hi = -16;
                lo = 16;
                break;
            case 619:
                hi = -16;
                lo = 20;
                break;
            case 620:
                hi = -16;
                lo = 24;
                break;
            case 621:
                hi = -8;
                lo = 24;
                break;
            case 623:
                hi = 8;
                lo = 24;
                break;
            case 624:
                hi = 16;
                lo = 24;
                break;
            case 625:
            case 626:
                hi = 16;
                lo = 16;
                break;
            case 627:
                hi = 16;
                lo = 4;
                break;
            case 628:
                hi = 12;
                lo = 0;
                break;
            case 629:
                hi = 6;
                lo = -4;
                break;
            case 631:
                hi = -6;
                lo = -4;
                break;
            case 632:
                hi = -12;
                lo = 0;
                break;
            case 630:
                lo = 0;
                hi = 0;
                break;
            case 622:
                hi = 0;
                lo = 28;
                break;
            }
        }
        else if ((u16)t->frame >= 4345 && (u16)t->frame <= 4368)
        {
            if ((u16)t->frame >= 4345 && (u16)t->frame <= 4348)
            {
                hi = -8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = 8;
            }
            else if ((u16)t->frame >= 4349 && (u16)t->frame <= 4352)
            {
                hi = 8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = -8;
            }
            else if ((u16)t->frame >= 4353 && (u16)t->frame <= 4356)
            {
                hi = 0;
                lo = 8;
            }
            else if ((u16)t->frame >= 4357 && (u16)t->frame <= 4360)
            {
                hi = 12;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = lo;
            }
            else if ((u16)t->frame >= 4361 && (u16)t->frame <= 4364)
            {
                hi = -16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = 16;
            }
            else
            {
                hi = 16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = -16;
            }
        }
        break;
    case ABILITY_SPARK:
        t = &gTasks[a0];
        if ((u16)t->frame >= 675 && (u16)t->frame <= 694)
        {
            hi = 0;
            lo = 6;
        }
        else if ((u16)t->frame >= 727 && (u16)t->frame <= 728)
        {
            if ((t->spriteFlags & 0x8000) == 0)
                hi = -12;
            else
                hi = 12;
            lo = 4;
        }
        else if ((u16)t->frame >= 729 && (u16)t->frame <= 750)
        {
            switch (t->frame)
            {
            case 731:
            case 737:
                hi = 0;
                lo = 8;
                break;
            case 732:
            case 733:
            case 738:
            case 741:
            case 742:
                hi = 0;
                lo = 16;
                break;
            case 734:
            case 743:
            case 744:
            case 745:
            case 746:
                hi = 0;
                lo = 20;
                break;
            case 735:
            case 747:
            case 748:
            case 749:
                lo = 0x5A5A;
                hi = lo;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 751 && (u16)t->frame <= 766)
        {
            hi = 0;
            lo = -4;
        }
        else if ((u16)t->frame >= 767 && (u16)t->frame <= 818)
        {
            switch (t->frame)
            {
            case 777:
            case 778:
            case 779:
            case 790:
            case 791:
            case 792:
            case 803:
            case 804:
            case 805:
            case 816:
            case 817:
            case 818:
                hi = 0;
                lo = 8;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 819 && (u16)t->frame <= 823)
        {
            if ((u16)t->frame == 823)
                lo = 4;
            else
                lo = 8;
            if ((t->spriteFlags & 0x8000) == 0)
                hi = -8;
            else
                hi = 8;
        }
        else if ((u16)t->frame >= 824 && (u16)t->frame <= 826)
        {
            switch (t->frame)
            {
            default:
                hi = 0;
                break;
            case 825:
                if ((t->spriteFlags & 0x8000) == 0)
                    hi = 6;
                else
                    hi = -6;
                lo = 0;
                break;
            case 826:
                if ((t->spriteFlags & 0x8000) == 0)
                    hi = -8;
                else
                    hi = 8;
                lo = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 858 && (u16)t->frame <= 873)
        {
            switch (t->frame)
            {
            case 858:
                hi = -16;
                lo = 4;
                break;
            case 859:
                hi = -16;
                lo = 16;
                break;
            case 860:
                hi = -16;
                lo = 20;
                break;
            case 861:
                hi = -16;
                lo = 24;
                break;
            case 862:
                hi = -8;
                lo = 24;
                break;
            case 864:
                hi = 8;
                lo = 24;
                break;
            case 865:
                hi = 16;
                lo = 24;
                break;
            case 866:
            case 867:
                hi = 16;
                lo = 16;
                break;
            case 868:
                hi = 16;
                lo = 4;
                break;
            case 869:
                hi = 12;
                lo = 0;
                break;
            case 870:
                hi = 6;
                lo = -4;
                break;
            case 872:
                hi = -6;
                lo = -4;
                break;
            case 873:
                hi = -12;
                lo = 0;
                break;
            case 871:
                lo = 0;
                hi = 0;
                break;
            case 863:
                hi = 0;
                lo = 28;
                break;
            }
        }
        else if ((u16)t->frame >= 4369 && (u16)t->frame <= 4392)
        {
            if ((u16)t->frame >= 4369 && (u16)t->frame <= 4372)
            {
                hi = -8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = 8;
            }
            else if ((u16)t->frame >= 4373 && (u16)t->frame <= 4376)
            {
                hi = 8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = -8;
            }
            else if ((u16)t->frame >= 4377 && (u16)t->frame <= 4380)
            {
                hi = 0;
                lo = 8;
            }
            else if ((u16)t->frame >= 4381 && (u16)t->frame <= 4384)
            {
                hi = 12;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = lo;
            }
            else if ((u16)t->frame >= 4385 && (u16)t->frame <= 4388)
            {
                hi = -16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = 16;
            }
            else
            {
                hi = 16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = -16;
            }
        }
        else if ((u16)t->frame >= 843 && (u16)t->frame <= 853)
        {
            gCurTask->tileWord = (gCurTask->tileWord & ~15) | 12;
        }
        else
        {
            gCurTask->tileWord = (gCurTask->tileWord & ~15) | 8;
        }
        break;
    case ABILITY_BURNING:
        t = &gTasks[a0];
        if ((u16)t->frame >= 1284 && (u16)t->frame <= 1303)
        {
            hi = 0;
            lo = 6;
        }
        else if ((u16)t->frame >= 1336 && (u16)t->frame <= 1337)
        {
            if ((t->spriteFlags & 0x8000) == 0)
                hi = -12;
            else
                hi = 12;
            lo = 4;
        }
        else if ((u16)t->frame >= 1338 && (u16)t->frame <= 1359)
        {
            switch (t->frame)
            {
            case 1340:
            case 1346:
                hi = 0;
                lo = 8;
                break;
            case 1341:
            case 1342:
            case 1347:
            case 1350:
            case 1351:
                hi = 0;
                lo = 16;
                break;
            case 1343:
            case 1352:
            case 1353:
            case 1354:
            case 1355:
                hi = 0;
                lo = 20;
                break;
            case 1344:
            case 1356:
            case 1357:
            case 1358:
                lo = 0x5A5A;
                hi = lo;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 1360 && (u16)t->frame <= 1375)
        {
            hi = 0;
            lo = -4;
        }
        else if ((u16)t->frame >= 1376 && (u16)t->frame <= 1427)
        {
            switch (t->frame)
            {
            case 1386:
            case 1387:
            case 1388:
            case 1399:
            case 1400:
            case 1401:
            case 1412:
            case 1413:
            case 1414:
            case 1425:
            case 1426:
            case 1427:
                hi = 0;
                lo = 8;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 1428 && (u16)t->frame <= 1432)
        {
            if ((u16)t->frame == 1432)
                lo = 4;
            else
                lo = 8;
            if ((t->spriteFlags & 0x8000) == 0)
                hi = -8;
            else
                hi = 8;
        }
        else if ((u16)t->frame >= 1433 && (u16)t->frame <= 1435)
        {
            switch (t->frame)
            {
            default:
                hi = 0;
                break;
            case 1434:
                if ((t->spriteFlags & 0x8000) == 0)
                    hi = 6;
                else
                    hi = -6;
                lo = 0;
                break;
            case 1435:
                if ((t->spriteFlags & 0x8000) == 0)
                    hi = -8;
                else
                    hi = 8;
                lo = 0;
                break;
            }
        }
        else if ((u16)t->frame >= 1467 && (u16)t->frame <= 1482)
        {
            switch (t->frame)
            {
            case 1467:
                hi = -16;
                lo = 4;
                break;
            case 1468:
                hi = -16;
                lo = 16;
                break;
            case 1469:
                hi = -16;
                lo = 20;
                break;
            case 1470:
                hi = -16;
                lo = 24;
                break;
            case 1471:
                hi = -8;
                lo = 24;
                break;
            case 1473:
                hi = 8;
                lo = 24;
                break;
            case 1474:
                hi = 16;
                lo = 24;
                break;
            case 1475:
            case 1476:
                hi = 16;
                lo = 16;
                break;
            case 1477:
                hi = 16;
                lo = 4;
                break;
            case 1478:
                hi = 12;
                lo = 0;
                break;
            case 1479:
                hi = 6;
                lo = -4;
                break;
            case 1481:
                hi = -6;
                lo = -4;
                break;
            case 1482:
                hi = -12;
                lo = 0;
                break;
            case 1480:
                lo = 0;
                hi = 0;
                break;
            case 1472:
                hi = 0;
                lo = 28;
                break;
            }
        }
        else if ((u16)t->frame >= 4405 && (u16)t->frame <= 4428)
        {
            if ((u16)t->frame >= 4405 && (u16)t->frame <= 4408)
            {
                hi = -8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = 8;
            }
            else if ((u16)t->frame >= 4409 && (u16)t->frame <= 4412)
            {
                hi = 8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = -8;
            }
            else if ((u16)t->frame >= 4413 && (u16)t->frame <= 4416)
            {
                hi = 0;
                lo = 8;
            }
            else if ((u16)t->frame >= 4417 && (u16)t->frame <= 4420)
            {
                hi = 12;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = lo;
            }
            else if ((u16)t->frame >= 4421 && (u16)t->frame <= 4424)
            {
                hi = -16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = 16;
            }
            else
            {
                hi = 16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->spriteFlags & 0x8000))
                    hi = -16;
            }
        }
        break;
    }
    if (t->frame == -1 || ((u16)t->frame >= 4551 && (u16)t->frame <= 4554))
    {
        lo = 0x5A5A;
        hi = lo;
    }
    return (hi << 16) | lo;
}

void FreePlayerEffectsAndObjects(s8 a0)
{
    s32 i, n;

    if (a0 == 0)
        n = 4;
    else if (a0 == 1)
        n = 7;
    else if (a0 == 2)
        n = 10;
    else if (a0 == 3)
        n = 13;
    else
        return;
    for (i = n; i < n + 3; i++)
    {
        if ((s16)gTaskSlotTypes[i] != -1)
            TaskFree(i);
    }
    if (a0 == 0)
        n = 16;
    else if (a0 == 1)
        n = 20;
    else if (a0 == 2)
        n = 24;
    else if (a0 == 3)
        n = 28;
    for (i = n; i < n + 4; i++)
    {
        if ((s16)gTaskSlotTypes[i] != -1)
            TaskFree(i);
    }
    for (i = 32; i <= 62; i++)
    {
        if (gTaskSlotTypes[i] == TASK_PLAYER_EFFECT || gTaskSlotTypes[i] == TASK_PLAYER_OBJECT)
            TaskFree(i);
    }
}

void sub_0803f6e0(void)
{
    s32 sel[4];
    s32 i, k, v;
    s32 *p;

    gUnk_020055C4[0]++;
    if (gActivePlayerCount == 1)
    {
        gCurTask->unk2C = 0;
        return;
    }
    v = -1;
    p = &sel[3];
    do
    {
        *p = v;
        p--;
    } while ((s32)p >= (s32)sel);
    for (i = 0; i < gActivePlayerCount; i++)
    {
    retry:
        gUnk_03001F2C = RandomRange(gActivePlayerCount);
        for (k = 0; k < gActivePlayerCount; k++)
        {
            if (sel[k] == gUnk_03001F2C)
                goto retry;
        }
        sel[i] = gUnk_03001F2C;
    }
    for (i = 0; i < gPlayerCount; i++)
        gTasks[i].unk2C = -1;
    k = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            gTasks[i].unk2C = sel[k++];
    }
}

u16 sub_0803f7e0(u16 a0)
{
    s32 k;

    switch (gCurTask->player->ability)
    {
    default:
        k = 0;
        break;
    case ABILITY_SWORD:
        k = 1;
        break;
    case ABILITY_HAMMER:
        k = 2;
        break;
    case ABILITY_PARASOL:
        k = 3;
        break;
    case ABILITY_UFO:
        k = 4;
        break;
    }
    return gUnk_0873D79E[k * 7 + a0];
}

void sub_0803f834(u16 a0, void *src)
{
    CpuSet(src, gUnk_02007F60, 32);
    gUnk_02007F60[a0 * 2 + 16] = 0xFFFF;
    gUnk_02007F60[a0 * 2 + 17] = -1;
}

void PlayerTurnToHeldDirection(void)
{
    if (PlayerFaceHeldDirection() != 0)
        PlayerUpdateFlip();
}

s32 PlayerGetHeldDirection(void)
{
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
    {
        if (((gLatchedHeldKeys[gCurTask->player->playerIndex] & 16) && gCurTask->facing == 1)
         || ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->facing == -1))
            return 1;
        return 2;
    }
    return 0;
}

void PlayerCheckBump(void)
{
    u32 v;

    if (gCurTask->onGround & 1)
    {
        if (gCurTask->velY > 45823)
        {
            gCurTask->player->bumpKind = 2;
            return;
        }
    }
    else if (gTerrainResult[1] != 0)
    {
        if (gCurTask->velY < 0 && gCurTask->velY <= 0xFFFF4D00)
            gCurTask->player->bumpKind = 1;
        return;
    }
    if (gTerrainResult[0] != 0)
    {
        v = (gCurTask->velX < 0) ? -gCurTask->velX : gCurTask->velX;
        if (v > 45823)
        {
            if (gTerrainResult[0] == 1)
            {
                if (gCurTask->facing == 1)
                    gCurTask->player->bumpKind = 3;
                else
                    gCurTask->player->bumpKind = 4;
            }
            else if (gCurTask->facing == -1)
                gCurTask->player->bumpKind = 5;
            else
                gCurTask->player->bumpKind = 6;
        }
    }
}

void PlayerStopAtCeilingAndWall(void)
{
    struct Task *t = gCurTask;

    if (t->velY < 0 && gTerrainResult[1] != 0)
        t->velY = 0;
    PlayerStopAtWall();
}

s32 PlayerStopAtWall(void)
{
    if (gTerrainResult[0] == 0)
        return 0;
    if (((s32 *)gCurTask->player)[21] == 0
     || (gTerrainResult[0] == 1 && gCurTask->facing == 1)
     || (gTerrainResult[0] == 2 && gCurTask->facing == -1))
    {
        PlayerStopAxes(1);
        return 1;
    }
    return 0;
}

s32 PlayerCheckLanding(void)
{
    struct Task *t = gCurTask;

    if ((t->onGround & 1) && t->velY != 0)
    {
        PlayerStopAxes(2);
        return 1;
    }
    return 0;
}

s32 PlayerCheckDie(void)
{
    if (gCurTask->health != 0
     && ((gCurTask->player->unk42 & 1024)
      || (IsTaskBelowPlayerBounds(gCurTask) == 0
       && (gUnk_02005574[0] == 0 || (gCurTask->onGround & 1) == 0
           || (gCurTask->player->boundsClamp & 4) == 0)
       && (gTerrainResult[0] != 1 || (gCurTask->player->boundsClamp & 1) == 0)
       && (gTerrainResult[0] != 2 || (gCurTask->player->boundsClamp & 2) == 0))))
        return 0;
    AddPlayerHealth(-gPlayerHealth[gCurTask->player->playerIndex], gCurTask->player->playerIndex);
    gCurTask->hitKind = HIT_KIND_DEFEAT;
    gCurTask->player->requestedAction = PLAYER_ACTION_DIE;
    return gCurTask->player->requestedAction;
}

void sub_0803fb54(void)
{
    u16 v;

    if (gCurTask->waterFlags & 1)
    {
        if (gCurTask->player->unk40 & 16)
        {
            gCurTask->player->unk40 &= ~16;
            gCurTask->player->running = 0;
            gCurTask->player->runTapTimer = 0;
        }
        return;
    }
    v = gCurTask->player->unk40 & 16;
    if (v == 0)
    {
        if (gCurTask->player->running != 0)
            gCurTask->player->runTapTimer = 0;
        else if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) == 0)
        {
            if ((s8)gCurTask->player->runTapTimer <= 16)
                gCurTask->player->runTapTimer++;
        }
        else if (gCurTask->player->invincible != 0)
        {
            gCurTask->player->running = 1;
            gCurTask->player->unk40 |= 16;
            gCurTask->player->runTapTimer = 10;
        }
        else if ((s8)gCurTask->player->runTapTimer != 0
              && (s8)gCurTask->player->runTapTimer <= 16)
        {
            if (((gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
                 && gCurTask->facing == 1)
             || ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32)
                 && gCurTask->facing == -1))
            {
                gCurTask->player->running = 1;
                gCurTask->player->unk40 |= 16;
                gCurTask->player->runTapTimer = 10;
            }
        }
        else
            gCurTask->player->runTapTimer = 0;
    }
    else if (gCurTask->player->mode == 2)
    {
        gCurTask->player->unk40 &= ~16;
        gCurTask->player->runTapTimer = 0;
    }
    else if ((s8)gCurTask->player->runTapTimer != 0)
        gCurTask->player->runTapTimer--;
    else
    {
        gCurTask->player->running = 0;
        gCurTask->player->unk40 &= ~16;
        gCurTask->player->runTapTimer = 0;
    }
}

s32 PlayerHasCrossedWaterSurface(s32 a)
{
    if (gCurTask->waterFlags & 128)
    {
        if (a == 0)
        {
            if (gCurTask->waterFlags & 1)
                return 1;
        }
        else if ((gCurTask->waterFlags & 1) == 0)
            return 1;
    }
    return 0;
}

s32 PlayerGetFacingSlope(s32 a0)
{
    if (gTasks[a0].onGround == 0)
        return 0;
    if (gTasks[a0].facing == 1)
        return gUnk_0873AF30[gPlayerStates[a0].slope][0];
    return gUnk_0873AF30[gPlayerStates[a0].slope][1];
}

s32 PlayerCheckSkid(void)
{
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
    {
        if (gCurTask->facing == -1)
            gCurTask->player->requestedAction = PLAYER_ACTION_SKID;
    }
    else if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 32) && gCurTask->facing == 1)
        gCurTask->player->requestedAction = PLAYER_ACTION_SKID;
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckJump(void)
{
    if ((gCurTask->onGround & 1)
     && (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128) == 0
     && (gLatchedPressedKeys[gCurTask->player->playerIndex] & 1)
     && gCurTask->player->unk37 != 2)
    {
        if (gCurTask->player->unk37 != 3)
            gCurTask->player->requestedAction = PLAYER_ACTION_JUMP;
        else
            gCurTask->player->requestedAction = PLAYER_ACTION_STAR_ROD_JUMP;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckFallOrWater(void)
{
    struct Task *t = gCurTask;

    if ((t->onGround & 1) == 0)
        t->player->requestedAction = PLAYER_ACTION_FALL;
    else if (t->waterFlags & 1)
    {
        if (t->velX != 0)
            t->player->requestedAction = PLAYER_ACTION_WALK_IN_WATER;
        else
            t->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckDuckOrSwallow(void)
{
    if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
    {
        if (gCurTask->player->mouthState == 1 && gRoomExitKind != 1)
        {
            if (((s8 *)gCurTask->player)[11] != 0)
                gCurTask->player->requestedAction = PLAYER_ACTION_GET_ABILITY;
            else if ((gCurTask->waterFlags & 1) == 0)
                gCurTask->player->requestedAction = PLAYER_ACTION_SWALLOW;
            else
                gCurTask->player->requestedAction = PLAYER_ACTION_SWALLOW_IN_WATER;
            if (gCurTask->player->ownStarInMouth != 0)
            {
                if (gCurTask->player->ownStarSwallowCount <= 2)
                    gCurTask->player->ownStarSwallowCount++;
                gCurTask->player->ownStarInMouth = 0;
            }
            else
                gCurTask->player->ownStarSwallowCount = 0;
        }
        else if ((gCurTask->waterFlags & 1) == 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_DUCK;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckLadder(void)
{
    if (gCurTask->player->mouthState == 0 && gTerrainResult[6] != 0)
    {
        if (gTerrainResult[6] & 1)
        {
            if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 64)
                gCurTask->player->requestedAction = PLAYER_ACTION_LADDER;
        }
        else if (gLatchedPressedKeys[gCurTask->player->playerIndex] & 128)
            gCurTask->player->requestedAction = PLAYER_ACTION_LADDER;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckFloat(void)
{
    u16 v;

    if (gMetaKnightmareMode == 1)
        return 0;
    if (gCurTask->player->mouthState != 1)
    {
        v = gLatchedHeldKeys[gCurTask->player->playerIndex] & 64;
        if (v != 0)
        {
            if (gCurTask->onGround & 1)
            {
                if (++((s8 *)gCurTask->player)[16] == 9)
                {
                    ((s8 *)gCurTask->player)[16] = 0;
                    gCurTask->player->requestedAction = PLAYER_ACTION_FLOAT;
                }
            }
            else if ((gCurTask->waterFlags & 1) == 0)
            {
                ((s8 *)gCurTask->player)[16] = 0;
                gCurTask->player->requestedAction = PLAYER_ACTION_FLOAT;
            }
        }
        else
            ((s8 *)gCurTask->player)[16] = v;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckAirFloat(void)
{
    if (gCurTask->player->mouthState == 0
     && (gLatchedPressedKeys[gCurTask->player->playerIndex] & 1))
        gCurTask->player->requestedAction = PLAYER_ACTION_FLOAT;
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckBButton(void)
{
    u8 v;

    if (gMetaKnightmareMode == 1)
    {
        if ((gLatchedPressedKeys[gCurTask->player->playerIndex] & 2) == 0)
            goto out;
        if (gCurTask->onGround & 1)
        {
            if (gCurTask->player->running != 0)
                gCurTask->player->requestedAction = PLAYER_ACTION_SPIT_IN_WATER;
            else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
                gCurTask->player->requestedAction = PLAYER_ACTION_WATER_SHOT;
            else
                gCurTask->player->requestedAction = PLAYER_ACTION_SWALLOW_IN_WATER;
        }
        else
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 128)
                gCurTask->player->requestedAction = PLAYER_ACTION_GET_ABILITY;
            else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
                gCurTask->player->requestedAction = PLAYER_ACTION_WATER_SHOT;
            else
                gCurTask->player->requestedAction = PLAYER_ACTION_SWALLOW_IN_WATER;
        }
        gCurTask->player->running = 0;
        goto out;
    }
    if (gCurTask->player->ability == ABILITY_HI_JUMP
     && ((gCurTask->onGround & 1) || (gCurTask->waterFlags & 1)))
        gCurTask->player->hiJumpsLeft = 1;
    if ((gLatchedPressedKeys[gCurTask->player->playerIndex] & 2) == 0)
        goto out;
    gCurTask->player->running = 0;
    v = gAbilityBButtonActions[(gCurTask->waterFlags & 1) + gCurTask->player->ability * 2];
    if (gCurTask->player->mouthState == 1)
    {
        if ((gCurTask->waterFlags & 1) == 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_SPIT;
        else
            gCurTask->player->requestedAction = PLAYER_ACTION_SPIT_IN_WATER;
        goto out;
    }
    if (gCurTask->player->ability == ABILITY_HI_JUMP && (gCurTask->player->unk42 & 4) == 0
     && (gCurTask->waterFlags & 1) == 0)
    {
        if (gCurTask->player->hiJumpsLeft == 0)
            goto out;
        gCurTask->player->hiJumpsLeft--;
        if (gCurTask->player->mode == 5)
            gCurTask->player->mode = 4;
    }
    if ((gCurTask->player->unk42 & 4) == 0 || v == 13)
        gCurTask->player->requestedAction = v;
out:
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckEnterWater(void)
{
    if (gCurTask->velY > 0 && PlayerHasCrossedWaterSurface(0) != 0)
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckEnterDoor(void)
{
    if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 64) && gRoomExitKind == 0
     && gCurTask->player->atDoor != 0
     && (gCurTask->player->unk42 & 1) == 0
     && FindDoorAt(gCurTask->pixelX, gCurTask->pixelY) != 0)
    {
        gRoomExitKind = 1;
        gPauseDisabled = 1;
        gCurTask->player->unk42 |= 2;
        SetPlayerInvulnerability(3, 0, gCurTask->player->playerIndex);
        gCurTask->player->requestedAction = PLAYER_ACTION_ENTER_DOOR;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckDropAbility(void)
{
    if (gMetaKnightmareMode == 1)
        return 0;
    if ((gCurTask->player->unk42 & 2) == 0
     && gCurTask->player->unk37 == 0
     && (gLatchedPressedKeys[gCurTask->player->playerIndex] & 4)
     && gCurTask->player->ability != ABILITY_NORMAL)
    {
        CreateAbilityStar(gCurTask->player->ownStarSwallowCount);
        PlaySfxIfLocalPlayer(182, (u16)gCurTask->player->playerIndex);
        SetPlayerAbility(ABILITY_NORMAL, -1, gCurTask->player->playerIndex);
        gCurTask->player->requestedAction = gCurTask->player->action;
    }
    return gCurTask->player->requestedAction;
}

s32 PlayerCheckStartSwim(void)
{
    if ((gLatchedPressedKeys[gCurTask->player->playerIndex] & 65) || (gCurTask->onGround & 1) == 0)
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    return gCurTask->player->requestedAction;
}

s32 PlayerRequestLocomotion(void)
{
    if ((gCurTask->waterFlags & 1) == 0)
    {
        if ((gCurTask->onGround & 1) == 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_FALL;
        else if (gCurTask->velX == 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_STAND;
        else if (gCurTask->player->running == 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_WALK;
        else
            gCurTask->player->requestedAction = PLAYER_ACTION_RUN;
    }
    else if ((gCurTask->onGround & 1) == 0)
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    else if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 65)
        gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
    else if (gCurTask->velX == 0)
        gCurTask->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
    else
        gCurTask->player->requestedAction = PLAYER_ACTION_WALK_IN_WATER;
    return gCurTask->player->requestedAction;
}

s32 sub_080404e4(void)
{
    if (gMetaKnightmareMode == 1)
        return 0;
    if (gCurTask->player->mouthState == 2)
        return 1;
    if (gCurTask->player->mouthState == 1)
        return 2;
    return 0;
}

/* PlayerState+0x38 is a u16 counter (task.h: `u16 unk38`), read through
   a one-field struct: only a struct field gives agbcc's `lsls #16; cmp`
   test instead of `lsls #16; lsrs #16; cmp`.  Whether a plain
   `p->unk38` now matches is untested. */
struct M11Ctr { u16 unk00; };
#define CTR(p) (((struct M11Ctr *)&(p)->shareTimer)->unk00)

s32 PlayerCheckShareItem(void)
{
    struct Task *u;
    struct PlayerState *q;
    s32 i;

    if (CTR(gCurTask->player) == 0)
        return 0;
    if (--CTR(gCurTask->player) == 0)
    {
        gCurTask->player->shareItem = 0;
        gCurTask->player->sharedMask = 0;
        return 0;
    }
    if (gRoomExitKind != 0)
        return 0;
    if (gActivePlayerCount == 1)
        return 0;
    if (gCurTask->player->mode == 10 || gCurTask->player->mode == 11
     || gCurTask->player->mode == 12 || gCurTask->player->mode == 16
     || gCurTask->player->mode == 17 || gCurTask->player->mode == 18
     || gCurTask->player->mode == 19 || gCurTask->player->mode == 22
     || gCurTask->player->mode == 23 || gCurTask->player->requestedAction == PLAYER_ACTION_SHARE_ITEM
     || gCurTask->hitKind == HIT_KIND_DEFEAT || gCurTask->hitKind == HIT_KIND_DAMAGE
     || (gCurTask->player->mode == 13
         && (gCurTask->player->ability != ABILITY_UFO || gCurTask->variant <= 6)))
        return 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (i == gCurTask->player->playerIndex)
            continue;
        if ((gCurTask->player->sharedMask >> i) & 1)
            continue;
        if (gPlayerHealth[i] == 0)
            continue;
        u = &gTasks[i];
        if (u->skipMask != 0)
            continue;
        if (u->pixelX - gCurTask->pixelX >= 0)
        {
            if (u->pixelX - gCurTask->pixelX > 10)
                continue;
        }
        else if (gCurTask->pixelX - u->pixelX > 10)
            continue;
        if (u->pixelY - gCurTask->pixelY >= 0)
        {
            if (u->pixelY - gCurTask->pixelY > 5)
                continue;
        }
        else if (gCurTask->pixelY - u->pixelY > 5)
            continue;
        q = &gPlayerStates[i];
        if (q->mode == 10 || q->mode == 11 || q->mode == 12 || q->mode == 16
         || q->mode == 17 || q->mode == 18 || q->mode == 19 || q->mode == 22
         || q->mode == 23 || q->requestedAction == PLAYER_ACTION_SHARE_ITEM || u->hitKind == HIT_KIND_DEFEAT || u->hitKind == HIT_KIND_DAMAGE)
            continue;
        if (q->mode == 13)
        {
            if (q->ability != ABILITY_UFO)
                return 0;
            if (u->variant <= 6)
                continue;
        }
        gCurTask->player->requestedAction = PLAYER_ACTION_SHARE_ITEM;
        q->requestedAction = PLAYER_ACTION_SHARE_ITEM;
        gCurTask->unk18 = u->unk18 = i;
        u->taskClass++;
        u->unk1C = gCurTaskIdx;
        break;
    }
    return gCurTask->player->requestedAction;
}

void PlayerRequestStandOrFall(void)
{
    struct Task *t = gCurTask;

    if ((t->waterFlags & 1) == 0)
    {
        if (t->onGround & 1)
            t->player->requestedAction = PLAYER_ACTION_STAND;
        else
            t->player->requestedAction = PLAYER_ACTION_FALL;
    }
    else
    {
        if (!(t->onGround & 1) || (gLatchedHeldKeys[t->player->playerIndex] & 65))
            t->player->requestedAction = PLAYER_ACTION_SWIM;
        else
            t->player->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
    }
}

void LatchPlayerKeys(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        gLatchedHeldKeys[i] = gPlayerHeldKeys[i];
        gLatchedPressedKeys[i] = gPlayerPressedKeys[i];

        if (gPlayerStates[i].unk42 & 64)
        {
            gLatchedPressedKeys[i] = 0;
            gLatchedHeldKeys[i] = 0;
        }
    }
}

void sub_08040808(s32 a0)
{
    s32 t;

    if (gPlayerCount != 1 && a0 == gLocalPlayer)
    {
        t = CreatePlayerEffectHighSlot((s8)a0, 21, 0);
        gTasks[t].parent = a0;
        gTasks[t].player = gTasks[a0].player;
    }
}

void sub_08040858(s32 a0)
{
    struct PlayerState *p = gPlayerStates + a0;

    p->unk40 |= 32;
    p->unk42 |= 2;
}

void PlayerGiveInvincibleCandy(s32 a0)
{
    (gPlayerStates + a0)->unk40 |= 64;
}

void PlayerStartItemShare(s32 a0, u8 a1)
{
    struct PlayerState *p;

    if (gActivePlayerCount > 1)
    {
        p = gPlayerStates + a0;
        p->shareTimer = 300;
        p->shareItem = a1;
        p->sharedMask = 0;
    }
    else
    {
        /* `p` is genuinely uninitialized here in the ROM: the else arm stores
           through whatever register the pointer was allocated to. */
        p->shareTimer = 0;
        p->sharedMask = 0;
        p->shareItem = 0;
    }
}

s32 sub_080408e4(void)
{
    s32 i;
    s32 r = 0;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerStates[i].invincible != 0 && (s16)gPlayerStates[i].invincibleTimer > 240)
        {
            r = 1;
            break;
        }
    }

    return r;
}

void sub_08040934(s32 a0)
{
    s32 i;
    s32 b;

    b = gUnk_0873AF3A[a0][0];
    for (i = 0; i <= 2; i++)
    {
        if (gTaskSlotTypes[i + b] != -1)
            TaskFree(i + b);
    }

    b = gUnk_0873AF3A[a0][1];
    for (i = 0; i <= 3; i++)
    {
        if (gTaskSlotTypes[i + b] != -1)
            TaskFree(i + b);
    }

    sub_080409b8(a0);
}

void sub_080409b8(s32 a0)
{
    s32 n;

    switch (gPlayerStates[a0].ability)
    {
    case ABILITY_FIRE:
    case ABILITY_SPARK:
        n = CreatePlayerEffect((s8)a0, 15, 0);
        if (n != -1)
        {
            struct Task *s = &gTasks[a0];
            struct Task *d = &gTasks[n];

            d->posX = s->posX;
            d->pixelX = s->pixelX;
            d->posY = s->posY;
            d->pixelY = s->pixelY;
            d->facing = s->facing;
            d->player = s->player;
            d->parent = a0;
        }
        break;
    }
}

void sub_08040a44(s16 p0, s16 p1)
{
    u16 a0 = p0;
    u16 a1 = p1;
    struct Task *t = gCurTask;
    s32 idx;
    s32 v;

    if ((t->waterFlags & 1) == 0)
    {
        idx = gUnk_0873DB44[t->player->ability][1];
        v = t->frame - gUnk_0873DB44[t->player->ability][0];
    }
    else
    {
        idx = 0;
        switch (t->player->ability)
        {
        default:
            v = gCurTask->frame - 138;
            break;
        case ABILITY_SWORD:
            v = t->frame - 1122;
            break;
        case ABILITY_HAMMER:
            v = t->frame - 1923;
            break;
        case ABILITY_PARASOL:
            v = t->frame - 2180;
            break;
        }
    }

    if ((u32)v <= 10)
    {
        v = gUnk_0873AF42[v + idx * 11];

        if (v != 255)
            QueueSprite(6, gUnk_087519CC[v], gCurTask->spriteFlags & 0xA000, 0, (s16)a0, (s16)a1);
    }
}
