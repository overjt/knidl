#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "camera.h"
#include "player.h"

/* level_23948.c (0x08023948-0x080242CF, issue #93).
 *
 * Room loaders, part 2: LoadHubRoom and LoadBigSwitchViewRoom (M02's screen
 * setups HubInit and BigSwitchViewInit) and LoadGoalGameRoom, plus the
 * room-task variants 1 and 2 of task type #3 (RoomTaskHubInit, RoomTaskBigSwitchViewInit)
 * and their per-frame bodies (RoomTaskHubUpdateCamera ... RoomTaskBigSwitchViewLateUpdate).
 * LoadHubRoom and LoadBigSwitchViewRoom build their map in the second buffer
 * gHubRoomMapBuffer through sub_08027a6c instead of gRoomMapBuffer, spawn the
 * door objects and set up the multi-player cameras; LoadGoalGameRoom loads
 * the fixed room gRoomTable[8][7][0] with the player at (136, 928) and
 * BGM 1.  Every loader ends with the per-player loop that refills health,
 * rebuilds the player mask gActivePlayerMask and restarts the player tasks
 * (CreatePlayer and InitPlayerState/sub_0803d1c4). */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void LoadHubRoom(void)
{
    s32 i;
    u32 a;

    ResetTasksAndOam();
    gInHub = 1;
    gCurLevel = gStageIndex;
    gUnk_03001F20 = 16;
    if (gHubUnlockFlags != 0 || gRoomEntryMode == 2)
        gCameraMode = 4;
    else
        gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(1);
    gCurRoomDef = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gRoomBg3FullShake = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gRoomWidth = gCurRoomDef->width;
    gRoomHeight = gCurRoomDef->height;
    gRoomMetatileCount = gRoomWidth * gRoomHeight;
    gRoomBorder[0] = gCurRoomDef->borderX;
    gRoomBorder[1] = gCurRoomDef->borderY;
    gBg3Border[0] = gCurRoomDef->bg3BorderX;
    gBg3Border[1] = gCurRoomDef->bg3BorderY;
    gRoomMap = gHubRoomMapBuffer;
    sub_08027a6c();
    gRoomBgLayout = 0;
    gCurTileDrifts = gTileDrifts;
    *gUnk_02005574 = 0;
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    InitDoors();
    SetRoomEntryPoint();
    SpawnDoorObjects();
    PlayHubRoomBgm();
    gActivePlayerMask = 0;
    gActivePlayerCount = 0;
    gLivingPlayerCount = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerLives[i] != 0 || gPlayerHealth[i] != 0)
        {
            if (gPlayerHealth[i] == 0)
            {
                gPlayerHealth[i] = gMaxHealth;
                gSavedPlayerAbilities[i] = 0;
                gSavedPlayerAbilityUses[i] = 0xFFFF;
                AddPlayerLives(-1, i);
            }
            if ((s16)gSavedPlayerAbilities[i] != 0)
            {
                gPlayerAbilities[i] = gSavedPlayerAbilities[i];
                gPlayerAbilityUses[i] = gSavedPlayerAbilityUses[i];
                gSavedPlayerAbilities[i] = 0;
                gSavedPlayerAbilityUses[i] = 0xFFFF;
            }
            gActivePlayerMask |= 1 << i;
            gActivePlayerCount++;
            gLivingPlayerCount++;
            gPlayerCameraMode[i] = 0;
        }
        else
        {
            gPlayerCameraMode[i] = 3;
        }
        CreatePlayer(i);
        sub_0803d1c4(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    CameraResetBounds();
    HudInit(gLocalPlayer);
    switch (gCameraMode)
    {
    default:
    case 0:
    case 3:
        HubCameraFollowFocus();
        break;
    case 2:
    case 4:
        CameraSnapPlayersToAnchor();
        break;
    case 1:
        HubCameraFollowFocusPlayer();
        break;
    }
    CameraInitPos();
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    if (gHubUnlockFlags != 0)
        DrawBg123View(gCameraPos[0], gCameraPos[1]);
    else
        DrawBg23View(gCameraPos[0], gCameraPos[1]);
}

void LoadBigSwitchViewRoom(void)
{
    u8 z;
    u32 w;
    u32 zero;

    gInHub = 1;
    gCurLevel = gStageIndex;
    gUnk_03001F20 = 16;
    gCameraMode = 2;
    LoadGfxSet(1);
    CreateRoomTask(2);
    gCurRoomDef = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gRoomBg3FullShake = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gRoomWidth = gCurRoomDef->width;
    gRoomHeight = gCurRoomDef->height;
    gRoomMetatileCount = gRoomWidth * gRoomHeight;
    gRoomBorder[0] = gCurRoomDef->borderX;
    gRoomBorder[1] = gCurRoomDef->borderY;
    gBg3Border[0] = gCurRoomDef->bg3BorderX;
    gBg3Border[1] = gCurRoomDef->bg3BorderY;
    gRoomMap = gHubRoomMapBuffer;
    z = 0;
    w = 0;
    sub_08027a6c();
    gRoomBgLayout = z;
    gCurTileDrifts = gTileDrifts;
    gUnk_02005574[0] = z;
    gUnk_02000020 = z;
    gUnk_0200B078 = z;
    gHBlankScrollStarted = z;
    gRoomUpdateFlags = 3;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    InitDoors();
    SpawnDoorObjects();
    sub_08029034();
    sub_08026b60();
    CreateBigSwitchUnlockPan();
    CameraResetBounds();
    CameraSnapBoundsToAnchor();
    CameraInitPos();
    CameraWriteScrollParallax();
    zero = w;
    CpuFastSet(&zero, (void *)(BG_VRAM + 0x2000), 0x01000400);
    DrawBg123View(gCameraPos[0], gCameraPos[1]);
}

void RoomTaskHubInit(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = (u32)RoomTaskDraw;
    t->updateCallback = (u32)RoomTaskHubUpdateCamera;
    if (gHubUnlockFlags != 0)
        t->lateUpdateCallback = (u32)RoomTaskHubLateUpdateBg123;
    else
        t->lateUpdateCallback = (u32)RoomTaskHubLateUpdateBg23;
    TaskSleepForever();
}

void RoomTaskBigSwitchViewInit(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)RoomTaskBigSwitchViewUpdateCamera;
    t->lateUpdateCallback = (u32)RoomTaskBigSwitchViewLateUpdate;
    TaskSleepForever();
}

void RoomTaskHubUpdateCamera(void)
{
    if (gRoomUpdateFlags & 1)
    {
        switch (gCameraMode)
        {
        default:
        case 0:
            HubCameraFollowFocus();
            break;
        case 2:
        case 4:
            CameraSnapPlayersToAnchor();
            break;
        case 3:
            HubCameraGlideToPlayers();
            break;
        case 1:
            HubCameraFollowFocusPlayer();
            break;
        }
    }
}

void RoomTaskBigSwitchViewUpdateCamera(void)
{
    if (gRoomUpdateFlags & 1)
        CameraSnapBoundsToAnchor();
}

void RoomTaskHubLateUpdateBg23(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        CameraUpdatePosNoParallax();
        StreamBg23Maps();
    }
    CameraWriteScrollBg23();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    HudUpdateAbilityPanel();
}

void RoomTaskHubLateUpdateBg123(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        CameraUpdatePosNoParallax();
        StreamBg123Maps();
    }
    CameraWriteScrollBg123();
    if (gRoomUpdateFlags & 16)
        UpdateDoors();
    HudUpdateAbilityPanel();
}

void RoomTaskBigSwitchViewLateUpdate(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        CameraUpdatePosNoParallax();
        StreamBg123Maps();
    }
    CameraWriteScrollBg123();
}

void LoadGoalGameRoom(void)
{
    s32 i;
    u32 a;

    gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(4);
    gCurRoomDef = gRoomTable[8][7][0];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gRoomBg3FullShake = gCurRoomDef->unk55;
    ClearBg2Bg3Maps();
    LoadBg2Gfx();
    LoadBg3Gfx();
    SelectBg3MapShape();
    gInHub = 0;
    gRoomWidth = gCurRoomDef->width;
    gRoomHeight = gCurRoomDef->height;
    gRoomMetatileCount = gRoomWidth * gRoomHeight;
    gRoomBorder[0] = gCurRoomDef->borderX;
    gRoomBorder[1] = gCurRoomDef->borderY;
    gBg3Border[0] = gCurRoomDef->bg3BorderX;
    gBg3Border[1] = gCurRoomDef->bg3BorderY;
    gRoomMap = gRoomMapBuffer;
    if (gCurRoomDef->mapsCompressed != 0)
        RequestCopy(8, (u32)gCurRoomDef->metatileMap, (u32)gRoomMapBuffer, 0);
    else
        CpuSet(gCurRoomDef->metatileMap, gRoomMapBuffer, (gRoomMetatileCount * 2) & 0x1FFFFF);
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    gRoomBgLayout = 0;
    gCurTileDrifts = gTileDrifts;
    *gUnk_02005574 = 0;
    gStageExitFlags = 0;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    LoadRoomBgAnims();
    gCameraFocusX = 136;
    gCameraFocusY = 928;
    PlayBgm(1);
    gRoomBgLayout = 0;
    gUnk_0200B078 = 0;
    gUnk_02000020 = 1;
    gActivePlayerMask = 0;
    gActivePlayerCount = 0;
    gLivingPlayerCount = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerLives[i] != 0 || gPlayerHealth[i] != 0)
        {
            if (gPlayerHealth[i] == 0)
            {
                gPlayerHealth[i] = gMaxHealth;
                gSavedPlayerAbilities[i] = 0;
                gSavedPlayerAbilityUses[i] = 0xFFFF;
                AddPlayerLives(-1, i);
            }
            if ((s16)gSavedPlayerAbilities[i] != 0)
            {
                gPlayerAbilities[i] = gSavedPlayerAbilities[i];
                gPlayerAbilityUses[i] = gSavedPlayerAbilityUses[i];
                gSavedPlayerAbilities[i] = 0;
                gSavedPlayerAbilityUses[i] = 0xFFFF;
            }
            gActivePlayerMask |= 1 << i;
            gActivePlayerCount++;
            gLivingPlayerCount++;
        }
        else
        {
            gPlayerCameraMode[i] = 0;
        }
        CreatePlayer(i);
        InitPlayerState(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    LatchPlayerKeys();
    CameraResetBounds();
    HudInit(gLocalPlayer);
    CameraSnapToFocus();
    CameraInitPos();
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    DrawBg23View(gCameraPos[0], gCameraPos[1]);
}
