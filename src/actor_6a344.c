/* game_code_and_rodata 0x0806A344-0x0806AD18 (issue #64, module M18 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806A344 0x0806AD18 src/actor_6a344.c --newpb
 *
 * Actor defeat bodies, the variants ActorDie (gUnk_0873E5BC) runs (an older
 * reading called them warp-star exits, a level-clear dance and a death
 * sequence): ActorDefeatByEffect dispatches the hit effect code Task.unk82
 * through gActorDefeatsByEffect to the knock-away defeats
 * ActorDefeatPlain/Burning/Shocked (the shake and launch
 * ActorDefeatKnockAway, then ActorDefeatBlinkAndBurst) and to
 * ActorDefeatFrozen, which turns the actor into a kickable ice block
 * (ActorFreezeIntoIceBlock; per-frame sub_0806a7f4; states gUnk_0873E670:
 * sub_0806a8f4 shakes, sub_0806a980 slides away when kicked), and a family
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

    a = gCurTask->unk8C;
    p = (s8 *)a->hitReactions;
    a->hitState = 2;
    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSetFrame(0);
    if (p != NULL)
        CallTableEntry(p[1], 11, gUnk_0873E5BC);
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
    if (t->unk82 > 3)
        t->unk82 = 0;
    CallTableEntry(gCurTask->unk82, 4, gActorDefeatsByEffect);
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
    a = t->unk8C;
    a->savedPaletteBits = t->tileWord & 0xF000;
    t->unk2C = t->pixelY;
    t->unk30 = 14;
    t->unk34 = 14;
    t->unk24 = 0;
    p = gUnk_0873E610;
    for (i = 0; i < 8; i++)
    {
        u = gCurTask;
        u->pixelY = u->unk2C + p[i];
        u->posY = u->pixelY << 16;
        TaskYieldTrampoline(1);
    }
    v = gCurTask;
    v->posY = v->unk2C << 16;
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
    if (t->unk30 > 0)
    {
        t->unk30--;
        if ((t->unk30 & 1) == 0)
            t->tileWord = (t->tileWord & 0xFFF) | 0xF000;
        else
            t->tileWord = (t->tileWord & 0xFFF) | t->unk8C->savedPaletteBits;
    }
    u = gCurTask;
    if (u->unk34 <= 0)
    {
        if (u->unk24 == 0)
        {
            CreateBurstEffect(3, 6);
            gCurTask->unk24 = 1;
        }
    }
    else
    {
        u->unk34--;
    }
}

void ActorDefeatPlain(void)
{
    gCurTask->updateCallback = (u32)sub_0806a524;
    ActorDefeatKnockAway();
    TaskStop();
    sub_0806a0cc();
}

void sub_0806a524(void)
{
    ActorDefeatBlinkAndBurst();
}

void ActorDefeatBurning(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_0806a55c;
    ActorAttachEffect(t->unk82, 0);
    ActorDefeatKnockAway();
    TaskStop();
    sub_0806a0cc();
}

void sub_0806a55c(void)
{
    ActorDefeatBlinkAndBurst();
}

void ActorDefeatShocked(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_0806a594;
    ActorAttachEffect(t->unk82, 0);
    ActorDefeatKnockAway();
    TaskStop();
    sub_0806a0cc();
}

void sub_0806a594(void)
{
    ActorDefeatBlinkAndBurst();
}

void sub_0806a5a0(void)
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
    i = t->unk24 >> 1;
    if (i <= 5)
    {
        if (t->unk20 <= 0)
        {
            t->unk24 = t->unk24 + 1;
            if (t->unk1C != 0)
                t->frame = 0;
            else
                t->frame = 0xFFFF;
            u = gCurTask;
            tbl = gUnk_0873E5F8;
            n = u->unk1C;
            a = n << 1;
            a += i << 2;
            a += (u32)tbl;
            u->unk20 = *(s16 *)a;
            n ^= 1;
            u->unk1C = n;
        }
        v = gCurTask;
        v->unk20--;
    }
    else
    {
        t->frameTable = gUnk_0874CB7C;
        t->tileWord = 0;
        if (gUnk_0300244C != 0)
            t->unk8C->savedPaletteBits = 0;
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

    a = gCurTask->unk8C;
    v = CreateChildTaskHere(171, 0);
    t = gCurTask;
    t->unk46 = v;
    z = 0;
    zero = 0;
    t->unk18 = z;
    t->unk1C = z;
    t->unk20 = z;
    t->unk24 = z;
    t->unk28 = a->unk0E;
    ActorAttachEffect(3, 0);
    gCurTask->unk80 = zero;
    gCurTask->onGround = zero;
    ActorSetState(0);
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    fn = a->teardown;
    ActorLoadDef((u32)&gUnk_0873F6BC);
    a->teardown = fn;
}

void sub_0806a6a0(void)
{
    if ((u8)(gPlayerStates[gCurTask->unk28].ability - 13) > 1)
    {
        ActorSetState(2);
        TaskSetEntry(sub_0806a8d8, gCurTaskIdx);
    }
}

void sub_0806a6e0(void)
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
    ActorSetState(1);
    TaskSetEntry(sub_0806a8d8, gCurTaskIdx);
}

void ActorDefeatFrozen(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = 0;
    t->unk46 = 0xFFFF;
    TaskSetFrame(0);
    TaskYieldTrampoline(6);
    gCurTask->updateCallback = (u32)sub_0806a7f4;
    TaskStop();
    gCurTask->layer = 7;
    ActorFreezeIntoIceBlock();
    CallTableEntry(gCurTask->state, 3, gUnk_0873E670);
}

void sub_0806a7f4(void)
{
    struct Task *t;
    struct Actor *a;
    s32 r;

    gCurTask->onGround = 0;
    CallTableEntry(gCurTask->updateState, 3, gUnk_0873E67C);
    sub_0806a5a0();
    t = gCurTask;
    if (t->state == 1)
    {
        if ((s8)t->hitKind != 0)
        {
            ActorSetState(2);
            TaskSetEntry(sub_0806a8d8, gCurTaskIdx);
        }
        else
        {
            RegisterCollider((u8)gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873F880);
        }
    }
    else if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorCheckHits();
        sub_0806a6e0();
        r = ActorReactToHit();
        if (r == 0)
        {
            gCurTask->hitKind = r;
            if (gCurTask->state != 2)
                sub_0806a6a0();
        }
        else
        {
            a = gCurTask->unk8C;
            if (a->attachedTask != -1)
            {
                TaskFree(a->attachedTask);
                a->attachedTask = 0xFFFF;
            }
        }
    }
}

void sub_0806a8d8(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_0873E670);
}

void sub_0806a8f4(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(2);
        t = gCurTask;
        t->unk6C = t->unk6C + 1;
    } while ((s16)t->unk6C <= 7);
    gCurTask->velY = 0;
    TaskYieldTrampoline(120);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_0806a958(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_0806a8d8, gCurTaskIdx);
}

void sub_0806a980(void)
{
    struct Task *t;
    struct Actor *a;
    u8 one;

    t = gCurTask;
    a = t->unk8C;
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

void sub_0806a9d4(void)
{
}

void sub_0806a9d8(void)
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
    u->frameTable = gUnk_0874C9D8;
    u->tileWord = zero;
    PlayRayBurstAnim();
    ActorDestroy();
}

void sub_0806aa0c(void)
{
}

void sub_0806aa10(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk82 > 3)
        t->unk82 = 0;
    CallTableEntry(gCurTask->unk82, 4, gUnk_0873E688);
}

void sub_0806aa40(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    if (t->actorKind == 0)
    {
        t->updateCallback = 0;
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
    }
    gCurTask->health = 127;
    ActorPlaySfx(189, 0);
    PlayExplosionAnim();
}

void sub_0806aa80(void)
{
    sub_0806aa40();
}

void sub_0806aa8c(void)
{
    sub_0806aa40();
}

void sub_0806aa98(void)
{
    ActorDefeatFrozen();
}

void ExplosionScreenFlash(void)
{
    struct Task *t;
    u16 x;

    x = gBgPalette;
    gCurTask->unk6C = 0;
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
        t->unk6C = t->unk6C + 1;
    } while ((s16)t->unk6C <= 2);
    LoadBackdropColor(&x);
}

void sub_0806ab34(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;
    s32 zero;
    u8 zb;

    t = gCurTask;
    a = t->unk8C;
    zero = 0;
    t->updateCallback = zero;
    t->player = (struct PlayerState *)zero;
    TaskSetFrame(0);
    TaskYieldTrampoline(1);
    gCurTask->taskClass = 1;
    u = gCurTask;
    zb = 0;
    u->health = 127;
    u->updateCallback = (u32)sub_0806aba4;
    a->attackBox = (u32)&gUnk_0873F6E8;
    u->unk80 = zb;
    v = gCurTask;
    v->unk2C = zero;
    CreateChildTaskHere(163, 1);
    RequestScreenShake(2);
    ActorPlaySfx(189, 0);
    PlayExplosionAnim();
}

void sub_0806aba4(void)
{
    struct Task *u;

    if (gCurTask->unk2C <= 15)
    {
        ActorCheckHits();
        RegisterCollider((u8)gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY, gUnk_0873F81C);
        u = gCurTask;
        u->unk2C = u->unk2C + 1;
    }
}

void sub_0806abec(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    s32 zero;

    t = gCurTask;
    a = t->unk8C;
    zero = 0;
    t->updateCallback = zero;
    TaskSetFrame(0);
    TaskYieldTrampoline(1);
    gCurTask->taskClass = 1;
    u = gCurTask;
    u->health = 127;
    u->updateCallback = (u32)sub_0806ac48;
    a->attackBox = (u32)&gUnk_0873F704;
    u->unk2C = zero;
    RequestScreenShake(2);
    ActorPlaySfx(189, 0);
    PlayExplosionAnim();
}

void sub_0806ac48(void)
{
    struct Task *u;

    if (gCurTask->unk2C <= 15)
    {
        ActorCheckHits();
        u = gCurTask;
        u->unk2C = u->unk2C + 1;
    }
}

void sub_0806ac6c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 zero;

    t = gCurTask;
    if (t->actorKind == 0)
    {
        t->updateCallback = 0;
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
    }
    u = gCurTask;
    u->health = 127;
    zero = 0;
    u->updateCallback = (u32)sub_0806acc4;
    ActorPlaySfx(109, 0);
    v = gCurTask;
    v->frameTable = gUnk_0874C9D8;
    v->tileWord = zero;
    PlayRayBurstAnim();
}

void sub_0806acc4(void)
{
}

void sub_0806acc8(void)
{
    struct Task *t;

    TaskStop();
    TaskSetFrame(0);
    t = gCurTask;
    t->frameTable = gUnk_0874C9D8;
    t->tileWord = 0;
    PlaySfx(125);
    PlayRayBurstAnim();
}

u32 sub_0806acf8(void)
{
    if (gCurTask->unk8C->extraFrame == -1)
        return 0;
    return 1;
}
