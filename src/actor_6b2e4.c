/* game_code_and_rodata 0x0806B2E4-0x0806C2A4 (issue #64, module M18 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806B2E4 0x0806C2A4 src/actor_6b2e4.c --newpb
 *
 * The "carried by / riding on another task" movement block: the per-frame
 * position integrators sub_0806b410 and sub_0806b670 that walk the two stride-5
 * offset tables at 0x0873E7C4 / 0x0873E864, the handover helpers that hand the
 * actor back to the generic task body (ActorAttachedDie), the player-record
 * bookkeeping around gPlayerStates[] (ActorAttachedBindCarrier, ActorAttachedEnterMouth, sub_0806be4c),
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
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetFrame(*(s16 *)&gCurTask->actorDrownFrame);
    gCurTask->velY = 0x4000;
    TaskSleepForever();
}

void ActorDrownSinkUpdate(void)
{
}

void ActorDrownState1(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    TaskSetFrame(*(s16 *)&gCurTask->actorDrownFrame);
    TaskYieldTrampoline(30);
    ActorSetState(2);
    TaskSleepForever();
}

void ActorDrownState1Update(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(ActorDrownEnterState, gCurTaskIdx);
}

void ActorDrownState2(void)
{
    struct Task *t;

    gCurTask->updateState = 2;
    t = gCurTask;
    t->frameTable = gUnk_0874C9D8;
    t->tileWord = 0;
    ActorPlayRandomDefeatSfx();
    PlayRayBurstAnim();
    ActorDestroy();
}

void ActorDrownState2Update(void)
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

void sub_0806b410(void)
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
        if (v->actorKind == 1)
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
        ActorSetState(6);
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

void sub_0806b670(void)
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
        ActorSetState(2);
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

void sub_0806b848(void)
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
    if (t->actorKind == 0)
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

void sub_0806b938(void)
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
    if (t->actorKind != 0 || t->u76.subtype != 40)
        p->unk09 = 1;
    u = gCurTask;
    if (u->drawCallback == (u32)ActorDrawWorldInViewOrDestroyWithExtra)
        u->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    if (gCurTask->actorKind != 1)
        ActorSetHitReactions(gUnk_0873F92C);
    v = gCurTask;
    c = v->u8C.actor;
    w = v->tileWord;
    m = 0xF000;
    m &= w;
    c->savedPaletteBits = m;
    v->actorPaletteRestoreDelay = 2;
    a->unk04 = v->hitEffect;
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

void sub_0806ba34(void)
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
        sub_08065d44(z, t->u76.subtype, a->paletteVariant, a->paletteColorCount, t->actorKind, a->palette);
    else if (t->actorKind == 0 || t->actorKind == 3)
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
    sub_0806ba34();
    switch (gCurTask->hitEffect)
    {
    case 1:
        sub_0806b95c();
        ActorSetState(0);
        break;
    case 2:
        sub_0806b95c();
        sub_0806bc54();
        ActorSetState(4);
        break;
    case 3:
        sub_0806b95c();
        sub_0806bc54();
        ActorSetState(1);
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

void sub_0806bc28(void)
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

void sub_0806bc54(void)
{
    struct Task *t;
    s16 n;

    t = gCurTask;
    if (t->actorKind == 1)
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
        if (*(s8 *)&p->attachedCount == 1 && t->actorKind == 6 && t->u76.subtype != 0)
        {
            p->unk09 = 3;
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
    if (*(s8 *)&a->ability == 0)
        return;
    u = gCurTask;
    if (u->actorKind == 6)
    {
        f = u->u76.subtype;
        if (f == 0)
        {
            p->pendingAbility = u->unk18;
            v = gCurTask;
            p->pendingAbilityUses = v->unk1C;
            w = gCurTask;
            if (w->unk20 == w->parent)
                p->ownStarInMouth = 1;
            if (gUnk_0300244C == 0)
                return;
            p->unk0A = f;
            gUnk_02007CF4[p->playerIndex] = 1;
            return;
        }
    }
    if (gUnk_0300244C == 0 || gUnk_02007CF4[p->playerIndex] != 1)
        p->unk0A++;
    if (*(s8 *)&p->pendingAbility != 0)
        return;
    p->pendingAbility = a->ability;
    switch ((s8)a->ability)
    {
    case 7:
        q = 3;
        break;
    case 11:
    case 20:
    case 21:
        q = 1;
        break;
    default:
        q = 255;
        break;
    }
    p->pendingAbilityUses = q;
}

void sub_0806be4c(u32 i)
{
    struct Task *s;
    struct Actor *a;
    struct PlayerState *p;

    s = &gTasks[i];
    a = s->u8C.actor;
    p = s->player;
    if (a->unk04 == 1)
    {
        if (*(s8 *)&p->heldCount != 0)
        {
            p->attachedCount = p->heldCount;
        }
        else
        {
            p->unk0A = 0;
            p->unk09 = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
    }
    else
    {
        p->unk0A = 0;
        p->unk09 = 0;
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
        sub_0806be4c(gCurTaskIdx);
        t = gCurTask;
        w = t->tileWord;
        m = 0xFFF;
        m &= w;
        t->tileWord = m | t->u8C.actor->savedPaletteBits;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (t->actorKind != 1 && t->actorKind != 6)
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
        if (t->actorKind == 0)
            ActorAwardScore(t->parent, 1 << t->actorSwallowOrder);
        else
            ActorAwardScore(t->parent, 1);
        u = gCurTask;
        if (u->actorKind != 1)
        {
            if (u->actorKind == 6)
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
    sub_0806b938();
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
    t->hitKind = 0;
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
        sub_0806b670();
    ActorAttachedRestorePalette();
    u = gCurTask;
    if (u->actorKind == 1)
        RegisterCollider((u8)gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873F844);
    else
        RegisterCollider((u8)gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873F830);
}

void ActorAttachedBackdropHeldLateUpdate(void)
{
    sub_0806b848();
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
    gCurTask->hitKind = 0;
    gCurTask->u80.attackAbility = 0;
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
        ActorSetState(3);
        TaskSetEntry(ActorAttachedRunState, gCurTaskIdx);
        return;
    }
    t = gCurTask;
    if (t->actorKind == 1)
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F844);
    else
        RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F830);
}
