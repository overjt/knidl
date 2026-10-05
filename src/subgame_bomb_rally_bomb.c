/* game_code_and_rodata_080653ec_0806ef5c 0x080BF994-0x080C0DE8
 * (issue #66, module M36 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BF994 0x080C0DE8 src/subgame_bomb_rally_bomb.c --newpb
 *
 * The middle of M36: the four per-slot tasks and the geometry the whole
 * module places sprites with.  A slot task carries its own index in
 * Task.unk1C and the id of the task that spawned it in Task.parent, so
 * `&gTasks[t->unk44]` is the mode task whose Task.unk34 says whose
 * turn it is.
 *
 *   BombRallyPlayerBubblesServe   slot-task entry: seat the sprite from the per-slot lists
 *       gUnk_0875674C / gUnk_0875675C (16.16 x/y) and gBombRallyBubblesFrames
 *       (animation), then branch on whether this slot has the turn.
 *   BombRallyPlayerBubblesServeUpdate / BombRallyPlayerBubblesWait / BombRallyPlayerBubblesWaitUpdate   its three short states.
 *   BombRallyPlayerBubblesThrow   the hand-off animation: six frames off gUnk_0875676C
 *       counted down in Task.unk30/unk34, then the position update through
 *       CreateBombRallyStarBurst.
 *   BombRallyBombStart / BombRallyBombStartUpdate   the eliminated and winner states.
 *   BombRallyBombPass   the free-running driver: waits out the intro, then loops
 *       for ever re-reading the mode task's slot and, whenever it changes,
 *       re-arms the sprite and recomputes the hand-off speed as
 *       Div((gBombRallyBombScales[next] - gBombRallyBombScales[cur]) << 4, frames).
 *   BombRallyBombPassUpdate / BombRallyBombExplode / BombRallyBombExplodeUpdate   the three placement bodies
 *       that walk a turn: BombRallyScrollAlongPass for the thrower, BombRallyBombSetArcPos for the
 *       projectile and BombRallyBombDrawShadow for its shadow.
 *   BombRallyBombPlaceAtSeat / BombRallyScrollToSeat   set a slot's sprite frame / animation.
 *   BombRallyBombSetArcPos   position on the 16.16 parabola p0 + v*t + (a*t*t)/2:
 *       p0 from gBombRallySeatBombX / gBombRallySeatBombY, v from gBombRallyArcVelX[c][b],
 *       and the two coefficient rows from gUnk_08756D3C[c][0..1].
 *   BombRallyPanToSeat / BombRallyBombSmoke / BombRallyBombSmokeUpdate / BombRallyStarBurst   the
 *       remaining table-driven animation helpers.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "subgame.h"

/* Not from main.h: this file's view of gBgPalette differs (lesson 3.517). */
extern vu16 gFadeSteps;
extern u8 gObjPalette[];
extern vs32 gBg3ScrollX;
extern vs32 gBg3ScrollY;
extern vu16 gPlayerPressedKeys[];

extern vu16 gBgPalette[];
extern vu16 gDispCnt;

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BeginFastFadeInFromWhite(void);
extern void BeginFastFadeOutToWhite(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetFrame(s32 a);
extern void PlaySfx(u32 a);
extern void sub_080060c0(void);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void TaskStop(void);
extern u32 RandomRange(u32 range);
extern void TaskSleepForever(void);

extern void TaskStopY(void);
/* NOTE: CreateBombRallyBombSmoke's third parameter is `u16` at its definition
   (src/subgame_bomb_rally.c) but the ROM's call sites here sign-extend the
   argument, so the declaration visible here is the wider `s32` - the
   original source had the same prototype mismatch. */
extern void CreateBombRallyBombSmoke(s32 a, s32 b, s32 c, s32 d);

void BombRallyPlayerBubblesServe(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateState = BOMB_RALLY_PLAYER_STATE_BUBBLES_SERVE;
    v = gCurTask;
    v->posX = gUnk_0875674C[v->bombRallyObjectSeat] << 16;
    v->posY = gUnk_0875675C[v->bombRallyObjectSeat] << 16;
    v->frameTable = gBombRallyBubblesFrames[v->bombRallyObjectSeat];
    v->tileWord = 0;
    if (v->bombRallyObjectSeat == 3)
        v->facing = -1;
    else
        v->facing = 1;
    TaskSetFrame(0);
    w = gCurTask;
    u = &gTasks[w->parent];
    w->bombRallyObjectAimSide = 0;
    if (u->bombRallyTurnSeat == w->bombRallyObjectSeat) {
        TaskYieldTrampoline(30);
        u->bombRallyThrow = -1;
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(16);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        PlaySfx(254);
        z = gCurTask;
        CreateBombRallyStarBurst(z->posX + (gBombRallyStarBurstOffsetX[z->bombRallyObjectSeat] << 16) * z->facing,
                     z->posY + (gBombRallyStarBurstOffsetY[z->bombRallyObjectSeat] << 16),
                     z->bombRallyObjectSeat, z->facing);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        u->bombRallyThrow = u->bombRallyNextThrow;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    } else {
        while (u->bombRallyThrow < 0)
            TaskYieldTrampoline(1);
    }
    gCurTask->state = BOMB_RALLY_PLAYER_STATE_BUBBLES_WAIT;
    TaskSleepForever();
}

void BombRallyPlayerBubblesServeUpdate(void)
{
    if (gCurTask->state != BOMB_RALLY_PLAYER_STATE_BUBBLES_SERVE)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerBubblesWait(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = BOMB_RALLY_PLAYER_STATE_BUBBLES_WAIT;
    while (1) {
        gCurTask->frame = 0;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(5);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(5);
    }
}

void BombRallyPlayerBubblesWaitUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 3) & 3) && u->bombRallyThrow <= 2)
     || (u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 1) & 3) && u->bombRallyThrow > 2)) {
        gCurTask->state = BOMB_RALLY_PLAYER_STATE_BUBBLES_THROW;
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
    }
}

void BombRallyPlayerBubblesThrow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;
    s32 m;
    struct Task **gp;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->updateState = BOMB_RALLY_PLAYER_STATE_BUBBLES_THROW;
    if (RandomRange(8) == 0) {
        if (u->bombRallyThrow <= 2)
            u->bombRallyNextThrow = u->bombRallyThrow + 3;
        else
            u->bombRallyNextThrow = u->bombRallyThrow;
    } else {
        if (u->bombRallyThrow > 2)
            u->bombRallyNextThrow = u->bombRallyThrow - 3;
        else
            u->bombRallyNextThrow = u->bombRallyThrow;
    }
    v = gCurTask;
    v->unk28 = u->bombRallyNextThrow;
    m = u->bombRallyPassFrames;
    v->bombRallyObjectBubblesFrameTimer = 6;
    v->bombRallyObjectBubblesFrameStep = 0;
    gp = &gCurTask;
    v->bombRallyObjectBubblesThrowDelay = m - 8;
    if (v->bombRallyObjectBubblesThrowDelay != 0) {
        do {
        w = gCurTask;
        w->bombRallyObjectBubblesFrameTimer--;
        if (w->bombRallyObjectBubblesFrameTimer == 0) {
            w->bombRallyObjectBubblesFrameStep++;
            if (w->bombRallyObjectBubblesFrameStep == 4)
                w->bombRallyObjectBubblesFrameStep = 0;
            x = gCurTask;
            x->frame = gUnk_0875676C[x->bombRallyObjectBubblesFrameStep];
            if (x->bombRallyObjectBubblesFrameStep & 1)
                x->bombRallyObjectBubblesFrameTimer = 6;
            else
                x->bombRallyObjectBubblesFrameTimer = 5;
        }
        TaskYieldTrampoline(1);
        y = *gp;
        y->bombRallyObjectBubblesThrowDelay--;
        } while (y->bombRallyObjectBubblesThrowDelay != 0);
    }
    gCurTask->frame = 0;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlaySfx(254);
    z = gCurTask;
    CreateBombRallyStarBurst(z->posX + (gBombRallyStarBurstOffsetX[z->bombRallyObjectSeat] << 16) * z->facing,
                 z->posY + (gBombRallyStarBurstOffsetY[z->bombRallyObjectSeat] << 16),
                 z->bombRallyObjectSeat, z->facing);
    gCurTask->frame++;
    TaskYieldTrampoline(7);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->state = BOMB_RALLY_PLAYER_STATE_BUBBLES_WAIT;
    TaskSleepForever();
}

void BombRallyPlayerBubblesThrowUpdate(void)
{
    if (gCurTask->state != BOMB_RALLY_PLAYER_STATE_BUBBLES_THROW)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyBomb(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)BombRallyBombUpdate;
    t->state = BOMB_RALLY_BOMB_STATE_START;
    CallTableEntry(gCurTask->state, 3, gBombRallyBombStates);
    TaskSleepForever();
}

void BombRallyBombUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gBombRallyBombStateUpdates);
}

void BombRallyBombEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gBombRallyBombStates);
}

void BombRallyBombStart(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->drawCallback = 0;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateState = BOMB_RALLY_BOMB_STATE_START;
    CreateBombRallyStartSign();
    v = gCurTask;
    v->bombRallyObjectFrameCount = 0;
    v->bombRallyObjectBombSeat = u->bombRallyTurnSeat;
    BombRallyBombPlaceAtSeat(v->bombRallyObjectBombSeat);
    BombRallyScrollToSeat(gCurTask->bombRallyObjectStartSeat);
    TaskYieldTrampoline(30);
    BombRallyPanToSeat(gCurTask->bombRallyObjectBombSeat);
    BombRallyScrollToSeat(gCurTask->bombRallyObjectBombSeat);
    w = gCurTask;
    w->frameTable = 0;
    TaskYieldTrampoline(30);
    u->bombRallyThrow = -2;
    gCurTask->state = BOMB_RALLY_BOMB_STATE_PASS;
    TaskSleepForever();
}

void BombRallyBombStartUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;
    s32 m;
    s32 k;

    t = gCurTask;
    n = t->bombRallyObjectFrameCount + 1;
    t->bombRallyObjectFrameCount = n;
    if (t->pixelX >= -63 && t->pixelX <= 303 && t->pixelY > -64 && t->pixelY <= 223
     && (n & 8)) {
        m = n & 7;
        k = 0;
        if (m > 2) {
            k = 2;
            if (m <= 4)
                k = 1;
        }
        QueueSprite(8, DrawAffineSprite(gBombRallyBombFrames[k],
                                    gBombRallyBombScales[gCurTask->bombRallyObjectBombSeat],
                                    gBombRallyBombScales[gCurTask->bombRallyObjectBombSeat], 0),
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->pixelX, gCurTask->pixelY);
    }
    if (gCurTask->state != BOMB_RALLY_BOMB_STATE_START)
        TaskSetEntry(BombRallyBombEnterState, gCurTaskIdx);
}

void BombRallyBombPass(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *p;
    struct Task *q;
    s32 d;
    s32 m;
    s32 msk;
    s32 n;
    s16 *tb;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->drawCallback = 0;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateState = BOMB_RALLY_BOMB_STATE_PASS;
    gCurTask->layer = 8;
    v = gCurTask;
    v->frameTable = gBombRallyBombFrames;
    v->bombRallyObjectBombPhase = 0;
    v->bombRallyObjectScaleStep = 0;
    v->bombRallyObjectFrameCount = 0;
    v->bombRallyObjectBombAngle = 0;
    v->bombRallyObjectBombSeat = u->bombRallyTurnSeat;
    BombRallyScrollToSeat(u->bombRallyTurnSeat);
    while (u->bombRallyThrow < -1)
        TaskYieldTrampoline(1);
    w = gCurTask;
    w->frame = (u->bombRallyTurnSeat * 3) << 1;
    BombRallyBombPlaceAtSeat(u->bombRallyTurnSeat);
    x = gCurTask;
    x->velY = 0xFFFBC000;
    x->accelY = 128 << 7;
    x->bombRallyObjectBombPhase = 1;
    while (u->bombRallyThrow < 0) {
        tb = gBombRallySeatBombY;
        if (gCurTask->posY > (tb[u->bombRallyTurnSeat] << 16)) {
            TaskStopY();
            BombRallyBombPlaceAtSeat(u->bombRallyTurnSeat);
        }
        TaskYieldTrampoline(1);
    }
    TaskStopY();
    y = gCurTask;
    y->bombRallyObjectBombSeat = u->bombRallyTurnSeat - 1;
    while (1) {
        z = gCurTask;
        tb = gBombRallyBombScales;
        u = &gTasks[z->parent];
        if (z->bombRallyObjectBombSeat != u->bombRallyTurnSeat) {
            BombRallyBombPlaceAtSeat(u->bombRallyTurnSeat);
            BombRallyScrollToSeat(u->bombRallyTurnSeat);
            p = gCurTask;
            m = u->bombRallyTurnSeat;
            p->bombRallyObjectBombSeat = m;
            p->bombRallyObjectBombPhase = 2;
            if (u->bombRallyThrow <= 2) {
                n = m + 1;
                msk = 3;
                d = n & msk;
            } else {
                n = m - 1;
                msk = 3;
                d = n & msk;
            }
            n = tb[d];
            n -= tb[m];
            gCurTask->bombRallyObjectScaleStep = Div(n << 4, u->bombRallyPassFrames);
        }
        TaskYieldTrampoline(1);
    }
}

void BombRallyBombPassUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *p;
    struct Task *q;
    s32 n;
    s32 st;
    s32 k;
    s32 m;
    s32 c;
    s32 x;
    s32 y;

    t = gCurTask;
    u = &gTasks[t->parent];
    n = t->bombRallyObjectFrameCount + 1;
    t->bombRallyObjectFrameCount = n;
    if (u->bombRallyThrow == 6) {
        t->state = BOMB_RALLY_BOMB_STATE_EXPLODE;
        TaskSetEntry(BombRallyBombEnterState, gCurTaskIdx);
        return;
    }
    st = t->bombRallyObjectBombPhase;
    switch (st) {
    case 0:
        TaskStop();
        gCurTask->frame = 0xFFFF;
        break;
    case 3:
        if (n & 1)
            t->posX += 0x40000;
        else
            t->posX -= 0x40000;
        /* fall through */
    case 1:
        v = gCurTask;
        m = v->bombRallyObjectFrameCount & 7;
        if (m <= 2)
            v->frame = 0;
        else if (m <= 4)
            v->frame = 1;
        else
            v->frame = 2;
        break;
    case 2:
        if (u->bombRallyThrow > 2) {
            k = u->bombRallyThrow - 3;
            BombRallyScrollAlongPass(u->bombRallyTurnSeat, k, u->bombRallySpeedLevel, u->bombRallyPassTimer, u->bombRallyPassFrames, 1);
            BombRallyBombSetArcPos((u->bombRallyTurnSeat + 3) & 3, k, u->bombRallySpeedLevel, u->bombRallyPassFrames - u->bombRallyPassTimer);
            BombRallyBombDrawShadow((u->bombRallyTurnSeat + 3) & 3, k, u->bombRallySpeedLevel, u->bombRallyPassFrames - u->bombRallyPassTimer,
                         (s16)(gUnk_08756778[gCurTask->bombRallyObjectBombSeat]
                               + ((gCurTask->bombRallyObjectScaleStep * u->bombRallyPassTimer) >> 5)));
            if (u->bombRallyTurnSeat == 0 || u->bombRallyTurnSeat == 3) {
                w = gCurTask;
                w->bombRallyObjectBombAngle -= 4 << (st - k);
                if (w->bombRallyObjectBombAngle < 0)
                    w->bombRallyObjectBombAngle += 512;
            } else {
                w = gCurTask;
                w->bombRallyObjectBombAngle += 4 << (st - k);
                if (w->bombRallyObjectBombAngle > 511)
                    w->bombRallyObjectBombAngle -= 512;
            }
        } else {
            k = u->bombRallyThrow;
            BombRallyScrollAlongPass(u->bombRallyTurnSeat, k, u->bombRallySpeedLevel, u->bombRallyPassTimer, u->bombRallyPassFrames, 0);
            BombRallyBombSetArcPos(u->bombRallyTurnSeat, k, u->bombRallySpeedLevel, u->bombRallyPassTimer);
            BombRallyBombDrawShadow(u->bombRallyTurnSeat, k, u->bombRallySpeedLevel, u->bombRallyPassTimer,
                         (s16)(gUnk_08756778[gCurTask->bombRallyObjectBombSeat]
                               + ((gCurTask->bombRallyObjectScaleStep * u->bombRallyPassTimer) >> 5)));
            if (u->bombRallyTurnSeat == 0 || u->bombRallyTurnSeat == 3) {
                w = gCurTask;
                w->bombRallyObjectBombAngle += 4 << (st - k);
                if (w->bombRallyObjectBombAngle > 511)
                    w->bombRallyObjectBombAngle -= 512;
            } else {
                w = gCurTask;
                w->bombRallyObjectBombAngle -= 4 << (st - k);
                if (w->bombRallyObjectBombAngle < 0)
                    w->bombRallyObjectBombAngle += 512;
            }
        }
        p = gCurTask;
        m = p->bombRallyObjectFrameCount & 7;
        if (m <= 2)
            p->frame = 0;
        else if (m <= 4)
            p->frame = 1;
        else
            p->frame = 2;
        if (u->bombRallyPassTimer == u->bombRallyPassFrames)
            gCurTask->bombRallyObjectBombPhase = 3;
        break;
    }
    q = gCurTask;
    if ((s16)q->frame != -1) {
        if ((q->bombRallyObjectFrameCount & 3) == 0)
            CreateBombRallyBombSmoke(q->posX, q->posY,
                         (s16)(gBombRallyBombScales[q->bombRallyObjectBombSeat]
                               + ((q->bombRallyObjectScaleStep * u->bombRallyPassTimer) >> 4)),
                         q->bombRallyObjectBombAngle);
        x = (gCurTask->posX - gBg3ScrollX) >> 16;
        y = (gCurTask->posY - gBg3ScrollY) >> 16;
        if (x >= -63 && x <= 303 && y > -64 && y <= 223)
            QueueSprite(8,
                DrawAffineSprite(gBombRallyBombFrames[(s16)gCurTask->frame],
                    (s16)(gBombRallyBombScales[gCurTask->bombRallyObjectBombSeat]
                          + ((gCurTask->bombRallyObjectScaleStep * u->bombRallyPassTimer) >> 4)),
                    (s16)(gBombRallyBombScales[gCurTask->bombRallyObjectBombSeat]
                          + ((gCurTask->bombRallyObjectScaleStep * u->bombRallyPassTimer) >> 4)),
                    (s16)gCurTask->bombRallyObjectBombAngle),
                gCurTask->spriteFlags, gCurTask->tileWord, x, y);
    }
}

void BombRallyBombExplode(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->drawCallback = (u32)TaskDrawScreen;
    t->updateState = BOMB_RALLY_BOMB_STATE_EXPLODE;
    gBgPalette[0] = 0x7FFF;
    v = gCurTask;
    v->bombRallyObjectBlastSeat = u->bombRallyTurnSeat;
    v->bombRallyObjectShakeStep = 0;
    v->bombRallyObjectFrameCount = 0;
    BombRallyScrollToSeat(v->bombRallyObjectBlastSeat);
    gCurTask->layer = 6;
    TaskStop();
    w = gCurTask;
    w->posX = gBombRallySeatX[w->bombRallyObjectBlastSeat] << 16;
    w->posY = (gBombRallySeatY[w->bombRallyObjectBlastSeat] + gUnk_0875673C[w->bombRallyObjectBlastSeat]) << 16;
    PlaySfx(255);
    x = gCurTask;
    CreateBombRallyStarBurst(x->posX, x->posY, x->bombRallyObjectBlastSeat, x->facing);
    y = gCurTask;
    if (y->bombRallyObjectBlastSeat == 0)
        y->frameTable = gBombRallyBlastNearFrames;
    else if (y->bombRallyObjectBlastSeat == 2)
        y->frameTable = gBombRallyBlastFarFrames;
    else
        y->frameTable = gBombRallyBlastSideFrames;
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->bombRallyObjectLoopCount = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->bombRallyObjectLoopCount++;
    } while ((s16)gCurTask->bombRallyObjectLoopCount <= 2);
    gCurTask->bombRallyObjectLoopCount = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->bombRallyObjectLoopCount++;
    } while ((s16)gCurTask->bombRallyObjectLoopCount <= 6);
    gCurTask->bombRallyObjectLoopCount = 0;
    do {
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->bombRallyObjectLoopCount++;
    } while ((s16)gCurTask->bombRallyObjectLoopCount <= 1);
    gCurTask->bombRallyObjectLoopCount = 0;
    do {
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->bombRallyObjectLoopCount++;
    } while ((s16)gCurTask->bombRallyObjectLoopCount <= 1);
    TaskSleepForever();
}

void BombRallyBombExplodeUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->parent];
    n = BombRallyShakeScreen(t->bombRallyObjectBlastSeat, t->bombRallyObjectShakeStep);
    v = gCurTask;
    v->bombRallyObjectShakeStep = n;
    if (v->bombRallyObjectFrameCount <= 7) {
        if (((v->bombRallyObjectFrameCount >> 1) & 1) == 0) {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x80 << 5;
        } else {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0xC0 << 5;
        }
        gCurTask->bombRallyObjectFrameCount++;
    }
    if (u->bombRallyThrow == -4) {
        gCurTask->state = BOMB_RALLY_BOMB_STATE_PASS;
        TaskSetEntry(BombRallyBombEnterState, gCurTaskIdx);
    }
}

void BombRallyBombPlaceAtSeat(u32 a)
{
    struct Task *t = gCurTask;

    t->posX = gBombRallySeatBombX[a] << 16;
    t->posY = gBombRallySeatBombY[a] << 16;
}

void BombRallyBombSetArcPos(s32 a, s32 b, s32 c, s32 d)
{
    struct Task *t;
    struct Task *u;
    s32 *p1;
    s32 *p2;

    p1 = gUnk_08756D3C[c][0];
    p2 = gUnk_08756D3C[c][1];
    if (a == 0 || a == 3)
        gCurTask->posX = (gBombRallySeatBombX[a] << 16) - gBombRallyArcVelX[c][b] * d;
    else
        gCurTask->posX = (gBombRallySeatBombX[a] << 16) + gBombRallyArcVelX[c][b] * d;
    gCurTask->posY = (gBombRallySeatBombY[a] << 16) + p1[b * 4 + a] * d
             + ((p2[b * 4 + a] * d * d) >> 1);
}

void BombRallyScrollToSeat(u32 a)
{
    gBg3ScrollX = gBombRallySeatScrollX[a];
    gBg3ScrollY = gBombRallySeatScrollY[a];
}

void BombRallyScrollAlongPass(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    if (f == 0) {
        if (a == 0 || a == 3)
            gBg3ScrollX = gBombRallySeatScrollX[a] - gUnk_08756D74[c * 3 + b] * d
                          + ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        else
            gBg3ScrollX = gBombRallySeatScrollX[a] + gUnk_08756D74[c * 3 + b] * d
                          - ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        switch (b) {
        case 2:
            gBg3ScrollY = gBombRallySeatScrollY[a] + gUnk_08756F34[c][a] * d
                          + ((gUnk_08756FA4[c][a] * d * d) >> 1);
            break;
        case 1:
            gBg3ScrollY = gBombRallySeatScrollY[a] + gUnk_08756E54[c][a] * d
                          + ((gUnk_08756EC4[c][a] * d * d) >> 1);
            break;
        case 0:
            if (a <= 1)
                gBg3ScrollY = gBombRallySeatScrollY[a] - gUnk_08756E1C[c] * d
                              + ((gUnk_08756E38[c] * d * d) >> 1);
            else
                gBg3ScrollY = gBombRallySeatScrollY[a] + gUnk_08756E1C[c] * d
                              - ((gUnk_08756E38[c] * d * d) >> 1);
            break;
        }
    } else {
        if (a <= 1)
            gBg3ScrollX = gBombRallySeatScrollX[a] + gUnk_08756D74[c * 3 + b] * d
                          - ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        else
            gBg3ScrollX = gBombRallySeatScrollX[a] - gUnk_08756D74[c * 3 + b] * d
                          + ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        switch (b) {
        case 2:
            gBg3ScrollY = gBombRallySeatScrollY[(a + 3) & 3]
                          + gUnk_08756F34[c][(a + 3) & 3] * (e - d)
                          + ((gUnk_08756FA4[c][(a + 3) & 3] * (e - d) * (e - d)) >> 1);
            break;
        case 1:
            gBg3ScrollY = gBombRallySeatScrollY[(a + 3) & 3]
                          + gUnk_08756E54[c][(a + 3) & 3] * (e - d)
                          + ((gUnk_08756EC4[c][(a + 3) & 3] * (e - d) * (e - d)) >> 1);
            break;
        case 0:
            if (a == 0 || a == 3)
                gBg3ScrollY = gBombRallySeatScrollY[a] - gUnk_08756E1C[c] * d
                              + ((gUnk_08756E38[c] * d * d) >> 1);
            else
                gBg3ScrollY = gBombRallySeatScrollY[a] + gUnk_08756E1C[c] * d
                              - ((gUnk_08756E38[c] * d * d) >> 1);
            break;
        }
    }
}

void BombRallyBombDrawShadow(s32 a, s32 b, s32 c, s32 d, s16 e)
{
    s32 x;
    s32 y;
    s32 *p;

    if ((s16)gCurTask->frame != -1) {
        p = gUnk_0875716C[b];
        if (a == 0 || a == 3)
            x = (gBombRallySeatBombX[a] << 16) - gBombRallyArcVelX[c][b] * d;
        else
            x = (gBombRallySeatBombX[a] << 16) + gBombRallyArcVelX[c][b] * d;
        y = (gBombRallySeatShadowY[a] << 16) + p[c * 4 + a] * d;
        QueueSprite(10, DrawAffineSprite(gBombRallyBombShadowFrames, (s16)e, (s16)e, 0), 0, 0,
                     (x >> 16) - (gBg3ScrollX >> 16),
                     (s16)((y >> 16) - (gBg3ScrollY >> 16)));
    }
}

void BombRallyPanToSeat(u32 a)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *z;

    t = gCurTask;
    t->bombRallyObjectPanVelX = -gBombRallyPanStepX[t->bombRallyObjectStartSeat][a];
    t->bombRallyObjectPanVelY = -gBombRallyPanStepY[t->bombRallyObjectStartSeat][a];
    t->bombRallyObjectPanLoopCount = 0;
    do {
        u = gCurTask;
        if ((s16)u->bombRallyObjectPanLoopCount <= 16) {
            u->bombRallyObjectPanVelX += gBombRallyPanStepX[u->bombRallyObjectStartSeat][a];
            u->bombRallyObjectPanVelY += gBombRallyPanStepY[u->bombRallyObjectStartSeat][a];
        } else {
            u->bombRallyObjectPanVelX -= gBombRallyPanStepX[u->bombRallyObjectStartSeat][a];
            u->bombRallyObjectPanVelY -= gBombRallyPanStepY[u->bombRallyObjectStartSeat][a];
        }
        gBg3ScrollX += gCurTask->bombRallyObjectPanVelX;
        gBg3ScrollY += gCurTask->bombRallyObjectPanVelY;
        if ((gCurTask->bombRallyObjectPanVelX < 0 && gBg3ScrollX < gBombRallySeatScrollX[a])
         || (gCurTask->bombRallyObjectPanVelX > 0 && gBg3ScrollX > gBombRallySeatScrollX[a]))
            gBg3ScrollX = gBombRallySeatScrollX[a];
        w = gCurTask;
        if ((w->bombRallyObjectPanVelY < 0 && gBg3ScrollY < gBombRallySeatScrollY[a])
         || (w->bombRallyObjectPanVelY > 0 && gBg3ScrollY > gBombRallySeatScrollY[a]))
            gBg3ScrollY = gBombRallySeatScrollY[a];
        TaskYieldTrampoline(1);
        z = gCurTask;
        z->bombRallyObjectPanLoopCount++;
    } while ((s16)z->bombRallyObjectPanLoopCount <= 31);
}

void BombRallyBombSmoke(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = 0;
    t->moveCallback = (u32)TaskMove;
    t->updateCallback = (u32)BombRallyBombSmokeUpdate;
    t->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

void BombRallyBombSmokeUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;
    s32 y;
    s32 n;

    t = gCurTask;
    n = t->bombRallyObjectSmokeAngle + 8;
    t->bombRallyObjectSmokeAngle = n;
    if (n > 255)
        t->bombRallyObjectSmokeAngle = n & 255;
    u = gCurTask;
    x = (u->posX - gBg3ScrollX) >> 16;
    y = (u->posY - gBg3ScrollY) >> 16;
    if (x >= -63 && x <= 303 && y > -64 && y <= 223)
        QueueSprite(7, DrawAffineSprite(gBombRallyBombSmokeFrames[(s16)u->frame], u->bombRallyObjectSmokeScale,
                                     u->bombRallyObjectSmokeScale, (s16)u->bombRallyObjectSmokeAngle),
                     0, 0, x, y);
}

void BombRallyStarBurst(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)TaskDrawScreen;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateCallback = 0;
    t->frameTable = gBombRallyStarFrames;
    switch (t->bombRallyObjectBurstKind) {
    case 0:
        CreateBombRallyBurstStar(4);
        CreateBombRallyBurstStar(5);
        CreateBombRallyBurstStar(6);
        CreateBombRallyBurstStar(7);
        break;
    case 1:
    case 3:
        CreateBombRallyBurstStar(8);
        CreateBombRallyBurstStar(9);
        CreateBombRallyBurstStar(10);
        CreateBombRallyBurstStar(11);
        break;
    case 2:
        CreateBombRallyBurstStar(12);
        CreateBombRallyBurstStar(13);
        CreateBombRallyBurstStar(14);
        CreateBombRallyBurstStar(15);
        break;
    }
    u = gCurTask;
    u->state = u->bombRallyObjectBurstKind;
    CallTableEntry(gCurTask->state, 16, gBombRallyStarBurstStates);
    TaskSleepForever();
}
