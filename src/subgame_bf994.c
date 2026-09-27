/* game_code_and_rodata_080653ec_0806ef5c 0x080BF994-0x080C0DE8
 * (issue #66, module M36 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BF994 0x080C0DE8 src/subgame_bf994.c --newpb
 *
 * The middle of M36: the four per-slot tasks and the geometry the whole
 * module places sprites with.  A slot task carries its own index in
 * Task.unk1C and the id of the task that spawned it in Task.parent, so
 * `&gTasks[t->unk44]` is the mode task whose Task.unk34 says whose
 * turn it is.
 *
 *   BombRallyPlayerAutoServe   slot-task entry: seat the sprite from the per-slot lists
 *       gUnk_0875674C / gUnk_0875675C (16.16 x/y) and gUnk_0875671C
 *       (animation), then branch on whether this slot has the turn.
 *   BombRallyPlayerAutoServeUpdate / BombRallyPlayerAutoWait / BombRallyPlayerAutoWaitUpdate   its three short states.
 *   BombRallyPlayerAutoThrow   the hand-off animation: six frames off gUnk_0875676C
 *       counted down in Task.unk30/unk34, then the position update through
 *       sub_080bdf3c.
 *   sub_080bfde8 / sub_080bfe64   the eliminated and winner states.
 *   sub_080bff28   the free-running driver: waits out the intro, then loops
 *       for ever re-reading the mode task's slot and, whenever it changes,
 *       re-arms the sprite and recomputes the hand-off speed as
 *       Div((gUnk_08756770[next] - gUnk_08756770[cur]) << 4, frames).
 *   sub_080c0074 / sub_080c0388 / sub_080c0540   the three placement bodies
 *       that walk a turn: sub_080c072c for the thrower, sub_080c061c for the
 *       projectile and sub_080c0a10 for its shadow.
 *   sub_080c05f0 / sub_080c0704   set a slot's sprite frame / animation.
 *   sub_080c061c   position on the 16.16 parabola p0 + v*t + (a*t*t)/2:
 *       p0 from gUnk_08756798 / gUnk_087567A0, v from gUnk_087567A8[c][b],
 *       and the two coefficient rows from gUnk_08756D3C[c][0..1].
 *   sub_080c0b18 / sub_080c0c58 / sub_080c0ca4 / sub_080c0d30   the
 *       remaining table-driven animation helpers.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "subgame.h"

/* Not from main.h: this file's view of gBgPalette differs (lesson 3.517). */
extern vu16 gFadeSteps;
extern u8 gObjPalette[];
extern vs32 gBg3ScrollX;
extern vs32 gBg3ScrollY;
extern vu16 gPlayerPressedKeys[];

extern vu16 gBgPalette[];
extern vu16 gDispCnt;

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BeginFastFadeInFromWhite(void);
extern void BeginFastFadeOutToWhite(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetFrame(s32 a);
extern void PlaySfx(u32 a);
extern void sub_080060c0(void);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void TaskStop(void);
extern u32 RandomRange(u32 range);
extern void TaskSleepForever(void);

extern void TaskStopY(void);
/* NOTE: sub_080bdebc's third parameter is `u16` at its definition
   (src/subgame_bda2c.c) but the ROM's call sites here sign-extend the
   argument, so the declaration visible here is the wider `s32` - the
   original source had the same prototype mismatch. */
extern void sub_080bdebc(s32 a, s32 b, s32 c, s32 d);

void BombRallyPlayerAutoServe(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateState = 10;
    v = gCurTask;
    v->posX = gUnk_0875674C[v->unk1C] << 16;
    v->posY = gUnk_0875675C[v->unk1C] << 16;
    v->frameTable = gUnk_0875671C[v->unk1C];
    v->tileWord = 0;
    if (v->unk1C == 3)
        v->facing = -1;
    else
        v->facing = 1;
    TaskSetFrame(0);
    w = gCurTask;
    u = &gTasks[w->parent];
    w->unk20 = 0;
    if (u->unk34 == w->unk1C) {
        TaskYieldTrampoline(30);
        u->unk28 = -1;
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(16);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        PlaySfx(254);
        z = gCurTask;
        sub_080bdf3c(z->posX + (gUnk_08756560[z->unk1C] << 16) * z->facing,
                     z->posY + (gUnk_08756564[z->unk1C] << 16),
                     z->unk1C, z->facing);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        u->unk28 = u->unk20;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    } else {
        while (u->unk28 < 0)
            TaskYieldTrampoline(1);
    }
    gCurTask->state = 11;
    TaskSleepForever();
}

void BombRallyPlayerAutoServeUpdate(void)
{
    if (gCurTask->state != 10)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerAutoWait(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateState = 11;
    while (1) {
        gCurTask->frame = 0;
        TaskYieldTrampoline(6);
        gCurTask->frame++;
        TaskYieldTrampoline(5);
        gCurTask->frame++;
        TaskYieldTrampoline(6);
        gCurTask->frame--;
        TaskYieldTrampoline(5);
    }
}

void BombRallyPlayerAutoWaitUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u->unk34 == ((t->unk1C + 3) & 3) && u->unk28 <= 2)
     || (u->unk34 == ((t->unk1C + 1) & 3) && u->unk28 > 2)) {
        gCurTask->state = 12;
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
    }
}

void BombRallyPlayerAutoThrow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;
    s32 m;
    struct Task **gp;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->updateState = 12;
    if (RandomRange(8) == 0) {
        if (u->unk28 <= 2)
            u->unk20 = u->unk28 + 3;
        else
            u->unk20 = u->unk28;
    } else {
        if (u->unk28 > 2)
            u->unk20 = u->unk28 - 3;
        else
            u->unk20 = u->unk28;
    }
    v = gCurTask;
    v->unk28 = u->unk20;
    m = u->unk24;
    v->unk30 = 6;
    v->unk34 = 0;
    gp = &gCurTask;
    v->unk2C = m - 8;
    if (v->unk2C != 0) {
        do {
        w = gCurTask;
        w->unk30--;
        if (w->unk30 == 0) {
            w->unk34++;
            if (w->unk34 == 4)
                w->unk34 = 0;
            x = gCurTask;
            x->frame = gUnk_0875676C[x->unk34];
            if (x->unk34 & 1)
                x->unk30 = 6;
            else
                x->unk30 = 5;
        }
        TaskYieldTrampoline(1);
        y = *gp;
        y->unk2C--;
        } while (y->unk2C != 0);
    }
    gCurTask->frame = 0;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    PlaySfx(254);
    z = gCurTask;
    sub_080bdf3c(z->posX + (gUnk_08756560[z->unk1C] << 16) * z->facing,
                 z->posY + (gUnk_08756564[z->unk1C] << 16),
                 z->unk1C, z->facing);
    gCurTask->frame++;
    TaskYieldTrampoline(7);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->state = 11;
    TaskSleepForever();
}

void BombRallyPlayerAutoThrowUpdate(void)
{
    if (gCurTask->state != 12)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void sub_080bfd80(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080bfdb0;
    t->state = 0;
    CallTableEntry(gCurTask->state, 3, gUnk_08756780);
    TaskSleepForever();
}

void sub_080bfdb0(void)
{
    CallTableEntry(gCurTask->updateState, 3, gUnk_0875678C);
}

void sub_080bfdcc(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_08756780);
}

void sub_080bfde8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->drawCallback = 0;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateState = 0;
    sub_080be010();
    v = gCurTask;
    v->unk30 = 0;
    v->unk24 = u->unk34;
    sub_080c05f0(v->unk24);
    sub_080c0704(gCurTask->unk18);
    TaskYieldTrampoline(30);
    sub_080c0b18(gCurTask->unk24);
    sub_080c0704(gCurTask->unk24);
    w = gCurTask;
    w->frameTable = 0;
    TaskYieldTrampoline(30);
    u->unk28 = -2;
    gCurTask->state = 1;
    TaskSleepForever();
}

void sub_080bfe64(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;
    s32 m;
    s32 k;

    t = gCurTask;
    n = t->unk30 + 1;
    t->unk30 = n;
    if (t->pixelX >= -63 && t->pixelX <= 303 && t->pixelY > -64 && t->pixelY <= 223
     && (n & 8)) {
        m = n & 7;
        k = 0;
        if (m > 2) {
            k = 2;
            if (m <= 4)
                k = 1;
        }
        QueueSprite(8, DrawAffineSprite(gUnk_08755E00[k],
                                    gUnk_08756770[gCurTask->unk24],
                                    gUnk_08756770[gCurTask->unk24], 0),
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->pixelX, gCurTask->pixelY);
    }
    if (gCurTask->state != 0)
        TaskSetEntry(sub_080bfdcc, gCurTaskIdx);
}

void sub_080bff28(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Task *p;
    struct Task *q;
    s32 d;
    s32 m;
    s32 msk;
    s32 n;
    s16 *tb;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->drawCallback = 0;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateState = 1;
    gCurTask->layer = 8;
    v = gCurTask;
    v->frameTable = gUnk_08755E00;
    v->unk28 = 0;
    v->unk2C = 0;
    v->unk30 = 0;
    v->unk34 = 0;
    v->unk24 = u->unk34;
    sub_080c0704(u->unk34);
    while (u->unk28 < -1)
        TaskYieldTrampoline(1);
    w = gCurTask;
    w->frame = (u->unk34 * 3) << 1;
    sub_080c05f0(u->unk34);
    x = gCurTask;
    x->velY = 0xFFFBC000;
    x->accelY = 128 << 7;
    x->unk28 = 1;
    while (u->unk28 < 0) {
        tb = gUnk_087567A0;
        if (gCurTask->posY > (tb[u->unk34] << 16)) {
            TaskStopY();
            sub_080c05f0(u->unk34);
        }
        TaskYieldTrampoline(1);
    }
    TaskStopY();
    y = gCurTask;
    y->unk24 = u->unk34 - 1;
    while (1) {
        z = gCurTask;
        tb = gUnk_08756770;
        u = &gTasks[z->parent];
        if (z->unk24 != u->unk34) {
            sub_080c05f0(u->unk34);
            sub_080c0704(u->unk34);
            p = gCurTask;
            m = u->unk34;
            p->unk24 = m;
            p->unk28 = 2;
            if (u->unk28 <= 2) {
                n = m + 1;
                msk = 3;
                d = n & msk;
            } else {
                n = m - 1;
                msk = 3;
                d = n & msk;
            }
            n = tb[d];
            n -= tb[m];
            gCurTask->unk2C = Div(n << 4, u->unk24);
        }
        TaskYieldTrampoline(1);
    }
}

void sub_080c0074(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *p;
    struct Task *q;
    s32 n;
    s32 st;
    s32 k;
    s32 m;
    s32 c;
    s32 x;
    s32 y;

    t = gCurTask;
    u = &gTasks[t->parent];
    n = t->unk30 + 1;
    t->unk30 = n;
    if (u->unk28 == 6) {
        t->state = 2;
        TaskSetEntry(sub_080bfdcc, gCurTaskIdx);
        return;
    }
    st = t->unk28;
    switch (st) {
    case 0:
        TaskStop();
        gCurTask->frame = 0xFFFF;
        break;
    case 3:
        if (n & 1)
            t->posX += 0x40000;
        else
            t->posX -= 0x40000;
        /* fall through */
    case 1:
        v = gCurTask;
        m = v->unk30 & 7;
        if (m <= 2)
            v->frame = 0;
        else if (m <= 4)
            v->frame = 1;
        else
            v->frame = 2;
        break;
    case 2:
        if (u->unk28 > 2) {
            k = u->unk28 - 3;
            sub_080c072c(u->unk34, k, u->unk2C, u->unk30, u->unk24, 1);
            sub_080c061c((u->unk34 + 3) & 3, k, u->unk2C, u->unk24 - u->unk30);
            sub_080c0a10((u->unk34 + 3) & 3, k, u->unk2C, u->unk24 - u->unk30,
                         (s16)(gUnk_08756778[gCurTask->unk24]
                               + ((gCurTask->unk2C * u->unk30) >> 5)));
            if (u->unk34 == 0 || u->unk34 == 3) {
                w = gCurTask;
                w->unk34 -= 4 << (st - k);
                if (w->unk34 < 0)
                    w->unk34 += 512;
            } else {
                w = gCurTask;
                w->unk34 += 4 << (st - k);
                if (w->unk34 > 511)
                    w->unk34 -= 512;
            }
        } else {
            k = u->unk28;
            sub_080c072c(u->unk34, k, u->unk2C, u->unk30, u->unk24, 0);
            sub_080c061c(u->unk34, k, u->unk2C, u->unk30);
            sub_080c0a10(u->unk34, k, u->unk2C, u->unk30,
                         (s16)(gUnk_08756778[gCurTask->unk24]
                               + ((gCurTask->unk2C * u->unk30) >> 5)));
            if (u->unk34 == 0 || u->unk34 == 3) {
                w = gCurTask;
                w->unk34 += 4 << (st - k);
                if (w->unk34 > 511)
                    w->unk34 -= 512;
            } else {
                w = gCurTask;
                w->unk34 -= 4 << (st - k);
                if (w->unk34 < 0)
                    w->unk34 += 512;
            }
        }
        p = gCurTask;
        m = p->unk30 & 7;
        if (m <= 2)
            p->frame = 0;
        else if (m <= 4)
            p->frame = 1;
        else
            p->frame = 2;
        if (u->unk30 == u->unk24)
            gCurTask->unk28 = 3;
        break;
    }
    q = gCurTask;
    if ((s16)q->frame != -1) {
        if ((q->unk30 & 3) == 0)
            sub_080bdebc(q->posX, q->posY,
                         (s16)(gUnk_08756770[q->unk24]
                               + ((q->unk2C * u->unk30) >> 4)),
                         q->unk34);
        x = (gCurTask->posX - gBg3ScrollX) >> 16;
        y = (gCurTask->posY - gBg3ScrollY) >> 16;
        if (x >= -63 && x <= 303 && y > -64 && y <= 223)
            QueueSprite(8,
                DrawAffineSprite(gUnk_08755E00[(s16)gCurTask->frame],
                    (s16)(gUnk_08756770[gCurTask->unk24]
                          + ((gCurTask->unk2C * u->unk30) >> 4)),
                    (s16)(gUnk_08756770[gCurTask->unk24]
                          + ((gCurTask->unk2C * u->unk30) >> 4)),
                    (s16)gCurTask->unk34),
                gCurTask->spriteFlags, gCurTask->tileWord, x, y);
    }
}

void sub_080c0388(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->drawCallback = (u32)TaskDrawScreen;
    t->updateState = 2;
    gBgPalette[0] = 0x7FFF;
    v = gCurTask;
    v->unk28 = u->unk34;
    v->unk2C = 0;
    v->unk30 = 0;
    sub_080c0704(v->unk28);
    gCurTask->layer = 6;
    TaskStop();
    w = gCurTask;
    w->posX = gUnk_0875672C[w->unk28] << 16;
    w->posY = (gUnk_08756734[w->unk28] + gUnk_0875673C[w->unk28]) << 16;
    PlaySfx(255);
    x = gCurTask;
    sub_080bdf3c(x->posX, x->posY, x->unk28, x->facing);
    y = gCurTask;
    if (y->unk28 == 0)
        y->frameTable = gUnk_08755E0C;
    else if (y->unk28 == 2)
        y->frameTable = gUnk_08755E7C;
    else
        y->frameTable = gUnk_08755E44;
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 12;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do {
        gCurTask->frame = 13;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskSleepForever();
}

void sub_080c0540(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->parent];
    n = sub_080c1ebc(t->unk28, t->unk2C);
    v = gCurTask;
    v->unk2C = n;
    if (v->unk30 <= 7) {
        if (((v->unk30 >> 1) & 1) == 0) {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x80 << 5;
        } else {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0xC0 << 5;
        }
        gCurTask->unk30++;
    }
    if (u->unk28 == -4) {
        gCurTask->state = 1;
        TaskSetEntry(sub_080bfdcc, gCurTaskIdx);
    }
}

void sub_080c05f0(u32 a)
{
    struct Task *t = gCurTask;

    t->posX = gUnk_08756798[a] << 16;
    t->posY = gUnk_087567A0[a] << 16;
}

void sub_080c061c(s32 a, s32 b, s32 c, s32 d)
{
    struct Task *t;
    struct Task *u;
    s32 *p1;
    s32 *p2;

    p1 = gUnk_08756D3C[c][0];
    p2 = gUnk_08756D3C[c][1];
    if (a == 0 || a == 3)
        gCurTask->posX = (gUnk_08756798[a] << 16) - gUnk_087567A8[c][b] * d;
    else
        gCurTask->posX = (gUnk_08756798[a] << 16) + gUnk_087567A8[c][b] * d;
    gCurTask->posY = (gUnk_087567A0[a] << 16) + p1[b * 4 + a] * d
             + ((p2[b * 4 + a] * d * d) >> 1);
}

void sub_080c0704(u32 a)
{
    gBg3ScrollX = gUnk_08756540[a];
    gBg3ScrollY = gUnk_08756550[a];
}

void sub_080c072c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    if (f == 0) {
        if (a == 0 || a == 3)
            gBg3ScrollX = gUnk_08756540[a] - gUnk_08756D74[c * 3 + b] * d
                          + ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        else
            gBg3ScrollX = gUnk_08756540[a] + gUnk_08756D74[c * 3 + b] * d
                          - ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        switch (b) {
        case 2:
            gBg3ScrollY = gUnk_08756550[a] + gUnk_08756F34[c][a] * d
                          + ((gUnk_08756FA4[c][a] * d * d) >> 1);
            break;
        case 1:
            gBg3ScrollY = gUnk_08756550[a] + gUnk_08756E54[c][a] * d
                          + ((gUnk_08756EC4[c][a] * d * d) >> 1);
            break;
        case 0:
            if (a <= 1)
                gBg3ScrollY = gUnk_08756550[a] - gUnk_08756E1C[c] * d
                              + ((gUnk_08756E38[c] * d * d) >> 1);
            else
                gBg3ScrollY = gUnk_08756550[a] + gUnk_08756E1C[c] * d
                              - ((gUnk_08756E38[c] * d * d) >> 1);
            break;
        }
    } else {
        if (a <= 1)
            gBg3ScrollX = gUnk_08756540[a] + gUnk_08756D74[c * 3 + b] * d
                          - ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        else
            gBg3ScrollX = gUnk_08756540[a] - gUnk_08756D74[c * 3 + b] * d
                          + ((gUnk_08756DC8[c * 3 + b] * d * d) >> 1);
        switch (b) {
        case 2:
            gBg3ScrollY = gUnk_08756550[(a + 3) & 3]
                          + gUnk_08756F34[c][(a + 3) & 3] * (e - d)
                          + ((gUnk_08756FA4[c][(a + 3) & 3] * (e - d) * (e - d)) >> 1);
            break;
        case 1:
            gBg3ScrollY = gUnk_08756550[(a + 3) & 3]
                          + gUnk_08756E54[c][(a + 3) & 3] * (e - d)
                          + ((gUnk_08756EC4[c][(a + 3) & 3] * (e - d) * (e - d)) >> 1);
            break;
        case 0:
            if (a == 0 || a == 3)
                gBg3ScrollY = gUnk_08756550[a] - gUnk_08756E1C[c] * d
                              + ((gUnk_08756E38[c] * d * d) >> 1);
            else
                gBg3ScrollY = gUnk_08756550[a] + gUnk_08756E1C[c] * d
                              - ((gUnk_08756E38[c] * d * d) >> 1);
            break;
        }
    }
}

void sub_080c0a10(s32 a, s32 b, s32 c, s32 d, s16 e)
{
    s32 x;
    s32 y;
    s32 *p;

    if ((s16)gCurTask->frame != -1) {
        p = gUnk_0875716C[b];
        if (a == 0 || a == 3)
            x = (gUnk_08756798[a] << 16) - gUnk_087567A8[c][b] * d;
        else
            x = (gUnk_08756798[a] << 16) + gUnk_087567A8[c][b] * d;
        y = (gUnk_08757014[a] << 16) + p[c * 4 + a] * d;
        QueueSprite(10, DrawAffineSprite(gUnk_08755EB4, (s16)e, (s16)e, 0), 0, 0,
                     (x >> 16) - (gBg3ScrollX >> 16),
                     (s16)((y >> 16) - (gBg3ScrollY >> 16)));
    }
}

void sub_080c0b18(u32 a)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    struct Task *z;

    t = gCurTask;
    t->unk1C = -gUnk_08757178[t->unk18][a];
    t->unk20 = -gUnk_087571B8[t->unk18][a];
    t->unk6C = 0;
    do {
        u = gCurTask;
        if ((s16)u->unk6C <= 16) {
            u->unk1C += gUnk_08757178[u->unk18][a];
            u->unk20 += gUnk_087571B8[u->unk18][a];
        } else {
            u->unk1C -= gUnk_08757178[u->unk18][a];
            u->unk20 -= gUnk_087571B8[u->unk18][a];
        }
        gBg3ScrollX += gCurTask->unk1C;
        gBg3ScrollY += gCurTask->unk20;
        if ((gCurTask->unk1C < 0 && gBg3ScrollX < gUnk_08756540[a])
         || (gCurTask->unk1C > 0 && gBg3ScrollX > gUnk_08756540[a]))
            gBg3ScrollX = gUnk_08756540[a];
        w = gCurTask;
        if ((w->unk20 < 0 && gBg3ScrollY < gUnk_08756550[a])
         || (w->unk20 > 0 && gBg3ScrollY > gUnk_08756550[a]))
            gBg3ScrollY = gUnk_08756550[a];
        TaskYieldTrampoline(1);
        z = gCurTask;
        z->unk6C++;
    } while ((s16)z->unk6C <= 31);
}

void sub_080c0c58(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = 0;
    t->moveCallback = (u32)TaskMove;
    t->updateCallback = (u32)sub_080c0ca4;
    t->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(6);
    TaskExitTrampoline();
}

void sub_080c0ca4(void)
{
    struct Task *t;
    struct Task *u;
    s32 x;
    s32 y;
    s32 n;

    t = gCurTask;
    n = t->unk1C + 8;
    t->unk1C = n;
    if (n > 255)
        t->unk1C = n & 255;
    u = gCurTask;
    x = (u->posX - gBg3ScrollX) >> 16;
    y = (u->posY - gBg3ScrollY) >> 16;
    if (x >= -63 && x <= 303 && y > -64 && y <= 223)
        QueueSprite(7, DrawAffineSprite(gUnk_08755EB8[(s16)u->frame], u->unk18,
                                     u->unk18, (s16)u->unk1C),
                     0, 0, x, y);
}

void sub_080c0d30(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)TaskDrawScreen;
    t->moveCallback = (u32)TaskMoveRelativeToBg3;
    t->updateCallback = 0;
    t->frameTable = gUnk_08755EC4;
    switch (t->unk74) {
    case 0:
        sub_080bdf9c(4);
        sub_080bdf9c(5);
        sub_080bdf9c(6);
        sub_080bdf9c(7);
        break;
    case 1:
    case 3:
        sub_080bdf9c(8);
        sub_080bdf9c(9);
        sub_080bdf9c(10);
        sub_080bdf9c(11);
        break;
    case 2:
        sub_080bdf9c(12);
        sub_080bdf9c(13);
        sub_080bdf9c(14);
        sub_080bdf9c(15);
        break;
    }
    u = gCurTask;
    u->state = u->unk74;
    CallTableEntry(gCurTask->state, 16, gUnk_087571F8);
    TaskSleepForever();
}
