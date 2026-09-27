#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "camera.h"

/* camera_296a0.c (0x080296A0-0x08029C73, issue #86).
 *
 * Per-frame camera -> BG glue.  gCameraPos is the camera position in
 * pixels and gCameraStreamPos the position the BG maps were last streamed at.
 * sub_080296a0, StreamBg123Maps, StreamBg23Maps and StreamBg23Rows compare the two
 * in 8-pixel tiles and stream the tile columns and rows that scrolled into
 * view (bgmap_2a9cc.c), one per room layout; sub_08029b30 streams the whole
 * view.  CameraWriteScrollParallax, CameraWriteScrollHBlank, CameraWriteScrollBg23 and CameraWriteScrollBg123 write
 * the 16.16 BG scroll shadows and the sprite camera
 * gSpriteCameraX/gSpriteCameraY from the camera plus the screen-shake offset
 * gScreenShake (BG3 follows gBg3Pos, at half the shake unless
 * gUnk_02000000 is set).  SpawnRoomObjectsScrolledIn calls SpawnRoomObjectsInRect for every edge of
 * the visible rectangle gViewRect that moved past the previous one,
 * gObjectSpawnViewRect, while gRoomObjectList is set, then remembers it. */

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void sub_08023f18(void);
void SpawnRoomObjectsInRect(s32 x0, s32 x1, s32 y0, s32 y1);
void sub_0802aae8(s32 x);
void DrawBg123Row(s32 x0, s32 x1, s32 y);
void DrawBg123Column(s32 x, s32 y0, s32 y1);
void DrawBg23Row(s32 x0, s32 x1, s32 y);
void DrawBg23Column(s32 x, s32 y0, s32 y1);
void DrawBg23FullRow(s32 y);
void sub_0802b25c(s32 x);

void sub_080296a0(void)
{
    s32 x, x0, x1, d;

    x = gCameraPos[0] >> 3;
    x0 = x - 1;
    x1 = x + 30;
    d = x - (gCameraStreamPos[0] >> 3);
    if (d > 0)
    {
        gUnk_03001F2C = x0 - 8;
        gUnk_03002448 = x + 38;
        if (!(x1 & 1))
        {
            sub_0802b25c(gUnk_03001F2C >> 1);
            sub_0802b25c(gUnk_03002448 >> 1);
        }
        sub_0802aae8(x1);
        gCameraStreamPos[0] = gCameraPos[0];
    }
    else if (d < 0)
    {
        sub_0802aae8(x0);
        gCameraStreamPos[0] = gCameraPos[0];
    }
}

void StreamBg123Maps(void)
{
    s32 x, x0, x1, y, y0, y1, d, i;

    x = gCameraPos[0] >> 3;
    x0 = x - 1;
    x1 = x + 30;
    y = gCameraPos[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = x - (gCameraStreamPos[0] >> 3);
    if (d > 0)
    {
        DrawBg123Column(x1, y0, y1);
        gCameraStreamPos[0] = gCameraPos[0];
    }
    else if (d < 0)
    {
        DrawBg123Column(x0, y0, y1);
        gCameraStreamPos[0] = gCameraPos[0];
    }
    d = (gCameraPos[1] >> 3) - (gCameraStreamPos[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg123Row(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg123Row(x0, x1, y0 + i);
        }
        gCameraStreamPos[1] = gCameraPos[1];
    }
}

void StreamBg23Maps(void)
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
                DrawBg23Column(x1 - i, y0, y1);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg23Column(x0 + i, y0, y1);
        }
        gCameraStreamPos[0] = gCameraPos[0];
    }
    d = (gCameraPos[1] >> 3) - (gCameraStreamPos[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg23Row(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg23Row(x0, x1, y0 + i);
        }
        gCameraStreamPos[1] = gCameraPos[1];
    }
}

void StreamBg23Rows(void)
{
    s32 y, y0, y1, d, i;

    y = gCameraPos[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = y - (gCameraStreamPos[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                DrawBg23FullRow(y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                DrawBg23FullRow(y0 + i);
        }
        gCameraStreamPos[1] = gCameraPos[1];
    }
}

void CameraWriteScrollParallax(void)
{
    gBg2ScrollX = (gCameraPos[0] + gScreenShake.unk2) << 16;
    gBg2ScrollY = (gCameraPos[1] + gScreenShake.unk4) << 16;
    if (gUnk_02000000 != 0)
    {
        gBg3ScrollX = (gBg3Pos[0] + gScreenShake.unk2) << 16;
        gBg3ScrollY = (gBg3Pos[1] + gScreenShake.unk4) << 16;
    }
    else
    {
        gBg3ScrollX = (gBg3Pos[0] + (gScreenShake.unk2 >> 1)) << 16;
        gBg3ScrollY = gBg3Pos[1] << 16;
    }
    gSpriteCameraX = gCameraPos[0] + gScreenShake.unk2;
    gSpriteCameraY = gCameraPos[1] + gScreenShake.unk4;
}

void CameraWriteScrollHBlank(void)
{
    gBg2ScrollY = (gCameraPos[1] + gScreenShake.unk4) << 16;
    gBg3ScrollY = (gBg3Pos[1] + gScreenShake.unk4) << 16;
    gSpriteCameraX = gCameraPos[0] + gScreenShake.unk2;
    gSpriteCameraY = gCameraPos[1] + gScreenShake.unk4;
    gUnk_02016C30 = gCameraPos[0];
}

void CameraWriteScrollBg23(void)
{
    gBg3ScrollX = gBg2ScrollX = (gCameraPos[0] + gScreenShake.unk2) << 16;
    gBg3ScrollY = gBg2ScrollY = (gCameraPos[1] + gScreenShake.unk4) << 16;
    gSpriteCameraX = gCameraPos[0] + gScreenShake.unk2;
    gSpriteCameraY = gCameraPos[1] + gScreenShake.unk4;
}

void CameraWriteScrollBg123(void)
{
    gBg1ScrollX = gBg3ScrollX = gBg2ScrollX = (gCameraPos[0] + gScreenShake.unk2) << 16;
    gBg1ScrollY = gBg3ScrollY = gBg2ScrollY = (gCameraPos[1] + gScreenShake.unk4) << 16;
    gSpriteCameraX = gCameraPos[0] + gScreenShake.unk2;
    gSpriteCameraY = gCameraPos[1] + gScreenShake.unk4;
}

void sub_08029b30(void)
{
    s32 x, x1, y0, y1;

    y0 = (gCameraPos[1] >> 3) - 3;
    y1 = (gCameraPos[1] >> 3) + 22;
    x1 = (gCameraPos[0] >> 3) - 1;
    for (x = (gCameraPos[0] >> 3) - 3; x <= x1; x++)
        DrawBg23Column(x, y0, y1);
    x1 = (gCameraPos[0] >> 3) + 32;
    for (x = (gCameraPos[0] >> 3) + 30; x <= x1; x++)
        DrawBg23Column(x, y0, y1);
    gCameraStreamPos[0] = gCameraPos[0];
    gCameraStreamPos[1] = gCameraPos[1];
    gUnk_03004B00.unk8 = (u32)sub_08023f18;
    gBg1ScrollX = gBg1ScrollY = 0;
}

void SpawnRoomObjectsScrolledIn(void)
{
    if (gRoomObjectList.count != 0)
    {
        if (gViewRect[0] < gObjectSpawnViewRect[0])
            SpawnRoomObjectsInRect(gObjectSpawnViewRect[0] - 36, gObjectSpawnViewRect[0] - 28, gObjectSpawnViewRect[2] - 40, gObjectSpawnViewRect[3] + 40);
        if (gObjectSpawnViewRect[1] < gViewRect[1])
            SpawnRoomObjectsInRect(gObjectSpawnViewRect[1] + 28, gObjectSpawnViewRect[1] + 36, gObjectSpawnViewRect[2] - 40, gObjectSpawnViewRect[3] + 40);
        if (gViewRect[2] < gObjectSpawnViewRect[2])
            SpawnRoomObjectsInRect(gObjectSpawnViewRect[0] - 36, gObjectSpawnViewRect[1] + 36, gObjectSpawnViewRect[2] - 40, gObjectSpawnViewRect[2] - 32);
        if (gObjectSpawnViewRect[3] < gViewRect[3])
            SpawnRoomObjectsInRect(gObjectSpawnViewRect[0] - 36, gObjectSpawnViewRect[1] + 36, gObjectSpawnViewRect[3] + 32, gObjectSpawnViewRect[3] + 40);
        *(long long *)gObjectSpawnViewRect = *(long long *)gViewRect;
    }
}
