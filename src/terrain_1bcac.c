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
void sub_08022810(void);

void sub_08022810(void);

void sub_0801bcac(const s8 *p)
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
                sub_0801c7cc();
            else
                sub_0801c690();
        }
        sub_0801c8dc();
        sub_0801d394();
    }
    else
    {
        v = gTerrainVelX;
        if (v != 0)
        {
            if (v < 0)
                sub_0801dc88();
            else
                sub_0801d9c8();
        }
        sub_0801dee8();
        sub_0801ecd0();
    }
    TerrainProbeWater();
    TerrainProbeEnd(p);
}

void sub_0801bde0(const s8 *p)
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
    sub_08022810();
    if (gTerrainProbeResult.onGround != 0)
    {
        v = gTerrainVelX;
        if (v != 0)
        {
            if (v < 0)
                sub_0801c7cc();
            else
                sub_0801c690();
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
                sub_0801dc88();
            else
                sub_0801d9c8();
        }
        sub_0801dee8();
        sub_0801e178();
    }
    TerrainProbeWater();
    sub_08021564();
    TerrainProbeEnd(p);
}

void sub_0801bf1c(const s8 *p)
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

void sub_0801c030(const s8 *p)
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

void sub_0801c12c(const s8 *p)
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

void sub_0801c230(const s8 *p)
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
    sub_0801ff84();
    sub_080214e0();
    TerrainProbeEnd(p);
}
