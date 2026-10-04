#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "camera.h"

/* stage_273a0.c (0x080273A0-0x08027A6B, issue #93).
 *
 * Stage helpers, part 3.  ReturnToRestartPoint picks the arrival door after a
 * level change (the stage door kind in gUnk_02008054), sub_08027548 and
 * sub_08027588/sub_080275cc keep the two-player race record
 * gUnk_02006098 (flags|0x80, lo, hi, previous, direction),
 * HoldPlayerCamera/ReleaseDeadPlayerView/AreInactivePlayerCamerasParked/ArePlayerCamerasDoneGliding the per-player
 * camera modes gPlayerCameraMode, and CameraStartHoldAnchorAt, CameraStartFollowFocusAt,
 * CameraStartFollowingPlayer and CameraStartPlayersAtAnchor set the camera mode and target
 * (gCameraAnchorX/gCameraAnchorY) for one or all players. */

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void sub_08009e2c(s32 a);

void ReturnToRestartPoint(void)
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
        d = r->doors;
        for (i = 0; i < r->doorCount; d++, i++)
        {
            if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == k)
                break;
        }
        gRoomEntryX = (d->unk2 << 4) + 22;
        gRoomEntryY = (d->unk4 << 4) + 5;
        gRoomEntrySet = 1;
        gEntryDoorEvent = 0;
        gRoomEntryMode = ROOM_ENTRY_DOOR;
        gGameState = GAME_STATE_HUB;
    }
    else
    {
        if (gCurLevel == 7)
        {
            gUnk_02007FF0++;
            if (gUnk_02007FF0 > 5)
                gUnk_02007FF0 = 5;
        }
        gRoomIndex = gUnk_02008054;
        gRoomEntryX = gUnk_0200AFF4;
        gRoomEntryY = gUnk_02008050;
        gRoomEntrySet = 1;
        gEntryDoorEvent = 0;
        gRoomEntryMode = ROOM_ENTRY_NORMAL;
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

s32 HoldPlayerCamera(s32 a)
{
    gPlayerCameraMode[gCurTask->player->playerIndex] = 1;
    return a;
}

s32 ReleaseDeadPlayerView(s32 i)
{
    if (gPlayerCount > 1 && gActivePlayerMask != 0)
    {
        if (gCameraMode != CAMERA_MODE_HOLD_ANCHOR)
            gPlayerCameraMode[i] = 2;
        else
            gPlayerCameraMode[i] = 3;
        if (i == gLocalPlayer)
        {
            if (gPlayerLives[i] != 0)
                HudShowAbilityAnimated(ABILITY_PICTURE_WAIT, gCurTask->player->playerIndex);
            else
                sub_08009e2c(i);
        }
    }
}

s32 AreInactivePlayerCamerasParked(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (((gActivePlayerMask >> i) & 1) == 0 && gPlayerCameraMode[i] != 3)
            return 0;
    }
    return 1;
}

void CameraStartHoldAnchorAt(s32 x, s32 y)
{
    gCameraAnchorX = x;
    gCameraAnchorY = y;
    gCameraCenterX = gCameraAnchorX << 16;
    gCameraCenterY = gCameraAnchorY << 16;
    if (gInHub != 0)
        gCameraMode = 4;
    else
        gCameraMode = CAMERA_MODE_HOLD_ANCHOR;
}

void CameraStartFollowFocusAt(s32 x, s32 y)
{
    gCameraFocusX = x;
    gCameraFocusY = y;
    if (gInHub != 0)
        gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
    else
        gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
    if (gUnk_02007D64 != 2)
    {
        if (gInHub != 0)
            CalcRoomAndCameraBounds();
        else
            CameraResetRoomView();
    }
}

void CameraStartFollowingPlayer(s32 a)
{
    s32 i;
    s32 x;
    s32 y;

    if (gPlayerCount == 1)
    {
        gCameraFocusPlayer = gLocalPlayer;
        gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
    }
    else
    {
        gCameraFocusPlayer = a;
        gCameraMode = CAMERA_MODE_FOLLOW_PLAYER;
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

void CameraStartPlayersAtAnchor(void)
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
        gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
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
            if (i == gCameraFocusPlayer)
                gPlayerCameraMode[i] = 0;
            else
                gPlayerCameraMode[i] = 2;
        }
        *m = 3;
    }
}

s32 ArePlayerCamerasDoneGliding(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerCameraMode[i] == 2)
            return 0;
    }
    return 1;
}

void CameraResumeFollowFocus(void)
{
    gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
}
