/* game_code_and_rodata 0x080692FC-0x0806A344 (issue #64, module M18 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080692FC 0x0806A344 src/actor_692fc.c --newpb
 *
 * The player-input dispatch layer of the actor core.  Six probe wrappers
 * (0x080694E0-0x080696A0) snapshot the current task's directional state into a
 * 6-byte stack record (ActorGetTerrainBox) and hand it to one of the input decoders
 * at 0x0801BCAC..0x0801C3A4; the three big dispatchers (ActorCollideTerrain,
 * ActorCollideTerrainInCameraBounds, ActorCollideTerrainFloor) then walk the actor's seven-entry handler table
 * at Actor.terrainHandlers, calling the first handler that claims the frame.  The tail
 * of the module is the class-1 "carried" task body: state machine entry
 * points (ActorReactToHitKind/PickupReactToHit), the ActorPlayHitSfx sound dispatcher and
 * the ActorStartHitStun/ActorEndHitStun push/pop of the actor's transform.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "actor.h"

/* The 6-byte directional record ActorGetTerrainBox fills on the stack: three raw
   bytes copied from Actor.terrainBox plus three that are negated when the task
   faces left (Task.facing == -1). */
struct TerrainCollisionBox
{
    /*0x00*/ u8 offsetX;
    /*0x01*/ u8 offsetY;
    /*0x02*/ u8 top;
    /*0x03*/ u8 bottom;
    /*0x04*/ u8 left;
    /*0x05*/ u8 right;
};

/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void TerrainCollideBox(struct TerrainCollisionBox *p);
extern void TerrainCollideBoxInCameraBounds(struct TerrainCollisionBox *p);
extern void TerrainCollideBoxWalls(struct TerrainCollisionBox *p);
extern void TerrainCollideBoxCeilingAndFloor(struct TerrainCollisionBox *p);
extern void TerrainCollideBoxFloor(struct TerrainCollisionBox *p);
extern void TerrainCollideBoxAlongVelocity(struct TerrainCollisionBox *p);
extern void TerrainCollidePointPushOut(struct TerrainCollisionBox *p);
extern u32 TerrainCollidePointStop(struct TerrainCollisionBox *p);
extern u32 sub_0802205c(struct TerrainCollisionBox *p);
extern void TaskInitWaterFlagsSlot(s32 i);
extern void ActorSetState(u8 v);
extern void ActorSetTerrainHandlers(u32 v);
extern void ActorCheckHits(void);

u32 ActorReactToHit(void);

u32 ActorCollideTerrain(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct TerrainCollisionBox v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->u8C.actor;
    r = 0;
    i = 4;
    k = t->onGround;
    f = t->waterFlags;
    ActorGetTerrainBox(&v);
    TerrainCollideBox(&v);
    u = gCurTask;
    if ((u->waterFlags & 0x80) != 0)
        CreateChildTaskAt(TASK_ACTOR_SPLASH, u->pixelX, ((s16 *)gTerrainResult)[i], 0);
    if ((f & 1) != 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gCurTask->waterFlags & 1) == 0 && (gCurTask->waterFlags & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->leaveWaterCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if ((f & 1) == 0)
        goto b2;
    if ((f & 0x40) == 0)
        goto s2;
b2:
    if ((gCurTask->waterFlags & 1) != 0 && (gCurTask->waterFlags & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->enterWaterCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s2:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gCurTask->onGround & 1) != 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->terrainHandlers)->leaveGroundCallback;
    }
    else
    {
        if ((gCurTask->onGround & 1) == 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->terrainHandlers)->landCallback;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s3:
    if (r == 1)
        return 1;
    if ((gTerrainResult[0] & 3) != 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->hitWallCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->onGround & 1) != 0 && (gTerrainResult[3] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->unk14;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->onGround & 1) == 0 && (gTerrainResult[1] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->hitCeilingCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    return r;
}

u32 sub_080694e0(void)
{
    struct TerrainCollisionBox v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    ActorGetTerrainBox(&v);
    sub_0802205c(&v);
}

u32 ActorCollideTerrainAlongVelocity(void)
{
    struct TerrainCollisionBox v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        TerrainCollideBoxAlongVelocity(&v);
        if ((*(u32 *)gTerrainResult & 0x00FFFFFF) != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 ActorCollideTerrainCeilingAndFloor(void)
{
    struct TerrainCollisionBox v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        TerrainCollideBoxCeilingAndFloor(&v);
        if ((*(u32 *)gTerrainResult & 0x00FFFF00) != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 ActorCollideTerrainWalls(void)
{
    struct TerrainCollisionBox v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        TerrainCollideBoxWalls(&v);
        if (gTerrainResult[0] != 0)
            r = 1;
        else
            r = 0;
        return r;
    }
    return 0;
}

u32 ActorCollideTerrainPointPushOut(void)
{
    struct TerrainCollisionBox v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    r = 0;
    ActorGetTerrainBox(&v);
    TerrainCollidePointPushOut(&v);
    if (gTerrainResult[0] != 0 || gTerrainResult[4] != 0 || gTerrainResult[1] != 0)
        r = 1;
    return r;
}

u32 ActorCollideTerrainPointStop(void)
{
    struct TerrainCollisionBox v;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        ActorGetTerrainBox(&v);
        r = (u8)TerrainCollidePointStop(&v);
    }
    else
    {
        r = 0;
    }
    return r;
}

u32 ActorCollideTerrainInCameraBounds(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct TerrainCollisionBox v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->u8C.actor;
    r = 0;
    i = 4;
    k = t->onGround;
    f = t->waterFlags;
    ActorGetTerrainBox(&v);
    TerrainCollideBoxInCameraBounds(&v);
    TaskBounceOffCameraBounds();
    u = gCurTask;
    if ((u->waterFlags & 0x80) != 0)
        CreateChildTaskAt(TASK_ACTOR_SPLASH, u->pixelX, ((s16 *)gTerrainResult)[i], 0);
    if ((f & 1) != 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gCurTask->waterFlags & 1) == 0 && (gCurTask->waterFlags & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->leaveWaterCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if ((f & 1) == 0)
        goto b2;
    if ((f & 0x40) == 0)
        goto s2;
b2:
    if ((gCurTask->waterFlags & 1) != 0 && (gCurTask->waterFlags & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->enterWaterCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s2:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gCurTask->onGround & 1) != 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->terrainHandlers)->leaveGroundCallback;
    }
    else
    {
        if ((gCurTask->onGround & 1) == 0)
            goto s3;
        fn = ((struct ActorHandlers *)a->terrainHandlers)->landCallback;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s3:
    if (r == 1)
        return 1;
    if ((gTerrainResult[0] & 3) != 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->hitWallCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->onGround & 1) != 0 && (gTerrainResult[3] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->unk14;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    if (r == 1)
        return 1;
    if ((gCurTask->onGround & 1) == 0 && (gTerrainResult[1] & 1) != 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->hitCeilingCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
    return r;
}

u32 ActorCollideTerrainFloor(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct TerrainCollisionBox v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->u8C.actor;
    r = 0;
    i = 4;
    k = t->onGround;
    f = t->waterFlags;
    ActorGetTerrainBox(&v);
    TerrainCollideBoxFloor(&v);
    u = gCurTask;
    if ((u->waterFlags & 0x80) != 0)
        CreateChildTaskAt(TASK_ACTOR_SPLASH, u->pixelX, ((s16 *)gTerrainResult)[i], 0);
    if ((f & 1) == 0)
        goto b1;
    if ((f & 0x40) == 0)
        goto s1;
b1:
    if ((gCurTask->waterFlags & 1) != 0 && (gCurTask->waterFlags & 0x40) == 0)
    {
        fn = ((struct ActorHandlers *)a->terrainHandlers)->enterWaterCallback;
        if (fn != 0)
            r = ((u8 (*)(void))fn)();
    }
s1:
    if (r == 1)
        return 1;
    if (k == 1)
    {
        if ((gCurTask->onGround & 1) != 0)
            goto s2;
        fn = ((struct ActorHandlers *)a->terrainHandlers)->leaveGroundCallback;
    }
    else
    {
        if ((gCurTask->onGround & 1) == 0)
            goto s2;
        fn = ((struct ActorHandlers *)a->terrainHandlers)->landCallback;
    }
    if (fn != 0)
        r = ((u8 (*)(void))fn)();
s2:
    if (r == 1)
        return 1;
    return r;
}

u32 ActorStepBackFromSlope(void)
{
    struct Task *t;
    struct Task *u;
    s16 y;
    s16 m;
    s16 d;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        t = gCurTask;
        if ((t->actorFlatGroundY & 0xFFFF0000) != 0
         && (t->onGround & 1) != 0
         && (gTerrainResult[4] == 1 || gTerrainResult[4] == 2
          || gTerrainResult[4] == 3 || gTerrainResult[4] == 4))
        {
            if (t->velX >= 0)
            {
                m = t->pixelX - 16;
                y = m | 0xF;
            }
            else
            {
                m = t->pixelX + 16;
                y = m & 0xFFF0;
            }
            u = gCurTask;
            d = y - u->pixelX;
            u->pixelX = y + d;
            u->pixelY = u->actorFlatGroundY;
            u->posX = u->pixelX << 16;
            u->posY = u->pixelY << 16;
            return 1;
        }
    }
    return 0;
}

void ActorGetTerrainBox(struct TerrainCollisionBox *out)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    out->offsetY = ((struct TerrainCollisionBox *)a->terrainBox)->offsetY;
    out->top = ((struct TerrainCollisionBox *)a->terrainBox)->top;
    out->bottom = ((struct TerrainCollisionBox *)a->terrainBox)->bottom;
    if (t->facing == -1)
    {
        out->offsetX = -((struct TerrainCollisionBox *)a->terrainBox)->offsetX;
        out->left = -((struct TerrainCollisionBox *)a->terrainBox)->right;
        out->right = -((struct TerrainCollisionBox *)a->terrainBox)->left;
    }
    else
    {
        out->offsetX = ((struct TerrainCollisionBox *)a->terrainBox)->offsetX;
        out->left = ((struct TerrainCollisionBox *)a->terrainBox)->left;
        out->right = ((struct TerrainCollisionBox *)a->terrainBox)->right;
    }
}

void ActorInitTerrainFlagsSlot(s32 i)
{
    struct Task *t;

    t = &gTasks[i];
    TaskInitWaterFlagsSlot(i);
    t->onGround = 1;
}

u32 ActorReactToHitKind(s8 a)
{
    u32 r;

    r = 0;
    switch (a)
    {
    case 1:
        r = ActorReactToDefeat();
        break;
    case 3:
        r = ActorAttachToHitter();
        break;
    case 4:
        r = ActorAttachToHitter();
        break;
    case 2:
    case 5:
        r = ActorReactToDamage();
        break;
    case 6:
    case 7:
    case 8:
        break;
    }
    return r;
}

u32 ActorReactToHit(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    return ActorReactToHitKind(gCurTask->hitKind);
}

u32 ActorReactToHitOrTerrainDamage(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    return ActorReactToHitKind(ActorHitKindWithTerrainDamage());
}

u32 PickupReactToHit(void)
{
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    gCurTask->facing = 1;
    r = 0;
    switch ((s8)gCurTask->hitKind)
    {
    case HIT_KIND_DEFEAT:
    case HIT_KIND_DAMAGE:
    case HIT_KIND_SLIDE:
    case 7:
        /* PickupCollect is void in the ROM but its result is consumed here:
           the original had no prototype in scope at this point, so the
           implicit `int ()` declaration was used.  The cast reproduces the
           direct `bl` without tripping -Wimplicit -Werror. */
        r = ((u32 (*)(void))PickupCollect)();
        break;
    case HIT_KIND_INHALE:
    case HIT_KIND_GRAB:
        r = ActorAttachToHitter();
        break;
    case HIT_KIND_NO_DAMAGE:
    case HIT_KIND_CATCH:
        break;
    }
    return r;
}

s8 ActorHitKindWithTerrainDamage(void)
{
    struct Task *t;
    u8 *g;
    u8 v;
    s32 c;

    t = gCurTask;
    if ((s8)t->hitKind != HIT_KIND_NONE)
    {
        v = t->hitKind;
    }
    else
    {
        g = gTerrainResult;
        if (g[12] != 0)
        {
            v = g[12];
            c = ((s8 *)g)[12];
            t->hitKind = c;
            gCurTask->hitEffect = 0;
        }
        else
        {
            v = 0;
        }
    }
    return v;
}

void ActorPlayHitSfx(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->u8C.actor->hitterClass == 16 || t->u8C.actor->hitterClass == 32)
    {
        u = &gTasks[t->hitterSlot];
        switch (u->u80.attackAbility)
        {
        case ABILITY_CUTTER:
            PlaySfx(SE_CUTTER_HIT);
            break;
        case ABILITY_SWORD:
            PlaySfx(SE_SWORD_HIT);
            break;
        case ABILITY_HAMMER:
            PlaySfx(SE_HAMMER_HIT);
            break;
        case ABILITY_NEEDLE:
            PlaySfx(SE_NEEDLE_HIT);
            break;
        case ABILITY_ICE:
        case ABILITY_FREEZE:
            PlaySfx(SE_ICE_HIT);
            break;
        case ABILITY_NORMAL:
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_LASER:
        case ABILITY_MIKE:
        case ABILITY_WHEEL:
        case ABILITY_PARASOL:
        case ABILITY_SLEEP:
        case ABILITY_HI_JUMP:
        case ABILITY_BEAM:
        case ABILITY_STONE:
        case ABILITY_BALL:
        case ABILITY_TORNADO:
        case ABILITY_CRASH:
        case ABILITY_LIGHT:
        case ABILITY_BACKDROP:
        case ABILITY_THROW:
        case ABILITY_UFO:
        case ABILITY_STAR_ROD:
            ActorPlayDefaultHitSfx();
            break;
        }
    }
    else
    {
        ActorPlayDefaultHitSfx();
    }
}

void ActorPlayDefaultHitSfx(void)
{
    switch (gCurTask->actorKind)
    {
    case ACTOR_KIND_ENEMY:
    case 3:
    case ACTOR_KIND_CHILD:
    case ACTOR_KIND_OBJECT:
        PlaySfx(127);
        break;
    case ACTOR_KIND_MID_BOSS:
    case ACTOR_KIND_BOSS:
        PlaySfx(508);
        break;
    }
}

void ActorStartHitStun(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    TaskSetSkipMask((TASK_SKIP_COROUTINE | TASK_SKIP_MOVE | TASK_SKIP_UPDATE), gCurTaskIdx);
    u = gCurTask;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    a->savedFrame = u->frame;
    TaskSetFrame(0);
    gCurTask->lateUpdateCallback = (u32)ActorHitStunLateUpdate;
    a->hitStunTimer = 11;
    v = gCurTask;
    v->u8C.actor->savedPaletteBits = v->tileWord & 0xF000;
    if (v->hitEffect > 3)
        v->hitEffect = 0;
    ActorAttachEffect(gCurTask->hitEffect, 1);
}

void ActorEndHitStun(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    TaskSetSkipMask(0, gCurTaskIdx);
    u = gCurTask;
    u->lateUpdateCallback = 0;
    u->pixelX = u->posX >> 16;
    u->pixelY = u->posY >> 16;
    u->frame = a->savedFrame;
    u->tileWord = (u->tileWord & 0xFFF) | u->u8C.actor->savedPaletteBits;
}

u32 ActorReactToDamage(void)
{
    struct Actor *a;
    struct ActorVt *p;
    u8 r;

    a = gCurTask->u8C.actor;
    p = (struct ActorVt *)a->hitReactions;
    r = 0;
    ActorPlayHitSfx();
    if ((gCurTask->actorKind == ACTOR_KIND_MID_BOSS || gCurTask->actorKind == ACTOR_KIND_BOSS) && a->hitState != 2)
        HudAnimateTaskHpBar();
    if (p != NULL)
    {
        if (p->damageKind != -1)
        {
            ActorStartHitStun();
            r = 0;
        }
        else if (p->damageCallback != 0)
        {
            r = ((u8 (*)(void))p->damageCallback)();
        }
        else
        {
            sub_0806ee2c();
        }
    }
    else
    {
        sub_0806ee2c();
    }
    return r;
}

void ActorHitStunShake(void)
{
    struct Task *t;
    struct Actor *a;
    s16 j;

    t = gCurTask;
    a = t->u8C.actor;
    j = gUnk_0873E5A4[(s8)a->hitStunTimer] * 2;
    t->pixelX += gUnk_0873E58C[j];
    t->pixelY += gUnk_0873E58C[j + 1];
    if ((s8)--a->hitStunTimer < 0)
        ActorEndHitStun();
}

void ActorHitStunBlink(void)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    if ((a->hitStunTimer & 1) == 0)
        t->tileWord = (t->tileWord & 0xFFF) | 0xF000;
    else
        t->tileWord = (t->tileWord & 0xFFF) | a->savedPaletteBits;
}

void ActorHitStunLateUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
    ActorHitStunBlink();
    ActorHitStunShake();
}

void ActorPlayBurstDefeatAnim(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == TASK_GLUNK_SHOT || gTaskSlotTypes[gCurTaskIdx] == TASK_SHOTZO_CANNONBALL
     || gTaskSlotTypes[gCurTaskIdx] == TASK_GIP_STAR)
        PlaySmallBlastAnim();
    else
        PlayRayBurstAnim();
}

void ActorFaceHitter(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_MID_BOSS || t->actorKind == ACTOR_KIND_BOSS)
        gCurTask->facing = TaskGetFacingToward(t->hitterPlayer);
}

s16 TaskGetHitAngle(void)
{
    s16 r;

    r = 0;
    switch ((s8)gCurTask->hitDirection)
    {
    case 7:
        r += 64;
    case 6:
        r += 64;
    case 5:
        r += 64;
    case 4:
        r += 64;
    case 3:
        r += 64;
    case 2:
        r += 64;
    case 1:
        r += 64;
    case 0:
        break;
    }
    return r;
}

void ActorPlayRandomDefeatSfx(void)
{
    u32 v;

    switch (RandomRange(3))
    {
    default:
        v = 167;
        break;
    case 0:
        v = 109;
        break;
    case 1:
        v = 166;
        break;
    }
    ActorPlaySfx(v, 0);
}

void ActorStartDrown(s32 a)
{
    struct Task *t;
    struct Actor *b;

    t = gCurTask;
    b = t->u8C.actor;
    if (a == -2)
        t->actorDrownFrame = 0;
    else
        t->actorDrownFrame = a;
    b->hitState = 2;
    ActorSetTerrainHandlers((u32)gActorDrownTerrainHandlers);
    if (gCurTask->onGround & 1)
        ActorSetState(ACTOR_DROWN_STATE_WAIT);
    else
        ActorSetState(ACTOR_DROWN_STATE_SINK);
    TaskSetEntry(ActorDrownInit, gCurTaskIdx);
}

/* No return value: the ROM's epilogue is `pop {r0}; bx r0`.  Its caller
   PickupReactToHit nevertheless propagates whatever r0 holds - see the comment
   there. */
void PickupCollect(void)
{
    struct Task *t;

    if (gCurTask->u76.subtype != 0)
        MarkRoomObjectUsed(gCurTaskIdx);
    t = gCurTask;
    switch (t->u76.subtype)
    {
    case 1:
        if (gLocalPlayer == t->hitterSlot)
            PlaySfx(220);
        AddPlayerLives(1, gCurTask->hitterSlot);
        ActorDestroy();
        break;
    case 3:
        if (gLocalPlayer == t->hitterSlot)
            PlaySfx(198);
        PlayerGiveInvincibleCandy(gCurTask->hitterSlot);
        ActorDestroy();
        break;
    case 2:
        if (gLocalPlayer == t->hitterSlot)
            PlaySfx(198);
        ActorSetState(0);
        TaskSetEntry(PickupHeal, gCurTaskIdx);
        break;
    case 4:
        if (gLocalPlayer == t->hitterSlot)
            PlaySfx(198);
        ActorSetState(1);
        TaskSetEntry(PickupHeal, gCurTaskIdx);
        break;
    default:
        ActorDestroy();
        break;
    }
}

u32 ActorReactToDefeat(void)
{
    struct Actor *a;
    struct ActorVt *p;
    struct Task *t;
    u8 r;

    a = gCurTask->u8C.actor;
    p = (struct ActorVt *)a->hitReactions;
    r = 0;
    ActorPlayHitSfx();
    if (gCurTask->actorKind == ACTOR_KIND_MID_BOSS || gCurTask->actorKind == ACTOR_KIND_BOSS)
        HudAnimateTaskHpBar();
    t = gCurTask;
    if (t->actorKind == ACTOR_KIND_MID_BOSS)
        ActorAwardScore(t->hitterPlayer, 1);
    else
        ActorAwardScore(t->hitterPlayer, 2);
    if (p != NULL)
    {
        if (p->defeatKind != -1)
        {
            TaskSetEntry(ActorDie, gCurTaskIdx);
            r = 1;
        }
        else
        {
            if (gCurTask->actorKind == ACTOR_KIND_MID_BOSS)
            {
                if (gGameState != GAME_STATE_ARENA)
                    PlaySfx(510);
                else
                    PlaySfx(514);
            }
            if (p->defeatCallback != 0)
                r = ((u8 (*)(void))p->defeatCallback)();
            else
                sub_0806ee2c();
        }
        a->hitState = 2;
    }
    else
    {
        sub_0806ee2c();
    }
    if (a->keepExtraOnDefeat == 0)
        a->extraFrame = 0xFFFF;
    return r;
}
