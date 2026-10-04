#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"

/* External functions */
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 PlaySfx(s32 id);
extern u32 TaskIsOnScreen(void);
extern void TaskSetEntry(void *a, u32 i);
extern void HudStartHpBar();
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void RequestScreenShake(u32 a);
extern void TaskBreakBlocksNoPlayer();
extern void TaskBreakTopBlockRow();
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern s16 ActorComputeHealth(void);
extern s32 CreateInhalableStar(s16 x, s16 y, s16 dir, u8 p8);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_0806951c(void);
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);

/* Module functions */
void sub_080a2b2c();
void ReleaseRoomObject();
s32 LoadRoomEnemyGfx();
s32 LoadRoomMidBossGfx();
s32 LoadRoomBossGfx();
void LoadRoomMetaKnightsGfx();
s32 sub_080b5a94();
s32 SpawnRoomEnemy();
s32 sub_080b5d84();

void Task_NightmarePowerOrb(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *u2;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gNightmarePowerOrbFrames;
    b42 = &t->layer;
    z = 0;
    *b42 = 11;
    sub_08066088(0);
    sub_080b05e8();
    u = *c;
    u->updateCallback = (u32)NightmarePowerOrbUpdate;
    sub_080ae4c4();
    sub_08066580();
    v = *c;
    v->posX = 192 << 16;
    v->posY = 152 << 15;
    v->unk6C = z;
    do
    {
        sub_080ae548();
        sub_080ae548();
        sub_080ae628();
        sub_080ae628();
        sub_080ae79c();
        sub_080ae628();
        sub_080ae628();
        NightmarePowerOrbShoot();
        sub_080ae548();
        sub_080ae548();
        sub_080ae79c();
        sub_080ae628();
        sub_080ae628();
        NightmarePowerOrbDash();
        u2 = gCurTask;
        u2->unk6C++;
    } while ((s16)u2->unk6C <= 1);
    sub_080aeef8();
    sub_080aef30();
}

void NightmarePowerOrbUpdate(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    sub_080aef5c();
    c = &gCurTask;
    CallTableEntry((*c)->updateState, 4, gNightmarePowerOrbStateUpdates);
    t = *c;
    t->hitTimer = 0;
    u = *c;
    if ((s16)u->health != 0)
    {
        sub_08068f68();
        ActorReactToHit();
        sub_080af1d4();
        if (gUnk_02007D00[3] > 0)
            gUnk_02007D00[3]--;
    }
}

void sub_080ae4c4(void)
{
    u16 *q;
    struct Task **c;
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    s32 *p;
    s16 w;

    q = (u16 *)gUnk_082FEFF4;
    RequestCopy(4, ((u32 *)q)[3], OBJ_VRAM0 + 0x2000, q[1] << 5);
    RequestCopy(2, ((u32 *)q)[2], IWRAM_START + 0x15B0, q[0] << 5);
    sub_08063a00((u32)gUnk_0874B450);
    c = &gCurTask;
    t = *c;
    t->u8C.actor->animScript = 0;
    t->unk28 = 0;
    t->facing = 1;
    u = *c;
    u->unk2C = 0;
    p = gUnk_02007D00;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    p[6] = 0;
    p[7] = 0;
    p[8] = 176 << 15;
    p[9] = 0;
    w = ActorComputeHealth();
    t2 = *c;
    t2->unk24 = w;
}

void sub_080ae548(void)
{
    struct Task **c;
    s32 z;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 v4;
    s32 v5;
    s32 v6;
    struct Task *t;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    ActorSetState(0);
    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk2C = z;
    sub_080aefd4((u32)gUnk_0874AD74);
    (*c)->updateState = z;
    u1 = *c;
    v1 = 0xFFFF0000;
    u1->velY = v1;
    TaskYieldTrampoline(10);
    u2 = *c;
    v2 = 0xFFFF8000;
    u2->velY = v2;
    TaskYieldTrampoline(10);
    u3 = *c;
    v3 = 0xFFFFE000;
    u3->velY = v3;
    TaskYieldTrampoline(10);
    u4 = *c;
    v4 = 128 << 6;
    u4->velY = v4;
    TaskYieldTrampoline(10);
    u5 = *c;
    v5 = 128 << 8;
    u5->velY = v5;
    TaskYieldTrampoline(10);
    u6 = *c;
    v6 = 128 << 9;
    u6->velY = v6;
    TaskYieldTrampoline(10);
    (*c)->velY = v6;
    TaskYieldTrampoline(10);
    (*c)->velY = v5;
    TaskYieldTrampoline(10);
    (*c)->velY = v4;
    TaskYieldTrampoline(10);
    (*c)->velY = v3;
    TaskYieldTrampoline(10);
    (*c)->velY = v2;
    TaskYieldTrampoline(10);
    (*c)->velY = v1;
    TaskYieldTrampoline(10);
    TaskStop();
}

void sub_080ae628(void)
{
    struct Task **c;
    s32 z;
    s32 va;
    s32 vb;
    s32 vc;
    s32 vd;
    s32 ve;
    struct Task *t;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;
    struct Task *u8;
    struct Task *u9;
    struct Task *u10;
    struct Task *u11;
    struct Task *u12;
    struct Task *u13;
    struct Task *u14;
    struct Task *u15;
    struct Task *u16;
    struct Task *u17;
    struct Task *u18;
    struct Task *u19;
    struct Task *u20;

    ActorSetState(1);
    sub_080aefd4((u32)gUnk_0874ADA8);
    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 1;
    u1 = *c;
    va = 0xFFFE0000;
    u1->velX = va;
    u1->velY = va;
    TaskYieldTrampoline(6);
    u2 = *c;
    vb = 0xFFFF0000;
    u2->velX = vb;
    u2->velY = va;
    TaskYieldTrampoline(6);
    u3 = *c;
    u3->velX = z;
    u3->velY = 0xFFFEC000;
    TaskYieldTrampoline(6);
    u4 = *c;
    vc = 128 << 9;
    u4->velX = vc;
    u4->velY = vb;
    TaskYieldTrampoline(6);
    u5 = *c;
    vd = 128 << 10;
    u5->velX = vd;
    u5->velY = 0xFFFF8000;
    TaskYieldTrampoline(6);
    u6 = *c;
    u6->velX = vd;
    u6->velY = 128 << 8;
    TaskYieldTrampoline(6);
    u7 = *c;
    u7->velX = vc;
    u7->velY = vc;
    TaskYieldTrampoline(6);
    u8 = *c;
    u8->velX = z;
    ve = 160 << 9;
    u8->velY = ve;
    TaskYieldTrampoline(6);
    u9 = *c;
    u9->velX = vb;
    u9->velY = vd;
    TaskYieldTrampoline(6);
    u10 = *c;
    u10->velX = va;
    u10->velY = vd;
    TaskYieldTrampoline(6);
    u11 = *c;
    u11->velX = va;
    u11->velY = vd;
    TaskYieldTrampoline(6);
    u12 = *c;
    u12->velX = vb;
    u12->velY = vd;
    TaskYieldTrampoline(6);
    u13 = *c;
    u13->velX = z;
    u13->velY = ve;
    TaskYieldTrampoline(6);
    u14 = *c;
    u14->velX = vc;
    u14->velY = vc;
    TaskYieldTrampoline(6);
    u15 = *c;
    u15->velX = vd;
    u15->velY = 128 << 8;
    TaskYieldTrampoline(6);
    u16 = *c;
    u16->velX = vd;
    u16->velY = 0xFFFF8000;
    TaskYieldTrampoline(6);
    u17 = *c;
    u17->velX = vc;
    u17->velY = vb;
    TaskYieldTrampoline(6);
    u18 = *c;
    u18->velX = z;
    u18->velY = 0xFFFEC000;
    TaskYieldTrampoline(6);
    u19 = *c;
    u19->velX = vb;
    u19->velY = va;
    TaskYieldTrampoline(6);
    u20 = *c;
    u20->velX = va;
    u20->velY = va;
    TaskYieldTrampoline(6);
    TaskStop();
}

void sub_080ae79c(void)
{
    ActorSetState(2);
    sub_080aefd4((u32)gUnk_0874AEAC);
    gCurTask->updateState = 3;
    TaskYieldTrampoline(40);
    CreateNightmarePowerOrbStar(0);
    CreateNightmarePowerOrbStar(1);
    CreateNightmarePowerOrbStar(2);
    CreateNightmarePowerOrbStar(3);
    gUnk_02007D00[3] = 140;
    TaskYieldTrampoline(80);
}

void NightmarePowerOrbShoot(void)
{
    struct Task **c;
    s32 v1;
    s32 v2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;

    ActorSetState(3);
    sub_080aefd4((u32)gUnk_0874ADA8);
    c = &gCurTask;
    (*c)->updateState = 3;
    TaskStop();
    TaskYieldTrampoline(8);
    u1 = *c;
    v1 = 0xFFFA0000;
    u1->velY = v1;
    TaskYieldTrampoline(10);
    NightmarePowerOrbShootStar();
    u2 = *c;
    v2 = 192 << 11;
    u2->velY = v2;
    TaskYieldTrampoline(16);
    NightmarePowerOrbShootStar();
    u3 = *c;
    u3->velY = v1;
    TaskYieldTrampoline(12);
    NightmarePowerOrbShootStar();
    u4 = *c;
    u4->velY = v2;
    TaskYieldTrampoline(16);
    NightmarePowerOrbShootStar();
    u5 = *c;
    u5->velY = v1;
    TaskYieldTrampoline(10);
    NightmarePowerOrbShootStar();
    TaskStop();
}

void NightmarePowerOrbShootStar(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;

    TaskStop();
    TaskYieldTrampoline(8);
    PlaySfx(0x22D);
    CreateNightmarePowerOrbStar(11);
    c = &gCurTask;
    u1 = *c;
    u1->velX = 128 << 12;
    TaskYieldTrampoline(2);
    u2 = *c;
    u2->velX = 128 << 11;
    TaskYieldTrampoline(2);
    u3 = *c;
    u3->velX = 0xFFFC0000;
    TaskYieldTrampoline(2);
    u4 = *c;
    u4->velX = 0xFFF80000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(8);
}

void NightmarePowerOrbDash(void)
{
    ActorSetState(4);
    sub_080aefd4((u32)gUnk_0874AF90);
    gCurTask->updateState = 3;
    switch ((u8)NightmarePowerOrbPickDashLane())
    {
    case 0:
        gCurTask->velX = 0xFFFD0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 192 << 10;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 8;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 6;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0;
        TaskYieldTrampoline(4);
        PlaySfx(144 << 2);
        gCurTask->velX = 0xFFFA8000;
        TaskYieldTrampoline(32);
        gCurTask->velX = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0;
        TaskYieldTrampoline(32);
        sub_080aec00();
        break;
    case 1:
        gCurTask->velX = 128 << 10;
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 9;
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 8;
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 6;
        gCurTask->velY = 128 << 6;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
        PlaySfx(144 << 2);
        gCurTask->velX = 0xFFFF8000;
        gCurTask->velY = 0xFFFC0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFF0000;
        gCurTask->velY = 0xFFFD0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFE0000;
        gCurTask->velY = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFD0000;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFC0000;
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFA0000;
        gCurTask->velY = 0xFFFFE000;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0xFFFD0000;
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFE0000;
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFF0000;
        gCurTask->velY = 128 << 11;
        TaskYieldTrampoline(8);
        sub_080aec00();
        break;
    case 2:
        gCurTask->velX = 128 << 10;
        gCurTask->velY = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 9;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 8;
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 128 << 6;
        gCurTask->velY = 0xFFFFE000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0;
        gCurTask->velY = 0;
        TaskYieldTrampoline(16);
        PlaySfx(144 << 2);
        gCurTask->velX = 0xFFFF8000;
        gCurTask->velY = 128 << 11;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFF0000;
        gCurTask->velY = 192 << 10;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFE0000;
        gCurTask->velY = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFD0000;
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFC0000;
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFA0000;
        gCurTask->velY = 128 << 6;
        TaskYieldTrampoline(12);
        gCurTask->velX = 0xFFFD0000;
        gCurTask->velY = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFE0000;
        gCurTask->velY = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFF0000;
        gCurTask->velY = 0xFFFC0000;
        TaskYieldTrampoline(8);
        sub_080aed5c();
        break;
    }
    TaskStop();
}

void sub_080aec00(void)
{
    sub_080aefd4((u32)gUnk_0874B0A0);
    gCurTask->updateState = 3;
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    gCurTask->velX = 128 << 11;
    gCurTask->velY = 0;
    TaskYieldTrampoline(6);
    gCurTask->velX = 128 << 10;
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 128 << 9;
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 128 << 8;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0xFFFE0000;
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0xFFFF0000;
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(6);
    gCurTask->velX = 0;
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(32);
}

void sub_080aed5c(void)
{
    struct Task **c;
    s32 z;
    s32 va;
    s32 vb;
    s32 vc;
    s32 vd;
    s32 ve;
    struct Task *t;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    struct Task *q6;
    struct Task *q7;
    struct Task *q8;
    struct Task *q9;
    struct Task *q10;
    struct Task *q11;
    struct Task *q12;
    struct Task *q13;
    struct Task *q14;
    struct Task *q15;
    struct Task *q16;
    struct Task *q17;
    struct Task *q18;
    struct Task *q19;

    sub_080aefd4((u32)gUnk_0874B0A0);
    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 3;
    q1 = *c;
    va = 128 << 10;
    q1->velX = va;
    vd = 0xFFFE0000;
    q1->velY = vd;
    TaskYieldTrampoline(8);
    q2 = *c;
    q2->velX = va;
    vc = 0xFFFF0000;
    q2->velY = vc;
    TaskYieldTrampoline(8);
    q3 = *c;
    q3->velX = va;
    vb = 128 << 9;
    q3->velY = vb;
    TaskYieldTrampoline(8);
    q4 = *c;
    q4->velX = va;
    q4->velY = va;
    TaskYieldTrampoline(8);
    q5 = *c;
    q5->velX = va;
    q5->velY = va;
    TaskYieldTrampoline(8);
    q6 = *c;
    q6->velX = va;
    q6->velY = vb;
    TaskYieldTrampoline(8);
    q7 = *c;
    q7->velX = va;
    q7->velY = vc;
    TaskYieldTrampoline(8);
    q8 = *c;
    q8->velX = va;
    q8->velY = vd;
    TaskYieldTrampoline(8);
    q9 = *c;
    q9->velX = va;
    q9->velY = vd;
    TaskYieldTrampoline(8);
    q10 = *c;
    q10->velX = va;
    q10->velY = vc;
    TaskYieldTrampoline(8);
    q11 = *c;
    q11->velX = 128 << 11;
    q11->velY = z;
    TaskYieldTrampoline(6);
    q12 = *c;
    q12->velX = va;
    ve = 128 << 8;
    q12->velY = ve;
    TaskYieldTrampoline(6);
    q13 = *c;
    q13->velX = vb;
    q13->velY = vb;
    TaskYieldTrampoline(6);
    q14 = *c;
    q14->velX = ve;
    q14->velY = va;
    TaskYieldTrampoline(6);
    q15 = *c;
    q15->velX = vc;
    q15->velY = va;
    TaskYieldTrampoline(6);
    q16 = *c;
    q16->velX = vd;
    q16->velY = vb;
    TaskYieldTrampoline(6);
    q17 = *c;
    q17->velX = vc;
    q17->velY = 0xFFFF8000;
    TaskYieldTrampoline(6);
    q18 = *c;
    q18->velX = vc;
    q18->velY = vc;
    TaskYieldTrampoline(6);
    q19 = *c;
    q19->velX = z;
    q19->velY = vc;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(32);
}

s32 NightmarePowerOrbPickDashLane(void)
{
    struct Task *tt;
    struct Task *tt2;
    u32 o;
    u32 o2;
    s32 i;
    s32 i2;

    i = TaskFindNearestPlayer();
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    if (tt->pixelY <= 63)
        return 1;
    i2 = TaskFindNearestPlayer();
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    if (tt2->pixelY > 128)
        return 2;
    return 0;
}

void sub_080aeef8(void)
{
    struct Task **c;
    struct Task *u;

    ActorSetState(5);
    sub_080aefd4((u32)gUnk_0874B0C4);
    c = &gCurTask;
    (*c)->updateState = 3;
    u = *c;
    u->velX = 128 << 9;
    u->accelY = 0xFFFFE000;
    TaskYieldTrampoline(60);
}

void sub_080aef30(void)
{
    gCurTask->updateCallback = (u32)sub_080aef50;
    TaskStop();
    TaskSleepForever();
}

void sub_080aef50(void)
{
    sub_080aef5c();
}

void sub_080aef5c(void)
{
    s32 *p;

    p = gUnk_02007D00;
    if (p[9] == 0)
    {
        p[8] += 204 << 6;
        gCameraAnchorY = p[8] >> 16;
    }
    if (p[5] > 23)
        p[5] = 0;
    BlendColors((u16 *)gUnk_082FE0E4, (u16 *)gUnk_082FE104, gUnk_0874AD44[p[5]], 16, (u16 *)(((gCurTask->tileWord >> 12) << 5) + (u32)gObjPalette));
    p[5]++;
}

void sub_080aefd4(u32 a)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->u8C.actor->animScript = (struct AnimCmd *)a;
    t->u8C.actor->animScriptPos = 0;
    u = gCurTask;
    u->unk28 = 0;
    sub_080af020();
}

void sub_080aeff8(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->u8C.actor->animScript != 0)
    {
        t->unk28--;
        if (t->unk28 <= 0)
            sub_080af020();
    }
}

void sub_080af020(void)
{
    struct Task *t;
    struct Actor *a;
    struct Actor *a2;
    struct Task *t3;
    struct Actor *a3;
    struct Task *t4;
    struct Actor *a4;
    struct Task *t5;
    struct Actor *a5;
    struct Task *t6;
    struct Actor *a6;
    struct Task *u;
    s32 i;
    s32 v;
    s32 r;

top:
    t = gCurTask;
    a = t->u8C.actor;
    i = a->animScriptPos;
    v = ((s32 *)a->animScript)[i];
    switch (v)
    {
    case -1:
        break;
    case -2:
        a->animScriptPos = 0;
        goto top;
    case -3:
        a->animScriptPos = i + 1;
        a2 = gCurTask->u8C.actor;
        ((void (*)(void))((s32 *)a2->animScript)[a2->animScriptPos++])();
        goto top;
    case -4:
        a->animScriptPos = i + 1;
        t3 = gCurTask;
        a3 = t3->u8C.actor;
        t3->frame = ((s32 *)a3->animScript)[a3->animScriptPos++];
        t4 = gCurTask;
        a4 = t4->u8C.actor;
        r = ((s32 (*)(void))((s32 *)a4->animScript)[a4->animScriptPos++])();
        u = gCurTask;
        u->unk28 = r;
        break;
    default:
        t5 = gCurTask;
        a5 = t5->u8C.actor;
        t5->frame = ((s32 *)a5->animScript)[a5->animScriptPos++];
        t6 = gCurTask;
        a6 = t6->u8C.actor;
        t6->unk28 = ((s32 *)a6->animScript)[a6->animScriptPos++];
        break;
    }
}

s32 sub_080af100(void)
{
    return ((s16)(u16)gCurTask->health >> 3) + 1;
}

void sub_080af114(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    sub_080aeff8();
    c = &gCurTask;
    t = *c;
    t->unk2C++;
    if (t->unk2C > 39)
    {
        sub_080aefd4((u32)gUnk_0874ADA8);
        u = *c;
        u->updateState = 1;
    }
}

void sub_080af144(void)
{
    gUnk_02007D00[1] = RandomRange(4) + 1;
    gCurTask->updateState = 2;
    if (gUnk_02007D00[3] == 0)
    {
        gUnk_02007D00[3] = 140;
        CreateNightmarePowerOrbStar(4);
    }
}

void sub_080af178(void)
{
    gUnk_02007D00[1]--;
}

void sub_080af188(void)
{
    struct Task *t;

    sub_080aeff8();
    if (gUnk_02007D00[1] <= 0)
    {
        sub_080aefd4((u32)gUnk_0874AD74);
        t = gCurTask;
        t->unk2C = 0;
        t->updateState = 0;
    }
}

void sub_080af1b8(void)
{
    sub_080aefd4((u32)gUnk_0874B07C);
}

void sub_080af1c8(void)
{
    sub_080aeff8();
}

void sub_080af1d4(void)
{
    s32 *p;
    s32 v;

    p = gUnk_02007D00;
    v = p[2];
    if (v != 0)
    {
        gCurTask->frame = gUnk_0874B1A8[v];
        p[2]++;
        if (p[2] > 24)
            p[2] = 0;
    }
}

void CreateNightmarePowerOrbStar(u8 a)
{
    struct ActorSpawn sp;

    if (gUnk_02007D00[0] == 0)
    {
        sp.subtype = 32;
        sp.taskType = 135;
        sp.variant = a;
        sp.spawnArg = gCurTask->unk74;
        sp.tileWord = 0xA110;
        sp.x = gCurTask->posX >> 16;
        sp.y = gCurTask->posY >> 16;
        sp.checkTerrain = 0;
        gCurTask->unk46 = CreateActorFromDesc(&sp, 1);
    }
}

void NightmarePowerOrbReactToDamage(void)
{
    gUnk_02007D00[2] = 1;
}

void NightmarePowerOrbReactToDefeat(void)
{
    StopBgm();
    TaskSetEntry(NightmarePowerOrbDefeat, gCurTaskIdx);
}

void NightmarePowerOrbDefeat(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    TaskStop();
    c = &gCurTask;
    t = *c;
    t->u8C.actor->animScript = 0;
    t->updateState = 3;
    gUnk_02007D00[0] = 1;
    ActorSetAttackBox(0);
    (*c)->u8C.actor->sfxOverride = 0x23E;
    HudRemoveHpBar();
    sub_0806b05c();
    sub_0806b098();
    u = *c;
    sub_08066f50(u->pixelX, u->pixelY);
    sub_080aeef8();
    (*c)->updateCallback = (u32)sub_080aef50;
    sub_080b09ac();
}

void sub_080af308(void)
{
}

void Task_NightmarePowerOrbStar(void)
{
    struct Task **c;
    s32 z;
    u8 *b42;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    b42 = &t->layer;
    z = 0;
    *b42 = 9;
    u = *c;
    u->frameTable = gUnk_08754504;
    u->facing = 1;
    v = *c;
    v->unk2C = z;
    CallTableEntry(v->variant, 12, gNightmarePowerOrbStarVariants);
}

void NightmarePowerOrbStarUpdate(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    if (gUnk_02007D00[0] != 0)
        ActorDestroy();
    c = &gCurTask;
    r = ActorTickAnim((*c)->unk28);
    u = *c;
    u->unk28 = r;
    ActorCheckHits();
    ActorReactToHit();
}

void NightmarePowerOrbStarVariant0(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 8;
    u1->velX = va;
    u1->velY = 0xFFFE0000;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->velX = va;
    u2->velY = 0xFFFF0000;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->velX = va;
    u3->velY = 0xFFFF8000;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->velX = va;
    u4->velY = 0xFFFFE000;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->velX = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(211, t2->posX >> 16, t2->posY >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = CreateChildTaskAt(212, t4->posX >> 16, t4->posY >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = CreateChildTaskAt(210, t6->posX >> 16, t6->posY >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void NightmarePowerOrbStarVariant1(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 6;
    u1->velX = va;
    u1->velY = 0xFFFF0000;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->velX = va;
    u2->velY = 0xFFFF8000;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->velX = va;
    u3->velY = 0xFFFFE000;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->velX = va;
    u4->velY = 0xFFFFF800;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->velX = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(211, t2->posX >> 16, t2->posY >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = CreateChildTaskAt(212, t4->posX >> 16, t4->posY >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = CreateChildTaskAt(210, t6->posX >> 16, t6->posY >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void NightmarePowerOrbStarVariant2(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 6;
    u1->velX = va;
    u1->velY = 128 << 9;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->velX = va;
    u2->velY = 128 << 8;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->velX = va;
    u3->velY = va;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->velX = va;
    u4->velY = 128 << 4;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->velX = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(211, t2->posX >> 16, t2->posY >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = CreateChildTaskAt(212, t4->posX >> 16, t4->posY >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = CreateChildTaskAt(210, t6->posX >> 16, t6->posY >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void NightmarePowerOrbStarVariant3(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 8;
    u1->velX = va;
    u1->velY = 128 << 10;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->velX = va;
    u2->velY = 128 << 9;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->velX = va;
    u3->velY = va;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->velX = va;
    u4->velY = 128 << 6;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->velX = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(211, t2->posX >> 16, t2->posY >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = CreateChildTaskAt(212, t4->posX >> 16, t4->posY >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = CreateChildTaskAt(210, t6->posX >> 16, t6->posY >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void sub_080af7d4(void)
{
    struct Task *t;
    struct Task *e;
    struct Task *e2;
    struct Task *t3;
    s32 i;
    s32 j;
    u32 o;
    u32 o2;
    s32 w;

    t = gCurTask;
    i = t->parent;
    o = i * 144;
    e = (struct Task *)((u8 *)gTasks + o);
    if ((s16)e->health < e->unk24 >> 1)
    {
        j = TaskFindNearestPlayer();
        o2 = j * 144;
        e2 = (struct Task *)((u8 *)gTasks + o2);
        w = e2->pixelY;
        t3 = gCurTask;
        if (w > t3->pixelY - gViewRect[2])
            t3->velY = 128 << 10;
        else
            t3->velY = 0xFFFE0000;
    }
}

void NightmarePowerOrbStarVariant4(void)
{
    struct Task *t;
    struct Task *u;

    switch (gUnk_02007D00[4])
    {
    case 0:
        PlaySfx(0x22A);
        CreateNightmarePowerOrbStar(5);
        TaskYieldTrampoline(16);
        t = gCurTask;
        t->posX = ((struct Task *)(t->parent * 144 + (u32)gTasks))->posX;
        t->posY = ((struct Task *)(t->parent * 144 + (u32)gTasks))->posY;
        PlaySfx(0x22A);
        CreateNightmarePowerOrbStar(5);
        TaskYieldTrampoline(16);
        u = gCurTask;
        u->posX = ((struct Task *)(u->parent * 144 + (u32)gTasks))->posX;
        u->posY = ((struct Task *)(u->parent * 144 + (u32)gTasks))->posY;
        PlaySfx(0x22A);
        CreateNightmarePowerOrbStar(5);
        gUnk_02007D00[4] = 1;
        break;
    case 1:
        PlaySfx(0x22B);
        CreateNightmarePowerOrbStar(6);
        CreateNightmarePowerOrbStar(7);
        CreateNightmarePowerOrbStar(8);
        gUnk_02007D00[4] = 2;
        break;
    case 2:
        PlaySfx(0x22B);
        CreateNightmarePowerOrbStar(9);
        CreateNightmarePowerOrbStar(10);
        gUnk_02007D00[4] = 0;
        break;
    }
    TaskExitTrampoline();
}

void NightmarePowerOrbStarVariant5(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFC0000;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(210, t2->posX >> 16, (s16)((t2->posY >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = CreateChildTaskAt(213, t3->posX >> 16, t3->posY >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = CreateChildTaskAt(210, t4->posX >> 16, (s16)((t4->posY >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void NightmarePowerOrbStarVariant6(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFDCCCD;
    u1->velY = 0xFFFE4CCD;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(210, (s16)((t2->posX >> 16) + 1), (s16)((t2->posY >> 16) - 1), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = CreateChildTaskAt(213, t3->posX >> 16, t3->posY >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = CreateChildTaskAt(210, (s16)((t4->posX >> 16) - 1), (s16)((t4->posY >> 16) + 1), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void NightmarePowerOrbStarVariant7(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFD0000;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(210, t2->posX >> 16, (s16)((t2->posY >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = CreateChildTaskAt(213, t3->posX >> 16, t3->posY >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = CreateChildTaskAt(210, t4->posX >> 16, (s16)((t4->posY >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void NightmarePowerOrbStarVariant8(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFDCCCD;
    u1->velY = 0x0001B333;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(210, (s16)((t2->posX >> 16) - 1), (s16)((t2->posY >> 16) - 1), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = CreateChildTaskAt(213, t3->posX >> 16, t3->posY >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = CreateChildTaskAt(210, (s16)((t4->posX >> 16) + 1), (s16)((t4->posY >> 16) + 1), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void NightmarePowerOrbStarVariant9(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFE0000;
    u1->velY = 0xFFFE0000;
    TaskYieldTrampoline(16);
    u2 = *c;
    u2->velX = 0xFFFD0000;
    u2->velY = 0;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(210, t2->posX >> 16, (s16)((t2->posY >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = CreateChildTaskAt(213, t3->posX >> 16, t3->posY >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = CreateChildTaskAt(210, t4->posX >> 16, (s16)((t4->posY >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void NightmarePowerOrbStarVariant10(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B488);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFE0000;
    u1->velY = 128 << 10;
    TaskYieldTrampoline(16);
    u2 = *c;
    u2->velX = 0xFFFD0000;
    u2->velY = 0;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = CreateChildTaskAt(210, t2->posX >> 16, (s16)((t2->posY >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = CreateChildTaskAt(213, t3->posX >> 16, t3->posY >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = CreateChildTaskAt(210, t4->posX >> 16, (s16)((t4->posY >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void NightmarePowerOrbStarVariant11(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    s16 w;
    s32 r;

    c = &gCurTask;
    (*c)->updateCallback = (u32)NightmarePowerOrbStarUpdate;
    ActorSetAttackBox((u32)gUnk_0874B4A4);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B234);
    u1 = *c;
    u1->unk28 = r;
    u1->velX = 0xFFFA0000;
top:
    t = *c;
    w = CreateChildTaskAt(213, t->posX >> 16, t->posY >> 16, 1);
    (*c)->unk46 = w;
    TaskYieldTrampoline(16);
    goto top;
}

void Task_NightmarePowerOrbStarTrail(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    struct Task *x;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    gCurTask->frameTable = gUnk_08754504;
    gCurTask->facing = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->parent].velX) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->parent].velY) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    u = gCurTask;
    u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX;
    /* a second task pointer: the unk58 store after the if goes through it */
    x = gCurTask;
    v = (gUnk_030023D4 << 17) >> 4;
    w = gTasks[x->parent].velY;
    /* the arms re-read the cell: `v + cell` keeps the ROM's operand order */
    if (w >= 0)
        w = gTasks[x->parent].velY - v;
    else
        w = v + gTasks[x->parent].velY;
    x->velY = w;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 19;
        TaskYieldTrampoline(2);
        gCurTask->frame = 20;
        TaskYieldTrampoline(2);
        gCurTask->frame = 21;
        TaskYieldTrampoline(2);
        gCurTask->frame = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void Task_NightmarePowerOrbStarTrailUp(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    s32 y;
    struct Task *x;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    gCurTask->frameTable = gUnk_08754504;
    gCurTask->facing = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->parent].velX) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->parent].velY) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    /* u for unk54, a second pointer x per arm for the single unk58 store */
    u = gCurTask;
    w = gTasks[u->parent].velY;
    if (w > 0)
    {
        u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX + 0xFFFEE000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->parent].velY;
        if (w >= 0)
            y = w - v + 0xFFFFC000;
        else
            y = v + w + 0xFFFFC000;
    }
    else if (w < 0)
    {
        u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX + (128 << 7);
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->parent].velY;
        if (w >= 0)
            y = w - v + 0xFFFF4000;
        else
            y = v + w + 0xFFFF4000;
    }
    else
    {
        u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX + 0xFFFF0000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->parent].velY;
        if (w >= 0)
            y = w - v + 0xFFFF8000;
        else
            y = v + w + 0xFFFF8000;
    }
    x->velY = y;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 19;
        TaskYieldTrampoline(2);
        gCurTask->frame = 20;
        TaskYieldTrampoline(2);
        gCurTask->frame = 21;
        TaskYieldTrampoline(2);
        gCurTask->frame = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void Task_NightmarePowerOrbStarTrailDown(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    s32 y;
    struct Task *x;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 12;
    gCurTask->frameTable = gUnk_08754504;
    gCurTask->facing = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->parent].velX) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->parent].velY) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    /* u for unk54, a second pointer x per arm for the single unk58 store */
    u = gCurTask;
    w = gTasks[u->parent].velY;
    if (w > 0)
    {
        u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX + (128 << 7);
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->parent].velY;
        if (w >= 0)
            y = w - v + (192 << 8);
        else
            y = v + w + (192 << 8);
    }
    else if (w < 0)
    {
        u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX + 0xFFFEE000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->parent].velY;
        if (w >= 0)
            y = w - v + (128 << 7);
        else
            y = v + w + (128 << 7);
    }
    else
    {
        u->velX = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->parent].velX + 0xFFFF0000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->parent].velY;
        if (w >= 0)
            y = w - v + (128 << 8);
        else
            y = v + w + (128 << 8);
    }
    x->velY = y;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame = 19;
        TaskYieldTrampoline(2);
        gCurTask->frame = 20;
        TaskYieldTrampoline(2);
        gCurTask->frame = 21;
        TaskYieldTrampoline(2);
        gCurTask->frame = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void Task_NightmarePowerOrbStarAfterimage(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    struct Task *x;

    t = gCurTask;
    t->moveCallback = (u32)TaskMoveRelativeToView;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 13;
    gCurTask->frameTable = gUnk_08754504;
    gCurTask->facing = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->parent].velX) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->parent].velY) << 4,
                        abs(gTasks[gCurTask->parent].velX) + abs(gTasks[gCurTask->parent].velY));
    u = gCurTask;
    u->velX = ((gUnk_030023B4 << 16) >> 4) + gTasks[u->parent].velX;
    /* a second task pointer: the unk58 store after the if goes through it */
    x = gCurTask;
    v = (gUnk_030023D4 << 16) >> 4;
    w = gTasks[x->parent].velY;
    /* the arms re-read the cell: `v + cell` keeps the ROM's operand order */
    if (w >= 0)
        w = gTasks[x->parent].velY - v;
    else
        w = v + gTasks[x->parent].velY;
    x->velY = w;
    switch (gTasks[gCurTask->parent].variant)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame = 17;
            TaskYieldTrampoline(2);
            gCurTask->frame = 16;
            TaskYieldTrampoline(2);
            gCurTask->frame = 15;
            TaskYieldTrampoline(2);
            gCurTask->frame = 14;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        if (gTasks[gCurTask->parent].frame == 3)
        {
            gCurTask->frame = 13;
            TaskYieldTrampoline(4);
            gCurTask->frame = 12;
            TaskYieldTrampoline(4);
            gCurTask->frame = 11;
            TaskYieldTrampoline(4);
            gCurTask->frame = 10;
            TaskYieldTrampoline(4);
        }
        else
        {
            gCurTask->frame = 11;
            TaskYieldTrampoline(4);
            gCurTask->frame = 10;
            TaskYieldTrampoline(4);
            gCurTask->frame = 13;
            TaskYieldTrampoline(4);
            gCurTask->frame = 12;
            TaskYieldTrampoline(4);
        }
        break;
    case 11:
        gCurTask->frame = 18;
        TaskYieldTrampoline(16);
        break;
    }
    TaskExitTrampoline();
}

void sub_080b0570(void)
{
    struct Task *t1;
    struct Task *t;
    struct Task *tb;
    s16 *a;
    struct Task *tt;
    struct Task *tt2;
    u32 o;
    u32 o2;
    s32 x;
    s32 y;

    t1 = gCurTask;
    if (t1->velX != 0 || t1->velY != 0 || t1->accelX != 0 || t1->accelY != 0)
        TaskIntegrateMotion();
    t = gCurTask;
    x = t->posX;
    tb = gTasks;
    a = &t->parent;
    o = *a * 144;
    tt = (struct Task *)((u8 *)tb + o);
    t->pixelX = (x >> 16) + (u16)tt->pixelX;
    y = t->posY;
    o2 = *a * 144;
    tt2 = (struct Task *)((u8 *)tb + o2);
    t->pixelY = (y >> 16) + (u16)tt2->pixelY;
}

void sub_080b05e8(void)
{
    struct Task **c;
    s32 *p;
    s32 z;
    s32 vA;
    s32 vB;
    s32 vC;
    s32 vD;
    s32 vE;
    s32 vF;
    s32 vG;
    struct Task *u;
    s32 r;
    s32 r2;
    s32 r3;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;
    struct Task *u8;
    struct Task *u9;
    struct Task *u10;
    struct Task *u11;
    struct Task *u12;
    struct Task *u13;
    struct Task *u14;
    struct Task *u15;
    struct Task *u16;
    struct Task *u17;
    struct Task *u18;
    struct Task *u19;
    struct Task *u20;
    struct Task *u21;

    PlayBgm(32);
    sub_08003184();
    SetBgmVolume(0);
    p = gUnk_02007D00;
    z = 0;
    p[9] = z;
    c = &gCurTask;
    (*c)->facing = 1;
    u = *c;
    u->frame = 0xFFFF;
    u->u8C.actor->animScript = (struct AnimCmd *)z;
    p[6] = z;
    p[7] = 3;
    sub_08063698(80, 32);
    (*c)->updateCallback = (u32)sub_080b07d8;
    TaskYieldTrampoline(65);
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B240);
    u2 = *c;
    u2->unk28 = r;
    u2->posX = 128 << 17;
    u2->posY = 0xFFF80000;
    TaskYieldTrampoline(28);
    u3 = *c;
    vA = 0xFFFF0000;
    u3->velX = vA;
    vB = 128 << 9;
    u3->velY = vB;
    TaskYieldTrampoline(72);
    u4 = *c;
    vC = 128 << 8;
    u4->velY = vC;
    TaskYieldTrampoline(10);
    u5 = *c;
    u5->velY = 128 << 6;
    TaskYieldTrampoline(10);
    u6 = *c;
    u6->velY = z;
    TaskYieldTrampoline(16);
    CreateChildTaskHere(220, 1);
    PlaySfx(0x241);
    r2 = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B254);
    u7 = *c;
    u7->unk28 = r2;
    vD = 128 << 11;
    u7->velX = vD;
    u7->velY = vA;
    TaskYieldTrampoline(8);
    u8 = *c;
    vE = 128 << 10;
    u8->velX = vE;
    TaskYieldTrampoline(8);
    u9 = *c;
    vF = 0xFFFE0000;
    u9->velY = vF;
    TaskYieldTrampoline(8);
    u10 = *c;
    u10->velX = vB;
    u10->velY = vA;
    TaskYieldTrampoline(6);
    u11 = *c;
    u11->velX = vC;
    u11->velY = 0xFFFF8000;
    TaskYieldTrampoline(4);
    u12 = *c;
    u12->velX = vF;
    u12->velY = vC;
    TaskYieldTrampoline(6);
    r3 = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_0874B2C8);
    u13 = *c;
    u13->unk28 = r3;
    u13->velX = 0xFFFC0000;
    vG = 192 << 8;
    u13->velY = vG;
    TaskYieldTrampoline(6);
    u14 = *c;
    u14->velY = 192 << 9;
    TaskYieldTrampoline(11);
    u15 = *c;
    u15->velX = vA;
    TaskYieldTrampoline(6);
    u16 = *c;
    u16->velX = vB;
    TaskYieldTrampoline(6);
    u17 = *c;
    u17->velX = vE;
    TaskYieldTrampoline(6);
    u18 = *c;
    u18->velX = vD;
    u18->velY = vG;
    TaskYieldTrampoline(6);
    u19 = *c;
    u19->velX = vE;
    u19->velY = 0xFFFFC000;
    TaskYieldTrampoline(6);
    u20 = *c;
    u20->velX = vC;
    u20->velY = 0xFFFF4000;
    TaskYieldTrampoline(6);
    u21 = *c;
    u21->velX = 128 << 7;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(18);
}

void sub_080b07d8(void)
{
    struct Task **c;
    struct Task *u;
    s32 *p;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnimFacingNearestPlayer((*c)->unk28);
    u = *c;
    u->unk28 = r;
    p = gUnk_02007D00;
    if (p[5] > 23)
        p[5] = 0;
    BlendColors((u16 *)gUnk_082FE0E4, (u16 *)gUnk_082FE104, gUnk_0874AD44[p[5]], 16, (u16 *)((((*c)->tileWord >> 12) << 5) + (u32)gObjPalette));
    p[5]++;
}

void Task_NightmarePowerOrbIntroScroll(void)
{
    struct Task **c;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;

    c = &gCurTask;
    u = *c;
    u->updateCallback = (u32)sub_080b08a0;
    u->unk28 = -4;
    TaskYieldTrampoline(143);
    u2 = *c;
    u2->unk28 = -3;
    TaskYieldTrampoline(32);
    u3 = *c;
    u3->unk28 = -2;
    TaskYieldTrampoline(32);
    u4 = *c;
    u4->unk28 = -1;
    TaskYieldTrampoline(32);
    u5 = *c;
    u5->unk28 = 0;
    TaskYieldTrampoline(16);
    TaskExitTrampoline();
}

void sub_080b08a0(void)
{
    s32 *p;

    *(u16 *)&gCameraAnchorY += gCurTask->unk28;
    p = gUnk_02007D00;
    if (p[9] < 0)
        SetBgmVolume(0);
    if (p[9] <= 255)
    {
        SetBgmVolume((u16)p[9]);
        p[9]++;
    }
}

void sub_080b08e4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)TaskDrawWorld;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gNightmarePowerOrbFrames;
    u->tileWord |= 0x800;
    u->posX = (gTasks[u->parent].pixelX - 16) << 16;
    u->posY = gTasks[u->parent].pixelY << 16;
    u->frame = 14;
    TaskYieldTrampoline(2);
    gCurTask->frame = 15;
    TaskYieldTrampoline(1);
    gCurTask->frame = 16;
    TaskYieldTrampoline(1);
    gCurTask->frame = 17;
    TaskYieldTrampoline(2);
    gCurTask->frame = 18;
    TaskYieldTrampoline(2);
    gCurTask->frame = 19;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080b09ac(void)
{
    TaskStop();
    while (gViewRect[2] > 8)
        TaskYieldTrampoline(1);
    gCurTask->updateCallback = 0;
    gCurTask->drawCallback = (u32)sub_080b0b50;
    sub_080b0b04();
    gCurTask->facing = 1;
    TaskSetFrame(0);
    gCurTask->unk18 = 128 << 13;
    gCurTask->unk28 = 0xFFFFF300;
    gCurTask->posX = 138 << 16;
    gCurTask->posY = 0xFFF00000;
    gCurTask->velX = 0xFFFF0000;
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(28);
    gCurTask->velX = 0xFFFF4000;
    TaskYieldTrampoline(28);
    gCurTask->velX = 0xFFFF8000;
    TaskYieldTrampoline(28);
    gCurTask->velX = 0xFFFFC000;
    TaskYieldTrampoline(14);
    gCurTask->velX = 0xFFFFF000;
    gCurTask->velY = 128 << 7;
    TaskYieldTrampoline(14);
    gCurTask->velX = 0xFFFFF800;
    TaskYieldTrampoline(14);
    gCurTask->velX = 128 << 5;
    TaskYieldTrampoline(14);
    gCurTask->velX = 128 << 6;
    TaskYieldTrampoline(14);
    gCurTask->velY = 128 << 6;
    TaskYieldTrampoline(62);
    gCurTask->frame = 1;
    gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
    gCurTask->unk18 = 0;
    gCurTask->unk28 = 0;
    TaskYieldTrampoline(48);
    gCurTask->velY = 128 << 5;
    TaskYieldTrampoline(84);
    gCurTask->velX = 128 << 4;
    gCurTask->velY = 128 << 3;
    TaskSleepForever();
}

void sub_080b0b04(void)
{
    u16 *q;
    struct Task *t;

    q = (u16 *)gUnk_082FFDF0;
    RequestCopy(4, ((u32 *)q)[3], OBJ_VRAM0, q[1] << 5);
    RequestCopy(2, ((u32 *)q)[2], (u32)gUnk_03001570, 32);
    t = gCurTask;
    t->tileWord = 0x8010;
    t->frameTable = gUnk_08754560;
}

void sub_080b0b50(void)
{
    struct Task *t;
    u32 *tbl;
    s8 sign;
    s32 r;

    if (gCurTask->unk18 > 0)
    {
        gCurTask->unk18 += gCurTask->unk28;
        if (gCurTask->unk18 < 0)
            gCurTask->unk18 = 0;
        if (gCurTask->unk18 > (252 << 14))
            gCurTask->unk18 = 252 << 14;
    }
    if (gCurTask->frameTable == 0)
        return;
    if (gCurTask->frame == -1)
        return;
    if (ActorIsInView() == 0)
        return;
    if (TaskIsOnScreen() != 0)
    {
        t = gCurTask;
        tbl = t->frameTable;
        if (t->unk18 > 0)
        {
            sign = (t->spriteFlags & 0x8000) ? -1 : 1;
            t->spriteFlags &= 0x7FFF;
            r = DrawAffineSprite(tbl[t->frame], (u16)gUnk_0873FF98[t->unk18 >> 16] * sign, gUnk_0873FF98[t->unk18 >> 16], 0);
            QueueSprite(gCurTask->layer, r, gCurTask->spriteFlags, gCurTask->tileWord,
                         gCurTask->pixelX - gSpriteCameraX,
                         (s16)(gCurTask->pixelY - gSpriteCameraY));
            if (sign < 0)
                gCurTask->spriteFlags |= 0x8000;
        }
        else
        {
            QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, t->tileWord, t->pixelX - gSpriteCameraX,
                         (s16)(t->pixelY - gSpriteCameraY));
        }
    }
}

void Task_PaintRollerPainting(void)
{
    struct Task *t;
    s32 d; /* an int: the ROM sign-extends the copied byte before the strb */

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gPaintRollerPaintingFrames;
    t->layer = 9;
    d = ((struct Task *)(gCurTask->parent * 144 + (u32)gTasks))->facing;
    gCurTask->facing = d;
    gCurTask->onGround = 0;
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(2);
        TaskYieldTrampoline(2);
        TaskSetFrame(-1);
        TaskYieldTrampoline(2);
        TaskSetFrame(3);
        TaskYieldTrampoline(2);
        TaskSetFrame(-1);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    PaintRollerPaintingPickSubject();
    CallTableEntry(gCurTask->state, 8, gPaintRollerPaintingStates);
}

void PaintRollerPaintingCar(void)
{
    gCurTask->frameTable = gPaintRollerPaintingCarFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874B96C);
    gCurTask->tileWord += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)sub_080b1564;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
    gCurTask->accelY = 168 << 5;
    gCurTask->speedLimitY = 192 << 10;
    while (1)
    {
        if (gCurTask->onGround != 0)
        {
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
        }
    }
}

void PaintRollerPaintingKirby(void)
{
    struct Task **c;
    struct Task *u;

    c = &gCurTask;
    (*c)->frameTable = gPaintRollerPaintingKirbyFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874B998);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    (*c)->updateCallback = (u32)sub_080b1564;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    u = *c;
    u->accelY = 168 << 5;
    u->speedLimitY = 192 << 10;
top:
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    goto top;
}

void PaintRollerPaintingWaddleDee(void)
{
    gCurTask->frameTable = gPaintRollerPaintingWaddleDeeFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874B9C4);
    gCurTask->tileWord += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)sub_080b1564;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->accelY = 168 << 5;
    gCurTask->speedLimitY = 192 << 10;
    while (1)
    {
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
    }
}

void PaintRollerPaintingMike(void)
{
    gCurTask->frameTable = gPaintRollerPaintingMikeFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874B9F0);
    gCurTask->tileWord += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)sub_080b1564;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->accelY = 168 << 5;
    gCurTask->speedLimitY = 192 << 10;
    while (1)
    {
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
    }
}

void PaintRollerPaintingBaseball(void)
{
    gCurTask->frameTable = gPaintRollerPaintingBaseballFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874BA1C);
    gCurTask->tileWord += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)PaintRollerPaintingBaseballUpdate;
    AngleToVector(TaskGetAngleToNearestPlayer(3), 128 << 2);
    gCurTask->velX = gUnk_030023B4;
    gCurTask->velY = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(10);
        TaskSetFrame(5);
        TaskYieldTrampoline(10);
        TaskSetFrame(6);
        TaskYieldTrampoline(10);
        TaskSetFrame(7);
        TaskYieldTrampoline(10);
    }
}

void PaintRollerPaintingBomb(void)
{
    gCurTask->frameTable = gPaintRollerPaintingBombFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874BA48);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)PaintRollerPaintingBombUpdate;
    gCurTask->unk28 = ActorStartAnim((struct AnimCmd *)gUnk_0874B560);
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(6);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(6);
    gCurTask->velY = 128 << 10;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    gCurTask->onGround = 0;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(6);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(6);
    gCurTask->velY = 128 << 10;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(10);
    gCurTask->unk28 = ActorStartAnim((struct AnimCmd *)gUnk_0874B568);
    TaskYieldTrampoline(44);
    gCurTask->unk28 = ActorStartAnim(0);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    PlaySfx(189);
    ActorSetAttackBox((u32)gUnk_0874BB98);
    PlayExplosionAnim();
    ActorDestroy();
}

void PaintRollerPaintingBombUpdate(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk28);
    u = *c;
    u->unk28 = r;
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void PaintRollerPaintingCloud(void)
{
    struct ActorSpawn sp;

    gCurTask->frameTable = gPaintRollerPaintingCloudFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874BA74);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->updateCallback = (u32)PaintRollerPaintingCloudUpdate;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(16);
    gCurTask->unk28 = ActorStartAnim((struct AnimCmd *)gUnk_0874B574);
    TaskYieldTrampoline(16);
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    while (1)
    {
        PlaySfx(0x222);
        sp.subtype = 15;
        sp.taskType = 117;
        sp.variant = gCurTask->variant;
        sp.spawnArg = gCurTask->unk74;
        sp.checkTerrain = 1;
        gCurTask->unk46 = CreateActorFromDescHere(&sp, 0);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velY = 0xFFFF8000;
            TaskYieldTrampoline(6);
            gCurTask->velY = 0xFFFF0000;
            TaskYieldTrampoline(6);
            gCurTask->velY = 0xFFFF8000;
            TaskYieldTrampoline(6);
            gCurTask->velY = 128 << 8;
            TaskYieldTrampoline(6);
            gCurTask->velY = 128 << 9;
            TaskYieldTrampoline(6);
            gCurTask->velY = 128 << 8;
            TaskYieldTrampoline(6);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
    }
}

void PaintRollerPaintingCloudUpdate(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk28);
    u = *c;
    u->unk28 = r;
    ActorCheckHits();
    ActorReactToHit();
}

void Task_PaintRollerLightning(void)
{
    struct Task *t;
    s32 d; /* an int: the ROM sign-extends the copied byte before the strb */

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    gCurTask->frameTable = gPaintRollerLightningFrames;
    d = ((struct Task *)(gCurTask->parent * 144 + (u32)gTasks))->facing;
    gCurTask->facing = d;
    gCurTask->onGround = 0;
    ActorLoadDef((struct ActorDef *)gPaintRollerLightningDef);
    gCurTask->updateCallback = (u32)sub_080b1564;
    gCurTask->velY = 128 << 11;
    while (gCurTask->onGround == 0)
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
    }
    gCurTask->updateCallback = 0;
    TaskStop();
    TaskSetFrame(2);
    TaskYieldTrampoline(4);
    TaskSetFrame(3);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    ActorDestroy();
}

void PaintRollerPaintingParasol(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    c = &gCurTask;
    (*c)->frameTable = gPaintRollerPaintingParasolFrames;
    ActorLoadDef((struct ActorDef *)gUnk_0874BAA0);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    (*c)->updateCallback = (u32)PaintRollerPaintingParasolUpdate;
    r = ActorStartAnim((struct AnimCmd *)gUnk_0874B590);
    u = *c;
    u->unk28 = r;
    u->unk2C = -32;
    TaskSleepForever();
}

void PaintRollerPaintingParasolUpdate(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;
    s32 w;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk28);
    u = *c;
    u->unk28 = r;
    w = u->unk2C + 1;
    u->unk2C = w;
    if (w > 128 << 1)
    {
        ActorSetHitReactions((u32)gUnk_0874BFD4);
        ActorReactToDefeat();
    }
    else if ((w & 15) == 0)
        TaskAccelerateTowardNearestPlayer(154 << 7, 0x18100);
    if (gCurTask->unk2C > 0)
    {
        ActorCheckHits();
        ActorReactToHit();
    }
}

void sub_080b1564(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void PaintRollerPaintingBaseballUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_080b1588(void)
{
    TaskStopY();
    return 0;
}

s32 sub_080b1594(void)
{
    struct Task *t;

    TaskStopY();
    t = gCurTask;
    t->accelY = 168 << 5;
    t->speedLimitY = 192 << 10;
    return 0;
}

s32 sub_080b15b4(void)
{
    TaskTurnAroundAndReverseX();
    return 0;
}

void PaintRollerPaintingPickSubject(void)
{
    s32 ofs;

    if (((struct Task *)(gCurTask->parent * 144 + (u32)gTasks))->health >= gUnk_02007D00[9] >> 1)
        ofs = 8;
    else
        ofs = 0;
    gUnk_030023D4 = 255 - gUnk_0874B5A4[ofs + (gUnk_02007D00[3] >> 16)];
    gUnk_030023D4 -= gUnk_0874B5A4[ofs + (gUnk_02007D00[3] & 0xFFFF)];
    gUnk_030023D4 = RandomRange(gUnk_030023D4);
    gUnk_030023B4 = 7;
    while (1)
    {
        if (gUnk_030023B4 < 0)
            while (1);
        if (gUnk_030023B4 != gUnk_02007D00[3] >> 16 && gUnk_030023B4 != (gUnk_02007D00[3] & 0xFFFF))
            gUnk_030023D4 -= gUnk_0874B5A4[ofs + gUnk_030023B4];
        if (gUnk_030023D4 <= 0)
            break;
        gUnk_030023B4--;
    }
    switch (gUnk_030023B4)
    {
    case 0:
        ActorSetState(0);
        break;
    case 1:
        ActorSetState(1);
        break;
    case 2:
        ActorSetState(2);
        break;
    case 3:
        ActorSetState(3);
        break;
    case 4:
        ActorSetState(4);
        break;
    case 5:
        ActorSetState(5);
        break;
    case 6:
        ActorSetState(6);
        break;
    case 7:
        ActorSetState(7);
        break;
    }
    gUnk_02007D00[3] = ((s16 *)gUnk_02007D00)[7] + (gUnk_030023B4 << 16);
}

void Task_HeavyMoleUpperArm(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->moveCallback = z;
    t->drawCallback = (u32)TaskDrawWorld;
    t->frameTable = gUnk_08753E8C;
    b42 = &t->layer;
    *b42 = 11;
    u = *c;
    u->lateUpdateCallback = (u32)HeavyMoleArmUpdate;
    u->facing = 255;
    u2 = *c;
    u2->posX = z;
    u2->posY = z;
    u2->unk28 = 0x10078;
    ActorSetState(0);
    u3 = *c;
    CallTableEntry(u3->state, 6, gHeavyMoleArmStates);
}

void Task_HeavyMoleLowerArm(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->moveCallback = z;
    t->drawCallback = (u32)HeavyMoleLowerArmDraw;
    t->frameTable = gUnk_08753E8C;
    b42 = &t->layer;
    *b42 = 8;
    u = *c;
    u->lateUpdateCallback = (u32)HeavyMoleArmUpdate;
    u->facing = 1;
    u2 = *c;
    u2->posX = z;
    u2->posY = z;
    u2->unk28 = 0x10078;
    ActorSetState(0);
    u3 = *c;
    CallTableEntry(u3->state, 6, gHeavyMoleArmStates);
}

void HeavyMoleUpperArmFollowBody(void)
{
    struct Task *t;
    struct Task *t4;
    struct Task *tt;
    struct Task *tt2;
    s16 *a;
    u32 o;
    u16 *e;
    u16 *e2;
    u32 o2;

    TaskIntegrateMotion();
    t = gCurTask;
    a = &t->parent;
    o = *a * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    e = (u16 *)((u8 *)tt + 72);
    t4 = t;
    t->pixelX = (t4->posX >> 16) + *e - 8;
    o2 = *a * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    e2 = (u16 *)((u8 *)tt2 + 74);
    t->pixelY = (t4->posY >> 16) + *e2 - 24;
}

void HeavyMoleLowerArmFollowBody(void)
{
    struct Task *t;
    struct Task *t4;
    struct Task *tt;
    struct Task *tt2;
    s16 *a;
    u32 o;
    u16 *e;
    u16 *e2;
    u32 o2;

    TaskIntegrateMotion();
    t = gCurTask;
    a = &t->parent;
    o = *a * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    e = (u16 *)((u8 *)tt + 72);
    t4 = t;
    t->pixelX = (t4->posX >> 16) + *e - 8;
    o2 = *a * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    e2 = (u16 *)((u8 *)tt2 + 74);
    t->pixelY = (t4->posY >> 16) + *e2 + 24;
}

void HeavyMoleLowerArmDraw(void)
{
    struct Task *t;
    u32 *tbl;
    u8 j;

    TaskDrawWorld();
    t = gCurTask;
    tbl = t->frameTable;
    j = gUnk_0874B63E[t->frame];
    if (gUnk_0874B614[j] != -1)
        QueueSprite(11, tbl[gUnk_0874B614[j]], t->spriteFlags, t->tileWord, t->pixelX - gSpriteCameraX,
                     (s16)(t->pixelY - gSpriteCameraY));
}

void HeavyMoleArmUpdate(void)
{
    struct Unk0200D120 *td;
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 i;

    td = gUnk_0200D120;
    c = &gCurTask;
    t = *c;
    i = t->parent - 32;
    if (td[i].hitState == 2)
    {
        if (t->facing == -1)
            HeavyMoleUpperArmFollowBody();
        else
            HeavyMoleLowerArmFollowBody();
        TaskSetEntry(sub_080b2294, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(t->updateState, 6, gHeavyMoleArmStateUpdates);
        u = *c;
        if (u->facing == -1)
            HeavyMoleUpperArmFollowBody();
        else
            HeavyMoleLowerArmFollowBody();
        HeavyMoleArmCheckHits();
    }
}

void HeavyMoleArmCheckHits(void)
{
    u8 *t63;
    u8 n;
    u32 *t4;
    u32 o;
    u8 j;

    t63 = (u8 *)gUnk_0874B63E;
    j = t63[gCurTask->frame];
    n = ((u8 *)gUnk_0874B6BC)[j];
    j <<= 1;
    if (n != 0)
    {
        t4 = gUnk_0874B6D4;
        do
        {
            o = j << 2;
            ActorSetAttackBox(*(u32 *)(o + (u32)t4));
            ActorCheckHits();
            TaskBreakBlocksNoPlayer(*(u32 *)(o + (u32)gUnk_0874B77C));
            n = n - 1;
            j = j + 1;
        } while (n != 0);
    }
}

void HeavyMoleArmEnterState(void)
{
    CallTableEntry(gCurTask->state, 6, gHeavyMoleArmStates);
}

void HeavyMoleArmState0(void)
{
    gCurTask->updateState = 0;
    if (gCurTask->unk28 >> 16 > 2)
        gCurTask->unk28 = (gCurTask->unk28 & 0xFFFF) + (128 << 10);
    gUnk_030023D4 = 0;
    if (gCurTask->facing == 1)
        gUnk_030023D4 = 1;
    gCurTask->unk2C = gUnk_0874B824[gUnk_030023D4 * 2 + gUnk_02007D00[3]];
    if (gCurTask->facing == -1)
    {
        while (1)
        {
            gCurTask->frame = 3;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->frame = 2;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->frame = 1;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->frame = 0;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
        }
    }
    else
    {
        while (1)
        {
            gCurTask->frame = 0;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->frame = 1;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->frame = 2;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->frame = 3;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
        }
    }
}

void sub_080b1b2c(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    v = t->unk28;
    if (*(u16 *)&t->unk28 != 0)
    {
        t->unk28 = v - 1;
        if ((t->unk28 & 15) == 0)
        {
            gUnk_030023D4 = ((struct Task *)(t->parent * 144 + (u32)gTasks))->velX;
            if (gUnk_030023D4 > 0)
            {
                if (t->unk28 >> 16 <= 1)
                    t->unk28 += 0x10000;
            }
            else if (gUnk_030023D4 < 0)
            {
                if (t->unk28 >> 16 != 0)
                    t->unk28 -= 0x10000;
            }
            else if (((struct Task *)(t->parent * 144 + (u32)gTasks))->velY != 0)
            {
                if (t->unk28 >> 16 <= 1)
                    t->unk28 += 0x10000;
            }
        }
    }
    else
    {
        if (v & 0xFFFF0000)
            t->unk28 = v - 0x10000;
        gCurTask->unk28 += 120;
    }
    gCurTask->unk2C--;
    if (gCurTask->unk2C == 0)
    {
        gCurTask->unk2C = gUnk_0874B82E[gUnk_02007D00[3]];
        ActorSetState(1);
        TaskSetEntry(HeavyMoleArmEnterState, gCurTaskIdx);
    }
}

void HeavyMoleArmState1(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q4b;
    s32 zz;
    u8 *tj;
    u8 *tj2;
    s32 k;
    s32 x;
    s32 x2;

    t = gCurTask;
    t->updateState = 1;
    u = gCurTask;
    k = u->facing;
    if (k == -1)
    {
        c = &gCurTask;
        zz = 0;
        tj = (u8 *)gUnk_0874B82A;
        for (;;)
        {
            u1 = *c;
            x = *(s16 *)((u8 *)u1 + 42);
            if (x == 3)
            {
                u1->frame = 4;
                TaskYieldTrampoline(1);
                u2 = *c;
                u2->frame = 2;
                TaskYieldTrampoline(1);
                u3 = *c;
                u3->frame = 5;
                TaskYieldTrampoline(1);
                u4 = *c;
                u4->frame = zz;
                TaskYieldTrampoline(1);
            }
            else
            {
                u1->frame = 3;
                TaskYieldTrampoline(*(u8 *)(x + (u32)tj));
                u2 = *c;
                u2->frame = 2;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)u2 + 42) + (u32)tj));
                u3 = *c;
                u3->frame = 1;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)u3 + 42) + (u32)tj));
                u4 = *c;
                u4->frame = zz;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)u4 + 42) + (u32)tj));
            }
        }
    }
    else
    {
        for (;;)
        {
            q1 = gCurTask;
            x2 = *(s16 *)((u8 *)q1 + 42);
            if (x2 == 3)
            {
                q1->frame = 4;
                TaskYieldTrampoline(1);
                q2 = gCurTask;
                q2->frame = 1;
                TaskYieldTrampoline(1);
                q3 = gCurTask;
                q3->frame = 5;
                TaskYieldTrampoline(1);
                q4b = gCurTask;
                q4b->frame = x2;
                TaskYieldTrampoline(1);
            }
            else
            {
                q1->frame = 0;
                TaskYieldTrampoline(*(u8 *)(x2 + (u32)gUnk_0874B82A));
                q2 = gCurTask;
                q2->frame = 1;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)q2 + 42) + (u32)gUnk_0874B82A));
                q3 = gCurTask;
                q3->frame = 2;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)q3 + 42) + (u32)gUnk_0874B82A));
                q4 = gCurTask;
                q4->frame = 3;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)q4 + 42) + (u32)gUnk_0874B82A));
            }
        }
    }
}

void sub_080b1d2c(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    u8 *tb;
    s32 v;
    s32 v2;
    u32 lo;

    c = &gCurTask;
    t = *c;
    t->unk2C--;
    if (t->unk2C == 0)
    {
        tb = (u8 *)gUnk_0874B831;
        ActorSetState(tb[RandomRange(4)]);
        TaskSetEntry(HeavyMoleArmEnterState, gCurTaskIdx);
    }
    u = *c;
    v = u->unk28;
    lo = *(u16 *)&u->unk28;
    if (lo != 0)
    {
        v2 = v - 1;
        u->unk28 = v2;
        if ((v2 & 15) == 0 && v2 >> 16 <= 2)
            u->unk28 = v + 0xFFFF;
    }
    else
        u->unk28 = v + 120;
}

void HeavyMoleArmState2(void)
{
    struct Task **c;
    struct Task **c2;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    u8 *t35;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 2;
    r = RandomRange(5);
    u = *c;
    u->unk34 = r;
    u->unk30 = z;
    t35 = (u8 *)gUnk_0874B835;
    TaskYieldTrampoline(t35[gUnk_02007D00[3]]);
    u2 = *c;
    u2->unk30 = 1;
    w = u2->unk34;
    if (w == 4)
        u2->unk30 = -u2->unk30;
    else if (w != 0 && RandomRange(2) != 0)
    {
        u3 = *c;
        u3->unk30 = -u3->unk30;
    }
    sub_080b2058(0);
    c2 = &gCurTask;
    u4 = *c2;
    u4->unk30 = -u4->unk30;
    sub_080b2058(1);
    u5 = *c2;
    u5->unk30 = 0;
    TaskYieldTrampoline(2);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080b1e20(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != 2)
    {
        TaskSetEntry(HeavyMoleArmEnterState, gCurTaskIdx);
        return;
    }
    if (t->unk30 == 0)
    {
        if (t->facing == -1)
            gUnk_030023D4 = t->unk34 * 6 + 36;
        else
            gUnk_030023D4 = t->unk34 * 6 + 96;
    }
    else
    {
        if (t->facing == -1)
            gUnk_030023D4 = t->unk34 * 6 + 6;
        else
            gUnk_030023D4 = t->unk34 * 6 + 66;
    }
    if (gFrameCount & 1)
        gCurTask->frame = gUnk_030023D4 + (((gFrameCount >> 1) & 1) + 4);
    else
        gCurTask->frame = gUnk_030023D4 + (gFrameCount & 3);
}

void HeavyMoleArmState3(void)
{
    struct Task **c;
    struct Task **c2;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    u8 *t35;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 3;
    r = RandomRange(5);
    u = *c;
    u->unk34 = r;
    u->unk30 = z;
    t35 = (u8 *)gUnk_0874B838;
    TaskYieldTrampoline(t35[gUnk_02007D00[3]]);
    u2 = *c;
    u2->unk30 = 2;
    w = u2->unk34;
    if (w == 4)
        u2->unk30 = -u2->unk30;
    else if (w != 0 && RandomRange(2) != 0)
    {
        u3 = *c;
        u3->unk30 = -u3->unk30;
    }
    sub_080b2058(0);
    c2 = &gCurTask;
    u4 = *c2;
    u4->unk30 = -u4->unk30;
    sub_080b2058(1);
    u5 = *c2;
    u5->unk30 = 0;
    TaskYieldTrampoline(1);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080b1f80(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->state != 3)
    {
        TaskSetEntry(HeavyMoleArmEnterState, gCurTaskIdx);
        return;
    }
    if (t->unk30 == 0)
    {
        if (t->facing == -1)
            gUnk_030023D4 = t->unk34 * 6 + 36;
        else
            gUnk_030023D4 = t->unk34 * 6 + 96;
    }
    else
    {
        if (t->facing == -1)
            gUnk_030023D4 = t->unk34 * 6 + 6;
        else
            gUnk_030023D4 = t->unk34 * 6 + 66;
    }
    if (gFrameCount & 1)
        gCurTask->frame = gUnk_030023D4 + (((gFrameCount >> 1) & 1) + 4);
    else
        gCurTask->frame = gUnk_030023D4 + (gFrameCount & 3);
}

void sub_080b2058(u8 a)
{
    struct Task **c;
    struct Task *u;
    struct Task *u2;
    u8 *tb;
    u8 *tb2;
    s32 a2;
    s32 w;
    s32 w2;
    s32 w0;
    s32 w3;
    s32 w4;

    w0 = gCurTask->unk30;
    c = &gCurTask;
    if (w0 > 0)
    {
        a2 = a << 1;
        goto xbody1;
xinc1:
        u->unk34 = u->unk34 + 1;
xbody1:
        tb = (u8 *)gUnk_0874B83B;
        w = (*c)->unk30;
        if (w < 0)
            w = -w;
        w3 = a2 + w - 1;
        TaskYieldTrampoline(tb[w3]);
        u = *c;
        if (u->unk34 <= 3)
            goto xinc1;
    }
    else
    {
        a2 = a << 1;
        goto xbody2;
xinc2:
        u2->unk34 = u2->unk34 - 1;
xbody2:
        tb2 = (u8 *)gUnk_0874B83B;
        w2 = (*c)->unk30;
        if (w2 < 0)
            w2 = -w2;
        w4 = a2 + w2 - 1;
        TaskYieldTrampoline(tb2[w4]);
        u2 = *c;
        if (u2->unk34 > 0)
            goto xinc2;
    }
}

void HeavyMoleArmState5(void)
{
    struct Task **c;
    u32 *t40;
    struct Task *t;
    struct Task *t2;
    struct Task *u;
    struct Task *u2;
    s32 z;
    s32 v;
    u32 o;
    s32 k;

    t = gCurTask;
    z = 0;
    t->updateState = 5;
    t2 = gCurTask;
    t2->unk2C = 11;
    t2->unk34 = z;
    c = &gCurTask;
    t40 = gUnk_0874B840;
loop:
    u = *c;
    v = u->unk2C - 1;
    u->unk2C = v;
    o = v * 4;
    u->velX = *(u32 *)(o + (u32)t40);
    k = u->facing;
    u->velY = *(u32 *)(o + (u32)gUnk_0874B86C) * k;
    TaskYieldTrampoline(*(u32 *)(o + (u32)gUnk_0874B898));
    if ((*c)->unk2C > 0)
        goto loop;
    TaskStop();
    u2 = gCurTask;
    u2->posX = 0;
    u2->posY = 0;
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080b214c(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    vu16 *pm;

    c = &gCurTask;
    if (gCurTask->state != 5)
        TaskSetEntry(HeavyMoleArmEnterState, gCurTaskIdx);
    pm = (vu16 *)&gFrameCount;
    if (*pm & 1)
    {
        t = *c;
        t->frame = ((*pm >> 1) & 1) + 4;
    }
    else
    {
        t2 = *c;
        t2->frame = *pm & 3;
    }
}

void HeavyMoleArmState4(void)
{
    struct Task *t;

    gCurTask->updateState = 4;
    t = gCurTask;
    t->unk2C = 24;
    t->unk30 = gUnk_0874B8C4[gUnk_02007D00[3]];
    do
    {
        gCurTask->unk2C--;
        if (gCurTask->facing == -1)
            gCurTask->unk34 = gUnk_0874B8C8[gCurTask->unk2C];
        else
            gCurTask->unk34 = gUnk_0874B8F8[gCurTask->unk2C];
        TaskYieldTrampoline(gUnk_0874B928[gCurTask->unk2C]);
    } while (gCurTask->unk2C > 0);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080b2228(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t3;
    vu16 *pm;

    c = &gCurTask;
    t = *c;
    t->unk30--;
    if (t->unk30 < 0)
        ActorSetState(0);
    t3 = *c;
    if (t3->state != 4)
    {
        TaskSetEntry(HeavyMoleArmEnterState, gCurTaskIdx);
        return;
    }
    pm = (vu16 *)&gFrameCount;
    if (*pm & 1)
        t3->frame = t3->unk34 + (((*pm >> 1) & 1) + 4);
    else
        t3->frame = t3->unk34 + (*pm & 3);
}

void sub_080b2294(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080b22b8;
    t->lateUpdateCallback = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080b22b8(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gTasks;
    t = gCurTask;
    if (tb[t->parent].frame == -1)
        t->drawCallback = 0;
    else
        t->drawCallback = (u32)TaskDrawWorld;
}

void Task_HeavyMoleYellowMissile(void)
{
    struct Task **c;
    struct Task **c4;
    struct Task **c5;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *p1;
    struct Task *p2;
    struct Task *p3;
    u8 *b42;
    s16 *a2;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gUnk_087540AC;
    b42 = &t->layer;
    z = 0;
    *b42 = 11;
    u = *c;
    u->updateCallback = (u32)HeavyMoleMissileUpdate;
    u->facing = 255;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    r = RandomRange(32);
    u2 = *c;
    w = r + 1;
    a2 = (s16 *)((u8 *)u2 + 110);
    *a2 = w;
    u2->unk6C = z;
    if (z < *a2)
    {
        c4 = c;
        do
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            q1 = *c4;
            q1->frame++;
            TaskYieldTrampoline(2);
            q2 = *c4;
            q2->frame++;
            TaskYieldTrampoline(2);
            q3 = *c4;
            q3->frame++;
            TaskYieldTrampoline(2);
            q4 = *c4;
            q4->unk6C++;
        } while ((s16)q4->unk6C < *(s16 *)((u8 *)q4 + 110));
    }
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    TaskSetFrame(13);
    TaskYieldTrampoline(2);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    c5 = &gCurTask;
    for (;;)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        p1 = *c5;
        p1->frame++;
        TaskYieldTrampoline(2);
        p2 = *c5;
        p2->frame++;
        TaskYieldTrampoline(2);
        p3 = *c5;
        p3->frame++;
        TaskYieldTrampoline(2);
    }
}

void Task_HeavyMoleRedMissile(void)
{
    struct Task **c;
    struct Task **c4;
    struct Task **c5;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *p1;
    struct Task *p2;
    struct Task *p3;
    u8 *b42;
    s16 *a2;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->frameTable = gUnk_087540EC;
    b42 = &t->layer;
    z = 0;
    *b42 = 11;
    u = *c;
    u->updateCallback = (u32)HeavyMoleMissileUpdate;
    u->facing = 255;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    r = RandomRange(32);
    u2 = *c;
    w = r + 1;
    a2 = (s16 *)((u8 *)u2 + 110);
    *a2 = w;
    u2->unk6C = z;
    if (z < *a2)
    {
        c4 = c;
        do
        {
            TaskSetFrame(12);
            TaskYieldTrampoline(2);
            q1 = *c4;
            q1->frame++;
            TaskYieldTrampoline(2);
            q2 = *c4;
            q2->frame++;
            TaskYieldTrampoline(2);
            q3 = *c4;
            q3->frame++;
            TaskYieldTrampoline(2);
            q4 = *c4;
            q4->unk6C++;
        } while ((s16)q4->unk6C < *(s16 *)((u8 *)q4 + 110));
    }
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(15);
    TaskYieldTrampoline(2);
    c5 = &gCurTask;
    for (;;)
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(2);
        p1 = *c5;
        p1->frame++;
        TaskYieldTrampoline(2);
        p2 = *c5;
        p2->frame++;
        TaskYieldTrampoline(2);
        p3 = *c5;
        p3->frame++;
        TaskYieldTrampoline(2);
    }
}

void HeavyMoleMissileUpdate(void)
{
    ActorCheckHits();
    ActorReactToHit();
    TaskBreakBlocksNoPlayer(gUnk_0874C108);
}

void sub_080b2550(void)
{
    ActorSetHitReactions((u32)gUnk_0874C210);
    TaskSetEntry(ActorDie, gCurTaskIdx);
}

void sub_080b2574(void)
{
    struct Task *t;

    RequestScreenShake(4);
    t = gCurTask;
    t->unk2C = 32;
    sub_0806619c(-1, (u32)sub_080b25e8, 0, 0, 0);
}

void sub_080b25a4(void)
{
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    TaskSetFrame(5);
    TaskYieldTrampoline(1);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
}

void sub_080b25e8(void)
{
    struct Task *t2;
    struct Task *u;
    s32 v;

    switch (gCurTask->unk2C)
    {
    case 32:
        TaskSetFrame(9);
        break;
    case 20:
        TaskSetFrame(7);
        break;
    case 16:
        TaskSetFrame(4);
        break;
    case 0:
        sub_0806621c();
        break;
    }
    t2 = gCurTask;
    v = t2->unk30;
    t2->unk30 = v - 1;
    switch (v)
    {
    case 100:
    case 150:
        CreateWhispyWoodsApple();
        break;
    case 50:
        CreateWhispyWoodsApple();
        break;
    }
    u = gCurTask;
    u->unk2C--;
}

void sub_080b2664(void)
{
    struct Task **c2;
    struct Task *ta;
    struct Task *tb;
    struct Task *tc;
    struct Task *td;
    struct Task *te;
    struct Task *tf;
    struct Task *tg;
    struct Task *th;
    s32 w;

    w = gCurTask->unk34;
    c2 = &gCurTask;
    switch (w)
    {
    case 0:
        if (RandomRange(4) == 0)
        {
            ta = gCurTask;
            ta->unk34 = 1;
            ActorSetState(2);
        }
        else
        {
            tb = gCurTask;
            tb->unk34 = 2;
            ActorSetState(3);
        }
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
        break;
    case 1:
        tc = *c2;
        tc->unk34 = 2;
        ActorSetState(3);
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
        break;
    case 2:
        if (RandomRange(4) == 0)
        {
            td = gCurTask;
            td->unk34 = 3;
            ActorSetState(3);
        }
        else
        {
            te = gCurTask;
            te->unk34 = 0;
            ActorSetState(1);
        }
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
        break;
    case 3:
        tf = *c2;
        tf->unk34 = 0;
        ActorSetState(1);
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
        break;
    case 4:
        if (RandomRange(2) == 0)
        {
            tg = gCurTask;
            tg->unk34 = 1;
            ActorSetState(2);
        }
        else
        {
            th = gCurTask;
            th->unk34 = 2;
            ActorSetState(3);
        }
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
        break;
    }
}

void CreateWhispyWoodsAirPuff(void)
{
    struct ActorSpawn sp;
    s32 z;

    PlaySfx(0x223);
    sp.subtype = 23;
    sp.taskType = 126;
    sp.variant = 0;
    sp.spawnArg = 0;
    z = 0;
    sp.x = 24;
    sp.y = 40;
    sp.tileWord = sub_08066630(8);
    sp.checkTerrain = z;
    CreateActorFromDescAtOffsetFacing(&sp, 1);
}

void CreateWhispyWoodsApple(void)
{
    struct ActorSpawn sp;
    s32 z;

    sp.subtype = 22;
    sp.taskType = 125;
    sp.variant = 0;
    sp.spawnArg = 0;
    sp.x = (u8)sub_080b2804();
    z = 0;
    sp.y = 224;
    sp.tileWord = sub_08066630(1);
    sp.checkTerrain = z;
    CreateChildTask(170, sp.x, sp.y, 0);
    CreateActorFromDesc(&sp, 1);
}

s32 sub_080b2804(void)
{
    u32 v;

    switch (RandomRange(6))
    {
    case 0:
        TaskGetNearestPlayerPos();
        v = gUnk_030023B4;
        if (v < 32)
            return 16;
        if (v - 32 <= 31)
            return 48;
        if (v - 64 <= 31)
            return 80;
        if (v - 96 <= 31)
            return 112;
        if (v - 128 <= 126)
            return 144;
        break;
    case 1:
        return 16;
    case 2:
        return 48;
    case 3:
        return 80;
    case 4:
        return 112;
    case 5:
        return 144;
    }
}

void sub_080b2884(void)
{
    TaskSetFrameFlip(10);
}

void sub_080b2890(void)
{
    struct Task **c;
    struct Task *t0;
    struct Task *t;
    u16 *a;
    struct Task *u;
    struct Task *u2;
    struct Task *t2;
    struct Task *t3;
    u16 *a74;
    struct Task *t5;
    u32 *tbv;
    s32 a42v;
    s32 a42b;
    s32 a42c;
    u32 *tbv2;
    u32 *tbv3;
    u16 *aw;
    u16 *aw2;
    s32 w9;
    s32 w9b;
    u16 *a74b;

    c = &gCurTask;
    t0 = *c;
    if (t0->frameTable == 0)
        return;
    if (t0->frame == -1)
        return;
    if ((u8)sub_08066a6c() && TaskIsOnScreen())
    {
        t = *c;
        a42v = t->layer;
        tbv = (u32 *)gWhispyWoodsFrames;
        t5 = t;
        QueueSprite(a42v, tbv[t5->frame], t5->spriteFlags, t5->tileWord,
                     t->pixelX - gSpriteCameraX,
                     (s16)((u16)t->pixelY - (u16)gSpriteCameraY));
    }
    u = gCurTask;
    a74 = (u16 *)((u8 *)u + 74);
    *a74 += 26;
    if ((u8)sub_08066a6c() && TaskIsOnScreen())
    {
        t2 = gCurTask;
        aw = (u16 *)(74 + (u32)t2);
        a = aw;
        w9 = *aw - 26;
        *a = w9;
        a42b = t2->layer;
        tbv2 = (u32 *)gUnk_08754358;
        QueueSprite(a42b, tbv2[t2->frame], t2->spriteFlags, t2->tileWord,
                     t2->pixelX - gSpriteCameraX,
                     (s16)((u16)*a - (u16)gSpriteCameraY));
    }
    u2 = gCurTask;
    a74b = (u16 *)((u8 *)u2 + 74);
    *a74b += 42;
    if ((u8)sub_08066a6c() && TaskIsOnScreen())
    {
        t3 = gCurTask;
        aw2 = (u16 *)(74 + (u32)t3);
        a = aw2;
        w9b = *aw2 - 42;
        *a = w9b;
        a42c = t3->layer;
        tbv3 = (u32 *)gUnk_087543A0;
        QueueSprite(a42c, tbv3[t3->frame], t3->spriteFlags, t3->tileWord,
                     t3->pixelX - gSpriteCameraX,
                     (s16)((u16)*a - (u16)gSpriteCameraY));
    }
}

void Task_WhispyWoods(void)
{
    struct Task **c;
    struct AnimCmd *k4;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;

    sub_08066088(0);
    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)sub_080b2890;
    t->layer = 11;
    u = *c;
    u->frameTable = gWhispyWoodsFrames;
    u->unk18 = 1;
    u->facing = 255;
    u2 = *c;
    u2->posX = 206 << 16;
    u2->posY = 128 << 17;
    k4 = (struct AnimCmd *)gUnk_0874C110;
    sub_080666f8(k4);
    sub_080664e0(k4);
    u3 = *c;
    CallTableEntry(u3->variant, 1, gWhispyWoodsVariants);
}

void WhispyWoodsInit(void)
{
    struct Task **c;
    struct Task *u;

    c = &gCurTask;
    (*c)->updateCallback = (u32)WhispyWoodsUpdate;
    ActorSetState(0);
    u = *c;
    CallTableEntry(u->state, 4, gWhispyWoodsStates);
}

void WhispyWoodsUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 4, gWhispyWoodsStateUpdates);
    ActorCheckHits();
    ActorReactToHit();
}

void WhispyWoodsEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gWhispyWoodsStates);
}

void WhispyWoodsWait(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *u;
    s32 z;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = z;
    t2 = *c;
    t2->unk28 = z;
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    sub_080b25a4();
    TaskSetFrame(17);
    TaskYieldTrampoline(63);
    sub_080b25a4();
    u = *c;
    u->unk28 = 1;
    TaskSleepForever();
}

void WhispyWoodsWaitUpdate(void)
{
    if (gCurTask->unk28 != 0)
        sub_080b2664();
}

void WhispyWoodsState1(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 1;
    t2 = gCurTask;
    t2->unk6C = z;
    c = &gCurTask;
    do
    {
        TaskSetFrame(14);
        TaskYieldTrampoline(1);
        q1 = *c;
        q1->frame++;
        TaskYieldTrampoline(1);
        q2 = *c;
        q2->frame++;
        TaskYieldTrampoline(8);
        CreateWhispyWoodsAirPuff();
        TaskYieldTrampoline(8);
        q3 = *c;
        q3->frame--;
        TaskYieldTrampoline(2);
        q4 = *c;
        q4->frame--;
        TaskYieldTrampoline(2);
        q5 = *c;
        q5->unk6C++;
    } while ((s16)q5->unk6C <= 1);
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->frame--;
    TaskYieldTrampoline(2);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080b2bf0(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
}

void WhispyWoodsState2(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 2;
    t2 = gCurTask;
    t2->unk6C = z;
    c = &gCurTask;
    do
    {
        TaskSetFrame(14);
        TaskYieldTrampoline(1);
        q1 = *c;
        q1->frame++;
        TaskYieldTrampoline(1);
        q2 = *c;
        q2->frame++;
        TaskYieldTrampoline(8);
        CreateWhispyWoodsAirPuff();
        TaskYieldTrampoline(8);
        q3 = *c;
        q3->frame--;
        TaskYieldTrampoline(2);
        q4 = *c;
        q4->frame--;
        TaskYieldTrampoline(2);
        q5 = *c;
        q5->unk6C++;
    } while ((s16)q5->unk6C <= 3);
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->frame--;
    TaskYieldTrampoline(2);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_080b2cc8(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
}

void WhispyWoodsDropApples(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    s32 z;

    t = gCurTask;
    z = 0;
    t->updateState = 3;
    t2 = gCurTask;
    t2->unk30 = 150;
    t2->unk6C = z;
    c = &gCurTask;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        q1 = *c;
        q1->frame++;
        TaskYieldTrampoline(4);
        q2 = *c;
        q2->frame++;
        TaskYieldTrampoline(4);
        q3 = *c;
        q3->frame--;
        TaskYieldTrampoline(4);
        q4 = *c;
        q4->frame--;
        TaskYieldTrampoline(4);
        TaskSetFrame(17);
        TaskYieldTrampoline(30);
        q5 = *c;
        q5->unk6C++;
    } while ((s16)q5->unk6C <= 2);
    ActorSetState(0);
    TaskSleepForever();
}

void WhispyWoodsDropApplesUpdate(void)
{
    struct Task *t2;
    s32 v;

    t2 = gCurTask;
    v = t2->unk30;
    t2->unk30 = v - 1;
    switch (v)
    {
    case 100:
    case 150:
        CreateWhispyWoodsApple();
        break;
    case 50:
        CreateWhispyWoodsApple();
        break;
    }
    if (gCurTask->state != 3)
        TaskSetEntry(WhispyWoodsEnterState, gCurTaskIdx);
}

void sub_080b2dd4(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    t->updateCallback = (u32)sub_080b2e20;
    t->moveCallback = (u32)ActorMove;
    t->layer = 11;
    u = *c;
    u->frameTable = gWhispyWoodsFrames;
    u->facing = 255;
    ActorSetState(0);
    u2 = *c;
    CallTableEntry(u2->state, 1, gUnk_0874C150);
}

void sub_080b2e20(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_0874C154);
}

void sub_080b2e3c(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_0874C150);
}

void sub_080b2e58(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *u1;
    struct Task *q1;
    struct Task *u2;
    struct Task *q2;

    c = &gCurTask;
    t = *c;
    t->updateState = 0;
    t2 = *c;
    CreateStarRodPiece(1, (s16)((u16)t2->pixelX - 32), (s16)((u16)t2->pixelY + 16));
    TaskSetFrame(10);
    TaskYieldTrampoline(4);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
top:
    u1 = *c;
    u1->unk6C = 0;
    do
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(4);
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        q1 = *c;
        q1->unk6C++;
    } while ((s16)q1->unk6C <= 7);
    TaskSetFrame(12);
    TaskYieldTrampoline(30);
    u2 = *c;
    u2->unk6C = 0;
    do
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(50);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
        q2 = *c;
        q2->unk6C++;
    } while ((s16)q2->unk6C <= 3);
    TaskSetFrame(12);
    TaskYieldTrampoline(180);
    TaskSetFrame(13);
    TaskYieldTrampoline(60);
    TaskSetFrame(12);
    TaskYieldTrampoline(60);
    goto top;
}

void sub_080b2f34(void)
{
}

s32 sub_080b2f38(void)
{
    struct Task *t;
    s32 z;

    t = gCurTask;
    z = t->state;
    if (z == 0)
    {
        t->unk28 = z;
        t->unk2C = t->pixelY;
        ActorSetState(1);
        TaskSetEntry(WhispyWoodsAppleEnterState, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_080b2f78(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->onGround = 0;
    TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
    u = *c;
    u->velY = 0xFFFFC000;
    u->accelY = 224 << 3;
}

void sub_080b2fb0(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->onGround = 0;
    TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
    u = *c;
    u->velY = 0xFFFFC000;
    u->accelY = 224 << 3;
}
