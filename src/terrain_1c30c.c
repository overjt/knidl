#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C30C-0x0801C444.
 *
 * Two per-frame entry points that load the actor's terrain box (TerrainProbeBegin),
 * derive the actor's position relative to the room from Task.posX/unk50 and
 * Task.unk54/unk58, run the probes and write the results back (TerrainProbeEnd).
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
extern u8 gUnk_087334F0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_08732FF0[];
extern s8 gUnk_08733AF0[];
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
extern s32 gUnk_03005580;
extern s32 gTerrainVelX;
extern s32 gUnk_030055A8;
extern s16 gRoomHeight;
extern s16 gRoomWidth;

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};
extern struct MapCell *gRoomMap;

extern s16 *gUnk_0300558C;

struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
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
    /*0x04*/ u8 unk4;
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
void sub_080214e0(void);
u32 sub_0802069c(void);
s32 GetTilePushDown(u16 a);
s32 GetTilePushRight(u16 a);
s32 TerrainQueryPixel(u32 x, u32 y);

void TerrainProbeBegin(const s8 *p);
void TerrainProbeEnd(const s8 *p);
void sub_080207a0(void);
void sub_080214e0(void);
void TerrainProbeWater(void);
u32 sub_0802069c(void);
void sub_080222b0(s16 x, s16 y);

void sub_0801c30c(const s8 *p)
{

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->unk54;
    gTerrainVelY = gCurTask->unk58;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    if (gTerrainProbeResult.unkB & 0x80)
        sub_080222b0(gTerrainPrevX, gTerrainPrevY);
    sub_080207a0();
    sub_080214e0();
    TerrainProbeEnd(p);
}

u16 sub_0801c3a4(const s8 *p)
{
    u16 r;

    TerrainProbeBegin(p);
    gTerrainVelX = gCurTask->unk54;
    gTerrainVelY = gCurTask->unk58;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->posX & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->posY & 0xFFFF) - gTerrainVelY) >> 16;
    if (gTerrainProbeResult.unkB & 0x80)
        sub_080222b0(gTerrainPrevX, gTerrainPrevY);
    r = sub_0802069c();
    TerrainProbeWater();
    TerrainProbeEnd(p);
    return r;
}
