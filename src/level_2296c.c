#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "hud.h"
#include "cutscene.h"
#include "room.h"
#include "camera.h"
#include "player.h"

/* level_2296c.c (0x0802296C-0x08023617, issue #93).
 *
 * Level resets and room loaders, part 1.  sub_0802296c (M02's
 * ResetScoresAndMaxHealth) and its twin sub_08022c3c clear the level state, rebuild
 * the per-stage door masks gUsedSubGameDoors[] and the cleared-stage mask
 * gWarpStarStationLevels from the save flags (gHubDoorUnlocks[level][6] names each
 * stage's flag) and place the player at the matching door of the hub
 * room gRoomTable[8][stage][0]; sub_08022f50 (AgbMain) resets level,
 * stage and room; sub_08022f98/sub_08022f9c are M02's screen-setup hooks;
 * LoadRoom and sub_080233e0 are the loaders of M02's first screen
 * setup StageInit (see level_242d0.c); CreateRoomTask spawns task type
 * #3 with its variant index. */

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void sub_0802296c(void)
{
    s32 i;
    s32 j;
    struct RoomDef *room;
    struct Door *d;

    gUnk_02004C98 = 0;
    gHubUnlockFlags = 0;
    gHubUnlockSource = 0;
    gBigSwitchPressActive = 0;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
    gUnk_02007D60 = 0;
    gUnk_0200AF0C = -1;
    gWarpStarStationLevels = 0;
    for (i = 0; i <= 7; i++)
        for (j = 7; j >= 0; j--)
            gUsedRoomObjects[i][j] = 0;
    for (i = 0; i <= 6; i++)
    {
        if (gMetaKnightmareMode)
            gUsedSubGameDoors[i] = 15;
        else
            gUsedSubGameDoors[i] = 0;
        if (gHubDoorUnlocks[i][6] & 0x100)
        {
            if (gBigSwitchFlags[0] & (1 << (gHubDoorUnlocks[i][6] & 0xFF)))
                gWarpStarStationLevels |= 1 << i;
        }
        else if (gStageClearStatus[i][gHubDoorUnlocks[i][6]] != 0)
        {
            gWarpStarStationLevels |= 1 << i;
        }
    }
    gLevelIndex = 8;
    gStageIndex = gCurLevel;
    gRoomIndex = 0;
    gSkipNextHubBgm = 0;
    gContinueLevel = gStageIndex;
    gUnk_02007FF8 = gUnk_03001F20;
    room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    d = room->doors;
    if (gFurthestLevel == 0 && gFurthestStage == 0)
    {
        gRoomEntrySet = 0;
        gRoomEntryMode = 2;
    }
    else
    {
        if ((s8)gUnk_03001F20 == 16)
        {
            for (i = 0; i < room->doorCount; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 1)
                    break;
            }
            gRoomEntryX = d->unk2 * 16 + 22;
            gRoomEntryY = d->unk4 * 16 + 5;
            gRoomEntryMode = 1;
            gEntryDoorEvent = 0;
        }
        else if ((s8)gUnk_03001F20 == 32)
        {
            for (i = 0; i < room->doorCount; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 2)
                    break;
            }
            gRoomEntryX = d->unk2 * 16 + 16;
            gRoomEntryY = d->unk4 * 16 + 53;
            gRoomEntryMode = 0;
        }
        else
        {
            for (i = 0; i < room->doorCount; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
                    break;
            }
            gRoomEntryX = d->unk2 * 16 + 22;
            gRoomEntryY = d->unk4 * 16 + 5;
            gRoomEntryMode = 1;
            gEntryDoorEvent = 0;
        }
        gRoomEntrySet = 1;
    }
    gCutscenePending = 1;
}

void sub_08022c3c(void)
{
    s32 i;
    s32 j;
    struct RoomDef *room;
    struct Door *d;

    gUnk_02004C98 = 0;
    gHubUnlockFlags = 0;
    gHubUnlockSource = 0;
    gBigSwitchPressActive = 0;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
    gUnk_02007D60 = 0;
    gUnk_0200AF0C = -1;
    gWarpStarStationLevels = 0;
    for (i = 0; i <= 7; i++)
        for (j = 7; j >= 0; j--)
            gUsedRoomObjects[i][j] = 0;
    for (i = 0; i <= 6; i++)
    {
        if (gMetaKnightmareMode)
            gUsedSubGameDoors[i] = 15;
        else
            gUsedSubGameDoors[i] = 0;
        if (gHubDoorUnlocks[i][6] & 0x100)
        {
            if (gBigSwitchFlags[0] & (1 << (gHubDoorUnlocks[i][6] & 0xFF)))
                gWarpStarStationLevels |= 1 << i;
        }
        else if (gStageClearStatus[i][gHubDoorUnlocks[i][6]] != 0)
        {
            gWarpStarStationLevels |= 1 << i;
        }
    }
    if (gCurLevel == 7)
    {
        gLevelIndex = 7;
        gStageIndex = 0;
        gRoomIndex = 0;
        gRoomEntrySet = 0;
        gStageRequest = STAGE_REQUEST_STAGE_START;
        gEntryDoorEvent = 0;
        gRoomEntryMode = 0;
    }
    else
    {
        gLevelIndex = 8;
        gStageIndex = gContinueLevel;
        gRoomIndex = 0;
        gSkipNextHubBgm = 0;
        if (gFurthestLevel == 0 && gFurthestStage == 0)
        {
            gRoomEntryX = 70;
            gRoomEntryY = 0x105;
            gRoomEntryMode = 0;
            gEntryDoorEvent = 0;
        }
        else
        {
            room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
            d = room->doors;
            if (gUnk_02007FF8 == 16)
            {
                for (i = 0; i < room->doorCount; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 1)
                        break;
                }
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntryMode = 1;
                gEntryDoorEvent = 0;
            }
            else if (gUnk_02007FF8 == 32)
            {
                for (i = 0; i < room->doorCount; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 2)
                        break;
                }
                gRoomEntryX = d->unk2 * 16 + 16;
                gRoomEntryY = d->unk4 * 16 + 53;
                gRoomEntryMode = 0;
                gEntryDoorEvent = 0;
            }
            else
            {
                for (i = 0; i < room->doorCount; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == gUnk_02007FF8)
                        break;
                }
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntryMode = 1;
                gEntryDoorEvent = 0;
            }
        }
        gStageRequest = STAGE_REQUEST_HUB;
        gRoomEntrySet = 1;
    }
    gUnk_0200B038 = 0;
    gCutscenePending = 1;
}

void sub_08022f50(void)
{
    gLevelIndex = 0;
    gStageIndex = gUnk_08334EB4[gLevelIndex] - 1;
    gRoomIndex = 0;
    gRoomEntryMode = 0;
    gRoomEntrySet = 0;
    gCutscenePending = 0;
    HudShowClock();
}

void sub_08022f98(void)
{
}

void sub_08022f9c(void)
{
    gRoomBgmStarted = 0;
}

void LoadRoom(void)
{
    s32 i;
    u32 a;

    ResetTasksAndOam();
    gInHub = 0;
    if (gLevelIndex == 8)
    {
        gCurLevel = gStageIndex;
        *(s8 *)&gUnk_03001F20 = -1;
    }
    else
    {
        gCurLevel = gLevelIndex;
        gUnk_03001F20 = gStageIndex;
    }
    if (gRoomEntryMode == 2)
        gCameraMode = CAMERA_MODE_HOLD_ANCHOR;
    else
        gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
    LoadGfxSet(1);
    CreateRoomTask(0);
    gCurRoomDef = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gRoomBg3FullShake = gCurRoomDef->bg3FullShake;
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
    gRoomObjectList.count = gCurRoomDef->objectCount;
    gRoomObjectList.sortedByY = gCurRoomDef->objectsSortedByY;
    gRoomObjectList.entries = gCurRoomDef->objects;
    gRoomMap = gRoomMapBuffer;
    if (gCurRoomDef->mapsCompressed != 0)
    {
        RequestCopy(8, (u32)gCurRoomDef->metatileMap, (u32)gRoomMapBuffer, 0);
        RequestCopy(8, (u32)gCurRoomDef->blockLayer, (u32)gBlockLayer, 0);
    }
    else
    {
        CpuSet(gCurRoomDef->metatileMap, gRoomMapBuffer, (gRoomMetatileCount * 2) & 0x1FFFFF);
        CpuSet(gCurRoomDef->blockLayer, gBlockLayer, gRoomMetatileCount & 0x1FFFFF);
    }
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gHBlankScrollStarted = 0;
    gRoomUpdateFlags = 31;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    InitRoomBgLayout();
    InitRoomObjects();
    SetRoomEntryPoint();
    PlayRoomBgm();
    LoadRoomBgAnims();
    InitDoors();
    StartRoomBlockAnims();
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
                gSavedPlayerAbilities[i] = ABILITY_NORMAL;
                gSavedPlayerAbilityUses[i] = 0xFFFF;
                AddPlayerLives(-1, i);
            }
            if ((s16)gSavedPlayerAbilities[i] != ABILITY_NORMAL)
            {
                gPlayerAbilities[i] = gSavedPlayerAbilities[i];
                gPlayerAbilityUses[i] = gSavedPlayerAbilityUses[i];
                gSavedPlayerAbilities[i] = ABILITY_NORMAL;
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
    SpawnRoomObjectsOnLoad();
    if (gPlayerCount == 1)
        CameraResetBounds();
    else
        CameraResetBoundsToGroup();
    HudInit(gLocalPlayer);
    switch (gCameraMode)
    {
    default:
    case CAMERA_MODE_FOLLOW_FOCUS:
    case CAMERA_MODE_FOLLOW_PLAYER:
        CameraFollowFocus();
        break;
    case CAMERA_MODE_SCROLL_LOCKED:
        CameraFollowScrollLocked();
        break;
    case CAMERA_MODE_HOLD_ANCHOR:
        CameraHoldAnchor();
        break;
    }
    CameraInitPos();
    CameraWriteScrollParallax();
    SpawnRoomObjectsInView();
    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    switch (gRoomBgLayout)
    {
    default:
    case 0:
    case 2:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        LoadBg3Map();
        break;
    case 3:
        DrawBg2ViewLooping(gCameraPos[0]);
        LoadBg3Map();
        break;
    case 5:
        DrawBg23FullRows(gCameraPos[1]);
        break;
    case 1:
    case 4:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        DrawBg3View(gBg3Pos[0], gBg3Pos[1]);
        break;
    }
}

void sub_080233e0(void)
{
    u32 a;

    TaskSetSkipMask(0, gPressedBigSwitchSlot);
    TaskSetOthersSkipMask(15, gPressedBigSwitchSlot);
    BigSwitchStartRefill(gPressedBigSwitchSlot);
    gInHub = 0;
    gCurLevel = gLevelIndex;
    gUnk_03001F20 = gStageIndex;
    gCameraMode = CAMERA_MODE_FOLLOW_FOCUS;
    LoadGfxSet(1);
    CreateRoomTask(0);
    gCurRoomDef = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    gUnk_02007D64 = gCurRoomDef->unk57;
    gRoomBg3FullShake = gCurRoomDef->bg3FullShake;
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
    gRoomObjectList.count = gCurRoomDef->objectCount;
    gRoomObjectList.sortedByY = gCurRoomDef->objectsSortedByY;
    gRoomObjectList.entries = gCurRoomDef->objects;
    gRoomMap = gRoomMapBuffer;
    RequestCopy(8, (u32)gCurRoomDef->metatileTiles, (u32)gMetatileTiles, 0);
    gRoomUpdateFlags = 0;
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    CalcRoomBounds();
    sub_08028130();
    LoadRoomObjectGfx();
    InitDoors();
    StartBlockAnims();
    CameraSetFocusToLocalPlayer();
    if (gPlayerCount == 1)
        CameraResetBounds();
    else
        CameraResetBoundsToGroup();
    switch (gCameraMode)
    {
    default:
    case CAMERA_MODE_FOLLOW_FOCUS:
    case CAMERA_MODE_FOLLOW_PLAYER:
        CameraFollowFocus();
        break;
    case CAMERA_MODE_SCROLL_LOCKED:
        CameraFollowScrollLocked();
        break;
    case CAMERA_MODE_HOLD_ANCHOR:
        CameraHoldAnchor();
        break;
    }
    CameraInitPos();
    if (gRoomBgLayout == 2)
        gBg3Pos[0] = gBigSwitchReturnBg3X;
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    if (gRoomBgLayout == 1)
    {
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        DrawBg3View(gBg3Pos[0], gBg3Pos[1]);
    }
    else
    {
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        LoadBg3Map();
    }
}

void CreateRoomTask(s32 a)
{
    s32 id = TaskCreateFrom(TASK_ROOM, 63);
    struct Task *t;

    if (id != -1)
    {
        t = &gTasks[id];
        t->state = a;
    }
}
