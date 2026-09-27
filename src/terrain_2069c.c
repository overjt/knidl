#include "gba/gba.h"
#include "global.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0802069C-0x080207A0.
 *
 * Tile-attribute lookups on the current room: the room descriptor cells at
 * 0x030055xx hold the map size (gRoomWidth x gRoomHeight cells of 16x16
 * pixels), the cell array pointer (gRoomMap, 4 bytes per cell, byte 3 is
 * the tile-set index) and the last query results; the 0x100-stride ROM index
 * tables at 0x087328F0.. map a tile-set index to its per-pixel attribute
 * table.  All query functions take pixel coordinates and return the signed
 * attribute byte for that pixel, or 0 when the coordinate is outside the map.
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

/* IWRAM room descriptor cells. */
extern u16 gUnk_03005504;
extern u16 gTerrainPixelIndex;
extern u16 gTerrainTileRight;
extern u16 gTerrainPrevX;
extern u16 gTerrainPrevY;
extern s16 gTerrainBoxLeft;
extern s16 gTerrainProbeX;
extern u16 gUnk_0300556C;
extern s16 gTerrainProbeY;
extern u16 gUnk_03005574;
extern u16 gTerrainTile;
extern s16 gTerrainBoxTop;
extern s16 gTerrainBoxBottom;
extern u16 gTerrainTileBelow;
extern u16 gTerrainTileLeft;
extern s16 gTerrainBoxRight;
extern s8 *gTerrainTileShape;
extern u16 gUnk_030055AC;
extern s16 gRoomMetatileCount;
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

struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
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
};
extern struct Unk03005530 gTerrainProbeResult;

s32 sub_080218f8(u32 x, u32 y);

s32 TerrainQueryPixelAndBelow(u32 x, u32 y);

u32 sub_0802069c(void)
{
    u32 result = 0;
    s8 v;

    if (TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY) == 0)
    {
        if (gUnk_087336F0[gTerrainTileBelow] != 0)
        {
            gTerrainProbeResult.unkB |= 1;
            if (gUnk_087336F0[gTerrainTile] != 0 && gCollisionTileSlope[gTerrainTile] != 0)
                gTerrainProbeResult.unkC = (u16)gTerrainProbeY >> 4;
            else
                gTerrainProbeResult.unkC = (gTerrainProbeY + 16) >> 4;
        }
        else
        {
            gTerrainProbeResult.unkB &= 0xFE;
        }
    }
    else
    {
        v = gUnk_087336F0[gTerrainTile];
        if (v == 0 || ((gTerrainProbeResult.unkB & 1) && gTerrainProbeResult.unkC <= gTerrainProbeY >> 4))
        {
            gTerrainProbeResult.unk2 = 1;
            gTerrainProbeResult.unk0 = 3;
            gTerrainProbeResult.unk1 = 1;
            gTerrainProbeResult.unk4 = gCollisionTileSlope[gTerrainTile];
            gTerrainProbeX = gTerrainPrevX;
            gTerrainProbeY = gTerrainPrevY;
            result = 1;
        }
    }
    return result;
}
