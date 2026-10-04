/* game_code_and_rodata 0x0806FF24-0x08070EC0 (issue #64, module M18 batch 8).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806FF24 0x08070EC0 pending/batch8/actor_6ff24.c --newpb
 *
 * The tail of M18: the class-1 "player intro / transformation" task family
 * that the 26-entry anchor table at 0x0873FB08 dispatches, continuing the
 * batch-7 range.  Each state is a body + a per-frame helper pair:
 *
 *   MetaKnightWarpStarRideState1 / MetaKnightWarpStarRideState1Update   state 1
 *   MetaKnightWarpStarRideState3 / MetaKnightWarpStarRideState3Update   state 3
 *   MetaKnightWarpStarRideState4 / MetaKnightWarpStarRideState4Update   state 4
 *   MetaKnightWarpStarRideState5 / MetaKnightWarpStarRideState5Update   state 5
 *   MetaKnightWarpStarRideState6 / MetaKnightWarpStarRideState6Update   state 6
 *
 * The bodies are all the same shape: set Task.updateState (the state), unk24 (the
 * sub-step the helper advances on a unk7A bit), unk0C (the draw hook) and
 * unk42/unk43, kick a sound with TaskSetFrame, then walk Task.velY (a 16.16
 * vertical speed) through a table of steps with TaskYieldTrampoline, and
 * finally wait for the helper to reach unk24 == 2.
 *
 * Also here: PlayerWarpStarRideDraw, the OAM draw routine for the family, and
 * PlayerBoardWarpStar / MetaKnightWarpStarRideInit, the entry points that re-seat the running
 * task from the player record (Task.player) and the id at 0x020055C0.
 *
 * NOTE for the coordinator - symbols.csv corrections this range needs:
 *   FALSE entries (pool-skip branches / mid-function): 0x0806FFF8,
 *   0x08070406, 0x080706A8.
 *   MISSING entries: 0x080702D8, 0x08070454.
 * See REPORT.md for the evidence.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "cutscene.h"
#include "room.h"
#include "player.h"
#include "actor.h"
#include "enemy.h"

/* RAM cells */
/* Not from main.h: this file's view of gObjPalette differs (lesson 3.517). */
extern u16 gObjPalette[];
extern s16 gSpriteCameraX;

extern s16 gSpriteCameraY;

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void RequestCopy(u32 mode, void *src, void *dst, u32 size);
extern void QueueSprite(u32 a, s32 b, u32 c, u32 d, s16 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 a);
extern void TaskSleepForever(void);
extern void TaskSetEntry(u32 fn, u32 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern s32 IsOnScreen(s16 x, s16 y);
extern void TerrainCollideBox(u8 *a);
extern void RequestScreenShake(s32 a);

void PlayerWarpStarRideState7Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->player->playerIndex == gLocalPlayer)
        SetCameraFocus(t->pixelX, t->pixelY);
    switch (gCurTask->playerRideLandCount)
    {
    case 1:
    case 2:
        PlayerStepTumble();
        break;
    case 3:
        PlayerStepHighFallPose();
        break;
    }
}

void PlayerWarpStarRideDraw(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gCurTask;
    if (t->frameTable == NULL)
        return;
    if (t->frame == -1)
        return;
    dx = t->pixelX - gSpriteCameraX;
    dy = t->pixelY - gSpriteCameraY;
    v = gTasks[t->parent].unk18;
    t->unk18 = v;
    if (v <= -2)
        return;
    if (v == -1)
        t->tileWord |= 0xC00;
    else
        t->tileWord &= 0xF3FF;
    u = gCurTask;
    if (u->unk18 > 0)
    {
        sign = (u->spriteFlags & SPRITE_FLAG_FLIP_X) ? -1 : 1;
        u->spriteFlags &= 0x7FFF;
        gfx = DrawAffineSprite(PlayerLoadFrameTilesAndPalette(0),
                           (u16)gSpriteScaleSteps[((s16 *)gCurTask)[13]] * sign,
                           gSpriteScaleSteps[((s16 *)gCurTask)[13]], 0);
        if (sign < 0)
            gCurTask->spriteFlags |= SPRITE_FLAG_FLIP_X;
    }
    else
    {
        gfx = PlayerLoadFrameTilesAndPalette(0);
    }
    if (gPlayerCount > 1)
        PlayerLoadPlayerPalette();
    sub_0803db74();
    x = dx;
    y = dy;
    if (IsOnScreen(x, y) == 0)
        return;
    QueueSprite(gCurTask->layer, gfx, gCurTask->spriteFlags,
                 gCurTask->tileWord | 0x800, x, y);
}

void PlayerSetWarpStarRideFrame(void)
{
    struct PlayerState *p;
    struct Task *t;
    s16 *tbl;
    s32 k;
    s32 pri;

    tbl = gPlayerWarpStarRideFrames[(p = gCurTask->player)->ability];
    k = p->playerIndex;
    if (k == 1 || k == 2)
        if (gPlayerCount == 4)
            k += 3;
    TaskSetFrame(tbl[k]);
    t = gCurTask;
    t->playerWarpStarRideFrame = t->frame;
    switch (gCurTask->player->playerIndex)
    {
    case 0:
        pri = 7;
        break;
    case 1:
        pri = 7;
        break;
    case 2:
        pri = 6;
        break;
    case 3:
        pri = 6;
        break;
    }
    gCurTask->layer = pri;
}

void PlayerStartRideBounce(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)sub_0803ddc0;
    t->spriteFlags &= 0x7FFF;
    if (t->playerRideIsCannon != 0)
    {
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    }
    else
    {
        switch (t->player->playerIndex)
        {
        case 0:
            TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
            break;
        case 1:
            TaskSetMotionXFacing(0x12000, 0x5A5A5A5A);
            break;
        case 2:
            TaskSetMotionXFacing(0xE000, 0x5A5A5A5A);
            break;
        case 3:
            TaskSetMotionXFacing(0x14000, 0x5A5A5A5A);
            break;
        }
    }
    TaskSetMotionY(0xFFFC0000, 0x3000, 0x40000);
    PlayerStartTumble();
}

void sub_08070208(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskCopyParentPixelPos;
    t->taskClass = 4;
    gCurTask->lateUpdateCallback = 0;
    PlayerSetWarpStarRideFrame();
}

void PlayerWaitForRideLanding(void)
{
    while (gCurTask->playerRideLandCount != 3)
        TaskYieldTrampoline(1);
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void PlayerStartHighFallPose(void)
{
    struct Task *t;
    s16 *row;

    row = gPlayerHighFallFrames[gCurTask->player->ability];
    TaskSetFrame(row[1]);
    t = gCurTask;
    t->playerTumbleFrameStep = 1;
    t->playerTumbleFrameTimer = 2;
}

void PlayerStartTumble(void)
{
    struct Task *t;

    t = gCurTask;
    t->playerTumbleFrameStep = -t->facing;
    t->playerTumbleFrameTimer = 1;
    t->frame = gPlayerTumbleFrames[t->player->ability] + 13;
    t->playerTumbleFrame = 13;
}

void PlayerStepHighFallPose(void)
{
    struct Task *t;
    u8 k;
    s32 v;

    k = gCurTask->player->ability;
    if (k == 1 || k == 2 || (s8)k == 5 || (s8)k == 15 || (s8)k == 16
        || (s8)k == 17 || (s8)k == 19 || (s8)k == 22 || (s8)k == 23)
    {
        t = gCurTask;
        if (t->playerTumbleFrameTimer <= 0)
        {
            v = t->playerTumbleFrameStep;
            t->frame += v;
            t->playerTumbleFrameTimer = 2;
            t->playerTumbleFrameStep = -v;
        }
        gCurTask->playerTumbleFrameTimer--;
    }
}

void PlayerStepTumble(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 d;
    s32 v;

    t = gCurTask;
    if (t->playerTumbleFrameTimer <= 0)
    {
        d = t->playerTumbleFrameStep;
        t->frame += d;
        v = t->playerTumbleFrame + d;
        t->playerTumbleFrame = v;
        t->playerTumbleFrameTimer = 1;
        if (v > 15)
        {
            t->frame = gPlayerTumbleFrames[t->player->ability];
            t->playerTumbleFrame = 0;
        }
        u = gCurTask;
        if (u->playerTumbleFrame < 0)
        {
            u->frame = gPlayerTumbleFrames[u->player->ability] + 15;
            u->playerTumbleFrame = 15;
        }
    }
    w = gCurTask;
    w->playerTumbleFrameTimer--;
}

void PlayerStepRideBounces(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->onGround & 1)
    {
        t->onGround = 0;
        u = gCurTask;
        u->playerRideLandCount++;
        switch (u->playerRideLandCount)
        {
        case 1:
            if (u->playerRideIsCannon == 1)
            {
                if (gCannonFuseState == -1)
                    PlaySfx(153);
                gCannonFuseState = 0;
            }
            RequestScreenShake(4);
            CreateBurstEffect(0, 0);
            PlayerStartRideBounce();
            break;
        case 2:
            TaskSetMotionY(0xFFFD0000, 0x3000, 0x40000);
            PlayerStartHighFallPose();
            break;
        }
    }
    PlayerStepRideBouncePose();
}

void PlayerStepRideBouncePose(void)
{
    switch (gCurTask->playerRideLandCount)
    {
    case 1:
        PlayerStepTumble();
        break;
    case 2:
        PlayerStepHighFallPose();
        break;
    }
}

void TaskCopyParentPixelPos(void)
{
    struct Task *t;

    t = gCurTask;
    t->pixelX = gTasks[t->parent].pixelX;
    t->pixelY = gTasks[t->parent].pixelY;
}

void PlayerBoardWarpStar(u32 a, s32 b)
{
    struct PlayerState *p;
    struct Task *e;
    struct Task *s;
    struct Task *t;
    s16 *tbl;
    s32 k;

    p = &gPlayerStates[a];
    e = &gTasks[a];
    s = &gTasks[b];
    PlayerSuspendControl(a, 1);
    e->pixelX = s->pixelX;
    e->pixelY = s->pixelY;
    e->posX = e->pixelX << 16;
    e->posY = e->pixelY << 16;
    e->taskClass = 4;
    e->parent = b;
    gWarpStarRideSlot = b;
    if (gUnk_0300244C != 0)
    {
        if (gMetaKnightmareMode == 0)
        {
            tbl = gPlayerWarpStarRideFrames[p->ability];
            k = p->playerIndex;
            if (k == 1 || k == 2)
                if (gPlayerCount == 4)
                    k += 3;
            e->frame = tbl[k];
            p->unk37 = 0;
            t = gCurTask;
            if (t->unk74 == 0)
            {
                if ((s8)gWarpStarFlightFacings[WarpStarPickFlightSlot(gCurTaskIdx)] == 1)
                    e->spriteFlags &= 0x7FFF;
                else
                    e->spriteFlags |= SPRITE_FLAG_FLIP_X;
            }
            else
            {
                if ((s8)gWarpStarFlightFacings[t->unk74] == 1)
                    e->spriteFlags &= 0x7FFF;
                else
                    e->spriteFlags |= SPRITE_FLAG_FLIP_X;
            }
        }
    }
    else
    {
        tbl = gPlayerWarpStarRideFrames[p->ability];
        k = p->playerIndex;
        if (k == 1 || k == 2)
            if (gPlayerCount == 4)
                k += 3;
        e->frame = tbl[k];
        p->unk37 = 0;
        e->spriteFlags &= 0x7FFF;
    }
    TaskSetEntry((u32)PlayerWarpStarRideInit, a);
}

void PlayerEndRideLanding(u32 a)
{
    struct PlayerState *p;

    PlayerResumeControl(a, 0, 1, 0);
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    CreateLocalPlayerArrow(a);
}

void MetaKnightWarpStarRideInit(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;

    t = gCurTask;
    t->drawCallback = (u32)MetaKnightWarpStarRideDraw;
    t->updateCallback = (u32)MetaKnightWarpStarRideUpdate;
    t->lateUpdateCallback = 0;
    t->taskClass = 4;
    TaskStop();
    u = gCurTask;
    u->spriteFlags &= 0x7FFF;
    RequestCopy(2, gUnk_0824A9E4.palette,
                 &gObjPalette[u->tileWord >> 12], gUnk_0824A9E4.paletteBankCount << 5);
    RequestCopy(3, gUnk_0824A9E4.tiles,
                 (u16 *)(OBJ_VRAM0 + ((gCurTask->tileWord & 0xFFF) << 5)),
                 gUnk_0824A9E4.tileCount << 5);
    v = gCurTask;
    v->parent = gWarpStarRideSlot;
    if (gTasks[i = v->parent].unk74 == 0)
        gCurTask->facing = gWarpStarFlightFacings[WarpStarPickFlightSlot(i)];
    else
        v->facing = gWarpStarFlightFacings[gTasks[i].unk74];
    w = gCurTask;
    w->state = META_KNIGHT_WARP_STAR_RIDE_STATE_2;
    CallTableEntry(gCurTask->state, 7, gMetaKnightWarpStarRideStates);
}

void MetaKnightWarpStarRideUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 7, gMetaKnightWarpStarRideStateUpdates);
}

void MetaKnightWarpStarRideEnterState(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    CallTableEntry(t->state, 7, gMetaKnightWarpStarRideStates);
}

void MetaKnightWarpStarRideState2(void)
{
    gCurTask->updateState = META_KNIGHT_WARP_STAR_RIDE_STATE_2;
    sub_08070ffc();
    TaskSleepForever();
}

void MetaKnightWarpStarRideState2Update(void)
{
}

void MetaKnightWarpStarRideState1(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_WARP_STAR_RIDE_STATE_1;
    t = gCurTask;
    t->playerRideLandCount = 1;
    t->playerRideIsCannon = 0;
    t->drawCallback = (u32)sub_0803ddc0;
    t->facing = 1;
    TaskSetFrame(0x11E4);
    TaskSetMotionXFacing(0x6000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskSetFrame(0x11E5);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E6);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E7);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E8);
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x8000;
    TaskSetFrame(0x11E9);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EA);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EB);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EC);
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->playerRideLandCount != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void MetaKnightWarpStarRideState1Update(void)
{
    struct Task *t;
    struct Task *u;

    TerrainCollideBox(gWarpStarRiderTerrainBox);
    t = gCurTask;
    if (t->onGround & 1)
    {
        t->onGround = 0;
        u = gCurTask;
        u->playerRideLandCount++;
        if (u->playerRideLandCount == 1)
        {
            RequestScreenShake(4);
            CreateBurstEffect(0, 0);
        }
    }
}

void MetaKnightWarpStarRideState3(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_WARP_STAR_RIDE_STATE_3;
    t = gCurTask;
    t->playerRideLandCount = 1;
    t->playerRideIsCannon = 0;
    t->drawCallback = (u32)sub_0803ddc0;
    t->layer = 7;
    TaskSetFrame(0x11E4);
    TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskSetFrame(0x11E5);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E6);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E7);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E8);
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x8000;
    TaskSetFrame(0x11E9);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EA);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EB);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11EC);
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->playerRideLandCount != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void MetaKnightWarpStarRideState3Update(void)
{
    struct Task *t;
    struct Task *u;

    TerrainCollideBox(gWarpStarRiderTerrainBox);
    t = gCurTask;
    if (t->onGround & 1)
    {
        t->onGround = 0;
        u = gCurTask;
        u->playerRideLandCount++;
        if (u->playerRideLandCount == 1)
        {
            RequestScreenShake(4);
            CreateBurstEffect(0, 0);
        }
    }
}

void MetaKnightWarpStarRideState4(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_WARP_STAR_RIDE_STATE_4;
    t = gCurTask;
    t->playerRideLandCount = 0;
    t->playerRideIsCannon = 0;
    t->drawCallback = (u32)sub_0803ddc0;
    t->layer = 7;
    gCurTask->facing = 1;
    TaskSetFrame(0x11EC);
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0xFFFF4000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFEC000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF4000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xC000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x14000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x40000;
    while (gCurTask->playerRideLandCount != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void MetaKnightWarpStarRideState4Update(void)
{
    struct Task *t;
    struct Task *u;

    TerrainCollideBox(gWarpStarRiderTerrainBox);
    t = gCurTask;
    if (t->onGround & 1)
    {
        t->onGround = 0;
        u = gCurTask;
        u->playerRideLandCount++;
        if (u->playerRideLandCount == 1)
        {
            RequestScreenShake(4);
            CreateBurstEffect(0, 0);
        }
    }
    PlayerStepRideBouncePose();
}

void MetaKnightWarpStarRideState5(void)
{
    struct Task *t;

    gCurTask->updateState = META_KNIGHT_WARP_STAR_RIDE_STATE_5;
    t = gCurTask;
    t->playerRideLandCount = 0;
    t->playerRideIsCannon = 0;
    t->drawCallback = (u32)sub_0803ddc0;
    t->layer = 7;
    gCurTask->facing = 1;
    TaskSetFrame(0x11EC);
    TaskSetMotionXFacing(0xFFFEF000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->playerRideLandCount != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void MetaKnightWarpStarRideState5Update(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    SetCameraFocus(t->pixelX, t->pixelY);
    TerrainCollideBox(gWarpStarRiderTerrainBox);
    u = gCurTask;
    if (u->onGround & 1)
    {
        u->onGround = 0;
        v = gCurTask;
        v->playerRideLandCount++;
    }
}

void MetaKnightWarpStarRideState6(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->updateState = META_KNIGHT_WARP_STAR_RIDE_STATE_6;
    t = gCurTask;
    t->playerRideLandCount = 1;
    t->playerRideIsCannon = 0;
    t->drawCallback = (u32)sub_0803ddc0;
    t->layer = 7;
    u = gCurTask;
    u->spriteFlags &= 0x7FFF;
    TaskSetFrame(0x11E4);
    TaskSetMotionXFacing(0x9000, 0x5A5A5A5A);
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->playerRideLandCount != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void MetaKnightWarpStarRideState6Update(void)
{
    struct Task *t;
    struct Task *u;

    TerrainCollideBox(gWarpStarRiderTerrainBox);
    t = gCurTask;
    if (t->onGround & 1)
    {
        t->onGround = 0;
        u = gCurTask;
        u->playerRideLandCount++;
        if (u->playerRideLandCount == 2)
        {
            RequestScreenShake(4);
            CreateBurstEffect(0, 0);
        }
    }
}
