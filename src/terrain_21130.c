#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x08021130-0x0802136C.
 *
 * The room probe for the player: sub_08021130 is TerrainProbeWater (still in asm)
 * with a tile-set special case in front (tile sets 64..79 and 192..207 pick a
 * pair of signed offsets from the gUnk_0300558C table into gUnk_030055A8 /
 * gUnk_03005580) and the gUnk_08733AF0 attribute copied to unkA behind.
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
        gUnk_030055A8 = gUnk_0300558C[(tile - 48) * 2];
        gUnk_03005580 = gUnk_0300558C[(gTerrainTile - 48) * 2 + 1];
    }
    else
    {
        t = tile - 192;
        if (t <= 15)
        {
            gUnk_030055A8 = gUnk_0300558C[(tile - 192) * 2];
            gUnk_03005580 = gUnk_0300558C[(gTerrainTile - 192) * 2 + 1];
        }
    }
    prev = gTerrainProbeResult.unk7;
    prev2 = prev;
    gTerrainProbeResult.unk8 = 0xFFFF;
    gTerrainProbeResult.unk7 = 0;
    zero = 0;
    y = gTerrainProbeY;
    h = gRoomHeight << 4;
    if (y >= h)
    {
        TerrainQueryPixel(gTerrainProbeX, h - 16);
        if (gTerrainTile & 0x80)
            gTerrainProbeResult.unk7 = 11;
    }
    else
    {
        f = gTerrainTile & 0x80;
        if (f)
        {
            gTerrainProbeResult.unk7 = 1;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxTop);
            if (gTerrainTile & 0x80)
            {
                v = gTerrainProbeResult.unk7 | 2 | zero;
                gTerrainProbeResult.unk7 = v;
                if ((prev & 2) == 0)
                {
                    gTerrainProbeResult.unk7 = v | 0x80;
                    gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxTop) & 0xFFF0;
                }
                else
                {
                    gTerrainProbeResult.unk7 = v | 8;
                }
            }
            else
            {
                gTerrainProbeResult.unk7 |= 0x48;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxTop) & 0xFFF0) + 16;
            }
        }
        else
        {
            gTerrainProbeResult.unk7 = f;
            TerrainQueryPixel(gTerrainProbeX, gTerrainProbeY + gTerrainBoxBottom);
            if (gTerrainTile & 0x80)
            {
                gTerrainProbeResult.unk7 |= 0x48;
                gTerrainProbeResult.unk8 = (gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0;
            }
            else if (prev2 & 8)
            {
                gTerrainProbeResult.unk7 |= 0x80;
                gTerrainProbeResult.unk8 = ((gTerrainProbeY + gTerrainBoxBottom) & 0xFFF0) + 16;
            }
        }
    }
    gTerrainProbeResult.unkA = 0;
    if (gUnk_08733AF0[tile] != 0)
        gTerrainProbeResult.unkA = 1;
}
