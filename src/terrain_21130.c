#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x08021130-0x0802136C.
 *
 * The room probe for the player: sub_08021130 is TerrainProbeWater (still in asm)
 * with a tile-set special case in front (tile sets 64..79 and 192..207 pick a
 * pair of signed offsets from the gCurTileDrifts table into gTerrainDriftX /
 * gTerrainDriftY) and the gCollisionTileDoor attribute copied to unkA behind.
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

s32 TerrainQueryPixel(u32 x, u32 y);
s32 TerrainQueryPixelAndBelow(u32 x, u32 y);

void sub_08021130(void)
{
    u8 prev;
    u8 prev2;
    u32 zero;
    s32 y;
    s32 h;
    u16 f;
    u8 v;
    u16 tile;
    u16 t;

    TerrainQueryPixelAndBelow(gTerrainProbeX, gTerrainProbeY);
    tile = gTerrainTile;
    t = tile - 64;
    if (t <= 15)
    {
        gTerrainDriftX = gCurTileDrifts[(tile - 48) * 2];
        gTerrainDriftY = gCurTileDrifts[(gTerrainTile - 48) * 2 + 1];
    }
    else
    {
        t = tile - 192;
        if (t <= 15)
        {
            gTerrainDriftX = gCurTileDrifts[(tile - 192) * 2];
            gTerrainDriftY = gCurTileDrifts[(gTerrainTile - 192) * 2 + 1];
        }
    }
    prev = gTerrainProbeResult.waterFlags;
    prev2 = prev;
    gTerrainProbeResult.unk8 = 0xFFFF;
    gTerrainProbeResult.waterFlags = 0;
    zero = 0;
    y = gTerrainProbeY;
    h = gRoomHeight << 4;
    if (y >= h)
    {
        TerrainQueryPixel(gTerrainProbeX, h - 16);
        if (gTerrainTile & 0x80)
            gTerrainProbeResult.waterFlags = 11;
    }
    else
    {
        f = gTerrainTile & 0x80;
        if (f)
        {
            gTerrainProbeResult.waterFlags = 1;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop);
            if (gTerrainTile & 0x80)
            {
                v = gTerrainProbeResult.waterFlags | 2 | zero;
                gTerrainProbeResult.waterFlags = v;
                if ((prev & 2) == 0)
                {
                    gTerrainProbeResult.waterFlags = v | 0x80;
                    gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxTop) & 0xFFF0;
                }
                else
                {
                    gTerrainProbeResult.waterFlags = v | 8;
                }
            }
            else
            {
                gTerrainProbeResult.waterFlags |= 0x48;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0) + 16;
            }
        }
        else
        {
            gTerrainProbeResult.waterFlags = f;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
            if (gTerrainTile & 0x80)
            {
                gTerrainProbeResult.waterFlags |= 0x48;
                gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0;
            }
            else if (prev2 & 8)
            {
                gTerrainProbeResult.waterFlags |= 0x80;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0) + 16;
            }
        }
    }
    gTerrainProbeResult.atDoor = 0;
    if (gCollisionTileDoor[tile] != 0)
        gTerrainProbeResult.atDoor = 1;
}
