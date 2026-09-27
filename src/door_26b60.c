#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* door_26b60.c (0x08026B60-0x080270CF, issue #93).
 *
 * The door objects: one gDoorStates record per RoomDef door that is not
 * one of the special ids 0x1A0A, 0x1E61 or 0x15B3.  sub_08026b60 and
 * UpdateDoors (the per-frame body, flag 16 of gRoomUpdateFlags) decide
 * whether a door is usable - in multi-player every present player must be
 * within 128 pixels - and step its animation (frame in the low nibble of
 * byte 4, timer in the high one, the star doors from gUnk_0873264C);
 * DrawDoors draws the visible ones with QueueSprite. */

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
    /*0x04*/ s8 bgm;
    /*0x05*/ u8 mapsCompressed;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *metatileMap;
    /*0x0C*/ void *blockLayer;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 width;
    /*0x16*/ u16 height;
    /*0x18*/ u16 *bg2Palette;
    /*0x1C*/ void *bg2Tiles;
    /*0x20*/ void *metatileTiles;
    /*0x24*/ u16 borderX;
    /*0x26*/ u16 borderY;
    /*0x28*/ u16 *bg3Palette;
    /*0x2C*/ void *bg3Tiles;
    /*0x30*/ struct BgMap *bg3Map;
    /*0x34*/ u16 bg3BorderX;
    /*0x36*/ u16 bg3BorderY;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 doorCount;
    /*0x3C*/ u16 objectCount;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 bgAnimSet;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *doors;
    /*0x48*/ void *objects;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 entryX;
    /*0x52*/ u16 entryY;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk4_0:4;
    /*0x04*/ u8 unk4_4:4;
    /*0x05*/ u8 filler05[3];
};

extern struct RoomDef *gCurRoomDef;
extern struct Unk02004B90 gDoorStates[];
extern s16 gRoomEntryY;
extern s32 gUnk_03001F2C;
extern s32 gUnk_03002448;
extern u16 gPlayerCount;
extern u8 gActivePlayerCount;
extern s32 gUnk_03002344;
extern s16 gRoomEntryX;
extern s8 gUnk_08733AF0[];
extern u32 gUnk_03002160;
extern u8 gActivePlayerMask;
extern u8 gUnk_0873264C[][2];
extern s8 gUnk_03002444;
extern u32 gUnk_03001F10;
extern u8 gUnk_02007CF0;
extern u32 gUnk_0874CDF8[];
extern s16 gSpriteCameraX;
extern u16 gSpriteCameraY;

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 IsWorldPosOnScreen(s16 x, s16 y);
s32 GetCollisionTileAtPixel(u16 x, u16 y);

void sub_08026b60(void)
{
    struct Door *d = gCurRoomDef->doors;
    s16 i;

    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s32 lim;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gDoorStates[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        p->unk1 = 0;
        if (gPlayerCount > 1 && gActivePlayerCount > 1)
        {
            gUnk_03002344 = 128;
            gUnk_03001F2C = x - gRoomEntryX;
            gUnk_03002448 = y - gRoomEntryY;
            lim = 0x4000;
            if (lim < gUnk_03001F2C * gUnk_03001F2C + gUnk_03002448 * gUnk_03002448)
                p->unk1 = 0;
            else
                p->unk1 = 1;
        }
        else
        {
            p->unk1 = 1;
        }
        if (--p->unk4_4 == 0)
        {
            if (++p->unk4_0 > 3)
                p->unk4_0 = 0;
            p->unk4_4 = 3;
        }
    }
}

void UpdateDoors(void)
{
    struct Door *d = gCurRoomDef->doors;
    s16 i;

    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s16 j;
        s16 k;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gDoorStates[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        p->unk1 = 0;
        k = GetCollisionTileAtPixel(x, y);
        if (gUnk_08733AF0[k] == 0)
            continue;
        if (gPlayerCount > 1 && gActivePlayerCount > 1)
        {
            gUnk_03002160 = 0;
            gUnk_03002344 = 128;
            for (j = 0; j < gPlayerCount; j++)
            {
                if ((gActivePlayerMask >> j) & 1)
                {
                    struct Task *t = &gTasks[j];

                    gUnk_03001F2C = x - t->pixelX;
                    gUnk_03002448 = y - t->pixelY;
                    if (gUnk_03002344 * gUnk_03002344 >= gUnk_03001F2C * gUnk_03001F2C + gUnk_03002448 * gUnk_03002448)
                        gUnk_03002160++;
                }
                else
                {
                    gUnk_03002160++;
                }
            }
            if (gUnk_03002160 == gPlayerCount)
                p->unk1 = 1;
            else
                p->unk1 = 0;
        }
        else
        {
            p->unk1 = 1;
        }
        if (p->unk0 != 2)
        {
            if (--p->unk4_4 == 0)
            {
                if (++p->unk4_0 > 3)
                    p->unk4_0 = 0;
                p->unk4_4 = 3;
            }
        }
        else
        {
            if (--p->unk4_4 == 0)
            {
                if (++p->unk4_0 > 5)
                    p->unk4_0 = 0;
                p->unk4_4 = gUnk_0873264C[p->unk4_0][1];
            }
        }
    }
}

void DrawDoors(void)
{
    struct Door *d;
    s16 i;

    if (gUnk_03002444 != 0)
        return;
    d = gCurRoomDef->doors;
    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s16 k;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gDoorStates[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        if (IsWorldPosOnScreen(x, y) == 0)
            continue;
        k = GetCollisionTileAtPixel(x, y);
        if (k != 16 && k != 144)
            continue;
        gUnk_03001F10 = -1;
        switch (p->unk0)
        {
        case 1:
            gUnk_03001F2C = 16;
            if ((gUnk_02007CF0 != 2 || gActivePlayerCount <= 1) && p->unk1 != 0)
                gUnk_03001F10 = 8;
            else
                gUnk_03001F10 = 12;
            gUnk_03001F10 += p->unk4_0;
            break;
        default:
        case 0:
            gUnk_03001F2C = 8;
            if ((gUnk_02007CF0 != 2 || gActivePlayerCount <= 1) && p->unk1 != 0)
                gUnk_03001F10 = 0;
            else
                gUnk_03001F10 = 4;
            gUnk_03001F10 += p->unk4_0;
            break;
        case 2:
            gUnk_03001F2C = 8;
            if (p->unk1 != 0)
                gUnk_03001F10 = 0;
            else
                gUnk_03001F10 = 6;
            gUnk_03001F10 += gUnk_0873264C[p->unk4_0][0];
            break;
        }
        QueueSprite(15, gUnk_0874CDF8[gUnk_03001F10], 0, 0, x + gUnk_03001F2C - gSpriteCameraX,
                     -gSpriteCameraY + y);
    }
}
