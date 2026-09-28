#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "room.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801BCAC-0x0801C30C.
 *
 * Six per-frame entry points sharing one body: load the actor's terrain box
 * (TerrainProbeBegin), derive the actor's room-relative position from
 * Task.posX/unk50 (16.16 fixed point) and the room origin in
 * Task.velX/unk58, form the box corners in gTerrainPrevBoxLeft/gTerrainPrevBoxRight
 * (x) and gTerrainPrevBoxTop/gTerrainPrevBoxBottom (y), then dispatch the probe set by
 * the actor's movement direction (the sign of gTerrainVelX) and finish with
 * the room probe TerrainProbeWater and the write-back TerrainProbeEnd.
 *
 * Matching note: the corner sums read gTerrainPrevX/gTerrainPrevY back
 * rather than keeping the position in a local - cse folds the re-read into
 * the register just stored and that is the only spelling that keeps the
 * position as the FIRST operand of the narrowed adds (lesson 3.359).
 */

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void TerrainClampBoxToCameraBounds(void);

void TerrainClampBoxToCameraBounds(void);

void TerrainCollideBox(const s8 *p)
{
    s32 v;

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    if (gTerrainProbeResult.unkB & 0x80)
    {
        gTerrainProbeResult.unkB = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom) >> 4;
    }
    if (gTerrainProbeResult.onGround != 0)
    {
        v = gTerrainVelX;
        if (v != 0)
        {
            if (v < 0)
                TerrainProbeWallLeftOnGround();
            else
                TerrainProbeWallRightOnGround();
        }
        sub_0801c8dc();
        TerrainProbeFloor();
    }
    else
    {
        v = gTerrainVelX;
        if (v != 0)
        {
            if (v < 0)
                TerrainProbeWallLeftInAir();
            else
                TerrainProbeWallRightInAir();
        }
        TerrainProbeCeiling();
        TerrainProbeLanding();
    }
    TerrainProbeWater();
    TerrainProbeEnd(p);
}

void TerrainCollideBoxInCameraBounds(const s8 *p)
{
    s32 v;

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    if (gTerrainProbeResult.unkB & 0x80)
    {
        gTerrainProbeResult.unkB = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom) >> 4;
    }
    TerrainClampBoxToCameraBounds();
    if (gTerrainProbeResult.onGround != 0)
    {
        v = gTerrainVelX;
        if (v != 0)
        {
            if (v < 0)
                TerrainProbeWallLeftOnGround();
            else
                TerrainProbeWallRightOnGround();
        }
        sub_0801c8dc();
        sub_0801c930();
    }
    else
    {
        v = gTerrainVelX;
        if (v != 0)
        {
            if (v < 0)
                TerrainProbeWallLeftInAir();
            else
                TerrainProbeWallRightInAir();
        }
        TerrainProbeCeiling();
        sub_0801e178();
    }
    TerrainProbeWater();
    TerrainProbeDamage();
    TerrainProbeEnd(p);
}

void TerrainCollideBoxWalls(const s8 *p)
{
    s32 v;

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    if (gTerrainProbeResult.unkB & 0x80)
    {
        gTerrainProbeResult.unkB = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
    v = gTerrainVelX;
    if (v != 0)
    {
        if (v < 0)
            sub_0801f6b0();
        else
            sub_0801f540();
    }
    TerrainProbeWater();
    TerrainProbeEnd(p);
}

void TerrainCollideBoxCeilingAndFloor(const s8 *p)
{
    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    if (gTerrainProbeResult.unkB & 0x80)
    {
        gTerrainProbeResult.unkB = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
    sub_0801f800();
    sub_0801f9b8();
    TerrainProbeWater();
    TerrainProbeEnd(p);
}

void TerrainCollideBoxFloor(const s8 *p)
{
    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    if (gTerrainProbeResult.unkB & 0x80)
    {
        gTerrainProbeResult.unkB = 1;
        gTerrainProbeResult.unkC = (gTerrainProbeY + gTerrainBoxBottom + 16) >> 4;
    }
    if (gTerrainProbeResult.onGround != 0)
        sub_0801fc48();
    else
        sub_0801fe2c();
    TerrainProbeWater();
    TerrainProbeEnd(p);
}

void TerrainCollideBoxAlongVelocity(const s8 *p)
{
    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->velX;
    gTerrainVelY = gCurTask->velY;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    gTerrainPrevBoxLeft = gTerrainPrevX + gTerrainBoxLeft;
    gTerrainPrevBoxRight = gTerrainPrevX + gTerrainBoxRight;
    gTerrainPrevBoxTop = gTerrainPrevY + gTerrainBoxTop;
    gTerrainPrevBoxBottom = gTerrainPrevY + gTerrainBoxBottom;
    if (gTerrainProbeResult.unkB & 0x80)
        gTerrainProbeResult.unkB = 0;
    TerrainProbeAlongVelocity();
    TerrainProbeWaterAtPoint();
    TerrainProbeEnd(p);
}
