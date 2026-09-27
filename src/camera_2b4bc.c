#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "room.h"
#include "camera.h"

/* camera_2b4bc.c (0x0802B4BC-0x0802C42B, issue #86).
 *
 * The per-frame camera updates of the room modes (called from M07).  With
 * one player, CameraFollowFocus, CameraSlideToScrollLock, CameraFollowScrollLocked and CameraSlideFromScrollLock
 * clamp the player position gCameraFocusX/gCameraFocusY to the camera
 * bounds gCameraBounds into that player's camera gPlayerCameraPos[i]; with
 * several they run the multiplayer helpers of camera_29c74.c.  While the
 * scroll lock of gScrollLock is armed (StartScrollLock), CameraSlideToScrollLock and
 * CameraSlideFromScrollLock slide the camera and its bounds towards the lock line by
 * gScrollLockSpeedX (x) / gScrollLockSpeedY (y) pixels a frame and stop on
 * arrival.  All of them end by writing the 16.16 target
 * gCameraCenterX/gCameraCenterY and the visible rectangle gViewRect. */

void UpdatePlayerGroupCenter(void);
void SetCameraBoundsToGroup(void);
void sub_08029ef4(void);
void UpdatePlayerCameras(void);
void sub_0802a260(void);
void SetPlayerBoundsFromCamera(void);
void sub_0802a484(void);
void SetViewRectToPlayers(void);
void sub_0802a568(void);
void sub_0802a63c(void);

void CameraFollowFocus(void)
{
    s32 x, y;
    struct CamPos *c;

    if (gActivePlayerCount)
    {
        if (gPlayerCount == 1)
        {
            x = gCameraFocusX;
            y = gCameraFocusY;
            if (gUnk_0200B078 == 2 && y < gCameraBounds[3])
            {
                struct CamRect *p;

                gCameraBounds[3] = y;
                p = gPlayerBounds;
                p[gLocalPlayer].y1 = y + 104;
            }
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
                c = gPlayerCameraPos;
                x = c[gLocalPlayer].x;
                y = c[gLocalPlayer].y;
            }
            gCameraCenterX = x << 16;
            gCameraCenterY = y << 16;
            SetViewRectToPlayers();
        }
    }
}

void CameraSlideToScrollLock(void)
{
    s32 x, y, i;
    u32 done;

    if (gActivePlayerCount == 0)
        return;
    if (gPlayerCount == 1)
    {
        done = 0;
        if (gScrollLock.lockedAxes & 1)
        {
            s32 d = gScrollLockSpeedX;

            if (d == 0)
            {
                done = 1;
            }
            else
            {
                gPlayerCameraPos[gLocalPlayer].x += d;
                if (d > 0)
                {
                    if (gPlayerCameraPos[gLocalPlayer].x >= gScrollLock.x0)
                    {
                        gPlayerCameraPos[gLocalPlayer].x = gScrollLock.x0;
                        gScrollLockSpeedX = 0;
                        done = 1;
                    }
                    gCameraBounds[0] = gPlayerCameraPos[gLocalPlayer].x;
                    gPlayerBounds[gLocalPlayer].x0 = gPlayerCameraPos[gLocalPlayer].x + -117;
                }
                else
                {
                    if (gScrollLock.x1 >= gPlayerCameraPos[gLocalPlayer].x)
                    {
                        gPlayerCameraPos[gLocalPlayer].x = gScrollLock.x1;
                        gScrollLockSpeedX = 0;
                        done = 1;
                    }
                    gCameraBounds[1] = gPlayerCameraPos[gLocalPlayer].x;
                    gPlayerBounds[gLocalPlayer].x1 = gPlayerCameraPos[gLocalPlayer].x + 117;
                }
            }
            x = gPlayerCameraPos[gLocalPlayer].x;
        }
        else
        {
            x = gCameraFocusX;
            if (x < gCameraBounds[0])
                x = gCameraBounds[0];
            if (x > gCameraBounds[1])
                x = gCameraBounds[1];
            gPlayerCameraPos[gLocalPlayer].x = x;
        }
        if (gScrollLock.lockedAxes & 2)
        {
            s32 d = gScrollLockSpeedY;

            if (d == 0)
            {
                done |= 2;
            }
            else
            {
                gPlayerCameraPos[gLocalPlayer].y += d;
                if (d > 0)
                {
                    if (gPlayerCameraPos[gLocalPlayer].y >= gScrollLock.y0)
                    {
                        gPlayerCameraPos[gLocalPlayer].y = gScrollLock.y0;
                        gScrollLockSpeedY = 0;
                        done |= 2;
                    }
                    gCameraBounds[2] = gPlayerCameraPos[gLocalPlayer].y;
                    gPlayerBounds[gLocalPlayer].y0 = gPlayerCameraPos[gLocalPlayer].y + -76;
                }
                else
                {
                    if (gScrollLock.y1 >= gPlayerCameraPos[gLocalPlayer].y)
                    {
                        gPlayerCameraPos[gLocalPlayer].y = gScrollLock.y1;
                        gScrollLockSpeedY = 0;
                        done |= 2;
                    }
                    gCameraBounds[3] = gPlayerCameraPos[gLocalPlayer].y;
                    gPlayerBounds[gLocalPlayer].y1 = gPlayerCameraPos[gLocalPlayer].y + 104;
                }
            }
            y = gPlayerCameraPos[gLocalPlayer].y;
        }
        else
        {
            y = gCameraFocusY;
            if (y < gCameraBounds[2])
                y = gCameraBounds[2];
            if (y > gCameraBounds[3])
                y = gCameraBounds[3];
            gPlayerCameraPos[gLocalPlayer].y = y;
        }
        if (done == gScrollLock.lockedAxes)
            gCameraMode = 3;
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
        sub_08029ef4();
        done = 0;
        if (gScrollLock.lockedAxes & 1)
        {
            if (gScrollLockSpeedX == 0)
            {
                done = 1;
                gCameraBounds[0] = gScrollLock.x0;
                gCameraBounds[1] = gScrollLock.x1;
            }
            else if (gScrollLockSpeedX > 0)
            {
                for (i = 0; i < gPlayerCount; i++)
                {
                    if (((gScrollLock.unk0 >> i) & 1) && ((gActivePlayerMask >> i) & 1))
                    {
                        gPlayerCameraPos[i].x += gScrollLockSpeedX;
                        if (gScrollLock.x0 <= gPlayerCameraPos[i].x)
                            gPlayerCameraPos[i].x = gScrollLock.x0;
                        gPlayerBounds[i].x0 = gPlayerCameraPos[i].x + -117;
                        gPlayerBounds[i].x1 = gScrollLock.x1 + 117;
                    }
                }
                if (gPlayerCameraPos[gPlayerCount].x < gScrollLock.x1)
                {
                    gPlayerCameraPos[gPlayerCount].x += gScrollLockSpeedX;
                    if (gScrollLock.x1 < gPlayerCameraPos[gPlayerCount].x)
                        gPlayerCameraPos[gPlayerCount].x = gScrollLock.x1;
                }
                if ((gScrollLock.unk0 >> gPlayerCount) & 1)
                {
                    gCameraBounds[0] += gScrollLockSpeedX;
                    if (gScrollLock.x0 <= gCameraBounds[0])
                    {
                        done |= 1;
                        gScrollLockSpeedX = 0;
                        gCameraBounds[0] = gScrollLock.x0;
                    }
                }
                gCameraBounds[1] = gScrollLock.x1;
            }
            else
            {
                for (i = 0; i < gPlayerCount; i++)
                {
                    if (((gScrollLock.unk0 >> i) & 1) && ((gActivePlayerMask >> i) & 1))
                    {
                        gPlayerCameraPos[i].x += gScrollLockSpeedX;
                        if (gPlayerCameraPos[i].x <= gScrollLock.x1)
                            gPlayerCameraPos[i].x = gScrollLock.x1;
                        gPlayerBounds[i].x0 = gScrollLock.x0 + -117;
                        gPlayerBounds[i].x1 = gPlayerCameraPos[i].x + 117;
                    }
                }
                if (gScrollLock.x0 < gPlayerCameraPos[gPlayerCount].x)
                {
                    gPlayerCameraPos[gPlayerCount].x += gScrollLockSpeedX;
                    if (gPlayerCameraPos[gPlayerCount].x < gScrollLock.x0)
                        gPlayerCameraPos[gPlayerCount].x = gScrollLock.x0;
                }
                if ((gScrollLock.unk0 >> gPlayerCount) & 1)
                {
                    gCameraBounds[1] += gScrollLockSpeedX;
                    if (gCameraBounds[1] <= gScrollLock.x1)
                    {
                        done |= 1;
                        gScrollLockSpeedX = 0;
                        gCameraBounds[1] = gScrollLock.x1;
                    }
                }
                gCameraBounds[0] = gScrollLock.x0;
            }
        }
        if (gScrollLock.lockedAxes & 2)
        {
            if (gScrollLockSpeedY == 0)
            {
                done |= 2;
                gCameraBounds[2] = gScrollLock.y0;
                gCameraBounds[3] = gScrollLock.y1;
            }
            else if (gScrollLockSpeedY > 0)
            {
                for (i = 0; i < gPlayerCount; i++)
                {
                    if (((gScrollLock.unk0 >> i) & 1) && ((gActivePlayerMask >> i) & 1))
                    {
                        gPlayerCameraPos[i].y += gScrollLockSpeedY;
                        if (gScrollLock.y0 <= gPlayerCameraPos[i].y)
                            gPlayerCameraPos[i].y = gScrollLock.y0;
                        gPlayerBounds[i].y0 = gPlayerCameraPos[i].y + -76;
                        gPlayerBounds[i].y1 = gScrollLock.y1 + 104;
                    }
                }
                if (gPlayerCameraPos[gPlayerCount].y < gScrollLock.y1)
                {
                    gPlayerCameraPos[gPlayerCount].y += gScrollLockSpeedY;
                    if (gScrollLock.y1 < gPlayerCameraPos[gPlayerCount].y)
                        gPlayerCameraPos[gPlayerCount].y = gScrollLock.y1;
                }
                if ((gScrollLock.unk0 >> gPlayerCount) & 1)
                {
                    gCameraBounds[2] += gScrollLockSpeedY;
                    if (gScrollLock.y0 <= gCameraBounds[2])
                    {
                        done |= 2;
                        gScrollLockSpeedY = 0;
                        gCameraBounds[2] = gScrollLock.y0;
                    }
                }
                gCameraBounds[3] = gScrollLock.y1;
            }
            else
            {
                for (i = 0; i < gPlayerCount; i++)
                {
                    if (((gScrollLock.unk0 >> i) & 1) && ((gActivePlayerMask >> i) & 1))
                    {
                        gPlayerCameraPos[i].y += gScrollLockSpeedY;
                        if (gPlayerCameraPos[i].y <= gScrollLock.y1)
                            gPlayerCameraPos[i].y = gScrollLock.y1;
                        gPlayerBounds[i].y1 = gPlayerCameraPos[i].y + 104;
                        gPlayerBounds[i].y0 = gScrollLock.y0 + -76;
                    }
                }
                if (gScrollLock.y0 < gPlayerCameraPos[gPlayerCount].y)
                {
                    gPlayerCameraPos[gPlayerCount].y += gScrollLockSpeedY;
                    if (gPlayerCameraPos[gPlayerCount].y < gScrollLock.y0)
                        gPlayerCameraPos[gPlayerCount].y = gScrollLock.y0;
                }
                if ((gScrollLock.unk0 >> gPlayerCount) & 1)
                {
                    gCameraBounds[3] += gScrollLockSpeedY;
                    if (gCameraBounds[3] <= gScrollLock.y1)
                    {
                        done |= 2;
                        gScrollLockSpeedY = 0;
                        gCameraBounds[3] = gScrollLock.y1;
                    }
                }
                gCameraBounds[2] = gScrollLock.y0;
            }
        }
        sub_0802a260();
        sub_0802a484();
        if ((gActivePlayerMask >> gLocalPlayer) & 1)
        {
            if ((gScrollLock.unk0 >> gLocalPlayer) & 1)
            {
                if (gScrollLock.lockedAxes & 1)
                {
                    x = gPlayerCameraPos[gLocalPlayer].x;
                }
                else
                {
                    x = gCameraFocusX;
                    if (x < gCameraBounds[0])
                        x = gCameraBounds[0];
                    if (x > gCameraBounds[1])
                        x = gCameraBounds[1];
                }
                if (gScrollLock.lockedAxes & 2)
                {
                    y = gPlayerCameraPos[gLocalPlayer].y;
                }
                else
                {
                    y = gCameraFocusY;
                    if (y < gCameraBounds[2])
                        y = gCameraBounds[2];
                    if (y > gCameraBounds[3])
                        y = gCameraBounds[3];
                }
            }
            else
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
        }
        else
        {
            x = gPlayerCameraPos[gLocalPlayer].x;
            y = gPlayerCameraPos[gLocalPlayer].y;
        }
        gCameraCenterX = x << 16;
        gCameraCenterY = y << 16;
        sub_0802a568();
        if (done == gScrollLock.lockedAxes)
            gCameraMode = 3;
        else
            sub_0802a63c();
    }
}

void CameraFollowScrollLocked(void)
{
    s32 x, y;

    if (gActivePlayerCount)
    {
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
            SetCameraBoundsToGroup();
            if (gScrollLock.lockedAxes & 1)
            {
                gCameraBounds[0] = gScrollLock.x0;
                gCameraBounds[1] = gScrollLock.x1;
            }
            if (gScrollLock.lockedAxes & 2)
            {
                gCameraBounds[2] = gScrollLock.y0;
                gCameraBounds[3] = gScrollLock.y1;
            }
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
                struct CamPos *c = gPlayerCameraPos;

                x = c[gLocalPlayer].x;
                y = c[gLocalPlayer].y;
            }
            gCameraCenterX = x << 16;
            gCameraCenterY = y << 16;
            SetViewRectToPlayers();
        }
    }
}

/* Single-player path: animate the camera bounds toward the room bounds by
   gScrollLockSpeedX / gScrollLockSpeedY per frame and stop once the clamped player
   position no longer differs.  u/v must be their own locals (reusing x/y or
   one shared temp permutes the whole allocation). */
void CameraSlideFromScrollLock(void)
{
    s32 x, y;
    s32 u, v;
    struct CamRect *p;

    if (gActivePlayerCount)
    {
        if (gPlayerCount == 1)
        {
            if (gScrollLock.lockedAxes & 1)
            {
                gCameraBounds[0] -= gScrollLockSpeedX;
                gCameraBounds[1] += gScrollLockSpeedX;
                if (gCameraBounds[0] < gRoomBounds[0])
                    gCameraBounds[0] = gRoomBounds[0];
                if (gRoomBounds[1] < gCameraBounds[1])
                    gCameraBounds[1] = gRoomBounds[1];
                u = gCameraFocusX;
                v = u;
                if (u < gRoomBounds[0])
                    v = gRoomBounds[0];
                if (gRoomBounds[1] < v)
                    v = gRoomBounds[1];
                if (u < gCameraBounds[0])
                    u = gCameraBounds[0];
                if (gCameraBounds[1] < u)
                    u = gCameraBounds[1];
                if (v == u)
                {
                    gScrollLockSpeedX = 0;
                    gScrollLock.lockedAxes &= ~1;
                    gCameraBounds[0] = gRoomBounds[0];
                    gCameraBounds[1] = gRoomBounds[1];
                }
                p = gPlayerBounds;
                p[gLocalPlayer].x0 = gCameraBounds[0] - 117;
                p[gLocalPlayer].x1 = gCameraBounds[1] + 117;
                /* bit 0 again, not bit 1: the ROM tests the x flag twice, so
                   the y bounds only animate while the x animation runs */
                if (gScrollLock.lockedAxes & 1)
                {
                    gCameraBounds[2] -= gScrollLockSpeedY;
                    gCameraBounds[3] += gScrollLockSpeedY;
                    if (gCameraBounds[2] < gRoomBounds[2])
                        gCameraBounds[2] = gRoomBounds[2];
                    if (gRoomBounds[3] < gCameraBounds[3])
                        gCameraBounds[3] = gRoomBounds[3];
                    u = gCameraFocusY;
                    v = u;
                    if (u < gRoomBounds[2])
                        v = gRoomBounds[2];
                    if (gRoomBounds[3] < v)
                        v = gRoomBounds[3];
                    if (u < gCameraBounds[2])
                        u = gCameraBounds[2];
                    if (gCameraBounds[3] < u)
                        u = gCameraBounds[3];
                    if (v == u)
                    {
                        gScrollLockSpeedY = 0;
                        gScrollLock.lockedAxes &= ~2;
                        gCameraBounds[2] = gRoomBounds[2];
                        gCameraBounds[3] = gRoomBounds[3];
                    }
                    p[gLocalPlayer].y0 = gCameraBounds[2] - 76;
                    p[gLocalPlayer].y1 = gCameraBounds[3] + 104;
                }
            }
            if (gScrollLock.lockedAxes == 0)
                gCameraMode = 0;
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
        }
        else
        {
            UpdatePlayerGroupCenter();
            SetCameraBoundsToGroup();
            if (gScrollLock.lockedAxes & 1)
            {
                gScrollLock.x0 -= gScrollLockSpeedX;
                gScrollLock.x1 += gScrollLockSpeedX;
                if (gScrollLock.x0 < gRoomBounds[0])
                    gScrollLock.x0 = gRoomBounds[0];
                if (gRoomBounds[1] < gScrollLock.x1)
                    gScrollLock.x1 = gRoomBounds[1];
                if (gScrollLock.x0 <= gCameraBounds[0] && gCameraBounds[1] <= gScrollLock.x1)
                {
                    gScrollLockSpeedX = 0;
                    gScrollLock.lockedAxes &= ~1;
                }
                else
                {
                    if (gCameraBounds[0] < gScrollLock.x0)
                        gCameraBounds[0] = gScrollLock.x0;
                    if (gScrollLock.x1 < gCameraBounds[1])
                        gCameraBounds[1] = gScrollLock.x1;
                }
            }
            if (gScrollLock.lockedAxes & 2)
            {
                gScrollLock.y0 -= gScrollLockSpeedY;
                gScrollLock.y1 += gScrollLockSpeedY;
                if (gScrollLock.y0 < gRoomBounds[2])
                    gScrollLock.y0 = gRoomBounds[2];
                if (gRoomBounds[3] < gScrollLock.y1)
                    gScrollLock.y1 = gRoomBounds[3];
                if (gScrollLock.y0 <= gCameraBounds[2] && gCameraBounds[3] <= gScrollLock.y1)
                {
                    gScrollLockSpeedY = 0;
                    gScrollLock.lockedAxes &= ~2;
                }
                else
                {
                    if (gCameraBounds[2] < gScrollLock.y0)
                        gCameraBounds[2] = gScrollLock.y0;
                    if (gScrollLock.y1 < gCameraBounds[3])
                        gCameraBounds[3] = gScrollLock.y1;
                }
            }
            if (gScrollLock.lockedAxes == 0)
                gCameraMode = 0;
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
                struct CamPos *c = gPlayerCameraPos;

                x = c[gLocalPlayer].x;
                y = c[gLocalPlayer].y;
            }
            gCameraCenterX = x << 16;
            gCameraCenterY = y << 16;
            SetViewRectToPlayers();
        }
    }
}
