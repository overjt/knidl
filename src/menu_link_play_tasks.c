#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "mode.h"
#include "menu.h"
#include "room.h"

/* menu_link_play_tasks.c (0x0800F180-0x0800FCBB, issue #99).
 *
 * Menu sprite tasks, last part: the link-play connection screen (menu
 * screens 8 and 9) and the menu's background tasks.  Task type #251
 * (Task_LinkPlayPlayerList, body LinkPlayPlayerListUpdate) spawns a #252 (Task_LinkPlayConsole, body
 * LinkPlayConsoleUpdate) and a #253 (Task_LinkPlayCable, body LinkPlayCableUpdate) per player
 * and slides them as partners join (sub_0800ffd8); #256 (Task_MenuScreenTitle,
 * body MenuScreenTitleUpdate) is the menu screen's title sprite; #259
 * (Task_MenuBgPaletteCycle) and #258 (Task_MenuBackground) cycle and cross-fade the
 * background palettes when the menu screen changes. */

/* Plain u8 here (vu8 elsewhere): a volatile byte load expands to a load plus
   two shifts, which lengthens this address's live range in LinkPlayConsoleUpdate enough
   to lose r6 to the hoisted copy of &gCurTask. */
/* Not from link.h: this file's view of gMultiBootStruct differs (lesson
   3.517). */
extern u8 gMultiBootStruct[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);

void Task_LinkPlayPlayerList(void)
{
    struct Task *t;
    struct Task *s;
    struct Task *u;
    struct Task *x;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)LinkPlayPlayerListUpdate;
    gCurTask->layer = 7;
    t = gCurTask;
    t->frameTable = gLinkPlayFrames;
    t->posX = 0xC40000;
    t->posY = 0x340000;
    t->linkPlayPlayerListLoopCount = 0;
    do {
        gCurTask->linkPlayPlayerListChildSlot = TaskCreateFrom(TASK_LINK_PLAY_CONSOLE, 32);
        u = gCurTask;
        x = &gTasks[u->linkPlayPlayerListChildSlot];
        x->parent = gCurTaskIdx;
        x->unk18 = (s16)u->linkPlayPlayerListLoopCount;
        gCurTask->linkPlayPlayerListChildSlot = TaskCreateFrom(TASK_LINK_PLAY_CABLE, 32);
        u = gCurTask;
        x = &gTasks[u->linkPlayPlayerListChildSlot];
        x->parent = gCurTaskIdx;
        x->unk18 = (s16)u->linkPlayPlayerListLoopCount;
    } while ((s16)++u->linkPlayPlayerListLoopCount <= 3);
    s = gCurTask;
    s->linkPlayPlayerListBlendFrom = 0;
    s->linkPlayPlayerListBlendTo = 1;
    s->linkPlayPlayerListBlendRatio = 0;
    s->linkPlayPlayerListStepDir = 0;
    s->linkPlayPlayerListShownCount = 0;
    s->linkPlayPlayerListStepTimer = 0;
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        s32 n = sub_0800ffd8();
        struct Task *v;

        u = gCurTask;
        u->linkPlayPlayerListLinkedCount = n;
        if (u->linkPlayPlayerListStepTimer == 0 || --u->linkPlayPlayerListStepTimer == 0) {
            v = gCurTask;
            if (v->linkPlayPlayerListShownCount < v->linkPlayPlayerListLinkedCount) {
                v->linkPlayPlayerListStepDir = 1;
                v->linkPlayPlayerListShownCount = v->linkPlayPlayerListShownCount + 1;
                v->linkPlayPlayerListStepTimer = 6;
            } else if (v->linkPlayPlayerListShownCount > v->linkPlayPlayerListLinkedCount) {
                v->linkPlayPlayerListStepDir = -1;
                v->linkPlayPlayerListShownCount = v->linkPlayPlayerListShownCount - 1;
                v->linkPlayPlayerListStepTimer = 6;
            } else {
                v->linkPlayPlayerListStepDir = 0;
            }
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void LinkPlayPlayerListUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 n;

    t = gCurTask;
    if (t->linkPlayPlayerListBlendRatio == 256) {
        if (++t->linkPlayPlayerListBlendFrom > 3)
            t->linkPlayPlayerListBlendFrom = 0;
        u = gCurTask;
        if (++u->linkPlayPlayerListBlendTo > 3)
            u->linkPlayPlayerListBlendTo = 0;
        gCurTask->linkPlayPlayerListBlendRatio = 0;
    }
    v = gCurTask;
    n = v->linkPlayPlayerListBlendRatio + 16;
    v->linkPlayPlayerListBlendRatio = n;
    if (n > 256)
        v->linkPlayPlayerListBlendRatio = 256;
    w = gCurTask;
    BlendColors(gUnk_08562FE4[w->linkPlayPlayerListBlendFrom], gUnk_08562FE4[w->linkPlayPlayerListBlendTo], (u16)w->linkPlayPlayerListBlendRatio, 8, gObjPaletteBank13Color1);
    y = gCurTask;
    BlendColors(gUnk_08563024[y->linkPlayPlayerListBlendFrom], gUnk_08563024[y->linkPlayPlayerListBlendTo], (u16)y->linkPlayPlayerListBlendRatio, 13, &gObjPaletteBank13Color1[16]);
    x = gCurTask;
    if (x->linkPlayPlayerListStepTimer == 0 && x->linkPlayPlayerListLinkedCount != 0 && gMultiBootStruct[0] == 0 && gUnk_02007FC8 == 0)
        x->frame = 17;
    else
        gCurTask->frame = 0xFFFF;
}

void Task_LinkPlayConsole(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)LinkPlayConsoleUpdate;
    gCurTask->layer = gCurTask->linkPlayConsoleIndex * 2 + 9;
    gCurTask->frameTable = gLinkPlayFrames;
    gCurTask->posX = 0x780000;
    gCurTask->posY = 0x480000;
    gCurTask->linkPlayConsoleMarkerTimer = 0;
    while (gMenuScreen == 8 || gMenuScreen == 9)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void LinkPlayConsoleUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *p = &gTasks[t->parent];
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    u32 *tbl;
    s32 k;
    s32 g;

    if (p->linkPlayPlayerListStepDir != 0) {
        if (p->linkPlayPlayerListStepDir == 1) {
            t->linkPlayConsoleStepRow = p->linkPlayPlayerListShownCount;
            if (p->linkPlayPlayerListShownCount == t->linkPlayConsoleIndex && gMultiBootStruct[0] == 0)
                t->linkPlayConsoleMarkerTimer = 24;
        } else {
            t->linkPlayConsoleStepRow = p->linkPlayPlayerListShownCount + 1;
        }
        u = gCurTask;
        u->velX = gUnk_08731E58[u->linkPlayConsoleStepRow][u->linkPlayConsoleIndex] * p->linkPlayPlayerListStepDir;
        u->velY = gUnk_08731E98[u->linkPlayConsoleStepRow] * p->linkPlayPlayerListStepDir;
    } else {
        t->velX = 0;
        t->velY = 0;
    }
    if (gUnk_02007FC8 == 0) {
        v = gCurTask;
        v->frame = gUnk_08731EA8[p->linkPlayPlayerListShownCount];
        if (TaskIsOnScreenNoCamera()) {
            w = gCurTask;
            tbl = w->frameTable;
            QueueSprite(w->layer - 1, tbl[gUnk_08731EB0[p->linkPlayPlayerListShownCount]], 0, 0, w->pixelX, w->pixelY);
            if (gMultiBootStruct[0] == (gCurTask->linkPlayConsoleIndex & 0xFF) && sub_0800ffd8() != 0) {
                x = gCurTask;
                if (p->linkPlayPlayerListShownCount >= x->linkPlayConsoleIndex)
                    QueueSprite(x->layer - 1, tbl[gMultiBootStruct[0] + 13], 0, 0, x->pixelX, x->pixelY + 16);
            }
            y = gCurTask;
            if (y->linkPlayConsoleMarkerTimer != 0 && --y->linkPlayConsoleMarkerTimer <= 17) {
                if (y->linkPlayConsoleMarkerTimer <= 3) {
                    k = (4 - y->linkPlayConsoleMarkerTimer) * 64 + 256;
                    y->linkPlayConsoleMarkerScale = k;
                    g = DrawAffineSprite(tbl[18], k, k, 0);
                    z = gCurTask;
                    QueueSprite(z->layer - 1, g, 0, 0, z->pixelX, z->pixelY + 28 + z->linkPlayConsoleMarkerTimer);
                } else {
                    QueueSprite(y->layer - 1, tbl[18], 0, 0, y->pixelX, y->pixelY + 32);
                }
            }
        }
    } else {
        gCurTask->frame = 0xFFFF;
    }
}

void Task_LinkPlayCable(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)LinkPlayCableUpdate;
    gCurTask->layer = gCurTask->linkPlayCableIndex * 2 + 9;
    t = gCurTask;
    t->frameTable = gLinkPlayFrames;
    t->posX = gUnk_08731EB8[t->linkPlayCableIndex] << 16;
    t->posY = gUnk_08731EC0[t->linkPlayCableIndex] << 16;
    t->linkPlayCableBobTimer = 1;
    t->linkPlayCableBobDir = 1;
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        struct Task *u = gCurTask;

        if (u->linkPlayCableIndex == 0 && --u->linkPlayCableBobTimer == 0) {
            u->linkPlayCableBobTimer = 16;
            u->linkPlayCableBobDir = -u->linkPlayCableBobDir;
            u->velY = u->linkPlayCableBobDir << 16;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void LinkPlayCableUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *p = &gTasks[t->parent];
    struct Task *u;
    s16 a;
    s16 b;

    if (t->linkPlayCableIndex == 0) {
        if (p->linkPlayPlayerListShownCount == 0 && p->linkPlayPlayerListStepDir == 0)
            t->frame = 6;
        else
            gCurTask->frame = 0xFFFF;
    } else if (p->linkPlayPlayerListStepDir != 0) {
        if (p->linkPlayPlayerListStepDir == -1)
            t->linkPlayCableStepRow = p->linkPlayPlayerListShownCount + 1;
        else
            t->linkPlayCableStepRow = p->linkPlayPlayerListShownCount;
        u = gCurTask;
        u->velX = gUnk_08731EC8[u->linkPlayCableStepRow][u->linkPlayCableIndex] * p->linkPlayPlayerListStepDir;
        u->velY = gUnk_08731F08[u->linkPlayCableStepRow][u->linkPlayCableIndex] * p->linkPlayPlayerListStepDir;
        a = gUnk_08731F48[p->linkPlayPlayerListShownCount][u->linkPlayCableIndex];
        if (a == -1 || (b = gUnk_08731F48[p->linkPlayPlayerListShownCount - p->linkPlayPlayerListStepDir][u->linkPlayCableIndex]) == -1)
            u->frame = 0xFFFF;
        else if (p->linkPlayPlayerListStepTimer > 3)
            u->frame = b;
        else
            u->frame = a;
    } else {
        t->frame = gUnk_08731F48[p->linkPlayPlayerListShownCount][t->linkPlayCableIndex];
        t->velX = 0;
        t->velY = 0;
    }
    if (gUnk_02007FC8 == 1)
        gCurTask->frame = 0xFFFF;
}

void Task_MenuScreenTitle(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)MenuScreenTitleUpdate;
    gCurTask->frameTable = gUnk_08755620;
    gCurTask->layer = 7;
    t = gCurTask;
    t->menuScreenTitleScreen = gMenuScreen;
    LoadGfxSet(gMenuScreenTitleGfx[t->menuScreenTitleScreen]);
    if (gMenuScreen == 7) {
        t = gCurTask;
        t->posX = 0x600000;
        t->posY = 0x200000;
        t->frame = 11;
    } else {
        t = gCurTask;
        t->posX = 0x640000;
        t->posY = 0xE0000;
        t->frame = 10;
    }
    gCurTask->menuScreenTitlePhase = 3;
    TaskSleepForever();
}

void MenuScreenTitleUpdate(void)
{
    u32 *tbl = gMenuScreenTitleGfx;
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    s32 old = t->menuScreenTitleScreen;

    if (tbl[old] != tbl[gMenuScreen]) {
        if (gMenuScreen == 7 || old == 7) {
            t->menuScreenTitlePhase = 0;
            t->menuScreenTitlePhaseTimer = 8;
            if (old == 7) {
                t->posX = 0x600000;
                t->posY = 0x200000;
                t->frame = 11;
                t->velX = 0xFFE00000;
            } else {
                t->posX = 0x640000;
                t->posY = 0xE0000;
                t->frame = 10;
                t->velX = 0x200000;
            }
        } else {
            t->menuScreenTitlePhaseTimer = (old == 11) ? 1 : 7;
            u = gCurTask;
            u->posX = 0x640000;
            u->posY = 0xE0000;
            u->velX = 0;
            if (gMenuScreenTitleGfx[gMenuScreen])
                u->menuScreenTitlePhase = 2;
            else
                u->menuScreenTitlePhase = 3;
        }
        gCurTask->menuScreenTitleScreen = gMenuScreen;
    }
    v = gCurTask;
    switch (v->menuScreenTitlePhase) {
    case 0:
        if (--v->menuScreenTitlePhaseTimer == 0) {
            LoadGfxSet(gMenuScreenTitleGfx[v->menuScreenTitleScreen]);
            if (gMenuScreen == 7) {
                u = gCurTask;
                u->posX = 0xFFE00000;
                u->posY = 0x200000;
                u->frame = 11;
                u->velX = 0x100000;
            } else {
                u = gCurTask;
                u->posX = 0xE40000;
                u->posY = 0xE0000;
                u->frame = 10;
                u->velX = 0xFFF00000;
            }
            u = gCurTask;
            u->menuScreenTitlePhaseTimer = 8;
            u->menuScreenTitlePhase = 1;
        }
        break;
    case 2:
        if (--v->menuScreenTitlePhaseTimer == 0) {
            if (gMenuScreen == 8 && gPrevMenuScreen == 5)
                LoadGfxSet(44);
            else
                LoadGfxSet(gMenuScreenTitleGfx[gCurTask->menuScreenTitleScreen]);
            if (gMenuScreen == 7)
                gCurTask->frame = 11;
            else
                gCurTask->frame = 10;
            gCurTask->menuScreenTitlePhase = 3;
        }
        break;
    case 1:
        if (--v->menuScreenTitlePhaseTimer == 0) {
            if (gMenuScreen == 7) {
                v->posX = 0x600000;
                v->posY = 0x200000;
            } else {
                v->posX = 0x640000;
                v->posY = 0xE0000;
            }
            gCurTask->menuScreenTitlePhase = 3;
            gCurTask->velX = 0;
        }
        break;
    }
}

void Task_MenuBgPaletteCycle(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w, *x, *y, *z, *a;
    u16 *p;
    s32 s;

    t->menuBgPaletteCycleBlendFrom = 0;
    t->menuBgPaletteCycleBlendTo = 1;
    t->menuBgPaletteCycleBlendRatio = 0;
    s = gMenuScreen;
    t->menuBgPaletteCycleScreen = s;
    t->menuBgPaletteCyclePrevScreen = s;
    t->menuBgPaletteCycleFading = 0;
    t->menuBgPaletteCycleFadeRatio = 0;
    for (;;) {
        p = gUnk_08731D28[gMenuScreen];
        if (p != NULL && gUnk_08731D28[gCurTask->menuBgPaletteCycleScreen] != p) {
            gCurTask->menuBgPaletteCyclePrevScreen = gCurTask->menuBgPaletteCycleScreen;
            gCurTask->menuBgPaletteCycleScreen = gMenuScreen;
            gCurTask->menuBgPaletteCycleFadeRatio = 0;
            gCurTask->menuBgPaletteCycleFading = 1;
        }
        u = gCurTask;
        if (u->menuBgPaletteCycleBlendRatio == 256) {
            if (++u->menuBgPaletteCycleBlendFrom > 5)
                u->menuBgPaletteCycleBlendFrom = 0;
            v = gCurTask;
            if (++v->menuBgPaletteCycleBlendTo > 5)
                v->menuBgPaletteCycleBlendTo = 0;
            gCurTask->menuBgPaletteCycleBlendRatio = 0;
        }
        w = gCurTask;
        if ((w->menuBgPaletteCycleBlendRatio += 32) > 256)
            w->menuBgPaletteCycleBlendRatio = 256;
        x = gCurTask;
        switch (x->menuBgPaletteCycleFading) {
        case 0:
            BlendColors(gUnk_08731D28[x->menuBgPaletteCycleScreen] + x->menuBgPaletteCycleBlendFrom * 16, gUnk_08731D28[x->menuBgPaletteCycleScreen] + x->menuBgPaletteCycleBlendTo * 16, (u16)x->menuBgPaletteCycleBlendRatio, 16, gBgPaletteBank14 - 176);
            break;
        case 1:
            BlendColors(gUnk_08731D28[x->menuBgPaletteCyclePrevScreen] + x->menuBgPaletteCycleBlendFrom * 16, gUnk_08731D28[x->menuBgPaletteCyclePrevScreen] + x->menuBgPaletteCycleBlendTo * 16, (u16)x->menuBgPaletteCycleBlendRatio, 16, gBgPaletteBank14);
            y = gCurTask;
            BlendColors(gUnk_08731D28[y->menuBgPaletteCycleScreen] + y->menuBgPaletteCycleBlendFrom * 16, gUnk_08731D28[y->menuBgPaletteCycleScreen] + y->menuBgPaletteCycleBlendTo * 16, (u16)y->menuBgPaletteCycleBlendRatio, 16, gBgPaletteBank14 + 16);
            z = gCurTask;
            z->menuBgPaletteCycleFadeRatio += 16;
            BlendColors(gBgPaletteBank14, gBgPaletteBank14 + 16, (u16)z->menuBgPaletteCycleFadeRatio, 16, gBgPaletteBank14 - 176);
            a = gCurTask;
            if (a->menuBgPaletteCycleFadeRatio == 256) {
                a->menuBgPaletteCycleFading = 0;
                a->menuBgPaletteCycleFadeRatio = 0;
            }
            break;
        }
        TaskYieldTrampoline(1);
    }
}

void Task_MenuBackground(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *w;
    u16 *p;
    s32 s;

    s = gMenuScreen;
    t->menuBackgroundScreen = s;
    t->menuBackgroundPrevScreen = s;
    t->menuBackgroundFadePhase = 0;
    t->menuBackgroundFadeRatio = 0;
    LoadGfxSet(gUnk_08731D58[s]);
    RequestCopy(2, (u32)gUnk_08731CF8[gCurTask->menuBackgroundScreen], (u32)gBgPaletteBank2, 32);
    for (;;) {
        p = gUnk_08731CF8[gMenuScreen];
        if (p != NULL && gUnk_08731CF8[gCurTask->menuBackgroundScreen] != p) {
            gCurTask->menuBackgroundPrevScreen = gCurTask->menuBackgroundScreen;
            gCurTask->menuBackgroundScreen = gMenuScreen;
            gCurTask->menuBackgroundFadeRatio = 0;
            gCurTask->menuBackgroundFadePhase = 1;
        }
        u = gCurTask;
        if (u->menuBackgroundFadePhase != 0) {
            u->menuBackgroundFadeRatio += 32;
            switch (u->menuBackgroundFadePhase) {
            case 1:
                BlendColors(gUnk_08731CF8[u->menuBackgroundPrevScreen], gMenuBackgroundPalette, (u16)u->menuBackgroundFadeRatio, 16, gBgPaletteBank2);
                w = gCurTask;
                if (w->menuBackgroundFadeRatio == 256) {
                    w->menuBackgroundFadeRatio = 0;
                    w->menuBackgroundFadePhase = 2;
                    LoadGfxSet(gUnk_08731D58[w->menuBackgroundScreen]);
                }
                break;
            case 2:
                BlendColors(gMenuBackgroundPalette, gUnk_08731CF8[u->menuBackgroundScreen], (u16)u->menuBackgroundFadeRatio, 16, gBgPaletteBank2);
                w = gCurTask;
                if (w->menuBackgroundFadeRatio == 256) {
                    w->menuBackgroundFadeRatio = 0;
                    w->menuBackgroundFadePhase = 0;
                }
                break;
            }
        }
        TaskYieldTrampoline(1);
    }
}
