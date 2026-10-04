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
 * gRoomTaskVariants: RoomTaskStageInit (index 0), RoomTaskHubInit (1), RoomTaskBigSwitchViewInit
 * (2), RoomTaskCutsceneInit (3), RoomTaskGoalGameInit (4), sub_08024904 (5) and
 * RoomTaskCreditsInit (6); every room loader spawns the task with its own index
 * through CreateRoomTask.  Each variant installs the task callbacks: unk0C
 * = RoomTaskDraw (door objects and HUD), unk04 = the camera update of the
 * camera mode gCameraMode (RoomTaskUpdateCamera here, M08's five camera modes)
 * and unk08 = one per-frame body chosen by the BG layout gRoomBgLayout
 * (RoomTaskLateUpdateBg2 ... RoomTaskLateUpdateBg3AutoScroll here).  The bodies are gated by the flag
 * cell gRoomUpdateFlags: 1 = camera, BG animation and BG streaming, 2 =
 * screen shake, 8 = HUD, 16 = door objects (UpdateDoors). */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));

void Task_Room(void)
{
    CallTableEntry(gCurTask->state, 7, gRoomTaskVariants);
}

void RoomTaskStageInit(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = (u32)RoomTaskDraw;
    t->updateCallback = (u32)RoomTaskUpdateCamera;
    switch (gRoomBgLayout)
    {
    default:
    case 0:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2;
        break;
    case 1:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2Bg3;
        break;
    case 2:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg3AutoScroll;
        break;
    case 3:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateLooping;
        break;
    case 4:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg23HBlank;
        break;
    case 5:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg23RowsHBlank;
        break;
    }
    TaskSleepForever();
}

void RoomTaskDraw(void)
{
    DrawDoors();
    HudUpdate();
}

void RoomTaskUpdateCamera(void)
{
    if (gRoomUpdateFlags & 1)
    {
        switch (gCameraMode)
        {
        default:
        case CAMERA_MODE_FOLLOW_FOCUS:
        case CAMERA_MODE_FOLLOW_PLAYER:
            CameraFollowFocus();
            break;
        case CAMERA_MODE_SLIDE_TO_LOCK:
            CameraSlideToScrollLock();
            break;
        case CAMERA_MODE_SCROLL_LOCKED:
            CameraFollowScrollLocked();
            break;
        case CAMERA_MODE_SLIDE_FROM_LOCK:
            CameraSlideFromScrollLock();
            break;
        case CAMERA_MODE_HOLD_ANCHOR:
            CameraHoldAnchor();
            break;
        }
    }
}

void RoomTaskLateUpdateBg2(void)
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

void RoomTaskLateUpdateLooping(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2MapLooping();
    }
    CameraWriteScrollParallax();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    if (gRoomUpdateFlags & 8)
        HudUpdateHpBars();
    HudUpdateAbilityPanel();
}

void RoomTaskLateUpdateBg2Bg3(void)
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

void RoomTaskLateUpdateBg23HBlank(void)
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

void RoomTaskLateUpdateBg23RowsHBlank(void)
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

void RoomTaskLateUpdateBg3AutoScroll(void)
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
