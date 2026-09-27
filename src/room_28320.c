#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* room_28320.c (0x08028320-0x08028B8B, issue #93).
 *
 * Room start-up services.  SpawnDoorObjects clears the door-object slots
 * gDoorObjectTasks[32][3], finds the door the player entered by and spawns
 * an M08 stage object for every locked or special door (a 9-way switch on
 * the door kind, gated by the save flags gUnk_08732348/gUnk_03002400);
 * CalcBg3Parallax computes the BG3 parallax factors, CalcRoomBounds the room
 * bounds, CameraResetBoundsToGroup the multi-player group bounds and CameraResetBounds
 * copies the room bounds into the camera and per-player bounds. */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
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

extern s8 gUnk_0200B034;
extern s8 gDoorObjectTasks[][3];
extern s32 gScrollLockSpeedX;
extern s32 gScrollLockSpeedY;
extern s32 gUnk_030055F0;
extern s32 gUnk_03005618;
extern s16 gUnk_020055D4;
extern u8 gUnk_020069F0;
extern s16 gRoomEntryX;
extern s16 gRoomEntryY;
extern struct RoomDef *gCurRoomDef;
extern u16 gUnk_08732348[][9];
extern s8 gUnk_030023B8;
extern u32 gUnk_030023C8[];
extern u8 gUnk_03002400[8][7];
extern s8 gUnk_030023E0;
extern s8 gUnk_03002384;
extern s8 gStageIndex;
extern u8 gUnk_0200AF00;
extern u8 gUsedSubGameDoors[];
extern u8 gUnk_0200AF08;
extern u8 gUnk_0200B04C;
extern s8 gUnk_03002444;
extern s32 gBg3ParallaxX;
extern s32 gBg3ParallaxY;
extern u16 gBg3Border[2];
extern u16 gRoomBorder[2];
extern s16 gRoomWidth;
extern s16 gRoomHeight;
extern s16 gRoomBounds[4];
extern u16 gPlayerCount;
extern u8 gActivePlayerMask;
extern u16 gPlayerGroupCenter[2];
extern s16 gCameraBounds[4];
extern struct CamRect gPlayerBounds[4];

void LoadGfxSet(u16 a0);
s32 GetCollisionTileAtPixel(u16 x, u16 y);
void UpdatePlayerCameras(void);
s32 sub_0802eac8(s32 x, s32 y, s32 a);
s32 sub_0802ec1c(s32 x, s32 y, s32 a);
s32 sub_0802ef90(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f05c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f1fc(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f31c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f420(s32 x, s32 y, s32 a);
s32 sub_0802f4c8(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f53c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f5b4(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f7dc(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802fa3c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802fdf4(s32 x, s32 y, s32 a, s32 b);

void SpawnDoorObjects(void)
{
    s32 i, x, y, k, m, r;
    u32 v;
    struct Door *d;

    LoadGfxSet(2);
    gUnk_0200B034 = -1;
    for (i = 0; i < 32; i++)
    {
        gDoorObjectTasks[i][0] = -1;
        gDoorObjectTasks[i][1] = -1;
        gDoorObjectTasks[i][2] = -1;
    }
    gScrollLockSpeedX = 0;
    gScrollLockSpeedY = 0;
    gUnk_030055F0 = 0;
    gUnk_03005618 = 0;
    gUnk_020055D4 = 0x4000;
    if (gUnk_020069F0 == 1 || gUnk_020069F0 == 4)
    {
        r = GetCollisionTileAtPixel(gRoomEntryX, gRoomEntryY);
        if (r == 55 || r == 183)
            x = (gRoomEntryX >> 4) - 1;
        else
            x = gRoomEntryX >> 4;
        y = gRoomEntryY >> 4;
        d = gCurRoomDef->unk44;
        for (i = 0; i < gCurRoomDef->unk3A; d++, i++)
        {
            if (d->unk2 == x && d->unk4 == y)
            {
                gUnk_0200B034 = i;
                break;
            }
        }
    }
    d = gCurRoomDef->unk44;
    for (i = 0; i < gCurRoomDef->unk3A; d++, i++)
    {
        if (d->unk0 != 0x270F)
            continue;
        x = (d->unk2 << 4) + 16;
        y = (d->unk4 << 4) + 8;
        k = d->unk6 & 0xFF;
        v = gUnk_08732348[gUnk_030023B8][k];
        if (v != 0xFFFF)
        {
            if (v & 0x100)
            {
                if (!(gUnk_030023C8[0] & (1 << (v & 0xFF))))
                    continue;
            }
            else
            {
                if (!gUnk_03002400[gUnk_030023B8][v])
                    continue;
            }
        }
        switch (k)
        {
        case 0:
            if (gUnk_030023E0 > gUnk_030023B8 || gUnk_03002384 >= d->unk8)
            {
                switch (gUnk_03002400[gStageIndex][d->unk8])
                {
                default:
                case 0:
                    gDoorObjectTasks[i][0] = sub_0802f4c8(x, y, d->unk8, i);
                    break;
                case 1:
                    if (i == gUnk_0200B034 && gUnk_0200AF00 == 1)
                    {
                        gDoorObjectTasks[i][0] = sub_0802f4c8(x, y, d->unk8, i);
                    }
                    else
                    {
                        gDoorObjectTasks[i][0] = sub_0802f53c(x, y, d->unk8, i);
                        gDoorObjectTasks[i][1] = sub_0802ef90(x, y, 15, 0x4000);
                    }
                    break;
                case 2:
                    if (i == gUnk_0200B034)
                    {
                        if (gUnk_020069F0 == 4)
                        {
                            gDoorObjectTasks[i][0] = sub_0802f53c(x, y, d->unk8, i);
                            gDoorObjectTasks[i][1] = sub_0802ef90(x, y, 15, 0x4000);
                            break;
                        }
                        if (gUnk_0200AF00 == 1)
                        {
                            gDoorObjectTasks[i][0] = sub_0802f4c8(x, y, d->unk8, i);
                            break;
                        }
                    }
                    gDoorObjectTasks[i][0] = sub_0802f5b4(x, y, d->unk8, i);
                    gDoorObjectTasks[i][1] = sub_0802ef90(x, y, 15, 0x4000);
                    break;
                }
            }
            break;
        case 1:
            gDoorObjectTasks[i][0] = sub_0802fdf4(x, y, 0, i);
            break;
        case 2:
            if (gUnk_030023B8 >= gUnk_030023E0)
                gDoorObjectTasks[i][0] = sub_0802ec1c(x, y, i);
            else
                gDoorObjectTasks[i][0] = sub_0802fdf4(x, y, 1, i);
            break;
        case 3:
            if (!(gUsedSubGameDoors[gStageIndex] & 1))
                gDoorObjectTasks[i][0] = sub_0802f1fc(x, y, 1, i);
            else if (i == gUnk_0200B034 && gUnk_0200AF00 == 2)
                gDoorObjectTasks[i][0] = sub_0802f1fc(x, y, 1, i);
            else
                gDoorObjectTasks[i][0] = sub_0802f1fc(x, y, 0, i);
            break;
        case 4:
            if (!(gUsedSubGameDoors[gStageIndex] & 2))
                gDoorObjectTasks[i][0] = sub_0802f31c(x, y, 1, i);
            else if (i == gUnk_0200B034 && gUnk_0200AF00 == 2)
                gDoorObjectTasks[i][0] = sub_0802f31c(x, y, 1, i);
            else
                gDoorObjectTasks[i][0] = sub_0802f31c(x, y, 0, i);
            break;
        case 5:
            if (!(gUsedSubGameDoors[gStageIndex] & 4))
                gDoorObjectTasks[i][0] = sub_0802f05c(x, y, 1, i);
            else if (i == gUnk_0200B034 && gUnk_0200AF00 == 2)
                gDoorObjectTasks[i][0] = sub_0802f05c(x, y, 1, i);
            else
                gDoorObjectTasks[i][0] = sub_0802f05c(x, y, 0, i);
            break;
        case 6:
            gUnk_020055D4 = 0x4000;
            m = 0;
            if (gUnk_0200AF08 != 0)
            {
                if (gUnk_0200AF08 & 16)
                    m = 1;
                else
                    gUnk_020055D4 = 0x2000;
            }
            if (gUnk_0200B04C & ~(1 << gStageIndex))
                gDoorObjectTasks[i][0] = sub_0802f7dc(x, y, 0, i);
            else
                gDoorObjectTasks[i][0] = sub_0802f7dc(x, y, 1, i);
            gDoorObjectTasks[i][1] = sub_0802fa3c(x, y, 0, m);
            gDoorObjectTasks[i][2] = sub_0802fa3c(x, y, 1, m);
            break;
        case 8:
            gDoorObjectTasks[i][0] = sub_0802eac8(x, y, i);
            break;
        case 7:
            gDoorObjectTasks[i][0] = sub_0802f420(x, y, i);
            break;
        }
    }
}

void CalcBg3Parallax(void)
{
    s32 num;
    s32 den;
    s32 a;
    s32 k;

    if (gUnk_03002444 != 0)
    {
        gBg3ParallaxX = 0x10000;
        gBg3ParallaxY = 0x10000;
        return;
    }
    a = gCurRoomDef->unk30->unk2;
    a <<= 3;
    k = gBg3Border[0] * 2 + 240;
    num = a - k;
    a = gRoomWidth;
    a <<= 4;
    k = gRoomBorder[0] * 2 + 240;
    den = a - k;
    if (den > 0)
    {
        gBg3ParallaxX = Div(num << 16, den);
        if (gBg3ParallaxX > 0x10000)
            gBg3ParallaxX = 0x10000;
    }
    else
    {
        gBg3ParallaxX = 0x10000;
    }
    a = gCurRoomDef->unk30->unk4;
    a <<= 3;
    k = gBg3Border[1] * 2 + 160;
    num = a - k;
    a = gRoomHeight;
    a <<= 4;
    k = gRoomBorder[1] * 2 + 160;
    den = a - k;
    if (den > 0)
    {
        gBg3ParallaxY = Div(num << 16, den);
        if (gBg3ParallaxY > 0x10000)
            gBg3ParallaxY = 0x10000;
    }
    else
    {
        gBg3ParallaxY = 0x10000;
    }
}

void CalcRoomBounds(void)
{
    gRoomBounds[0] = gRoomBorder[0] + 120;
    gRoomBounds[1] = gRoomWidth * 16 - gRoomBorder[0] - 120;
    gRoomBounds[2] = gRoomBorder[1] + 80;
    gRoomBounds[3] = gRoomHeight * 16 - gRoomBorder[1] - 80;
}

void CameraResetBoundsToGroup(void)
{
    s32 x0, x1, y0, y1, v, cx, cy, i;
    s16 t;

    x0 = gRoomWidth << 4;
    x1 = 0;
    y0 = gRoomHeight << 4;
    y1 = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            v = gTasks[i].unk48;
            if (v < gRoomBounds[0])
                v = gRoomBounds[0];
            if (gRoomBounds[1] < v)
                v = gRoomBounds[1];
            if (v < x0)
                x0 = v;
            if (x1 < v)
                x1 = v;
            v = gTasks[i].unk4A;
            if (v < gRoomBounds[2])
                v = gRoomBounds[2];
            if (gRoomBounds[3] < v)
                v = gRoomBounds[3];
            if (v < y0)
                y0 = v;
            if (y1 < v)
                y1 = v;
        }
    }
    gPlayerGroupCenter[0] = cx = (x0 + x1) >> 1;
    gPlayerGroupCenter[1] = cy = (y0 + y1) >> 1;
    gCameraBounds[0] = t = cx - 80;
    gCameraBounds[1] = cx + 80;
    gCameraBounds[2] = cy - 120;
    gCameraBounds[3] = cy + 120;
    if (t < gRoomBounds[0])
        gCameraBounds[0] = gRoomBounds[0];
    if (gRoomBounds[1] < gCameraBounds[1])
        gCameraBounds[1] = gRoomBounds[1];
    if (gCameraBounds[2] < gRoomBounds[2])
        gCameraBounds[2] = gRoomBounds[2];
    if (gRoomBounds[3] < gCameraBounds[3])
        gCameraBounds[3] = gRoomBounds[3];
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            gPlayerBounds[i].x0 = gCameraBounds[0] - 117;
            gPlayerBounds[i].x1 = gCameraBounds[1] + 117;
            gPlayerBounds[i].y0 = gCameraBounds[2] - 76;
            gPlayerBounds[i].y1 = gCameraBounds[3] + 104;
        }
    }
    UpdatePlayerCameras();
}

void CameraResetBounds(void)
{
    s32 i;

    gCameraBounds[0] = gRoomBounds[0];
    gCameraBounds[1] = gRoomBounds[1];
    gCameraBounds[2] = gRoomBounds[2];
    gCameraBounds[3] = gRoomBounds[3];
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            gPlayerBounds[i].x0 = gCameraBounds[0] - 117;
            gPlayerBounds[i].x1 = gCameraBounds[1] + 117;
            gPlayerBounds[i].y0 = gCameraBounds[2] - 76;
            gPlayerBounds[i].y1 = gCameraBounds[3] + 104;
        }
    }
}
