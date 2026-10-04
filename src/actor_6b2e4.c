/* game_code_and_rodata 0x0806B2E4-0x0806C2A4 (issue #64, module M18 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806B2E4 0x0806C2A4 src/actor_6b2e4.c --newpb
 *
 * The "carried by / riding on another task" movement block: the per-frame
 * position integrators ActorAttachedThrowHeldFollowCarrier and ActorAttachedBackdropHeldFollowCarrier that walk the two stride-5
 * offset tables at 0x0873E7C4 / 0x0873E864, the handover helpers that hand the
 * actor back to the generic task body (ActorAttachedDie), the player-record
 * bookkeeping around gPlayerStates[] (ActorAttachedBindCarrier, ActorAttachedEnterMouth, ActorAttachedReleaseCarrierSlot),
 * and the class-1 task bodies ActorAttachedSwallow / ActorAttachedBackdropHeld / ActorAttachedBackdropFlight with
 * their per-frame callbacks.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "player.h"
#include "actor.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(u32 a);
extern void TaskSetEntry(void *fn, s32 i);
extern void ActorSetState(s32 a);
extern u32 GetShapeAtPixelIgnoringOneWay(s32 x, s32 y);
extern void ActorSetTerrainBox(u32 *p);

extern void ActorSetHitReactions(struct ActorVt *p);
extern s32 TaskGetDxTo(s32 i);
extern void ActorSetTerrainHandlers(struct ActorHandlers *p);
extern void ActorCollideTerrain(void);
extern void RequestScreenShake(u32 a);
extern void RegisterCollider(u8 a, s16 x, s16 y, u32 *p);
/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

void ActorDrownEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gActorDrownStates);
}

void ActorDrownSink(void)
{
    gCurTask->updateState = ACTOR_DROWN_STATE_SINK;
    TaskStop();
    TaskSetFrame(*(s16 *)&gCurTask->actorDrownFrame);
    gCurTask->velY = 0x4000;
    TaskSleepForever();
}

void ActorDrownSinkUpdate(void)
{
}

void ActorDrownWait(void)
{
    gCurTask->updateState = ACTOR_DROWN_STATE_WAIT;
    TaskStop();
    TaskSetFrame(*(s16 *)&gCurTask->actorDrownFrame);
    TaskYieldTrampoline(30);
    ActorSetState(ACTOR_DROWN_STATE_BURST);
    TaskSleepForever();
}

void ActorDrownWaitUpdate(void)
{
    if (gCurTask->state != ACTOR_DROWN_STATE_WAIT)
        TaskSetEntry(ActorDrownEnterState, gCurTaskIdx);
}

void ActorDrownBurst(void)
{
    struct Task *t;

    gCurTask->updateState = ACTOR_DROWN_STATE_BURST;
    t = gCurTask;
    t->frameTable = gUnk_0874C9D8;
    t->tileWord = 0;
    ActorPlayRandomDefeatSfx();
    PlayRayBurstAnim();
    ActorDestroy();
}

void ActorDrownBurstUpdate(void)
{
}

void ActorDefeatPickup(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateCallback = 0;
    if (t->u76.subtype != 0)
        MarkRoomObjectUsed(gCurTaskIdx);
    TaskYieldTrampoline(1);
    u = gCurTask;
    u->frameTable = gUnk_0874C9D8;
    u->tileWord = 0;
    PlaySfx(109);
    PlayRayBurstAnim();
}

void sub_0806b40c(void)
{
}

void ActorAttachedThrowHeldFollowCarrier(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    struct PlayerState *p;
    struct Task *s;
    s16 e;
    s32 k;
    s16 n;
    s16 g;
    s16 h;
    u32 tb;
    u8 b;

    t = gCurTask;
    s = &gTasks[t->parent];
    p = t->player;
    if (t->actorCarrierFacing != s->facing)
        t->facing = -t->facing;
    u = gCurTask;
    u->actorCarrierFacing = s->facing;
    if (*(s8 *)&p->unk16 == -1)
    {
        u->pixelX = u->actorCarriedX;
        u->pixelY = u->actorCarriedY;
        ActorAttachedDie();
        return;
    }
    if ((u8)(p->unk16 + 5) <= 2)
    {
        u->facing = s->facing;
        v = gCurTask;
        v->unk70 = -*(s8 *)&p->unk16;
        v->pixelX = v->actorCarriedX;
        v->pixelY = v->actorCarriedY;
        k = *(s16 *)&v->unk70 - 3;
        if (v->actorKind == ACTOR_KIND_MID_BOSS)
        {
            ActorSetTerrainBox(gUnk_0873F8BC);
            w = gCurTask;
            w->pixelX += gUnk_0873EAD8[k][2] * w->facing;
            w->pixelY += gUnk_0873EAD8[k][3];
        }
        else
        {
            ActorSetTerrainBox(gUnk_0873F8B4);
            w = gCurTask;
            w->pixelX += gUnk_0873EAD8[k][0] * w->facing;
            w->pixelY += gUnk_0873EAD8[k][1];
        }
        x = gCurTask;
        x->posX = x->pixelX << 16;
        x->posY = x->pixelY << 16;
        if (GetShapeAtPixelIgnoringOneWay(x->pixelX, x->pixelY) != 0)
        {
            y = gCurTask;
            if (CanBreakBlock(y->pixelX >> 4, y->pixelY >> 4, 3, -1) == 0)
            {
                ActorAttachedDie();
                return;
            }
        }
        ActorSetState(ACTOR_ATTACHED_STATE_THROW_FLIGHT);
        TaskSetEntry(ActorAttachedRunState, gCurTaskIdx);
        return;
    }
    n = *(s8 *)&p->unk16 * 5;
    g = (gUnk_0873E7C4[n] + u->actorCarryOffsetX) * s->facing;
    if (e == 1)
        h = gUnk_0873E7C4[n + 1] - u->actorCarryOffsetY;
    else
        h = gUnk_0873E7C4[n + 1] + u->actorCarryOffsetY;
    e = gUnk_0873E7C4[n + 2];
    b = gUnk_0873E7C4[n + 4];
    z = gCurTask;
    z->pixelX += g;
    z->pixelY += h;
    z->actorCarriedX = z->pixelX;
    z->actorCarriedY = z->pixelY;
    TaskSetFrame((s16)(e + 2));
    gCurTask->layer = b;
}

void ActorAttachedBackdropHeldFollowCarrier(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *z;
    struct PlayerState *p;
    struct Task *s;
    s16 e;
    s16 n;
    s16 g;
    s16 h;
    u8 b;

    t = gCurTask;
    s = &gTasks[t->parent];
    p = t->player;
    if (*(s8 *)&p->unk16 == -1)
    {
        t->pixelX = t->actorCarriedX;
        t->pixelY = t->actorCarriedY;
        ActorAttachedDie();
        return;
    }
    if (*(s8 *)&p->unk16 == -2)
    {
        t->pixelX = t->actorCarriedX;
        t->pixelY = t->actorCarriedY;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (GetShapeAtPixelIgnoringOneWay(t->pixelX, t->pixelY) != 0)
        {
            u = gCurTask;
            if (CanBreakBlock(u->pixelX >> 4, u->pixelY >> 4, 3, -1) == 0)
            {
                ActorAttachedDie();
                return;
            }
            v = gCurTask;
            v->facing = s->facing;
        }
        else
        {
            w = gCurTask;
            w->facing = s->facing;
        }
        ActorSetState(ACTOR_ATTACHED_STATE_BACKDROP_FLIGHT);
        TaskSetEntry(ActorAttachedRunState, gCurTaskIdx);
        return;
    }
    n = *(s8 *)&p->unk16 * 5;
    g = (gUnk_0873E864[n] + t->actorCarryOffsetX) * s->facing;
    if (e == 1)
        h = gUnk_0873E864[n + 1] - t->actorCarryOffsetY;
    else
        h = gUnk_0873E864[n + 1] + t->actorCarryOffsetY;
    e = gUnk_0873E864[n + 2];
    b = gUnk_0873E864[n + 4];
    z = gCurTask;
    z->pixelX += g;
    z->pixelY += h;
    z->actorCarriedX = z->pixelX;
    z->actorCarriedY = z->pixelY;
    TaskSetFrame((s16)(e + 2));
    gCurTask->layer = b;
    if (gUnk_0873E864[n + 3] == 1)
    {
        x = gCurTask;
        if (x->facing == 1)
            x->spriteFlags |= 0x8000;
        else
            x->spriteFlags &= 0x7FFF;
    }
}

void ActorAttachedHeldAddPlayerOffset(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    p = t->player;
    t->pixelX += *(s16 *)&p->pixelOffsetX >> 8;
    t->actorCarriedX = t->pixelX;
    t->actorCarriedY = t->pixelY;
}

void ActorAttachedRestorePalette(void)
{
    struct Task *t;
    u32 m;
    u32 n;
    u32 v;
    u32 w;
    u32 q;

    t = gCurTask;
    v = t->tileWord;
    m = 0xF000;
    q = t->u8C.actor->savedPaletteBits;
    m &= v;
    if (m == q)
        return;
    if (--t->actorPaletteRestoreDelay >= 0)
        return;
    w = t->tileWord;
    n = 0xFFF;
    n &= w;
    t->tileWord = n | t->u8C.actor->savedPaletteBits;
}

void ActorAttachedDie(void)
{
    struct Task *t;
    struct Task *u;
    u16 v;

    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_ENEMY)
    {
        v = t->u76.subtype;
        if (v == 17 || v == 9 || v == 0 || v == 31 || v == 32)
            gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    }
    u = gCurTask;
    u->moveCallback = (u32)TaskMove;
    u->hitEffect = 0;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void ActorAttachedDieUnlessInhaling(void)
{
    struct PlayerState *p;
    u8 z;

    p = gCurTask->player;
    if (p->mode == 10)
        return;
    z = 0;
    p->heldCount = z;
    p->attachedCount = z;
    ActorAttachedDie();
}

void sub_0806b95c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    struct Actor *c;
    struct PlayerState *p;
    u32 m;
    u32 w;

    t = gCurTask;
    a = t->u8C.actor;
    p = t->player;
    if (t->actorKind != ACTOR_KIND_ENEMY || t->u76.subtype != 40)
        p->catchKind = 1;
    u = gCurTask;
    if (u->drawCallback == (u32)ActorDrawWorldInViewOrDestroyWithExtra)
        u->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    if (gCurTask->actorKind != ACTOR_KIND_MID_BOSS)
        ActorSetHitReactions(gUnk_0873F92C);
    v = gCurTask;
    c = v->u8C.actor;
    w = v->tileWord;
    m = 0xF000;
    m &= w;
    c->savedPaletteBits = m;
    v->actorPaletteRestoreDelay = 2;
    a->attachEffect = v->hitEffect;
}

void ActorAttachedBindCarrier(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    struct Task *s;
    t = gCurTask;
    t->parent = t->hitterSlot;
    t->player = p = &gPlayerStates[t->parent];
    p->attachedCount++;
    u = gCurTask;
    s = &gTasks[u->parent];
    u->actorCarrierFacing = s->facing;
}

void ActorAttachedReloadPalette(void)
{
    struct Task *t;
    struct Actor *a;
    u32 v;
    u32 m;
    u32 z;

    t = gCurTask;
    a = t->u8C.actor;
    v = t->tileWord;
    z = v >> 12;
    if (a->palette != 0)
        LoadActorPaletteVariant(z, t->u76.subtype, a->paletteVariant, a->paletteColorCount, t->actorKind, a->palette);
    else if (t->actorKind == ACTOR_KIND_ENEMY || t->actorKind == 3)
    {
        m = 0xFFF;
        m &= v;
        t->tileWord = m | a->savedPaletteBits;
    }
    a->paletteOverridden |= 1;
}

void TaskSetPosRelativeToParent(void)
{
    struct Task *t;
    struct Task *s;

    t = gCurTask;
    s = &gTasks[t->parent];
    t->posX = (t->pixelX - s->pixelX) << 16;
    t->posY = (t->pixelY - s->pixelY) << 16;
}

s32 TaskIsParentWithinX(s32 a)
{
    if (a > abs(TaskGetDxTo(gCurTask->parent)))
        return 1;
    return 0;
}

void sub_0806bb34(s32 a)
{
    struct Task *t;
    u32 m;
    u32 w;

    switch (a)
    {
    case 0:
        TaskSetFrame(1);
        break;
    case 1:
    case 4:
        TaskSetFrame(2);
        break;
    }
    t = gCurTask;
    w = t->tileWord;
    m = 0xFFF;
    m &= w;
    t->tileWord = m | 0xF000;
}

s32 ActorAttachToHitter(void)
{
    ActorAttachedBindCarrier();
    ActorAttachedReloadPalette();
    switch (gCurTask->hitEffect)
    {
    case HIT_EFFECT_INHALE:
        sub_0806b95c();
        ActorSetState(ACTOR_ATTACHED_STATE_SWALLOW);
        break;
    case HIT_EFFECT_THROW:
        sub_0806b95c();
        ActorInitCarryOffset();
        ActorSetState(ACTOR_ATTACHED_STATE_PULL_IN);
        break;
    case HIT_EFFECT_BACKDROP:
        sub_0806b95c();
        ActorInitCarryOffset();
        ActorSetState(ACTOR_ATTACHED_STATE_BACKDROP_HELD);
        break;
    }
    TaskSetEntry(ActorAttachedEnterState, gCurTaskIdx);
    return 1;
}

void ActorAttachedPullTowardCarrier(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;

    t = gCurTask;
    if (t->posX <= 0)
        t->accelX = 10752;
    else
        t->accelX = -10752;
    u = gCurTask;
    v = u->posY;
    if (v < 0)
        v = -v;
    v >>= 3;
    if (u->posY <= 0)
        u->velY = v;
    else
        u->velY = -v;
}

void ActorAttachedPullInStep(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    p = t->player;
    if (*(s8 *)&p->heldCount != 0)
    {
        t->actorMouthFull = 1;
        TaskStop();
    }
    else
    {
        ActorAttachedPullTowardCarrier();
    }
}

void ActorInitCarryOffset(void)
{
    struct Task *t;
    s16 n;

    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_MID_BOSS)
    {
        n = t->u76.subtype * 2;
        t->actorCarryOffsetX = gUnk_0873E7A4[n];
        t->actorCarryOffsetY = gUnk_0873E7A4[n + 1];
    }
    else
    {
        t->actorCarryOffsetY = 0;
        t->actorCarryOffsetX = 0;
    }
}

void sub_0806bc9c(void)
{
    struct Task *t;
    struct PlayerState *p;
    u8 one;

    t = gCurTask;
    p = t->player;
    ActorAwardScore(t->parent, 1);
    if (*(s8 *)&p->heldCount != 0)
    {
        gCurTask->actorMouthFull = 1;
        TaskStop();
    }
    else
    {
        one = 1;
        p->heldCount = one;
        p->attachedCount = one;
        TaskStop();
    }
}

void ActorAttachedSwallowStep(void)
{
    ActorAttachedPullTowardCarrier();
    if (*(s16 *)&gCurTask->actorEnteredMouth != 0)
        return;
    if (TaskIsParentWithinX(18) == 0)
        return;
    ActorAttachedEnterMouth();
    gCurTask->actorEnteredMouth = 1;
}

void ActorAttachedEnterMouth(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Actor *a;
    struct PlayerState *p;
    u32 f;
    u8 q;

    t = gCurTask;
    a = t->u8C.actor;
    p = t->player;
    if ((s8)a->def->isItem == 1)
    {
        if (*(s8 *)&p->attachedCount == 1 && t->actorKind == ACTOR_KIND_ITEM && t->u76.subtype != 0)
        {
            p->catchKind = 3;
            gCurTask->actorSwallowOrder = *(s8 *)&p->heldCount;
            p->heldCount++;
        }
        else if ((s8)p->attachedCount > 0)
        {
            p->attachedCount--;
        }
    }
    else
    {
        t->actorSwallowOrder = *(s8 *)&p->heldCount;
        p->heldCount++;
        p->mouthState = 1;
    }
    p->ownStarInMouth = 0;
    if (*(s8 *)&a->ability == ABILITY_NORMAL)
        return;
    u = gCurTask;
    if (u->actorKind == ACTOR_KIND_ITEM)
    {
        f = u->u76.subtype;
        if (f == 0)
        {
            p->pendingAbility = u->abilityStarAbility;
            v = gCurTask;
            p->pendingAbilityUses = v->abilityStarAbilityUses;
            w = gCurTask;
            if (w->abilityStarOwnerSlot == w->parent)
                p->ownStarInMouth = 1;
            if (gUnk_0300244C == 0)
                return;
            p->abilitySwallowCount = f;
            gUnk_02007CF4[p->playerIndex] = 1;
            return;
        }
    }
    if (gUnk_0300244C == 0 || gUnk_02007CF4[p->playerIndex] != 1)
        p->abilitySwallowCount++;
    if (*(s8 *)&p->pendingAbility != ABILITY_NORMAL)
        return;
    p->pendingAbility = a->ability;
    switch ((s8)a->ability)
    {
    case ABILITY_MIKE:
        q = 3;
        break;
    case ABILITY_SLEEP:
    case ABILITY_CRASH:
    case ABILITY_LIGHT:
        q = 1;
        break;
    default:
        q = 255;
        break;
    }
    p->pendingAbilityUses = q;
}

void ActorAttachedReleaseCarrierSlot(u32 i)
{
    struct Task *s;
    struct Actor *a;
    struct PlayerState *p;

    s = &gTasks[i];
    a = s->u8C.actor;
    p = s->player;
    if (a->attachEffect == 1)
    {
        if (*(s8 *)&p->heldCount != 0)
        {
            p->attachedCount = p->heldCount;
        }
        else
        {
            p->abilitySwallowCount = 0;
            p->catchKind = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
    }
    else
    {
        p->abilitySwallowCount = 0;
        p->catchKind = 0;
        p->heldCount = 0;
        p->attachedCount = 0;
    }
}

u8 ActorAttachedCheckScreenAttack(void)
{
    struct Task *t;
    u32 m;
    u32 w;

    if (gScreenAttackActive == 1)
    {
        ActorAttachedReleaseCarrierSlot(gCurTaskIdx);
        t = gCurTask;
        w = t->tileWord;
        m = 0xFFF;
        m &= w;
        t->tileWord = m | t->u8C.actor->savedPaletteBits;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (t->actorKind != ACTOR_KIND_MID_BOSS && t->actorKind != ACTOR_KIND_ITEM)
            ActorSetHitReactions(gUnk_0873F938);
        ActorAttachedDie();
    }
    return gScreenAttackActive;
}

void ActorAttachedEnterState(void)
{
    gCurTask->updateCallback = 0;
    TaskStop();
    gCurTask->onGround = 0;
    sub_0806bb34(gCurTask->state);
    CallTableEntry(gCurTask->state, 8, gActorAttachedStates);
    TaskSleepForever();
}

void ActorAttachedRunState(void)
{
    CallTableEntry(gCurTask->state, 8, gActorAttachedStates);
}

void ActorAttachedSwallow(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->lateUpdateCallback = (u32)ActorAttachedSwallowLateUpdate;
    ActorSetTerrainHandlers(gUnk_0873F8F4);
    TaskSetPosRelativeToParent();
    u = gCurTask;
    u->actorSwallowed = 0;
    u->actorEnteredMouth = 0;
    u->actorSwallowOrder = 0;
    u->layer = 6;
    while (TaskIsParentWithinX(16) == 0)
    {
        ActorAttachedSwallowStep();
        TaskYieldTrampoline(1);
    }
    if (*(s16 *)&gCurTask->actorEnteredMouth == 0)
    {
        ActorAttachedEnterMouth();
        gCurTask->actorEnteredMouth = 1;
    }
    gCurTask->actorSwallowed = 1;
    TaskSleepForever();
}

void ActorAttachedSwallowLateUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->actorSwallowed != 0)
    {
        if (t->actorKind == ACTOR_KIND_ENEMY)
            ActorAwardScore(t->parent, 1 << t->actorSwallowOrder);
        else
            ActorAwardScore(t->parent, 1);
        u = gCurTask;
        if (u->actorKind != ACTOR_KIND_MID_BOSS)
        {
            if (u->actorKind == ACTOR_KIND_ITEM)
            {
                PickupCollect();
                return;
            }
        }
        ActorDestroy();
        return;
    }
    if (*(s16 *)&t->actorEnteredMouth != 0)
        return;
    if (ActorAttachedCheckScreenAttack() != 0)
        return;
    ActorAttachedDieUnlessInhaling();
    ActorAttachedRestorePalette();
}

void ActorAttachedBackdropHeld(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToParent;
    t->updateCallback = (u32)ActorAttachedBackdropHeldUpdate;
    t->lateUpdateCallback = (u32)ActorAttachedBackdropHeldLateUpdate;
    t->posY = 0;
    t->posX = 0;
    t->hitKind = HIT_KIND_NONE;
    u = gCurTask;
    u->actorCarriedX = u->pixelX;
    u->actorCarriedY = u->pixelY;
    u->taskClass = 1;
    ActorSetTerrainHandlers(gUnk_0873F8F4);
    gCurTask->actorMouthFull = 0;
    sub_0806bc9c();
    TaskSleepForever();
}

void ActorAttachedBackdropHeldUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->health = 127;
    if (t->actorMouthFull == 1)
        ActorAttachedDie();
    else
        ActorAttachedBackdropHeldFollowCarrier();
    ActorAttachedRestorePalette();
    u = gCurTask;
    if (u->actorKind == ACTOR_KIND_MID_BOSS)
        RegisterCollider((u8)gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873F844);
    else
        RegisterCollider((u8)gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873F830);
}

void ActorAttachedBackdropHeldLateUpdate(void)
{
    ActorAttachedHeldAddPlayerOffset();
    ActorAttachedCheckScreenAttack();
}

void ActorAttachedBackdropFlight(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->updateCallback = (u32)ActorAttachedBackdropFlightUpdate;
    t->lateUpdateCallback = 0;
    if (gUnk_0300244C != 0 && t->u8C.actor->terrainBox == 0)
        ActorSetTerrainBox(gUnk_0873F894);
    TaskSetMotionXFacing(0x38000, 0x5A5A5A5A);
    TaskSetMotionY(0x30000, 0x8000, 0x60000);
    gCurTask->hitKind = HIT_KIND_NONE;
    gCurTask->u80.attackAbility = ABILITY_NORMAL;
    TaskSleepForever();
}

void ActorAttachedBackdropFlightUpdate(void)
{
    struct Task *t;

    gCurTask->health = 127;
    ActorCollideTerrain();
    if ((*(u32 *)gTerrainResult & 0xFFFFFF) != 0)
    {
        if (gTerrainResult[1] != 0)
            gCurTask->actorBounceSurface = 0;
        if (gTerrainResult[2] != 0)
            gCurTask->actorBounceSurface = 1;
        if (gTerrainResult[0] != 0)
            gCurTask->actorBounceSurface = 2;
        PlaySfx(179);
        RequestScreenShake(2);
        ActorSetState(ACTOR_ATTACHED_STATE_BACKDROP_BOUNCE_OFF);
        TaskSetEntry(ActorAttachedRunState, gCurTaskIdx);
        return;
    }
    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_MID_BOSS)
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F844);
    else
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F830);
}
