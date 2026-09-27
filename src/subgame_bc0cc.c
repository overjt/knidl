/* game_code_and_rodata_080653ec_0806ef5c 0x080BC0CC-0x080BD9E8
 * (issue #95, module M35, file 4 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BC0CC 0x080BD9E8 src/subgame_bc0cc.c --newpb
 *
 * Task type #94, the duel's sprite objects.  Task_QuickDrawObject dispatches the
 * table 0x087563B0 on Task.variant, the kind its spawner wrote (ten function
 * pointers; the call passes 12):
 *
 *   0  QuickDrawPlayer  a player: a six-state machine over 0x08756468 (entry
 *                    coroutines) / 0x08756480 (per-frame hooks), started in
 *                    state 0, 1 or 5 by Task.unk74
 *   1  sub_080bcdac  a label / icon sprite (draw callback sub_080bcbfc)
 *   2  QuickDrawTimer  a two-digit counter (QuickDrawTimerCount counts to 99 and
 *                    mirrors the value into the parent's Task.unk20)
 *   3  sub_080bcfa4  a scripted fly-in
 *   4  sub_080bd06c  a four-frame effect at (120, 96)
 *   5  QuickDrawOpponent  the single-player opponent CreateQuickDrawOpponent spawns: a
 *                    five-state machine over 0x087564E4 / 0x087564FC, one
 *                    animation set per level Task.unk18 (0-4) and its
 *                    reaction time from 0x087564B0[unk74 * 5 + unk18]; own
 *                    graphics loader QuickDrawLoadOpponentGraphics, draw callback sub_080bd938
 *   6  sub_080bd110  a three-frame sprite
 *   7  sub_080bd7f0  a sprite drawn by TaskDrawScreen (0x08755B90)
 *   8  sub_080bd8ac  the same graphics as a per-player award: in
 *                    gPrevGameState == 5, sub_080bd828 passes 1000 / 3000 /
 *                    5000 or 10 to AddPlayerScoreNoHud, or 1 to AddPlayerLivesNoHud
 *   9  sub_080bd9b0  a sprite drawn by sub_080bd938 (0x08755A68)
 *
 * sub_080bc1c4 places a player by the number of linked players and the
 * local id (x from 0x087563D8, then y / facing / animation from x);
 * sub_080bc0ec / sub_080bc168 slide it in and out.  gBrightness is a busy
 * counter the scripts wait on until it reaches 0.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "mode.h"
#include "hud.h"
#include "player.h"
#include "effect.h"
#include "subgame.h"

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void CallTableEntry(u32 a, u32 b, u32 *c);
s32 PlaySfx(s32 id);
void TaskSetEntry(void *fn, u32 i);

void Task_QuickDrawObject(void)
{
    CallTableEntry(gCurTask->variant, 12, gQuickDrawObjectKinds);
}

void sub_080bc0ec(void)
{
    struct Task *t;
    struct Task *u;

    if (gPlayerCount <= 2)
    {
        t = gCurTask;
        switch (t->pixelY)
        {
        case 120:
            if (t->pixelX > 67)
            {
                t->pixelX = 68;
                t->unk24 = 1;
            }
            else
                t->pixelX += 10;
            break;
        case 56:
            if (t->pixelX <= 164)
            {
                t->pixelX = 164;
                t->unk24 = 1;
            }
            else
                t->pixelX -= 10;
            break;
        }
    }
    u = gCurTask;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
}

void sub_080bc168(void)
{
    struct Task *t;

    sub_080bc1c4();
    if (gPlayerCount <= 2)
    {
        switch (gCurTask->pixelX)
        {
        case 68:
            gCurTask->pixelX = -44;
            break;
        case 164:
            gCurTask->pixelX = 276;
            break;
        }
        t = gCurTask;
        t->unk24 = 0;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
    }
}

void sub_080bc1c4(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 n;
    s32 k;

    t->spriteFlags &= 0x7FFF;
    t->tileWord = t->unk18 << 12;
    n = gPlayerCount - 1;
    k = gLocalPlayer - t->unk18;
    t->pixelX = gUnk_087563D8[n * 8 + k];
    t->layer = 11;
    switch (gCurTask->pixelX)
    {
    case 68:
        gCurTask->pixelY = 120;
        gCurTask->facing = 1;
        gCurTask->frameTable = gUnk_08755AF0;
        gCurTask->layer = 8;
        break;
    case 164:
        gCurTask->pixelY = 56;
        gCurTask->facing = -1;
        gCurTask->frameTable = gUnk_08755B68;
        gCurTask->layer = 11;
        break;
    case 188:
        gCurTask->pixelY = 104;
        gCurTask->facing = -1;
        gCurTask->frameTable = gUnk_08755B40;
        gCurTask->layer = 9;
        break;
    case 60:
        gCurTask->pixelY = 72;
        gCurTask->facing = 1;
        gCurTask->frameTable = gUnk_08755B18;
        gCurTask->layer = 10;
        break;
    default:
        gCurTask->pixelY = 0;
        gCurTask->facing = 1;
        gCurTask->frameTable = gUnk_08755AF0;
        gCurTask->layer = 12;
        break;
    }
    u = gCurTask;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    u->frame = 0;
    u->unk30 = -1;
    u->unk34 = -1;
    u->unk2C = u->pixelX;
}

void sub_080bc30c(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 v = t->unk20;

    if (v == 4)
    {
        t->pixelX = 120;
        t->pixelY = 96;
    }
    else
    {
        s32 n = gPlayerCount - 1;
        s32 k = gLocalPlayer - v;

        t->pixelX = gUnk_08756410[n * 8 + k];
        switch (t->pixelX)
        {
        case 120:
            t->pixelY = 112;
            break;
        case 136:
            t->pixelY = 80;
            break;
        case 144:
            t->pixelY = 104;
            break;
        case 112:
            t->pixelY = 96;
            break;
        default:
            gCurTask->pixelY = 0;
            break;
        }
    }
    u = gCurTask;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    u->frame = 1;
}

void sub_080bc3c8(void)
{
    struct Task *t = gCurTask;

    switch (t->unk2C)
    {
    case 68:
        t->velX = -0x20000;
        t->velY = -0x50000;
        t->accelY = 0x4000;
        t->speedLimitY = 0x50000;
        break;
    case 164:
        t->velX = 0x20000;
        t->velY = -0x50000;
        t->accelY = 0x3000;
        t->speedLimitY = 0x80000;
        break;
    case 188:
        t->velX = 0x40000;
        t->velY = -0x50000;
        t->accelY = 0x3000;
        t->speedLimitY = 0x50000;
        break;
    case 60:
        t->velX = -0x40000;
        t->velY = -0x50000;
        t->accelY = 0x3000;
        t->speedLimitY = 0x50000;
        break;
    default:
        TaskStop();
        break;
    }
    gCurTask->posX = gCurTask->pixelX << 16;
}

void sub_080bc460(void)
{
    struct Task *t = gCurTask;

    switch (t->unk2C)
    {
    case 68:
        t->unk20 = 0;
        break;
    case 164:
        t->unk20 = 3;
        break;
    case 188:
        t->unk20 = 1;
        break;
    case 60:
        t->unk20 = 2;
        break;
    default:
        gCurTask->unk20 = 0;
        break;
    }
    PlaySfx(256);
}

void sub_080bc4b0(void)
{
    s32 id = TaskCreateFrom(94, 32);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];
        struct Task *p;

        t->variant = 1;
        p = gCurTask;
        t->unk18 = p->unk20;
        t->unk1C = 6;
        t->unk20 = -1;
        t->unk24 = -1;
        t->pixelX = p->pixelX + gUnk_08756448[p->unk20] * p->facing;
        t->pixelY = p->pixelY - gUnk_08756450[p->unk20];
        t->unk34 = 0;
    }
    gCurTask->unk34 = id;
}

void sub_080bc54c(void)
{
    s32 id = TaskCreateFrom(94, 32);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];
        struct Task *p;

        t->variant = 6;
        p = gCurTask;
        t->unk18 = p->unk20;
        t->pixelX = p->pixelX + gUnk_08756458[p->unk20];
        t->pixelY = p->pixelY - gUnk_08756460[p->unk20];
    }
    gCurTask->unk30 = id;
}

void sub_080bc5cc(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
        s16 x = t->pixelX - 24;
        s16 y = t->pixelY - 32;
        struct Task *n;
        struct Task *u;

        /* The ROM keeps a dead `ldrsh` of t->unk48 here (same artifact as
           sub_080bb874): a switch whose arms all reduce to no-ops.  Two
           labels reproduce the allocation; one or four do not. */
        switch (t->pixelX)
        {
        case 120:
            x = x;
            break;
        case 56:
            x = x;
            break;
        }
        n = &gTasks[idx];
        n->variant = 1;
        u = gCurTask;
        n->unk18 = u->unk18;
        n->unk1C = 0;
        n->unk20 = 60;
        n->unk24 = -1;
        n->pixelX = x;
        n->pixelY = y;
        if (gLocalPlayer == u->unk18)
        {
            n->unk18 = 4;
            n->pixelY = u->pixelY - 32;
            n->pixelX += 12;
        }
        n->unk34 = 0;
        n->layer = gCurTask->layer - 8;
    }
    gCurTask->unk1C = 60;
}

void sub_080bc680(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
        s16 x = t->pixelX - 24;
        s16 y = t->pixelY - 32;
        struct Task *n;

        /* Dead `ldrsh` of t->unk48 in the ROM: a switch whose arms are all
           no-ops (same artifact as sub_080bb874). */
        switch (t->pixelX)
        {
        case 120:
            x = x;
            break;
        case 56:
            x = x;
            break;
        case 104:
            x = x;
            break;
        case 72:
            x = x;
            break;
        }
        n = &gTasks[idx];
        n->variant = 1;
        n->unk18 = gQuickDrawWins[gCurTask->unk18];
        n->unk1C = 2;
        n->unk20 = 60;
        n->unk24 = -1;
        n->pixelX = x + 4;
        n->pixelY = y;
    }
    gCurTask->unk1C = 60;
}

void sub_080bc70c(void)
{
    if (gCurTaskIdx == 0)
    {
        struct Task *p = &gTasks[gCurTask->parent];
        p->unk24 = 1;
    }
    gCurTask->state = 1;
}

void QuickDrawSetPlayerState(s32 a0, u16 a1)
{
    struct Task *t = &gTasks[a0];
    struct Task *p = &gTasks[t->parent];

    if (p->unk18 != 2)
    {
        t->state = a1;
        TaskSetEntry(QuickDrawPlayerEnterState, a0);
        if (t->unk34 != -1)
            TaskFree(t->unk34);
        if (t->unk30 != -1)
            TaskFree(t->unk30);
        t->unk30 = -1;
        t->unk34 = -1;
    }
}

void sub_080bc79c(u16 a0)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
        QuickDrawSetPlayerState(i, a0);
}

u8 sub_080bc7c8(void)
{
    struct Task *t = gCurTask;
    s16 x = t->pixelX;
    s16 y = t->pixelY;

    if (x > -64 && x < 304 && y > -64 && y < 224)
        return 1;
    return 0;
}

void sub_080bc800(void)
{
    gCurTask->tileWord = gCurTask->unk18 << 12;
    gCurTask->frameTable = gUnk_08755B18;
    gCurTask->layer = 8;
    gCurTask->frame = 0;
    gCurTask->pixelX = 120;
    gCurTask->pixelY = 68;
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
}

void QuickDrawPlayer(void)
{
    struct Task *t = gCurTask;

    t->drawCallback = (u32)TaskDrawScreen;
    t->updateCallback = (u32)QuickDrawPlayerUpdate;
    t->layer = 7;
    t = gCurTask;
    switch (t->unk74)
    {
    case 0:
        t->state = 0;
        break;
    case 1:
        t->state = 1;
        break;
    case 2:
        t->state = 5;
        break;
    }
    CallTableEntry(gCurTask->state, 6, gUnk_08756468);
    TaskSleepForever();
}

void QuickDrawPlayerUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 6, gUnk_08756480);
}

void QuickDrawPlayerEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gUnk_08756468);
}

void sub_080bc8e0(void)
{
    gCurTask->updateState = 0;
    sub_080bc168();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080bc5cc();
    TaskYieldTrampoline(gCurTask->unk1C);
    if (gPlayerCount != 1)
    {
        sub_080bc680();
        TaskYieldTrampoline(gCurTask->unk1C);
    }
    sub_080bc70c();
    TaskSleepForever();
}

void sub_080bc948(void)
{
    if (gBrightness == 0 && gCurTask->unk24 == 0)
        sub_080bc0ec();
    if (gCurTask->state != 0)
        TaskSetEntry(QuickDrawPlayerEnterState, gCurTaskIdx);
}

void sub_080bc988(void)
{
    gCurTask->updateState = 1;
    sub_080bc1c4();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_080bc9c0(void)
{
}

void sub_080bc9c4(void)
{
    struct Task *t;
    u16 saved;
    s16 x;
    s32 i;

    gCurTask->updateState = 2;
    sub_080bc30c();
    saved = gCurTask->pixelX;
    x = gCurTask->pixelX;
    for (i = 0; i < 2; i++)
    {
        t = gCurTask;
        t->pixelX = x + gUnk_08756498[0] * (u16)t->facing;
        t->posX = t->pixelX << 16;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->pixelX = x - gUnk_08756498[0] * (u16)t->facing;
        t->posX = t->pixelX << 16;
        TaskYieldTrampoline(1);
    }
    gCurTask->pixelX = saved;
    gCurTask->posX = gCurTask->pixelX << 16;
    TaskSleepForever();
}

void sub_080bca68(void)
{
}

void sub_080bca6c(void)
{
    s32 i;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->updateState = 3;
    sub_080bc3c8();
    while (1)
    {
        gCurTask->frame = 2;
        TaskYieldTrampoline(5);
        for (i = 0; i < 7; i++)
        {
            gCurTask->frame++;
            TaskYieldTrampoline(5);
        }
    }
}

void sub_080bcaac(void)
{
    if (gCurTask->velX != 0 && gCurTask->velY != 0)
    {
        u8 r = sub_080bc7c8();
        if (r == 0)
        {
            TaskStop();
            gCurTask->moveCallback = r;
        }
    }
}

void sub_080bcadc(void)
{
    struct Task *t;
    struct Task *p;
    u16 saved;
    s16 x;
    s32 i;

    gCurTask->updateState = 4;
    sub_080bc460();
    saved = gCurTask->pixelX;
    x = gCurTask->pixelX;
    for (i = 0; i < 2; i++)
    {
        t = gCurTask;
        t->pixelX = x + gUnk_08756498[t->unk20] * (u16)t->facing;
        t->posX = t->pixelX << 16;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->pixelX = x - gUnk_08756498[t->unk20] * (u16)t->facing;
        t->posX = t->pixelX << 16;
        TaskYieldTrampoline(1);
    }
    gCurTask->pixelX = saved;
    gCurTask->posX = gCurTask->pixelX << 16;
    sub_080bc4b0();
    sub_080bc54c();
    TaskYieldTrampoline(30);
    p = &gTasks[gCurTask->parent];
    p->hitTimer = 1;
    TaskSleepForever();
}

void sub_080bcbbc(void)
{
}

void sub_080bcbc0(void)
{
    gCurTask->updateState = 5;
    sub_080bc800();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_080bcbf8(void)
{
}

void sub_080bcbfc(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    u32 *tbl = t->frameTable;

    if (tbl != NULL && t->frame != -1 && (u16)(t->pixelX + 63) <= 366
        && t->pixelY > -64 && t->pixelY < 224)
    {
        QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
        u = gCurTask;
        if (u->unk24 != -1)
        {
            tbl = (u32 *)u->unk30;
            QueueSprite(u->layer, tbl[u->unk24], u->spriteFlags, u->unk34,
                         u->pixelX + u->unk28, (s16)(u->pixelY + u->unk2C));
        }
    }
}

void sub_080bccbc(void)
{
    struct Task *t;
    u8 *p = &gCurTask->layer;

    if (*p == 0)
        *p = 4;
    else
        *p += 4;
    switch (gCurTask->unk1C)
    {
    case 0:
        gCurTask->frameTable = gUnk_08755AA4;
        break;
    case 1:
        gCurTask->frameTable = gUnk_08755A78;
        break;
    case 2:
        gCurTask->frameTable = gUnk_08755A5C;
        break;
    case 3:
        gCurTask->frameTable = gUnk_08755A7C;
        break;
    case 4:
        gCurTask->frameTable = gUnk_08755A88;
        break;
    case 5:
        gCurTask->frameTable = gUnk_08755A34;
        break;
    case 6:
        gCurTask->frameTable = gUnk_08755AB8;
        break;
    case 7:
        gCurTask->frameTable = gUnk_08755AC8;
        break;
    }
    t = gCurTask;
    t->tileWord |= 0x800;
    t->unk34 = t->tileWord;
    if (t->unk20 != -1)
    {
        t->frame = t->unk18;
        TaskYieldTrampoline(t->unk20);
    }
    else
    {
        t->frame = t->unk18;
    }
}

void sub_080bcdac(void)
{
    gCurTask->drawCallback = (u32)sub_080bcbfc;
    sub_080bccbc();
    if (gCurTask->unk20 != -1)
        TaskExitTrampoline();
    else
        TaskSleepForever();
}

void sub_080bcde0(void)
{
    struct Task *t;

    gCurTask->taskClass = 4;
    t = gCurTask;
    t->frameTable = gUnk_08755A34;
    t->frame = 0;
    t->unk24 = 0;
    t->pixelX = 206;
    t->pixelY = 132;
    t->unk28 = -8;
    t->unk2C = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->tileWord |= 0x800;
}

void QuickDrawTimerCount(void)
{
    struct Task *t = gCurTask;
    struct Task *p = &gTasks[t->parent];

    if (t->unk18 <= 98)
    {
        s32 r;
        s32 q;

        t->unk18++;
        p->unk20 = t->unk18;
        r = t->unk18;
        q = 0;
        while (r > 9)
        {
            r -= 10;
            q++;
        }
        gCurTask->unk24 = q;
        gCurTask->frame = r;
    }
}

void sub_080bce74(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    u32 *tbl = t->frameTable;

    if (tbl != NULL && t->frame != -1 && (u16)(t->pixelX + 63) <= 366
        && t->pixelY > -64 && t->pixelY < 224)
    {
        QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
        u = gCurTask;
        QueueSprite(u->layer, tbl[u->unk24], u->spriteFlags, u->tileWord,
                     u->pixelX + u->unk28, (s16)(u->pixelY + u->unk2C));
        v = gCurTask;
        QueueSprite(v->layer + 1, gUnk_08755AD8[0], v->spriteFlags, v->tileWord,
                     v->pixelX, (s16)(v->pixelY + 8));
    }
}

void QuickDrawTimer(void)
{
    struct Task *t = gCurTask;

    t->drawCallback = (u32)sub_080bce74;
    t->updateCallback = (u32)sub_080bcf8c;
    t->layer = 4;
    sub_080bcde0();
    TaskSleepForever();
}

void sub_080bcf8c(void)
{
    if (gCurTask->unk1C != 0)
        QuickDrawTimerCount();
}

void sub_080bcfa4(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t->drawCallback = (u32)TaskDrawScreen;
    t->moveCallback = (u32)TaskMove;
    t->layer = 4;
    u = gCurTask;
    u->frameTable = gUnk_08755A7C;
    u->tileWord |= 0x800;
    u->pixelX = 240;
    u->pixelY = 0;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    TaskYieldTrampoline(2);
    v = gCurTask;
    v->velX = 0xFFD00000;
    v->velY = 0x180000;
    v->frame = 1;
    TaskYieldTrampoline(5);
    w = gCurTask;
    w->pixelX = 64;
    w->pixelY = -16;
    w->posX = w->pixelX << 16;
    w->posY = w->pixelY << 16;
    w->velX = 0x200000;
    w->velY = 0x400000;
    w->frame++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void sub_080bd06c(void)
{
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 4;
    gCurTask->frameTable = gUnk_08755ADC;
    gCurTask->tileWord |= 0x800;
    gCurTask->pixelX = 120;
    gCurTask->pixelY = 96;
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    gCurTask->frame = 0;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    TaskYieldTrampoline(60);
    TaskExitTrampoline();
}

void sub_080bd110(void)
{
    struct Task *t;

    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->layer = 4;
    t = gCurTask;
    t->frameTable = gUnk_08755AC8;
    t->tileWord |= 0x800;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    t->frame = t->unk18;
    t->velY = gUnk_087564A0[t->unk18];
    TaskYieldTrampoline(3);
    TaskStop();
    TaskSleepForever();
}

void QuickDrawLoadOpponentGraphics(s32 a0)
{
    struct GfxDesc *d = gUnk_087564D0[a0];

    LZ77UnCompWram(d->unk0C, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, 0x06013000, d->unk02 << 5);
    RequestCopy(2, d->unk08, (u32)gUnk_03001570, d->unk00 << 5);
}

void QuickDrawSetOpponentState(s32 a0, u16 a1)
{
    struct Task *t = &gTasks[a0];
    struct Task *u = &gTasks[t->parent];

    if (u->unk18 != 2)
    {
        t->state = a1;
        TaskSetEntry(QuickDrawOpponentEnterState, a0);
    }
}

void sub_080bd210(void)
{
    struct Task *t = gCurTask;

    if (t->pixelX <= 164)
    {
        t->pixelX = 164;
        t->unk24 = 1;
    }
    else
    {
        t->pixelX -= 10;
    }
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
}

void sub_080bd25c(void)
{
    struct Task *t;

    sub_080bd290();
    t = gCurTask;
    t->pixelX = 276;
    t->unk24 = 0;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
}

void sub_080bd290(void)
{
    s32 i;

    gCurTask->pixelX = 164;
    gCurTask->pixelY = 72;
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    gCurTask->facing = -1;
    gCurTask->spriteFlags &= 0x7FFF;
    switch (gCurTask->unk18)
    {
    case 0:
        gCurTask->frameTable = gUnk_087559E4;
        break;
    case 1:
        gCurTask->frameTable = gUnk_087559F4;
        break;
    case 2:
        gCurTask->frameTable = gUnk_08755A04;
        break;
    case 3:
        gCurTask->frameTable = gUnk_08755A24;
        break;
    case 4:
        gCurTask->frameTable = gUnk_08755A14;
        break;
    }
    gCurTask->tileWord = 0x8180;
    gCurTask->frame = 0;
    i = gCurTask->unk74 * 5 + gCurTask->unk18;
    gCurTask->unk1C = gUnk_087564B0[i];
}

void sub_080bd370(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
        s16 x = t->pixelX - 24;
        s16 y = t->pixelY - 32;
        struct Task *n;

        /* Dead `ldrsh` of t->unk48 in the ROM: a switch whose arms are all
           no-ops (same artifact as sub_080bb874/sub_080bc5cc/sub_080bc680).
           Three labels, the middle one touching y, reproduce the allocation
           (x r3, y r4, idx r5); two or four labels, or x-only arms, swap
           y and idx. */
        switch (t->pixelX)
        {
        case 120:
            x = x;
            break;
        case 56:
            y = y;
            break;
        case 104:
            x = x;
            break;
        }
        n = &gTasks[idx];
        n->variant = 5;
        n->unk76 = 1;
        n->pixelX = x;
        n->pixelY = y;
    }
    gCurTask->unk20 = 60;
}

void sub_080bd3dc(u8 a0)
{
    if (a0)
    {
        struct Task *t = gCurTask;

        t->pixelX = 120;
        t->pixelY = 96;
        switch (t->unk18)
        {
        case 0:
            PlaySfx(253);
            break;
        case 1:
            PlaySfx(258);
            break;
        case 2:
            PlaySfx(259);
            break;
        case 3:
            PlaySfx(261);
            break;
        case 4:
            PlaySfx(260);
            break;
        }
    }
    else
    {
        gCurTask->pixelX = 148;
        gCurTask->pixelY = 80;
    }
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    gCurTask->frame = 1;
}

void sub_080bd494(void)
{
    struct Task *t = gCurTask;

    t->velX = 0x20000;
    t->velY = -0x50000;
    t->accelY = 0x3000;
    t->speedLimitY = 0x80000;
    t->frame = 2;
}

void QuickDrawOpponent(void)
{
    struct Task *t;

    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = (u32)QuickDrawOpponentUpdate;
    gCurTask->layer = 12;
    t = gCurTask;
    t->unk18 = 0;
    if (t->unk76 != 0)
        t->state = 5;
    else
        t->state = 0;
    CallTableEntry(gCurTask->state, 6, gUnk_087564E4);
    TaskSleepForever();
}

void QuickDrawOpponentUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 6, gUnk_087564FC);
}

void QuickDrawOpponentEnterState(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    CallTableEntry(t->state, 6, gUnk_087564E4);
}

void sub_080bd544(void)
{
    gCurTask->updateState = 0;
    sub_080bd25c();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080bd370();
    TaskYieldTrampoline(gCurTask->unk20);
    gCurTask->state = 1;
    TaskSleepForever();
}

void sub_080bd594(void)
{
    if (gBrightness == 0 && gCurTask->unk24 == 0)
        sub_080bd210();
    if (gCurTask->state != 0)
        TaskSetEntry(QuickDrawOpponentEnterState, gCurTaskIdx);
}

void sub_080bd5d4(void)
{
    gCurTask->updateState = 1;
    sub_080bd290();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_080bd60c(void)
{
}

void sub_080bd610(void)
{
    gCurTask->updateState = 2;
    sub_080bd3dc(1);
    TaskSleepForever();
}

void sub_080bd62c(void)
{
}

void sub_080bd630(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->updateState = 3;
    sub_080bd494();
    gUnk_0200B048 = ++gCurTask->unk18;
    TaskSleepForever();
}

void sub_080bd664(void)
{
    if (gCurTask->velX != 0 && gCurTask->velY != 0)
    {
        u8 r = sub_080bc7c8();
        if (r == 0)
        {
            TaskStop();
            gCurTask->moveCallback = r;
        }
    }
}

void sub_080bd694(void)
{
    gCurTask->updateState = 4;
    sub_080bd3dc(0);
    TaskSleepForever();
}

void sub_080bd6b0(void)
{
}

void sub_080bd6b4(void)
{
    struct Task *t;

    gCurTask->updateCallback = 0;
    gCurTask->layer = 4;
    switch (gUnk_0200B048)
    {
    case 0:
        gCurTask->frameTable = gUnk_087559E4;
        break;
    case 1:
        gCurTask->frameTable = gUnk_087559F4;
        break;
    case 2:
        gCurTask->frameTable = gUnk_08755A04;
        break;
    case 3:
        gCurTask->frameTable = gUnk_08755A24;
        break;
    case 4:
        gCurTask->frameTable = gUnk_08755A14;
        break;
    }
    t = gCurTask;
    t->tileWord = 0x9180;
    t->frame = 3;
    switch (gUnk_0200B048)
    {
    case 0:
        gCurTask->pixelX -= 16;
        gCurTask->pixelY -= 8;
        break;
    case 1:
        gCurTask->pixelY -= 12;
        break;
    case 2:
        gCurTask->pixelX -= 32;
        gCurTask->pixelY -= 28;
        break;
    case 3:
        gCurTask->pixelX -= 20;
        gCurTask->pixelY -= 32;
        break;
    case 4:
        gCurTask->pixelX -= 24;
        gCurTask->pixelY -= 8;
        break;
    }
    gCurTask->posX = gCurTask->pixelX << 16;
    gCurTask->posY = gCurTask->pixelY << 16;
    TaskYieldTrampoline(60);
    TaskExitTrampoline();
}

void sub_080bd7ec(void)
{
}

void sub_080bd7f0(void)
{
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 4;
    gCurTask->frameTable = gUnk_08755B90;
    gCurTask->tileWord |= 0x800;
    TaskSleepForever();
}

void sub_080bd828(void)
{
    if (gPrevGameState == 5)
    {
        switch (gCurTask->frame)
        {
        case 5:
            AddPlayerScoreNoHud(10, gCurTask->unk1C);
            break;
        case 2:
            AddPlayerScoreNoHud(1000, gCurTask->unk1C);
            break;
        case 3:
            AddPlayerScoreNoHud(3000, gCurTask->unk1C);
            break;
        case 4:
            AddPlayerScoreNoHud(5000, gCurTask->unk1C);
            break;
        case 6:
            AddPlayerLivesNoHud(1, gCurTask->unk1C);
            break;
        }
    }
}

void sub_080bd8ac(void)
{
    struct Task *t;

    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 4;
    t = gCurTask;
    t->frameTable = gUnk_08755B90;
    if (gPlayerCount != 1)
        t->frame = gUnk_08756514[t->unk18];
    else
        t->frame = gUnk_0875651C[t->unk18];
    if (gCurTask->frame == 6 && gLocalPlayer == gCurTask->unk1C)
        PlaySfx(220);
    gCurTask->tileWord |= 0x800;
    sub_080bd828();
    TaskSleepForever();
}

void sub_080bd938(void)
{
    struct Task *t = gCurTask;
    u32 *p = t->frameTable;

    if (p != NULL && t->frame != -1)
    {
        if (t->pixelX >= -63 && t->pixelX <= 303 && t->pixelY > -64 && t->pixelY < 224)
            QueueSprite(t->layer, p[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
}

void sub_080bd9b0(void)
{
    gCurTask->drawCallback = (u32)sub_080bd938;
    gCurTask->layer = 4;
    gCurTask->frameTable = gUnk_08755A68;
    gCurTask->tileWord |= 0x800;
    TaskSleepForever();
}
