#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "player.h"
#include "ending.h"

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s16 f);
void BlendColors(s32 a, s32 b, s32 c, s32 d, void *e);
s32 PlaySfx(s32 id);
s32 IsOnScreen(s16 x, s16 y);

void sub_0801a07c(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)TaskDrawScreen;
    t->layer = 12;
    gCurTask->frameTable = gUnk_0874CE90;
    gCurTask->tileWord = 0;
    gCurTask->posX = 0;
    gCurTask->posY = 0;
    TaskStop();
    PlaySfx(0x242);
    gCurTask->cutsceneActorLoopCount = 0;
    do
    {
        gCurTask->frame = 0;
        TaskYieldTrampoline(1);
        gCurTask->frame = 16;
        TaskYieldTrampoline(1);
        gCurTask->frame = 1;
        TaskYieldTrampoline(1);
        gCurTask->frame = 17;
        TaskYieldTrampoline(1);
        gCurTask->frame = 2;
        TaskYieldTrampoline(1);
        gCurTask->frame = 18;
        TaskYieldTrampoline(1);
        gCurTask->frame = 3;
        TaskYieldTrampoline(1);
        gCurTask->frame = 19;
        TaskYieldTrampoline(1);
        gCurTask->frame = 4;
        TaskYieldTrampoline(1);
        gCurTask->frame = 20;
        TaskYieldTrampoline(1);
        gCurTask->frame = 5;
        TaskYieldTrampoline(1);
        gCurTask->frame = 21;
        TaskYieldTrampoline(1);
        gCurTask->frame = 6;
        TaskYieldTrampoline(1);
        gCurTask->frame = 11;
        TaskYieldTrampoline(1);
        gCurTask->frame = 7;
        TaskYieldTrampoline(1);
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 9;
        TaskYieldTrampoline(1);
        gCurTask->frame = 14;
        TaskYieldTrampoline(1);
        gCurTask->frame = 10;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(1);
        gCurTask->cutsceneActorLoopCount++;
    } while ((s16)gCurTask->cutsceneActorLoopCount <= 5);
    TaskExitTrampoline();
}

void sub_0801a1ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    struct Task *q;
    s32 mode;
    s32 anim;
    s32 n;
    u16 dx;
    u16 dy;
    s16 x;
    s16 y;

    t = gCurTask;
    if (t->frameTable == NULL)
        return;
    if (t->frame == -1)
        return;
    mode = 0;
    if (t->frame > 49 && (gFrameCount & 3) == 0)
        mode = 73;
    u = gCurTask;
    dx = u->pixelX;
    dy = u->pixelY;
    anim = PlayerLoadFrameTilesAndPalette(mode);
    v = gCurTask;
    if (v->frame > 49)
    {
        n = v->cutsceneActorFlashPhase + 1;
        v->cutsceneActorFlashPhase = n;
        if (n < 0)
            v->cutsceneActorFlashPhase = 0;
        w = gCurTask;
        if (w->cutsceneActorFlashPhase > 10)
            w->cutsceneActorFlashPhase = 0;
        BlendColors((u32)gUnk_0859A0B0, (u32)gUnk_0859A0D0, gUnk_08757368[gCurTask->cutsceneActorFlashPhase], 16,
                     (void *)((u32)gObjPalette
                              + (((gCurTask->tileWord >> 12) + 1) << 5)));
    }
    if (gPlayerCount > 1)
        RequestCopy(2, (gLocalPlayer << 5) + (u32)gPlayerPalettes,
                     (u32)gObjPalette + ((gCurTask->tileWord >> 12) << 5), 22);
    x = dx;
    y = dy;
    if (IsOnScreen(x, y) == 0)
        return;
    q = gCurTask;
    QueueSprite(q->layer, anim, q->spriteFlags, 0x800 | q->tileWord, x, y);
}

void sub_0801a310(void)
{
    struct Task *t;
    struct Task *u;
    struct TaskGfx *g;
    u16 *p;
    u16 *q;
    u32 dst;
    u32 prio;

    t = gCurTask;
    if (t->frameTable == NULL)
        return;
    if (t->frame == -1)
        return;
    prio = t->tileWord;
    dst = ((prio & 0x7FF) << 5) + (BG_VRAM + 0xFE00);
    g = (struct TaskGfx *)t->frameTable[t->frame];
    p = g->tiles;
    if (*p != 0xFFFF)
    {
        do
        {
            q = p + 1;
            RequestCopy(4, (u32)q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        } while (*p != 0xFFFF);
    }
    RequestCopy(2, (u32)(g->palette + 1), (u32)gObjPalette + ((prio >> 12) << 5),
                 *g->palette);
    u = gCurTask;
    QueueSprite(u->layer, g->oamTemplate, u->spriteFlags, 0x800 | u->tileWord, u->pixelX, u->pixelY);
}
