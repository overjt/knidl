#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C51C-0x0801C690.
 *
 * TerrainProbeBegin copies the actor's six signed box offsets and the task fields
 * into the room-descriptor cells and clears the probe result block at
 * gTerrainProbeResult; TerrainProbeEnd writes the probe results back into the task
 * (re-seating Task.posX/posY when the probe moved the actor) and mirrors the
 * result block into gTerrainResult.
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
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 onGround;
    /*0x07*/ u8 waterFlags;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 onSlipperyFloor;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};
extern struct Unk03005530 gTerrainProbeResult;

struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 onSlipperyFloor;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};
extern struct Unk03005550 gTerrainResult;

s32 TerrainQueryPixelAndSides(u32 x, u32 y);
void TerrainProbeBegin(const s8 *p);
void sub_080207a0(void);
void sub_080214e0(void);
u32 sub_0802069c(void);
s32 GetTilePushDown(u16 a);
s32 GetTilePushRight(u16 a);
s32 TerrainQueryPixel(u32 x, u32 y);

void TerrainProbeBegin(const s8 *p)
{
    gTerrainProbeX = (gCurTask->posX >> 16) + p[0];
    gTerrainProbeY = (gCurTask->posY >> 16) + p[1];
    gTerrainBoxTop = p[2];
    gTerrainBoxBottom = p[3];
    gTerrainBoxLeft = p[4];
    gTerrainBoxRight = p[5];
    gTerrainFacing = gCurTask->facing;
    gTerrainProbeResult.unk0 = gTerrainProbeResult.ceilingHits = gTerrainProbeResult.unk2 = gTerrainProbeResult.unk3 = gTerrainProbeResult.slope = gTerrainProbeResult.unk5 = gTerrainProbeResult.onSlipperyFloor = gTerrainProbeResult.unkF = gTerrainProbeResult.unk10 = 0;
    gTerrainProbeResult.onGround = gCurTask->onGround;
    gTerrainProbeResult.waterFlags = gCurTask->waterFlags;
    gTerrainProbeResult.unkB = gCurTask->unk84;
    gTerrainProbeResult.unkC = gCurTask->unk84 >> 8;
}

void TerrainProbeEnd(const s8 *p)
{
    gCurTask->onGround = gTerrainProbeResult.onGround;
    gCurTask->waterFlags = gTerrainProbeResult.waterFlags;
    if (gCurTask->posX >> 16 != gTerrainProbeX - p[0])
    {
        gCurTask->posX = ((gTerrainProbeX - p[0]) << 16) + 0x8000;
        gCurTask->pixelX = gTerrainProbeX - p[0];
    }
    if (gCurTask->posY >> 16 != gTerrainProbeY - p[1])
    {
        gCurTask->posY = ((gTerrainProbeY - p[1]) << 16) + 0x8000;
        gCurTask->pixelY = gTerrainProbeY - p[1];
    }
    gTerrainResult.unk0 = gTerrainProbeResult.unk0;
    gTerrainResult.ceilingHits = gTerrainProbeResult.ceilingHits;
    gTerrainResult.unk2 = gTerrainProbeResult.unk2;
    gTerrainResult.unk3 = gTerrainProbeResult.unk3;
    gTerrainResult.slope = gTerrainProbeResult.slope;
    gTerrainResult.unk5 = gTerrainProbeResult.unk5;
    gTerrainResult.unk8 = gTerrainProbeResult.unk8;
    gTerrainResult.onSlipperyFloor = gTerrainProbeResult.onSlipperyFloor;
    gTerrainResult.unkC = gTerrainProbeResult.unkF;
    gTerrainResult.unkD = gTerrainProbeResult.unk10;
    gCurTask->unk84 = (gTerrainProbeResult.unkC << 8) | gTerrainProbeResult.unkB;
}
