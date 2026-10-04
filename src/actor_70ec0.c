/* game_code_and_rodata 0x08070EC0-0x08072D8C (issue #79, module M19 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08070EC0 0x08072D8C src/actor_70ec0.c --newpb
 *
 * M19 batch 1: the class-3 entries for task types #74 (Task_WarpStar) and the
 * warp-star/intro coroutines they install, plus the shared sprite-frame and
 * palette helpers the rest of the bank calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "cutscene.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "actor.h"
#include "enemy.h"

/* callees */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern s32 PlaySfx(u32 a);
extern s32 IsOnScreen(s16 a, s16 b);
extern u32 RandomRange(u32 range);
extern u32 TaskIsOnScreen(void);
extern u32 ActorCheckHits(void);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *a, u32 i);
extern void TerrainCollideBox(u32 *p);
extern void RequestScreenShake(u32 a);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);

void sub_08070ec0(void)
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
        sign = (u->spriteFlags & 0x8000) ? -1 : 1;
        u->spriteFlags &= 0x7FFF;
        gfx = DrawAffineSprite((s32)gUnk_0824A9CC,
                           (u16)gUnk_0873FF98[u->unk18 >> 16] * sign,
                           gUnk_0873FF98[u->unk18 >> 16], 0);
        if (sign < 0)
            gCurTask->spriteFlags |= 0x8000;
    }
    else
    {
        gfx = (s32)gUnk_0824A9CC;
    }
    x = dx;
    y = dy;
    if (IsOnScreen(x, y) == 0)
        return;
    QueueSprite(gCurTask->layer, gfx, gCurTask->spriteFlags,
                 gCurTask->tileWord | 0x800, x, y);
}

void sub_08070ffc(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)TaskCopyParentPixelPos;
        t->taskClass = 4;
    }
    {
        struct Task *t = gCurTask;

        t->lateUpdateCallback = 0;
        t->posY = 0;
        t->posX = 0;
        t->layer = 7;
    }
    TaskSetFrame(-1);
}

void Task_WarpStar(void)
{
    {
        struct Task *t = gCurTask;

        if (t->actorSpawnArg != 23)
            t->moveCallback = (u32)ActorMove;
        else
            t->moveCallback = (u32)TaskMoveRelativeToView;
    }
    {
        struct Task *t = gCurTask;

        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gWarpStarFrames;
        t->updateCallback = (u32)WarpStarUpdate;
    }
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 3, gWarpStarStates);
}

void WarpStarUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gWarpStarStateUpdates);
    if (gBoardedWarpStarSlot == -1 || gBoardedWarpStarSlot == gCurTaskIdx)
    {
        if (ActorCheckHits())
            WarpStarBoard();
    }
}

void WarpStarEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gWarpStarStates);
}

void WarpStarAnimateTiles(void)
{
    u32 off;

    {
        struct Task *t = gCurTask;

        if (t->frameTable != gWarpStarFrames)
            return;
        t->warpStarTilePhase++;
        if (t->warpStarTilePhase > 5)
            t->warpStarTilePhase = 0;
    }
    {
        struct Task *t = gCurTask;

        u32 dst;

        t->frame = 13;
        off = (t->tileWord & 0x7FF) << 5;
        dst = OBJ_VRAM0 + off;
        RequestCopy(1, gUnk_0873FB7C[t->warpStarTilePhase], dst, 128);
    }
    RequestCopy(1, gUnk_0873FB7C[gCurTask->warpStarTilePhase] + 128, (OBJ_VRAM0 + 0x400) + off, 128);
    RequestCopy(1, gUnk_0873FB7C[gCurTask->warpStarTilePhase] + 256, (OBJ_VRAM0 + 0x800) + off, 128);
    RequestCopy(1, gUnk_0873FB7C[gCurTask->warpStarTilePhase] + 384, (OBJ_VRAM0 + 0xC00) + off, 128);
}

void WarpStarBoard(void)
{
    if (gUnk_0300244C != 0 && gPlayerHealth[gCurTask->hitterSlot] <= 0)
        return;
    if (gUnk_020061E0 == 0)
    {
        struct Task *t = gCurTask;
        u16 f;

        t->frame = 13;
        if (t->actorSpawnArg != 23)
        {
            t->pixelY -= 16;
            t->posX = t->pixelX << 16;
            t->posY = t->pixelY << 16;
        }
        else
        {
            t->pixelY -= 8;
            t->posX = (t->pixelX - gViewRect[0]) << 16;
            t->posY = (t->pixelY - gViewRect[2]) << 16;
        }
        f = WarpStarCopyTilesToRider(gCurTask->hitterSlot);
        {
            struct Task *t2 = gCurTask;

            t2->tileWord = f;
            gBoardedWarpStarSlot = gCurTaskIdx;
            gRoomExitKind = 2;
            if (t2->warpStarSparkleSlot != -1)
            {
                TaskFree(t2->warpStarSparkleSlot);
                gCurTask->warpStarSparkleSlot = 0xFFFF;
            }
        }
        ActorSetAttackBox((u32)gUnk_0873F554);
    }
    gPlayerStates[gCurTask->hitterSlot].mode = 16;
    sub_08040934(gCurTask->hitterSlot);
    gCurTask->warpStarRiderCount++;
    PlayerBoardWarpStar(gCurTask->hitterSlot, gCurTaskIdx);
    CreateBurstEffect(0, 0);
    if (gLocalPlayer == gCurTask->hitterSlot)
        PlaySfx(219);
    if (gCurLevel == 7)
        gUnk_02007D00[9] = 1;
    ActorSetState(1);
    TaskSetEntry(WarpStarEnterState, gCurTaskIdx);
}

u16 WarpStarCopyTilesToRider(s32 idx)
{
    u16 v = gTasks[idx].tileWord & 0x7FF;
    u32 off = v << 5;

    RequestCopy(1, (u32)gUnk_0825D2C8, (OBJ_VRAM0 + 0x180) + off, 128);
    RequestCopy(1, (u32)gUnk_0825D2C8 + 128, (OBJ_VRAM0 + 0x580) + off, 128);
    RequestCopy(1, (u32)gUnk_0825D2C8 + 256, (OBJ_VRAM0 + 0x980) + off, 128);
    RequestCopy(1, (u32)gUnk_0825D2C8 + 384, (OBJ_VRAM0 + 0xD80) + off, 128);
    gUnk_020061E0++;
    return v + 0xF00C;
}

void CreateWarpStar(int x, int y, int c)
{
    CreateActorByKind(ACTOR_KIND_OBJECT, 0, 0, c, x, y, 0);
}

void WarpStarState0(void)
{
    gCurTask->updateState = 0;
    {
        struct Task *t = gCurTask;

        t->warpStarRiderCount = 0;
        t->warpStarBobTimer = 16;
        t->warpStarBobPhase = 0;
    }
    gCurTask->warpStarSparkleSlot = CreateChildTaskHere(TASK_WARP_STAR_SPARKLE, 0);
    while (1)
    {
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(3);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(3);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(3);
    }
}

void WarpStarState0Update(void)
{
    if (gRoomExitKind != 0 && gActivePlayerCount != 1)
    {
        struct Task *t = gCurTask;

        if (t->warpStarSparkleSlot != -1)
        {
            TaskFree(t->warpStarSparkleSlot);
            gCurTask->warpStarSparkleSlot = 0xFFFF;
        }
        ActorSetState(2);
        TaskSetEntry(WarpStarEnterState, gCurTaskIdx);
    }
    else
    {
        {
            struct Task *t = gCurTask;

            if (t->warpStarBobTimer <= 0)
            {
                t->velY = gUnk_0873FB94[t->warpStarBobPhase];
                t->warpStarBobPhase++;
                if (t->warpStarBobPhase > 5)
                    t->warpStarBobPhase = 0;
                gCurTask->warpStarBobTimer = 16;
            }
        }
        gCurTask->warpStarBobTimer--;
    }
}

void WarpStarState1(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    while (1)
    {
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(18);
        gCurTask->velY = -0x4000;
        TaskYieldTrampoline(18);
    }
}

void WarpStarState1Update(void)
{
    WarpStarAnimateTiles();
    if (gCurTask->warpStarRiderCount == gActivePlayerCount && AreInactivePlayerCamerasParked())
    {
        struct Task *t = gCurTask;
        u32 v = t->actorSpawnArg;

        if (v == 0)
        {
            v = sub_08025e88(gCurTaskIdx);
            t = gCurTask;
        }
        t->state = v;
        TaskSetEntry(WarpStarStartFlight, gCurTaskIdx);
    }
}

void WarpStarVanish(void)
{
    gCurTask->updateCallback = 0;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->frameTable = gUnk_08752D8C;
        t->frame = 0;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(2);
    gCurTask->frame = 6;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    ActorDestroy();
}

void sub_08071774(void)
{
}

void WarpStarStartFlight(void)
{
    struct Task *t;

    {
        struct Task *u = gCurTask;

        u->drawCallback = (u32)WarpStarDrawFlight;
        u->layer = 10;
    }
    t = gCurTask;
    t->frameTable = gWarpStarFrames;
    t->updateCallback = (u32)WarpStarFlightUpdate;
    t->warpStarScale = 0;
    t->warpStarScaleSpeed = 0;
    t->health = 0xFFFF;
    t->warpStarTrailTimer = 0;
    if (t->actorSpawnArg == 0)
        gCurTask->facing = gUnk_0873FAE8[sub_08025e88(gCurTaskIdx)];
    else
        t->facing = gUnk_0873FAE8[t->actorSpawnArg];
    CreateChildTaskAt(TASK_WARP_STAR_CAMERA, gViewRect[0] + 120, gViewRect[2] + 80, 0);
    CallTableEntry(gCurTask->state, 26, gWarpStarFlights);
}

void WarpStarFlightUpdate(void)
{
    WarpStarAnimateTiles();
    CallTableEntry(gCurTask->updateState, 26, gWarpStarFlightUpdates);
}

void WarpStarFlightEnterState(void)
{
    CallTableEntry(gCurTask->state, 26, gWarpStarFlights);
}

void WarpStarSetTrail(int a, int b, int c, int d)
{
    {
        struct Task *t = gCurTask;

        t->health = (s8)a;
        t->warpStarTrailDir = b;
    }
    gCurTask->hitTimer = c;
    gCurTask->warpStarTrailSpeed = (s16)d;
}

void WarpStarStopTrail(void)
{
    {
        struct Task *t = gCurTask;

        t->health = 0xFFFF;
        t->warpStarTrailDir = 0;
    }
    gCurTask->hitTimer = 0;
    gCurTask->warpStarTrailSpeed = 0;
}

void WarpStarEmitTrailStars(void)
{
    struct Task *t = gCurTask;
    u32 v;
    s32 r;

    if (t->health == -1)
        return;
    t->warpStarTrailTimer--;
    if ((s16)t->warpStarTrailTimer > 0)
        return;
    if ((s8)t->hitTimer == -1)
    {
        if (t->health == 2 || t->health == 0)
        {
            v = (u8)(u16)t->health;
        }
        else
        {
            r = RandomRange(3);
            v = 1;
            if (r != 0)
                v = 0;
        }
        CreateWarpStarTrailStar(gCurTask->warpStarTrailDir << 5, (s16)gCurTask->warpStarTrailSpeed, v);
        gCurTask->health = 0xFFFF;
    }
    else
    {
        gUnk_030023D4 = RandomRange(32) - 16;
        gUnk_030023D4 += gCurTask->warpStarTrailDir << 5;
        if (gUnk_030023D4 < 0)
            gUnk_030023D4 += 512;
        {
            u16 a1 = gUnk_030023D4;
            s32 a2 = (s16)gCurTask->warpStarTrailSpeed;

            v = RandomRange(3);
            /* r (the other branch's RNG result) carries the byte, so regmove keeps the AND on v */
            r = (u8)gCurTask->health;
            CreateWarpStarTrailStar(a1, a2, v & r);
        }
        gCurTask->warpStarTrailTimer = (s8)gCurTask->hitTimer;
    }
}

void WarpStarDrawFlight(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 v;
    s32 gfx;
    s8 sign;
    u32 *g;

    v = t->warpStarScale;
    if (v <= -2)
    {
        if (gMetaKnightmareMode != 0)
            return;
        g = t->frameTable;
        QueueSprite(t->layer, g[t->frame], t->spriteFlags, t->tileWord,
                     t->pixelX - gSpriteCameraX, t->pixelY - gSpriteCameraY);
        return;
    }
    if (v == -1)
        t->tileWord |= 0xC00;
    else
        t->tileWord &= 0xF3FF;
    {
        struct Task *w = gCurTask;

        if (w->warpStarScale > 0)
        {
            w->warpStarScale += w->warpStarScaleSpeed;
            if (w->warpStarScale < 0)
                w->warpStarScale = 0;
            if (gCurTask->warpStarScale > 0x3F0000)
                gCurTask->warpStarScale = 0x3F0000;
        }
    }
    if (gMetaKnightmareMode != 0)
        return;
    if (gCurTask->frameTable == NULL)
        return;
    if (gCurTask->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    u = gCurTask;
    g = u->frameTable;
    if (u->warpStarScale > 0)
    {
        sign = (u->spriteFlags & 0x8000) ? -1 : 1;
        u->spriteFlags &= 0x7FFF;
        gfx = DrawAffineSprite(g[u->frame],
                           (u16)gUnk_0873FF98[u->warpStarScale >> 16] * sign,
                           gUnk_0873FF98[u->warpStarScale >> 16], 0);
        {
            struct Task *x = gCurTask;

            QueueSprite(x->layer, gfx, x->spriteFlags, x->tileWord,
                         x->pixelX - gSpriteCameraX, x->pixelY - gSpriteCameraY);
        }
        if (sign < 0)
            gCurTask->spriteFlags |= 0x8000;
    }
    else
    {
        QueueSprite(u->layer, g[u->frame], u->spriteFlags, u->tileWord,
                     u->pixelX - gSpriteCameraX, u->pixelY - gSpriteCameraY);
    }
}

void WarpStarSetRiderState(u16 a)
{
    s32 i;

    if (gMetaKnightmareMode != 0)
        while (1)
            ;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct Task *t = &gTasks[i];

            t->facing = gCurTask->facing;
            t->posX = t->pixelX << 16;
            t->posY = t->pixelY << 16;
            t->state = a;
            TaskSetEntry(PlayerWarpStarRideEnterState, i);
        }
    }
}

void WarpStarSetMetaKnightRiderState(u16 a)
{
    s32 i;

    if (gMetaKnightmareMode != 1)
        while (1)
            ;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct Task *t = &gTasks[i];

            t->facing = gCurTask->facing;
            t->posX = t->pixelX << 16;
            t->posY = t->pixelY << 16;
            t->state = a;
            TaskSetEntry(MetaKnightWarpStarRideEnterState, i);
        }
    }
}

void CreateFlyingWarpStar(int x, int y, int c)
{
    s32 id = CreateActorByKind(ACTOR_KIND_OBJECT, 0, 0, c, x >> 16, y >> 16, 0);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];

        t->state = c;
        TaskSetEntry(WarpStarStartFlight, id);
        t->tileWord = WarpStarCopyTilesToRider(gCurTaskIdx);
    }
    gWarpStarRideSlot = id;
    DisablePause();
}

void sub_08071d2c(void)
{
    PlaySfx(219);
    TaskSetMotionY(0x30000, -0x4000, 0x30000);
    TaskYieldTrampoline(4);
    PlaySfx(216);
    gCurTask->unk24 = 0;
}

void sub_08071d60(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    {
        t->posX = (t->pixelX - gViewRect[0]) << 16;
        t->posY = (t->pixelY - gViewRect[2]) << 16;
        t->moveCallback = (u32)TaskMoveRelativeToView;
        t->warpStarExitRequested = 0;
    }
    {
        s32 r;

        gWarpStarFlightSfx = 216;
        r = PlaySfx(216);
        gWarpStarFlightSfxPlayer = r;
    }
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = Div(0x780000 - gCurTask->posY, 32);
    TaskYieldTrampoline(32);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    CreateChildTaskAt(TASK_STAR_SCATTER, gCurTask->pixelX, gCurTask->pixelY + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(32);
    WarpStarSetTrail(1, 4, 4, 0x400);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x14000;
    TaskYieldTrampoline(8);
    gCurTask->velY = -0x28000;
    TaskYieldTrampoline(40);
    ExitOnWarpStar();
    gCurTask->warpStarExitRequested = 1;
    TaskSleepForever();
}

void sub_08071e74(void)
{
    WarpStarEmitTrailStars();
}

void sub_08071e80(void)
{
    gCurTask->updateState = 1;
    gCurTask->moveCallback = (u32)TaskMove;
    TaskStop();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->velY = 0x20000;
    TaskSleepForever();
}

void sub_08071ebc(void)
{
    struct Task *t;

    WarpStarEmitTrailStars();
    TerrainCollideBox(gUnk_0873F5CC);
    t = gCurTask;
    if (t->onGround & 1)
    {
        CameraStartFollowFocusAt(t->pixelX, t->pixelY);
        RequestScreenShake(4);
        StopSfxOnPlayer(gWarpStarFlightSfxPlayer, gWarpStarFlightSfx);
        PlaySfx(219);
        CreateBurstEffect(0, 0);
        PlaySfx(272);
        if (gMetaKnightmareMode == 0)
            WarpStarSetRiderState(3);
        else
            WarpStarSetMetaKnightRiderState(3);
        gUnk_020061E0 = 0;
        ActorDestroy();
    }
}

void WarpStarFlight5(void)
{
    struct Task *t;

    gCurTask->updateState = 5;
    t = gCurTask;
    t->posX = (t->pixelX - gViewRect[0]) << 16;
    t->posY = (t->pixelY - gViewRect[2]) << 16;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->warpStarExitRequested = 0;
    TaskStop();
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    {
        s32 r;

        gWarpStarFlightSfx = 126;
        r = PlaySfx(126);
        gWarpStarFlightSfxPlayer = r;
    }
    gCurTask->velX = -0x8000;
    WarpStarSetTrail(1, 4, 4, 0x600);
    gCurTask->velY = -0x80000;
    TaskYieldTrampoline(6);
    WarpStarSetTrail(1, 5, 4, 0x600);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(6);
    WarpStarSetTrail(1, 6, 4, 0x600);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(6);
    WarpStarSetTrail(1, 7, 6, 0x600);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(6);
    WarpStarSetTrail(-1, 0, 0, 0);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(6);
    PlaySfx(272);
    WarpStarSetTrail(1, 4, 4, 0x600);
    {
        struct Task *u = gCurTask;

        u->velX = -0x20000;
        u->velY = -0x80000;
    }
    TaskYieldTrampoline(12);
    TaskStop();
    ExitOnWarpStar();
    gCurTask->warpStarExitRequested = 1;
    TaskSleepForever();
}

void WarpStarFlight5Update(void)
{
    WarpStarEmitTrailStars();
}

void WarpStarFlight6(void)
{
    struct Task *t;

    gCurTask->updateState = 6;
    t = gCurTask;
    t->posX = (t->pixelX - gViewRect[0]) << 16;
    t->posY = (t->pixelY - gViewRect[2]) << 16;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->warpStarExitRequested = 0;
    WarpStarSetTrail(1, 4, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->velX = -0x40000;
        u->velY = -0x80000;
    }
    TaskYieldTrampoline(4);
    WarpStarSetTrail(1, 2, 4, 0x300);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(3);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(3);
    WarpStarSetTrail(-1, 0, 0, 0);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(4);
    PlaySfx(272);
    gCurTask->velX = 0;
    TaskYieldTrampoline(2);
    WarpStarSetTrail(1, 7, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->velX = 0x10000;
        u->velY = -0x80000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(3);
    {
        struct Task *u = gCurTask;

        u->velX = 0x40000;
        u->velY = -0x60000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    WarpStarSetTrail(-1, 0, 0, 0);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(8);
    PlaySfx(272);
    {
        struct Task *u = gCurTask;

        u->velX = 0;
        u->velY = -0x60000;
    }
    TaskYieldTrampoline(2);
    WarpStarSetTrail(1, 1, 4, 0x300);
    gCurTask->velX = -0x60000;
    TaskYieldTrampoline(3);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(3);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0x40000;
    TaskYieldTrampoline(3);
    WarpStarSetTrail(-1, 0, 0, 0);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 0x80000;
    TaskYieldTrampoline(3);
    PlaySfx(272);
    TaskStop();
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(2);
    WarpStarSetTrail(1, 7, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->velX = 0x40000;
        u->velY = -0x20000;
    }
    TaskYieldTrampoline(14);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(10);
    WarpStarSetTrail(1, 6, 8, 0x300);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    WarpStarSetTrail(1, 6, 10, 0x300);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    ExitOnWarpStar();
    gCurTask->warpStarExitRequested = 1;
    TaskSleepForever();
}

void WarpStarFlight6Update(void)
{
    WarpStarEmitTrailStars();
}

void WarpStarFlight8(void)
{
    struct Task *t;

    gCurTask->updateState = 8;
    t = gCurTask;
    t->posX = (t->pixelX - gViewRect[0]) << 16;
    t->posY = (t->pixelY - gViewRect[2]) << 16;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->warpStarExitRequested = 0;
    {
        s32 r;

        gWarpStarFlightSfx = 126;
        r = PlaySfx(126);
        gWarpStarFlightSfxPlayer = r;
    }
    gCurTask->warpStarScale = 0x3F0000;
    WarpStarStopTrail();
    TaskStop();
    {
        struct Task *u = gCurTask;

        u->velX = 0x60000;
        u->velY = -0x30000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = 0x20000;
        u->velY = -0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = 0x10000;
        u->velY = -0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x10000;
        u->velY = -0x8000;
    }
    TaskYieldTrampoline(6);
    gCurTask->velX = -0x20000;
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x40000;
        u->velY = 0x2000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x60000;
        u->velY = 0x8000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x40000;
        u->velY = 0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x20000;
        u->velY = 0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x10000;
        u->velY = 0x30000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = 0x8000;
        u->velY = 0x20000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = 0x10000;
        u->velY = 0x10000;
    }
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = 0x20000;
        u->velY = 0x8000;
    }
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x20000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0x10000;
    TaskYieldTrampoline(12);
    CreateChildTaskAt(TASK_STAR_SCATTER, gCurTask->pixelX, gCurTask->pixelY + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    gCurTask->warpStarScaleSpeed = -0x2600;
    WarpStarSetTrail(1, 4, 3, 0x400);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x2000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velX = -0x10000;
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->velX = -0x8000;
        u->velY = 0x8000;
    }
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(10);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(10);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(16);
    WarpStarSetTrail(-1, 0, 0, 0);
    TaskStop();
    gCurTask->velX = 0xC000;
    TaskYieldTrampoline(48);
    {
        struct Task *u = gCurTask;

        u->warpStarScale = 0x100000;
        u->warpStarScaleSpeed = -0xC00;
    }
    StopSfxOnPlayer(gWarpStarFlightSfxPlayer, gWarpStarFlightSfx);
    {
        s32 r;

        gWarpStarFlightSfx = 217;
        r = PlaySfx(217);
        gWarpStarFlightSfxPlayer = r;
    }
    TaskStop();
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(40);
    gCurTask->velY = 0x6000;
    TaskYieldTrampoline(40);
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(40);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(40);
    gCurTask->velY = 0x1000;
    TaskYieldTrampoline(40);
    ExitOnWarpStar();
    gCurTask->warpStarExitRequested = 1;
    TaskSleepForever();
}

void WarpStarFlight8Update(void)
{
    WarpStarEmitTrailStars();
}

void WarpStarFlight10(void)
{
    struct Task *t;

    gCurTask->updateState = 10;
    t = gCurTask;
    t->posX = (t->pixelX - gViewRect[0]) << 16;
    t->posY = (t->pixelY - gViewRect[2]) << 16;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->warpStarExitRequested = 0;
    {
        s32 r;

        gWarpStarFlightSfx = 250;
        r = PlaySfx(250);
        gWarpStarFlightSfxPlayer = r;
    }
    WarpStarStopTrail();
    TaskStop();
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(6);
    {
        struct Task *u = gCurTask;

        u->velX = -0x8000;
        u->velY = -0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x10000;
        u->velY = -0x10000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x30000;
        u->velY = -0x40000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velX = -0x20000;
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->velX = -0x10000;
        u->velY = 0x10000;
    }
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->velX = -0x8000;
        u->velY = 0x8000;
    }
    TaskYieldTrampoline(8);
    WarpStarSetTrail(1, 6, 4, 0x300);
    gCurTask->warpStarLoopCount = 0;
    do
    {
        {
            struct Task *u = gCurTask;

            u->velX = 0x20000;
            u->velY = -0x20000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->velX = -0x20000;
            u->velY = 0x20000;
        }
        TaskYieldTrampoline(2);
        gCurTask->warpStarLoopCount++;
    } while ((s16)gCurTask->warpStarLoopCount <= 7);
    CreateChildTaskAt(TASK_STAR_SCATTER, gCurTask->pixelX, gCurTask->pixelY + 16, 0);
    RequestScreenShake(2);
    PlaySfx(272);
    {
        struct Task *u = gCurTask;

        u->velX = 0x20000;
        u->velY = -0x20000;
    }
    TaskYieldTrampoline(2);
    {
        struct Task *u = gCurTask;

        u->velX = -0x20000;
        u->velY = 0x20000;
    }
    TaskYieldTrampoline(2);
    {
        struct Task *u = gCurTask;

        u->velX = 0x80000;
        u->velY = -0x60000;
    }
    TaskYieldTrampoline(12);
    gCurTask->velX = 0x60000;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velX = 0x30000;
    TaskYieldTrampoline(4);
    TaskStop();
    ExitOnWarpStar();
    gCurTask->warpStarExitRequested = 1;
    TaskSleepForever();
}

void WarpStarFlight10Update(void)
{
    WarpStarEmitTrailStars();
}

void WarpStarFlight11(void)
{
    struct Task *t;

    gCurTask->updateState = 11;
    t = gCurTask;
    t->posX = (t->pixelX - gViewRect[0]) << 16;
    t->posY = (t->pixelY - gViewRect[2]) << 16;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->warpStarExitRequested = 0;
    WarpStarStopTrail();
    TaskStop();
    gCurTask->velX = 0x80000;
    TaskYieldTrampoline(1);
    gCurTask->warpStarLoopCount = 0;
    while (1)
    {
        WarpStarSetTrail(1, 7, 4, 0x300);
        {
            struct Task *u = gCurTask;

            u->velX = 0x80000;
            u->velY = -0x20000;
        }
        TaskYieldTrampoline(12);
        gCurTask->velX = 0x60000;
        TaskYieldTrampoline(6);
        gCurTask->velX = 0x40000;
        TaskYieldTrampoline(6);
        WarpStarSetTrail(1, 5, -1, 0x300);
        gCurTask->velX = 0x20000;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(4);
        gCurTask->warpStarScale = -1;
        WarpStarSetTrail(1, 1, 4, 0x300);
        {
            struct Task *u = gCurTask;

            u->velX = -0x10000;
            u->velY = -0x14000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gCurTask;

            u->velX = -0x20000;
            u->velY = -0x10000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gCurTask;

            u->velX = -0x40000;
            u->velY = -0x8000;
        }
        TaskYieldTrampoline(4);
        {
            struct Task *u = gCurTask;

            u->velX = -0x80000;
            u->velY = 0x30000;
        }
        TaskYieldTrampoline(18);
        {
            struct Task *u = gCurTask;

            u->velX = -0x40000;
            u->velY = 0x10000;
        }
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->warpStarScale = 0;
        WarpStarSetTrail(1, 7, 4, 0x300);
        {
            struct Task *u = gCurTask;

            u->velX = 0x20000;
            u->velY = -0x8000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->velX = 0x40000;
            u->velY = -0x10000;
        }
        TaskYieldTrampoline(4);
        gCurTask->warpStarLoopCount++;
        if ((s16)gCurTask->warpStarLoopCount > 2)
            break;
    }
    {
        struct Task *u = gCurTask;

        u->velX = 0x80000;
        u->velY = -0x20000;
    }
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(4);
    WarpStarSetTrail(1, 5, -1, 0x300);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(4);
    WarpStarSetTrail(1, 4, -1, 0x300);
    gCurTask->velY = -0x80000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(4);
    ExitOnWarpStar();
    gCurTask->warpStarExitRequested = 1;
    TaskSleepForever();
}

void WarpStarFlight11Update(void)
{
    WarpStarEmitTrailStars();
}

void WarpStarFlight12(void)
{
    struct Task *t;

    gCurTask->updateState = 12;
    t = gCurTask;
    t->posX = (t->pixelX - gViewRect[0]) << 16;
    t->posY = (t->pixelY - gViewRect[2]) << 16;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    {
        s32 r;

        gWarpStarFlightSfx = 218;
        r = PlaySfx(218);
        gWarpStarFlightSfxPlayer = r;
    }
    TaskStop();
    WarpStarSetTrail(1, 7, 4, 0x300);
    {
        struct Task *u = gCurTask;

        u->velX = 0x60000;
        u->velY = -0x30000;
    }
    TaskYieldTrampoline(14);
    WarpStarSetTrail(1, 5, 4, 0x300);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(4);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(4);
    WarpStarSetTrail(1, 4, 4, 0x300);
    gCurTask->warpStarLoopCount = 0;
    do
    {
        {
            struct Task *u = gCurTask;

            u->velX = 0x60000;
            u->velY = -0x60000;
        }
        TaskYieldTrampoline(2);
        {
            struct Task *u = gCurTask;

            u->velX = -0x60000;
            u->velY = 0x60000;
        }
        TaskYieldTrampoline(1);
        gCurTask->warpStarLoopCount++;
    } while ((s16)gCurTask->warpStarLoopCount <= 2);
    TaskStop();
    gCurTask->warpStarLoopCount = 0;
    do
    {
        gCurTask->velY = -0x60000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x60000;
        TaskYieldTrampoline(1);
        gCurTask->warpStarLoopCount++;
    } while ((s16)gCurTask->warpStarLoopCount <= 2);
    gCurTask->warpStarLoopCount = 0;
    do
    {
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(1);
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(1);
        gCurTask->warpStarLoopCount++;
    } while ((s16)gCurTask->warpStarLoopCount <= 2);
    WarpStarStopTrail();
    TaskStopY();
    TaskYieldTrampoline(8);
    {
        struct Task *u = gCurTask;

        u->velX = -0x800;
        u->velY = 0x800;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x2000;
        u->velY = 0x2000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x8000;
        u->velY = 0x8000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x10000;
        u->velY = 0x10000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x20000;
        u->velY = 0x20000;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *u = gCurTask;

        u->velX = -0x40000;
        u->velY = 0x40000;
    }
    TaskYieldTrampoline(6);
    gCurTask->velX = -0x80000;
    TaskYieldTrampoline(6);
    gCurTask->facing = 255;
    CameraStartFollowFocusAt(gCurTask->pixelX, gCurTask->pixelY);
    RequestScreenShake(4);
    StopSfxOnPlayer(gWarpStarFlightSfxPlayer, gWarpStarFlightSfx);
    PlaySfx(219);
    CreateBurstEffect(0, 0);
    PlaySfx(272);
    if (gMetaKnightmareMode == 0)
        WarpStarSetRiderState(4);
    else
        WarpStarSetMetaKnightRiderState(4);
    gUnk_020061E0 = 0;
    ActorDestroy();
}

void WarpStarFlight12Update(void)
{
    WarpStarEmitTrailStars();
}
