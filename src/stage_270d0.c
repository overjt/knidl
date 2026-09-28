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
 * the 0x800 flag), sub_08027128/sub_08027178/sub_08027198 tear the level
 * down before a state change (flags in gStageExitFlags), PauseRoom/
 * SetRoomUpdateFlags/ResumeRoom set the per-frame flags gRoomUpdateFlags, and
 * sub_08027228/sub_08027240 save and restore the OBJ palette and tiles
 * around M02's pause screen.  sub_080272dc picks the hub door the player
 * returns to (gRoomEntryX/gRoomEntryY).  The file stops before
 * sub_080273a0 because that function only matches without these nine in
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

void sub_08027128(void)
{
    PauseBlockAnims();
    StopHBlankScroll();
    TaskFree(63);
    if (gStageExitFlags & 1)
        sub_08026998();
    if (gStageExitFlags & 2)
        StopAllSfx();
    if (gStageExitFlags & 4)
        TaskSetOthersSkipMask(31, 63);
    gStageExitFlags = 0;
}

void sub_08027178(void)
{
    PauseBlockAnims();
    StopHBlankScroll();
    TaskFree(63);
    gStageExitFlags = 0;
}

void sub_08027198(void)
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

void sub_08027228(void)
{
    CpuSet(gUnk_03001370, gUnk_02008060, 128);
}

void sub_08027240(void)
{
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1F00;
    RequestCopy(8, (u32)gCurRoomDef->bg3Tiles, 0x06008000, 0);
    CpuSet(gUnk_02008060, gUnk_03001370, 128);
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

void sub_080272dc(void)
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
        if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
            break;
    }
    gRoomEntryX = (d->unk2 << 4) + 22;
    gRoomEntryY = (d->unk4 << 4) + 5;
    gRoomEntrySet = 1;
    gEntryDoorEvent = 0;
    gRoomEntryMode = 1;
}
