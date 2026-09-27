#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_261c0.c (0x080261C0-0x08026B5F, issue #93).
 *
 * Stage helpers the rest of the game calls.  RequestScreenShake starts a screen
 * shake (gScreenShake, levels 1-4 only upgrade), TaskCreateHighSlot spawns a task
 * in one of the free high slots 32-62, SetCameraFocus/SetCameraFocusOrAnchor place the
 * player or the camera target, sub_080262e8 spawns a map event at a
 * tabled position, sub_08026308 wraps every camera, object and task
 * coordinate back by 0x200 pixels in a looping room, sub_08026900 clamps
 * the player to the room bounds and sub_0802695c starts the next stage.
 * sub_080264b0, sub_0802651c, sub_0802653c, sub_08026584 and sub_08026704
 * spawn and adjust the M08 stage objects of the door the player entered by
 * (gUnk_0200B034, its slots in gDoorObjectTasks); sub_0802672c/sub_08026834
 * spawn a map-event task and put the camera on the player or a partner.
 * sub_08026a0c, sub_08026a80 and sub_08026aec arm the scroll lock of one
 * room each.  sub_08026994 is an empty dead export. */

struct Unk03005670
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 filler01;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ u8 unk6;
};

struct CamRect { s16 x0, x1, y0, y1; };

struct Unk020061F0
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u8 filler08[0x18];
};

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

struct RoomDef
{
    /*0x00*/ u8 filler00[4];
    /*0x04*/ s8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 *unk18;
    /*0x1C*/ void *unk1C;
    /*0x20*/ void *unk20;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u16 *unk28;
    /*0x2C*/ void *unk2C;
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u16 unk34;
    /*0x36*/ u16 unk36;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 unk3A;
    /*0x3C*/ u16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *unk44;
    /*0x48*/ void *unk48;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 unk50;
    /*0x52*/ u16 unk52;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct Unk03005680
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
};

extern struct Unk03005670 gScreenShake;
extern vs16 gTaskSlotTypes[];
extern s16 gCameraFocusX;
extern s16 gCameraFocusY;
extern s8 gUnk_03002444;
extern u16 gCameraMode;
extern s16 gCameraAnchorX;
extern s16 gCameraAnchorY;
extern u8 gUnk_0200D080;
extern u16 gUnk_08732638[][2];
extern u16 gCameraStreamPos[2];
extern u16 gCameraPos[2];
extern s32 gCameraCenterX;
extern u16 gBg3Pos[2];
extern u16 gBg3StreamPos[2];
extern s16 gViewRect[4];
extern s16 gCameraBounds[4];
extern s16 gUnk_020055B8[4];
extern u16 gPlayerCount;
extern struct CamRect gPlayerBounds[4];
extern s16 gSpriteCameraX;
extern u16 gUnk_02007FA0;
extern struct Unk020061F0 gBreakingBlocks[];
extern s16 gRoomWidth;
extern struct RoomDef *gCurRoomDef;
extern s8 gDoorObjectTasks[][3];
extern s8 gUnk_0200B034;
extern u8 gUnk_0200AF00;
extern u8 gUnk_03002400[8][7];
extern s8 gStageIndex;
extern u8 gUnk_0200AF08;
extern s16 gRoomBounds[4];
extern u8 gUnk_020055E8;
extern u8 gUnk_02007D38;
extern s16 gRoomEntryX;
extern s16 gRoomEntryY;
extern s8 gUnk_030023E0;
extern s8 gUnk_030023B8;
extern s8 gUnk_03002384;
extern u8 gUnk_03001F20;
extern u16 gUnk_02007FB0;
extern u8 gUnk_03001F30;
extern s32 gCurSaveSlot;
extern u8 gUnk_02005578;
extern s8 gUnk_02007FF8;
extern struct Unk03005680 gScrollLock;

void PlaySfx(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskSetEntry(void *a, u32 i);
void CameraWriteScrollParallax(void);
void StartScrollLock(s32 x0, s32 x1, s32 y0, s32 y1);
void CameraStartHoldAnchor(void);
s32 CreateMapEvent(s32 type);
s32 sub_0802d478(s32 x, s32 y);
s32 sub_0802ed94(s32 x, s32 y, s32 a);
s32 sub_0802ef90(s32 x, s32 y, s32 a, s32 b);
void sub_0802f1dc(void);
void sub_0802f2fc(void);
void sub_0802f400(void);
void sub_0802f6c0(void);
void sub_0802f6f4(void);
s32 CreateStageEffect(s32 a, s32 x, s32 y);
s32 BreakBlockAt(u32 x, u32 y);
void sub_08031738(u32 x, u32 y);
void SaveProgress(s32 a);

s32 CreateBlockBreakEffect(s32 x, s32 y)
{
    return CreateStageEffect(0, x, y);
}

void RequestScreenShake(u16 a)
{
    if (a > 7)
        return;
    if (a == 0)
    {
        gScreenShake.unk0 = 0;
        gScreenShake.unk2 = 0;
        gScreenShake.unk4 = 0;
        gScreenShake.unk6 = 0;
    }
    else if (gScreenShake.unk0 == 0 || gScreenShake.unk0 > 4 || gScreenShake.unk0 <= a)
    {
        gScreenShake.unk0 = a;
        gScreenShake.unk2 = 0;
        gScreenShake.unk4 = 0;
        gScreenShake.unk6 = 0;
    }
}

s32 TaskCreateHighSlot(s32 type)
{
    s32 i = 62;

    while (gTaskSlotTypes[i] != -1)
    {
        i--;
        if (i <= 31)
            return -1;
    }
    return TaskCreateFrom(type, i);
}

void SetCameraFocus(s32 x, s32 y)
{
    gCameraFocusX = x;
    gCameraFocusY = y;
}

void SetCameraFocusOrAnchor(s32 x, s32 y)
{
    if (gUnk_03002444 != 0)
    {
        if (gCameraMode != 2 && gCameraMode != 4)
        {
            gCameraFocusX = x;
            gCameraFocusY = y;
            return;
        }
    }
    else if (gCameraMode != 5)
    {
        gCameraFocusX = x;
        gCameraFocusY = y;
        return;
    }
    gCameraAnchorX = x;
    gCameraAnchorY = y;
}

void sub_080262dc(void)
{
    gUnk_0200D080 = 2;
}

void sub_080262e8(s32 a)
{
    sub_0802d478(gUnk_08732638[a][0], gUnk_08732638[a][1]);
}

void sub_08026308(void)
{
    s32 i;

    if (gCameraStreamPos[0] > 0x2FF && gCameraPos[0] > 0x300)
    {
        gCameraCenterX &= 0x1FFFFFF;
        gCameraPos[0] &= 0x1FF;
        gCameraStreamPos[0] &= 0x1FF;
        gBg3Pos[0] &= 0x1FF;
        gBg3StreamPos[0] &= 0x1FF;
        CameraWriteScrollParallax();
        gViewRect[0] &= 0x1FF;
        gViewRect[1] &= 0x1FF;
        gCameraBounds[0] &= 0x1FF;
        gCameraBounds[1] &= 0x1FF;
        gUnk_020055B8[0] &= 0x1FF;
        gUnk_020055B8[1] &= 0x1FF;
        for (i = 0; i < gPlayerCount; i++)
        {
            gPlayerBounds[i].x0 &= 0x1FF;
            gPlayerBounds[i].x1 &= 0x1FF;
        }
        gCameraFocusX &= 0x1FF;
        gCameraAnchorX &= 0x1FF;
        gSpriteCameraX &= 0x1FF;
        gUnk_02007FA0 &= 31;
        for (i = 0; i < 64; i++)
        {
            if (gBreakingBlocks[i].unk6 != 0xFFFF)
            {
                gBreakingBlocks[i].unk0 &= 31;
                gBreakingBlocks[i].unk4 = gBreakingBlocks[i].unk0 + gBreakingBlocks[i].unk2 * gRoomWidth;
            }
        }
        for (i = 0; i < 64; i++)
        {
            if (gTaskSlotTypes[i] != -1)
            {
                gTasks[i].posX &= 0x1FFFFFF;
                gTasks[i].unk48 &= 0x1FF;
            }
        }
    }
}

s32 sub_080264b0(void)
{
    s32 r = -1;
    struct Door *d;

    if (gDoorObjectTasks[gUnk_0200B034][0] != -1 && gUnk_0200B034 != -1)
    {
        d = &gCurRoomDef->unk44[gUnk_0200B034];
        r = sub_0802ed94((d->unk2 << 4) + 16, (d->unk4 << 4) + 8, gTasks[gDoorObjectTasks[gUnk_0200B034][0]].unk20);
    }
    return r;
}

void sub_0802651c(s32 i)
{
    if (i != -1)
        gTasks[i].unk1C = 1;
}

s32 sub_0802653c(void)
{
    s32 r = -1;
    struct Door *d;

    if (gUnk_0200B034 != -1)
    {
        d = &gCurRoomDef->unk44[gUnk_0200B034];
        r = sub_0802ef90((d->unk2 << 4) + 16, (d->unk4 << 4) + 8, 3, 0x2000);
    }
    return r;
}

s32 sub_08026584(void)
{
    s32 n = 0;
    struct Door *d;
    struct Door *doors;

    switch (gUnk_0200AF00)
    {
    case 0:
        break;
    case 1:
    case 3:
        if (gDoorObjectTasks[gUnk_0200B034][0] == -1 || gUnk_0200B034 == -1)
            break;
        doors = gCurRoomDef->unk44;
        d = (struct Door *)(gUnk_0200B034 * sizeof(struct Door) + (u32)doors);
        switch (gUnk_03002400[gStageIndex][d->unk8])
        {
        case 0:
            break;
        case 1:
            TaskSetEntry(sub_0802f6c0, gDoorObjectTasks[gUnk_0200B034][0]);
            break;
        case 2:
            CreateStageEffect(4, (d->unk2 << 4) + 16, (d->unk4 << 4) + 8);
            n = 1;
            TaskSetEntry(sub_0802f6f4, gDoorObjectTasks[gUnk_0200B034][0]);
            break;
        }
        break;
    case 2:
        if (gDoorObjectTasks[gUnk_0200B034][0] == -1 || gUnk_0200B034 == -1)
            break;
        switch (*(u8 *)&gCurRoomDef->unk44[gUnk_0200B034].unk6)
        {
        case 3:
            TaskSetEntry(sub_0802f2fc, gDoorObjectTasks[gUnk_0200B034][0]);
            break;
        case 4:
            TaskSetEntry(sub_0802f400, gDoorObjectTasks[gUnk_0200B034][0]);
            break;
        case 5:
            TaskSetEntry(sub_0802f1dc, gDoorObjectTasks[gUnk_0200B034][0]);
            break;
        default:
            return;
        }
        CreateStageEffect(4, (gCurRoomDef->unk44[gUnk_0200B034].unk2 << 4) + 16, (gCurRoomDef->unk44[gUnk_0200B034].unk4 << 4) + 8);
        n++;
        PlaySfx(0x11B);
        break;
    }
    return n;
}

void sub_08026704(s32 i)
{
    struct Task *t;

    if (i != -1)
    {
        t = &gTasks[i];
        t->layer = 15;
        t->unk3E = 0x4000;
    }
}

s32 sub_0802672c(void)
{
    s32 id = CreateMapEvent(5);
    struct Task *t;
    u8 f;

    if (id != -1)
    {
        t = &gTasks[id];
        f = gUnk_0200AF08 & 16;
        if (f != 0)
            t->unk24 = 1;
        else
            t->unk24 = 0;
        if (gPlayerCount == 1)
        {
            gCameraAnchorX = gCameraFocusX;
            gCameraAnchorY = gCameraFocusY;
        }
        else
        {
            gCameraAnchorX = gTasks[gUnk_02007D38].unk48;
            gCameraAnchorY = gTasks[gUnk_02007D38].unk4A;
        }
        if (gCameraAnchorX < gRoomBounds[0])
            gCameraAnchorX = gRoomBounds[0];
        if (gCameraAnchorX > gRoomBounds[1])
            gCameraAnchorX = gRoomBounds[1];
        if (gCameraAnchorY < gRoomBounds[2])
            gCameraAnchorY = gRoomBounds[2];
        if (gCameraAnchorY > gRoomBounds[3])
            gCameraAnchorY = gRoomBounds[3];
        CameraStartHoldAnchor();
        gUnk_020055E8 = 0;
    }
    return id;
}

s32 sub_08026834(void)
{
    s32 id = CreateMapEvent(6);
    struct Task *t;
    u8 f;

    if (id != -1)
    {
        t = &gTasks[id];
        f = gUnk_0200AF08 & 16;
        if (f != 0)
            t->unk24 = 1;
        else
            t->unk24 = 0;
        gCameraAnchorX = gCameraFocusX = gRoomEntryX;
        gCameraAnchorY = gCameraFocusY = gRoomEntryY;
        if (gCameraAnchorX < gRoomBounds[0])
            gCameraAnchorX = gRoomBounds[0];
        if (gCameraAnchorX > gRoomBounds[1])
            gCameraAnchorX = gRoomBounds[1];
        if (gCameraAnchorY < gRoomBounds[2])
            gCameraAnchorY = gRoomBounds[2];
        if (gCameraAnchorY > gRoomBounds[3])
            gCameraAnchorY = gRoomBounds[3];
        CameraStartHoldAnchor();
        gUnk_020055E8 = 0;
    }
    return id;
}

void sub_08026900(void)
{
    if (gCameraFocusX < gRoomBounds[0])
        gCameraFocusX = gRoomBounds[0];
    if (gCameraFocusX > gRoomBounds[1])
        gCameraFocusX = gRoomBounds[1];
    if (gCameraFocusY < gRoomBounds[2])
        gCameraFocusY = gRoomBounds[2];
    if (gCameraFocusY > gRoomBounds[3])
        gCameraFocusY = gRoomBounds[3];
}

void sub_0802695c(void)
{
    gUnk_030023E0 = gUnk_030023B8 + 1;
    gUnk_03002384 = 0;
    gUnk_030023B8 = gUnk_030023E0;
    gUnk_03001F20 = 16;
    gUnk_02007FB0 |= 1;
}

void sub_08026994(void)
{
}

void sub_08026998(void)
{
    if (gUnk_03001F30 == 0)
        SaveProgress(gCurSaveSlot);
    gUnk_02005578 = gUnk_030023B8;
    gUnk_02007FF8 = gUnk_03001F20;
}

void sub_080269d8(u32 x, u32 y)
{
    sub_08031738(x >> 4, y >> 4);
}

void sub_080269e8(void)
{
    s32 i;
    s32 j;

    for (j = 2; j <= 4; j++)
        for (i = 5; i <= 8; i++)
            BreakBlockAt(i, j);
}

u32 sub_08026a0c(void)
{
    switch (gCameraMode)
    {
    case 0:
        if (gViewRect[3] > 0x167)
        {
            gScrollLock.unkC = 280;
            StartScrollLock(0xFFFF, 0xFFFF, 200, 360);
        }
        break;
    case 3:
        return 1;
    case 1:
    case 2:
    case 4:
    case 5:
        break;
    }
    return 0;
}

u32 sub_08026a80(void)
{
    switch (gCameraMode)
    {
    case 0:
        if (gViewRect[2] <= 69)
        {
            gScrollLock.unkC = 149;
            StartScrollLock(0xFFFF, 0xFFFF, 16, 176);
        }
        break;
    case 3:
        return 1;
    case 1:
    case 2:
    case 4:
    case 5:
        break;
    }
    return 0;
}

u32 sub_08026aec(void)
{
    switch (gCameraMode)
    {
    case 0:
        if (gViewRect[3] > 0x147)
        {
            gScrollLock.unkC = 248;
            StartScrollLock(0xFFFF, 0xFFFF, 168, 328);
        }
        break;
    case 3:
        return 1;
    case 1:
    case 2:
    case 4:
    case 5:
        break;
    }
    return 0;
}
