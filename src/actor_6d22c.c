/* game_code_and_rodata 0x0806D22C-0x0806E0F0 (issue #64, module M18 batch 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806D22C 0x0806E0F0 src/actor_6d22c.c --newpb
 *
 * Class-1 task bodies for a family of scripted set-piece actors: the
 * three-stage entrance at Task_DustBurst (three TaskSetMotionXFacing sweeps with the
 * position recomputed from the parent task each time), the four short
 * animation-table players PlayRayBurstAnim/6e4/730/77c and their dispatch
 * wrappers Task_StarScatter/564/574/5a4/5b8/5cc, the CreateChildTaskHere spawner
 * helpers CreateBurstEffect/d928/da3c, the eight-way "carried" body
 * Task_ImpactStar, the gTasks[].unk73-keyed body Task_CannonSmoke (with its
 * per-frame mover CannonSmokeInit), the two-sprite draw callback CannonFuseSparkDraw,
 * and the two random-walk bodies Task_CannonFuseSpark and Task_HitFrost.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "actor.h"
#include "enemy.h"

extern s32 RandomRange(s32 a);
extern s32 TaskIsOnScreen(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern s32 RandomSpread(s32 a, s32 b, s32 c);

void Task_DustBurst(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gDustFrames;
    u->updateCallback = (u32)DustBurstCheckParent;
    TaskFaceLikeParent();

    v = gCurTask;
    v->posX = (gTasks[v->parent].pixelX + -v->facing * v->dustBurstOffsetX) << 16;
    v->posY = (gTasks[v->parent].pixelY + v->dustBurstOffsetY) << 16;
    TaskSetMotionXFacing(0xFFFD0000, 0);
    w = gCurTask;
    w->velY = 0;
    w->accelY = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    CreateDustPuff();
    TaskSetFrameByFacing(4);
    TaskYieldTrampoline(1);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(1);
    TaskStop();

    x = gCurTask;
    x->posX = (gTasks[x->parent].pixelX + -x->facing * x->dustBurstOffsetX) << 16;
    x->posY = (gTasks[x->parent].pixelY + x->dustBurstOffsetY) << 16;
    TaskSetMotionXFacing(0xFFFDC000, 0x1000);
    w = gCurTask;
    w->velY = -0x4000;
    w->accelY = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskStop();

    y = gCurTask;
    y->posX = (gTasks[y->parent].pixelX + -y->facing * y->dustBurstOffsetX) << 16;
    y->posY = (gTasks[y->parent].pixelY + y->dustBurstOffsetY) << 16;
    TaskSetMotionXFacing(0xFFFEE000, 0x1800);
    w = gCurTask;
    w->velY = -0x4000;
    w->accelY = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    TaskSetFrameByFacing(6);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void DustBurstCheckParent(void)
{
    if (gTaskSlotTypes[gCurTask->parent] == -1
     || TaskHasSameSerial(gCurTask->parent) != 1)
        TaskFree(gCurTaskIdx);
}

void CreateBurstEffect(u32 a, s32 b)
{
    s32 i;
    struct Task *p;

    switch (a)
    {
    case 1:
        i = CreateChildTaskHere(TASK_STAR_SCATTER, 0);
        break;
    case 0:
        i = CreateChildTaskHere(TASK_RAY_BURST, 0);
        break;
    case 2:
        i = CreateChildTaskHere(TASK_SMALL_BLAST, 0);
        break;
    case 4:
        i = CreateChildTaskHere(TASK_STAR_SCATTER_ON_PARENT, 0);
        break;
    case 3:
        i = CreateChildTaskHere(TASK_RAY_BURST_ON_PARENT, 0);
        break;
    case 5:
        i = CreateChildTaskHere(TASK_SMALL_BLAST_ON_PARENT, 0);
        break;
    }
    if (i != -1 && b > 0)
    {
        p = &gTasks[i];
        p->burstStickFrames = b;
    }
}

void Task_StarScatter(void)
{
    PlayStarScatterAnim();
    TaskExitTrampoline();
}

void Task_RayBurst(void)
{
    PlayRayBurstAnim();
    TaskExitTrampoline();
}

void Task_SmallBlast(void)
{
    PlaySmallBlastAnim();
    TaskExitTrampoline();
}

void BurstStickToParent(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->posY = 0;
    t->posX = 0;
    t->updateCallback = (u32)BurstStickToParentUpdate;
}

void Task_StarScatterOnParent(void)
{
    BurstStickToParent();
    PlayStarScatterAnim();
    TaskExitTrampoline();
}

void Task_RayBurstOnParent(void)
{
    BurstStickToParent();
    PlayRayBurstAnim();
    TaskExitTrampoline();
}

void Task_SmallBlastOnParent(void)
{
    BurstStickToParent();
    PlaySmallBlastAnim();
    TaskExitTrampoline();
}

void BurstStickToParentUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (--t->burstStickFrames <= 0)
    {
        t->updateCallback = 0;
        t->moveCallback = (u32)ActorMove;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
    }
    else
    {
        if (gTaskSlotTypes[t->parent] == -1 || TaskHasSameSerial(t->parent) != 1)
            TaskFree(gCurTaskIdx);
    }
}

void PlayRayBurstAnim(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gRayBurstFrames;
    t->layer = 10;
    u = gCurTask;
    u->tileWord = 0;
    u->frame = 0;
    TaskYieldTrampoline(2);
    for (i = 0; i < 10; i++)
    {
        gCurTask->frame = gUnk_0873E620[i];
        TaskYieldTrampoline(1);
    }
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(4);
    for (i = 0; i < 6; i++)
    {
        gCurTask->frame = gUnk_0873E634[i];
        TaskYieldTrampoline(1);
    }
}

void PlayStarScatterAnim(void)
{
    struct Task *t;
    s32 i;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gStarScatterFrames;
    t->layer = 10;
    gCurTask->tileWord = 0;
    for (i = 0; i < 23; i++)
    {
        gCurTask->frame = gUnk_0873E700[i];
        TaskYieldTrampoline(1);
    }
}

void PlayExplosionAnim(void)
{
    struct Task *t;
    s32 i;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gExplosionFrames;
    t->layer = 4;
    gCurTask->tileWord = 0;
    for (i = 0; i < 23; i++)
    {
        gCurTask->frame = gExplosionAnimFrames[i];
        TaskYieldTrampoline(1);
    }
}

void PlaySmallBlastAnim(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gUnk_0874CAD8;
    t->layer = 10;
    u = gCurTask;
    u->tileWord = 0;
    u->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
}

void Task_ImpactStar(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 k;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 8;
    gCurTask->frameTable = gUnk_0874C500;
    n = RandomRange(8);
    u = gCurTask;
    u->impactStarDir = n;
    u->posX = (u->pixelX + gUnk_0873EB40[n]) << 16;
    k = 4;
    u->posY = (u->pixelY + ((gUnk_0873EB40 + 8)[n] + k)) << 16;
    a = gUnk_0873EB60[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    u->velX = b;
    a = (gUnk_0873EB60 + 8)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    u->velY = b;
    u->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    v = gCurTask;
    a = gUnk_0873EB80[v->impactStarDir];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->velX = b;
    a = (gUnk_0873EB80 + 8)[v->impactStarDir];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->velY = b;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->velX = 0;
    w->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void CreateStarRing(void)
{
    s32 i;
    s32 ringStarSlot;
    struct Task *p;

    for (i = 0; i < 8; i++)
    {
        ringStarSlot = CreateChildTaskHere(TASK_RING_STAR, 0);
        if (ringStarSlot != -1)
        {
            p = &gTasks[ringStarSlot];
            p->variant = i;
        }
    }
}

void Task_RingStar(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 i;
    s32 k;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gRingStarFrames;
    u->updateCallback = (u32)RingStarUpdate;
    u->ringStarFrameTimer = 2;
    u->frame = 0;
    k = u->variant * 4;
    for (i = 0; i < 4; i++)
    {
        v = gCurTask;
        v->velX = gUnk_0873EBA0[k + i];
        v->velY = gUnk_0873EC20[k + i];
        TaskYieldTrampoline(6);
    }
    TaskSleepForever();
}

void RingStarUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->ringStarFrameTimer <= 0)
    {
        if (t->frame > 7)
        {
            ActorDestroy();
        }
        else
        {
            t->frame++;
            t->ringStarFrameTimer = 2;
        }
    }
    else
    {
        t->ringStarFrameTimer--;
    }
}

void Task_BossScreenFlash(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = 0;
    BossDefeatScreenFlash();
    TaskExitTrampoline();
}

void Task_ExplosionScreenFlash(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = 0;
    ExplosionScreenFlash();
    TaskExitTrampoline();
}

void CreateCannonSmoke(u32 a, u32 b)
{
    s32 cannonSmokeSlot;
    struct Task *cannonSmoke;

    cannonSmokeSlot = CreateChildTaskHere(TASK_CANNON_SMOKE, 0);
    if (cannonSmokeSlot != -1)
    {
        cannonSmoke = &gTasks[cannonSmokeSlot];
        cannonSmoke->variant = a;
        cannonSmoke->cannonSmokeSpot = b;
    }
}

void CannonSmokeInit(void)
{
    struct Task *t;
    s32 m;
    s32 j;

    t = gCurTask;
    if (t->variant != 0)
    {
        m = t->cannonSmokeSpot;
        j = m * 2;
        t->pixelX += gUnk_0873ECA0[m * 2];
        t->pixelY += gUnk_0873ECA0[j + 1];
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (t->variant == 1)
        {
            t->velX = gUnk_0873ECC0[m * 2];
            t->velY = gUnk_0873ECC0[j + 1];
        }
    }
}

void Task_CannonSmoke(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gCannonSmokeFrames;
    t->layer = 10;
    gCurTask->tileWord = 0;
    CannonSmokeInit();
    u = gCurTask;
    switch (u->variant)
    {
    case 0:
        u->frame = 8;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    case 1:
        u->cannonSmokePuffCount = 0;
        do
        {
            gCurTask->frame = 0;
            TaskYieldTrampoline(1);
            gCurTask->cannonSmokeLoopCount = 0;
            do
            {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while (++*(s16 *)&gCurTask->cannonSmokeLoopCount <= 6);
        } while (++*(s16 *)&gCurTask->cannonSmokePuffCount <= 8);
        break;
    case 2:
        u->frame = 33;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame = 15;
        TaskYieldTrampoline(2);
        gCurTask->cannonSmokeLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->cannonSmokeLoopCount <= 5);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    case 3:
        u->frame = 24;
        TaskYieldTrampoline(2);
        gCurTask->cannonSmokeLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        } while (++*(s16 *)&gCurTask->cannonSmokeLoopCount <= 5);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void CannonFuseSparkDraw(void)
{
    struct Task *p;
    struct Task *t;
    struct Task *u;
    u32 *tbl;

    p = gCurTask;
    if (p->frameTable == NULL)
        return;
    if (p->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    t = gCurTask;
    tbl = t->frameTable;
    QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord,
                 t->pixelX - gSpriteCameraX,
                 (s16)(t->pixelY - gSpriteCameraY));
    u = gCurTask;
    QueueSprite(u->layer - 1, tbl[7], u->spriteFlags, u->tileWord,
                 u->pixelX + u->cannonFuseSparkOffsetX - gSpriteCameraX,
                 (s16)(u->pixelY + u->cannonFuseSparkOffsetY - gSpriteCameraY));
}

void Task_CannonFuseSpark(void)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)CannonFuseSparkDraw;
    t->frameTable = gCannonFuseSparkFrames;
    t->layer = 10;
    u = gCurTask;
    u->updateCallback = (u32)CannonFuseSparkCheckParent;
    u->tileWord = 0;
    u->cannonFuseSparkOffsetX = 0;
    u->cannonFuseSparkOffsetY = 0;
    while (1)
    {
        for (i = 0; i < 8; i++)
        {
            gCurTask->frame = gUnk_0873ECD0[i];
            TaskYieldTrampoline(3);
            gCurTask->cannonFuseSparkOffsetX = (u16)RandomSpread(-12, 1, 24);
            gCurTask->cannonFuseSparkOffsetY = (u16)RandomSpread(-12, 1, 24);
        }
    }
}

void CannonFuseSparkCheckParent(void)
{
    if (gTaskSlotTypes[gCurTask->parent] == -1
     || TaskHasSameSerial(gCurTask->parent) != 1)
        TaskFree(gCurTaskIdx);
}

void Task_TrailFlash(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskUpdatePixelPos;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gTrailFlashFrames;
    t->layer = 10;
    u = gCurTask;
    u->tileWord = 0;
    u->posX = gTasks[u->parent].posX;
    u->posY = gTasks[u->parent].posY;
    u->frame = 0;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void ActorUpdateAttachedEffect(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    if (a->attachedTask == -1)
        return;
    if (*(s16 *)&a->attachedTaskLifetime == -2)
        return;
    if (*(s16 *)&a->attachedTaskLifetime <= 0)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
        a->attachedTaskLifetime = 0xFFFE;
    }
    a->attachedTaskLifetime--;
}

void ActorAttachEffect(s32 a, s32 b)
{
    struct Actor *p;
    s32 c;

    p = gCurTask->u8C.actor;
    if (p->attachedTask != -1)
    {
        TaskFree(p->attachedTask);
        p->attachedTask = 0xFFFF;
    }
    switch (a)
    {
    case 1:
        c = CreateChildTaskHere(TASK_HIT_FLAMES, 0);
        break;
    case 2:
        c = CreateChildTaskHere(TASK_HIT_SPARKS, 0);
        break;
    case 3:
        c = CreateChildTaskHere(TASK_HIT_FROST, 0);
        break;
    case 0:
    default:
        c = -1;
        break;
    }
    p->attachedTask = c;
    if (b == 1)
        p->attachedTaskLifetime = 60;
    else
        p->attachedTaskLifetime = 0xFFFE;
}

void Task_HitFrost(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->frameTable = gHitFrostFrames;
    t->layer = 6;
    u = gCurTask;
    u->updateCallback = (u32)HitFrostCheckParent;
    u->tileWord = 0;
    u->hitFrostCycleCount = 0;
    do
    {
        gCurTask->posX = RandomSpread(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        TaskSetMotion(0x4000, 0xFFFFF900, 0x5A5A5A5A, 0xFFFFC000, 0xFFFFF000,
                     0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->hitFrostLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        } while (++*(s16 *)&gCurTask->hitFrostLoopCount <= 4);
        gCurTask->posX = RandomSpread(-12, 1, 24) << 16;
        gCurTask->posY = RandomSpread(-12, 1, 24) << 16;
        TaskSetMotion(0xFFFFC000, 0x700, 0x5A5A5A5A, 0xFFFFC000, 0xFFFFF000,
                     0x5A5A5A5A);
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
        gCurTask->hitFrostLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(3);
        } while (++*(s16 *)&gCurTask->hitFrostLoopCount <= 4);
    } while (++*(s16 *)&gCurTask->hitFrostCycleCount <= 1);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}
