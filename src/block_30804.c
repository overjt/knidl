#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* block_30804.c (0x08030804-0x080318B3, issue #92).
 *
 * Breakable blocks, part 1: the hit-box scans and the block spawner.
 * gUnk_02008160[] is the room's block layer, one u16 per metatile: 0 = no
 * block, the low byte = which replacement metatile of RoomDef.unk10[] the
 * cell turns into, bit 15 = being broken.  gUnk_020061F0[64] holds the
 * blocks being broken (struct Unk020061F0).  An attack's hit-box set
 * (struct HitBoxSet) is placed at the task's position and facing by the
 * six wrappers sub_08030804 ... sub_08030db8 and scanned tile by tile:
 * sub_0803097c tries every metatile its boxes cover and returns how many
 * blocks broke; sub_08030b14 breaks the first block of the row at the
 * set's centre (then the rows above and below) and records its pixel
 * position in gUnk_02007FA0/gUnk_02004B6C; sub_08030e00 finds the top of
 * the block column under the box and breaks that row with attack id 6.
 * sub_0803111c(x, y, id, player) decides whether the attack breaks the
 * block at a metatile - an 8-way switch on the block kind (id & 0xFF)
 * against the per-collision-byte tables gUnk_0873A494/gUnk_0873A5D4 and
 * the on-screen test sub_08031310 - and latches it into the cursor cells
 * gUnk_02007D68/gUnk_0200AEFC/gUnk_020060C8 (x, y, map index),
 * gUnk_02004B40 (the player), gUnk_02004B48, gUnk_02006174 (the kind) and
 * gUnk_020061D0 (the collision byte); sub_08031374 then takes a free
 * record, points it at the BG map entry at 0x06002000, plays the sound
 * (sub_080031b8), awards points to the player (sub_0800a04c) and starts
 * the animation script gUnk_0873A47C[kind].  sub_08030f78 (M08's map
 * events) and sub_08031738 (M07) break a block at a metatile directly;
 * sub_08030f1c tests a metatile for an unbroken block. */

/* A hit-box set: unk0 & 0x8000 = mirror with the task's facing, unk0 & 0xFFF
   = the attack id passed to sub_0803111c; unk2/unk3 = (x, y) offset of the
   set; unk4 = the boxes, {y0, y1, x0, x1} each (x mirrored as -x1..-x0),
   terminated by y0 == 127. */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ s8 (*unk4)[4];
};

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

/* gUnk_020061F0[64] (and gUnk_0200A6F0[64] for the second block layer
   gUnk_02004CA0): one 32-byte record per block being broken.  unk0/unk2 =
   metatile x/y, unk4 = its map index, unk6 = the position in the animation
   script (0x7FFF = free slot; bit 15 = already stepped this frame), unk8 =
   the metatile the cell shows next (RoomDef.unk10[] + the layer's low byte,
   advanced by one per drawn frame), unkC = its tile entry in the BG map,
   unk10 = the script (gUnk_0873A47C[kind], {op, arg} pairs: 1 and 2 draw a
   frame, 3 breaks the four neighbours, 4 waits arg frames, 0x8000/0x8001
   free the record), unk14 = the frames left to wait, unk16/unk18 = the
   metatile index and collision byte written back to the map, unk1A = the
   block kind, unk1C = the player that broke it (-1 = none). */
struct Unk020061F0
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ struct MapTile *unk8;
    /*0x0C*/ u16 *unkC;
    /*0x10*/ u16 *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
    /*0x1C*/ s8 unk1C;
    /*0x1D*/ u8 filler1D[3];
};

/* M09's view of the room header (M07's struct RoomDef, src/level_*.c): the
   only field read here is unk10, the table of replacement metatiles the low
   byte of gUnk_02008160[] indexes (M07 types it void *). */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x10];
    /*0x10*/ struct MapTile *unk10;
};

extern s16 gUnk_03005620;               /* map width in metatiles */
extern s16 gUnk_0300561C;               /* map height in metatiles */
extern u16 gUnk_02007FA0;               /* the block sub_08030b14 broke: x (pixels) */
extern u16 gUnk_02004B6C;               /*   y (pixels) */
extern u32 gUnk_030004A0;               /* per-frame stage hook (sub_080318b4 / sub_08031de4 / sub_08032428) */
extern u16 gUnk_02008160[];             /* per-cell block layer: low byte = replacement index, 0x8000 = being broken */
extern struct MapTile *gUnk_03005660;   /* the room's metatile map */
extern struct RoomDef *gUnk_030055EC;   /* the current room header */
extern struct Unk020061F0 gUnk_020061F0[];
extern u16 gUnk_02007D68;               /* the block sub_0803111c accepted: x */
extern u16 gUnk_0200AEFC;               /*   y */
extern u16 gUnk_020060C8;               /*   map index */
extern u8 gUnk_02004B40;                /*   the player that hit it */
extern u8 gUnk_02004B48;                /*   hit-box id bit 11 */
extern u16 gUnk_02006174;               /*   the block kind (hit-box id low byte) */
extern s16 gUnk_020061D0;               /*   the metatile's collision byte */
extern s8 gUnk_0873A494[];
extern u16 *gUnk_0873A47C[];            /* animation script per block kind */
extern s8 gUnk_0873A5D4[];
extern s16 gUnk_03002158[];
extern u8 gUnk_0200B078;

s32 sub_080031b8(s32 id);
void sub_0800a04c(s32 a, u32 b);
s32 sub_080261c0(s32 x, s32 y);
void sub_080261d4(u16 a);
s32 sub_080301e8(s32 a, s32 x, s32 y);
void sub_08031ab8(struct Unk020061F0 *b, s32 n);
void sub_08031ebc(struct Unk020061F0 *b);
u16 sub_0803097c(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e);
u16 sub_08030b14(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e);
u16 sub_08030e00(struct HitBoxSet *p, s32 x, s32 y, s32 dir);
s32 sub_08030f1c(u32 x, u32 y);
s32 sub_0803111c(s32 x, s32 y, s32 id, s32 e);
s32 sub_08031310(s32 x, s32 y);
s32 sub_08031374(void);

u16 sub_08030804(struct HitBoxSet *p, s32 x, s32 y, s32 e)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gUnk_03002490->unk43;
    else
        dir = 1;
    return sub_0803097c(p, x, y, dir, e);
}

u16 sub_08030848(struct HitBoxSet *p, s32 e)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gUnk_03002490->unk43;
    else
        dir = 1;
    return sub_0803097c(p, gUnk_03002490->unk48, gUnk_03002490->unk4A, dir, e);
}

u16 sub_08030898(struct HitBoxSet *p, s32 e)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gUnk_03002490->unk43;
    else
        dir = 1;
    return sub_08030b14(p, gUnk_03002490->unk48, gUnk_03002490->unk4A, dir, e);
}

u16 sub_080308e8(struct HitBoxSet *p)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gUnk_03002490->unk43;
    else
        dir = 1;
    return sub_0803097c(p, gUnk_03002490->unk48, gUnk_03002490->unk4A, dir, -1);
}

u16 sub_0803093c(struct HitBoxSet *p, s32 x, s32 y)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gUnk_03002490->unk43;
    else
        dir = 1;
    return sub_0803097c(p, x, y, dir, -1);
}

u16 sub_0803097c(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e)
{
    s32 count = 0;
    s8 (*box)[4];
    s16 x0, x1, y0, y1;
    s16 tx, ty;

    if (dir == 1)
        x += p->unk2;
    else
        x -= p->unk2;
    y += p->unk3;
    if (x < 0)
        x = 0;
    if (x > gUnk_03005620 * 16)
        x = gUnk_03005620 * 16 - 1;
    if (y < 0)
        y = 0;
    if (y > gUnk_0300561C * 16)
        y = gUnk_0300561C * 16 - 1;
    for (box = p->unk4; (*box)[0] != 127; box++)
    {
        if (dir == 1)
        {
            x0 = (x + (*box)[2]) >> 4;
            x1 = (x + (*box)[3]) >> 4;
        }
        else
        {
            x0 = (x - (*box)[3]) >> 4;
            x1 = (x - (*box)[2]) >> 4;
        }
        y0 = (y + (*box)[0]) >> 4;
        y1 = (y + (*box)[1]) >> 4;
        if (x0 < 0)
            x0 = 0;
        if (x1 >= gUnk_03005620)
            x1 = gUnk_03005620 - 1;
        if (y0 < 0)
            y0 = 0;
        if (y1 >= gUnk_0300561C)
            y1 = gUnk_0300561C - 1;
        for (ty = y0; ty <= y1; ty++)
        {
            for (tx = x0; tx <= x1; tx++)
            {
                if (sub_0803111c(tx, ty, p->unk0 & 0xFFF, e) && sub_08031374() != -1)
                    count++;
            }
        }
    }
    return count;
}

u16 sub_08030b14(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e)
{
    s8 (*box)[4];
    s16 x0, x1, y0, y1;
    s16 tx;

    if (dir == 1)
        x += p->unk2;
    else
        x -= p->unk2;
    y += p->unk3;
    if (x < 0)
        x = 0;
    if (x > gUnk_03005620 * 16)
        x = gUnk_03005620 * 16 - 1;
    if (y < 0)
        y = 0;
    if (y > gUnk_0300561C * 16)
        y = gUnk_0300561C * 16 - 1;
    box = p->unk4;
    if (dir == 1)
    {
        x0 = (x + (*box)[2]) >> 4;
        x1 = (x + (*box)[3]) >> 4;
    }
    else
    {
        x0 = (x - (*box)[3]) >> 4;
        x1 = (x - (*box)[2]) >> 4;
    }
    y0 = (y + (*box)[0]) >> 4;
    y1 = (y + (*box)[1]) >> 4;
    if (x0 < 0)
        x0 = 0;
    if (x1 >= gUnk_03005620)
        x1 = gUnk_03005620 - 1;
    if (y0 < 0)
        y0 = 0;
    if (y1 >= gUnk_0300561C)
        y1 = gUnk_0300561C - 1;
    y >>= 4;
    if (dir == 1)
    {
        for (tx = x0; tx <= x1; tx++)
        {
            if (sub_0803111c(tx, y, 1, e) && sub_08031374() != -1)
            {
                gUnk_02007FA0 = tx * 16;
                gUnk_02004B6C = y * 16;
                goto found;
            }
        }
        if (y0 < y)
        {
            for (tx = x0; tx <= x1; tx++)
            {
                if (sub_0803111c(tx, y - 1, 1, e) && sub_08031374() != -1)
                {
                    gUnk_02007FA0 = tx * 16;
                    gUnk_02004B6C = (y - 1) * 16;
                    goto found;
                }
            }
        }
        if (y1 > y)
        {
            for (tx = x0; tx <= x1; tx++)
            {
                if (sub_0803111c(tx, y + 1, 1, e) && sub_08031374() != -1)
                {
                    gUnk_02007FA0 = tx * 16;
                    gUnk_02004B6C = (y + 1) * 16;
                    goto found;
                }
            }
        }
    }
    else
    {
        for (tx = x1; tx >= x0; tx--)
        {
            if (sub_0803111c(tx, y, 1, e) && sub_08031374() != -1)
            {
                gUnk_02007FA0 = tx * 16;
                gUnk_02004B6C = y * 16;
                goto found;
            }
        }
        if (y0 < y)
        {
            for (tx = x1; tx >= x0; tx--)
            {
                if (sub_0803111c(tx, y - 1, 1, e) && sub_08031374() != -1)
                {
                    gUnk_02007FA0 = tx * 16;
                    gUnk_02004B6C = (y - 1) * 16;
                    goto found;
                }
            }
        }
        if (y1 > y)
        {
            for (tx = x1; tx >= x0; tx--)
            {
                if (sub_0803111c(tx, y + 1, 1, e) && sub_08031374() != -1)
                {
                    gUnk_02007FA0 = tx * 16;
                    gUnk_02004B6C = (y + 1) * 16;
                    goto found;
                }
            }
        }
    }
    return 0;
found:
    return 1;
}

u16 sub_08030db8(struct HitBoxSet *p)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gUnk_03002490->unk43;
    else
        dir = 1;
    return sub_08030e00(p, gUnk_03002490->unk48, gUnk_03002490->unk4A, dir);
}

u16 sub_08030e00(struct HitBoxSet *p, s32 x, s32 y, s32 dir)
{
    s32 count = 0;
    s8 (*box)[4];
    s32 x0, x1, tx, ty;

    if (dir == 1)
        x += p->unk2;
    else
        x -= p->unk2;
    y += p->unk3;
    if (x < 0)
        x = 0;
    if (x > gUnk_03005620 * 16)
        x = gUnk_03005620 * 16 - 1;
    if (y < 0)
        y = 0;
    if (y > gUnk_0300561C * 16)
        y = gUnk_0300561C * 16 - 1;
    box = p->unk4;
    if ((*box)[0] == 127)
        return;
    if (dir == 1)
    {
        x0 = (x + (*box)[2]) >> 4;
        x1 = (x + (*box)[3]) >> 4;
    }
    else
    {
        x0 = (x - (*box)[3]) >> 4;
        x1 = (x - (*box)[2]) >> 4;
    }
    if (x0 < 0)
        x0 = 0;
    if (x1 >= gUnk_03005620)
        x1 = gUnk_03005620 - 1;
    tx = x0;
    ty = y >> 4;
    while (tx < gUnk_03005620 && sub_08030f1c(tx, ty) == 0)
    {
        tx++;
        if (x1 < tx)
            return 0;
    }
    while (--ty >= 0 && sub_08030f1c(tx, ty) != 0)
        ;
    ty++;
    for (tx = x0; tx <= x1; tx++)
    {
        if (sub_0803111c(tx, ty, 6, -1) && sub_08031374() != -1)
            count++;
    }
    return count;
}

s32 sub_08030f1c(u32 x, u32 y)
{
    s32 i;
    if (gUnk_030004A0 != 0 && x < gUnk_03005620 && y < gUnk_0300561C)
    {
        i = y * gUnk_03005620 + x;
        if (gUnk_02008160[i] != 0 && !(gUnk_02008160[i] & 0x8000))
            return 1;
    }
    return 0;
}

s32 sub_08030f78(u32 x, u32 y)
{
    s32 i = 0;
    struct Unk020061F0 *b;

    if (gUnk_030004A0 != 0 && x < gUnk_03005620 && y < gUnk_0300561C)
    {
        gUnk_02004B40 = 0xFF;
        gUnk_02007D68 = x;
        gUnk_0200AEFC = y;
        gUnk_020060C8 = x + gUnk_0200AEFC * gUnk_03005620;
        if (gUnk_02008160[gUnk_020060C8] != 0 && !(gUnk_02008160[gUnk_020060C8] & 0x8000))
        {
            gUnk_020061D0 = gUnk_03005660[gUnk_020060C8].unk3;
            gUnk_02004B48 = 0;
            gUnk_02006174 = 0;
            if (gUnk_0873A494[gUnk_020061D0] <= 4)
            {
                while (gUnk_020061F0[i].unk6 != 0x7FFF)
                {
                    i++;
                    if (i > 63)
                        return -1;
                }
                b = &gUnk_020061F0[i];
                b->unk4 = gUnk_020060C8;
                b->unk8 = gUnk_030055EC->unk10 + gUnk_02008160[gUnk_020060C8];
                b->unk0 = gUnk_02007D68;
                b->unk2 = gUnk_0200AEFC;
                b->unkC = (u16 *)0x06002000 + ((gUnk_02007D68 * 2 & 31) + ((gUnk_0200AEFC * 2 & 31) + (gUnk_02007D68 & 16) * 2) * 32);
                gUnk_02008160[gUnk_020060C8] |= 0x8000;
                b->unk1A = 0;
                b->unk10 = gUnk_0873A47C[0];
                b->unk14 = 0;
                b->unk6 = 0;
                if (b->unk10[0] == 1)
                    sub_08031ab8(b, ((s16 *)b->unk10)[1]);
                return i;
            }
        }
    }
    return -1;
}

s32 sub_0803111c(s32 x, s32 y, s32 id, s32 e)
{
    if (gUnk_030004A0 != 0 && x < gUnk_03005620 && y < gUnk_0300561C)
    {
        gUnk_02004B40 = e;
        gUnk_02007D68 = x;
        gUnk_0200AEFC = y;
        gUnk_020060C8 = x + gUnk_0200AEFC * gUnk_03005620;
        if (gUnk_02008160[gUnk_020060C8] != 0 && !(gUnk_02008160[gUnk_020060C8] & 0x8000))
        {
            gUnk_020061D0 = gUnk_03005660[gUnk_020060C8].unk3;
            if (id & 0x800)
                gUnk_02004B48 = 1;
            else
                gUnk_02004B48 = 0;
            gUnk_02006174 = id & 0xFF;
            switch (gUnk_02006174)
            {
            case 0:
            case 6:
                if (gUnk_0873A494[gUnk_020061D0] > 4)
                    return 0;
                return 1;
            case 2:
                if (!sub_08031310(gUnk_02007D68, gUnk_0200AEFC) || gUnk_0873A494[gUnk_020061D0] > 1)
                    return 0;
                return 1;
            case 1:
                if (!sub_08031310(gUnk_02007D68, gUnk_0200AEFC) || gUnk_0873A494[gUnk_020061D0] != 0)
                    return 0;
                return 1;
            case 3:
                if (!sub_08031310(gUnk_02007D68, gUnk_0200AEFC) || gUnk_0873A494[gUnk_020061D0] > 2)
                    return 0;
                return 1;
            case 4:
                if (!sub_08031310(gUnk_02007D68, gUnk_0200AEFC) || gUnk_0873A494[gUnk_020061D0] > 3)
                    return 0;
                return 1;
            case 5:
                if (gUnk_0873A5D4[gUnk_020061D0] == 0)
                    return 0;
            case 7:
                return 1;
            }
        }
    }
    return 0;
}

s32 sub_08031310(s32 x, s32 y)
{
    s32 lim;

    x = x * 16 + 8;
    y = y * 16 + 8;
    lim = gUnk_03002158[0] - 16;
    if (lim < 0)
        lim = 0;
    if (x > lim)
    {
        lim = gUnk_03002158[1] + 16;
        if (gUnk_03005620 * 16 < lim)
            lim = gUnk_03005620 * 16;
        if (lim > x)
        {
            lim = gUnk_03002158[2] - 16;
            if (lim < 0)
                lim = 0;
            if (y > lim)
                return 1;
        }
    }
    return 0;
}

s32 sub_08031374(void)
{
    s32 i;
    s32 k;
    struct Unk020061F0 *b;

    i = 0;
    while (gUnk_020061F0[i].unk6 != 0x7FFF)
    {
        i++;
        if (i > 63)
            return -1;
    }
    b = &gUnk_020061F0[i];
    b->unk4 = gUnk_020060C8;
    b->unk8 = gUnk_030055EC->unk10 + gUnk_02008160[gUnk_020060C8];
    b->unk0 = gUnk_02007D68;
    b->unk2 = gUnk_0200AEFC;
    b->unk1C = gUnk_02004B40;
    if (gUnk_0200B078 == 1)
        b->unkC = (u16 *)0x06002000 + ((gUnk_02007D68 * 2 & 31) + ((gUnk_0200AEFC * 2 & 63) << 5));
    else
        b->unkC = (u16 *)0x06002000 + ((gUnk_02007D68 * 2 & 31) + ((gUnk_0200AEFC * 2 & 31) + (gUnk_02007D68 & 16) * 2) * 32);
    gUnk_02008160[gUnk_020060C8] |= 0x8000;
    switch (gUnk_02006174)
    {
    case 0:
        sub_080261c0(gUnk_02007D68 * 16 + 8, gUnk_0200AEFC * 16 + 8);
        sub_080031b8(159);
        if (gUnk_0873A5D4[gUnk_020061D0] != 0)
        {
            k = 4;
            b->unk1A = 0x805;
        }
        else
        {
            k = 0;
            b->unk1A = 0;
        }
        break;
    case 1:
        k = 1;
        b->unk1A = 1;
        break;
    case 2:
        sub_080261c0(gUnk_02007D68 * 16 + 8, gUnk_0200AEFC * 16 + 8);
        sub_080031b8(159);
        if (b->unk1C != -1)
            sub_0800a04c(10, b->unk1C);
        if (gUnk_0873A5D4[gUnk_020061D0] != 0)
        {
            k = 4;
            b->unk1A = 0x805;
            if (b->unk1C != -1)
                sub_0800a04c(50, b->unk1C);
        }
        else
        {
            k = 0;
            b->unk1A = 2;
        }
        break;
    case 4:
        if (b->unk1C != -1)
            sub_0800a04c(10, b->unk1C);
        if (gUnk_020061D0 == 51)
        {
            sub_080031b8(159);
            k = 4;
            b->unk1A = 0x805;
            if (b->unk1C != -1)
                sub_0800a04c(50, b->unk1C);
        }
        else
        {
            sub_080261c0(gUnk_02007D68 * 16 + 8, gUnk_0200AEFC * 16 + 8);
            if (gUnk_0200B078 == 1)
                sub_080031b8(224);
            else
                sub_080031b8(159);
            if (gUnk_0873A5D4[gUnk_020061D0] != 0)
            {
                k = 4;
                b->unk1A = 0x805;
                if (b->unk1C != -1)
                    sub_0800a04c(50, b->unk1C);
            }
            else
            {
                k = 0;
                b->unk1A = 2;
            }
        }
        break;
    case 3:
        if (b->unk1C != -1)
            sub_0800a04c(10, b->unk1C);
        sub_080261c0(gUnk_02007D68 * 16 + 8, gUnk_0200AEFC * 16 + 8);
        sub_080031b8(159);
        if (gUnk_0873A5D4[gUnk_020061D0] != 0)
        {
            k = 4;
            b->unk1A = 0x805;
            if (b->unk1C != -1)
                sub_0800a04c(50, b->unk1C);
        }
        else
        {
            k = 0;
            b->unk1A = 3;
        }
        break;
    case 5:
        sub_080261c0(gUnk_02007D68 * 16 + 8, gUnk_0200AEFC * 16 + 8);
        sub_080031b8(159);
        if (b->unk1C != -1)
            sub_0800a04c(10, b->unk1C);
        if (gUnk_0873A5D4[gUnk_020061D0] != 0)
        {
            k = 4;
            b->unk1A = 0x805;
        }
        else
        {
            k = 0;
            b->unk1A = 2;
        }
        break;
    case 6:
        sub_080301e8(3, gUnk_02007D68 * 16 + 8, gUnk_0200AEFC * 16 + 20);
        sub_080031b8(159);
        if (gUnk_0873A5D4[gUnk_020061D0] != 0)
            k = 3;
        else
            k = 2;
        b->unk1A = 6;
        break;
    case 7:
        b->unk1A = 7;
        k = 5;
        break;
    default:
        return -1;
    }
    if (gUnk_02004B48 != 0)
        sub_080261d4(1);
    b->unk10 = gUnk_0873A47C[k];
    b->unk14 = 0;
    b->unk6 = 0;
    if (b->unk10[0] == 1)
    {
        if (gUnk_0200B078 == 1)
            sub_08031ebc(b);
        else
            sub_08031ab8(b, ((s16 *)b->unk10)[1]);
    }
    return i;
}

s32 sub_08031738(u32 x, u32 y, s32 n)
{
    s32 i;
    struct Unk020061F0 *b;
    struct MapTile *t;

    if (x >= gUnk_03005620 || y >= gUnk_0300561C)
        return -1;
    gUnk_02007D68 = x;
    gUnk_0200AEFC = y;
    gUnk_02004B40 = 0xFF;
    gUnk_020060C8 = x + gUnk_0200AEFC * gUnk_03005620;
    if (gUnk_02008160[gUnk_020060C8] == 0 || (gUnk_02008160[gUnk_020060C8] & 0x8000))
        return -1;
    i = 0;
    while (gUnk_020061F0[i].unk6 != 0x7FFF)
    {
        i++;
        if (i > 63)
            return -1;
    }
    b = &gUnk_020061F0[i];
    b->unk4 = gUnk_020060C8;
    t = gUnk_030055EC->unk10 + gUnk_02008160[gUnk_020060C8] + n;
    b->unk8 = t;
    b->unk0 = gUnk_02007D68;
    b->unk2 = gUnk_0200AEFC;
    b->unk1C = gUnk_02004B40;
    b->unkC = (u16 *)0x06002000 + ((gUnk_02007D68 * 2 & 31) + ((gUnk_0200AEFC * 2 & 31) + (gUnk_02007D68 & 16) * 2) * 32);
    b->unk16 = t->unk0;
    b->unk18 = t->unk3;
    gUnk_02008160[gUnk_020060C8] |= 0x8000;
    gUnk_03005660[gUnk_020060C8].unk0 = b->unk16;
    gUnk_03005660[gUnk_020060C8].unk3 = b->unk18;
    b->unk1A = 7;
    b->unk10 = gUnk_0873A47C[5];
    b->unk14 = 0;
    b->unk6 = 0;
    return i;
}
