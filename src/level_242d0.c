#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "hud.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"

/* level_242d0.c (0x080242D0-0x080261BF, issue #93).
 *
 * Room loaders, part 3: LoadCutsceneRoom (M02's stage sequence state,
 * CutsceneMain), LoadEndingRoom (entered through LoadEndingEpilogueRoom and
 * LoadEndingStarRodReturnRoom, which preset the level/stage/room and the player
 * position) and LoadCreditsRoom, plus the room-task variants 3-6 of task type
 * #3 (RoomTaskGoalGameInit, RoomTaskCutsceneInit, sub_08024904, RoomTaskCreditsInit) and their
 * per-frame bodies.  A loader looks the room header up in
 * gRoomTable[level][stage][room] and stores it in gCurRoomDef, copies
 * its size, origin, BG3 origin and object list into the camera cells,
 * decompresses (or CpuSet-copies) its metatile map into gRoomMapBuffer and
 * its metatile table into gMetatileTiles, resets the camera (M08) and the
 * players, streams the whole view and spawns task type #3.  LoadCreditsRoom is
 * the same shape as LoadRoom.
 * 
 * The doors, from FindDoorAt on (one translation unit with the loaders:
 * split at 0x08024E40, EnterDoor and ExitClearedStage swap hoisted address
 * registers, lesson 4.79).  FindDoorAt(x, y) finds an enterable door at a
 * pixel - the door metatiles 16/144, 54/182 and 55/183, the door records
 * RoomDef.doors and their locks gUsedSubGameDoors[]/gWarpStarStationLevels - and records
 * it in gFoundDoor (type << 8 | index); EnterDoor enters it, a 9-way
 * switch on the door kind (RoomDef door byte +6) that sets the next
 * level/stage/room, the arrival position gRoomEntryX/gRoomEntryY and
 * the stage request gStageRequest for M02's state bodies.  ExitClearedStage
 * (a stage cleared: the hub's next stage door, or on to the next level),
 * ExitKingDededeStage, ExitToNextRoom, ExitToNextRoomOnWarpStar, ExitToEnding, PressBigSwitch and
 * ReturnFromBigSwitchView are the other exits, each setting the level/stage/room and a
 * stage request; sub_08025e0c lowers the room's bottom bound, sub_08025e88
 * reads an object-list entry's parameter, and sub_08025f00, sub_080260b0
 * and sub_0802610c pick the position the player arrives at. */

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 x;
    /*0x06*/ u16 y;
};

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void RoomTaskGoalGameInit(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = (u32)RoomTaskDraw;
    t->updateCallback = (u32)RoomTaskSnapCameraToFocus;
    t->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2Bg3NoObjects;
    TaskSleepForever();
}

void LoadCutsceneRoom(void)
{
    u32 a;

    gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(3);
    if (gLevelIndex == 8)
    {
        gCurLevel = gStageIndex;
        gUnk_03001F20 = 48;
    }
    else
    {
        gCurLevel = gLevelIndex;
        gUnk_03001F20 = 48;
    }
    gCurRoomDef = gRoomTable[8][gCurLevel][gUnk_08732630[gCurLevel]];
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
    ResetBlockAnims();
    StopScreenShake();
    CalcBg3Parallax();
    InitRoomBgLayout();
    CalcRoomBounds();
    LoadRoomBgAnims();
    gCameraFocusX = gBg3Border[0] + 120;
    gCameraFocusY = gBg3Border[1] + 80;
    gActivePlayerMask = 0;
    gRoomBgLayout = 0;
    gUnk_0200B078 = 0;
    gUnk_02000020 = 0;
    CameraResetBounds();
    CameraSnapToFocus();
    CameraInitPos();
    CameraWriteScrollParallax();
    SetViewRectToPlayers();
    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    HudReset();
    if (gBg3MapShape != 0)
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

void RoomTaskCutsceneInit(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)RoomTaskSnapCameraToFocus;
    if (gBg3MapShape != 0)
        t->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2Bg3NoObjects;
    else
        t->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2NoObjects;
    TaskSleepForever();
}

void RoomTaskSnapCameraToFocus(void)
{
    if (gRoomUpdateFlags & 1)
        CameraSnapToFocus();
}

void RoomTaskLateUpdateBg2NoObjects(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2Map();
    }
    CameraWriteScrollParallax();
}

void RoomTaskLateUpdateBg2Bg3NoObjects(void)
{
    if (gRoomUpdateFlags & 2)
        UpdateScreenShake();
    if (gRoomUpdateFlags & 1)
    {
        UpdateBgAnims();
        CameraUpdatePos();
        StreamBg2Map();
        StreamBg3Map();
    }
    CameraWriteScrollParallax();
    HudUpdateAbilityPanel();
}

void LoadEndingEpilogueRoom(s32 x, s32 y)
{
    gLevelIndex = 7;
    gStageIndex = 0;
    gRoomIndex = 2;
    gRoomEntryX = x;
    gRoomEntryY = y;
    gRoomEntrySet = 1;
    LoadEndingRoom(0);
}

void LoadEndingStarRodReturnRoom(s32 x, s32 y)
{
    gLevelIndex = 6;
    gStageIndex = 6;
    gRoomIndex = 0;
    gRoomEntryX = x;
    gRoomEntryY = y;
    gRoomEntrySet = 1;
    LoadEndingRoom(1);
}

void LoadEndingRoom(s32 a0)
{
    u32 a;

    ResetTasksAndOam();
    gInHub = 0;
    gCurLevel = gLevelIndex;
    gUnk_03001F20 = gStageIndex;
    gCameraMode = 0;
    LoadGfxSet(1);
    CreateRoomTask(0);
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
    InitEndingRoomBgLayout(a0);
    SetRoomEntryPoint();
    LoadRoomBgAnims();
    StartRoomBlockAnims();
    CameraResetBounds();
    HudReset();
    switch (gCameraMode)
    {
    default:
    case 0:
    case 1:
    case 3:
        CameraFollowFocus();
        break;
    case 5:
        CameraHoldAnchor();
        break;
    }
    CameraInitPos();
    CameraWriteScrollParallax();
    a = 0;
    CpuFastSet(&a, (u32 *)(BG_VRAM + 0x2000), 0x01000400);
    switch (gRoomBgLayout)
    {
    default:
    case 0:
    case 2:
    case 3:
    case 5:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        LoadBg3Map();
        break;
    case 1:
    case 4:
        DrawBg2View(gCameraPos[0], gCameraPos[1]);
        DrawBg3View(gBg3Pos[0], gBg3Pos[1]);
        break;
    }
}

void sub_08024904(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)RoomTaskSnapCameraToFocus;
    switch (gRoomBgLayout)
    {
    case 1:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2Bg3;
        break;
    case 4:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg23HBlank;
        break;
    default:
    case 0:
    case 2:
    case 3:
    case 5:
        gCurTask->lateUpdateCallback = (u32)RoomTaskLateUpdateBg2;
        break;
    }
    TaskSleepForever();
}

void LoadCreditsRoom(void)
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
        gCameraMode = 5;
    else
        gCameraMode = 0;
    CreateRoomTask(6);
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
        InitPlayerState(i);
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    }
    SpawnRoomObjectsOnLoad();
    if (gPlayerCount == 1)
        CameraResetBounds();
    else
        CameraResetBoundsToGroup();
    HudReset();
    switch (gCameraMode)
    {
    default:
    case 0:
    case 1:
        CameraFollowFocus();
        break;
    case 3:
        CameraFollowScrollLocked();
        break;
    case 5:
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

void RoomTaskCreditsInit(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
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

s32 FindDoorAt(s32 x, s32 y)
{
    s32 type;
    s32 i;
    struct Door *d;

    gFoundDoor = 0xFF;
    if (gCurRoomDef->doorCount == 0)
        return 0;
    switch (GetCollisionTileAtPixel(x, y))
    {
    case 55:
    case 183:
        x -= 16;
        type = 1;
        break;
    case 54:
    case 182:
        type = 2;
        break;
    case 16:
    case 144:
        type = 1;
        break;
    default:
        return 0;
    }
    d = gCurRoomDef->doors;
    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        if (gInHub != 0)
        {
            if (d->unk0 != 0x270F)
                continue;
        }
        else
        {
            if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61)
                continue;
        }
        if (d->unk2 == (x >> 4) && d->unk4 == (y >> 4))
        {
            if (d->unk0 == 0x15B3)
            {
                gUnk_0200B038 = 1;
                d++;
                i++;
            }
            else
            {
                gUnk_0200B038 = 0;
            }
            break;
        }
    }
    if (gDoorStates[i].unk1 == 0)
        return 0;
    if (gInHub != 0)
    {
        switch ((u8)d->unk6)
        {
        case 3:
            if (gUsedSubGameDoors[gCurLevel] & 1)
                return 0;
            break;
        case 4:
            if (gUsedSubGameDoors[gCurLevel] & 2)
                return 0;
            break;
        case 5:
            if (gUsedSubGameDoors[gCurLevel] & 4)
                return 0;
            break;
        case 6:
            if ((gWarpStarStationLevels & ~(1 << gCurLevel)) == 0)
                return 0;
            break;
        }
    }
    gFoundDoor = (type << 8) | i;
    return 1;
}

s32 EnterDoor(void)
{
    struct RoomDef *room;
    struct Door *d;
    struct Door *e;
    s32 i;
    s32 idx;
    s32 t;

    idx = gFoundDoor & 0xFF;
    if (idx == 0xFF)
        return 0;
    d = &gCurRoomDef->doors[idx];
    gRoomEntrySet = 0;
    gHubUnlockFlags = 0;
    if (gInHub != 0)
    {
        if (d->unk0 == 0x270F)
        {
            gEntryDoorIndex = idx;
            switch (d->unk6 & 0xFF)
            {
            case 0:
                gLevelIndex = gStageIndex;
                gStageIndex = d->unk8;
                gRoomIndex = 0;
                gStageRequest = 2;
                gRoomEntryMode = 0;
                gEntryDoorEvent = 0;
                break;
            case 1:
                if (--gStageIndex < 0)
                    gStageIndex = 0;
                gRoomIndex = 0;
                room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
                e = room->doors;
                for (i = 0; i < room->doorCount; e++, i++)
                {
                    if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 2)
                        break;
                }
                gRoomEntryX = e->unk2 * 16 + 22;
                gRoomEntryY = e->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 1;
                gRoomEntryMode = 1;
                gEntryDoorEvent = 0;
                gCutscenePending = 0;
                break;
            case 2:
                if (gCurLevel >= gFurthestLevel)
                {
                    gLevelIndex = gStageIndex;
                    gStageIndex = gUnk_08334EB4[gLevelIndex] - 1;
                    gRoomIndex = 0;
                    gUnk_02008054 = 0x100;
                    gUnk_0200B038 = 1;
                    gStageRequest = 2;
                    gRoomEntryMode = 0;
                }
                else
                {
                    if (++gStageIndex > 7)
                        gStageIndex = 7;
                    gRoomIndex = 0;
                    room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
                    e = room->doors;
                    for (i = 0; i < room->doorCount; e++, i++)
                    {
                        if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 1)
                            break;
                    }
                    gRoomEntryX = e->unk2 * 16 + 22;
                    gRoomEntryY = e->unk4 * 16 + 5;
                    gRoomEntrySet = 1;
                    gStageRequest = 1;
                    gRoomEntryMode = 1;
                    gCutscenePending = 0;
                }
                gEntryDoorEvent = 0;
                break;
            case 3:
                gSubGameLevel = gUnk_087323E2[gCurLevel][0][gExtraMode];
                gUsedSubGameDoors[gStageIndex] |= 1;
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 10;
                gRoomEntryMode = 1;
                gEntryDoorEvent = 2;
                break;
            case 4:
                gSubGameLevel = gUnk_087323E2[gCurLevel][1][gExtraMode];
                gUsedSubGameDoors[gStageIndex] |= 2;
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 11;
                gRoomEntryMode = 1;
                gEntryDoorEvent = 2;
                break;
            case 5:
                gSubGameLevel = gUnk_087323E2[gCurLevel][2][gExtraMode];
                gUsedSubGameDoors[gStageIndex] |= 4;
                gRoomEntryX = d->unk2 * 16 + 22;
                gRoomEntryY = d->unk4 * 16 + 5;
                gRoomEntrySet = 1;
                gStageRequest = 9;
                gRoomEntryMode = 1;
                gEntryDoorEvent = 2;
                break;
            case 6:
            case 7:
            case 8:
                gStageRequest = *(u8 *)&d->unk6 + 6;
                d++;
                gRoomIndex = d->unk0;
                gRoomEntryX = d->unk6;
                gRoomEntryY = d->unk8 + 0xFFFD;
                gRoomEntrySet = 1;
                gUnk_02008054 = 0x200;
                gUnk_0200B038 = 1;
                gRoomEntryMode = 0;
                gEntryDoorEvent = 0;
                break;
            default:
                return 0;
            }
        }
    }
    else if (gUnk_02007D64 != 0)
    {
        gRoomIndex = d->unk0;
        t = gGameState - 11;
        room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
        e = room->doors;
        for (i = 0; i < room->doorCount; e++, i++)
        {
            if (e->unk0 == 0x270F && (e->unk6 & 0xFF) == t)
                break;
        }
        gRoomEntryX = e->unk2 * 16 + 22;
        gRoomEntryY = e->unk4 * 16 + 5;
        gRoomEntrySet = 1;
        gStageRequest = 1;
        gEntryDoorEvent = 0;
        gRoomEntryMode = 1;
    }
    else
    {
        if (d->unk0 == 0x22B8)
        {
            switch (gStageClearStatus[gLevelIndex][gStageIndex])
            {
            case 0:
                gEntryDoorEvent = 1;
                gHubUnlockFlags = 1;
                gHubUnlockSource = gStageIndex + 1;
                if (gHubDoorUnlocks[gLevelIndex][6] == (s8)gUnk_03001F20)
                {
                    gHubUnlockFlags = 17;
                    gWarpStarStationLevels |= 1 << gLevelIndex;
                }
                if (gUnk_08732302[gLevelIndex][gStageIndex] == -1
                    || (gBigSwitchFlags[0] & (1 << gUnk_08732302[gLevelIndex][gStageIndex])))
                    gStageClearStatus[gLevelIndex][gStageIndex] = 2;
                else
                    gStageClearStatus[gLevelIndex][gStageIndex] = 1;
                if (gFurthestLevel <= gLevelIndex && gFurthestStage <= gStageIndex)
                    gFurthestStage = gStageIndex + 1;
                break;
            case 1:
                if (gUnk_08732302[gLevelIndex][gStageIndex] == -1
                    || (gBigSwitchFlags[0] & (1 << gUnk_08732302[gLevelIndex][gStageIndex])))
                    gStageClearStatus[gLevelIndex][gStageIndex] = 2;
            case 2:
            default:
                gEntryDoorEvent = 0;
                break;
            }
            gLevelIndex = 8;
            gStageIndex = gCurLevel;
            gRoomIndex = 0;
            room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
            e = room->doors;
            for (i = 0; i < room->doorCount; e++, i++)
            {
                if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 0 && e->unk8 == (s8)gUnk_03001F20)
                    break;
            }
            gRoomEntryX = e->unk2 * 16 + 22;
            gRoomEntryY = e->unk4 * 16 + 5;
            gRoomEntrySet = 1;
            if (gMetaKnightmareMode == 0)
            {
                gStageRequest = 8;
            }
            else
            {
                gStageRequest = 1;
                gContinueLevel = gCurLevel;
                gUnk_02007FF8 = gUnk_03001F20;
            }
            gRoomEntryMode = 1;
        }
        else
        {
            if (gUnk_0200B038 != 0)
            {
                gUnk_02008054 = gRoomIndex;
                gUnk_0200AFF4 = d->unk2 * 16 + 16;
                gUnk_02008050 = d->unk4 * 16 + 5;
            }
            gRoomIndex = d->unk0;
            gRoomEntryX = d->unk6;
            gRoomEntryY = d->unk8 + 0xFFFD;
            gRoomEntrySet = 1;
            gStageRequest = 3;
            gEntryDoorEvent = 0;
            gRoomEntryMode = 0;
        }
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 = 0;
    }
    return gFoundDoor >> 8;
}

void ExitClearedStage(void)
{
    if (gGameState == 8)
    {
        struct RoomDef *room;
        struct Door *d;
        s32 i;

        gStageIndex = gCurLevel;
        gLevelIndex = 8;
        gStageIndex++;
        if (gStageIndex > 7)
            gStageIndex = 7;
        gRoomIndex = 0;
        room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
        d = room->doors;
        for (i = 0; i < room->doorCount; i++)
        {
            if (d->unk0 == 0x270F && (u8)d->unk6 == 1)
                break;
            d++;
        }
        gRoomEntryX = d->unk2 * 16 + 22;
        gRoomEntryY = d->unk4 * 16 + 5;
        gRoomEntrySet = 1;
        gStageRequest = 1;
        gEntryDoorEvent = 0;
        gRoomEntryMode = 1;
        gCutscenePending = 1;
        sub_0802695c();
        gUnk_02007FF0 = 0;
    }
    else
    {
        gLevelIndex = gCurLevel + 1;
        gStageIndex = gUnk_08334EB4[gLevelIndex] - 1;
        gRoomIndex = 0;
        gRoomEntryMode = 0;
        gRoomEntrySet = 0;
        gStageRequest = 1;
    }
    gUnk_0200B038 = 0;
}

void ExitKingDededeStage(void)
{
    if (gMetaKnightmareMode == 0)
    {
        gLevelIndex = 7;
        gStageIndex = 0;
        gRoomIndex = 0;
        gRoomEntrySet = 0;
        if (gGameState == 8)
        {
            gCutscenePending = 1;
            gStageRequest = 2;
        }
        else
        {
            gStageRequest = 1;
        }
        gEntryDoorEvent = 0;
        gRoomEntryMode = 0;
        gUnk_0200B038 = 0;
    }
    else
    {
        gStageRequest = 7;
    }
    gUnk_02007FF0 = 0;
    gStageExitFlags |= 2;
}

void ExitToNextRoom(void)
{
    gRoomIndex++;
    gRoomEntrySet = 0;
    gStageRequest = 3;
    gEntryDoorEvent = 0;
    gRoomEntryMode = 0;
    gStageExitFlags |= 2;
}

void ExitToNextRoomOnWarpStar(void)
{
    gRoomIndex++;
    gRoomEntrySet = 0;
    gStageRequest = 3;
    gEntryDoorEvent = 0;
    gUnk_02004C98 = 0;
    gRoomEntryMode = 2;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
}

void ExitToEnding(void)
{
    if (gGameState == 8)
    {
        if (gExtraMode != 0)
            gMilestoneFlags |= 2;
        else
            gMilestoneFlags |= 1;
        gCurLevel = 6;
        gUnk_03001F20 = 32;
        gStageExitFlags |= 1;
    }
    gUnk_02007FF0 = 0;
    gStageRequest = 7;
}

void PressBigSwitch(s32 id)
{
    struct RoomDef *room;
    struct Door *d;
    s32 i;
    s8 v;
    s32 lvl;

    if (gUnk_02005590[id - 32] == -1)
        return;
    gBigSwitchReturnLevel = gLevelIndex;
    gBigSwitchReturnStage = gStageIndex;
    gBigSwitchReturnRoom = gRoomIndex;
    gLevelIndex = 8;
    gStageIndex = gCurLevel;
    gRoomIndex = 0;
    room = gRoomTable[gLevelIndex][gStageIndex][gRoomIndex];
    d = room->doors;
    for (i = 0; i < room->doorCount; d++, i++)
    {
        if (d->unk0 == 0x270F && (u8)d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
            break;
    }
    gPressedBigSwitchSlot = id;
    gHubUnlockFlags = 1;
    v = ((s8 *)gRoomObjectList.entries[gUnk_02005590[id - 32]].filler0)[2];
    gHubUnlockSource = v | 0x100;
    gBigSwitchFlags[0] |= 1 << v;
    if (gStageClearStatus[gCurLevel][(s8)gUnk_03001F20] == 1)
        gStageClearStatus[gCurLevel][(s8)gUnk_03001F20] = 2;
    lvl = gCurLevel;
    if (gHubDoorUnlocks[lvl][6] == gHubUnlockSource)
    {
        gHubUnlockFlags |= 16;
        gWarpStarStationLevels |= 1 << lvl;
    }
    gRoomEntryMode = 4;
    gEntryDoorEvent = 3;
    gRoomEntryX = d->unk2 * 16 + 22;
    gRoomEntryY = d->unk4 * 16 + 5;
    gRoomEntrySet = 1;
    if (gRoomBgLayout == 2)
        gBigSwitchReturnBg3X = gBg3Pos[0];
    gStageRequest = 4;
    gStageExitFlags |= 5;
}

void ReturnFromBigSwitchView(void)
{
    gLevelIndex = gBigSwitchReturnLevel;
    gStageIndex = gBigSwitchReturnStage;
    gRoomIndex = gBigSwitchReturnRoom;
    gStageRequest = 3;
}

void sub_08025e00(void)
{
    CameraResetRoomView();
}

void sub_08025e0c(void)
{
    s32 i;

    gRoomBounds[2] = gRoomHeight * 16 - gRoomBorder[1] - 80;
    gCameraBounds[2] = gRoomBounds[2];
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            gPlayerBounds[i].y0 = gCameraBounds[2] - 76;
    }
    gViewRect[2] = gRoomBounds[2] - 80;
}

s32 sub_08025e88(s32 i)
{
    struct Unk020055D8Entry *e = &gRoomObjectList.entries[gUnk_02005590[i - 32]];

    if (gUnk_0200B078 == 3)
    {
        gUnk_0200B02C = e->filler0[3] - 1;
        if (gUnk_0200B02C < gStageIndex)
        {
            gUnk_02004C98 = 1;
            return 2;
        }
        gUnk_02004C98 = 2;
        return 0;
    }
    gUnk_02004C98 = 0;
    return ((s8 *)e->filler0)[2];
}

s32 sub_08025f00(void)
{
    s32 i;

    if (gUnk_02004C98 == 0)
    {
        struct Door *d = gCurRoomDef->doors;

        for (i = 0; i < gCurRoomDef->doorCount; i++)
        {
            if (d->unk0 == 0x1E61 || d->unk0 == 0x1A0A)
                break;
            d++;
        }
        gRoomIndex = d->unk2;
        gRoomEntryX = d->unk6;
        gRoomEntryY = d->unk8;
        gRoomEntrySet = 1;
        gStageRequest = 3;
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 = 0;
    }
    else
    {
        gStageIndex = gUnk_0200B02C;
        gRoomIndex = 0;
        for (i = 0; i < 32; i++)
        {
            if (gRoomTable[gLevelIndex][gStageIndex][i]->unk54 == 3)
            {
                gRoomIndex = i;
                break;
            }
        }
        if (gUnk_02004C98 == 2)
            gRoomEntryX = 0;
        else
            gRoomEntryX = gRoomWidth * 16;
        gRoomEntryY = gRoomHeight * 16 - gRoomBorder[1] - 168;
        gRoomEntrySet = 1;
        gStageRequest = 12;
        gCutscenePending = 0;
    }
    gRoomEntryMode = 2;
    gUnk_0200B038 = 0;
    return 1;
}

s32 sub_080260b0(void)
{
    s32 r;

    if (gUnk_02004C98 == 0)
    {
        struct Door *d = gCurRoomDef->doors;
        s32 i;

        for (i = 0; i < gCurRoomDef->doorCount; i++)
        {
            if (d->unk0 == 0x1A0A)
                break;
            d++;
        }
        r = d->unk4;
    }
    else
    {
        r = 3;
        if (gUnk_02004C98 == 2)
            r = 1;
    }
    gUnk_02004C98 = 0;
    return r;
}

s32 sub_0802610c(void)
{
    struct Door *d = gCurRoomDef->doors;
    s32 i;

    for (i = 0; i < gCurRoomDef->doorCount; i++)
    {
        if (d->unk0 == 0x1E61)
            break;
        d++;
    }
    gRoomIndex = d->unk2;
    gRoomEntryX = d->unk6;
    gRoomEntryY = d->unk8;
    gRoomEntrySet = 1;
    gStageRequest = 3;
    gRoomEntryMode = 3;
    gUnk_0200B038 = 0;
    if (gUnk_02007D60 & 0x8000)
        gUnk_02007D60 = 0;
    return 1;
}
