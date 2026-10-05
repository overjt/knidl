/* game_code_and_rodata 0x0806A344-0x0806AD18 (issue #64, module M18 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806A344 0x0806AD18 src/actor_defeat.c --newpb
 *
 * Actor defeat bodies, the variants ActorDie (gActorDefeats) runs (an older
 * reading called them warp-star exits, a level-clear dance and a death
 * sequence): ActorDefeatByEffect dispatches the hit effect code Task.hitEffect
 * through gActorDefeatsByEffect to the knock-away defeats
 * ActorDefeatPlain/Burning/Shocked (the shake and launch
 * ActorDefeatKnockAway, then ActorDefeatBlinkAndBurst) and to
 * ActorDefeatFrozen, which turns the actor into a kickable ice block
 * (ActorFreezeIntoIceBlock; per-frame ActorDefeatFrozenUpdate; states gActorDefeatFrozenStates:
 * ActorDefeatFrozenShake shakes, ActorDefeatFrozenSlide slides away when kicked), and a family
 * of one-shot bodies that re-arm the actor and hand control to
 * PlayRayBurstAnim / PlayExplosionAnim.  Every function here runs as gCurTask (the current
 * task), so almost all of them are a run of `gCurTask->field = K`
 * statements interleaved with TaskYieldTrampoline() waits.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "link.h"
#include "actor.h"

/* Not from main.h: this file's view of gBgPalette differs (lesson 3.517). */
extern u16 gBgPalette;
extern vu16 gDispCnt;

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskStop(void);
extern void TaskSetFrame(s32 a);
extern void ActorSetState(u8 v);
extern void ActorLoadDef(u32 def);
extern void TaskSetEntry(void *fn, s32 i);
extern u32 RandomRange(s32 a);
extern void RegisterCollider(u8 a, s16 b, s16 c, void *d);
extern void ActorCheckHits(void);
extern s32 ActorReactToHit(void);
extern void TaskSleepForever(void);
extern void PlaySfx(s32 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void LoadBackdropColor(u16 *p);
extern void RequestScreenShake(s32 a);
extern void AngleToVector(s32 a, s32 b);

void ActorDie(void)
{
    struct Task *t;
    struct Actor *a;
    s8 *p;

    a = gCurTask->u8C.actor;
    p = (s8 *)a->hitReactions;
    a->hitState = 2;
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSetFrame(0);
    if (p != NULL)
        CallTableEntry(p[1], 11, gActorDefeats);
    if (a->attachedTask != -1)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
    }
    ActorDestroy();
}

void ActorDefeatByEffect(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->hitEffect > 3)
        t->hitEffect = 0;
    CallTableEntry(gCurTask->hitEffect, 4, gActorDefeatsByEffect);
}

void ActorDefeatKnockAway(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Actor *a;
    s32 i;
    u16 *p;

    TaskStop();
    TaskSetFrame(0);
    gCurTask->onGround = 0;
    t = gCurTask;
    a = t->u8C.actor;
    a->savedPaletteBits = t->tileWord & 0xF000;
    t->actorKnockAwayBaseY = t->pixelY;
    t->actorDefeatBlinkTimer = 14;
    t->actorBurstDelay = 14;
    t->actorBurstDone = 0;
    p = gUnk_0873E610;
    for (i = 0; i < 8; i++)
    {
        u = gCurTask;
        u->pixelY = u->actorKnockAwayBaseY + p[i];
        u->posY = u->pixelY << 16;
        TaskYieldTrampoline(1);
    }
    v = gCurTask;
    v->posY = v->actorKnockAwayBaseY << 16;
    AngleToVector(TaskGetHitAngle(), 512);
    w = gCurTask;
    w->velX = gUnk_030023B4;
    w->velY = gUnk_030023D4;
    TaskYieldTrampoline(12);
}

void ActorDefeatBlinkAndBurst(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->actorDefeatBlinkTimer > 0)
    {
        t->actorDefeatBlinkTimer--;
        if ((t->actorDefeatBlinkTimer & 1) == 0)
            t->tileWord = (t->tileWord & 0xFFF) | 0xF000;
        else
            t->tileWord = (t->tileWord & 0xFFF) | t->u8C.actor->savedPaletteBits;
    }
    u = gCurTask;
    if (u->actorBurstDelay <= 0)
    {
        if (u->actorBurstDone == 0)
        {
            CreateBurstEffect(3, 6);
            gCurTask->actorBurstDone = 1;
        }
    }
    else
    {
        u->actorBurstDelay--;
    }
}

void ActorDefeatPlain(void)
{
    gCurTask->updateCallback = (u32)ActorDefeatPlainUpdate;
    ActorDefeatKnockAway();
    TaskStop();
    ActorPlayRandomDefeatSfx();
}

void ActorDefeatPlainUpdate(void)
{
    ActorDefeatBlinkAndBurst();
}

void ActorDefeatBurning(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)ActorDefeatBurningUpdate;
    ActorAttachEffect(t->hitEffect, 0);
    ActorDefeatKnockAway();
    TaskStop();
    ActorPlayRandomDefeatSfx();
}

void ActorDefeatBurningUpdate(void)
{
    ActorDefeatBlinkAndBurst();
}

void ActorDefeatShocked(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)ActorDefeatShockedUpdate;
    ActorAttachEffect(t->hitEffect, 0);
    ActorDefeatKnockAway();
    TaskStop();
    ActorPlayRandomDefeatSfx();
}

void ActorDefeatShockedUpdate(void)
{
    ActorDefeatBlinkAndBurst();
}

void ActorDefeatFrozenBlink(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    s32 n;
    u32 a;
    s16 *tbl;

    t = gCurTask;
    i = t->actorFreezeBlinkStep >> 1;
    if (i <= 5)
    {
        if (t->actorFreezeBlinkTimer <= 0)
        {
            t->actorFreezeBlinkStep = t->actorFreezeBlinkStep + 1;
            if (t->actorFreezeBlinkShown != 0)
                t->frame = 0;
            else
                t->frame = 0xFFFF;
            u = gCurTask;
            tbl = gIceBlockBlinkTimes;
            n = u->actorFreezeBlinkShown;
            a = n << 1;
            a += i << 2;
            a += (u32)tbl;
            u->actorFreezeBlinkTimer = *(s16 *)a;
            n ^= 1;
            u->actorFreezeBlinkShown = n;
        }
        v = gCurTask;
        v->actorFreezeBlinkTimer--;
    }
    else
    {
        t->frameTable = gIceBlockFrames;
        t->tileWord = 0;
        if (gUnk_0300244C != 0)
            t->u8C.actor->savedPaletteBits = 0;
        w = gCurTask;
        if (w->frame == -1)
            w->frame = 4;
    }
}

void ActorFreezeIntoIceBlock(void)
{
    struct Task *t;
    struct Actor *a;
    void (*fn)(void);
    s32 v;
    s32 z;
    u8 zero;

    a = gCurTask->u8C.actor;
    v = CreateChildTaskHere(TASK_ICE_BLOCK, 0);
    t = gCurTask;
    t->actorIceBlockSlot = v;
    z = 0;
    zero = 0;
    t->unk18 = z;
    t->actorFreezeBlinkShown = z;
    t->actorFreezeBlinkTimer = z;
    t->actorFreezeBlinkStep = z;
    t->actorFreezerPlayer = a->hitterParent;
    ActorAttachEffect(3, 0);
    gCurTask->u80.attackAbility = zero;
    gCurTask->onGround = zero;
    ActorSetState(ACTOR_DEFEAT_FROZEN_STATE_SHAKE);
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    fn = a->teardown;
    ActorLoadDef((u32)&gUnk_0873F6BC);
    a->teardown = fn;
}

void ActorDefeatFrozenCheckFreezerAbility(void)
{
    if ((u8)(gPlayerStates[gCurTask->actorFreezerPlayer].ability - 13) > 1)
    {
        ActorSetState(ACTOR_DEFEAT_FROZEN_STATE_BURST);
        TaskSetEntry(ActorDefeatFrozenEnterState, gCurTaskIdx);
    }
}

void ActorDefeatFrozenCheckPush(void)
{
    struct Task *t;
    struct Task *u;
    struct Task **g;
    s8 *pb;
    u32 addr;
    u32 h;
    s32 one;
    u8 v;

    g = &gCurTask;
    t = *g;
    if ((s8)t->hitKind != 7)
        return;
    pb = &t->hitterSlot;
    addr = *pb * 144;
    addr += (u32)gTasks;
    addr += 118;
    h = *(u16 *)addr;
    one = 1;
    h |= one;
    *(u16 *)addr = h;
    v = TaskGetXDirBitTo(*pb);
    switch (v)
    {
    case 8:
        goto set_one;
    case 0:
        if (RandomRange(2) != 0)
            goto set_one;
    case 4:
        (*g)->facing = -1;
        break;
    set_one:
        (*g)->facing = one;
        break;
    }
    u = gCurTask;
    u->parent = u->hitterSlot;
    u->player = &gPlayerStates[u->parent];
    ActorSetState(ACTOR_DEFEAT_FROZEN_STATE_SLIDE);
    TaskSetEntry(ActorDefeatFrozenEnterState, gCurTaskIdx);
}

void ActorDefeatFrozen(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = 0;
    t->actorIceBlockSlot = 0xFFFF;
    TaskSetFrame(0);
    TaskYieldTrampoline(6);
    gCurTask->updateCallback = (u32)ActorDefeatFrozenUpdate;
    TaskStop();
    gCurTask->layer = 7;
    ActorFreezeIntoIceBlock();
    CallTableEntry(gCurTask->state, 3, gActorDefeatFrozenStates);
}

void ActorDefeatFrozenUpdate(void)
{
    struct Task *t;
    struct Actor *a;
    s32 r;

    gCurTask->onGround = 0;
    CallTableEntry(gCurTask->updateState, 3, gActorDefeatFrozenStateUpdates);
    ActorDefeatFrozenBlink();
    t = gCurTask;
    if (t->state == ACTOR_DEFEAT_FROZEN_STATE_SLIDE)
    {
        if ((s8)t->hitKind != HIT_KIND_NONE)
        {
            ActorSetState(ACTOR_DEFEAT_FROZEN_STATE_BURST);
            TaskSetEntry(ActorDefeatFrozenEnterState, gCurTaskIdx);
        }
        else
        {
            RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F880);
        }
    }
    else if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        ActorDefeatFrozenCheckPush();
        r = ActorReactToHit();
        if (r == 0)
        {
            gCurTask->hitKind = r;
            if (gCurTask->state != ACTOR_DEFEAT_FROZEN_STATE_BURST)
                ActorDefeatFrozenCheckFreezerAbility();
        }
        else
        {
            a = gCurTask->u8C.actor;
            if (a->attachedTask != -1)
            {
                TaskFree(a->attachedTask);
                a->attachedTask = 0xFFFF;
            }
        }
    }
}

void ActorDefeatFrozenEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gActorDefeatFrozenStates);
}

void ActorDefeatFrozenShake(void)
{
    struct Task *t;

    gCurTask->updateState = ACTOR_DEFEAT_FROZEN_STATE_SHAKE;
    gCurTask->actorLoopCount = 0;
    do
    {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->actorLoopCount = t->actorLoopCount + 1;
    } while ((s16)t->actorLoopCount <= 7);
    gCurTask->velY = 0;
    TaskYieldTrampoline(120);
    ActorSetState(ACTOR_DEFEAT_FROZEN_STATE_BURST);
    TaskSleepForever();
}

void ActorDefeatFrozenShakeUpdate(void)
{
    if (gCurTask->state != ACTOR_DEFEAT_FROZEN_STATE_SHAKE)
        TaskSetEntry(ActorDefeatFrozenEnterState, gCurTaskIdx);
}

void ActorDefeatFrozenSlide(void)
{
    struct Task *t;
    struct Actor *a;
    u8 one;

    t = gCurTask;
    a = t->u8C.actor;
    one = 1;
    t->updateState = one;
    gCurTask->taskClass = one;
    if (a->attachedTask != -1)
    {
        TaskFree(a->attachedTask);
        a->attachedTask = 0xFFFF;
    }
    TaskStop();
    PlaySfx(229);
    TaskSetMotionXFacing(0x40000, 0x5A5A5A5A);
    TaskSleepForever();
}

void ActorDefeatFrozenSlideUpdate(void)
{
}

void ActorDefeatFrozenBurst(void)
{
    struct Task *t;
    struct Task *u;
    s32 zero;

    t = gCurTask;
    zero = 0;
    t->updateCallback = zero;
    PlaySfx(242);
    TaskStop();
    u = gCurTask;
    u->frameTable = gRayBurstFrames;
    u->tileWord = zero;
    PlayRayBurstAnim();
    ActorDestroy();
}

void sub_0806aa0c(void)
{
}

void ActorDefeatExplodeByEffect(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->hitEffect > 3)
        t->hitEffect = 0;
    CallTableEntry(gCurTask->hitEffect, 4, gActorExplodeDefeatsByEffect);
}

void ActorDefeatExplode(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_ENEMY)
    {
        t->updateCallback = 0;
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
    }
    gCurTask->health = 127;
    ActorPlaySfx(189, 0);
    PlayExplosionAnim();
}

void ActorExplodeDefeatBurning(void)
{
    ActorDefeatExplode();
}

void ActorExplodeDefeatShocked(void)
{
    ActorDefeatExplode();
}

void ActorExplodeDefeatFrozen(void)
{
    ActorDefeatFrozen();
}

void ExplosionScreenFlash(void)
{
    struct Task *t;
    u16 x;

    x = gBgPalette;
    gCurTask->explosionScreenFlashLoopCount = 0;
    do
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1000;
        LoadBackdropColor(gUnk_0873E698);
        TaskYieldTrampoline(3);
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        LoadBackdropColor(&x);
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->explosionScreenFlashLoopCount = t->explosionScreenFlashLoopCount + 1;
    } while ((s16)t->explosionScreenFlashLoopCount <= 2);
    LoadBackdropColor(&x);
}

void ActorDefeat2(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s32 zero;
    u8 zb;

    t = gCurTask;
    a = t->u8C.actor;
    zero = 0;
    t->updateCallback = zero;
    t->player = (struct PlayerState *)zero;
    TaskSetFrame(0);
    TaskYieldTrampoline(1);
    gCurTask->taskClass = 1;
    u = gCurTask;
    zb = 0;
    u->health = 127;
    u->updateCallback = (u32)ActorDefeat2Update;
    a->attackBox = (u32)&gUnk_0873F6E8;
    u->u80.attackAbility = zb;
    v = gCurTask;
    v->actorExplosionTimer = zero;
    CreateChildTaskHere(TASK_EXPLOSION_SCREEN_FLASH, 1);
    RequestScreenShake(2);
    ActorPlaySfx(189, 0);
    PlayExplosionAnim();
}

void ActorDefeat2Update(void)
{
    struct Task *u;

    if (gCurTask->actorExplosionTimer <= 15)
    {
        ActorCheckHits();
        RegisterCollider((u8)gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873F81C);
        u = gCurTask;
        u->actorExplosionTimer = u->actorExplosionTimer + 1;
    }
}

void ActorDefeat3(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->u8C.actor;
    zero = 0;
    t->updateCallback = zero;
    TaskSetFrame(0);
    TaskYieldTrampoline(1);
    gCurTask->taskClass = 1;
    u = gCurTask;
    u->health = 127;
    u->updateCallback = (u32)ActorDefeat3Update;
    a->attackBox = (u32)&gUnk_0873F704;
    u->actorExplosionTimer = zero;
    RequestScreenShake(2);
    ActorPlaySfx(189, 0);
    PlayExplosionAnim();
}

void ActorDefeat3Update(void)
{
    struct Task *u;

    if (gCurTask->actorExplosionTimer <= 15)
    {
        ActorCheckHits();
        u = gCurTask;
        u->actorExplosionTimer = u->actorExplosionTimer + 1;
    }
}

void ActorDefeat4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_ENEMY)
    {
        t->updateCallback = 0;
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
    }
    u = gCurTask;
    u->health = 127;
    zero = 0;
    u->updateCallback = (u32)ActorDefeat4Update;
    ActorPlaySfx(109, 0);
    v = gCurTask;
    v->frameTable = gRayBurstFrames;
    v->tileWord = zero;
    PlayRayBurstAnim();
}

void ActorDefeat4Update(void)
{
}

void ActorDefeatAbilityStar(void)
{
    struct Task *t;

    TaskStop();
    TaskSetFrame(0);
    t = gCurTask;
    t->frameTable = gRayBurstFrames;
    t->tileWord = 0;
    PlaySfx(125);
    PlayRayBurstAnim();
}

u32 ActorHasExtraFrame(void)
{
    if (gCurTask->u8C.actor->extraFrame == -1)
        return 0;
    return 1;
}
