#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_082d0.c (0x080082D0-0x08008663, issue #96).
 *
 * Task type #265, the two decorations of the state-13 title screen:
 * sub_080082d0 spawns them, sub_08008348 is the body (it dispatches one of
 * two scripts from the anchor table at 0x0873078C through CallTableEntry),
 * and sub_080083b0/sub_08008460/sub_080084dc/sub_08008558 are the four
 * script bodies (a sprite pair, a BG scroll plus palette cycle, a sprite
 * loop and a palette pulse). */

extern u8 gUnk_02006090;
extern s8 gSubGameLevel;
extern vs32 gBg0ScrollX;
extern vu16 gBgPalette[];
extern u16 gObjPalette[];
extern u16 gUnk_03001490[];
extern u16 gFrameCount;
extern u16 gPrevGameState;
extern u16 gLocalPlayer;
extern s32 gUnk_03005280;
extern u8 gUnk_085B6A90[];
extern u8 gUnk_085B6AC0[];
extern u8 gUnk_085B6AC8[];
extern u16 gUnk_085B6E78[][3][16];
extern u16 gUnk_085B6F98[];
extern void (*gUnk_0873078C[])(void);
extern void (*gUnk_08730794[])(void);
extern u8 gUnk_0873079C[];
extern u32 gUnk_08756054[];

void TaskYieldTrampoline(u32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u16 f);
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskSleepForever(void);
void sub_08008394(void);

void sub_080082d0(void)
{
    s32 idx;
    struct Task *t;

    if (gUnk_02006090 <= 2) {
        idx = TaskCreateFrom(0x109, 32);
        if (idx != -1) {
            t = &gTasks[idx];
            t->unk44 = gCurTaskIdx;
            t->unk73 = 0;
        }
        idx = TaskCreateFrom(0x109, 32);
        if (idx != -1) {
            t = &gTasks[idx];
            t->unk44 = gCurTaskIdx;
            t->unk73 = 1;
        }
    }
}

void sub_08008348(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_08008394;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    if (t->unk73 == 0)
        t->unk14 = 0;
    else
        t->unk14 = 1;
    CallTableEntry(gCurTask->unk14, 2, gUnk_0873078C);
    TaskSleepForever();
}

void sub_08008394(void)
{
    CallTableEntry(gCurTask->unk15, 2, gUnk_08730794);
}

void sub_080083b0(void)
{
    gCurTask->unk15 = 0;
    gCurTask->unk2C = 0;
    for (;;) {
        if (gUnk_03005280 == 0) {
            QueueSprite(8, (u32)gUnk_085B6AC0, 0, 0, 120, 88);
        } else if (gUnk_03005280 == 1) {
            QueueSprite(8, DrawAffineSprite((s32)gUnk_085B6AC0, 0x100, 0x100, (s16)((16 - (gFrameCount & 15)) << 4)), 0, 0, 120, 88);
        } else if (gUnk_03005280 == 2) {
            QueueSprite(8, (u32)gUnk_085B6AC8, 0, 0, 120, 88);
        } else {
            TaskSleepForever();
            return;
        }
        QueueSprite(9, (u32)gUnk_085B6A90, 0, 0, 120, 88);
        TaskYieldTrampoline(1);
    }
}

void sub_08008460(void)
{
    if (gUnk_03005280 == 1) {
        gBg0ScrollX += 0x20000;
        if (gBg0ScrollX > 0x2000000)
            gBg0ScrollX -= 0x2000000;
    }
    if (gUnk_03005280 == 2) {
        BlendColors(gUnk_085B6F98, gUnk_085B6F98 + 16, gUnk_0873079C[gCurTask->unk2C], 16, gObjPalette);
        gCurTask->unk2C = (gCurTask->unk2C + 1) & 15;
    }
}

void sub_080084dc(void)
{
    gCurTask->unk15 = 1;
    for (;;) {
        if (gPrevGameState == 4 && gLocalPlayer == 0)
            QueueSprite(8, gUnk_08756054[gSubGameLevel + 4], 0, 0, 120, 144);
        else
            QueueSprite(8, gUnk_08756054[gSubGameLevel + 7], 0, 0, 200, 144);
        TaskYieldTrampoline(1);
    }
}

/* The shift is written in BOTH arms of each triangle-wave `if`: jump2's
   cross-jumping merges the two identical `lsls` into the one at the join,
   and because at combine time the shift sits in a different block from the
   call's (u16) zero-extension, it is not folded into `lsls #22`.  The
   `r -= 4; r = 4 - r` pair must stay two statements (a single expression
   folds to `8 - r`; split, CSE reuses the register known to hold 4). */
void sub_08008558(void)
{
    u32 r;
    u32 i;

    if (gUnk_03005280 == 4) {
        r = gFrameCount & 7;
        if (r > 3) {
            r -= 4;
            r = 4 - r;
            r <<= 6;
        } else {
            r <<= 6;
        }
        BlendColors(gUnk_085B6E78[gSubGameLevel][0], gUnk_085B6E78[gSubGameLevel][2], (u16)r, 16, gUnk_03001490);
    } else if (gUnk_03005280 == 3) {
        r = gFrameCount & 15;
        if (r > 7) {
            r = 16 - r;
            r <<= 5;
        } else {
            r <<= 5;
        }
        BlendColors(gUnk_085B6E78[gSubGameLevel][0], gUnk_085B6E78[gSubGameLevel][1], (u16)r, 16, gUnk_03001490);
    } else {
        for (i = 0; i < 16; i++)
            gBgPalette[0x110 + i] = gUnk_085B6E78[0][0][i];
        gBgPalette[0x111] = gUnk_085B6E78[1][0][1];
        gBgPalette[0x112] = gUnk_085B6E78[1][0][2];
        gBgPalette[0x11D] = gUnk_085B6E78[0][0][12];
        gBgPalette[0x119] = gUnk_085B6E78[0][0][7];
        gBgPalette[0x11B] = gUnk_085B6E78[0][0][8];
    }
}
