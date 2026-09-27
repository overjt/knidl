#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_273a0.c (0x080273A0-0x08027A6B, issue #93).
 *
 * Stage helpers, part 3.  sub_080273a0 picks the arrival door after a
 * level change (the stage door kind in gUnk_02008054), sub_08027548 and
 * sub_08027588/sub_080275cc keep the two-player race record
 * gUnk_02006098 (flags|0x80, lo, hi, previous, direction),
 * sub_080276ac/sub_080276cc/sub_08027750/sub_08027a30 the per-player
 * camera modes gPlayerCameraMode, and sub_08027798, sub_080277f0,
 * sub_08027850 and sub_08027908 set the camera mode and target
 * (gCameraAnchorX/gCameraAnchorY) for one or all players. */

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
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

struct CamPos { u16 x, y; };

extern struct RoomDef *gCurRoomDef;
extern struct RoomDef **gRoomTable[][8];
extern u16 gUnk_02008054;
extern s8 gStageIndex;
extern s8 gLevelIndex;
extern s8 gRoomIndex;
extern u16 gUnk_02007FF0;
extern u16 gGameState;
extern s16 gRoomEntryX;
extern s16 gRoomEntryY;
extern u8 gRoomEntrySet;
extern u8 gUnk_0200AF00;
extern u8 gUnk_020069F0;
extern s8 gUnk_030023B8;
extern u16 gUnk_0200AFF4;
extern u16 gUnk_02008050;
extern s8 gUnk_02007D64;
extern u16 gUnk_02007D60;
extern u8 gUnk_0200B078;
extern s8 gUnk_02006098[];
extern u8 gPlayerCameraMode[];
extern u16 gPlayerCount;
extern u8 gActivePlayerMask;
extern u16 gCameraMode;
extern u16 gLocalPlayer;
extern s16 gPlayerLives[];
extern s16 gCameraAnchorX;
extern s16 gCameraAnchorY;
extern s32 gCameraCenterX;
extern s32 gCameraCenterY;
extern s8 gUnk_03002444;
extern s16 gCameraFocusX;
extern s16 gCameraFocusY;
extern u8 gUnk_02007D38;
extern s16 gCameraBounds[4];
extern struct CamPos gPlayerCameraPos[4];

void sub_08009e2c(s32 a);
void sub_0800a0dc(s32 a, s32 b);
void sub_08028b8c(void);
void sub_08028e3c(void);
s32 CreateMapEvent(s32 a);

void sub_080273a0(void)
{
    struct RoomDef *r;
    struct Door *d;
    s32 i;
    s32 k;

    if (gUnk_02008054 & 0xFF00)
    {
        if (gUnk_02008054 == 0x100)
        {
            gStageIndex = gLevelIndex;
            gLevelIndex = 8;
            gRoomIndex = 0;
            k = 2;
            gUnk_02007FF0++;
            if (gUnk_02007FF0 > 5)
                gUnk_02007FF0 = 5;
        }
        else
        {
            gRoomIndex = 0;
            k = gGameState - 11;
        }
        r = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
        d = r->unk44;
        for (i = 0; i < r->unk3A; d++, i++)
        {
            if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == k)
                break;
        }
        gRoomEntryX = (d->unk2 << 4) + 22;
        gRoomEntryY = (d->unk4 << 4) + 5;
        gRoomEntrySet = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 1;
        gGameState = 5;
    }
    else
    {
        if (gUnk_030023B8 == 7)
        {
            gUnk_02007FF0++;
            if (gUnk_02007FF0 > 5)
                gUnk_02007FF0 = 5;
        }
        gRoomIndex = gUnk_02008054;
        gRoomEntryX = gUnk_0200AFF4;
        gRoomEntryY = gUnk_02008050;
        gRoomEntrySet = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 0;
    }
}

void sub_08027548(void)
{
    if (gUnk_02007D64 != 4)
    {
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 &= 0x7FFF;
        gUnk_02007D60++;
        if (gUnk_02007D60 > 5)
            gUnk_02007D60 = 5;
    }
}

s32 sub_08027588(void)
{
    if (gUnk_0200B078 != 4 || gUnk_02006098[0] != 0 || CreateMapEvent(1) == -1)
        return 0;
    gUnk_02006098[0] |= 0x80;
    return 1;
}

s32 sub_080275cc(s32 a)
{
    s32 lo;
    s32 hi;
    s32 prev;
    u8 dir;
    s8 *p;

    if (gUnk_0200B078 != 5)
        return 0;
    p = gUnk_02006098;
    if (p[0] & 0x80)
    {
        if (p[4] == 1)
        {
            if (p[2] == a)
                ((u8 *)p)[3] = 0xFF;
            else
            {
                if (p[1] == a)
                {
                    p[4] = -1 * p[4];
                    ((u8 *)p)[3] = 0xFF;
                }
                else
                {
                    if (a < p[1])
                        p[4] = -1 * p[4];
                    p[3] = a;
                }
            }
        }
        else
        {
            if (p[1] == a)
                ((u8 *)p)[3] = 0xFF;
            else
            {
                if (p[2] == a)
                {
                    p[4] = -1 * p[4];
                    ((u8 *)p)[3] = 0xFF;
                }
                else
                {
                    if (p[2] < a)
                        p[4] = -1 * p[4];
                    p[3] = a;
                }
            }
        }
        return;
    }
    if (p[0] == a)
        return 0;
    if (CreateMapEvent(4) != -1)
    {
        if (gUnk_02006098[0] != 1 && a != 1)
        {
            lo = gUnk_02006098[0];
            hi = 1;
            prev = a;
        }
        else
        {
            lo = gUnk_02006098[0];
            hi = a;
            prev = -1;
        }
        if (gUnk_02006098[0] < a)
        {
            gUnk_02006098[1] = lo;
            gUnk_02006098[2] = hi;
            dir = 1;
        }
        else
        {
            gUnk_02006098[1] = hi;
            gUnk_02006098[2] = lo;
            dir = 0xFF;
        }
        gUnk_02006098[4] = dir;
        gUnk_02006098[3] = prev;
        gUnk_02006098[0] |= 0x80;
        return 1;
    }
    return 0;
}

s32 sub_080276ac(s32 a)
{
    gPlayerCameraMode[gCurTask->unk88->unk00] = 1;
    return a;
}

s32 sub_080276cc(s32 i)
{
    if (gPlayerCount > 1 && gActivePlayerMask != 0)
    {
        if (gCameraMode != 5)
            gPlayerCameraMode[i] = 2;
        else
            gPlayerCameraMode[i] = 3;
        if (i == gLocalPlayer)
        {
            if (gPlayerLives[i] != 0)
                sub_0800a0dc(26, gCurTask->unk88->unk00);
            else
                sub_08009e2c(i);
        }
    }
}

s32 sub_08027750(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (((gActivePlayerMask >> i) & 1) == 0 && gPlayerCameraMode[i] != 3)
            return 0;
    }
    return 1;
}

void sub_08027798(s32 x, s32 y)
{
    gCameraAnchorX = x;
    gCameraAnchorY = y;
    gCameraCenterX = gCameraAnchorX << 16;
    gCameraCenterY = gCameraAnchorY << 16;
    if (gUnk_03002444 != 0)
        gCameraMode = 4;
    else
        gCameraMode = 5;
}

void sub_080277f0(s32 x, s32 y)
{
    gCameraFocusX = x;
    gCameraFocusY = y;
    if (gUnk_03002444 != 0)
        gCameraMode = 0;
    else
        gCameraMode = 0;
    if (gUnk_02007D64 != 2)
    {
        if (gUnk_03002444 != 0)
            sub_08028e3c();
        else
            sub_08028b8c();
    }
}

void sub_08027850(s32 a)
{
    s32 i;
    s32 x;
    s32 y;

    if (gPlayerCount == 1)
    {
        gUnk_02007D38 = gLocalPlayer;
        gCameraMode = 0;
    }
    else
    {
        gUnk_02007D38 = a;
        gCameraMode = 1;
        for (i = 0; i < gPlayerCount; i++)
        {
            x = gCameraAnchorX;
            y = gCameraAnchorY;
            if (x < gCameraBounds[0])
                x = gCameraBounds[0];
            if (gCameraBounds[1] < x)
                x = gCameraBounds[1];
            if (y < gCameraBounds[2])
                y = gCameraBounds[2];
            if (gCameraBounds[3] < y)
                y = gCameraBounds[3];
            gPlayerCameraPos[i].x = x;
            gPlayerCameraPos[i].y = y;
        }
    }
}

void sub_08027908(void)
{
    s32 i;
    s32 x;
    s32 y;
    u16 *m;

    if (gPlayerCount == 1)
    {
        x = gCameraAnchorX;
        y = gCameraAnchorY;
        if (x < gCameraBounds[0])
            x = gCameraBounds[0];
        if (gCameraBounds[1] < x)
            x = gCameraBounds[1];
        if (y < gCameraBounds[2])
            y = gCameraBounds[2];
        if (gCameraBounds[3] < y)
            y = gCameraBounds[3];
        gPlayerCameraPos[gLocalPlayer].x = x;
        gPlayerCameraPos[gLocalPlayer].y = y;
        gCameraMode = 0;
    }
    else
    {
        i = 0;
        m = &gCameraMode;
        for (; i < gPlayerCount; i++)
        {
            x = gCameraAnchorX;
            y = gCameraAnchorY;
            if (x < gCameraBounds[0])
                x = gCameraBounds[0];
            if (gCameraBounds[1] < x)
                x = gCameraBounds[1];
            if (y < gCameraBounds[2])
                y = gCameraBounds[2];
            if (gCameraBounds[3] < y)
                y = gCameraBounds[3];
            gPlayerCameraPos[i].x = x;
            gPlayerCameraPos[i].y = y;
            if (i == gUnk_02007D38)
                gPlayerCameraMode[i] = 0;
            else
                gPlayerCameraMode[i] = 2;
        }
        *m = 3;
    }
}

s32 sub_08027a30(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerCameraMode[i] == 2)
            return 0;
    }
    return 1;
}

void sub_08027a60(void)
{
    gCameraMode = 0;
}
