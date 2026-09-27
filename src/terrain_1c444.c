#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C444-0x0801C51C.
 *
 * sub_0801c444: the same entry-point body as src/terrain_1bcac.c, clearing
 * the pending-collision flag and running the tile-edge probe sub_08020b38.
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
extern s16 gUnk_0300550C;
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
extern s16 gUnk_03005590;
extern u16 gTerrainTileLeft;
extern s16 gTerrainBoxRight;
extern s8 *gTerrainTileShape;
extern s16 gUnk_030055A4;
extern u16 gUnk_030055AC;
extern s16 gUnk_030055B0;
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
void sub_08020b38(void);

void sub_0801c444(const s8 *p)
{
    TerrainProbeBegin(p);
    if (gTerrainProbeResult.unkB & 0x80)
        gTerrainProbeResult.unkB = 0;
    gTerrainVelX = gCurTask->unk54;
    gTerrainVelY = gCurTask->unk58;
    gTerrainPrevX = ((gTerrainProbeX << 16) + (gCurTask->unk4C & 0xFFFF) - gTerrainVelX) >> 16;
    gTerrainPrevY = ((gTerrainProbeY << 16) + (gCurTask->unk50 & 0xFFFF) - gTerrainVelY) >> 16;
    gUnk_0300550C = gTerrainPrevX + gTerrainBoxLeft;
    gUnk_03005590 = gTerrainPrevX + gTerrainBoxRight;
    gUnk_030055A4 = gTerrainPrevY + gTerrainBoxTop;
    gUnk_030055B0 = gTerrainPrevY + gTerrainBoxBottom;
    sub_08020b38();
    TerrainProbeEnd(p);
}
