#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "hud.h"
#include "room.h"
#include "camera.h"
#include "save.h"

/* stage_270d0.c (0x080270D0-0x0802739F, issue #93).
 *
 * Stage helpers, part 2.  sub_080270d0 restarts the room's BGM (with
 * the 0x800 flag), StopRoomAndApplyExitFlags/StopRoom/FreeRoomAndDoorObjects tear the level
 * down before a state change (flags in gStageExitFlags), PauseRoom/
 * SetRoomUpdateFlags/ResumeRoom set the per-frame flags gRoomUpdateFlags, and
 * PauseSaveBgPalette/PauseRestoreRoomGraphics save and restore the OBJ palette and tiles
 * around M02's pause screen.  ReturnToHubStageDoor picks the hub door the player
 * returns to (gRoomEntryX/gRoomEntryY).  The file stops before
 * ReturnToRestartPoint because that function only matches without these nine in
 * front of it in the translation unit (lesson 4.79). */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void sub_080270d0(void)
{
    s32 bgm = gCurRoomDef->bgm;
    s16 v;

    if (bgm == -1)
    {
        StopBgm();
    }
    else if (gUnk_087325A2[bgm] != -1)
    {
        v = gUnk_087325A2[bgm] | 0x800;
        PlayBgm(v);
    }
    else
    {
        PlayBgm(bgm | 0x800);
    }
}

void StopRoomAndApplyExitFlags(void)
{
    PauseBlockAnims();
    StopHBlankScroll();
    TaskFree(63);
    if (gStageExitFlags & 1)
        SaveAndSetContinuePoint();
    if (gStageExitFlags & 2)
        StopAllSfx();
    if (gStageExitFlags & 4)
        TaskSetOthersSkipMask(31, 63);
    gStageExitFlags = 0;
}

void StopRoom(void)
{
    PauseBlockAnims();
    StopHBlankScroll();
    TaskFree(63);
    gStageExitFlags = 0;
}

void FreeRoomAndDoorObjects(void)
{
    s32 i;
    s32 j;

    TaskFree(63);
    for (j = 0; j <= 31; j++)
        for (i = 0; i <= 2; i++)
            if (gDoorObjectTasks[j][i] != -1)
                TaskFree(gDoorObjectTasks[j][i]);
    PauseBlockAnims();
}

void PauseRoom(void)
{
    gRoomUpdateFlags = 0;
    PauseBlockAnims();
    HoldHBlankScroll();
}

void SetRoomUpdateFlags(u32 a)
{
    gRoomUpdateFlags = a;
}

void ResumeRoom(void)
{
    gRoomUpdateFlags = 31;
    ResumeBlockAnims();
    ResumeHBlankScroll();
}

void PauseSaveBgPalette(void)
{
    CpuSet(gBgPaletteBank8, gPauseSavedBgPalette, 128);
}

void PauseRestoreRoomGraphics(void)
{
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1F00;
    RequestCopy(8, (u32)gCurRoomDef->bg3Tiles, BG_VRAM + 0x8000, 0);
    CpuSet(gPauseSavedBgPalette, gBgPaletteBank8, 128);
    if (gBg3MapShape == 1)
        SetBg3ScreenSize(0x8000);
    if (gUnk_0200B078 == 6)
        SetBg23ScreenSize(0);
    else if (gUnk_0200B078 == 1)
        SetBg23ScreenSize(0x8000);
    CameraWriteScrollParallax();
    if (gHBlankScrollStarted != 0)
        RestoreRoomHBlankScroll();
    HudRedrawClock();
}

void ReturnToHubStageDoor(void)
{
    struct RoomDef *r;
    struct Door *d;
    s32 i;

    gLevelIndex = 8;
    gStageIndex = gCurLevel;
    gRoomIndex = 0;
    r = gRoomTable[gLevelIndex][gStageIndex][0];
    d = r->doors;
    for (i = 0; i < r->doorCount; d++, i++)
    {
        if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == DOOR_KIND_STAGE && d->unk8 == (s8)gCurStage)
            break;
    }
    gRoomEntryX = (d->unk2 << 4) + 22;
    gRoomEntryY = (d->unk4 << 4) + 5;
    gRoomEntrySet = 1;
    gEntryDoorEvent = 0;
    gRoomEntryMode = ROOM_ENTRY_DOOR;
}
