
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"

/* RAM cells / ROM tables */
/* Not from room.h: this file's view of gCurSaveSlot differs (lesson 3.517). */
extern u8 gMetaKnightmareMode;
extern u8 gActivePlayerMask;
extern u32 gCurSaveSlot[];

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetHitReactions(u32 *p);
extern s16 ActorComputeHealth(void);
extern void sub_080689c8(s32 i, s32 d);
extern void sub_08068f68(void);
extern u32 ActorReactToHit(void);

void sub_0809fbd0(void)
{
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        if (((gActivePlayerMask >> i) & 1) != 0)
            gPlayerStates[i].unk42 |= 64;
    }
}

void sub_0809fc08(void)
{
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        if (((gActivePlayerMask >> i) & 1) != 0)
            gPlayerStates[i].unk42 &= 0xFFBF;
    }
}

void Task_KingDedede(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    sub_08066088(0);
    v = ActorComputeHealth();
    v = (v * 85) >> 8;
    t = gCurTask;
    t->unk70 = v;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)sub_08065438;
    t->layer = 11;
    gCurTask->frameTable = gKingDededeFrames;
    CallTableEntry(gCurTask->variant, 1, gKingDededeVariants);
}

void sub_0809fca4(void)
{
    sub_08068f68();
    ActorReactToHit();
}

void sub_0809fcb4(void)
{
    struct Task *t;
    s32 one;

    RequestScreenShake(4);
    TaskSetSkipMask(7, gCurTaskIdx);
    t = gCurTask;
    t->lateUpdateCallback = (u32)sub_080a0a84;
    gUnk_02006190[0] = t->pixelX;
    gUnk_02006190[1] = t->pixelY;
    gUnk_02006190[2] = t->frame;
    one = 1;
    gUnk_02006190[7] = one;
    TaskSetFrame(8);
    gUnk_02006190[3] = one;
    gUnk_02006190[4] = -2;
    gUnk_02006190[5] = 0;
    CreateStarFlash(1, 0, 0);
}

void sub_0809fd20(void)
{
    struct Task *t;
    s32 z;

    TaskSetSkipMask(0, gCurTaskIdx);
    t = gCurTask;
    z = 0;
    t->lateUpdateCallback = z;
    t->pixelX = gUnk_02006190[0];
    t->pixelY = gUnk_02006190[1];
    t->frame = gUnk_02006190[2];
    gUnk_02006190[7] = z;
    sub_08066468();
}

u8 sub_0809fd64(void)
{
    struct Task *t;
    s16 *p;

    ActorSetHitReactions(gUnk_08748974);
    TaskSetFrame(9);
    t = gCurTask;
    p = &t->unk46;
    if (*p != -1)
    {
        TaskFree(*p);
        gCurTask->unk46 = 0xFFFF;
    }
    if (gUnk_02007D00[8] != -1 && gUnk_02007D00[1] != -1)
    {
        sub_080689c8(gUnk_02007D00[8], -gCurTask->facing);
        gUnk_02007D00[8] = -1;
    }
    if (gUnk_02007D00[9] != -1)
    {
        StopSfxOnPlayer(gUnk_02007D00[9], 0x21B);
        gUnk_02007D00[9] = -1;
    }
    if (gMetaKnightmareMode == 1)
    {
        sub_0800a698();
        sub_080b7c00(gCurSaveSlot[0]);
    }
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

u8 sub_0809fe10(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct Actor *a;
    u8 s0;

    t = gCurTask;
    a = t->u8C.actor;
    s0 = t->state;
    sub_0809fcb4();
    switch (gCurTask->state)
    {
    case 3:
        u = gCurTask;
        if ((u->onGround & 1) == 0)
        {
            gUnk_02006190[2] = 43;
            ActorSetState(10);
            goto install;
        }
        else
        {
            u->unk2C = 1;
            gUnk_02006190[2] = 4;
            ActorSetState(1);
        }
        goto install;
    case 4:
        v = gCurTask;
        if ((v->onGround & 1) == 0)
        {
            if (gUnk_0300244C != 0)
                gUnk_02006190[2] = 43;
            ActorSetState(10);
            BLOCK_CROSS_JUMP
            goto install;
        }
        else
        {
            v->unk2C = 1;
            gUnk_02006190[2] = 4;
            ActorSetState(1);
        }
        BLOCK_CROSS_JUMP
        goto install;
    case 7:
        w = gCurTask;
        if (w->unk24 == 2)
        {
            if (gUnk_0300244C != 0)
                gUnk_02006190[2] = 43;
            ActorSetState(10);
            BLOCK_CROSS_JUMP
            goto install;
        }
        else
        {
            w->unk2C = 1;
            gUnk_02006190[2] = 4;
            ActorSetState(1);
        }
        BLOCK_CROSS_JUMP
        goto install;
    case 2:
        x = gCurTask;
        x->unk2C = 1;
        gUnk_02006190[2] = 4;
        ActorSetState(1);
        a->prevState = gCurTask->unk1C;
    install:
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
        break;
    case 8:
    case 9:
        TaskSetSkipMask(0, gCurTaskIdx);
        y = gCurTask;
        y->lateUpdateCallback = 0;
        y->frame = gUnk_02006190[2];
        gUnk_02006190[5] = 14;
        break;
    case 0:
        gCurTask->state = 1;
    case 1:
        z = gCurTask;
        z->unk2C = 1;
        gUnk_02006190[2] = 4;
        ActorSetState(1);
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
        break;
    case 5:
    case 6:
        break;
    }
    if (s0 == gCurTask->state)
        return 0;
    return 1;
}

void sub_0809ffec(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->hitKind == 6 && t->hitEffect == 4)
        gPlayerStates[t->hitterSlot].requestedAction = 18;
}

void sub_080a0028(void)
{
    struct ActorSpawn sp;

    TaskGetScreenPos();
    if (gUnk_030023B4 <= 127)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -1;
    TaskSetFrame(29);
    sp.subtype = 10;
    sp.taskType = 112;
    sp.variant = 0;
    sp.spawnArg = 0;
    sp.tileWord = 0;
    sp.x = 32;
    sp.y = 16;
    sp.checkTerrain = 0;
    CreateActorFromDescAtOffsetFacing(&sp, 1);
    PlaySfx(0x21D);
}

void sub_080a0098(void)
{
    struct Task **tp;
    s32 *p;
    s32 *q;
    s32 *r;
    s32 z;

    tp = &gCurTask;
    r = gUnk_02007D00;
    q = gUnk_02006040;
    z = 0;
    p = q + 9;
    do
    {
        *p = z;
        p--;
    } while ((s32)p >= (s32)q);
    (*tp)->unk20 = 1;
    (*tp)->unk34 = 90;
    r[0] = -1;
    (*tp)->unk46 = CreateChildTaskAtOffsetFacing(183, 38, 10, 0);
    r[9] = PlaySfx(0x21B);
}
