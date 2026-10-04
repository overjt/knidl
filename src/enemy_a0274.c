
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "room.h"
#include "camera.h"
#include "actor.h"
#include "enemy.h"

/* Externals */
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern s32 PlaySfx(s32 id);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestScreenShake(s32 a);
extern void ActorSetState(u16 v);
extern void ActorSetExtraAttackBox(u32 v);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern u8 ActorCollideTerrain(void);

void CreateKingDededeLandingStar(void)
{
    CreateChildTaskAtOffsetFacing(TASK_KING_DEDEDE_LANDING_STAR, -24, -8, 0);
}

void CreateKingDededeAirPuff(void)
{
    struct ActorSpawn sp;
    struct Actor *a;

    a = gCurTask->u8C.actor;
    sp.subtype = 11;
    sp.taskType = TASK_KING_DEDEDE_AIR_PUFF;
    sp.variant = KING_DEDEDE_AIR_PUFF_VARIANT_INIT;
    sp.spawnArg = 0;
    sp.x = 32;
    sp.y = 16;
    sp.tileWord = a->savedTileWord;
    sp.checkTerrain = 0;
    gCurTask->kingDededeChildSlot = CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void KingDededeCreateImpactStars(u8 a)
{
    struct Task *t;
    u16 x;
    u16 y;
    s16 yy;
    s32 d;

    t = gCurTask;
    y = t->pixelY + 24;
    if (a == 1)
    {
        x = t->pixelX + 32;
        d = 1;
    }
    else
    {
        x = t->pixelX + (t->facing << 5);
        d = 0;
    }
    yy = y;
    CreateInhalableStar((s16)x, yy, d, 0);
    if (a == 1)
        CreateInhalableStar(gCurTask->pixelX - 32, yy, -1, 0);
}

void KingDededeStartWalk(void)
{
    struct Task *t;
    s32 n;
    s32 r;

    t = gCurTask;
    switch (t->kingDededeNextState)
    {
    case 3:
        n = 46;
        break;
    case 7:
        r = CreateChildTaskHere(TASK_KING_DEDEDE_HAMMER_HIT_BOX, 0);
        t = gCurTask;
        t->kingDededeChildSlot = r;
        n = 52;
        break;
    case 8:
        TaskGetNearestPlayerPos();
        gCurTask->kingDededeWalkTargetX = gUnk_030023B4;
        if (gUnk_030023B4 < gRoomBounds[0] - 87)
            gCurTask->kingDededeWalkTargetX = gRoomBounds[0] - 87;
        if (gCurTask->kingDededeWalkTargetX > gRoomBounds[1] + 87)
            gCurTask->kingDededeWalkTargetX = gRoomBounds[1] + 87;
        if (gCurTask->pixelX == gUnk_030023B4)
            goto tail;
        if ((u8)TaskGetXDirBitToNearestPlayer() == 4)
        {
            t = gCurTask;
            t->kingDededeWalkTargetX -= 16;
        }
        else
        {
            t = gCurTask;
            t->kingDededeWalkTargetX += 16;
        }
        t = gCurTask;
        n = 8;
        break;
    default:
        t = gCurTask;
        n = 0;
        break;
    }
    t->kingDededeWalkStopDist = n;
tail:
    if (gCurTask->kingDededeWalkTargetX < gRoomBounds[0] - 87)
        gCurTask->kingDededeWalkTargetX = gRoomBounds[0] - 87;
    if (gCurTask->kingDededeWalkTargetX > gRoomBounds[1] + 87)
        gCurTask->kingDededeWalkTargetX = gRoomBounds[1] + 87;
}

void KingDededePickSlamKind(void)
{
    struct Task *t;
    s32 n;
    s32 one;

    TaskGetNearestPlayerScreenPos();
    t = gCurTask;
    one = 1;
    t->kingDededeSlamCount = one;
    if (gUnk_030023D4 <= 111)
    {
        t->kingDededeSlamKind = 2;
    }
    else
    {
        n = (t->kingDededeSlamSetupCount + 1) & 3;
        t->kingDededeSlamSetupCount = n;
        if (n == 0)
        {
            t->kingDededeSlamKind = one;
            t->kingDededeSlamCount = 4;
        }
        else
        {
            t->kingDededeSlamKind = 0;
        }
    }
}

void KingDededeAimHighJump(u8 a)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u16 f;

    if (a == 0)
    {
        if (abs(TaskGetNearestPlayerDx()) <= 21)
            gCurTask->kingDededeJumpForward = 0;
        else
            gCurTask->kingDededeJumpForward = 1;
    }
    else
    {
        t = gCurTask;
        f = t->facing;
        TaskFaceNearestPlayer();
        u = gCurTask;
        if ((s16)f != u->facing)
            u->kingDededeJumpForward = 0;
        else
            u->kingDededeJumpForward = 1;
    }
    v = gCurTask;
    if (v->kingDededeJumpForward == 1)
    {
        if (v->health < (s16)v->kingDededeThirdHealth)
            TaskSetMotionXFacing(136 << 10, 0x5A5A5A5A);
        else
            TaskSetMotionXFacing(160 << 9, 0x5A5A5A5A);
    }
}

u8 KingDededeLand(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    switch (t->state)
    {
    case 3:
        TaskStopY();
        break;
    case 4:
        t->kingDededeFloatBumpTimer = 4;
        t->onGround = 0;
        sub_0806d08c(24, 8, 32);
        TaskSetFrame(23);
        u = gCurTask;
        TaskSetMotionY(-u->velY, -u->accelY, u->speedLimitY);
        break;
    }
    return 0;
}

void KingDededeHitWall(void)
{
    gCurTask->velX = 0;
}

u8 KingDededeHitCeiling(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->state == 4)
    {
        t->kingDededeFloatBumpTimer = 4;
        TaskSetFrame(23);
        u = gCurTask;
        TaskSetMotionY(-u->velY, -u->accelY, u->speedLimitY);
    }
    return 0;
}

void sub_080a05c8(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk34 = 0;
    t->kingDededeSlamSetupCount = 0;
    t->unk2C = 0;
    t->unk28 = 0;
    t->unk24 = 0;
    t->unk20 = 0;
    t->kingDededeNextState = 0;
    t->kingDededePickCount = 0;
    t->kingDededeChildSlot = 0xFFFF;
    gUnk_02007D00[8] = -1;
    t->kingDededeChildSlot = -1;
    TaskFaceNearestPlayer();
    ActorSetState(KING_DEDEDE_STATE_INTRO);
}

u8 sub_080a060c(void)
{
    struct Task *t;
    s16 dx;
    u16 dy;
    s16 y;

    t = gCurTask;
    dx = t->pixelX - gViewRect[0];
    dy = t->pixelY - gViewRect[2];
    if ((u16)(dx + 19) <= 278)
    {
        y = dy;
        if (y > -20)
        {
            if (y <= 179)
                return 1;
        }
    }
    return 0;
}

void KingDededeStartWait(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 i;
    s32 n;
    s32 n2;
    s32 n3;

    if (sub_080a060c() != 0)
    {
        i = RandomRange(3);
        n = ActorStartAnim(gUnk_08748384);
        t = gCurTask;
        t->actorAnimDelay24 = n;
        if (t->health < (s16)t->kingDededeThirdHealth)
        {
            i += 3;
            n2 = ActorStartAnim(gKingDededeStartWaitAnim);
            gCurTask->actorAnimDelay24 = n2;
        }
        u = gCurTask;
        if (u->unk2C != 0)
            u->kingDededeWaitTimer = 0;
        else
            u->kingDededeWaitTimer = gUnk_08748374[i];
        gCurTask->unk2C = 0;
    }
    else
    {
        n3 = ActorStartAnim(gKingDededeStartWaitAnim);
        v = gCurTask;
        v->actorAnimDelay24 = n3;
        v->kingDededeWaitTimer = 1;
    }
}

void KingDededeChooseNextState(void)
{
    struct Task *t;
    struct Actor *a;
    u16 *tab;
    u16 *p;
    s32 q;
    s32 i;
    s32 n;

    t = gCurTask;
    a = t->u8C.actor;
    n = t->kingDededePickCount + 1;
    t->kingDededePickCount = n;
    if (n == 6)
    {
        t->kingDededePickCount = 0;
        ActorSetState(KING_DEDEDE_STATE_FLOAT);
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
    else
    {
        tab = gUnk_087482A8[a->prevState];
        i = RandomRange(8);
        q = i * 4 + (s32)tab;
        ActorSetState(*(u16 *)q);
        gCurTask->kingDededeNextState = *(u16 *)(q + 2);
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeJumpSlam(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    TaskSetFrame(37);
    TaskYieldTrampoline(24);
    gCurTask->onGround = 0;
    t = gCurTask;
    t->frame++;
    PlaySfx(0x21E);
    TaskSetMotionY(0xFFFB0000, 160 << 6, 160 << 11);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(136 << 2);
    TaskSetFrame(36);
    TaskYieldTrampoline(4);
    u = gCurTask;
    u->frame--;
    TaskYieldTrampoline(1);
    v = gCurTask;
    v->frame--;
    v->accelY = 160 << 6;
    v->speedLimitY = 160 << 11;
    while ((gCurTask->onGround & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(0x1F7);
    KingDededeCreateImpactStars(0);
    RequestScreenShake(4);
    TaskYieldTrampoline(20);
}

void KingDededeGroundSlam(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    u16 a;
    u16 b;
    u16 c;
    u16 d;
    s32 z;

    i = (u16)(gCurTask->kingDededeSlamKind * 3);
    a = gUnk_08748420[i];
    b = gUnk_08748420[i + 1];
    c = gUnk_08748420[i + 2];
    d = gUnk_08748420[i + 3];
    TaskSetFrame(39);
    TaskYieldTrampoline(24);
    PlaySfx(136 << 2);
    TaskSetFrame(36);
    TaskYieldTrampoline(a);
    t = gCurTask;
    t->frame--;
    z = 0;
    TaskYieldTrampoline(b);
    RequestScreenShake(d);
    KingDededeCreateImpactStars(0);
    u = gCurTask;
    u->frame--;
    TaskYieldTrampoline(c);
    v = gCurTask;
    v->kingDededeSlamCount--;
    v->kingDededeLoopCount = z;
    while ((s16)gCurTask->kingDededeLoopCount < gCurTask->kingDededeSlamCount)
    {
        w = gCurTask;
        w->frame--;
        TaskYieldTrampoline(6);
        PlaySfx(136 << 2);
        gCurTask->frame = 36;
        TaskYieldTrampoline(a);
        gCurTask->frame--;
        TaskYieldTrampoline(b);
        gCurTask->frame--;
        TaskYieldTrampoline(c);
        gCurTask->kingDededeLoopCount++;
    }
}

void sub_080a094c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 anim;

    t = gCurTask;
    anim = (t->kingDededeNextState == 7) ? 30 : 13;
    while (1)
    {
        gCurTask->onGround = 0;
        TaskYieldTrampoline(1);
        TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
        gCurTask->velY = 0;
        PlaySfx(0x201);
        TaskSetFrame((s16)anim);
        TaskYieldTrampoline(4);
        gCurTask->onGround = 0;
        u = gCurTask;
        u->velY = 0xFFFF8000;
        u->frame++;
        TaskYieldTrampoline(10);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0;
        PlaySfx(0x201);
        v = gCurTask;
        v->frame++;
        TaskYieldTrampoline(4);
        gCurTask->onGround = 0;
        w = gCurTask;
        w->velY = 0xFFFF8000;
        w->frame++;
        TaskYieldTrampoline(10);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(10);
    }
}

void KingDededeInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)KingDededeUpdate;
    sub_080a05c8();
    CallTableEntry(gCurTask->state, 11, gKingDededeStates);
}

void KingDededeUpdate(void)
{
    if (ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 11, gKingDededeStateUpdates);
    if ((s16)gTaskSlotTypes[gCurTaskIdx] != -1)
        sub_0809fca4();
}

void KingDededeHitStunLateUpdate(void)
{
    s32 n;
    s32 m;

    sub_0809fca4();
    ActorFlashPalette(gUnk_082D8638, 16);
    if (gUnk_02006190[5] <= 0)
    {
        n = gUnk_02006190[4];
        if (n <= 11)
        {
            gUnk_02006190[4] = n + 2;
            gUnk_02006190[5] = gUnk_08748430[n + 3];
        }
        else
        {
            gUnk_02006190[4] = 0;
            m = gUnk_02006190[3] - 1;
            gUnk_02006190[3] = m;
            gUnk_02006190[5] = gUnk_08748430[1];
            if (m <= 0)
            {
                KingDededeEndHitStun();
                return;
            }
        }
    }
    gUnk_02006190[5]--;
    gCurTask->pixelX += gUnk_08748430[gUnk_02006190[4]];
}

void KingDededeEnterState(void)
{
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->state, 11, gKingDededeStates);
}

void KingDededeIntro(void)
{
    struct Task *t;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = z;
    TaskStop();
    ActorCollideTerrain();
    ActorIntroPoseUntilScrollLocked(gUnk_08748384);
    ActorIntroPoseUntilHpBarFull(gUnk_08748384);
    ActorSetExtraAttackBox((u32)gKingDededeIntroExtraAttackBox);
    gCurTask->unk2C = z;
    KingDededeStartWait();
    TaskSleepForever();
}

void KingDededeIntroUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gCurTask;
    n = ActorTickAnimFacingNearestPlayer(t->actorAnimDelay24);
    u = gCurTask;
    u->actorAnimDelay24 = n;
    n = u->kingDededeWaitTimer - 1;
    u->kingDededeWaitTimer = n;
    if (n == 0)
    {
        u->kingDededeNextState = 3;
        ActorSetState(KING_DEDEDE_STATE_WALK);
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeWait(void)
{
    struct Task *t;
    s32 one;

    t = gCurTask;
    one = 1;
    t->updateState = one;
    TaskStop();
    gCurTask->onGround = one;
    KingDededeStartWait();
    TaskSleepForever();
}

void KingDededeWaitUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gCurTask;
    n = ActorTickAnimFacingNearestPlayer(t->actorAnimDelay24);
    u = gCurTask;
    u->actorAnimDelay24 = n;
    if (u->kingDededeWaitTimer == 0)
        KingDededeChooseNextState();
    else
        u->kingDededeWaitTimer--;
}

void KingDededeWalk(void)
{
    gCurTask->updateState = KING_DEDEDE_STATE_WALK;
    TaskStop();
    KingDededeStartWalk();
    sub_080a094c();
}

void KingDededeWalkUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 v2;
    s32 w;
    s32 f;
    s32 f2;

    t = gCurTask;
    if (t->kingDededeNextState == 8)
    {
        v = t->kingDededeWalkTargetX - t->pixelX;
        f = t->facing;
        if (v < 0)
        {
            if (f != -1)
                goto zero;
        }
        else if (f != 1)
        {
        zero:
            v = 0;
        }
        w = v;
    }
    else
    {
        v2 = TaskGetNearestPlayerDx();
        f2 = gCurTask->facing;
        if (v2 < 0)
        {
            if (f2 != -1)
                goto zero2;
        }
        else if (f2 != 1)
        {
        zero2:
            v2 = 0;
        }
        w = v2;
    }
    w = abs(w);
    u = gCurTask;
    if (w <= u->kingDededeWalkStopDist && u->kingDededeNextState != 11)
    {
        u->onGround = 1;
        ActorSetState(gCurTask->kingDededeNextState);
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeJump(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 z;
    s32 k;
    s32 r;

    t = gCurTask;
    z = 0;
    t->updateState = KING_DEDEDE_STATE_JUMP;
    TaskStop();
    gCurTask->onGround = z;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFC0000, 128 << 8, 128 << 11);
    TaskSetFrame(17);
    while ((gCurTask->onGround & 1) == 0)
        TaskYieldTrampoline(1);
    PlaySfx(0x1F7);
    RequestScreenShake(4);
    r = CreateDustBurst(24, 32);
    u = gCurTask;
    u->kingDededeChildSlot = r;
    CreateKingDededeLandingStar();
    TaskSetMotionXFacing(160 << 9, k = 0x5A5A5A5A);
    v = gCurTask;
    v->frame++;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 9, k);
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 8, k);
    w = gCurTask;
    w->frame++;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 6, k);
    TaskYieldTrampoline(10);
    TaskStop();
    ActorSetState(KING_DEDEDE_STATE_WAIT);
    TaskSleepForever();
}

void KingDededeJumpUpdate(void)
{
    struct Task *t;
    s16 *p;

    t = gCurTask;
    if (t->state != KING_DEDEDE_STATE_JUMP)
    {
        p = &t->kingDededeChildSlot;
        if (*p != -1)
        {
            TaskFree(*p);
            gCurTask->kingDededeChildSlot = 0xFFFF;
        }
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeFloat(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 i;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = KING_DEDEDE_STATE_FLOAT;
    TaskStop();
    u = gCurTask;
    u->kingDededeFloatTimer = -2;
    u->kingDededeFloatBumpTimer = -2;
    TaskSetFrame(25);
    TaskYieldTrampoline(20);
    gCurTask->kingDededeLoopCount = z;
    do
    {
        TaskSetFrame(26);
        TaskYieldTrampoline(10);
        gCurTask->frame++;
        TaskYieldTrampoline(10);
    } while ((s16)++gCurTask->kingDededeLoopCount <= 3);
    PlaySfx(0x21F);
    gCurTask->onGround = 0;
    v = gCurTask;
    v->frame++;
    for (i = 0; i <= 10; i += 2)
    {
        gCurTask->velY = gUnk_087483B8[i];
        TaskYieldTrampoline(gUnk_087483B8[i + 1]);
    }
    TaskStop();
    TaskSetFrame(22);
    w = gCurTask;
    w->speedLimitY = 192 << 9;
    w->speedLimitX = 192 << 9;
    w->kingDededeFloatTimer = 150 << 1;
    while (1)
    {
        TaskYieldTrampoline(12);
        TaskFaceNearestPlayer();
    }
}

void KingDededeFloatUpdate(void)
{
    struct Task *t;
    s32 m2;
    s32 n;
    s32 n2;

    t = gCurTask;
    n = t->kingDededeFloatTimer;
    m2 = -2;
    if (n == m2)
        return;
    if (n == 0)
    {
        ActorSetState(KING_DEDEDE_STATE_EXHALE);
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
        return;
    }
    t->kingDededeFloatTimer = n - 1;
    n2 = t->kingDededeFloatBumpTimer;
    if (n2 != m2)
    {
        t->kingDededeFloatBumpTimer = n2 - 1;
        if (n2 <= 0)
        {
            TaskSetFrame(22);
            gCurTask->kingDededeFloatBumpTimer = m2;
        }
    }
    if ((u8)TaskGetYDirBitToNearestPlayer() == 2)
        gCurTask->accelY = 0xFFFFFB00;
    else
        gCurTask->accelY = 160 << 3;
    if ((u8)TaskGetXDirBitToNearestPlayer() == 8)
        gCurTask->accelX = 0xFFFFF800;
    else
        gCurTask->accelX = 128 << 4;
    TaskSetFrame(gCurTask->frame);
}

void KingDededeExhale(void)
{
    struct Task *t;
    s32 z;
    s32 i;

    t = gCurTask;
    z = 0;
    t->updateState = KING_DEDEDE_STATE_EXHALE;
    TaskStop();
    gCurTask->onGround = z;
    TaskSetFrame(22);
    TaskSetFrame(29);
    PlaySfx(0x21A);
    CreateKingDededeAirPuff();
    for (i = 0; i <= 3; i++)
    {
        TaskSetMotionXFacing(gUnk_08748410[i], 0x5A5A5A5A);
        TaskYieldTrampoline(2);
    }
    TaskStopX();
    TaskSetFrame(43);
    TaskSetMotionY(0, 148 << 6, 192 << 10);
    while ((gCurTask->onGround & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(41);
    TaskYieldTrampoline(10);
    ActorSetState(KING_DEDEDE_STATE_WAIT);
    TaskSleepForever();
}

void KingDededeExhaleUpdate(void)
{
    if (gCurTask->state != KING_DEDEDE_STATE_EXHALE)
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
}

void KingDededeHighJump(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = KING_DEDEDE_STATE_HIGH_JUMP;
    TaskStop();
    TaskSetFrame(41);
    TaskYieldTrampoline(24);
    gCurTask->onGround = z;
    KingDededeAimHighJump(0);
    u = gCurTask;
    u->frame++;
    PlaySfx(0x21E);
    TaskSetMotionY(0xFFFB0000, 160 << 6, 160 << 11);
    while (gCurTask->velY < 0)
        TaskYieldTrampoline(1);
    TaskStop();
    v = gCurTask;
    v->frame++;
    v->accelY = 160 << 6;
    v->speedLimitY = 160 << 11;
    if (v->kingDededeJumpForward != 0)
        KingDededeAimHighJump(1);
    while ((gCurTask->onGround & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    PlaySfx(252 << 1);
    RequestScreenShake(2);
    KingDededeCreateImpactStars(1);
    TaskSetFrame(41);
    TaskYieldTrampoline(34);
    ActorSetState(KING_DEDEDE_STATE_WAIT);
    TaskSleepForever();
}

void KingDededeHighJumpUpdate(void)
{
    if (gCurTask->state != KING_DEDEDE_STATE_HIGH_JUMP)
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
}

void KingDededeSlam(void)
{
    gCurTask->updateState = KING_DEDEDE_STATE_SLAM;
    TaskStop();
    KingDededePickSlamKind();
    if (gCurTask->kingDededeSlamKind != 2)
        KingDededeGroundSlam();
    else
        KingDededeJumpSlam();
    ActorSetState(KING_DEDEDE_STATE_WAIT);
    TaskSleepForever();
}

void KingDededeSlamUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != KING_DEDEDE_STATE_SLAM)
    {
        TaskFree(t->kingDededeChildSlot);
        gCurTask->kingDededeChildSlot = 0xFFFF;
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeInhale(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    s16 *p;
    s32 z;
    s32 z2;

    t = gCurTask;
    z = 0;
    t->updateState = KING_DEDEDE_STATE_INHALE;
    TaskStop();
    gCurTask->kingDededeInhaling = z;
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    u = gCurTask;
    u->frame++;
    TaskYieldTrampoline(20);
    sub_080a0098();
    while (gUnk_02007D00[0] != 1)
    {
        TaskSetFrame(26);
        TaskYieldTrampoline(4);
        v = gCurTask;
        v->frame++;
        TaskYieldTrampoline(6);
    }
    w = gCurTask;
    z2 = 0;
    w->kingDededeInhaling = z2;
    if (gUnk_02007D00[9] != -1)
    {
        StopSfxOnPlayer(gUnk_02007D00[9], 0x21B);
        gUnk_02007D00[9] = -1;
    }
    x = gCurTask;
    p = &x->kingDededeChildSlot;
    if (*p != -1)
    {
        TaskFree(*p);
        gCurTask->kingDededeChildSlot = 0xFFFF;
    }
    PlaySfx(135 << 2);
    TaskSetFrame(28);
    gCurTask->kingDededeLoopCount = z2;
    do
    {
        TaskStepForward(-2);
        TaskYieldTrampoline(2);
        TaskStepForward(2);
        TaskYieldTrampoline(2);
    } while ((s16)++gCurTask->kingDededeLoopCount <= 3);
    TaskStepForward(0);
    TaskYieldTrampoline(14);
    ActorSetState(KING_DEDEDE_STATE_SPIT);
    TaskSleepForever();
}

void KingDededeInhaleUpdate(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    if (gUnk_02006190[7] == 1)
    {
        ActorFlashPalette(gUnk_082D8638, 16);
        if (gUnk_02006190[5] <= 0)
        {
            gUnk_02006190[7] = 0;
            ActorClearPaletteOverride();
        }
        else
        {
            gUnk_02006190[5]--;
        }
    }
    t = gCurTask;
    if (t->kingDededeInhaling != 0)
    {
        if (gUnk_02007D00[8] != -1)
        {
            if (t->kingDededeChildSlot != -1)
            {
                TaskFree(t->kingDededeChildSlot);
                gCurTask->kingDededeChildSlot = 0xFFFF;
            }
        }
        sub_080a00ec();
        n = gUnk_02007D00[0];
        if (n == -1)
        {
            u = gCurTask;
            if (u->kingDededeInhaleTimer <= 0)
            {
                if (u->kingDededeChildSlot != -1)
                {
                    TaskFree(u->kingDededeChildSlot);
                    gCurTask->kingDededeChildSlot = 0xFFFF;
                }
                if (gUnk_02007D00[9] != n)
                {
                    StopSfxOnPlayer(gUnk_02007D00[9], 0x21B);
                    gUnk_02007D00[9] = n;
                }
                ActorSetState(KING_DEDEDE_STATE_WAIT);
                TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
            }
            else
            {
                u->kingDededeInhaleTimer--;
            }
        }
    }
    if (gCurTask->state != KING_DEDEDE_STATE_INHALE)
    {
        ActorClearPaletteOverride();
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeSpit(void)
{
    gCurTask->updateState = KING_DEDEDE_STATE_SPIT;
    TaskStop();
    CreateKingDededeStar();
    TaskYieldTrampoline(51);
    ActorSetState(KING_DEDEDE_STATE_WAIT);
    TaskSleepForever();
}

void KingDededeSpitUpdate(void)
{
    if (gUnk_02006190[7] == 1)
    {
        ActorFlashPalette(gUnk_082D8638, 16);
        if (gUnk_02006190[5] <= 0)
        {
            gUnk_02006190[7] = 0;
            ActorClearPaletteOverride();
        }
        else
        {
            gUnk_02006190[5]--;
        }
    }
    if (gCurTask->state != KING_DEDEDE_STATE_SPIT)
    {
        if (gUnk_02006190[5] > 0)
        {
            gUnk_02006190[7] = 0;
            ActorClearPaletteOverride();
        }
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
    }
}

void KingDededeFall(void)
{
    struct Task *t;
    struct Actor *a;
    u8 f;

    t = gCurTask;
    a = t->u8C.actor;
    t->updateState = KING_DEDEDE_STATE_FALL;
    TaskStop();
    TaskSetFrame(43);
    TaskSetMotionY(0, 148 << 6, 192 << 10);
    while ((gCurTask->onGround & 1) == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSetFrame(41);
    TaskYieldTrampoline(10);
    f = a->prevState;
    ActorSetState(KING_DEDEDE_STATE_WAIT);
    a->prevState = (s8)f;
    TaskSleepForever();
}

void KingDededeFallUpdate(void)
{
    if (gCurTask->state != KING_DEDEDE_STATE_FALL)
        TaskSetEntry(KingDededeEnterState, gCurTaskIdx);
}

void KingDededeDefeatedInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)ActorDrawStreamedFrameNearView;
    t->updateCallback = (u32)KingDededeDefeatedUpdate;
    t->frameTable = gKingDededeFrames;
    TaskFaceNearestPlayer();
    ActorSetState(KING_DEDEDE_DEFEATED_STATE_FALL);
    CallTableEntry(gCurTask->state, 2, gKingDededeDefeatedStates);
}

void KingDededeDefeatedUpdate(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->updateState, 2, gKingDededeDefeatedStateUpdates);
}

void KingDededeDefeatedEnterState(void)
{
    TaskFaceNearestPlayer();
    CallTableEntry(gCurTask->state, 2, gKingDededeDefeatedStates);
}
