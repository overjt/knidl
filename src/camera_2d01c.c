#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_2d01c.c (0x0802D01C-0x0802D38B, issue #86).
 *
 * Camera mode switches, the room's BG animation scripts and the camera
 * task spawners.  gCameraMode is the camera mode: CameraLeaveScrollLock leaves
 * mode 3 for 4 or 0 from the flags in gScrollLock.unk1, CameraStartHoldAnchor
 * snaps the 16.16 camera target gCameraCenterX/gCameraCenterY to the player
 * and picks mode 4 or 5, and sub_0802d0c4 is a dead export (both arms of
 * its test store 0).  LoadRoomBgAnims resets the ten BG animation slots
 * gBgAnims[] and loads the room's scripts from
 * gRoomBgAnimScripts[gCurRoomDef->unk40]; UpdateBgAnims runs them every frame,
 * eight-byte commands: 0 copies tiles to 0x06004000 (BgAnimCopyTiles), 1
 * starts a palette fade (BgAnimStartPaletteFade, stepped by BgAnimStepPaletteFade into the
 * palette buffer gBgPaletteBank2), 2 waits, 3 loops, 5 sets a metatile's
 * solid flag (SetCollisionTile), 6 plays a sound effect, anything else stops
 * the slot (BgAnimStop).  CreateMapEvent spawns task type #4 through M07's
 * TaskCreateHighSlot; Task_MapEvent, the type's body, dispatches on Task.state
 * into the seven camera tasks of the anchor table gMapEventVariants. */

struct Unk03005680
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 lockedAxes;
    /*0x02*/ u16 x0;
    /*0x04*/ u16 x1;
    /*0x06*/ u16 y0;
    /*0x08*/ u16 y1;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
};

/* One of the ten BG animation slots at 0x02007D70 (28 bytes): a script of
   8-byte commands (unk4, pc unk0, wait timer unk2) plus a palette fade
   (unk8 step, read signed, 0xFFFF = idle; unkC/unk10 the two palettes, unk14 the colour
   index into the BG palette buffer, unk16 the count, unk18 the rate). */
struct Unk02007D70Cmd
{
    /*0x00*/ u16 op;
    /*0x02*/ u16 arg;
    /*0x04*/ void *ptr;
};

struct Unk02007D70
{
    /*0x00*/ u16 cmdIndex;
    /*0x02*/ s16 waitFrames;
    /*0x04*/ struct Unk02007D70Cmd *script;
    /*0x08*/ u16 fadeFrame;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 *fadeSrc;
    /*0x10*/ u16 *fadeDst;
    /*0x14*/ u16 fadeColorIndex;
    /*0x16*/ u16 fadeColorCount;
    /*0x18*/ u32 fadeRate;
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 width;
    /*0x04*/ u16 height;
    /*0x06*/ u16 unk6[0];
};

/* The room header gCurRoomDef points at (one entry of the gRoomTable
   room table): unk18/unk28 are length-prefixed palettes, unk30 the BG map
   streamed into 0x06003000, unk40 the room's BG animation script set. */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x18];
    /*0x18*/ u16 *bg2Palette;
    /*0x1C*/ u8 filler1C[0xC];
    /*0x28*/ u16 *bg3Palette;
    /*0x2C*/ u8 filler2C[4];
    /*0x30*/ struct BgMap *bg3Map;
    /*0x34*/ u8 filler34[0xC];
    /*0x40*/ u16 bgAnimSet;
};

struct Unk0802D25C
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4[0];
};

struct Unk0802D278
{
    /*0x00*/ u16 *src;
    /*0x04*/ u16 *dst;
    /*0x08*/ u16 colorIndex;
    /*0x0A*/ u16 colorCount;
    /*0x0C*/ u32 rate;
};

struct MapTile
{
    /*0x00*/ u16 metatile;
    /*0x02*/ u8 slopeIndex;
    /*0x03*/ u8 collisionTile;
};

/* Not from room.h: this file's view of gRoomMap differs (lesson 3.517). */
extern u16 gCameraMode;
extern struct Unk03005680 gScrollLock;
extern s32 gScrollLockSpeedX;
extern s32 gScrollLockSpeedY;
extern u8 gActivePlayerMask;
extern s32 gCameraCenterX;
extern s16 gCameraAnchorX;
extern s32 gCameraCenterY;
extern s16 gCameraAnchorY;
extern s8 gInHub;
extern struct RoomDef *gCurRoomDef;
extern struct Unk02007D70 gBgAnims[];
extern struct Unk02007D70Cmd **gRoomBgAnimScripts[];
extern u16 gBgPaletteBank2[];
extern s16 gRoomHeight;
extern s16 gRoomWidth;
extern struct MapTile *gRoomMap;
extern void (*gMapEventVariants[])(void);

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void PlaySfx(u32 a);
s32 TaskCreateHighSlot(s32 type);
void CalcRoomBounds(void);
void CameraResetBounds(void);
void BgAnimCopyTiles(struct Unk0802D25C *a);
void BgAnimStartPaletteFade(struct Unk02007D70 *p, struct Unk0802D278 *q);
void BgAnimStepPaletteFade(struct Unk02007D70 *p);
void SetCollisionTile(u32 x, u32 y, u32 v);
void BgAnimStop(struct Unk02007D70 *p);

void CameraLeaveScrollLock(void)
{
    if (gCameraMode == 3)
    {
        if (gScrollLock.lockedAxes & 1)
            gScrollLockSpeedX = 6;
        if (gScrollLock.lockedAxes & 2)
            gScrollLockSpeedY = 3;
        if (gScrollLock.lockedAxes != 0)
            gCameraMode = 4;
        else
            gCameraMode = 0;
        gScrollLock.unk0 = gActivePlayerMask;
    }
}

void CameraStartHoldAnchor(void)
{
    gCameraCenterX = gCameraAnchorX << 16;
    gCameraCenterY = gCameraAnchorY << 16;
    if (gInHub != 0)
        gCameraMode = 4;
    else
        gCameraMode = 5;
}

void sub_0802d0c4(void)
{
    CalcRoomBounds();
    CameraResetBounds();
    /* both arms store 0 in the ROM too (CameraStartHoldAnchor stores 4 / 5) */
    if (gInHub != 0)
        gCameraMode = 0;
    else
        gCameraMode = 0;
}

void LoadRoomBgAnims(void)
{
    s32 i;
    s32 k;

    i = 0;
    for (k = 0; k < 10; k++)
    {
        gBgAnims[k].cmdIndex = 0x7FFF;
        gBgAnims[k].fadeFrame |= 0xFFFF;
    }
    if (gCurRoomDef->bgAnimSet != 0)
    {
        while (gRoomBgAnimScripts[gCurRoomDef->bgAnimSet][i] != 0)
        {
            struct Unk02007D70 *p = &gBgAnims[i];
            p->script = gRoomBgAnimScripts[gCurRoomDef->bgAnimSet][i];
            p->cmdIndex = 0;
            p->waitFrames = 0;
            i++;
        }
    }
}

/* The do { } while (0) around the loop body is load-bearing: its loop
   notes weight every reference inside by one more loop level, which lifts
   the slot pointer above the command pointer in global-alloc priority
   (p = r4, cmd = r5 as in the ROM).  The command loop itself is a goto
   loop, as a real one gets rotated. */
void UpdateBgAnims(void)
{
    s32 i;
    struct Unk02007D70 *p;
    struct Unk02007D70Cmd *cmd;

    for (i = 0; i < 10; i++)
    {
        do
        {
            p = &gBgAnims[i];
            if (p->cmdIndex == 0x7FFF)
                continue;
        loop:
            if ((s16)p->fadeFrame != -1)
                BgAnimStepPaletteFade(p);
            if (--p->waitFrames > 0)
                continue;
            cmd = &p->script[p->cmdIndex];
            switch (cmd->op)
            {
            case 0:
                BgAnimCopyTiles(cmd->ptr);
                p->waitFrames = cmd->arg + 1;
                p->cmdIndex++;
                goto loop;
            case 1:
                BgAnimStartPaletteFade(p, cmd->ptr);
                p->waitFrames = cmd->arg + 1;
                p->cmdIndex++;
                goto loop;
            case 2:
                p->waitFrames = cmd->arg + 1;
                p->cmdIndex++;
                goto loop;
            case 3:
                p->cmdIndex = 0;
                goto loop;
            case 5:
                SetCollisionTile(cmd->arg >> 8, cmd->arg & 0xFF, (u16)(u32)cmd->ptr);
                p->cmdIndex++;
                goto loop;
            case 6:
                PlaySfx(cmd->arg);
                p->cmdIndex++;
                goto loop;
            default:
                BgAnimStop(p);
                break;
            }
        } while (0);
    }
}

void BgAnimCopyTiles(struct Unk0802D25C *a)
{
    RequestCopy(1, (u32)a->unk4, (BG_VRAM + 0x4000) + a->unk0 * 32, a->unk2);
}

void BgAnimStartPaletteFade(struct Unk02007D70 *p, struct Unk0802D278 *q)
{
    p->fadeSrc = q->src;
    p->fadeDst = q->dst;
    p->fadeColorIndex = q->colorIndex;
    p->fadeColorCount = q->colorCount;
    p->fadeRate = q->rate;
    p->fadeFrame = 0;
}

void BgAnimStepPaletteFade(struct Unk02007D70 *p)
{
    s32 t;

    p->fadeFrame++;
    t = (p->fadeRate * (s16)p->fadeFrame) >> 8;
    if (t > 0x100)
        t = 0x100;
    BlendColors(p->fadeSrc, p->fadeDst, (u16)t, p->fadeColorCount, &gBgPaletteBank2[p->fadeColorIndex]);
    if (t == 0x100)
        p->fadeFrame = -1;
}

void SetCollisionTile(u32 x, u32 y, u32 v)
{
    if (x < gRoomWidth && y < gRoomHeight)
        gRoomMap[y * gRoomWidth + x].collisionTile = v;
}

void BgAnimStop(struct Unk02007D70 *p)
{
    p->script = 0;
    p->cmdIndex = 0x7FFF;
    p->waitFrames = 0;
    p->fadeFrame = -1;
}

s32 CreateMapEvent(s32 a)
{
    s32 id;
    struct Task *t;

    id = TaskCreateHighSlot(TASK_MAP_EVENT);
    if (id != -1)
    {
        t = &gTasks[id];
        t->state = a;
    }
    return id;
}

void Task_MapEvent(void)
{
    CallTableEntry(gCurTask->state, 7, gMapEventVariants);
}
