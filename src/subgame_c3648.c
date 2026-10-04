#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "subgame.h"

/* subgame_c3648.c (0x080C3648-0x080C3F43, issue #98).
 *
 * Sub-game 2: the racers' movement (task type #96 variant 0's callbacks).
 * 
 *   AirGrindRacerRaceUpdate / AirGrindRacerIdleUpdate   the two per-frame bodies variant 0 installs
 *       in Task.lateUpdateCallback (racing / before the start and after the finish): run
 *       AirGrindRacerUpdateDepth, then AirGrindRacerRaceStep or AirGrindRacerIdleStep, then AirGrindRacerUpdateScreenPos.
 *   AirGrindRacerUpdateDepth   player 0 drives the camera (AirGrindScrollCourseTo) and rebuilds the
 *       course view (AirGrindDrawCourse) and keeps the leader's position in
 *       AirGrindState.leaderCoursePos; every racer sets Task.layer/unk3E from the course
 *       record's unk18 and Task.unk28 from its unk08.
 *   AirGrindRacerRaceStep   the racing step: holding A (AirGrindRacerState.heldKeys & 1) on the
 *       course (record unk14 != 0) accelerates Task.velX by the level's
 *       thresholds gAirGrindRacerSpeeds[level][], a well-timed press gives a boost
 *       (AirGrindRacerTryBoost, capped by gAirGrindRacerSpeeds[level][0]), and holding A
 *       while the record's unk14 is 0 starts a 24-frame penalty
 *       (AirGrindRacerState.penaltyTimer); the tilt and animation frame
 *       Task.frame come from the tables gAirGrindRacerFrameSteps/gUnk_080CFEE9/
 *       gUnk_080CFF01, and player 0's effects are variant 2 tasks
 *       (CreateAirGrindEffect).
 *   AirGrindRacerIdleStep   the idle step used before the start and after the finish.
 *   AirGrindRacerUpdateScreenPos   the racer's screen position from the course record and
 *       the scale AirGrindGetDepthScale, and the computer racers' distance fade
 *       (BlendColors on their palette row gUnk_08609D42[pal]).
 *   AirGrindRacerSlowDown / AirGrindScrollCourseTo / AirGrindUpdateEngineSound   the speed floor 0x18000,
 *       the camera clamp (scroll = min(pos, finish line), the script cursor's
 *       unk2/unk4 into the course record), and player 0's engine sound
 *       (song 400, pitch from the speed through m4aMPlayPitchControl). */

void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);

void AirGrindRacerSlowDown(void)
{
    struct Task *t = gCurTask;

    t->accelX = -0x3800;
    if (t->velX < 0x18000) {
        t->accelX = 0;
        t->velX = 0x18000;
    }
}

void AirGrindScrollCourseTo(s32 pos)
{
    struct AirGrindCourse *c = gAirGrindCoursePtr;

    c->scrollPos = pos > c->finishLine ? c->finishLine : pos;
    c->unk004 = gAirGrindScript.unk4;
    c->unk008 = gAirGrindScript.unk2;
}

s32 AirGrindRacerTryBoost(s32 player)
{
    s32 speed = gCurTask->velX;
    s32 ret = 0;

    if (gAirGrindCoursePtr->players[player].coursePos > gAirGrindCoursePtr->finishLine - 50)
        return 0;
    if (abs(gAirGrindPtr->players[player].segmentEndDistance) <= abs((speed * 3) >> 16)) {
        if (abs(gAirGrindPtr->players[player].segmentEndDistance) <= abs(speed >> 16)) {
            if (player == 0)
                CreateAirGrindEffect(0, gCurTask->unk18, 6);
            gAirGrindPtr->players[player].fullBoostCount++;
            gCurTask->velX += 0x10000;
            if (gCurTask->velX > gAirGrindRacerSpeeds[gAirGrindPtr->level][0])
                gCurTask->velX = gAirGrindRacerSpeeds[gAirGrindPtr->level][0];
            ret = 2;
        } else {
            if (player == 0)
                CreateAirGrindEffect(0, gCurTask->unk18, 7);
            gCurTask->velX += 0x8000;
            if (gCurTask->velX > gAirGrindRacerSpeeds[gAirGrindPtr->level][0])
                gCurTask->velX = gAirGrindRacerSpeeds[gAirGrindPtr->level][0];
            ret = 1;
        }
        gAirGrindPtr->players[player].boostCooldown = 5;
    }
    return ret;
}

void AirGrindUpdateEngineSound(s32 a, s32 on)
{
    if (a == 0) {
        if (on) {
            if (gAirGrindPtr->engineSfxPlayer == -1)
                gAirGrindPtr->engineSfxPlayer = PlaySfx(400);
            else
                m4aMPlayPitchControl(gMPlayTable[gAirGrindPtr->engineSfxPlayer].info, 0xFFFF,
                                     (gCurTask->velX - 0x40000) >> 5);
        } else {
            if (gAirGrindPtr->engineSfxPlayer != -1) {
                StopSfxOnPlayer(gAirGrindPtr->engineSfxPlayer, 400);
                gAirGrindPtr->engineSfxPlayer = -1;
            }
        }
    }
}

void AirGrindRacerUpdateDepth(s32 player)
{
    s32 max;
    s32 i;
    u16 rank;

    if (player == 0) {
        AirGrindScrollCourseTo((gCurTask->posX >> 16) + 48);
        AirGrindDrawCourse();
        max = 0;
        for (i = 0; i < 4; i++) {
            if (gAirGrindCoursePtr->players[i].coursePos > max)
                max = gAirGrindCoursePtr->players[i].coursePos;
        }
        gAirGrindPtr->leaderCoursePos = max;
    }
    rank = (u16)gAirGrindCoursePtr->players[player].depthRank;
    gCurTask->layer = rank + 4;
    if (rank > 2)
        rank = 2;
    gCurTask->spriteFlags = (rank << 13) & 0x6000;
    gCurTask->unk28 = gAirGrindCoursePtr->players[player].depth;
}

void AirGrindRacerRaceStep(s32 player)
{
    s32 ret;

    if (gAirGrindPtr->players[player].penaltyTimer != 0) {
        AirGrindRacerSlowDown();
        if (player == 0 && gAirGrindPtr->players[0].penaltyTimer > 19)
            CreateAirGrindEffect(0, gCurTask->unk18, (gAirGrindPtr->players[0].penaltyTimer & 1) ? 3 : 4);
    } else if (gAirGrindPtr->players[player].heldKeys & 1) {
        if (gAirGrindCoursePtr->players[player].onEvenSegment != 0) {
            ret = 0;
            AirGrindUpdateEngineSound(player, 1);
            if (gCurTask->velX > gAirGrindRacerSpeeds[gAirGrindPtr->level][1])
                gCurTask->accelX = 0x200;
            else if (gCurTask->velX > gAirGrindRacerSpeeds[gAirGrindPtr->level][2])
                gCurTask->accelX = 0xA00;
            else
                gCurTask->accelX = 0x8000;
            if (player == 0) {
                if ((gAirGrindFrame & 3) == 0)
                    CreateAirGrindEffect(0, gCurTask->unk18, 0);
                if ((gAirGrindFrame & 7) == 1)
                    CreateAirGrindEffect(0, gCurTask->unk18, 1);
            }
            if (gAirGrindPtr->players[player].boostCooldown == 0 && gAirGrindPtr->players[player].grinding == 0)
                ret = AirGrindRacerTryBoost(player);
            if (gAirGrindPtr->players[player].pressedKeys & 1) {
                if (player == 0 && gAirGrindPtr->pressEffectShown == 0)
                    CreateAirGrindEffect(0, gCurTask->unk18, 2);
                gAirGrindPtr->players[player].dashTimer = 4;
                if (ret == 1) {
                    gAirGrindPtr->players[player].dashTimer = 6;
                    if (player == 0)
                        PlaySfx(149);
                } else if (ret == 2) {
                    gAirGrindPtr->players[player].dashTimer = 8;
                    if (player == 0)
                        PlaySfx(149);
                }
            }
            gAirGrindPtr->players[player].grinding = 1;
        } else {
            gAirGrindPtr->players[player].dashTimer = 0;
            AirGrindUpdateEngineSound(player, 0);
            AirGrindRacerSlowDown();
            gAirGrindPtr->players[player].penaltyTimer = 24;
            if (player == 0)
                AirGrindStartScript(3);
            CreateAirGrindEffect(player, gCurTask->unk18, 8);
            gAirGrindPtr->players[player].grinding = 0;
        }
        gAirGrindPtr->players[player].liftY -= 0x20000;
        if (gAirGrindPtr->players[player].liftY < 0)
            gAirGrindPtr->players[player].liftY = 0;
    } else {
        gAirGrindPtr->players[player].dashTimer = 0;
        AirGrindUpdateEngineSound(player, 0);
        if (gCurTask->velX < 0x18000) {
            gCurTask->accelX = 0;
            gCurTask->velX = 0x18000;
        } else {
            gCurTask->accelX = -0x800;
        }
        gAirGrindPtr->players[player].liftY += 0x20000;
        if (gAirGrindPtr->players[player].liftY > 0x80000)
            gAirGrindPtr->players[player].liftY = 0x80000;
        if (gAirGrindPtr->players[player].grinding == 1) {
            if (player == 0 && gAirGrindPtr->releaseEffectShown == 0)
                CreateAirGrindEffect(0, gCurTask->unk18, 5);
            if (gAirGrindPtr->players[player].boostCooldown == 0)
                AirGrindRacerTryBoost(player);
        }
        gAirGrindPtr->players[player].grinding = 0;
    }

    if (gAirGrindPtr->players[player].dashTimer != 0) {
        gAirGrindPtr->players[player].offsetX += 0x20000;
    } else {
        gAirGrindPtr->players[player].offsetX -= 0x20000;
        if (gAirGrindPtr->players[player].offsetX < 0)
            gAirGrindPtr->players[player].offsetX = 0;
    }
    if (gAirGrindPtr->players[player].segmentEndDistance < -20)
        gAirGrindPtr->players[player].segmentEnd = gAirGrindCoursePtr->players[player].segmentEnd;
    gAirGrindPtr->players[player].segmentEndDistance = gAirGrindPtr->players[player].segmentEnd - gAirGrindCoursePtr->players[player].coursePos;
    if (gAirGrindPtr->players[player].penaltyTimer == 0) {
        if (gCurTask->frame > 8)
            gAirGrindPtr->players[player].animStep = 0;
        if (gAirGrindCoursePtr->players[player].laneLean > 16)
            gCurTask->frame = 0;
        else if (gAirGrindCoursePtr->players[player].laneLean < -16)
            gCurTask->frame = 6;
        else
            gCurTask->frame = 3;
        if (gAirGrindPtr->players[player].heldKeys & 1) {
            if (gAirGrindPtr->players[player].animStep > 0)
                gAirGrindPtr->players[player].animStep--;
        } else {
            if (gAirGrindPtr->players[player].animStep <= 3)
                gAirGrindPtr->players[player].animStep++;
        }
        gCurTask->frame += gAirGrindRacerFrameSteps[gAirGrindPtr->players[player].animStep];
    } else {
        if (gCurTask->frame <= 8)
            gAirGrindPtr->players[player].animStep = 0;
        if (player == 0 && gAirGrindPtr->players[0].animStep == 0)
            PlaySfx(401);
        gCurTask->frame = 9;
        if (++gAirGrindPtr->players[player].animStep > 23)
            gAirGrindPtr->players[player].animStep = 0;
        gCurTask->frame += gUnk_080CFEE9[gAirGrindPtr->players[player].animStep];
        gAirGrindPtr->players[player].liftY = gUnk_080CFF01[gAirGrindPtr->players[player].animStep] << 16;
    }
}

void AirGrindRacerIdleStep(s32 player)
{
    AirGrindUpdateEngineSound(player, 0);
    if (gCurTask->state == 0) {
        gAirGrindPtr->players[player].liftY -= 0x20000;
        if (gAirGrindPtr->players[player].liftY < 0)
            gAirGrindPtr->players[player].liftY = 0;
    }
    if (gCurTask->frame > 8)
        gAirGrindPtr->players[player].animStep = 0;
    if (gAirGrindCoursePtr->players[player].laneLean > 16)
        gCurTask->frame = 0;
    else if (gAirGrindCoursePtr->players[player].laneLean < -16)
        gCurTask->frame = 6;
    else
        gCurTask->frame = 3;
    if (gCurTask->state == 0)
        gAirGrindPtr->players[player].animStep = 3;
    gCurTask->frame += gAirGrindRacerFrameSteps[gAirGrindPtr->players[player].animStep];
}

void AirGrindRacerUpdateScreenPos(s32 player)
{
    s32 scale;
    u8 pal;
    u16 ratio;

    scale = AirGrindGetDepthScale(gAirGrindCoursePtr->players[player].depth);
    gCurTask->pixelX = gAirGrindCoursePtr->players[player].screenX + ((scale * gAirGrindPtr->players[player].offsetX) >> 24);
    gCurTask->pixelY = gAirGrindCoursePtr->players[player].screenY - ((scale * gAirGrindPtr->players[player].liftY) >> 24);
    if (player != 0) {
        pal = gAirGrindLocalPlayerSlots[gAirGrindPtr->localPlayer][player];
        ratio = 0;
        if (scale < 256)
            ratio = 256 - scale;
        /* colour 1 of palette pal: the ROM scales the whole index
           (pal * 16 + 1) by 2, so the offset is written in bytes */
        BlendColors(gUnk_08609D42[pal], gUnk_080CFF1C, ratio, 15,
                     (u16 *)((u8 *)gObjPalette + (pal * 16 + 1) * 2));
    }
}

void AirGrindRacerRaceUpdate(void)
{
    s32 player = gCurTask->unk1C;

    AirGrindRacerUpdateDepth(player);
    AirGrindRacerRaceStep(player);
    AirGrindRacerUpdateScreenPos(player);
}

void AirGrindRacerIdleUpdate(void)
{
    s32 player = gCurTask->unk1C;

    AirGrindRacerUpdateDepth(player);
    AirGrindRacerIdleStep(player);
    AirGrindRacerUpdateScreenPos(player);
}
