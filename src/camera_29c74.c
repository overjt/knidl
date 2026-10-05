#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "camera.h"

/* camera_29c74.c (0x08029C74-0x0802A9CB, issue #86).
 *
 * Multiplayer camera targets.  Player i has a camera position
 * gPlayerCameraPos[i], bounds gPlayerBounds[i] and a mode gPlayerCameraMode[i]
 * (0 follows its task, 2 glides to the group centre, 3 is parked there);
 * gPlayerCount is the player count and gActivePlayerMask the mask of the
 * players present.  UpdatePlayerGroupCenter, SetCameraBoundsToGroupInScrollLock and SetPlayerGroupCenterFromTasks compute
 * the group centre gPlayerGroupCenter (the midpoint of the players' extremes,
 * clamped to the camera bounds gCameraBounds), SetCameraBoundsToGroup re-centres
 * those bounds on it inside the room bounds gRoomBounds,
 * UpdatePlayerCameras/UpdatePlayerCamerasInScrollLock update the per-player positions,
 * SetPlayerBoundsFromCamera/SetPlayerBoundsFromCameraInScrollLock their bounds and SetViewRectToPlayers/SetViewRectToPlayersInScrollLock
 * the visible rectangle gViewRect around them.  LockPlayersPastScrollLine moves
 * the bounds once every player has crossed the scroll line held in the
 * camera control block gScrollLock.  SpawnRoomObjectsInRect spawns the entries
 * of the room's object list gRoomObjectList (sorted along one axis) that
 * lie inside a rectangle, through SpawnRoomObject. */

struct RoomObjectEntry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void UpdatePlayerGroupCenter(void)
{
    s32 x0, x1, y0, y1, x, y, cx, cy, i;

    x0 = gRoomWidth << 4;
    x1 = 0;
    y0 = gRoomHeight << 4;
    y1 = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        switch (gPlayerCameraMode[i])
        {
        case 0:
            y = gTasks[i].pixelX;
            if (y < gCameraBounds[0])
                y = gCameraBounds[0];
            if (gCameraBounds[1] < y)
                y = gCameraBounds[1];
            if (y < x0)
                x0 = y;
            if (x1 < y)
                x1 = y;
            y = gTasks[i].pixelY;
            if (y < gCameraBounds[2])
                y = gCameraBounds[2];
            if (gCameraBounds[3] < y)
                y = gCameraBounds[3];
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 1:
            x = gPlayerCameraPos[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gPlayerCameraPos[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 2:
            break;
        }
    }
    cx = (x0 + x1) >> 1;
    cy = (y0 + y1) >> 1;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerCameraMode[i] == 2)
        {
            if (cx < gPlayerCameraPos[i].x)
            {
                gPlayerCameraPos[i].x -= 6;
                if (gPlayerCameraPos[i].x < cx)
                    gPlayerCameraPos[i].x = cx;
            }
            else
            {
                gPlayerCameraPos[i].x += 6;
                if (cx < gPlayerCameraPos[i].x)
                    gPlayerCameraPos[i].x = cx;
            }
            if (cy < gPlayerCameraPos[i].y)
            {
                gPlayerCameraPos[i].y -= 6;
                if (gPlayerCameraPos[i].y < cy)
                    gPlayerCameraPos[i].y = cy;
            }
            else
            {
                gPlayerCameraPos[i].y += 6;
                if (cy < gPlayerCameraPos[i].y)
                    gPlayerCameraPos[i].y = cy;
            }
            x = gPlayerCameraPos[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gPlayerCameraPos[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            if (cx == x && cy == y)
                gPlayerCameraMode[i] = 3;
        }
    }
    gPlayerGroupCenter[0] = (x0 + x1) >> 1;
    gPlayerGroupCenter[1] = (y0 + y1) >> 1;
}

void SetCameraBoundsToGroup(void)
{
    s32 y, player;

    gCameraBounds[0] = gPlayerGroupCenter[0] - 80;
    gCameraBounds[1] = gPlayerGroupCenter[0] + 80;
    gCameraBounds[2] = gPlayerGroupCenter[1] - 120;
    if (gUnk_0200B078 == 2)
    {
        y = 0;
        for (player = 0; player < gPlayerCount; player++)
        {
            if (gPlayerCameraMode[player] != 3 && y < gPlayerCameraPos[player].y)
                y = gPlayerCameraPos[player].y;
        }
        if (y < gCameraBounds[3])
            gCameraBounds[3] = y;
    }
    else
    {
        gCameraBounds[3] = gPlayerGroupCenter[1] + 120;
    }
    if (gCameraBounds[0] < gRoomBounds[0])
        gCameraBounds[0] = gRoomBounds[0];
    if (gRoomBounds[1] < gCameraBounds[1])
        gCameraBounds[1] = gRoomBounds[1];
    if (gCameraBounds[2] < gRoomBounds[2])
        gCameraBounds[2] = gRoomBounds[2];
    if (gRoomBounds[3] < gCameraBounds[3])
        gCameraBounds[3] = gRoomBounds[3];
}

void SetCameraBoundsToGroupInScrollLock(void)
{
    s32 x0, x1, y0, y1, x, y, cx, cy, i;
    s16 t;

    x0 = gRoomWidth << 4;
    x1 = 0;
    y0 = gRoomHeight << 4;
    y1 = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        switch (gPlayerCameraMode[i])
        {
        case 0:
            if ((gScrollLock.unk0 >> i) & 1)
                break;
            x = gTasks[i].pixelX;
            if (x < gCameraBounds[0])
                x = gCameraBounds[0];
            if (gCameraBounds[1] < x)
                x = gCameraBounds[1];
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gTasks[i].pixelY;
            if (y < gCameraBounds[2])
                y = gCameraBounds[2];
            if (gCameraBounds[3] < y)
                y = gCameraBounds[3];
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 1:
            x = gPlayerCameraPos[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gPlayerCameraPos[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 2:
            x = gPlayerCameraPos[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gPlayerCameraPos[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        }
    }
    if (gScrollLock.lockedAxes & 1)
    {
        if (gScrollLockSpeedX > 0)
            x1 = gScrollLock.unkA;
        else
            x0 = gScrollLock.unkA;
    }
    if (gScrollLock.lockedAxes & 2)
    {
        if (gScrollLockSpeedY > 0)
            y1 = gScrollLock.unkC;
        else
            y0 = gScrollLock.unkC;
    }
    cx = (x0 + x1) >> 1;
    cy = (y0 + y1) >> 1;
    if (!((gScrollLock.unk0 >> gPlayerCount) & 1) || !(gScrollLock.lockedAxes & 1))
    {
        gCameraBounds[0] = t = cx - 80;
        gCameraBounds[1] = cx + 80;
        if (t < gRoomBounds[0])
            gCameraBounds[0] = gRoomBounds[0];
        if (gRoomBounds[1] < gCameraBounds[1])
            gCameraBounds[1] = gRoomBounds[1];
    }
    if (!((gScrollLock.unk0 >> gPlayerCount) & 1) || !(gScrollLock.lockedAxes & 2))
    {
        gCameraBounds[2] = cy - 120;
        if (gUnk_0200B078 == 2)
        {
            y1 = 0;
            for (i = 0; i < gPlayerCount; i++)
            {
                if (gPlayerCameraMode[i] != 3 && !(((gScrollLock.unk0 & gActivePlayerMask) >> i) & 1)
                    && y1 < gPlayerCameraPos[i].y)
                    y1 = gPlayerCameraPos[i].y;
            }
            if (y1 < gCameraBounds[3])
                gCameraBounds[3] = y1;
        }
        else
        {
            gCameraBounds[3] = cy + 120;
        }
        if (gCameraBounds[2] < gRoomBounds[2])
            gCameraBounds[2] = gRoomBounds[2];
        if (gRoomBounds[3] < gCameraBounds[3])
            gCameraBounds[3] = gRoomBounds[3];
    }
}

void UpdatePlayerCameras(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        switch (gPlayerCameraMode[i])
        {
        case 0:
            gPlayerCameraPos[i].x = gTasks[i].pixelX;
            if (gPlayerCameraPos[i].x < gCameraBounds[0])
                gPlayerCameraPos[i].x = gCameraBounds[0];
            if (gCameraBounds[1] < gPlayerCameraPos[i].x)
                gPlayerCameraPos[i].x = gCameraBounds[1];
            gPlayerCameraPos[i].y = gTasks[i].pixelY;
            if (gPlayerCameraPos[i].y < gCameraBounds[2])
                gPlayerCameraPos[i].y = gCameraBounds[2];
            if (gCameraBounds[3] < gPlayerCameraPos[i].y)
                gPlayerCameraPos[i].y = gCameraBounds[3];
            break;
        case 1:
        case 2:
            break;
        case 3:
            gPlayerCameraPos[i].x = gPlayerGroupCenter[0];
            gPlayerCameraPos[i].y = gPlayerGroupCenter[1];
            break;
        }
    }
}

void UpdatePlayerCamerasInScrollLock(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        switch (gPlayerCameraMode[i])
        {
        case 0:
            if ((gScrollLock.unk0 >> i) & 1)
                break;
            gPlayerCameraPos[i].x = gTasks[i].pixelX;
            if (gPlayerCameraPos[i].x < gCameraBounds[0])
                gPlayerCameraPos[i].x = gCameraBounds[0];
            if (gCameraBounds[1] < gPlayerCameraPos[i].x)
                gPlayerCameraPos[i].x = gCameraBounds[1];
            gPlayerCameraPos[i].y = gTasks[i].pixelY;
            if (gPlayerCameraPos[i].y < gCameraBounds[2])
                gPlayerCameraPos[i].y = gCameraBounds[2];
            if (gCameraBounds[3] < gPlayerCameraPos[i].y)
                gPlayerCameraPos[i].y = gCameraBounds[3];
            break;
        case 1:
        case 2:
            break;
        case 3:
            gPlayerCameraPos[i].x = gPlayerGroupCenter[0];
            gPlayerCameraPos[i].y = gPlayerGroupCenter[1];
            break;
        }
    }
}

void SetPlayerGroupCenterFromTasks(void)
{
    s32 x0, x1, y0, y1, v, i;

    x0 = gRoomWidth << 4;
    x1 = 0;
    y0 = gRoomHeight << 4;
    y1 = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            v = gTasks[i].pixelX;
            if (v < gCameraBounds[0])
                v = gCameraBounds[0];
            if (gCameraBounds[1] < v)
                v = gCameraBounds[1];
            if (v < x0)
                x0 = v;
            if (x1 < v)
                x1 = v;
            v = gTasks[i].pixelY;
            if (v < gCameraBounds[2])
                v = gCameraBounds[2];
            if (gCameraBounds[3] < v)
                v = gCameraBounds[3];
            if (v < y0)
                y0 = v;
            if (y1 < v)
                y1 = v;
        }
    }
    gPlayerGroupCenter[0] = (x0 + x1) >> 1;
    gPlayerGroupCenter[1] = (y0 + y1) >> 1;
}

void SetPlayerBoundsFromCamera(void)
{
    s32 i;

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

void SetPlayerBoundsFromCameraInScrollLock(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (((gActivePlayerMask >> i) & 1) && !((gScrollLock.unk0 >> i) & 1))
        {
            gPlayerBounds[i].x0 = gCameraBounds[0] - 117;
            gPlayerBounds[i].x1 = gCameraBounds[1] + 117;
            gPlayerBounds[i].y0 = gCameraBounds[2] - 76;
            gPlayerBounds[i].y1 = gCameraBounds[3] + 104;
        }
    }
}

void SetViewRectToPlayers(void)
{
    s32 x0, x1, y0, y1, v, player;

    x0 = gRoomWidth << 4;
    x1 = 0;
    y0 = gRoomHeight << 4;
    y1 = 0;
    for (player = 0; player < gPlayerCount; player++)
    {
        v = gPlayerCameraPos[player].x;
        if (v < x0)
            x0 = v;
        if (x1 < v)
            x1 = v;
        v = gPlayerCameraPos[player].y;
        if (v < y0)
            y0 = v;
        if (y1 < v)
            y1 = v;
    }
    gViewRect[0] = x0 - 120;
    gViewRect[1] = x1 + 120;
    gViewRect[2] = y0 - 80;
    gViewRect[3] = y1 + 80;
}

void SetViewRectToPlayersInScrollLock(void)
{
    s32 x0, x1, y0, y1, v, player;

    x0 = gRoomWidth << 4;
    x1 = 0;
    y0 = gRoomHeight << 4;
    y1 = 0;
    for (player = 0; player < gPlayerCount; player++)
    {
        v = gPlayerCameraPos[player].x;
        if (v < x0)
            x0 = v;
        if (x1 < v)
            x1 = v;
        v = gPlayerCameraPos[player].y;
        if (v < y0)
            y0 = v;
        if (y1 < v)
            y1 = v;
    }
    if (gScrollLock.lockedAxes & 1)
    {
        v = gPlayerCameraPos[gPlayerCount].x;
        if (v < x0)
            x0 = v;
        if (x1 < v)
            x1 = v;
    }
    if (gScrollLock.lockedAxes & 2)
    {
        v = gPlayerCameraPos[gPlayerCount].y;
        if (v < y0)
            y0 = v;
        if (y1 < v)
            y1 = v;
    }
    gViewRect[0] = x0 - 120;
    gViewRect[1] = x1 + 120;
    gViewRect[2] = y0 - 80;
    gViewRect[3] = y1 + 80;
}

void LockPlayersPastScrollLine(void)
{
    s32 i;

    if ((gScrollLock.unk0 >> gPlayerCount) & 1)
        return;
    if (gScrollLock.lockedAxes & 1)
    {
        for (i = 0; i < gPlayerCount; i++)
        {
            if (((gActivePlayerMask >> i) & 1) && !((gScrollLock.unk0 >> i) & 1))
            {
                if (gScrollLockSpeedX > 0 && gScrollLock.unkA <= gPlayerCameraPos[i].x)
                {
                    gScrollLock.unk0 |= 1 << i;
                    gPlayerBounds[i].x0 = gPlayerCameraPos[i].x + 0xFF8B;
                    gPlayerBounds[i].x1 = gScrollLock.x1 + 117;
                }
                else if (gScrollLockSpeedX < 0 && gPlayerCameraPos[i].x <= gScrollLock.unkA)
                {
                    gScrollLock.unk0 |= 1 << i;
                    gPlayerBounds[i].x0 = gScrollLock.x0 + 0xFF8B;
                    gPlayerBounds[i].x1 = gPlayerCameraPos[i].x + 117;
                }
            }
        }
    }
    if (gScrollLock.lockedAxes & 2)
    {
        for (i = 0; i < gPlayerCount; i++)
        {
            if (((gActivePlayerMask >> i) & 1) && !((gScrollLock.unk0 >> i) & 1))
            {
                if (gScrollLockSpeedY > 0 && gScrollLock.unkC <= gPlayerCameraPos[i].y)
                {
                    gScrollLock.unk0 |= 1 << i;
                    gPlayerBounds[i].y0 = gPlayerCameraPos[i].y + 0xFFB4;
                    gPlayerBounds[i].y1 = gScrollLock.y1 + 104;
                }
                else if (gScrollLockSpeedY < 0 && gPlayerCameraPos[i].y <= gScrollLock.unkC)
                {
                    gScrollLock.unk0 |= 1 << i;
                    gPlayerBounds[i].y0 = gScrollLock.y0 + 0xFFB4;
                    gPlayerBounds[i].y1 = gPlayerCameraPos[i].y + 104;
                }
            }
        }
    }
    if ((gScrollLock.unk0 & gActivePlayerMask) == gActivePlayerMask)
    {
        gScrollLock.unk0 |= 1 << gPlayerCount;
        if (gScrollLock.lockedAxes & 1)
        {
            if (gScrollLockSpeedX > 0)
                gCameraBounds[0] = gScrollLock.unkA;
            else
                gCameraBounds[1] = gScrollLock.unkA;
        }
        if (gScrollLock.lockedAxes & 2)
        {
            if (gScrollLockSpeedY > 0)
                gCameraBounds[2] = gScrollLock.unkC;
            else
                gCameraBounds[3] = gScrollLock.unkC;
        }
    }
}

void SpawnRoomObjectsInRect(s32 x0, s32 x1, s32 y0, s32 y1)
{
    s32 i;
    struct RoomObjectEntry *e;

    if (x0 < 0)
        x0 = 0;
    if (x1 < 0)
        x1 = 0;
    if (x0 > gRoomWidth << 4)
        x0 = gRoomWidth << 4;
    if (x1 > gRoomWidth << 4)
        x1 = gRoomWidth << 4;
    if (y0 < 0)
        y0 = 0;
    if (y1 < 0)
        y1 = 0;
    if (y0 > gRoomHeight << 4)
        y0 = gRoomHeight << 4;
    if (y1 > gRoomHeight << 4)
        y1 = gRoomHeight << 4;
    if (gRoomObjectList.sortedByY != 0)
    {
        if ((y1 + y0) >> 1 >= gRoomHeight << 3)
        {
            for (i = gRoomObjectList.count - 1; i >= 0; i--)
            {
                e = &gRoomObjectList.entries[i];
                if (e->y >= y1)
                    continue;
                if (e->y < y0)
                    break;
                if (e->x >= x0 && e->x < x1)
                    SpawnRoomObject(i);
            }
        }
        else
        {
            for (i = 0; i < gRoomObjectList.count; i++)
            {
                e = &gRoomObjectList.entries[i];
                if (e->y < y0)
                    continue;
                if (e->y >= y1)
                    break;
                if (e->x >= x0 && e->x < x1)
                    SpawnRoomObject(i);
            }
        }
    }
    else
    {
        if ((x1 + x0) >> 1 >= gRoomWidth << 3)
        {
            for (i = gRoomObjectList.count - 1; i >= 0; i--)
            {
                e = &gRoomObjectList.entries[i];
                if (e->x >= x1)
                    continue;
                if (e->x < x0)
                    break;
                if (e->y >= y0 && e->y < y1)
                    SpawnRoomObject(i);
            }
        }
        else
        {
            for (i = 0; i < gRoomObjectList.count; i++)
            {
                e = &gRoomObjectList.entries[i];
                if (e->x < x0)
                    continue;
                if (e->x >= x1)
                    break;
                if (e->y >= y0 && e->y < y1)
                    SpawnRoomObject(i);
            }
        }
    }
}
