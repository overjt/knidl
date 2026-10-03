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
 *   1  QuickDrawLabel  a label / icon sprite (draw callback QuickDrawLabelDraw)
 *   2  QuickDrawTimer  a two-digit counter (QuickDrawTimerCount counts to 99 and
 *                    mirrors the value into the parent's Task.unk20)
 *   3  QuickDrawSlash  a scripted fly-in
 *   4  QuickDrawBurst  a four-frame effect at (120, 96)
 *   5  QuickDrawOpponent  the single-player opponent CreateQuickDrawOpponent spawns: a
 *                    five-state machine over 0x087564E4 / 0x087564FC, one
 *                    animation set per level Task.unk18 (0-4) and its
 *                    reaction time from 0x087564B0[unk74 * 5 + unk18]; own
 *                    graphics loader QuickDrawLoadOpponentGraphics, draw callback QuickDrawRankLabelDraw
 *   6  QuickDrawSweatDrop  a three-frame sprite
 *   7  QuickDrawBonusSign  a sprite drawn by TaskDrawScreen (0x08755B90)
 *   8  QuickDrawBonus  the same graphics as a per-player award: in
 *                    gPrevGameState == 5, QuickDrawGiveBonus passes 1000 / 3000 /
 *                    5000 or 10 to AddPlayerScoreNoHud, or 1 to AddPlayerLivesNoHud
 *   9  QuickDrawRankLabel  a sprite drawn by QuickDrawRankLabelDraw (0x08755A68)
 *
 * QuickDrawPlacePlayer places a player by the number of linked players and the
 * local id (x from 0x087563D8, then y / facing / animation from x);
 * QuickDrawPlayerSlideIn / QuickDrawPlayerStartSlideIn slide it in and out.  gBrightness is a busy
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

void QuickDrawPlayerSlideIn(void)
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

void QuickDrawPlayerStartSlideIn(void)
{
    struct Task *t;

    QuickDrawPlacePlayer();
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

void QuickDrawPlacePlayer(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 n;
    s32 k;

    t->spriteFlags &= 0x7FFF;
    t->tileWord = t->unk18 << 12;
    n = gPlayerCount - 1;
    k = gLocalPlayer - t->unk18;
    t->pixelX = gQuickDrawSeatX[n * 8 + k];
    t->layer = 11;
    switch (gCurTask->pixelX)
    {
    case 68:
        gCurTask->pixelY = 120;
        gCurTask->facing = 1;
        gCurTask->frameTable = gQuickDrawPlayerNearFrames;
        gCurTask->layer = 8;
        break;
    case 164:
        gCurTask->pixelY = 56;
        gCurTask->facing = -1;
        gCurTask->frameTable = gQuickDrawPlayerFarFrames;
        gCurTask->layer = 11;
        break;
    case 188:
        gCurTask->pixelY = 104;
        gCurTask->facing = -1;
        gCurTask->frameTable = gQuickDrawPlayerRightFrames;
        gCurTask->layer = 9;
        break;
    case 60:
        gCurTask->pixelY = 72;
        gCurTask->facing = 1;
        gCurTask->frameTable = gQuickDrawPlayerLeftFrames;
        gCurTask->layer = 10;
        break;
    default:
        gCurTask->pixelY = 0;
        gCurTask->facing = 1;
        gCurTask->frameTable = gQuickDrawPlayerNearFrames;
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

void QuickDrawPlacePlayerStrike(void)
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

        t->pixelX = gQuickDrawStrikeX[n * 8 + k];
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

void QuickDrawPlayerSetKnockBack(void)
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

void CreateQuickDrawFalseStartMark(void)
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

void CreateQuickDrawSweatDrop(void)
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

void CreateQuickDrawPlayerTag(void)
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
           QuickDrawPlaceRankLabel): a switch whose arms all reduce to no-ops.  Two
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

void CreateQuickDrawWinCountLabel(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
        s16 x = t->pixelX - 24;
        s16 y = t->pixelY - 32;
        struct Task *n;

        /* Dead `ldrsh` of t->unk48 in the ROM: a switch whose arms are all
           no-ops (same artifact as QuickDrawPlaceRankLabel). */
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

void QuickDrawSetAllPlayersState(u16 a0)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
        QuickDrawSetPlayerState(i, a0);
}

u8 QuickDrawIsTaskOnScreen(void)
{
    struct Task *t = gCurTask;
    s16 x = t->pixelX;
    s16 y = t->pixelY;

    if (x > -64 && x < 304 && y > -64 && y < 224)
        return 1;
    return 0;
}

void QuickDrawPlacePlayerForResults(void)
{
    gCurTask->tileWord = gCurTask->unk18 << 12;
    gCurTask->frameTable = gQuickDrawPlayerLeftFrames;
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
    CallTableEntry(gCurTask->state, 6, gQuickDrawPlayerStates);
    TaskSleepForever();
}

void QuickDrawPlayerUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 6, gQuickDrawPlayerStateUpdates);
}

void QuickDrawPlayerEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gQuickDrawPlayerStates);
}

void QuickDrawPlayerArrive(void)
{
    gCurTask->updateState = 0;
    QuickDrawPlayerStartSlideIn();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    CreateQuickDrawPlayerTag();
    TaskYieldTrampoline(gCurTask->unk1C);
    if (gPlayerCount != 1)
    {
        CreateQuickDrawWinCountLabel();
        TaskYieldTrampoline(gCurTask->unk1C);
    }
    sub_080bc70c();
    TaskSleepForever();
}

void QuickDrawPlayerArriveUpdate(void)
{
    if (gBrightness == 0 && gCurTask->unk24 == 0)
        QuickDrawPlayerSlideIn();
    if (gCurTask->state != 0)
        TaskSetEntry(QuickDrawPlayerEnterState, gCurTaskIdx);
}

void QuickDrawPlayerReady(void)
{
    gCurTask->updateState = 1;
    QuickDrawPlacePlayer();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void QuickDrawPlayerReadyUpdate(void)
{
}

void QuickDrawPlayerStrike(void)
{
    struct Task *t;
    u16 saved;
    s16 x;
    s32 i;

    gCurTask->updateState = 2;
    QuickDrawPlacePlayerStrike();
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

void QuickDrawPlayerStrikeUpdate(void)
{
}

void QuickDrawPlayerLose(void)
{
    s32 i;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->updateState = 3;
    QuickDrawPlayerSetKnockBack();
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

void QuickDrawPlayerLoseUpdate(void)
{
    if (gCurTask->velX != 0 && gCurTask->velY != 0)
    {
        u8 r = QuickDrawIsTaskOnScreen();
        if (r == 0)
        {
            TaskStop();
            gCurTask->moveCallback = r;
        }
    }
}

void QuickDrawPlayerFalseStart(void)
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
    CreateQuickDrawFalseStartMark();
    CreateQuickDrawSweatDrop();
    TaskYieldTrampoline(30);
    p = &gTasks[gCurTask->parent];
    p->hitTimer = 1;
    TaskSleepForever();
}

void QuickDrawPlayerFalseStartUpdate(void)
{
}

void QuickDrawPlayerResults(void)
{
    gCurTask->updateState = 5;
    QuickDrawPlacePlayerForResults();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void QuickDrawPlayerResultsUpdate(void)
{
}

void QuickDrawLabelDraw(void)
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
        gCurTask->frameTable = gQuickDrawPlayerTagFrames;
        break;
    case 1:
        gCurTask->frameTable = gQuickDrawRedrawFrames;
        break;
    case 2:
        gCurTask->frameTable = gQuickDrawWinCountFrames;
        break;
    case 3:
        gCurTask->frameTable = gUnk_08755A7C;
        break;
    case 4:
        gCurTask->frameTable = gUnk_08755A88;
        break;
    case 5:
        gCurTask->frameTable = gQuickDrawDigitFrames;
        break;
    case 6:
        gCurTask->frameTable = gQuickDrawFalseStartMarkFrames;
        break;
    case 7:
        gCurTask->frameTable = gQuickDrawSweatDropFrames;
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

void QuickDrawLabel(void)
{
    gCurTask->drawCallback = (u32)QuickDrawLabelDraw;
    sub_080bccbc();
    if (gCurTask->unk20 != -1)
        TaskExitTrampoline();
    else
        TaskSleepForever();
}

void QuickDrawTimerInit(void)
{
    struct Task *t;

    gCurTask->taskClass = 4;
    t = gCurTask;
    t->frameTable = gQuickDrawDigitFrames;
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

void QuickDrawTimerDraw(void)
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
        QueueSprite(v->layer + 1, gQuickDrawTimerBoardFrames[0], v->spriteFlags, v->tileWord,
                     v->pixelX, (s16)(v->pixelY + 8));
    }
}

void QuickDrawTimer(void)
{
    struct Task *t = gCurTask;

    t->drawCallback = (u32)QuickDrawTimerDraw;
    t->updateCallback = (u32)QuickDrawTimerUpdate;
    t->layer = 4;
    QuickDrawTimerInit();
    TaskSleepForever();
}

void QuickDrawTimerUpdate(void)
{
    if (gCurTask->unk1C != 0)
        QuickDrawTimerCount();
}

void QuickDrawSlash(void)
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

void QuickDrawBurst(void)
{
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 4;
    gCurTask->frameTable = gQuickDrawBurstFrames;
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

void QuickDrawSweatDrop(void)
{
    struct Task *t;

    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->layer = 4;
    t = gCurTask;
    t->frameTable = gQuickDrawSweatDropFrames;
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
    struct GfxDesc *d = gQuickDrawOpponentGfx[a0];

    LZ77UnCompWram(d->unk0C, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, OBJ_VRAM0 + 0x3000, d->unk02 << 5);
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

void QuickDrawOpponentSlideIn(void)
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

void QuickDrawOpponentStartSlideIn(void)
{
    struct Task *t;

    QuickDrawPlaceOpponent();
    t = gCurTask;
    t->pixelX = 276;
    t->unk24 = 0;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
}

void QuickDrawPlaceOpponent(void)
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
        gCurTask->frameTable = gQuickDrawWaddleDooFrames;
        break;
    case 1:
        gCurTask->frameTable = gQuickDrawWheelieFrames;
        break;
    case 2:
        gCurTask->frameTable = gQuickDrawChefKawasakiFrames;
        break;
    case 3:
        gCurTask->frameTable = gQuickDrawKingDededeFrames;
        break;
    case 4:
        gCurTask->frameTable = gQuickDrawMetaKnightFrames;
        break;
    }
    gCurTask->tileWord = 0x8180;
    gCurTask->frame = 0;
    i = gCurTask->unk74 * 5 + gCurTask->unk18;
    gCurTask->unk1C = gQuickDrawOpponentReactionTimes[i];
}

void CreateQuickDrawOpponentTag(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
        s16 x = t->pixelX - 24;
        s16 y = t->pixelY - 32;
        struct Task *n;

        /* Dead `ldrsh` of t->unk48 in the ROM: a switch whose arms are all
           no-ops (same artifact as QuickDrawPlaceRankLabel/CreateQuickDrawPlayerTag/CreateQuickDrawWinCountLabel).
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

void QuickDrawPlaceOpponentStrike(u8 a0)
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

void QuickDrawOpponentSetKnockBack(void)
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
    CallTableEntry(gCurTask->state, 6, gQuickDrawOpponentStates);
    TaskSleepForever();
}

void QuickDrawOpponentUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 6, gQuickDrawOpponentStateUpdates);
}

void QuickDrawOpponentEnterState(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    CallTableEntry(t->state, 6, gQuickDrawOpponentStates);
}

void QuickDrawOpponentArrive(void)
{
    gCurTask->updateState = 0;
    QuickDrawOpponentStartSlideIn();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    CreateQuickDrawOpponentTag();
    TaskYieldTrampoline(gCurTask->unk20);
    gCurTask->state = 1;
    TaskSleepForever();
}

void QuickDrawOpponentArriveUpdate(void)
{
    if (gBrightness == 0 && gCurTask->unk24 == 0)
        QuickDrawOpponentSlideIn();
    if (gCurTask->state != 0)
        TaskSetEntry(QuickDrawOpponentEnterState, gCurTaskIdx);
}

void QuickDrawOpponentReady(void)
{
    gCurTask->updateState = 1;
    QuickDrawPlaceOpponent();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void QuickDrawOpponentReadyUpdate(void)
{
}

void QuickDrawOpponentStrike(void)
{
    gCurTask->updateState = 2;
    QuickDrawPlaceOpponentStrike(1);
    TaskSleepForever();
}

void QuickDrawOpponentStrikeUpdate(void)
{
}

void QuickDrawOpponentLose(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->updateState = 3;
    QuickDrawOpponentSetKnockBack();
    gUnk_0200B048 = ++gCurTask->unk18;
    TaskSleepForever();
}

void QuickDrawOpponentLoseUpdate(void)
{
    if (gCurTask->velX != 0 && gCurTask->velY != 0)
    {
        u8 r = QuickDrawIsTaskOnScreen();
        if (r == 0)
        {
            TaskStop();
            gCurTask->moveCallback = r;
        }
    }
}

void QuickDrawOpponentTie(void)
{
    gCurTask->updateState = 4;
    QuickDrawPlaceOpponentStrike(0);
    TaskSleepForever();
}

void QuickDrawOpponentTieUpdate(void)
{
}

void QuickDrawOpponentTag(void)
{
    struct Task *t;

    gCurTask->updateCallback = 0;
    gCurTask->layer = 4;
    switch (gUnk_0200B048)
    {
    case 0:
        gCurTask->frameTable = gQuickDrawWaddleDooFrames;
        break;
    case 1:
        gCurTask->frameTable = gQuickDrawWheelieFrames;
        break;
    case 2:
        gCurTask->frameTable = gQuickDrawChefKawasakiFrames;
        break;
    case 3:
        gCurTask->frameTable = gQuickDrawKingDededeFrames;
        break;
    case 4:
        gCurTask->frameTable = gQuickDrawMetaKnightFrames;
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

void QuickDrawOpponentTagUpdate(void)
{
}

void QuickDrawBonusSign(void)
{
    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 4;
    gCurTask->frameTable = gQuickDrawBonusFrames;
    gCurTask->tileWord |= 0x800;
    TaskSleepForever();
}

void QuickDrawGiveBonus(void)
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

void QuickDrawBonus(void)
{
    struct Task *t;

    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->layer = 4;
    t = gCurTask;
    t->frameTable = gQuickDrawBonusFrames;
    if (gPlayerCount != 1)
        t->frame = gQuickDrawRankBonuses[t->unk18];
    else
        t->frame = gQuickDrawDefeatBonuses[t->unk18];
    if (gCurTask->frame == 6 && gLocalPlayer == gCurTask->unk1C)
        PlaySfx(220);
    gCurTask->tileWord |= 0x800;
    QuickDrawGiveBonus();
    TaskSleepForever();
}

void QuickDrawRankLabelDraw(void)
{
    struct Task *t = gCurTask;
    u32 *p = t->frameTable;

    if (p != NULL && t->frame != -1)
    {
        if (t->pixelX >= -63 && t->pixelX <= 303 && t->pixelY > -64 && t->pixelY < 224)
            QueueSprite(t->layer, p[t->frame], t->spriteFlags, t->tileWord, t->pixelX, t->pixelY);
    }
}

void QuickDrawRankLabel(void)
{
    gCurTask->drawCallback = (u32)QuickDrawRankLabelDraw;
    gCurTask->layer = 4;
    gCurTask->frameTable = gQuickDrawRankFrames;
    gCurTask->tileWord |= 0x800;
    TaskSleepForever();
}
