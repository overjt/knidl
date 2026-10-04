#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "room.h"
#include "camera.h"

/* camera_2c42c.c (0x0802C42C-0x0802D01B, issue #86).
 *
 * Camera modes, the screen shake and the scroll-lock setup.
 * CameraHoldAnchor, CameraSnapBoundsToAnchor and CameraSnapPlayersToAnchor snap the camera, and the
 * per-player cameras and bounds, to the spawn point
 * gCameraAnchorX/gCameraAnchorY clamped to the room bounds gRoomBounds
 * (when gActivePlayerCount is set).  HubCameraFollowFocus, HubCameraFollowFocusPlayer, HubCameraGlideToPlayers
 * and CameraSnapToFocus follow the player (gCameraFocusX/gCameraFocusY, the
 * task gCameraFocusPlayer or the multiplayer group of camera_29c74.c) inside
 * the bounds gCameraBounds and write the 16.16 target
 * gCameraCenterX/gCameraCenterY and the visible rectangle gViewRect.
 * StopScreenShake clears and UpdateScreenShake steps the screen shake
 * gScreenShake through the offset lists of gScreenShakePatterns (0x8000 ends a
 * list, 0x9999 restarts it).  StartScrollLock(x0, x1, y0, y1) arms the scroll
 * lock of gScrollLock for a room region, 0xFFFF meaning no limit on
 * that axis (also called from M33, src/hud_b5840.c). */

void CameraHoldAnchor(void)
{
    s32 x, y, i;

    if (gActivePlayerCount)
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

void HubCameraFollowFocus(void)
{
    s32 x, y;

    if (gPlayerCount == 1)
    {
        struct CamPos *c;

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
        c = gPlayerCameraPos;
        c[gLocalPlayer].x = x;
        c[gLocalPlayer].y = y;
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x - 120 + 240;
        gViewRect[2] = y - 80;
        gViewRect[3] = y - 80 + 160;
    }
    else
    {
        UpdatePlayerGroupCenter();
        UpdatePlayerCameras();
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
            struct CamPos *c = gPlayerCameraPos;

            x = c[gLocalPlayer].x;
            y = c[gLocalPlayer].y;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        SetViewRectToPlayers();
    }
}

void HubCameraFollowFocusPlayer(void)
{
    s32 x, y;

    if (gPlayerCount == 1)
    {
        struct CamPos *c;

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
        c = gPlayerCameraPos;
        c[gLocalPlayer].x = x;
        c[gLocalPlayer].y = y;
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x - 120 + 240;
        gViewRect[2] = y - 80;
        gViewRect[3] = y - 80 + 160;
    }
    else
    {
        s32 i;

        x = gTasks[gCameraFocusPlayer].pixelX;
        y = gTasks[gCameraFocusPlayer].pixelY;
        if (x < gCameraBounds[0])
            x = gCameraBounds[0];
        if (x > gCameraBounds[1])
            x = gCameraBounds[1];
        if (y < gCameraBounds[2])
            y = gCameraBounds[2];
        if (y > gCameraBounds[3])
            y = gCameraBounds[3];
        /* indexed as a u16[][2] here: the struct spelling (.x/.y) gives the
           loop one address giv and reorders the hoisted rectangle sums */
        for (i = 0; i < gPlayerCount; i++)
        {
            ((u16 (*)[2])gPlayerCameraPos)[i][0] = x;
            ((u16 (*)[2])gPlayerCameraPos)[i][1] = y;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x + 120;
        gViewRect[2] = y - 80;
        gViewRect[3] = y + 80;
    }
}

void HubCameraGlideToPlayers(void)
{
    s32 x, y;

    if (gPlayerCount == 1)
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
        if (gPlayerCameraMode[gLocalPlayer] == 2)
        {
            if (x == gPlayerCameraPos[gLocalPlayer].x && y == gPlayerCameraPos[gLocalPlayer].y)
            {
                gPlayerCameraMode[gLocalPlayer] = 0;
            }
            else
            {
                if (x < gPlayerCameraPos[gLocalPlayer].x)
                {
                    gPlayerCameraPos[gLocalPlayer].x -= 3;
                    if (gPlayerCameraPos[gLocalPlayer].x < x)
                        gPlayerCameraPos[gLocalPlayer].x = x;
                }
                else
                {
                    gPlayerCameraPos[gLocalPlayer].x += 3;
                    if (x < gPlayerCameraPos[gLocalPlayer].x)
                        gPlayerCameraPos[gLocalPlayer].x = x;
                }
                if (y < gPlayerCameraPos[gLocalPlayer].y)
                {
                    gPlayerCameraPos[gLocalPlayer].y -= 3;
                    if (gPlayerCameraPos[gLocalPlayer].y < y)
                        gPlayerCameraPos[gLocalPlayer].y = y;
                }
                else
                {
                    gPlayerCameraPos[gLocalPlayer].y += 3;
                    if (y < gPlayerCameraPos[gLocalPlayer].y)
                        gPlayerCameraPos[gLocalPlayer].y = y;
                }
            }
            x = gPlayerCameraPos[gLocalPlayer].x;
            y = gPlayerCameraPos[gLocalPlayer].y;
        }
        else
        {
            gPlayerCameraPos[gLocalPlayer].x = x;
            gPlayerCameraPos[gLocalPlayer].y = y;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x - 120 + 240;
        gViewRect[2] = y - 80;
        gViewRect[3] = y - 80 + 160;
    }
    else
    {
        s32 i;

        SetPlayerGroupCenterFromTasks();
        for (i = 0; i < gPlayerCount; i++)
        {
            if ((gActivePlayerMask >> i) & 1)
            {
                x = gTasks[i].pixelX;
                y = gTasks[i].pixelY;
                if (x < gCameraBounds[0])
                    x = gCameraBounds[0];
                if (gCameraBounds[1] < x)
                    x = gCameraBounds[1];
                if (y < gCameraBounds[2])
                    y = gCameraBounds[2];
                if (gCameraBounds[3] < y)
                    y = gCameraBounds[3];
                if (gPlayerCameraMode[i] == 2)
                {
                    if (x == gPlayerCameraPos[i].x && y == gPlayerCameraPos[i].y)
                    {
                        gPlayerCameraMode[i] = 0;
                    }
                    else
                    {
                        if (x < gPlayerCameraPos[i].x)
                        {
                            gPlayerCameraPos[i].x -= 3;
                            if (gPlayerCameraPos[i].x < x)
                                gPlayerCameraPos[i].x = x;
                        }
                        else
                        {
                            gPlayerCameraPos[i].x += 3;
                            if (x < gPlayerCameraPos[i].x)
                                gPlayerCameraPos[i].x = x;
                        }
                        if (y < gPlayerCameraPos[i].y)
                        {
                            gPlayerCameraPos[i].y -= 3;
                            if (gPlayerCameraPos[i].y < y)
                                gPlayerCameraPos[i].y = y;
                        }
                        else
                        {
                            gPlayerCameraPos[i].y += 3;
                            if (y < gPlayerCameraPos[i].y)
                                gPlayerCameraPos[i].y = y;
                        }
                    }
                }
                else
                {
                    gPlayerCameraPos[i].x = x;
                    gPlayerCameraPos[i].y = y;
                }
            }
            else
            {
                x = gPlayerGroupCenter[0];
                if (x < gCameraBounds[0])
                    x = gCameraBounds[0];
                if (gCameraBounds[1] < x)
                    x = gCameraBounds[1];
                if (gPlayerCameraMode[i] == 2)
                {
                    if (x == gPlayerCameraPos[i].x)
                    {
                        gPlayerCameraMode[i] = 3;
                    }
                    else if (x < gPlayerCameraPos[i].x)
                    {
                        gPlayerCameraPos[i].x -= 3;
                        if (gPlayerCameraPos[i].x < x)
                            gPlayerCameraPos[i].x = x;
                    }
                    else
                    {
                        gPlayerCameraPos[i].x += 3;
                        if (x < gPlayerCameraPos[i].x)
                            gPlayerCameraPos[i].x = x;
                    }
                }
                else
                {
                    gPlayerCameraPos[i].x = x;
                }
            }
        }
        x = gPlayerCameraPos[gLocalPlayer].x;
        y = gPlayerCameraPos[gLocalPlayer].y;
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        SetViewRectToPlayers();
    }
}

void CameraSnapBoundsToAnchor(void)
{
    s32 x, y, i;

    if (gActivePlayerCount)
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
            struct CamRect *p = &gPlayerBounds[i];
            p->x0 = x - 117;
            p->x1 = x + 117;
            p->y0 = y - 76;
            p->y1 = y + 104;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        gViewRect[0] = x - 120;
        gViewRect[1] = x + 120;
        gViewRect[2] = y - 80;
        gViewRect[3] = y + 80;
    }
}

void CameraSnapPlayersToAnchor(void)
{
    s32 x, y, i;

    if (gActivePlayerCount)
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
        for (i = 0; i < gPlayerCount; i++)
        {
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

void CameraSnapToFocus(void)
{
    s32 x = gCameraFocusX;
    s32 y = gCameraFocusY;
    s32 t;
    struct CamPos *c;

    if (x < gCameraBounds[0])
        x = gCameraBounds[0];
    if (x > gCameraBounds[1])
        x = gCameraBounds[1];
    if (y < gCameraBounds[2])
        y = gCameraBounds[2];
    if (y > gCameraBounds[3])
        y = gCameraBounds[3];
    c = gPlayerCameraPos;
    c[gLocalPlayer].x = x;
    c[gLocalPlayer].y = y;
    gCameraCenterX = x << 16;
    gCameraCenterY = y << 16;
    gViewRect[0] = x - 120;
    gViewRect[1] = x - 120 + 240;
    gViewRect[2] = y - 80;
    gViewRect[3] = y - 80 + 160;
}

void StopScreenShake(void)
{
    gScreenShake.unk0 = 0;
    gScreenShake.unk2 = 0;
    gScreenShake.unk4 = 0;
    gScreenShake.unk6 = 0;
}

void UpdateScreenShake(void)
{
    u16 *p;

    if (gScreenShake.unk0 != 0)
    {
        p = gScreenShakePatterns[gScreenShake.unk0];
        switch (p[gScreenShake.unk6 * 2])
        {
        case 0x8000:
            gScreenShake.unk0 = 0;
            gScreenShake.unk2 = 0;
            gScreenShake.unk4 = 0;
            gScreenShake.unk6 = 0;
            return;
        case 0x9999:
            gScreenShake.unk6 = 0;
        }
        gScreenShake.unk2 = p[gScreenShake.unk6 * 2];
        gScreenShake.unk4 = p[gScreenShake.unk6 * 2 + 1];
        gScreenShake.unk6++;
    }
}

void StartScrollLock(s32 x0, s32 x1, s32 y0, s32 y1)
{
    u8 flags = 0;
    s32 mid;

    if (x0 != 0xFFFF && x1 != 0xFFFF)
    {
        gScrollLock.x0 = x0 + 120;
        gScrollLock.x1 = x1 - 120;
        if (gScrollLock.x0 < gRoomBounds[0])
            gScrollLock.x0 = gRoomBounds[0];
        if (gRoomBounds[1] < gScrollLock.x1)
            gScrollLock.x1 = gRoomBounds[1];
        flags = 1;
        mid = (gScrollLock.x0 + gScrollLock.x1) >> 1;
        if (gScrollLock.unkA <= mid)
        {
            gScrollLockSpeedX = 2;
            gCameraBounds[1] = gScrollLock.x1;
        }
        else
        {
            gScrollLockSpeedX = -2;
            gCameraBounds[0] = gScrollLock.x0;
        }
    }
    else
    {
        gScrollLock.x0 = 0xFFFF;
        gScrollLock.x1 = -1;
        gScrollLockSpeedX = 0;
    }
    if (y0 != 0xFFFF && y1 != 0xFFFF)
    {
        gScrollLock.y0 = y0 + 80;
        gScrollLock.y1 = y1 - 80;
        if (gScrollLock.y0 < gRoomBounds[2])
            gScrollLock.y0 = gRoomBounds[2];
        if (gScrollLock.y1 > gRoomBounds[3])
            gScrollLock.y1 = gRoomBounds[3];
        flags |= 2;
        mid = (gScrollLock.y0 + gScrollLock.y1) >> 1;
        if (gScrollLock.unkC <= mid)
        {
            gScrollLockSpeedY = 1;
            gCameraBounds[3] = gScrollLock.y1;
        }
        else
        {
            gScrollLockSpeedY = -1;
            gCameraBounds[2] = gScrollLock.y0;
        }
    }
    else
    {
        gScrollLock.y0 = 0xFFFF;
        gScrollLock.y1 = -1;
        gScrollLockSpeedY = 0;
    }
    if (flags != 0)
    {
        gCameraMode = 2;
        gScrollLock.lockedAxes = flags;
        gScrollLock.unk0 = 0;
        if (gPlayerCount == 1)
        {
            if (flags & 1)
            {
                struct CamRect *b;

                if (gScrollLockSpeedX > 0)
                {
                    gCameraBounds[0] = gPlayerCameraPos[gLocalPlayer].x;
                    gCameraBounds[1] = gScrollLock.x1;
                }
                else
                {
                    gCameraBounds[0] = gScrollLock.x0;
                    gCameraBounds[1] = gPlayerCameraPos[gLocalPlayer].x;
                }
                b = gPlayerBounds;
                b[gLocalPlayer].x0 = gCameraBounds[0] - 117;
                b[gLocalPlayer].x1 = gCameraBounds[1] + 117;
            }
            if (gScrollLock.lockedAxes & 2)
            {
                struct CamRect *b;

                if (gScrollLockSpeedY > 0)
                {
                    s16 *d = gCameraBounds;
                    struct CamPos *c = gPlayerCameraPos;

                    d[2] = c[gLocalPlayer].y;
                    gCameraBounds[3] = gScrollLock.y1;
                }
                else
                {
                    struct CamPos *c;

                    gCameraBounds[2] = gScrollLock.y0;
                    c = gPlayerCameraPos;
                    gCameraBounds[3] = c[gLocalPlayer].y;
                }
                b = gPlayerBounds;
                b[gLocalPlayer].y0 = gCameraBounds[2] - 76;
                b[gLocalPlayer].y1 = gCameraBounds[3] + 104;
            }
        }
        else
        {
            LockPlayersPastScrollLine();
            if (gScrollLock.lockedAxes & 1)
                gPlayerCameraPos[gPlayerCount].x = gScrollLock.unkA;
            if (gScrollLock.lockedAxes & 2)
            {
                struct CamPos *c = gPlayerCameraPos;

                c[gPlayerCount].y = gScrollLock.unkC;
            }
        }
    }
}
