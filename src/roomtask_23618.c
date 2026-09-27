#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "room.h"
#include "camera.h"

/* roomtask_23618.c (0x08023618-0x08023947, issue #93).
 *
 * Task type #3 (class 4), the room's per-frame driver.  Task_Room, the
 * type's body, dispatches Task.state through the anchor table
 * gRoomTaskVariants: sub_08023634 (index 0), sub_08023e34 (1), sub_08023e78
 * (2), sub_08024540 (3), sub_080242d0 (4), sub_08024904 (5) and
 * sub_08024da4 (6); every room loader spawns the task with its own index
 * through CreateRoomTask.  Each variant installs the task callbacks: unk0C
 * = RoomTaskDraw (door objects and HUD), unk04 = the camera update of the
 * camera mode gCameraMode (RoomTaskUpdateCamera here, M08's five camera modes)
 * and unk08 = one per-frame body chosen by the BG layout gRoomBgLayout
 * (sub_08023748 ... sub_080238ec here).  The bodies are gated by the flag
 * cell gRoomUpdateFlags: 1 = camera, BG animation and BG streaming, 2 =
 * screen shake, 8 = HUD, 16 = door objects (UpdateDoors). */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));

void Task_Room(void)
{
    CallTableEntry(gCurTask->state, 7, gRoomTaskVariants);
}

void sub_08023634(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = (u32)RoomTaskDraw;
    t->updateCallback = (u32)RoomTaskUpdateCamera;
    switch (gRoomBgLayout)
    {
    default:
    case 0:
        gCurTask->lateUpdateCallback = (u32)sub_08023748;
        break;
    case 1:
        gCurTask->lateUpdateCallback = (u32)sub_080237fc;
        break;
    case 2:
        gCurTask->lateUpdateCallback = (u32)sub_080238ec;
        break;
    case 3:
        gCurTask->lateUpdateCallback = (u32)sub_080237a4;
        break;
    case 4:
        gCurTask->lateUpdateCallback = (u32)sub_0802385c;
        break;
    case 5:
        gCurTask->lateUpdateCallback = (u32)sub_080238a4;
        break;
    }
    TaskSleepForever();
}

void RoomTaskDraw(void)
{
    DrawDoors();
    sub_0800a6a4();
}

void RoomTaskUpdateCamera(void)
{
    if (gRoomUpdateFlags & 1)
    {
        switch (gCameraMode)
        {
        default:
        case 0:
        case 1:
            CameraFollowFocus();
            break;
        case 2:
            CameraSlideToScrollLock();
            break;
        case 3:
            CameraFollowScrollLocked();
            break;
        case 4:
            CameraSlideFromScrollLock();
            break;
        case 5:
            CameraHoldAnchor();
            break;
        }
    }
}

void sub_08023748(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2Map();
        SpawnRoomObjectsScrolledIn();
    }
    CameraWriteScrollParallax();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}

void sub_080237a4(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        sub_080296a0();
    }
    CameraWriteScrollParallax();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}

void sub_080237fc(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2Map();
        StreamBg3Map();
        SpawnRoomObjectsScrolledIn();
    }
    CameraWriteScrollParallax();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}

void sub_0802385c(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePosNoParallax();
        StreamBg23Maps();
    }
    CameraWriteScrollHBlank();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}

void sub_080238a4(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePosNoParallax();
        StreamBg23Rows();
    }
    CameraWriteScrollHBlank();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}

void sub_080238ec(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePosBg3AutoScrollX();
        StreamBg2Map();
        SpawnRoomObjectsScrolledIn();
    }
    CameraWriteScrollParallax();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}
