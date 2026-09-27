#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_28b8c.c (0x08028B8C-0x0802969F, issue #93).
 *
 * Camera start-up for a new room.  sub_08028b8c (camera modes other than
 * 5) copies the room bounds into the camera bounds and puts the camera,
 * its 16.16 target and the visible rectangle on the player (one player)
 * or runs M08's multi-player updates; sub_08028e3c only sets the bounds.
 * SetRoomEntryPoint places the player at the room's start position
 * (RoomDef.unk50/unk52) unless a door already did, clamps it and records
 * the arrival for the next level change (gUnk_02008054, gUnk_0200AFF4,
 * gUnk_02008050).  sub_08029034 clamps the arrival position into the room
 * and makes it the camera target, sub_080290ac copies the current
 * player's camera position into the player cells, CameraInitPos and
 * CameraUpdatePos/CameraUpdatePosNoParallax/CameraUpdatePosBg3AutoScrollX
 * set the camera and BG3 positions (BG3 moves by the parallax factors of
 * room_28320.c), StreamBg2Map/StreamBg3Map stream the BG and BG3 maps
 * for a camera move (M08's StreamBg23Maps again), sub_08029110/
 * sub_08029194 start the room's BGM, LoadBg2Gfx ... SpawnRoomObjectsInView load
 * the room palettes, tiles and BG map (SelectBg3MapShape picks the BG layout
 * gBg3MapShape from the map size) and InitDoors builds the door
 * objects. */

struct CamPos { u16 x, y; };

struct CamRect { s16 x0, x1, y0, y1; };

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

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk4_0:4;
    /*0x04*/ u8 unk4_4:4;
    /*0x05*/ u8 filler05[3];
};

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
};

struct Unk020055D8
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk020055D8Entry *unk4;
};

extern u16 gCameraMode;
extern s8 gUnk_03002444;
extern u16 gPlayerCount;
extern s16 gCameraBounds[4];
extern s16 gRoomBounds[4];
extern struct CamRect gPlayerBounds[4];
extern u16 gLocalPlayer;
extern s16 gCameraFocusX;
extern s16 gCameraFocusY;
extern struct CamPos gPlayerCameraPos[4];
extern s32 gCameraCenterX;
extern s32 gCameraCenterY;
extern s16 gViewRect[4];
extern u8 gActivePlayerMask;
extern s16 gCameraAnchorX;
extern s16 gCameraAnchorY;
extern u8 gRoomEntrySet;
extern s16 gRoomEntryX;
extern s16 gRoomEntryY;
extern struct RoomDef *gCurRoomDef;
extern s8 gUnk_0200B038;
extern u16 gUnk_02008054;
extern s8 gRoomIndex;
extern u8 gUnk_020069F0;
extern u16 gUnk_0200AFF4;
extern u16 gUnk_02008050;
extern u16 gCameraStreamPos[2];
extern u16 gCameraPos[2];
extern u16 gBg3StreamPos[2];
extern u16 gBg3Pos[2];
extern u8 gUnk_020055C8;
extern s16 gUnk_087325A2[];
extern s8 gUnk_02007D64;
extern u8 gUnk_0200AF04;
extern u16 gUnk_030012B0[];
extern u8 gObjPalette[];
extern u8 gBg3MapShape;
extern struct Unk020055D8 gRoomObjectList;
extern s16 gObjectSpawnViewRect[4];
extern struct Unk02004B90 gDoorStates[];
extern u16 gRoomBorder[2];
extern s32 gBg3ParallaxX;
extern u16 gBg3Border[2];
extern s32 gBg3ParallaxY;

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 PlayBgm(s32 songId);
s32 GetCurrentBgm(void);
void PlaySfx(s32 id);
void StopBgm(void);
s32 GetCollisionTile(u32 x, u32 y);
void CalcRoomBounds(void);
void CameraResetBounds(void);
void UpdatePlayerGroupCenter(void);
void SetCameraBoundsToGroup(void);
void UpdatePlayerCameras(void);
void SetPlayerBoundsFromCamera(void);
void SetViewRectToPlayers(void);
void SpawnRoomObjectsInRect(s32 x0, s32 x1, s32 y0, s32 y1);
void DrawBg2Row(s32 x0, s32 x1, s32 y);
void DrawBg2Column(s32 x, s32 y0, s32 y1);
void DrawBg3Row(s32 x0, s32 x1, s32 y);
void DrawBg3Column(s32 x, s32 y0, s32 y1);
s32 sub_080408e4(void);
void CameraUpdatePos(void);

void sub_08028b8c(void)
{
    s32 x;
    s32 y;
    s32 i;

    CalcRoomBounds();
    if (gCameraMode != 5)
    {
        if (gUnk_03002444 != 0 || gPlayerCount == 1)
        {
            *(long long *)gCameraBounds = *(long long *)gRoomBounds;
            gPlayerBounds[gLocalPlayer].x0 = gCameraBounds[0] - 117;
            gPlayerBounds[gLocalPlayer].x1 = gCameraBounds[1] + 117;
            gPlayerBounds[gLocalPlayer].y0 = gCameraBounds[2] - 76;
            gPlayerBounds[gLocalPlayer].y1 = gCameraBounds[3] + 104;
            x = gCameraFocusX;
            y = gCameraFocusY;
            if (x < gCameraBounds[0])
                x = gCameraBounds[0];
            if (x > gCameraBounds[1])
                x = gCameraBounds[1];
            if (y < gCameraBounds[2])
                y = gCameraBounds[2];
            if (y > gCameraBounds[3])
                y = gCameraBounds[3];
            gPlayerCameraPos[gLocalPlayer].x = x;
            gPlayerCameraPos[gLocalPlayer].y = y;
            gCameraCenterX = x << 16;
            gCameraCenterY = y << 16;
            gViewRect[0] = x - 120;
            gViewRect[1] = x + 120;
            gViewRect[2] = y - 80;
            gViewRect[3] = y + 80;
        }
        else
        {
            UpdatePlayerGroupCenter();
            SetCameraBoundsToGroup();
            UpdatePlayerCameras();
            SetPlayerBoundsFromCamera();
            if ((gActivePlayerMask >> gLocalPlayer) & 1)
            {
                x = gCameraFocusX;
                y = gCameraFocusY;
                if (x < gCameraBounds[0])
                    x = gCameraBounds[0];
                if (x > gCameraBounds[1])
                    x = gCameraBounds[1];
                if (y < gCameraBounds[2])
                    y = gCameraBounds[2];
                if (y > gCameraBounds[3])
                    y = gCameraBounds[3];
            }
            else
            {
                x = gPlayerCameraPos[gLocalPlayer].x;
                y = gPlayerCameraPos[gLocalPlayer].y;
            }
            gCameraCenterX = x << 16;
            gCameraCenterY = y << 16;
            SetViewRectToPlayers();
        }
    }
    else
    {
        x = gCameraAnchorX;
        y = gCameraAnchorY;
        if (x < gRoomBounds[0])
            x = gRoomBounds[0];
        if (x > gRoomBounds[1])
            x = gRoomBounds[1];
        if (y < gRoomBounds[2])
            y = gRoomBounds[2];
        if (y > gRoomBounds[3])
            y = gRoomBounds[3];
        gCameraBounds[0] = gCameraBounds[1] = x;
        gCameraBounds[2] = gCameraBounds[3] = y;
        for (i = 0; i < gPlayerCount; i++)
        {
            gPlayerBounds[i].x0 = x - 117;
            gPlayerBounds[i].x1 = x + 117;
            gPlayerBounds[i].y0 = y - 76;
            gPlayerBounds[i].y1 = y + 104;
            gPlayerCameraPos[i].x = x;
            gPlayerCameraPos[i].y = y;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x + 120;
        gViewRect[2] = y - 80;
        gViewRect[3] = y + 80;
    }
}

void sub_08028e3c(void)
{
    CalcRoomBounds();
    CameraResetBounds();
}

void SetRoomEntryPoint(void)
{
    s32 v;

    if (gRoomEntrySet == 0)
    {
        gRoomEntryX = gCurRoomDef->unk50;
        gRoomEntryY = gCurRoomDef->unk52 + 0xFFFD;
        gRoomEntrySet = 0;
    }
    v = gRoomBounds[0] - 117;
    if (gRoomEntryX < v)
        gRoomEntryX = v;
    v = gRoomBounds[1] + 117;
    if (v < gRoomEntryX)
        gRoomEntryX = v;
    v = gRoomBounds[2] - 76;
    if (gRoomEntryY < v)
        gRoomEntryY = v;
    v = gRoomBounds[3] + 104;
    if (v < gRoomEntryY)
        gRoomEntryY = v;
    if (gUnk_0200B038 == 0)
    {
        gUnk_02008054 = gRoomIndex;
        if (gUnk_020069F0 == 2)
        {
            gUnk_0200AFF4 = gCurRoomDef->unk50;
            gUnk_02008050 = gCurRoomDef->unk52;
        }
        else
        {
            gUnk_0200AFF4 = gRoomEntryX;
            gUnk_02008050 = gRoomEntryY;
        }
        gUnk_0200B038 = 0;
    }
    gCameraFocusX = gRoomEntryX;
    gCameraFocusY = gRoomEntryY;
    if (gUnk_03002444 != 0)
    {
        if (gCameraMode == 2 || gCameraMode == 4)
        {
            gCameraAnchorX = gCameraFocusX;
            gCameraAnchorY = gCameraFocusY;
            if (gCameraAnchorX < gRoomBounds[0])
                gCameraAnchorX = gRoomBounds[0];
            if (gCameraAnchorX > gRoomBounds[1])
                gCameraAnchorX = gRoomBounds[1];
            if (gCameraAnchorY < gRoomBounds[2])
                gCameraAnchorY = gRoomBounds[2];
            if (gCameraAnchorY > gRoomBounds[3])
                gCameraAnchorY = gRoomBounds[3];
        }
    }
    else if (gCameraMode == 5)
    {
        gCameraAnchorX = gCameraFocusX;
        gCameraAnchorY = gCameraFocusY;
        if (gCameraAnchorX < gRoomBounds[0])
            gCameraAnchorX = gRoomBounds[0];
        if (gCameraAnchorX > gRoomBounds[1])
            gCameraAnchorX = gRoomBounds[1];
        if (gCameraAnchorY < gRoomBounds[2])
            gCameraAnchorY = gRoomBounds[2];
        if (gCameraAnchorY > gRoomBounds[3])
            gCameraAnchorY = gRoomBounds[3];
    }
}

void sub_08029034(void)
{
    s32 v;

    v = gRoomBounds[0] - 117;
    if (gRoomEntryX < v)
        gRoomEntryX = v;
    v = gRoomBounds[1] + 117;
    if (v < gRoomEntryX)
        gRoomEntryX = v;
    v = gRoomBounds[2] - 76;
    if (gRoomEntryY < v)
        gRoomEntryY = v;
    v = gRoomBounds[3] + 104;
    if (v < gRoomEntryY)
        gRoomEntryY = v;
    gCameraAnchorX = gRoomEntryX;
    gCameraAnchorY = gRoomEntryY;
}

void sub_080290ac(void)
{
    gCameraFocusX = gPlayerCameraPos[gLocalPlayer].x;
    gCameraFocusY = gPlayerCameraPos[gLocalPlayer].y;
}

void CameraInitPos(void)
{
    CameraUpdatePos();
    gCameraStreamPos[0] = gCameraPos[0];
    gCameraStreamPos[1] = gCameraPos[1];
    gBg3StreamPos[0] = gBg3Pos[0];
    gBg3StreamPos[1] = gBg3Pos[1];
}

void sub_08029110(void)
{
    s32 bgm;
    s32 cur;

    if (sub_080408e4() == 0)
    {
        bgm = gCurRoomDef->unk04;
        if (bgm == -1)
        {
            StopBgm();
        }
        else if (gUnk_020055C8 == 0)
        {
            PlayBgm(bgm);
            gUnk_020055C8 = 1;
        }
        else
        {
            cur = GetCurrentBgm();
            if (cur == -1 || cur != bgm)
            {
                if (gUnk_087325A2[bgm] != -1)
                    PlayBgm(gUnk_087325A2[bgm]);
                else
                    PlayBgm(bgm);
            }
        }
    }
    if (gUnk_02007D64 == 4)
        PlaySfx(249);
}

void sub_08029194(void)
{
    if (gUnk_0200AF04 == 0)
    {
        if (gCurRoomDef->unk04 == -1)
            StopBgm();
        else
            PlayBgm(gCurRoomDef->unk04);
    }
    else
    {
        gUnk_0200AF04 = 0;
    }
}

void LoadBg2Gfx(void)
{
    RequestCopy(8, (u32)gCurRoomDef->unk1C, 0x06004000, 0);
    RequestCopy(2, (u32)(gCurRoomDef->unk18 + 1), (u32)gUnk_030012B0, gCurRoomDef->unk18[0]);
}

void LoadBg3Gfx(void)
{
    RequestCopy(8, (u32)gCurRoomDef->unk2C, 0x06008000, 0);
    RequestCopy(2, (u32)(gCurRoomDef->unk28 + 1), (u32)(gObjPalette - gCurRoomDef->unk28[0]), gCurRoomDef->unk28[0]);
}

void ClearBg2Bg3Maps(void)
{
    u32 a;
    u32 b;

    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    b = 0;
    CpuFastSet(&b, (u32 *)0x06003000, 0x01000400);
}

void SelectBg3MapShape(void)
{
    s32 w;
    s32 h;

    w = gCurRoomDef->unk30->unk2;
    h = gCurRoomDef->unk30->unk4;
    if (w <= 64 && h <= 32)
        gBg3MapShape = 0;
    else if (w <= 32 && h <= 64)
        gBg3MapShape = 1;
    else
        gBg3MapShape = 2;
}

void LoadBg3Map(void)
{
    RequestCopy(8, (u32)gCurRoomDef->unk30 + 8, 0x06003000, 0);
}

void SpawnRoomObjectsInView(void)
{
    if (gRoomObjectList.unk0 != 0)
    {
        SpawnRoomObjectsInRect(gViewRect[0] - 36, gViewRect[1] + 36, gViewRect[2] - 40, gViewRect[3] + 40);
        *(long long *)gObjectSpawnViewRect = *(long long *)gViewRect;
    }
}

void InitDoors(void)
{
    struct Door *d;
    s32 i;
    s32 a;
    s32 b;

    d = gCurRoomDef->unk44;
    for (i = 0; i < gCurRoomDef->unk3A; i++)
    {
        gDoorStates[i].unk1 = 1;
        /* The whole byte at +4 (unk4_0/unk4_4) cleared with one strb
           through the struct: two bit-field stores leave the -16 mask and a
           zero in the loop and change what loop.c hoists (0x22B8 then goes
           to sl). */
        gDoorStates[i].filler02[2] = 0;
        a = GetCollisionTile(d->unk2, d->unk4);
        b = GetCollisionTile(d->unk2 + 1, d->unk4);
        if ((a == 16 && b == 55) || (a == 144 && b == 183))
        {
            gDoorStates[i].unk0 = 1;
            gDoorStates[i].unk4_0 = 8;
            gDoorStates[i].unk4_4 = 3;
        }
        else if (d->unk0 == 0x22B8)
        {
            gDoorStates[i].unk0 = 2;
            gDoorStates[i].unk4_0 = 2;
            gDoorStates[i].unk4_4 = 3;
        }
        else
        {
            gDoorStates[i].unk0 = 0;
            gDoorStates[i].unk4_0 = 0;
            gDoorStates[i].unk4_4 = 3;
        }
        d++;
    }
}

void CameraUpdatePos(void)
{
    gCameraPos[0] = (gCameraCenterX >> 16) - 120;
    gCameraPos[1] = (gCameraCenterY >> 16) - 80;
    gBg3Pos[0] = (((gCameraPos[0] - gRoomBorder[0]) * gBg3ParallaxX) >> 16) + gBg3Border[0];
    gBg3Pos[1] = (((gCameraPos[1] - gRoomBorder[1]) * gBg3ParallaxY) >> 16) + gBg3Border[1];
}

void CameraUpdatePosNoParallax(void)
{
    gBg3Pos[0] = gCameraPos[0] = (gCameraCenterX >> 16) - 120;
    gBg3Pos[1] = gCameraPos[1] = (gCameraCenterY >> 16) - 80;
}

void CameraUpdatePosBg3AutoScrollX(void)
{
    gCameraPos[0] = (gCameraCenterX >> 16) - 120;
    gCameraPos[1] = (gCameraCenterY >> 16) - 80;
    gBg3Pos[0] += 0xFFFF;
    gBg3Pos[1] = (((gCameraPos[1] - gRoomBorder[1]) * gBg3ParallaxY) >> 16) + gBg3Border[1];
}

void StreamBg2Map(void)
{
    s32 x, x0, x1, y, y0, y1, d, i;

    x = gCameraPos[0] >> 3;
    x0 = x - 3;
    x1 = x + 32;
    y = gCameraPos[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = x - (gCameraStreamPos[0] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg2Column(x1 - i, y0, y1);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg2Column(x0 + i, y0, y1);
        }
        gCameraStreamPos[0] = gCameraPos[0];
    }
    d = (gCameraPos[1] >> 3) - (gCameraStreamPos[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg2Row(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg2Row(x0, x1, y0 + i);
        }
        gCameraStreamPos[1] = gCameraPos[1];
    }
}

void StreamBg3Map(void)
{
    s32 x, x0, x1, y, y0, y1, d, i;

    x = gBg3Pos[0] >> 3;
    x0 = x - 3;
    x1 = x + 32;
    y = gBg3Pos[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = x - (gBg3StreamPos[0] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg3Column(x1 - i, y0, y1);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg3Column(x0 + i, y0, y1);
        }
        gBg3StreamPos[0] = gBg3Pos[0];
    }
    d = (gBg3Pos[1] >> 3) - (gBg3StreamPos[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg3Row(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg3Row(x0, x1, y0 + i);
        }
        gBg3StreamPos[1] = gBg3Pos[1];
    }
}
