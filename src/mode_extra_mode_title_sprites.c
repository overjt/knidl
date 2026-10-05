#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "mode.h"
#include "room.h"
#include "player.h"

/* mode_extra_mode_title_sprites.c (0x080082D0-0x08008663, issue #96).
 *
 * Task type #265, the two decorations of the state-13 title screen:
 * CreateExtraModeTitleSprites spawns them, Task_ExtraModeTitleSprite is the body (it dispatches one of
 * two scripts from the anchor table at 0x0873078C through CallTableEntry),
 * and ExtraModeTitleTransferIcon/ExtraModeTitleTransferIconUpdate/ExtraModeTitleLevelBar/ExtraModeTitleLevelBarUpdate are the four
 * script bodies (a sprite pair, a BG scroll plus palette cycle, a sprite
 * loop and a palette pulse). */

/* Not from main.h: this file's view of gBgPalette differs (lesson 3.517). */
extern vs32 gBg0ScrollX;
extern vu16 gBgPalette[];
extern u16 gObjPalette[];
extern vu16 gFrameCount;

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u16 f);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSleepForever(void);

void CreateExtraModeTitleSprites(void)
{
    s32 titleSpriteSlot;
    struct Task *titleSprite;

    if (gExtraModeTitleIndex <= 2) {
        titleSpriteSlot = TaskCreateFrom(TASK_EXTRA_MODE_TITLE_SPRITE, 32);
        if (titleSpriteSlot != -1) {
            titleSprite = &gTasks[titleSpriteSlot];
            titleSprite->parent = gCurTaskIdx;
            titleSprite->variant = 0;
        }
        titleSpriteSlot = TaskCreateFrom(TASK_EXTRA_MODE_TITLE_SPRITE, 32);
        if (titleSpriteSlot != -1) {
            titleSprite = &gTasks[titleSpriteSlot];
            titleSprite->parent = gCurTaskIdx;
            titleSprite->variant = 1;
        }
    }
}

void Task_ExtraModeTitleSprite(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)ExtraModeTitleSpriteUpdate;
    t->extraModeTitleSpriteBlendStep = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    if (t->variant == 0)
        t->state = 0;
    else
        t->state = 1;
    CallTableEntry(gCurTask->state, 2, gExtraModeTitleSpriteStates);
    TaskSleepForever();
}

void ExtraModeTitleSpriteUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 2, gExtraModeTitleSpriteStateUpdates);
}

void ExtraModeTitleTransferIcon(void)
{
    gCurTask->updateState = 0;
    gCurTask->extraModeTitleSpriteBlendStep = 0;
    for (;;) {
        if (gExtraModeTitlePhase == 0) {
            QueueSprite(8, (u32)gUnk_085B6AC0, 0, 0, 120, 88);
        } else if (gExtraModeTitlePhase == 1) {
            QueueSprite(8, DrawAffineSprite((s32)gUnk_085B6AC0, 0x100, 0x100, (s16)((16 - (gFrameCount & 15)) << 4)), 0, 0, 120, 88);
        } else if (gExtraModeTitlePhase == 2) {
            QueueSprite(8, (u32)gUnk_085B6AC8, 0, 0, 120, 88);
        } else {
            TaskSleepForever();
            return;
        }
        QueueSprite(9, (u32)gUnk_085B6A90, 0, 0, 120, 88);
        TaskYieldTrampoline(1);
    }
}

void ExtraModeTitleTransferIconUpdate(void)
{
    if (gExtraModeTitlePhase == 1) {
        gBg0ScrollX += 0x20000;
        /* raw: not an address, 512 px in 16.16 fixed point (the scroll wraps) */
        if (gBg0ScrollX > 0x2000000)
            gBg0ScrollX -= 0x2000000; /* raw: 512 px in 16.16, not an address */
    }
    if (gExtraModeTitlePhase == 2) {
        BlendColors(gUnk_085B6F98, gUnk_085B6F98 + 16, gUnk_0873079C[gCurTask->extraModeTitleSpriteBlendStep], 16, gObjPalette);
        gCurTask->extraModeTitleSpriteBlendStep = (gCurTask->extraModeTitleSpriteBlendStep + 1) & 15;
    }
}

void ExtraModeTitleLevelBar(void)
{
    gCurTask->updateState = 1;
    for (;;) {
        if (gPrevGameState == GAME_STATE_MAIN_MENU && gLocalPlayer == 0)
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
void ExtraModeTitleLevelBarUpdate(void)
{
    u32 r;
    u32 i;

    if (gExtraModeTitlePhase == 4) {
        r = gFrameCount & 7;
        if (r > 3) {
            r -= 4;
            r = 4 - r;
            r <<= 6;
        } else {
            r <<= 6;
        }
        BlendColors(gUnk_085B6E78[gSubGameLevel][0], gUnk_085B6E78[gSubGameLevel][2], (u16)r, 16, gObjPaletteBank1);
    } else if (gExtraModeTitlePhase == 3) {
        r = gFrameCount & 15;
        if (r > 7) {
            r = 16 - r;
            r <<= 5;
        } else {
            r <<= 5;
        }
        BlendColors(gUnk_085B6E78[gSubGameLevel][0], gUnk_085B6E78[gSubGameLevel][1], (u16)r, 16, gObjPaletteBank1);
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
