#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "mode.h"
#include "menu.h"
#include "room.h"
#include "effect.h"
#include "save.h"

/* menu_panel_tasks.c (0x0800E314-0x0800EA0B, issue #99).
 *
 * Menu sprite tasks, middle part.  Task types #245 (Task_NormalExtraPanel, body
 * NormalExtraPanelUpdate) and #246 (Task_PlayerCountPanel, body PlayerCountPanelUpdate) show the
 * pictures and palette pulses of menu screens 2 and 3; #247
 * (Task_ModeListCursor, body ModeListCursorUpdate) the mode list's picture and row
 * highlight (ModeListHighlightRow); #248 (Task_ModePlayerCountPanel, body ModePlayerCountPanelUpdate) the
 * picture of screen 5; and Task_EraseConfirmDialog is the entry of #243, whose
 * body EraseConfirmDialogUpdate is in menu_sound_test_tasks.c. */

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);

void Task_NormalExtraPanel(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)NormalExtraPanelUpdate;
    gCurTask->layer = 6;
    gCurTask->frameTable = gUnk_08755650;
    gCurTask->frame = 7;
    gCurTask->normalExtraPanelBlendFrom = 0;
    gCurTask->normalExtraPanelBlendTo = 1;
    gCurTask->normalExtraPanelBlendRatio = 0;
    gCurTask->posX = 0xA00000;
    gCurTask->posY = 0x300000;
    while (gMenuScreen == 2 || gMenuScreen == 3)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void NormalExtraPanelUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->normalExtraPanelBlendRatio == 256) {
        if (++t->normalExtraPanelBlendFrom > 1)
            t->normalExtraPanelBlendFrom = 0;
        u = gCurTask;
        if (++u->normalExtraPanelBlendTo > 1)
            u->normalExtraPanelBlendTo = 0;
        gCurTask->normalExtraPanelBlendRatio = 0;
    }
    v = gCurTask;
    if ((v->normalExtraPanelBlendRatio += 32) > 256)
        v->normalExtraPanelBlendRatio = 256;
    if (gMenuScreen == 2) {
        k = gMenuChoiceCursor * 3;
        w = gCurTask;
        BlendColors(gUnk_08559C24[k + w->normalExtraPanelBlendFrom], gUnk_08559C24[k + w->normalExtraPanelBlendTo], (u16)w->normalExtraPanelBlendRatio, 16, gObjPaletteBank8);
        MenuLoadPicture(1, gMenuChoiceCursor);
    } else {
        BlendColors((u16 *)gUnk_08559C24 + (gMenuChoiceCursor * 3 + 2) * 16, (u16 *)gUnk_08559C24 + (gMenuChoiceCursor * 3 + 2) * 16, (u16)gCurTask->normalExtraPanelBlendRatio, 16, gObjPaletteBank8);
    }
}

void Task_PlayerCountPanel(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)PlayerCountPanelUpdate;
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_08755650;
    gCurTask->frame = 6;
    gCurTask->playerCountPanelBlendFrom = 0;
    gCurTask->playerCountPanelBlendTo = 1;
    gCurTask->playerCountPanelBlendRatio = 0;
    if (gSaveSlots[gCurSaveSlot].milestoneFlags & 4) {
        gCurTask->posX = 0xB00000;
        gCurTask->posY = (gMenuChoiceCursor << 20) + 0x280000;
    } else {
        gCurTask->posX = 0xA80000;
        gCurTask->posY = 0x300000;
    }
    while (gMenuScreen == 3)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void PlayerCountPanelUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->playerCountPanelBlendRatio == 256) {
        if (++t->playerCountPanelBlendFrom > 1)
            t->playerCountPanelBlendFrom = 0;
        u = gCurTask;
        if (++u->playerCountPanelBlendTo > 1)
            u->playerCountPanelBlendTo = 0;
        gCurTask->playerCountPanelBlendRatio = 0;
    }
    v = gCurTask;
    if ((v->playerCountPanelBlendRatio += 32) > 256)
        v->playerCountPanelBlendRatio = 256;
    k = gPlayerCountCursor * 3;
    w = gCurTask;
    BlendColors(gUnk_08559C24[k + w->playerCountPanelBlendFrom], gUnk_08559C24[k + w->playerCountPanelBlendTo], (u16)w->playerCountPanelBlendRatio, 16, gObjPaletteBank7);
    MenuLoadPicture(2, gPlayerCountCursor);
}

void Task_ModeListCursor(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)ModeListCursorUpdate;
    gCurTask->frameTable = gUnk_08755650;
    LoadGfxSet(32);
    ModeListHighlightRow();
    if (gMenuScreen == 4)
        MenuLoadPicture(3, gMenuCursor);
    gCurTask->modeListCursorSavedCursor = gMenuCursor;
    gCurTask->modeListCursorBlendFrom = 0;
    gCurTask->modeListCursorBlendTo = 1;
    gCurTask->modeListCursorBlendRatio = 0;
    gCurTask->posX = 0x680000;
    while (1) {
        gCurTask->posY = (gModeListCursorBaseY[gModeListExtraRows] + gModeListCursorRowStep[gModeListExtraRows] * gMenuCursor) << 16;
        if (gMenuScreen == 1 || gMenuScreen == 8)
            break;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void ModeListCursorUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    u32 *tbl;

    if (t->modeListCursorBlendRatio == 256) {
        if (++t->modeListCursorBlendFrom > 1)
            t->modeListCursorBlendFrom = 0;
        u = gCurTask;
        if (++u->modeListCursorBlendTo > 1)
            u->modeListCursorBlendTo = 0;
        gCurTask->modeListCursorBlendRatio = 0;
    }
    v = gCurTask;
    if ((v->modeListCursorBlendRatio += 32) > 256)
        v->modeListCursorBlendRatio = 256;
    if (gMenuScreen == 4) {
        w = gCurTask;
        BlendColors(gUnk_0855D2F8[w->modeListCursorBlendFrom], gUnk_0855D2F8[w->modeListCursorBlendTo], (u16)w->modeListCursorBlendRatio, 10, gObjPaletteBank6Color6);
        if (gMenuCursor != gCurTask->modeListCursorSavedCursor) {
            ModeListHighlightRow();
            if (gMenuScreen == 4)
                MenuLoadPicture(3, gMenuCursor);
            gCurTask->modeListCursorSavedCursor = gMenuCursor;
        }
    } else {
        BlendColors(gUnk_0855D320, gUnk_0855D320, (u16)gCurTask->modeListCursorBlendRatio, 10, gObjPaletteBank6Color6);
        gCurTask->modeListCursorSavedCursor = -1;
    }
    if (TaskIsOnScreenNoCamera()) {
        tbl = gUnk_08755650;
        QueueSprite(6, tbl[13], 0, 0, gCurTask->pixelX, gCurTask->pixelY);
        QueueSprite(10, tbl[12], 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    }
}

void ModeListHighlightRow(void)
{
    s32 i;
    u16 *p;

    for (i = 0; i < gModeListExtraRows + 3; i++) {
        p = gBgPaletteBank4[0];
        if (i == gMenuCursor)
            RequestCopy(2, (u32)gUnk_08559CE6, (u32)&p[i * 3 + 1], 6);
        else
            RequestCopy(2, (u32)gUnk_08559CEC, (u32)&p[i * 3 + 1], 6);
    }
}

void Task_ModePlayerCountPanel(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)ModePlayerCountPanelUpdate;
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_08755650;
    gCurTask->frame = 6;
    gCurTask->modePlayerCountPanelBlendFrom = 0;
    gCurTask->modePlayerCountPanelBlendTo = 1;
    gCurTask->modePlayerCountPanelBlendRatio = 0;
    gCurTask->posX = 0xA80000;
    gCurTask->posY = (gModeListCursorBaseY[gModeListExtraRows] + gModeListCursorRowStep[gModeListExtraRows] * gMenuCursor) << 16;
    while (gMenuScreen == 5)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void ModePlayerCountPanelUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->modePlayerCountPanelBlendRatio == 256) {
        if (++t->modePlayerCountPanelBlendFrom > 1)
            t->modePlayerCountPanelBlendFrom = 0;
        u = gCurTask;
        if (++u->modePlayerCountPanelBlendTo > 1)
            u->modePlayerCountPanelBlendTo = 0;
        gCurTask->modePlayerCountPanelBlendRatio = 0;
    }
    v = gCurTask;
    if ((v->modePlayerCountPanelBlendRatio += 32) > 256)
        v->modePlayerCountPanelBlendRatio = 256;
    if (gMenuScreen == 5) {
        k = gMenuChoiceCursor * 2;
        w = gCurTask;
        BlendColors(gModePlayerCountPanelUpdatePalette[k + w->modePlayerCountPanelBlendFrom], gModePlayerCountPanelUpdatePalette[k + w->modePlayerCountPanelBlendTo], (u16)w->modePlayerCountPanelBlendRatio, 16, gObjPaletteBank7);
        MenuLoadPicture(4, gMenuCursor * 2 + gMenuChoiceCursor);
    } else {
        BlendColors((u16 *)gModePlayerCountPanelUpdatePalette + (gMenuChoiceCursor + 4) * 16, (u16 *)gModePlayerCountPanelUpdatePalette + (gMenuChoiceCursor + 4) * 16, (u16)gCurTask->modePlayerCountPanelBlendRatio, 16, gObjPaletteBank7);
    }
}

void Task_EraseConfirmDialog(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)EraseConfirmDialogUpdate;
    gCurTask->frameTable = gUnk_08755650;
    gCurTask->eraseConfirmDialogBlendFrom = 0;
    gCurTask->eraseConfirmDialogBlendTo = 1;
    gCurTask->eraseConfirmDialogBlendRatio = 0;
    gCurTask->posX = 0x800000;
    gCurTask->posY = 0x680000;
    while (gMenuScreen == 6 && gEraseConfirmCount != 2)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
