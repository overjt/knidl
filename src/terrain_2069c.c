#include "gba/gba.h"
#include "global.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0802069C-0x080207A0.
 *
 * Tile-attribute lookups on the current room: the room descriptor cells at
 * 0x030055xx hold the map size (gUnk_03005620 x gUnk_0300561C cells of 16x16
 * pixels), the cell array pointer (gUnk_03005660, 4 bytes per cell, byte 3 is
 * the tile-set index) and the last query results; the 0x100-stride ROM index
 * tables at 0x087328F0.. map a tile-set index to its per-pixel attribute
 * table.  All query functions take pixel coordinates and return the signed
 * attribute byte for that pixel, or 0 when the coordinate is outside the map.
 */


/* ROM pointer tables: one entry per tile set, each pointing at a byte table. */
extern u8 *const gUnk_08734BF0[];
extern u8 *const gUnk_08733BF0[];
extern u8 *const gUnk_08733FF0[];
extern u8 *const gUnk_087343F0[];
extern u8 *const gUnk_087347F0[];
extern s8 *const gUnk_087330F0[];
extern s8 *const gUnk_087328F0[];

/* ROM byte tables indexed by tile set. */
extern u8 gUnk_08732CF0[];
extern u8 gUnk_087337F0[];
extern u8 gUnk_087334F0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_08732FF0[];

/* IWRAM room descriptor cells. */
extern u16 gUnk_03005504;
extern u16 gUnk_03005508;
extern u16 gUnk_03005510;
extern u16 gUnk_03005518;
extern u16 gUnk_03005520;
extern s16 gUnk_0300551C;
extern s16 gUnk_03005560;
extern u16 gUnk_0300556C;
extern s16 gUnk_03005570;
extern u16 gUnk_03005574;
extern u16 gUnk_03005578;
extern s16 gUnk_0300557C;
extern s16 gUnk_03005584;
extern u16 gUnk_03005588;
extern u16 gUnk_03005594;
extern s16 gUnk_0300559C;
extern s8 *gUnk_030055A0;
extern u16 gUnk_030055AC;
extern s16 gUnk_030055E4;
extern s16 gUnk_0300561C;
extern s16 gUnk_03005620;

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};
extern struct MapCell *gUnk_03005660;

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
};
extern struct Unk03005530 gUnk_03005530;

s32 sub_080218f8(u32 x, u32 y);

s32 sub_080216d8(u32 x, u32 y);

u32 sub_0802069c(void)
{
    u32 result = 0;
    s8 v;

    if (sub_080216d8(gUnk_03005560, gUnk_03005570) == 0)
    {
        if (gUnk_087336F0[gUnk_03005588] != 0)
        {
            gUnk_03005530.unkB |= 1;
            if (gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0)
                gUnk_03005530.unkC = (u16)gUnk_03005570 >> 4;
            else
                gUnk_03005530.unkC = (gUnk_03005570 + 16) >> 4;
        }
        else
        {
            gUnk_03005530.unkB &= 0xFE;
        }
    }
    else
    {
        v = gUnk_087336F0[gUnk_03005578];
        if (v == 0 || ((gUnk_03005530.unkB & 1) && gUnk_03005530.unkC <= gUnk_03005570 >> 4))
        {
            gUnk_03005530.unk2 = 1;
            gUnk_03005530.unk0 = 3;
            gUnk_03005530.unk1 = 1;
            gUnk_03005530.unk4 = gUnk_08732CF0[gUnk_03005578];
            gUnk_03005560 = gUnk_03005518;
            gUnk_03005570 = gUnk_03005520;
            result = 1;
        }
    }
    return result;
}
