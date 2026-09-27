#include "gba/gba.h"
#include "global.h"
#include "task.h"

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


/* ROM pointer tables: one entry per tile set, each pointing at a byte table. */
extern u8 *const gCollisionTileFloorSnap[];
extern u8 *const gCollisionTilePushDown[];
extern u8 *const gCollisionTilePushUp[];
extern u8 *const gCollisionTilePushRight[];
extern u8 *const gCollisionTilePushLeft[];
extern s8 *const gUnk_087330F0[];
extern s8 *const gCollisionTileShapes[];

/* ROM byte tables indexed by tile set. */
extern u8 gCollisionTileSlope[];
extern u8 gUnk_087337F0[];
extern u8 gCollisionTileSlippery[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_08732FF0[];
extern s8 gCollisionTileDoor[];
extern s8 gCollisionTileShapeClass[];
extern s8 gUnk_087338F0[];
extern u8 gUnk_08732DF0[];

/* IWRAM room descriptor cells. */
extern u16 gUnk_03005504;
extern s16 gTerrainPrevBoxLeft;
extern u16 gTerrainPixelIndex;
extern u16 gTerrainTileRight;
extern s32 gTerrainVelY;
extern s16 gTerrainPrevX;
extern s16 gTerrainPrevY;
extern s16 gTerrainBoxLeft;
extern s16 gTerrainProbeX;
extern u8 gTerrainFacing;
extern u16 gUnk_0300556C;
extern s16 gTerrainProbeY;
extern u16 gUnk_03005574;
extern u16 gTerrainTile;
extern s16 gTerrainBoxTop;
extern s16 gTerrainBoxBottom;
extern u16 gTerrainTileBelow;
extern s16 gTerrainPrevBoxRight;
extern u16 gTerrainTileLeft;
extern s16 gTerrainBoxRight;
extern s8 *gTerrainTileShape;
extern s16 gTerrainPrevBoxTop;
extern u16 gUnk_030055AC;
extern s16 gTerrainPrevBoxBottom;
extern s16 gRoomMetatileCount;
extern s32 gTerrainDriftY;
extern s32 gTerrainVelX;
extern s32 gTerrainDriftX;
extern s16 gRoomHeight;
extern s16 gRoomWidth;

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};
extern struct MapCell *gRoomMap;

extern s16 *gCurTileDrifts;

struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};
extern struct Unk03005530 gTerrainProbeResult;

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
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};
extern struct Unk03005550 gTerrainResult;

s32 TerrainQueryPixelAndSides(u32 x, u32 y);
void TerrainProbeBegin(const s8 *p);
void sub_080207a0(void);
void sub_0801c690(void);
void sub_0801c8dc(void);
void sub_0801d394(void);
void sub_0801dc88(void);
void sub_0801e178(void);
void sub_0801f540(void);
void sub_0801f800(void);
void sub_08022810(void);
void sub_0801fc48(void);
void sub_0801ff84(void);
void TerrainProbeWater(void);
void sub_080222b0(s16 x, s16 y);
s32 GetTilePushUp(u16 a);
s32 GetTilePushLeft(u16 a);

void TerrainProbeBegin(const s8 *p);
void TerrainProbeEnd(const s8 *p);
void sub_0801c690(void);
void sub_0801c7cc(void);
void sub_0801c8dc(void);
void sub_0801c930(void);
void sub_0801d394(void);
void sub_0801d9c8(void);
void sub_0801dc88(void);
void sub_0801dee8(void);
void sub_0801e178(void);
void sub_0801ecd0(void);
void sub_0801f540(void);
void sub_0801f6b0(void);
void sub_0801f800(void);
void sub_0801f9b8(void);
void sub_0801fc48(void);
void sub_0801fe2c(void);
void sub_0801ff84(void);
void sub_080214e0(void);
void sub_08021564(void);
void TerrainProbeWater(void);
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
