#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"

/* room_camera_init.c (0x08028B8C-0x0802969F, issue #93).
 *
 * Camera start-up for a new room.  CameraResetRoomView (camera modes other than
 * 5) copies the room bounds into the camera bounds and puts the camera,
 * its 16.16 target and the visible rectangle on the player (one player)
 * or runs M08's multi-player updates; CalcRoomAndCameraBounds only sets the bounds.
 * SetRoomEntryPoint places the player at the room's start position
 * (RoomDef.entryX/unk52) unless a door already did, clamps it and records
 * the arrival for the next level change (gRestartPoint, gRestartPointX,
 * gRestartPointY).  ClampRoomEntryAndAnchorCamera clamps the arrival position into the room
 * and makes it the camera target, CameraSetFocusToLocalPlayer copies the current
 * player's camera position into the player cells, CameraInitPos and
 * CameraUpdatePos/CameraUpdatePosNoParallax/CameraUpdatePosBg3AutoScrollX
 * set the camera and BG3 positions (BG3 moves by the parallax factors of
 * room_spawn_door_objects.c), StreamBg2Map/StreamBg3Map stream the BG and BG3 maps
 * for a camera move (M08's StreamBg23Maps again), PlayRoomBgm/
 * PlayHubRoomBgm start the room's BGM, LoadBg2Gfx ... SpawnRoomObjectsInView load
 * the room palettes, tiles and BG map (SelectBg3MapShape picks the BG layout
 * gBg3MapShape from the map size) and InitDoors builds the door
 * objects. */

struct RoomObjectEntry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void PlaySfx(s32 id);

void CameraResetRoomView(void)
{
    s32 x;
    s32 y;
    s32 player;

    CalcRoomBounds();
    if (gCameraMode != CAMERA_MODE_HOLD_ANCHOR)
    {
        if (gInHub != 0 || gPlayerCount == 1)
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
        for (player = 0; player < gPlayerCount; player++)
        {
            gPlayerBounds[player].x0 = x - 117;
            gPlayerBounds[player].x1 = x + 117;
            gPlayerBounds[player].y0 = y - 76;
            gPlayerBounds[player].y1 = y + 104;
            gPlayerCameraPos[player].x = x;
            gPlayerCameraPos[player].y = y;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x + 120;
        gViewRect[2] = y - 80;
        gViewRect[3] = y + 80;
    }
}

void CalcRoomAndCameraBounds(void)
{
    CalcRoomBounds();
    CameraResetBounds();
}

void SetRoomEntryPoint(void)
{
    s32 v;

    if (gRoomEntrySet == 0)
    {
        gRoomEntryX = gCurRoomDef->entryX;
        gRoomEntryY = gCurRoomDef->entryY + 0xFFFD;
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
        gRestartPoint = gRoomIndex;
        if (gRoomEntryMode == ROOM_ENTRY_WARP_STAR)
        {
            gRestartPointX = gCurRoomDef->entryX;
            gRestartPointY = gCurRoomDef->entryY;
        }
        else
        {
            gRestartPointX = gRoomEntryX;
            gRestartPointY = gRoomEntryY;
        }
        gUnk_0200B038 = 0;
    }
    gCameraFocusX = gRoomEntryX;
    gCameraFocusY = gRoomEntryY;
    if (gInHub != 0)
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
    else if (gCameraMode == CAMERA_MODE_HOLD_ANCHOR)
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

void ClampRoomEntryAndAnchorCamera(void)
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

void CameraSetFocusToLocalPlayer(void)
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

void PlayRoomBgm(void)
{
    s32 bgm;
    s32 cur;

    if (sub_080408e4() == 0)
    {
        bgm = gCurRoomDef->bgm;
        if (bgm == -1)
        {
            StopBgm();
        }
        else if (gRoomBgmStarted == 0)
        {
            PlayBgm(bgm);
            gRoomBgmStarted = 1;
        }
        else
        {
            cur = GetCurrentBgm();
            if (cur == -1 || cur != bgm)
            {
                if (gRoomBgmRemap[bgm] != -1)
                    PlayBgm(gRoomBgmRemap[bgm]);
                else
                    PlayBgm(bgm);
            }
        }
    }
    if (gUnk_02007D64 == 4)
        PlaySfx(249);
}

void PlayHubRoomBgm(void)
{
    if (gSkipNextHubBgm == 0)
    {
        if (gCurRoomDef->bgm == -1)
            StopBgm();
        else
            PlayBgm(gCurRoomDef->bgm);
    }
    else
    {
        gSkipNextHubBgm = 0;
    }
}

void LoadBg2Gfx(void)
{
    RequestCopy(8, (u32)gCurRoomDef->bg2Tiles, BG_VRAM + 0x4000, 0);
    RequestCopy(2, (u32)(gCurRoomDef->bg2Palette + 1), (u32)gBgPaletteBank2, gCurRoomDef->bg2Palette[0]);
}

void LoadBg3Gfx(void)
{
    RequestCopy(8, (u32)gCurRoomDef->bg3Tiles, BG_VRAM + 0x8000, 0);
    RequestCopy(2, (u32)(gCurRoomDef->bg3Palette + 1), (u32)(gObjPalette - gCurRoomDef->bg3Palette[0]), gCurRoomDef->bg3Palette[0]);
}

void ClearBg2Bg3Maps(void)
{
    u32 a;
    u32 b;

    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    b = 0;
    CpuFastSet(&b, (u32 *)(BG_VRAM + 0x3000), 0x01000400);
}

void SelectBg3MapShape(void)
{
    s32 w;
    s32 h;

    w = gCurRoomDef->bg3Map->width;
    h = gCurRoomDef->bg3Map->height;
    if (w <= 64 && h <= 32)
        gBg3MapShape = 0;
    else if (w <= 32 && h <= 64)
        gBg3MapShape = 1;
    else
        gBg3MapShape = 2;
}

void LoadBg3Map(void)
{
    RequestCopy(8, (u32)gCurRoomDef->bg3Map + 8, BG_VRAM + 0x3000, 0);
}

void SpawnRoomObjectsInView(void)
{
    if (gRoomObjectList.count != 0)
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

    d = gCurRoomDef->doors;
    for (i = 0; i < gCurRoomDef->doorCount; i++)
    {
        gDoorStates[i].isOpen = 1;
        /* The whole byte at +4 (unk4_0/unk4_4) cleared with one strb
           through the struct: two bit-field stores leave the -16 mask and a
           zero in the loop and change what loop.c hoists (0x22B8 then goes
           to sl). */
        gDoorStates[i].filler02[2] = 0;
        a = GetCollisionTile(d->unk2, d->unk4);
        b = GetCollisionTile(d->unk2 + 1, d->unk4);
        if ((a == 16 && b == 55) || (a == 144 && b == 183))
        {
            gDoorStates[i].overlayKind = 1;
            gDoorStates[i].overlayFrame = 8;
            gDoorStates[i].overlayTimer = 3;
        }
        else if (d->unk0 == 0x22B8)
        {
            gDoorStates[i].overlayKind = 2;
            gDoorStates[i].overlayFrame = 2;
            gDoorStates[i].overlayTimer = 3;
        }
        else
        {
            gDoorStates[i].overlayKind = 0;
            gDoorStates[i].overlayFrame = 0;
            gDoorStates[i].overlayTimer = 3;
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
