#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_21b18.c (0x08021B18-0x0802296B, issue #93).
 *
 * Map and collision queries, the continuation of M06's terrain probes
 * (src/terrain_*.c).  gUnk_03005660 is the room's metatile map,
 * gUnk_03005620 x gUnk_0300561C cells of 16x16 pixels.  sub_08021b18,
 * sub_08021b2c, sub_08021b70 and sub_08021bb4 read a cell's tile-set byte
 * (or unk2) at pixel or metatile coordinates; sub_08021c14, sub_08021c4c
 * and sub_08022624 test a pixel for a wall through M06's sub_08021634 and
 * the per-tile-set tables; sub_08022540 and sub_0802259c read the second
 * map layer gUnk_02008160 and a pixel's attribute.  sub_08021c74 resolves
 * a task's hit box against the floor and slopes (position
 * gUnk_03005560/gUnk_03005570, box offsets gUnk_0300557C/84/1C/9C, results
 * in gUnk_03005530) and writes the corrected position back;
 * sub_0802205c and sub_0802233c do the same for walls, sub_080222b0 probes
 * the ground and sub_080224b0/sub_080224f8 set a task's in-wall state
 * (Task.unk7B).  The rest clamp a body or a task to the per-player bounds
 * gUnk_03005640, the camera bounds gUnk_030055F8 or the room bounds
 * gUnk_03005628 and return which edges were hit. */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

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

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

struct RoomDef
{
    /*0x00*/ u8 filler00[4];
    /*0x04*/ s8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 *unk18;
    /*0x1C*/ void *unk1C;
    /*0x20*/ void *unk20;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u16 *unk28;
    /*0x2C*/ void *unk2C;
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u16 unk34;
    /*0x36*/ u16 unk36;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 unk3A;
    /*0x3C*/ u16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *unk44;
    /*0x48*/ void *unk48;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 unk50;
    /*0x52*/ u16 unk52;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct CamRect { s16 x0, x1, y0, y1; };

extern struct MapCell *gUnk_03005660;
extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern u16 gUnk_03005578;
extern s8 gUnk_087339F0[];
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03005560;
extern s16 gUnk_03005570;
extern s16 gUnk_0300557C;
extern s16 gUnk_03005584;
extern s16 gUnk_0300551C;
extern s16 gUnk_0300559C;
extern s8 gUnk_08732EF0[];
extern u16 gUnk_03005574;
extern u16 gUnk_03005588;
extern u16 gUnk_030055AC;
extern u16 gUnk_03005508;
extern u16 gUnk_08735018[];
extern s8 gUnk_087336F0[];
extern u8 gUnk_08732CF0[];
extern u16 gUnk_03005510;
extern u8 gUnk_08732DF0[];
extern u16 gUnk_03005594;
extern u16 gUnk_02008160[];
extern struct RoomDef *gUnk_030055EC;
extern s8 *const gUnk_087328F0[];
extern s8 gUnk_087335F0[];
extern u8 gUnk_03005568;
extern u16 gUnk_03005544;
extern struct CamRect gUnk_03005640[4];
extern u8 gUnk_02005574[];
extern s16 gUnk_03005628[4];
extern s16 gUnk_030055F8[4];

s32 sub_08021634(u32 x, u32 y);
s32 sub_080216d8(u32 x, u32 y);
s32 sub_080217dc(u32 x, u32 y);
s32 sub_08021970(u16 a);
s32 sub_08021b2c(u32 x, u32 y);

s32 sub_08021b18(u16 x, u16 y)
{
    return sub_08021b2c(x >> 4, y >> 4);
}

s32 sub_08021b2c(u32 x, u32 y)
{
    s16 w = gUnk_03005620;
    u32 idx;

    if (x >= w || y >= gUnk_0300561C)
        return 0;
    idx = y * w;
    return (&gUnk_03005660[idx])[x].unk3;
}

s32 sub_08021b70(u32 x, u32 y)
{
    s16 w;
    u32 idx;

    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x >= w || y >= gUnk_0300561C)
        return 0;
    idx = y * w;
    return (&gUnk_03005660[idx])[x].unk2;
}

s32 sub_08021bb4(s16 x, s16 y, s16 dx, s16 dy)
{
    s32 cx = (x >> 4) + dx;
    s32 cy = (y >> 4) + dy;
    s32 w;
    s32 idx;

    if (cx <= 0 || cx >= (w = gUnk_03005620) - 1 || cy <= 0 || cy >= gUnk_0300561C - 1)
        return -1;
    idx = cy * w;
    return (&gUnk_03005660[idx])[cx].unk3;
}

u16 sub_08021c14(s16 x, s16 y)
{
    sub_08021634(x, y);
    if (gUnk_03005578 <= 127 && gUnk_087339F0[gUnk_03005578] == 0)
        return 0;
    return 1;
}

u8 sub_08021c4c(s16 x, s16 y)
{
    sub_08021634(x, y);
    if (gUnk_03005578 > 127)
        return 1;
    return 0;
}

void sub_08021c74(s8 *box, s32 id)
{
    struct Task *t = &gUnk_03002790[id];
    s32 tile;
    s32 flags;
    s32 n;

    gUnk_03005560 = (t->unk4C >> 16) + box[0];
    gUnk_03005570 = (t->unk50 >> 16) + box[1];
    gUnk_0300557C = box[2];
    gUnk_03005584 = box[3];
    gUnk_0300551C = box[4];
    gUnk_0300559C = box[5];
    gUnk_03005530.unk2++;
    gUnk_03005530.unk6 = 1;
    sub_080216d8(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
    if (gUnk_08732EF0[gUnk_03005578] == 0)
    {
        tile = gUnk_08735018[gUnk_03005574];
        if (tile != 0)
        {
            flags = 0;
            if (gUnk_03005574 & 1)
            {
                if ((gUnk_03005560 & 0xFFF0) != ((gUnk_03005560 + gUnk_0300551C) & 0xFFF0))
                    flags = 1;
            }
            else
            {
                if ((gUnk_03005560 & 0xFFF0) != ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0))
                    flags = 2;
            }
            if (flags != 0 || gUnk_03005508 + 16 <= 255)
                goto slope;
        }
        if (gUnk_08732EF0[gUnk_03005588] == 0)
        {
            tile = gUnk_08735018[gUnk_030055AC];
            if (tile == 0)
                goto edges;
            if (gUnk_030055AC & 1)
            {
                if ((gUnk_03005560 & 0xFFF0) != ((gUnk_03005560 + gUnk_0300551C) & 0xFFF0))
                    flags = 1;
            }
            else
            {
                if ((gUnk_03005560 & 0xFFF0) != ((gUnk_03005560 + gUnk_0300559C) & 0xFFF0))
                    flags = 2;
            }
            if (flags == 0)
                goto edges;
            goto slope;
        }
        gUnk_03005570 += sub_08021970(gUnk_03005588) + 16;
    }
    else
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
    }
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584) != 0)
        gUnk_03005570 += sub_08021970(gUnk_03005578);
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    goto done;

slope:
    /* Two identical arms: jump2 cross-jumps them into the ROM's
       `cmp #1; beq; cmp #2; bne` (one `flags == 1 || flags == 2` test
       folds to a `subs; cmp #1; bhi` range check). */
    if (flags == 1)
        gUnk_03005570 += sub_08021970(tile);
    else if (flags == 2)
        gUnk_03005570 += sub_08021970(tile);
    if (sub_08021634(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) == 0)
        flags &= ~1;
    if (sub_08021634(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) == 0)
        flags &= ~2;
    if (flags == 0)
        goto clear;
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    goto done;

edges:
    n = 0;
    if (sub_080216d8(gUnk_03005560 + gUnk_0300551C, gUnk_03005570 + gUnk_03005584) != 0)
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        n = 2;
    }
    if (sub_080216d8(gUnk_03005560 + gUnk_0300559C, gUnk_03005570 + gUnk_03005584) != 0)
    {
        gUnk_03005570 += sub_08021970(gUnk_03005578);
        n++;
    }
    if (n == 0)
        goto clear;
    gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 1) >> 4;
    goto done;

clear:
    gUnk_03005530.unk6 = 0;
    gUnk_03005530.unk2 = 0;
    gUnk_03005530.unkB = 0;

done:
    t->unk7A = gUnk_03005530.unk6;
    t->unk84 = (gUnk_03005530.unkC << 8) | gUnk_03005530.unkB;
    n = gUnk_03005560 - box[0];
    if ((t->unk4C >> 16) != n)
    {
        t->unk4C = (n << 16) + 0x8000;
        t->unk48 = n;
    }
    n = gUnk_03005570 - box[1];
    if ((t->unk50 >> 16) != n)
    {
        t->unk50 = (n << 16) + 0x8000;
        t->unk4A = n;
    }
}

void sub_0802205c(s8 *box)
{
    gUnk_03005560 = (gUnk_03002490->unk4C >> 16) + box[0];
    gUnk_03005570 = (gUnk_03002490->unk50 >> 16) + box[1];
    gUnk_0300557C = box[2];
    gUnk_03005584 = box[3];
    gUnk_0300551C = box[4];
    gUnk_0300559C = box[5];
    gUnk_03005530.unkB = 0;
    if (gUnk_03002490->unk43 != -1)
    {
        if (sub_080217dc(gUnk_03005560 + gUnk_0300559C, gUnk_03005570) == 0)
        {
            if ((gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0
                 && (gUnk_08732CF0[gUnk_03005578] & 1))
                || (gUnk_087336F0[gUnk_03005510] != 0 && gUnk_08732CF0[gUnk_03005510] != 0
                    && (gUnk_08732CF0[gUnk_03005510] & 1)
                    && ((gUnk_08732DF0[gUnk_03005510] & 0xCF) != 0x83 || (gUnk_03005570 & 15) <= 7)))
                gUnk_03005530.unkB |= 4;
            else
                goto end;
        }
    }
    else
    {
        if (sub_080217dc(gUnk_03005560 + gUnk_0300551C, gUnk_03005570) == 0)
        {
            if ((gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0
                 && !(gUnk_08732CF0[gUnk_03005578] & 1))
                || (gUnk_087336F0[gUnk_03005594] != 0 && gUnk_08732CF0[gUnk_03005594] != 0
                    && !(gUnk_08732CF0[gUnk_03005594] & 1)
                    && ((gUnk_08732DF0[gUnk_03005594] & 0xCF) != 0x83 || (gUnk_03005570 & 15) <= 7)))
                gUnk_03005530.unkB |= 2;
            else
                goto end;
        }
    }
    if (sub_080216d8(gUnk_03005560, gUnk_03005570 + gUnk_03005584) == 0 && gUnk_087336F0[gUnk_03005588] != 0)
    {
        gUnk_03005530.unkB |= 1;
        if (gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0)
            gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584) >> 4;
        else
            gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 16) >> 4;
    }
end:
    gUnk_03002490->unk84 = gUnk_03005530.unkB;
}

void sub_080222b0(s32 x, s32 y)
{
    gUnk_03005530.unkB = 0;
    gUnk_03005530.unkC = 0;
    if (sub_080216d8(x, y) == 0 && gUnk_087336F0[gUnk_03005588] != 0)
    {
        gUnk_03005530.unkB |= 1;
        if (gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0)
            gUnk_03005530.unkC = y >> 4;
        else
            gUnk_03005530.unkC = (y + 16) >> 4;
    }
    else
    {
        gUnk_03005530.unkB &= 0xFE;
        gUnk_03005530.unkC = 0;
    }
}

void sub_0802233c(s8 *off)
{
    gUnk_03005560 = (gUnk_03002490->unk4C >> 16) + off[0];
    gUnk_03005570 = (gUnk_03002490->unk50 >> 16) + off[1];
    gUnk_03005530.unkB = 0;
    if (sub_080217dc(gUnk_03005560, gUnk_03005570) == 0)
    {
        if (gUnk_03002490->unk43 != -1)
        {
            if ((gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0
                 && (gUnk_08732CF0[gUnk_03005578] & 1))
                || (gUnk_087336F0[gUnk_03005510] != 0 && gUnk_08732CF0[gUnk_03005510] != 0
                    && (gUnk_08732CF0[gUnk_03005510] & 1)
                    && ((gUnk_08732DF0[gUnk_03005510] & 0xCF) != 0x83 || (gUnk_03005570 & 15) <= 7)))
                gUnk_03005530.unkB |= 4;
        }
        else
        {
            if ((gUnk_087336F0[gUnk_03005578] != 0 && gUnk_08732CF0[gUnk_03005578] != 0
                 && !(gUnk_08732CF0[gUnk_03005578] & 1))
                || (gUnk_087336F0[gUnk_03005594] != 0 && gUnk_08732CF0[gUnk_03005594] != 0
                    && !(gUnk_08732CF0[gUnk_03005594] & 1)
                    && ((gUnk_08732DF0[gUnk_03005594] & 0xCF) != 0x83 || (gUnk_03005570 & 15) <= 7)))
                gUnk_03005530.unkB |= 2;
        }
    }
    gUnk_03002490->unk84 = gUnk_03005530.unkB;
}

void sub_080224b0(void)
{
    sub_08021634(gUnk_03002490->unk4C >> 16, gUnk_03002490->unk50 >> 16);
    if (gUnk_03005578 > 127)
        gUnk_03002490->unk7B = 3;
    else
        gUnk_03002490->unk7B = 0;
    gUnk_03002490->unk84 = 128;
}

void sub_080224f8(s32 id)
{
    struct Task *t = &gUnk_03002790[id];

    sub_08021634(t->unk4C >> 16, t->unk50 >> 16);
    if (gUnk_03005578 > 127)
        t->unk7B = 3;
    else
        t->unk7B = 0;
    t->unk84 = 128;
}

s32 sub_08022540(u32 x, u32 y)
{
    s16 w;
    u16 *p;
    s32 v;
    u32 idx;

    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x < w && y < gUnk_0300561C)
    {
        idx = y * w + x;
        if (gUnk_02008160[idx] != 0)
        {
            v = ((u8 *)gUnk_030055EC->unk10)[gUnk_02008160[idx] * 4 + 7] - 56;
            if ((u32)v <= 5)
                return v;
        }
    }
    return -1;
}

u16 sub_0802259c(u16 x, u16 y)
{
    s16 w;
    u32 idx;
    s32 tile;
    s32 r;

    x >>= 4;
    y >>= 4;
    w = gUnk_03005620;
    if (x >= w || y >= gUnk_0300561C)
        return 0;
    idx = y * w;
    tile = (&gUnk_03005660[idx])[x].unk3;
    r = 0;
    if (tile > 127)
        r = 256;
    if (gUnk_087328F0[tile][((y & 15) << 4) + (x & 15)] != 0)
        r |= gUnk_087335F0[tile];
    return r;
}

s32 sub_08022624(u16 x, u16 y)
{
    if (gUnk_08732EF0[sub_08021b18(x, y)] == 1)
        return 1;
    return 0;
}

void sub_08022650(void)
{
    gUnk_03005568 = 0;
    gUnk_03005544 = 0;
    if (gUnk_03005640[gCurTaskIdx].x0 > gUnk_03005560 + gUnk_0300551C)
    {
        gUnk_03005560 = gUnk_03005640[gCurTaskIdx].x0 - gUnk_0300551C;
        gUnk_03005568 = 1;
    }
    else if (gUnk_03005640[gCurTaskIdx].x1 < gUnk_03005560 + gUnk_0300559C)
    {
        gUnk_03005560 = gUnk_03005640[gCurTaskIdx].x1 - gUnk_0300559C;
        gUnk_03005568 = 2;
    }
    if (gUnk_03005640[gCurTaskIdx].y0 > gUnk_03005570 + gUnk_0300557C)
    {
        gUnk_03005570 = gUnk_03005640[gCurTaskIdx].y0 - gUnk_0300557C;
        gUnk_03005568 |= 4;
        if (gUnk_02005574[0] == 0)
            gUnk_03005544 = gUnk_03005640[gCurTaskIdx].y0;
    }
}

s32 sub_08022760(struct Task *t)
{
    if (gUnk_03005640[gCurTaskIdx].y1 < t->unk4A)
        return 1;
    return 0;
}

s32 sub_08022788(s32 y, s32 i)
{
    if (gUnk_03005640[i].y0 == y)
        return 1;
    return 0;
}

s32 sub_080227a4(struct Task *t)
{
    s32 r = 0;
    s32 lo = gUnk_03005628[0] - 111;
    s32 hi = gUnk_03005628[1] + 111;

    if (lo > t->unk48)
    {
        t->unk4C = lo << 16;
        t->unk48 = lo;
        r = 1;
    }
    else if (hi < t->unk48)
    {
        t->unk4C = hi << 16;
        t->unk48 = hi;
        r = 2;
    }
    lo = gUnk_03005628[2] - 72;
    if (lo > t->unk4A)
    {
        t->unk50 = lo << 16;
        t->unk4A = lo;
        r |= 4;
    }
    return r;
}

s32 sub_08022810(void)
{
    s32 lo;
    s32 hi;

    gUnk_03005568 = 0;
    lo = gUnk_030055F8[0] - 117;
    hi = gUnk_030055F8[1] + 117;
    if (lo > gUnk_03005560 + gUnk_0300551C)
    {
        gUnk_03005560 = lo - gUnk_0300551C;
        gUnk_03005568 = 1;
    }
    else if (hi < gUnk_03005560 + gUnk_0300559C)
    {
        gUnk_03005560 = hi - gUnk_0300559C;
        gUnk_03005568 = 2;
    }
    lo = gUnk_030055F8[2] - 76;
    if (lo > gUnk_03005570 + gUnk_0300557C)
    {
        gUnk_03005570 = lo - gUnk_0300557C;
        gUnk_03005568 |= 4;
    }
}

s32 sub_080228c4(struct Task *t)
{
    s32 r = 0;
    s32 lo = gUnk_030055F8[0] - 111;
    s32 hi = gUnk_030055F8[1] + 111;

    if (t->unk48 < lo)
    {
        if (lo - t->unk48 <= 11)
        {
            t->unk4C = lo << 16;
            t->unk48 = lo;
            r = 1;
        }
    }
    else if (hi < t->unk48)
    {
        if (t->unk48 - lo > 12)
        {
            t->unk4C = hi << 16;
            t->unk48 = hi;
            r = 2;
        }
    }
    lo = gUnk_030055F8[2] - 72;
    if (lo > t->unk4A)
    {
        if (lo - t->unk48 <= 11)
        {
            t->unk50 = lo << 16;
            t->unk4A = lo;
            r |= 4;
        }
    }
    return r;
}

s32 sub_0802294c(struct Task *t)
{
    if (gUnk_03005628[3] + 104 < t->unk4A)
        return 1;
    return 0;
}
