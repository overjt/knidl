#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "mode.h"
#include "collision.h"
#include "room.h"
#include "camera.h"

/* room_28320.c (0x08028320-0x08028B8B, issue #93).
 *
 * Room start-up services.  SpawnDoorObjects clears the door-object slots
 * gDoorObjectTasks[32][3], finds the door the player entered by and spawns
 * an M08 stage object for every locked or special door (a 9-way switch on
 * the door kind, gated by the save flags gHubDoorUnlocks/gStageClearStatus);
 * CalcBg3Parallax computes the BG3 parallax factors, CalcRoomBounds the room
 * bounds, CameraResetBoundsToGroup the multi-player group bounds and CameraResetBounds
 * copies the room bounds into the camera and per-player bounds. */

void SpawnDoorObjects(void)
{
    s32 i, x, y, k, m, r;
    u32 v;
    struct Door *d;

    LoadGfxSet(2);
    gEntryDoorIndex = -1;
    for (i = 0; i < 32; i++)
    {
        gDoorObjectTasks[i][0] = -1;
        gDoorObjectTasks[i][1] = -1;
        gDoorObjectTasks[i][2] = -1;
    }
    gScrollLockSpeedX = 0;
    gScrollLockSpeedY = 0;
    gRoomDriftVelX = 0;
    gRoomDriftVelY = 0;
    gUnk_020055D4 = 0x4000;
    if (gRoomEntryMode == ROOM_ENTRY_DOOR || gRoomEntryMode == ROOM_ENTRY_BIG_SWITCH)
    {
        r = GetCollisionTileAtPixel(gRoomEntryX, gRoomEntryY);
        if (r == 55 || r == 183)
            x = (gRoomEntryX >> 4) - 1;
        else
            x = gRoomEntryX >> 4;
        y = gRoomEntryY >> 4;
        d = gCurRoomDef->doors;
        for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
        {
            if (d->unk2 == x && d->unk4 == y)
            {
                gEntryDoorIndex = i;
                break;
            }
        }
    }
    d = gCurRoomDef->doors;
    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        if (d->unk0 != 0x270F)
            continue;
        x = (d->unk2 << 4) + 16;
        y = (d->unk4 << 4) + 8;
        k = d->unk6 & 0xFF;
        v = gHubDoorUnlocks[gCurLevel][k];
        if (v != 0xFFFF)
        {
            if (v & 0x100)
            {
                if (!(gBigSwitchFlags[0] & (1 << (v & 0xFF))))
                    continue;
            }
            else
            {
                if (!gStageClearStatus[gCurLevel][v])
                    continue;
            }
        }
        switch (k)
        {
        case 0:
            if (gFurthestLevel > gCurLevel || gFurthestStage >= d->unk8)
            {
                switch (gStageClearStatus[gStageIndex][d->unk8])
                {
                default:
                case 0:
                    gDoorObjectTasks[i][0] = CreateStageDoorSign(x, y, d->unk8, i);
                    break;
                case 1:
                    if (i == gEntryDoorIndex && gEntryDoorEvent == 1)
                    {
                        gDoorObjectTasks[i][0] = CreateStageDoorSign(x, y, d->unk8, i);
                    }
                    else
                    {
                        gDoorObjectTasks[i][0] = CreateClearedStageDoorSign(x, y, d->unk8, i);
                        gDoorObjectTasks[i][1] = CreateStageClearFlag(x, y, 15, 0x4000);
                    }
                    break;
                case 2:
                    if (i == gEntryDoorIndex)
                    {
                        if (gRoomEntryMode == ROOM_ENTRY_BIG_SWITCH)
                        {
                            gDoorObjectTasks[i][0] = CreateClearedStageDoorSign(x, y, d->unk8, i);
                            gDoorObjectTasks[i][1] = CreateStageClearFlag(x, y, 15, 0x4000);
                            break;
                        }
                        if (gEntryDoorEvent == 1)
                        {
                            gDoorObjectTasks[i][0] = CreateStageDoorSign(x, y, d->unk8, i);
                            break;
                        }
                    }
                    gDoorObjectTasks[i][0] = CreateCompletedStageDoorSign(x, y, d->unk8, i);
                    gDoorObjectTasks[i][1] = CreateStageClearFlag(x, y, 15, 0x4000);
                    break;
                }
            }
            break;
        case 1:
            gDoorObjectTasks[i][0] = CreateLevelDoorSign(x, y, 0, i);
            break;
        case 2:
            if (gCurLevel >= gFurthestLevel)
                gDoorObjectTasks[i][0] = CreateBossDoorSign(x, y, i);
            else
                gDoorObjectTasks[i][0] = CreateLevelDoorSign(x, y, 1, i);
            break;
        case 3:
            if (!(gUsedSubGameDoors[gStageIndex] & 1))
                gDoorObjectTasks[i][0] = CreateBombRallyDoorSign(x, y, 1, i);
            else if (i == gEntryDoorIndex && gEntryDoorEvent == 2)
                gDoorObjectTasks[i][0] = CreateBombRallyDoorSign(x, y, 1, i);
            else
                gDoorObjectTasks[i][0] = CreateBombRallyDoorSign(x, y, 0, i);
            break;
        case 4:
            if (!(gUsedSubGameDoors[gStageIndex] & 2))
                gDoorObjectTasks[i][0] = CreateAirGrindDoorSign(x, y, 1, i);
            else if (i == gEntryDoorIndex && gEntryDoorEvent == 2)
                gDoorObjectTasks[i][0] = CreateAirGrindDoorSign(x, y, 1, i);
            else
                gDoorObjectTasks[i][0] = CreateAirGrindDoorSign(x, y, 0, i);
            break;
        case 5:
            if (!(gUsedSubGameDoors[gStageIndex] & 4))
                gDoorObjectTasks[i][0] = CreateQuickDrawDoorSign(x, y, 1, i);
            else if (i == gEntryDoorIndex && gEntryDoorEvent == 2)
                gDoorObjectTasks[i][0] = CreateQuickDrawDoorSign(x, y, 1, i);
            else
                gDoorObjectTasks[i][0] = CreateQuickDrawDoorSign(x, y, 0, i);
            break;
        case 6:
            gUnk_020055D4 = 0x4000;
            m = 0;
            if (gHubUnlockFlags != 0)
            {
                if (gHubUnlockFlags & 16)
                    m = 1;
                else
                    gUnk_020055D4 = 0x2000;
            }
            if (gWarpStarStationLevels & ~(1 << gStageIndex))
                gDoorObjectTasks[i][0] = CreateWarpStarStationDoorSign(x, y, 0, i);
            else
                gDoorObjectTasks[i][0] = CreateWarpStarStationDoorSign(x, y, 1, i);
            gDoorObjectTasks[i][1] = CreateWarpStarStationDoorSparkle(x, y, 0, m);
            gDoorObjectTasks[i][2] = CreateWarpStarStationDoorSparkle(x, y, 1, m);
            break;
        case 8:
            gDoorObjectTasks[i][0] = CreateArenaDoorSign(x, y, i);
            break;
        case 7:
            gDoorObjectTasks[i][0] = CreateMuseumDoorSign(x, y, i);
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

    if (gInHub != 0)
    {
        gBg3ParallaxX = 0x10000;
        gBg3ParallaxY = 0x10000;
        return;
    }
    a = gCurRoomDef->bg3Map->width;
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
    a = gCurRoomDef->bg3Map->height;
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
            v = gTasks[i].pixelX;
            if (v < gRoomBounds[0])
                v = gRoomBounds[0];
            if (gRoomBounds[1] < v)
                v = gRoomBounds[1];
            if (v < x0)
                x0 = v;
            if (x1 < v)
                x1 = v;
            v = gTasks[i].pixelY;
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
