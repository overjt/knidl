#include "gba/gba.h"
#include "global.h"

/*
 * M06 terrain / collision query (issue #84), range 0x080214E0-0x08021B18.
 *
 * Tile-attribute lookups on the current room: the room descriptor cells at
 * 0x030055xx hold the map size (gUnk_03005620 x gUnk_0300561C cells of 16x16
 * pixels), the cell array pointer (gUnk_03005660, 4 bytes per cell, byte 3 is
 * the tile-set index) and the last query results; the 0x100-stride ROM index
 * tables at 0x087328F0.. map a tile-set index to its per-pixel attribute
 * table.  All query functions take pixel coordinates and return the signed
 * attribute byte for that pixel, or 0 when the coordinate is outside the map.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork -fprologue-bugfix):
 *  - the helpers return int, not s8: callers compare the result without
 *    re-extending it (sub_0802069c, sub_0801c690); the sign extension comes
 *    from the s8 element type (lesson 3.289);
 *  - `p = table[i]; return p[j];` orders the pointer load before the index
 *    load; writing table[i][j] in one expression loads the index first;
 *  - the row/column cell access is `(&gUnk_03005660[idx])[x]` with
 *    `idx = y * w` in its own statement: a `row` local hoists the map base
 *    load above the multiply and shifts the whole register allocation;
 *  - sub_08021ab4 is sub_08021a40 without the attribute guard, but its second
 *    range check is written positively (the fail path sits before the pool).
 */


/* ROM pointer tables: one entry per tile set, each pointing at a byte table. */
extern s8 *const gUnk_08734BF0[];
extern s8 *const gUnk_08733BF0[];
extern s8 *const gUnk_08733FF0[];
extern s8 *const gUnk_087343F0[];
extern s8 *const gUnk_087347F0[];
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

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080218f8(u32 x, u32 y);

void sub_080214e0(void)
{
    s32 y;
    s32 h;

    sub_080216d8(gUnk_03005560, gUnk_03005570);
    gUnk_03005530.unk8 = 0xFFFF;
    gUnk_03005530.unk7 = 0;
    y = gUnk_03005570;
    h = gUnk_0300561C << 4;
    if (y >= h)
    {
        sub_08021634(gUnk_03005560, h - 16);
        if (gUnk_03005578 & 0x80)
            gUnk_03005530.unk7 = 11;
    }
    else
    {
        sub_08021634(gUnk_03005560, y);
        if (gUnk_03005578 > 127)
            gUnk_03005530.unk7 = 129;
        else
            gUnk_03005530.unk7 = 0;
    }
}

void sub_08021564(void)
{
    gUnk_03005530.unkF = 0;
    gUnk_03005530.unkF = sub_080218f8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_0300557C) | gUnk_03005530.unkF;
    gUnk_03005530.unkF = sub_080218f8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_0300557C) | gUnk_03005530.unkF;
    gUnk_03005530.unkF = sub_080218f8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) | gUnk_03005530.unkF;
    gUnk_03005530.unkF = sub_080218f8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) | gUnk_03005530.unkF;
}

s32 sub_08021634(u32 x, u32 y)
{
    s16 w;
    u32 idx;

    gUnk_03005508 = ((y & 15) << 4) + (x & 15);
    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x < w && y < gUnk_0300561C)
    {
        idx = y * w + x;
        gUnk_03005578 = gUnk_03005660[idx].unk3;
        gUnk_03005574 = gUnk_03005660[idx].unk2;
        gUnk_030055A0 = gUnk_087328F0[gUnk_03005578];
        return gUnk_030055A0[gUnk_03005508];
    }
    gUnk_03005508 = gUnk_03005578 = gUnk_03005574 = 0;
    return 0;
}

s32 sub_080216d8(u32 x, u32 y)
{
    s16 w;
    s32 idx;
    s8 *p;

    gUnk_03005508 = ((y & 15) << 4) + (x & 15);
    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x < w && y < gUnk_0300561C)
    {
        idx = y * w + x;
        if (idx + w <= gUnk_030055E4)
        {
            gUnk_03005588 = (&gUnk_03005660[idx])[w].unk3;
            gUnk_030055AC = (&gUnk_03005660[idx])[gUnk_03005620].unk2;
        }
        else
        {
            gUnk_03005588 = gUnk_030055AC = 0;
        }
        gUnk_03005578 = gUnk_03005660[idx].unk3;
        gUnk_03005574 = gUnk_03005660[idx].unk2;
        p = gUnk_087328F0[gUnk_03005578];
        return p[gUnk_03005508];
    }
    gUnk_03005508 = gUnk_03005578 = gUnk_03005574 = gUnk_03005588 = gUnk_030055AC = 0;
    return 0;
}

s32 sub_080217dc(u32 x, u32 y)
{
    s16 w;
    u32 idx;
    s8 *p;

    gUnk_03005508 = ((y & 15) << 4) + (x & 15);
    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x < w && y < gUnk_0300561C)
    {
        idx = y * w + x;
        gUnk_03005594 = (&gUnk_03005660[idx])[-1].unk3;
        gUnk_03005504 = (&gUnk_03005660[idx])[-1].unk2;
        if (x + 1 < gUnk_03005620)
        {
            gUnk_03005510 = (&gUnk_03005660[idx])[1].unk3;
            gUnk_0300556C = (&gUnk_03005660[idx])[1].unk2;
        }
        else
        {
            gUnk_03005510 = gUnk_0300556C = 0;
        }
        gUnk_03005578 = gUnk_03005660[idx].unk3;
        gUnk_03005574 = gUnk_03005660[idx].unk2;
        p = gUnk_087328F0[gUnk_03005578];
        return p[gUnk_03005508];
    }
    gUnk_03005508 = gUnk_03005578 = gUnk_03005574 = gUnk_03005594 = gUnk_03005504 = gUnk_03005510 = gUnk_0300556C = 0;
    return 0;
}

s32 sub_080218f8(u32 x, u32 y)
{
    u32 off = ((y & 15) << 4) + (x & 15);
    s16 w;
    struct MapCell *row;
    u8 tile;
    u32 idx;

    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x >= w)
        return 0;
    if (y >= gUnk_0300561C)
        return 0;
    idx = y * w;
    tile = (&gUnk_03005660[idx])[x].unk3;
    if (gUnk_08732FF0[tile] == 0)
        return 0;
    return gUnk_087330F0[tile][off];
}

s32 sub_08021970(u16 a)
{
    s8 *p = gUnk_08734BF0[a];
    return p[gUnk_03005508];
}

s32 sub_08021990(u16 a)
{
    s8 *p = gUnk_08733BF0[a];
    return p[gUnk_03005508];
}

s32 sub_080219b0(u16 a)
{
    s8 *p = gUnk_08733FF0[a];
    return p[gUnk_03005508];
}

s32 sub_080219d0(u16 a)
{
    s8 *p = gUnk_087343F0[a];
    return p[gUnk_03005508];
}

s32 sub_080219f0(u16 a)
{
    s8 *p = gUnk_087347F0[a];
    return p[gUnk_03005508];
}

void sub_08021a10(u16 a)
{
    gUnk_03005530.unk4 = gUnk_08732CF0[a];
    gUnk_03005530.unk5 = gUnk_087337F0[a];
    gUnk_03005530.unkE = gUnk_087334F0[a];
}

s32 sub_08021a40(u32 x, u32 y)
{
    u32 cx = x >> 4;
    u32 cy;
    s16 w = gUnk_03005620;
    u8 tile;
    u32 idx;
    s8 *p;

    if (cx >= w)
        return 0;
    cy = y >> 4;
    if (cy >= gUnk_0300561C)
        return 0;
    idx = cy * w;
    tile = (&gUnk_03005660[idx])[cx].unk3;
    if (gUnk_087336F0[tile] != 0)
        return 0;
    p = gUnk_087328F0[tile];
    return p[((y & 15) << 4) + (x & 15)];
}

s32 sub_08021ab4(u32 x, u32 y)
{
    u32 cx = x >> 4;
    u32 cy;
    s16 w = gUnk_03005620;
    u8 tile;
    u32 idx;
    s8 *p;

    if (cx >= w)
        return 0;
    cy = y >> 4;
    if (cy < gUnk_0300561C)
    {
        idx = cy * w;
        tile = (&gUnk_03005660[idx])[cx].unk3;
        p = gUnk_087328F0[tile];
        return p[((y & 15) << 4) + (x & 15)];
    }
    return 0;
}
