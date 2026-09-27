#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"

/* block_30804.c (0x08030804-0x080318B3, issue #92).
 *
 * Breakable blocks, part 1: the hit-box scans and the block spawner.
 * gBlockLayer[] is the room's block layer, one u16 per metatile: 0 = no
 * block, the low byte = which replacement metatile of RoomDef.unk10[] the
 * cell turns into, bit 15 = being broken.  gBreakingBlocks[64] holds the
 * blocks being broken (struct Unk020061F0).  An attack's hit-box set
 * (struct HitBoxSet) is placed at the task's position and facing by the
 * six wrappers TaskBreakBlocksAt ... sub_08030db8 and scanned tile by tile:
 * BreakBlocksInHitBoxes tries every metatile its boxes cover and returns how many
 * blocks broke; sub_08030b14 breaks the first block of the row at the
 * set's centre (then the rows above and below) and records its pixel
 * position in gUnk_02007FA0/gUnk_02004B6C; sub_08030e00 finds the top of
 * the block column under the box and breaks that row with attack id 6.
 * CanBreakBlock(x, y, id, player) decides whether the attack breaks the
 * block at a metatile - an 8-way switch on the block kind (id & 0xFF)
 * against the per-collision-byte tables gUnk_0873A494/gUnk_0873A5D4 and
 * the on-screen test sub_08031310 - and latches it into the cursor cells
 * gBlockCursorX/gBlockCursorY/gBlockCursorIndex (x, y, map index),
 * gBlockCursorPlayer (the player), gUnk_02004B48, gUnk_02006174 (the kind) and
 * gBlockCursorTile (the collision byte); BreakBlockAtCursor then takes a free
 * record, points it at the BG map entry at 0x06002000, plays the sound
 * (PlaySfx), awards points to the player (AddPlayerScore) and starts
 * the animation script gUnk_0873A47C[kind].  BreakBlockAt (M08's map
 * events) and sub_08031738 (M07) break a block at a metatile directly;
 * sub_08030f1c tests a metatile for an unbroken block. */

/* A hit-box set: unk0 & 0x8000 = mirror with the task's facing, unk0 & 0xFFF
   = the attack id passed to CanBreakBlock; unk2/unk3 = (x, y) offset of the
   set; unk4 = the boxes, {y0, y1, x0, x1} each (x mirrored as -x1..-x0),
   terminated by y0 == 127. */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 offsetX;
    /*0x03*/ s8 offsetY;
    /*0x04*/ s8 (*boxes)[4];
};

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 collisionTile;
};

/* gBreakingBlocks[64] (and gBg1BreakingBlocks[64] for the second block layer
   gBg1MetatileMap): one 32-byte record per block being broken.  unk0/unk2 =
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
   byte of gBlockLayer[] indexes (M07 types it void *). */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x10];
    /*0x10*/ struct MapTile *unk10;
};

/* Not from room.h or player.h: this file's view of gRoomMap and gUnk_02004B6C
   differs (lesson 3.517). */
extern s16 gRoomWidth;               /* map width in metatiles */
extern s16 gRoomHeight;               /* map height in metatiles */
extern u16 gUnk_02007FA0;               /* the block sub_08030b14 broke: x (pixels) */
extern u16 gUnk_02004B6C;               /*   y (pixels) */
extern u16 gBlockLayer[];             /* per-cell block layer: low byte = replacement index, 0x8000 = being broken */
extern struct MapTile *gRoomMap;   /* the room's metatile map */
extern struct RoomDef *gCurRoomDef;   /* the current room header */
extern struct Unk020061F0 gBreakingBlocks[];
extern u16 gBlockCursorX;               /* the block CanBreakBlock accepted: x */
extern u16 gBlockCursorY;               /*   y */
extern u16 gBlockCursorIndex;               /*   map index */
extern u8 gBlockCursorPlayer;                /*   the player that hit it */
extern u8 gUnk_02004B48;                /*   hit-box id bit 11 */
extern u16 gUnk_02006174;               /*   the block kind (hit-box id low byte) */
extern s16 gBlockCursorTile;               /*   the metatile's collision byte */
extern s8 gUnk_0873A494[];
extern u16 *gUnk_0873A47C[];            /* animation script per block kind */
extern s8 gUnk_0873A5D4[];
extern s16 gViewRect[];
extern u8 gUnk_0200B078;

s32 PlaySfx(s32 id);
void AddPlayerScore(s32 a, u32 b);
s32 CreateBlockBreakEffect(s32 x, s32 y);
void RequestScreenShake(u16 a);
s32 CreateStageEffect(s32 a, s32 x, s32 y);
void sub_08031ab8(struct Unk020061F0 *b, s32 n);
void BlockAnimWriteMetatileWrapped(struct Unk020061F0 *b);
u16 BreakBlocksInHitBoxes(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e);
u16 sub_08030b14(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e);
u16 sub_08030e00(struct HitBoxSet *p, s32 x, s32 y, s32 dir);
s32 sub_08030f1c(u32 x, u32 y);
s32 CanBreakBlock(s32 x, s32 y, s32 id, s32 e);
s32 sub_08031310(s32 x, s32 y);
s32 BreakBlockAtCursor(void);

u16 TaskBreakBlocksAt(struct HitBoxSet *p, s32 x, s32 y, s32 e)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gCurTask->facing;
    else
        dir = 1;
    return BreakBlocksInHitBoxes(p, x, y, dir, e);
}

u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gCurTask->facing;
    else
        dir = 1;
    return BreakBlocksInHitBoxes(p, gCurTask->pixelX, gCurTask->pixelY, dir, e);
}

u16 sub_08030898(struct HitBoxSet *p, s32 e)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gCurTask->facing;
    else
        dir = 1;
    return sub_08030b14(p, gCurTask->pixelX, gCurTask->pixelY, dir, e);
}

u16 TaskBreakBlocksNoPlayer(struct HitBoxSet *p)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gCurTask->facing;
    else
        dir = 1;
    return BreakBlocksInHitBoxes(p, gCurTask->pixelX, gCurTask->pixelY, dir, -1);
}

u16 sub_0803093c(struct HitBoxSet *p, s32 x, s32 y)
{
    s32 dir;

    if (p->unk0 & 0x8000)
        dir = gCurTask->facing;
    else
        dir = 1;
    return BreakBlocksInHitBoxes(p, x, y, dir, -1);
}

u16 BreakBlocksInHitBoxes(struct HitBoxSet *p, s32 x, s32 y, s32 dir, s32 e)
{
    s32 count = 0;
    s8 (*box)[4];
    s16 x0, x1, y0, y1;
    s16 tx, ty;

    if (dir == 1)
        x += p->offsetX;
    else
        x -= p->offsetX;
    y += p->offsetY;
    if (x < 0)
        x = 0;
    if (x > gRoomWidth * 16)
        x = gRoomWidth * 16 - 1;
    if (y < 0)
        y = 0;
    if (y > gRoomHeight * 16)
        y = gRoomHeight * 16 - 1;
    for (box = p->boxes; (*box)[0] != 127; box++)
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
        if (x1 >= gRoomWidth)
            x1 = gRoomWidth - 1;
        if (y0 < 0)
            y0 = 0;
        if (y1 >= gRoomHeight)
            y1 = gRoomHeight - 1;
        for (ty = y0; ty <= y1; ty++)
        {
            for (tx = x0; tx <= x1; tx++)
            {
                if (CanBreakBlock(tx, ty, p->unk0 & 0xFFF, e) && BreakBlockAtCursor() != -1)
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
        x += p->offsetX;
    else
        x -= p->offsetX;
    y += p->offsetY;
    if (x < 0)
        x = 0;
    if (x > gRoomWidth * 16)
        x = gRoomWidth * 16 - 1;
    if (y < 0)
        y = 0;
    if (y > gRoomHeight * 16)
        y = gRoomHeight * 16 - 1;
    box = p->boxes;
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
    if (x1 >= gRoomWidth)
        x1 = gRoomWidth - 1;
    if (y0 < 0)
        y0 = 0;
    if (y1 >= gRoomHeight)
        y1 = gRoomHeight - 1;
    y >>= 4;
    if (dir == 1)
    {
        for (tx = x0; tx <= x1; tx++)
        {
            if (CanBreakBlock(tx, y, 1, e) && BreakBlockAtCursor() != -1)
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
                if (CanBreakBlock(tx, y - 1, 1, e) && BreakBlockAtCursor() != -1)
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
                if (CanBreakBlock(tx, y + 1, 1, e) && BreakBlockAtCursor() != -1)
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
            if (CanBreakBlock(tx, y, 1, e) && BreakBlockAtCursor() != -1)
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
                if (CanBreakBlock(tx, y - 1, 1, e) && BreakBlockAtCursor() != -1)
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
                if (CanBreakBlock(tx, y + 1, 1, e) && BreakBlockAtCursor() != -1)
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
        dir = gCurTask->facing;
    else
        dir = 1;
    return sub_08030e00(p, gCurTask->pixelX, gCurTask->pixelY, dir);
}

u16 sub_08030e00(struct HitBoxSet *p, s32 x, s32 y, s32 dir)
{
    s32 count = 0;
    s8 (*box)[4];
    s32 x0, x1, tx, ty;

    if (dir == 1)
        x += p->offsetX;
    else
        x -= p->offsetX;
    y += p->offsetY;
    if (x < 0)
        x = 0;
    if (x > gRoomWidth * 16)
        x = gRoomWidth * 16 - 1;
    if (y < 0)
        y = 0;
    if (y > gRoomHeight * 16)
        y = gRoomHeight * 16 - 1;
    box = p->boxes;
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
    if (x1 >= gRoomWidth)
        x1 = gRoomWidth - 1;
    tx = x0;
    ty = y >> 4;
    while (tx < gRoomWidth && sub_08030f1c(tx, ty) == 0)
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
        if (CanBreakBlock(tx, ty, 6, -1) && BreakBlockAtCursor() != -1)
            count++;
    }
    return count;
}

s32 sub_08030f1c(u32 x, u32 y)
{
    s32 i;
    if (gBlockAnimHook != 0 && x < gRoomWidth && y < gRoomHeight)
    {
        i = y * gRoomWidth + x;
        if (gBlockLayer[i] != 0 && !(gBlockLayer[i] & 0x8000))
            return 1;
    }
    return 0;
}

s32 BreakBlockAt(u32 x, u32 y)
{
    s32 i = 0;
    struct Unk020061F0 *b;

    if (gBlockAnimHook != 0 && x < gRoomWidth && y < gRoomHeight)
    {
        gBlockCursorPlayer = 0xFF;
        gBlockCursorX = x;
        gBlockCursorY = y;
        gBlockCursorIndex = x + gBlockCursorY * gRoomWidth;
        if (gBlockLayer[gBlockCursorIndex] != 0 && !(gBlockLayer[gBlockCursorIndex] & 0x8000))
        {
            gBlockCursorTile = gRoomMap[gBlockCursorIndex].collisionTile;
            gUnk_02004B48 = 0;
            gUnk_02006174 = 0;
            if (gUnk_0873A494[gBlockCursorTile] <= 4)
            {
                while (gBreakingBlocks[i].unk6 != 0x7FFF)
                {
                    i++;
                    if (i > 63)
                        return -1;
                }
                b = &gBreakingBlocks[i];
                b->unk4 = gBlockCursorIndex;
                b->unk8 = gCurRoomDef->unk10 + gBlockLayer[gBlockCursorIndex];
                b->unk0 = gBlockCursorX;
                b->unk2 = gBlockCursorY;
                b->unkC = (u16 *)0x06002000 + ((gBlockCursorX * 2 & 31) + ((gBlockCursorY * 2 & 31) + (gBlockCursorX & 16) * 2) * 32);
                gBlockLayer[gBlockCursorIndex] |= 0x8000;
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

s32 CanBreakBlock(s32 x, s32 y, s32 id, s32 e)
{
    if (gBlockAnimHook != 0 && x < gRoomWidth && y < gRoomHeight)
    {
        gBlockCursorPlayer = e;
        gBlockCursorX = x;
        gBlockCursorY = y;
        gBlockCursorIndex = x + gBlockCursorY * gRoomWidth;
        if (gBlockLayer[gBlockCursorIndex] != 0 && !(gBlockLayer[gBlockCursorIndex] & 0x8000))
        {
            gBlockCursorTile = gRoomMap[gBlockCursorIndex].collisionTile;
            if (id & 0x800)
                gUnk_02004B48 = 1;
            else
                gUnk_02004B48 = 0;
            gUnk_02006174 = id & 0xFF;
            switch (gUnk_02006174)
            {
            case 0:
            case 6:
                if (gUnk_0873A494[gBlockCursorTile] > 4)
                    return 0;
                return 1;
            case 2:
                if (!sub_08031310(gBlockCursorX, gBlockCursorY) || gUnk_0873A494[gBlockCursorTile] > 1)
                    return 0;
                return 1;
            case 1:
                if (!sub_08031310(gBlockCursorX, gBlockCursorY) || gUnk_0873A494[gBlockCursorTile] != 0)
                    return 0;
                return 1;
            case 3:
                if (!sub_08031310(gBlockCursorX, gBlockCursorY) || gUnk_0873A494[gBlockCursorTile] > 2)
                    return 0;
                return 1;
            case 4:
                if (!sub_08031310(gBlockCursorX, gBlockCursorY) || gUnk_0873A494[gBlockCursorTile] > 3)
                    return 0;
                return 1;
            case 5:
                if (gUnk_0873A5D4[gBlockCursorTile] == 0)
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
    lim = gViewRect[0] - 16;
    if (lim < 0)
        lim = 0;
    if (x > lim)
    {
        lim = gViewRect[1] + 16;
        if (gRoomWidth * 16 < lim)
            lim = gRoomWidth * 16;
        if (lim > x)
        {
            lim = gViewRect[2] - 16;
            if (lim < 0)
                lim = 0;
            if (y > lim)
                return 1;
        }
    }
    return 0;
}

s32 BreakBlockAtCursor(void)
{
    s32 i;
    s32 k;
    struct Unk020061F0 *b;

    i = 0;
    while (gBreakingBlocks[i].unk6 != 0x7FFF)
    {
        i++;
        if (i > 63)
            return -1;
    }
    b = &gBreakingBlocks[i];
    b->unk4 = gBlockCursorIndex;
    b->unk8 = gCurRoomDef->unk10 + gBlockLayer[gBlockCursorIndex];
    b->unk0 = gBlockCursorX;
    b->unk2 = gBlockCursorY;
    b->unk1C = gBlockCursorPlayer;
    if (gUnk_0200B078 == 1)
        b->unkC = (u16 *)0x06002000 + ((gBlockCursorX * 2 & 31) + ((gBlockCursorY * 2 & 63) << 5));
    else
        b->unkC = (u16 *)0x06002000 + ((gBlockCursorX * 2 & 31) + ((gBlockCursorY * 2 & 31) + (gBlockCursorX & 16) * 2) * 32);
    gBlockLayer[gBlockCursorIndex] |= 0x8000;
    switch (gUnk_02006174)
    {
    case 0:
        CreateBlockBreakEffect(gBlockCursorX * 16 + 8, gBlockCursorY * 16 + 8);
        PlaySfx(159);
        if (gUnk_0873A5D4[gBlockCursorTile] != 0)
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
        CreateBlockBreakEffect(gBlockCursorX * 16 + 8, gBlockCursorY * 16 + 8);
        PlaySfx(159);
        if (b->unk1C != -1)
            AddPlayerScore(10, b->unk1C);
        if (gUnk_0873A5D4[gBlockCursorTile] != 0)
        {
            k = 4;
            b->unk1A = 0x805;
            if (b->unk1C != -1)
                AddPlayerScore(50, b->unk1C);
        }
        else
        {
            k = 0;
            b->unk1A = 2;
        }
        break;
    case 4:
        if (b->unk1C != -1)
            AddPlayerScore(10, b->unk1C);
        if (gBlockCursorTile == 51)
        {
            PlaySfx(159);
            k = 4;
            b->unk1A = 0x805;
            if (b->unk1C != -1)
                AddPlayerScore(50, b->unk1C);
        }
        else
        {
            CreateBlockBreakEffect(gBlockCursorX * 16 + 8, gBlockCursorY * 16 + 8);
            if (gUnk_0200B078 == 1)
                PlaySfx(224);
            else
                PlaySfx(159);
            if (gUnk_0873A5D4[gBlockCursorTile] != 0)
            {
                k = 4;
                b->unk1A = 0x805;
                if (b->unk1C != -1)
                    AddPlayerScore(50, b->unk1C);
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
            AddPlayerScore(10, b->unk1C);
        CreateBlockBreakEffect(gBlockCursorX * 16 + 8, gBlockCursorY * 16 + 8);
        PlaySfx(159);
        if (gUnk_0873A5D4[gBlockCursorTile] != 0)
        {
            k = 4;
            b->unk1A = 0x805;
            if (b->unk1C != -1)
                AddPlayerScore(50, b->unk1C);
        }
        else
        {
            k = 0;
            b->unk1A = 3;
        }
        break;
    case 5:
        CreateBlockBreakEffect(gBlockCursorX * 16 + 8, gBlockCursorY * 16 + 8);
        PlaySfx(159);
        if (b->unk1C != -1)
            AddPlayerScore(10, b->unk1C);
        if (gUnk_0873A5D4[gBlockCursorTile] != 0)
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
        CreateStageEffect(3, gBlockCursorX * 16 + 8, gBlockCursorY * 16 + 20);
        PlaySfx(159);
        if (gUnk_0873A5D4[gBlockCursorTile] != 0)
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
        RequestScreenShake(1);
    b->unk10 = gUnk_0873A47C[k];
    b->unk14 = 0;
    b->unk6 = 0;
    if (b->unk10[0] == 1)
    {
        if (gUnk_0200B078 == 1)
            BlockAnimWriteMetatileWrapped(b);
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

    if (x >= gRoomWidth || y >= gRoomHeight)
        return -1;
    gBlockCursorX = x;
    gBlockCursorY = y;
    gBlockCursorPlayer = 0xFF;
    gBlockCursorIndex = x + gBlockCursorY * gRoomWidth;
    if (gBlockLayer[gBlockCursorIndex] == 0 || (gBlockLayer[gBlockCursorIndex] & 0x8000))
        return -1;
    i = 0;
    while (gBreakingBlocks[i].unk6 != 0x7FFF)
    {
        i++;
        if (i > 63)
            return -1;
    }
    b = &gBreakingBlocks[i];
    b->unk4 = gBlockCursorIndex;
    t = gCurRoomDef->unk10 + gBlockLayer[gBlockCursorIndex] + n;
    b->unk8 = t;
    b->unk0 = gBlockCursorX;
    b->unk2 = gBlockCursorY;
    b->unk1C = gBlockCursorPlayer;
    b->unkC = (u16 *)0x06002000 + ((gBlockCursorX * 2 & 31) + ((gBlockCursorY * 2 & 31) + (gBlockCursorX & 16) * 2) * 32);
    b->unk16 = t->metatile;
    b->unk18 = t->collisionTile;
    gBlockLayer[gBlockCursorIndex] |= 0x8000;
    gRoomMap[gBlockCursorIndex].metatile = b->unk16;
    gRoomMap[gBlockCursorIndex].collisionTile = b->unk18;
    b->unk1A = 7;
    b->unk10 = gUnk_0873A47C[5];
    b->unk14 = 0;
    b->unk6 = 0;
    return i;
}
