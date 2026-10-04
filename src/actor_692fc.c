/* game_code_and_rodata 0x080692FC-0x0806A344 (issue #64, module M18 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080692FC 0x0806A344 src/actor_692fc.c --newpb
 *
 * The player-input dispatch layer of the actor core.  Six probe wrappers
 * (0x080694E0-0x080696A0) snapshot the current task's directional state into a
 * 6-byte stack record (ActorGetTerrainBox) and hand it to one of the input decoders
 * at 0x0801BCAC..0x0801C3A4; the three big dispatchers (ActorCollideTerrain,
 * sub_080696a0, sub_08069888) then walk the actor's seven-entry handler table
 * at Actor.terrainHandlers, calling the first handler that claims the frame.  The tail
 * of the module is the class-1 "carried" task body: state machine entry
 * points (ActorReactToHitKind/sub_08069bbc), the ActorPlayHitSfx sound dispatcher and
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
struct InputState
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 unk05;
};

/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

extern u32 RandomRange(u32 range);
extern void PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void TerrainCollideBox(struct InputState *p);
extern void TerrainCollideBoxInCameraBounds(struct InputState *p);
extern void TerrainCollideBoxWalls(struct InputState *p);
extern void TerrainCollideBoxCeilingAndFloor(struct InputState *p);
extern void TerrainCollideBoxFloor(struct InputState *p);
extern void TerrainCollideBoxAlongVelocity(struct InputState *p);
extern void TerrainCollidePointPushOut(struct InputState *p);
extern u32 TerrainCollidePointStop(struct InputState *p);
extern u32 sub_0802205c(struct InputState *p);
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
    struct InputState v;

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
        CreateChildTaskAt(140, u->pixelX, ((s16 *)gTerrainResult)[i], 0);
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
    struct InputState v;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    ActorGetTerrainBox(&v);
    sub_0802205c(&v);
}

u32 sub_0806951c(void)
{
    struct InputState v;
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

u32 sub_0806956c(void)
{
    struct InputState v;
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

u32 sub_080695bc(void)
{
    struct InputState v;
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

u32 sub_08069604(void)
{
    struct InputState v;
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

u32 sub_08069660(void)
{
    struct InputState v;
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

u32 sub_080696a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

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
    sub_080b460c();
    u = gCurTask;
    if ((u->waterFlags & 0x80) != 0)
        CreateChildTaskAt(140, u->pixelX, ((s16 *)gTerrainResult)[i], 0);
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

u32 sub_08069888(void)
{
    struct Task *t;
    struct Task *u;
    struct Actor *a;
    u32 fn;
    s32 i;
    u8 r;
    s8 k;
    s8 f;
    struct InputState v;

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
        CreateChildTaskAt(140, u->pixelX, ((s16 *)gTerrainResult)[i], 0);
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

u32 sub_080699a8(void)
{
    struct Task *t;
    struct Task *u;
    s16 y;
    s16 m;
    s16 d;

    if (gTaskSlotTypes[gCurTaskIdx] != -1)
    {
        t = gCurTask;
        if ((t->unk24 & 0xFFFF0000) != 0
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
            u->pixelY = u->unk24;
            u->posX = u->pixelX << 16;
            u->posY = u->pixelY << 16;
            return 1;
        }
    }
    return 0;
}

void ActorGetTerrainBox(struct InputState *out)
{
    struct Task *t;
    struct Actor *a;

    t = gCurTask;
    a = t->u8C.actor;
    out->unk01 = ((struct InputState *)a->terrainBox)->unk01;
    out->unk02 = ((struct InputState *)a->terrainBox)->unk02;
    out->unk03 = ((struct InputState *)a->terrainBox)->unk03;
    if (t->facing == -1)
    {
        out->unk00 = -((struct InputState *)a->terrainBox)->unk00;
        out->unk04 = -((struct InputState *)a->terrainBox)->unk05;
        out->unk05 = -((struct InputState *)a->terrainBox)->unk04;
    }
    else
    {
        out->unk00 = ((struct InputState *)a->terrainBox)->unk00;
        out->unk04 = ((struct InputState *)a->terrainBox)->unk04;
        out->unk05 = ((struct InputState *)a->terrainBox)->unk05;
    }
}

void sub_08069ac4(s32 i)
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

u32 sub_08069b84(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    return ActorReactToHitKind(sub_08069c48());
}

u32 sub_08069bbc(void)
{
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    gCurTask->facing = 1;
    r = 0;
    switch ((s8)gCurTask->hitKind)
    {
    case 1:
    case 2:
    case 5:
    case 7:
        /* PickupCollect is void in the ROM but its result is consumed here:
           the original had no prototype in scope at this point, so the
           implicit `int ()` declaration was used.  The cast reproduces the
           direct `bl` without tripping -Wimplicit -Werror. */
        r = ((u32 (*)(void))PickupCollect)();
        break;
    case 3:
    case 4:
        r = ActorAttachToHitter();
        break;
    case 6:
    case 8:
        break;
    }
    return r;
}

s8 sub_08069c48(void)
{
    struct Task *t;
    u8 *g;
    u8 v;
    s32 c;

    t = gCurTask;
    if ((s8)t->hitKind != 0)
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
    if (t->u8C.actor->unk06 == 16 || t->u8C.actor->unk06 == 32)
    {
        u = &gTasks[t->hitterSlot];
        switch (u->u80.attackAbility)
        {
        case 3:
            PlaySfx(145);
            break;
        case 4:
            PlaySfx(146);
            break;
        case 9:
            PlaySfx(132);
            break;
        case 12:
            PlaySfx(139);
            break;
        case 13:
        case 14:
            PlaySfx(142);
            break;
        case 0:
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
        case 8:
        case 10:
        case 11:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
            sub_08069d78();
            break;
        }
    }
    else
    {
        sub_08069d78();
    }
}

void sub_08069d78(void)
{
    switch (gCurTask->actorKind)
    {
    case 0:
    case 3:
    case 4:
    case 5:
        PlaySfx(127);
        break;
    case 1:
    case 2:
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
    TaskSetSkipMask(7, gCurTaskIdx);
    u = gCurTask;
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
    a->savedFrame = u->frame;
    TaskSetFrame(0);
    gCurTask->lateUpdateCallback = (u32)sub_08069fb0;
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
    if ((gCurTask->actorKind == 1 || gCurTask->actorKind == 2) && a->hitState != 2)
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

void sub_08069f0c(void)
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

void sub_08069f70(void)
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

void sub_08069fb0(void)
{
    ActorCheckHits();
    ActorReactToHit();
    sub_08069f70();
    sub_08069f0c();
}

void sub_08069fc8(void)
{
    if (gTaskSlotTypes[gCurTaskIdx] == 107 || gTaskSlotTypes[gCurTaskIdx] == 109
     || gTaskSlotTypes[gCurTaskIdx] == 137)
        PlaySmallBlastAnim();
    else
        PlayRayBurstAnim();
}

void ActorFaceHitter(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->actorKind == 1 || t->actorKind == 2)
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
        t->unk18 = 0;
    else
        t->unk18 = a;
    b->hitState = 2;
    ActorSetTerrainHandlers((u32)gActorDrownTerrainHandlers);
    if (gCurTask->onGround & 1)
        ActorSetState(1);
    else
        ActorSetState(0);
    TaskSetEntry(ActorDrownInit, gCurTaskIdx);
}

/* No return value: the ROM's epilogue is `pop {r0}; bx r0`.  Its caller
   sub_08069bbc nevertheless propagates whatever r0 holds - see the comment
   there. */
void PickupCollect(void)
{
    struct Task *t;

    if (gCurTask->unk76 != 0)
        MarkRoomObjectUsed(gCurTaskIdx);
    t = gCurTask;
    switch (t->unk76)
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
        sub_0804087c(gCurTask->hitterSlot);
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
    if (gCurTask->actorKind == 1 || gCurTask->actorKind == 2)
        HudAnimateTaskHpBar();
    t = gCurTask;
    if (t->actorKind == 1)
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
            if (gCurTask->actorKind == 1)
            {
                if (gGameState != 19)
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
    if (a->unk0D == 0)
        a->extraFrame = 0xFFFF;
    return r;
}
