#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "mode.h"
#include "menu.h"
#include "room.h"
#include "effect.h"
#include "save.h"

/* menutask_0e314.c (0x0800E314-0x0800EA0B, issue #99).
 *
 * Menu sprite tasks, middle part.  Task types #245 (Task_NormalExtraPanel, body
 * NormalExtraPanelUpdate) and #246 (Task_PlayerCountPanel, body PlayerCountPanelUpdate) show the
 * pictures and palette pulses of menu screens 2 and 3; #247
 * (Task_ModeListCursor, body sub_0800e674) the mode list's picture and row
 * highlight (sub_0800e7b0); #248 (Task_ModePlayerCountPanel, body ModePlayerCountPanelUpdate) the
 * picture of screen 5; and Task_EraseConfirmDialog is the entry of #243, whose
 * body EraseConfirmDialogUpdate is in menutask_0ea0c.c. */

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
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
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

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gMenuScreen == 2) {
        k = gMenuChoiceCursor * 3;
        w = gCurTask;
        BlendColors(gUnk_08559C24[k + w->unk2C], gUnk_08559C24[k + w->unk30], (u16)w->unk34, 16, gUnk_03001570);
        MenuLoadPicture(1, gMenuChoiceCursor);
    } else {
        BlendColors((u16 *)gUnk_08559C24 + (gMenuChoiceCursor * 3 + 2) * 16, (u16 *)gUnk_08559C24 + (gMenuChoiceCursor * 3 + 2) * 16, (u16)gCurTask->unk34, 16, gUnk_03001570);
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
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
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

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    k = gPlayerCountCursor * 3;
    w = gCurTask;
    BlendColors(gUnk_08559C24[k + w->unk2C], gUnk_08559C24[k + w->unk30], (u16)w->unk34, 16, gUnk_03001550);
    MenuLoadPicture(2, gPlayerCountCursor);
}

void Task_ModeListCursor(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)sub_0800e674;
    gCurTask->frameTable = gUnk_08755650;
    LoadGfxSet(32);
    sub_0800e7b0();
    if (gMenuScreen == 4)
        MenuLoadPicture(3, gMenuCursor);
    gCurTask->unk28 = gMenuCursor;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->posX = 0x680000;
    while (1) {
        gCurTask->posY = (gUnk_08731E4C[gModeListExtraRows] + gUnk_08731E52[gModeListExtraRows] * gMenuCursor) << 16;
        if (gMenuScreen == 1 || gMenuScreen == 8)
            break;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800e674(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    u32 *tbl;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gMenuScreen == 4) {
        w = gCurTask;
        BlendColors(gUnk_0855D2F8[w->unk2C], gUnk_0855D2F8[w->unk30], (u16)w->unk34, 10, gUnk_0300153C);
        if (gMenuCursor != gCurTask->unk28) {
            sub_0800e7b0();
            if (gMenuScreen == 4)
                MenuLoadPicture(3, gMenuCursor);
            gCurTask->unk28 = gMenuCursor;
        }
    } else {
        BlendColors(gUnk_0855D320, gUnk_0855D320, (u16)gCurTask->unk34, 10, gUnk_0300153C);
        gCurTask->unk28 = -1;
    }
    if (TaskIsOnScreenNoCamera()) {
        tbl = gUnk_08755650;
        QueueSprite(6, tbl[13], 0, 0, gCurTask->pixelX, gCurTask->pixelY);
        QueueSprite(10, tbl[12], 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    }
}

void sub_0800e7b0(void)
{
    s32 i;
    u16 *p;

    for (i = 0; i < gModeListExtraRows + 3; i++) {
        p = gUnk_030012F0[0];
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
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->posX = 0xA80000;
    gCurTask->posY = (gUnk_08731E4C[gModeListExtraRows] + gUnk_08731E52[gModeListExtraRows] * gMenuCursor) << 16;
    while (gMenuScreen == 5)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void ModePlayerCountPanelUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gMenuScreen == 5) {
        k = gMenuChoiceCursor * 2;
        w = gCurTask;
        BlendColors(gUnk_0855D334[k + w->unk2C], gUnk_0855D334[k + w->unk30], (u16)w->unk34, 16, gUnk_03001550);
        MenuLoadPicture(4, gMenuCursor * 2 + gMenuChoiceCursor);
    } else {
        BlendColors((u16 *)gUnk_0855D334 + (gMenuChoiceCursor + 4) * 16, (u16 *)gUnk_0855D334 + (gMenuChoiceCursor + 4) * 16, (u16)gCurTask->unk34, 16, gUnk_03001550);
    }
}

void Task_EraseConfirmDialog(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = 0;
    gCurTask->updateCallback = (u32)EraseConfirmDialogUpdate;
    gCurTask->frameTable = gUnk_08755650;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->posX = 0x800000;
    gCurTask->posY = 0x680000;
    while (gMenuScreen == 6 && gEraseConfirmCount != 2)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
