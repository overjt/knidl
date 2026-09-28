#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"

/* RAM cells / ROM tables */
/* Not from collision.h or room.h: this file's view of gMaxHealth and
   gTerrainResult differs (lesson 3.517). */
extern u32 gMaxHealth[];
extern s16 gPlayerHealth[];
extern s8 gUnk_02005590[];
extern struct Unk020055D8 gRoomObjectList;
extern u8 gUnk_02005E10[];
extern u8 gRoomEntryMode;
extern u32 gUsedRoomObjects[8][8];
extern s16 gPlayerLives[];
extern u16 gUnk_02007D60;
extern s8 gUnk_02007D64;
extern s16 gUnk_0200AF0C;
extern u8 gWarpStarStationLevels;
extern u8 gUnk_0200B078;
extern u8 gUnk_0200D080;
extern s16 gCameraAnchorY;
extern s32 gUnk_03001F2C;
extern u8 gMetaKnightmareMode;
extern s16 gViewRect[];
extern u32 gUnk_03002160;
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern u8 gActivePlayerCount;
extern s8 gLevelIndex;
extern s16 gCameraAnchorX;
extern u32 gBigSwitchFlags[];
extern u16 gGameState;
extern s32 gCurSaveSlot;
extern s8 gStageIndex;
extern s32 gUnk_03002448;
extern u32 gExtraMode[];
extern s8 gRoomIndex;
extern u8 gTerrainResult[];
extern u32 gTerrainBoundsClamp[];
extern s16 gRoomBounds[];
extern struct Unk03005680 gScrollLock;

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
extern void TaskInitWaterFlags(void);
extern u8 ClampTaskToRoom(struct Task *t);
extern u32 sub_0802294c();
extern void ExitClearedStage();
extern void sub_08025a30();
extern void sub_08025acc();
extern void sub_08025b5c();
extern void RequestScreenShake(u32 a);
extern void sub_080275cc();
extern void StartScrollLock();
extern s32 CreateMapEvent();
extern void CreateWarpStarStationNumber();
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
extern s32 TaskIsInRectSlot(struct PointPair *box, s32 i);
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

void sub_080b2fe8(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk6C = 6;
    t->unk30 = t->pixelX;
    t->unk34 = t->pixelY;
}

void sub_080b3010(u8 a)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    u8 *tb;

    c = &gCurTask;
    t = *c;
    if (t->unk30 > 7)
        t->unk30 = 0;
    tb = (u8 *)gUnk_0874C24C;
    TaskSetFrame(tb[(*c)->unk30]);
    u = *c;
    u->unk30++;
    TaskYieldTrampoline(a);
}

void Task_WhispyWoodsApple(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 9;
    u = *c;
    u->frameTable = gWhispyWoodsAppleFrames;
    u->unk28 = 1;
    CallTableEntry(u->variant, 1, gUnk_0874C21C);
}

void sub_080b3090(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->updateCallback = (u32)sub_080b30c8;
    t->facing = t->unk74;
    ActorSetState(0);
    u = *c;
    CallTableEntry(u->state, 4, gUnk_0874C220);
}

void sub_080b30c8(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    if (t->unk28 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
        {
            u = *c;
            CallTableEntry(u->updateState, 4, gUnk_0874C230);
        }
    }
    else
        CallTableEntry(t->updateState, 4, gUnk_0874C230);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080b3110(void)
{
    CallTableEntry(gCurTask->state, 4, gUnk_0874C220);
}

void sub_080b312c(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *u3;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = z;
    TaskFaceNearestPlayer();
    u = *c;
    u->onGround = z;
    u2 = *c;
    u2->unk6C = z;
    do
    {
        q1 = *c;
        q1->frame = 4;
        TaskYieldTrampoline(4);
        q2 = *c;
        q2->frame = 0xFFFF;
        TaskYieldTrampoline(4);
        q3 = *c;
        q3->unk6C++;
    } while ((s16)q3->unk6C <= 5);
    u3 = gCurTask;
    u3->accelY = 148 << 6;
    u3->speedLimitY = 128 << 11;
    for (;;)
    {
        sub_080b3010(15);
        sub_080b3010(8);
        sub_080b3010(4);
    }
}

void sub_080b319c(void)
{
}

void sub_080b31a0(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 1;
    TaskFaceNearestPlayer();
    u = *c;
    u->onGround = z;
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    u2 = *c;
    u2->velY = 0xFFFE0000;
    u2->accelY = 168 << 5;
    for (;;)
        sub_080b3010(4);
}

void sub_080b31e0(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->pixelY > t->unk2C)
    {
        ActorSetState(2);
        TaskSetEntry(sub_080b3110, gCurTaskIdx);
    }
}

void sub_080b3214(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 2;
    TaskSetFrameNoFlip(4);
    u = *c;
    u->onGround = z;
    TaskSetMotionXFacing(160 << 9, 0x5A5A5A5A);
    u2 = *c;
    u2->velY = 0xFFFF0000;
    u2->accelY = 192 << 4;
    for (;;)
        sub_080b3010(8);
}

void sub_080b3258(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->pixelY > t->unk2C)
    {
        ActorSetState(3);
        TaskSetEntry(sub_080b3110, gCurTaskIdx);
    }
}

void sub_080b328c(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->updateState = 3;
    TaskSetFrameNoFlip(4);
    u = *c;
    u->onGround = z;
    TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
    u2 = *c;
    u2->velY = 0xFFFFC000;
    u2->accelY = 224 << 3;
    for (;;)
        sub_080b3010(8);
}

void sub_080b32d0(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    if (t->pixelY > t->unk2C)
    {
        t->onGround = 0;
        TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
        u = *c;
        u->velY = 0xFFFFC000;
        u->accelY = 224 << 3;
    }
}

void Task_WhispyWoodsAirPuff(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    b42 = &t->layer;
    z = 0;
    *b42 = 9;
    u = *c;
    u->frameTable = gUnk_0874C568;
    u->tileWord = z;
    u->facing = 255;
    u2 = *c;
    CallTableEntry(u2->variant, 1, gUnk_0874C254);
}

void sub_080b3368(void)
{
    struct Task **c;
    struct Task *u;

    c = &gCurTask;
    (*c)->updateCallback = (u32)sub_080b3398;
    ActorSetState(0);
    u = *c;
    CallTableEntry(u->state, 1, gUnk_0874C258);
}

void sub_080b3398(void)
{
    CallTableEntry(gCurTask->updateState, 1, gUnk_0874C25C);
    ActorCheckHits();
    ActorReactToHit();
}

void sub_080b33bc(void)
{
    CallTableEntry(gCurTask->state, 1, gUnk_0874C258);
}

void sub_080b33d8(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 v5;
    s32 v6;
    s32 z;
    struct Task *u0;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;
    struct Task *u8;
    struct Task *u9;

    t = gCurTask;
    z = 0;
    t->updateState = z;
    u = gCurTask;
    u->onGround = z;
    c = &gCurTask;
    v5 = 128 << 9;
    v6 = 0xFFFD0000;
top:
    u0 = *c;
    u0->velX = 0xFFFC0000;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    sub_080b2fe8();
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    sub_080b2fe8();
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u1 = *c;
    u1->velY = v5;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    u2 = *c;
    u2->velX = v6;
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u3 = *c;
    u3->velY = 128 << 8;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    u4 = *c;
    u4->velX = v6;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    u5 = *c;
    u5->velY = v5;
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u6 = *c;
    u6->velX = 0xFFFE0000;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    u7 = *c;
    u7->velY = 0;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    u8 = *c;
    u8->velX = 0xFFFF0000;
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    u9 = *c;
    u9->velY = v5;
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    TaskSetFrameFlip(2);
    TaskYieldTrampoline(1);
    TaskSetFrameFlip(0);
    TaskYieldTrampoline(2);
    TaskSetFrameFlip(4);
    TaskYieldTrampoline(1);
    goto top;
}

void sub_080b3758(void)
{
    struct Task *t;
    struct Task *t6;
    s16 *a;
    u32 h;
    s32 w;
    u8 *t46;
    u32 *t68;
    u8 *t40;
    s32 d;
    u8 dv;
    s32 a0v;

    t = gCurTask;
    a = (s16 *)((u8 *)t + 108);
    h = *(u16 *)a;
    if (*a != 0)
    {
        *a = h - 1;
        t46 = (u8 *)gUnk_0874C246;
        dv = t46[*a];
        d = (s8)dv;
        t6 = t;
        w = t6->unk30 - d;
        t6->unk30 = w;
        a0v = t->layer + 1;
        t68 = (u32 *)gUnk_0874C568;
        t40 = (u8 *)gUnk_0874C240;
        QueueSprite(a0v, t68[t40[*a]], t->spriteFlags, t->tileWord,
                     w - gSpriteCameraX,
                     (s16)(t->unk34 - (u16)gSpriteCameraY));
    }
}

void sub_080b37ec(void)
{
    gCurTask->health = gCurTask->u80.nearestPlayer;
    gCurTask->u80.nearestPlayer = gCurTask->unk46;
    gCurTask->unk46 = gCurTask->unk70;
    gCurTask->unk70 = (u32)gCurTask->unk24 >> 16;
    gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF) + (((u32)gCurTask->unk20 >> 16) << 16);
    gCurTask->unk20 = (gCurTask->unk20 & 0xFFFF) + (((u32)gCurTask->unk34 >> 16) << 16);
    gCurTask->unk34 = (gCurTask->unk34 & 0xFFFF) + (((u32)gCurTask->unk30 >> 16) << 16);
    gCurTask->unk30 = (gCurTask->unk30 & 0xFFFF) + (((u32)gCurTask->unk2C >> 16) << 16);
    /* the redundant mask is what loads 0xFFFF0000 here for the ANDs below
       (combine drops the AND before the shift, the constant stays) */
    gCurTask->unk2C = (gCurTask->unk2C & 0xFFFF) + (((u32)(gCurTask->unk28 & 0xFFFF0000) >> 16) << 16);
    gCurTask->unk28 = (gCurTask->unk28 & 0xFFFF) + (gCurTask->pixelX << 16);
    gCurTask->unk84 = gCurTask->hitterPlayer;
    gCurTask->hitterPlayer = gCurTask->parent;
    gCurTask->parent = gCurTask->facing;
    gCurTask->facing = gCurTask->unk24;
    gCurTask->unk24 = (gCurTask->unk24 & 0xFFFF0000) + (gCurTask->unk20 & 0xFFFF);
    gCurTask->unk20 = (gCurTask->unk20 & 0xFFFF0000) + (gCurTask->unk34 & 0xFFFF);
    gCurTask->unk34 = (gCurTask->unk34 & 0xFFFF0000) + (gCurTask->unk30 & 0xFFFF);
    gCurTask->unk30 = (gCurTask->unk30 & 0xFFFF0000) + (gCurTask->unk2C & 0xFFFF);
    gCurTask->unk2C = (gCurTask->unk2C & 0xFFFF0000) + (gCurTask->unk28 & 0xFFFF);
    gCurTask->unk28 = (gCurTask->unk28 & 0xFFFF0000) + gCurTask->pixelY;
}

void sub_080b38f0(void)
{
    register struct Task **c asm("r4");
    struct Task *t;
    struct Task *q;
    struct Task *t2;
    struct Task *t3;
    register u16 *a70 asm("r3");
    register s16 *a48 asm("r2");
    s32 w24c;
    s32 wv;
    s16 *a4A;
    u8 *aw;
    register u8 *ab asm("r1");
    register u8 *ac asm("r0");
    register s32 k6 asm("r2");
    s32 w;
    s32 w2;
    register s32 w3 asm("r0");
    register s32 w4 asm("r0");
    s32 w24b;
    register s32 w5 asm("r2");
    register s32 w6 asm("r2");

    c = &gCurTask;
    t = *c;
    a70 = (u16 *)((u8 *)t + 112);
    q = t;
    w24c = *(u16 *)&q->unk24;
    a48 = (s16 *)((u8 *)q + 72);
    t->unk24 = w24c + (*a48 << 16);
    t->unk20 = *(u16 *)&t->unk20 + (*a48 << 16);
    t->unk34 = *(u16 *)&t->unk34 + (*a48 << 16);
    t->unk30 = *(u16 *)&t->unk30 + (*a48 << 16);
    t->unk2C = *(u16 *)&t->unk2C + (*a48 << 16);
    t->unk28 = *(u16 *)&t->unk28 + (*a48 << 16);
    w = *a48;
    t->unk18 = w;
    *a70 = w;
    aw = (u8 *)t + 70;
    *(u16 *)aw = w;
    aw += 58;
    *aw = w;
    w2 = (s8)(u8)w;
    aw -= 8;
    *(s16 *)aw = w2;
    t2 = *c;
    wv = *(u16 *)((u8 *)t2 + 74);
    *(u8 *)((u8 *)t2 + 67) = wv;
    t3 = *c;
    w24b = t3->unk24;
    k6 = 0xFFFF0000;
    w24b &= k6;
    a4A = (s16 *)((u8 *)t3 + 74);
    t3->unk24 = w24b + *a4A;
    asm("" ::: "memory");
    t3->unk20 = (t3->unk20 & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk34 = (t3->unk34 & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk30 = (t3->unk30 & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk2C = (t3->unk2C & k6) + *a4A;
    asm("" ::: "memory");
    t3->unk28 = (t3->unk28 & k6) + *a4A;
    asm("" ::: "memory");
    w3 = *a4A;
    t3->unk1C = w3;
    ab = (u8 *)t3 + 67;
    *ab = w3;
    w4 = (s8)(u8)w3;
    w5 = w4;
    *(s16 *)(ab + 1) = w4;
    ac = (u8 *)t3 + 127;
    *ac = w5;
    w6 = (s8)(u8)w5;
    ac += 5;
    *(s16 *)ac = w6;
}

void sub_080b3a00(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    s32 r;
    s32 r2;
    s32 r3;
    s32 r4;
    s32 r5;
    s32 r6;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;

    r = RandomRange(30);
    c = &gCurTask;
    u1 = *c;
    u1->hitTimer = r - 10;
    r2 = RandomRange(30);
    u2 = *c;
    u2->onGround = r2 - 10;
    r3 = RandomRange(30);
    u3 = *c;
    w3 = r3 - 10;
    *(u8 *)((u8 *)u3 + 123) = w3;
    r4 = RandomRange(30);
    u4 = *c;
    w4 = r4 - 10;
    *(u8 *)((u8 *)u4 + 124) = w4;
    r5 = RandomRange(30);
    u5 = *c;
    w5 = r5 - 10;
    *(u8 *)((u8 *)u5 + 125) = w5;
    r6 = RandomRange(30);
    u6 = *c;
    w6 = r6 - 10;
    *(u16 *)((u8 *)u6 + 130) = w6;
}

void WhispyWoodsLeavesDraw(void)
{
    if (gUnk_0874C260[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C260[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->pixelX - gSpriteCameraX + gCurTask->hitTimer,
                     gCurTask->pixelY - gSpriteCameraY + gCurTask->hitKind);
    if (gUnk_0874C2A6[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C2A6[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     ((u32)gCurTask->unk20 >> 16) - gSpriteCameraX + gCurTask->onGround,
                     gCurTask->unk20 - gSpriteCameraY + (s8)gCurTask->hitDirection);
    if (gUnk_0874C2EC[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C2EC[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->health - gSpriteCameraX + gCurTask->waterFlags,
                     gCurTask->unk84 - gSpriteCameraY + (s8)gCurTask->hitEffect);
    if (gUnk_0874C332[65 - (s16)gCurTask->unk6C] != -1)
        QueueSprite(gCurTask->layer,
                     gUnk_0874CE68[gUnk_0874C332[65 - (s16)gCurTask->unk6C]],
                     gCurTask->spriteFlags, gCurTask->tileWord,
                     gCurTask->unk18 - gSpriteCameraX,
                     gCurTask->unk1C - gSpriteCameraY);
}

void Task_WhispyWoodsLeaves(void)
{
    s32 m;
    s32 z;

    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->tileWord = 0;
    gCurTask->drawCallback = (u32)WhispyWoodsLeavesDraw;
    gCurTask->layer = 8;
    gCurTask->frameTable = (u32 *)gUnk_0874CE68;
    gCurTask->updateCallback = (u32)WhispyWoodsLeavesUpdate;
    sub_080b38f0();
    sub_080b3a00();
    gCurTask->unk6C = 66;
    gCurTask->unk6E = 0;
    /* the two constants the loop keeps in r5/r8 are variables (lesson 3.471) */
    m = 0x8000;
    z = 0;
    do
    {
        gCurTask->velY = z;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x18000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x10000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = m;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0x4000;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(2);
        gCurTask->velX = z;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x18000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x10000;
        gCurTask->velY = m;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x8000;
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x4000;
        gCurTask->velY = 0x2000;
        TaskYieldTrampoline(2);
    } while (++gCurTask->unk6E <= 2);
    gCurTask->velX = 0;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x18000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x10000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x8000;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0x4000;
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(1);
    gCurTask->velX = 0;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x18000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x10000;
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x8000;
    gCurTask->velY = 0x4000;
    TaskYieldTrampoline(1);
    gCurTask->velX = -0x4000;
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(1);
    TaskSleepForever();
}

void WhispyWoodsLeavesUpdate(void)
{
    struct Task **c;
    u8 *su;
    s32 w;

    sub_080b37ec();
    c = &gCurTask;
    su = (u8 *)*c + 108;
    w = *(u16 *)su - 1;
    *(u16 *)su = w;
    if ((u16)w == 0)
        ActorDestroy();
}

void Task_OneUp(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gOneUpFrames;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gPickupVariants);
}

void Task_MaximTomato(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gMaximTomatoFrames;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gPickupVariants);
}

void Task_InvincibleCandy(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gInvincibleCandyFrames;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gPickupVariants);
}

void Task_EnergyDrink(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gEnergyDrinkFrames;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 2, gPickupVariants);
}

void sub_080b3f54(void)
{
    struct Actor *a;

    a = gCurTask->u8C.actor;
    if (a->animScript == NULL)
        gCurTask->unk34 = ActorStartAnim((struct AnimCmd *)gUnk_08756084);
    gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
    if (gMetaKnightmareMode == 1 && gCurTask->unk76 == 1)
        gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xE000;
    a->extraLayerOffset = 1;
    a->extraTileWord = 0xF000;
    a->extraOffsetY = 0;
    a->unk16 = 0;
}

void sub_080b3fcc(void)
{
    struct Task **c;
    struct Task *t;
    struct Actor *a;
    s32 u;

    c = &gCurTask;
    t = *c;
    a = *(struct Actor **)((u8 *)t + 140);
    if (t->frame == -1)
    {
        *(u16 *)((u8 *)a + 26) = 5;
        t->frame = 4;
    }
    else
    {
        u = 0xFFFF;
        *(u16 *)((u8 *)a + 26) = u;
    }
}

void sub_080b3ffc(void)
{
    if (gCurTask->frame != 5)
    {
        if (gMetaKnightmareMode == 1 && gCurTask->unk76 == 1)
            gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xE000;
    }
    else
        gCurTask->tileWord = (gCurTask->tileWord & 0xFFF) | 0xF000;
}

s32 sub_080b404c(void)
{
    ActorSetState(1);
    TaskSetEntry(PickupEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_080b406c(void)
{
    ActorSetState(0);
    TaskSetEntry(PickupEnterState, gCurTaskIdx);
    return 1;
}

s32 sub_080b408c(void)
{
    struct Task *t;

    t = gCurTask;
    t->accelY = 128 << 5;
    t->speedLimitY = 160 << 9;
    return 0;
}

void PickupInit(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->updateCallback = (u32)PickupUpdate;
    *(u8 *)((u8 *)ta + 122) = 0;
    TaskInitWaterFlags();
    tb = *c;
    if ((s8)*(u8 *)((u8 *)tb + 123) == 3)
        ActorSetState(2);
    else
        ActorSetState(1);
    sub_080b3f54();
    CallTableEntry(gCurTask->state, 3, gPickupStates);
}

void PickupUpdate(void)
{
    s32 v;

    if (gCurTask->variant == 0 && (u8)sub_08069888() == 0)
        CallTableEntry(gCurTask->updateState, 3, gPickupStateUpdates);
    v = gCurTask->unk34;
    gCurTask->unk34 = ActorTickAnim(v);
    if (v <= 0)
    {
        sub_080b3fcc();
        sub_080b3ffc();
    }
    ActorCheckHits();
    sub_08069bbc();
}

void PickupEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gPickupStates);
}

void sub_080b4174(void)
{
    gCurTask->updateState = 0;
    TaskStopY();
    TaskSleepForever();
}

void sub_080b4190(void)
{
}

void sub_080b4194(void)
{
    struct Task **c;
    struct Task *t;

    c = &gCurTask;
    (*c)->updateState = 1;
    t = *c;
    if (*(u8 *)((u8 *)t + 115) == 0)
    {
        t->accelY = 128 << 6;
        t->speedLimitY = 160 << 10;
    }
    else
    {
        TaskStop();
    }
    TaskSleepForever();
}

void sub_080b41c8(void)
{
}

void sub_080b41cc(void)
{
    struct Task **c;
    struct Task *t;

    c = &gCurTask;
    (*c)->updateState = 2;
    t = *c;
    if (*(u8 *)((u8 *)t + 115) == 0)
    {
        t->accelY = 128 << 5;
        t->speedLimitY = 160 << 9;
    }
    else
    {
        TaskStop();
    }
    TaskSleepForever();
}

void sub_080b4200(void)
{
}

s32 sub_080b4204(u32 a)
{
    s32 r;

    if ((gActivePlayerMask >> a) & 1)
    {
        r = AddPlayerHealth(8, a);
        if (r <= *(s16 *)gMaxHealth - 1)
            return 0;
    }
    return 1;
}

void PickupHeal(void)
{
    struct Task **c;
    struct Task *t;
    s16 *h;
    s32 w;
    u8 *e;

    c = &gCurTask;
    t = *c;
    t->moveCallback = 0;
    t->updateCallback = 0;
    t->lateUpdateCallback = 0;
    w = (s8)*(u8 *)((u8 *)t + 126);
    h = (s16 *)((u8 *)t + 68);
    *h = w;
    e = (u8 *)gPlayerHealth;
    if (*(s16 *)((*h << 1) + (u32)e) != 0)
    {
        FreezeStage(15);
        CallTableEntry((*c)->state, 2, gUnk_087560B8);
        ThawStage();
    }
    ActorDestroy();
}

void MaximTomatoHeal(void)
{
    struct Task **c;
    u8 k4;

    DisablePause();
    c = &gCurTask;
    do
    {
        if (gLocalPlayer == *(s16 *)((u8 *)*c + 68))
            PlaySfx(221);
        k4 = sub_080b4204(*(s16 *)((u8 *)*c + 68));
        TaskYieldTrampoline(8);
    } while (k4 == 0);
    PlayerStartItemShare(*(s16 *)((u8 *)gCurTask + 68), 1);
    EnablePause();
}

void EnergyDrinkHeal(void)
{
    struct Task **c;
    struct Task **c2;
    struct Task *t;
    struct Task *t2;
    u8 *h;
    s32 w;
    s32 w0;
    s32 n;
    u8 k4;
    s32 z;
    u8 *h0;

    w0 = *(u8 *)gExtraMode;
    n = 1;
    if (w0 == 0 && gMetaKnightmareMode == 0)
        n = 2;
    DisablePause();
    c2 = &gCurTask;
    t = *c2;
    h0 = (u8 *)t + 108;
    z = 0;
    *(u16 *)h0 = z;
    if (z >= n)
        goto xend;
    c = c2;
xbody:
    if (gLocalPlayer == *(s16 *)((u8 *)*c + 68))
        PlaySfx(221);
    k4 = sub_080b4204(*(s16 *)((u8 *)*c + 68));
    TaskYieldTrampoline(8);
    if (k4 != 0)
        goto xend;
    t2 = *c;
    h = (u8 *)t2 + 108;
    w = *(u16 *)h + 1;
    *(u16 *)h = w;
    if (*(s16 *)h < n)
        goto xbody;
xend:
    PlayerStartItemShare(*(s16 *)((u8 *)gCurTask + 68), 2);
    EnablePause();
}

s32 sub_080b4390(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    t = *c;
    if (t->state == 1)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        *(u8 *)((u8 *)t + 122) = 0;
        (*c)->velY = 0xFFFD0000;
        PlaySfx(157);
        r = 0;
    }
    return r;
}

s32 sub_080b43d4(void)
{
    ActorSetState(1);
    TaskSetEntry(sub_080b4754, gCurTaskIdx);
    return 1;
}

s32 sub_080b43f4(void)
{
    s32 r;

    if (gCurTask->state == 1)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        TaskToggleFacingAndReverseX();
        PlaySfx(157);
        r = 0;
    }
    return r;
}

s32 sub_080b442c(void)
{
    struct Task *t;
    s32 r;

    t = gCurTask;
    if (t->state == 1)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    else
    {
        t->velY = 0;
        if ((u8)(gTerrainResult[4] - 5) <= 3)
            TaskToggleFacingAndReverseX();
        PlaySfx(157);
        r = 0;
    }
    return r;
}

void sub_080b447c(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    u8 *b3;
    u8 *e;
    s32 i;
    s32 o;
    s32 w;
    s32 z;

    c = &gCurTask;
    t = *c;
    b3 = (u8 *)gTasks;
    i = *(s16 *)((u8 *)t + 68);
    o = i * 144;
    w = *(u8 *)(b3 + o + 67);
    w = -w;
    *(u8 *)((u8 *)t + 67) = w;
    e = (u8 *)gUnk_087560C0;
    TaskSetMotionXFacing(*(s32 *)((*(u8 *)((u8 *)*c + 116) << 2) + (u32)e), 0x5A5A5A5A);
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    t2 = *c;
    t2->unk28 = 130 << 1;
    t2->unk2C = 2;
    t2->unk30 = 48;
    z = 0;
    t2->frame = 4;
    *(u8 *)((u8 *)t2 + 122) = z;
}

void sub_080b44f0(void)
{
    struct Task **c;
    struct Task **c2;
    struct Task *t;
    struct Task *t3;
    s32 w0;
    s32 w;

    c = &gCurTask;
    t = *c;
    w0 = t->unk2C;
    c2 = c;
    if (w0 <= 0)
    {
        w = *(u16 *)((u8 *)t + 60) + 1;
        *(u16 *)((u8 *)t + 60) = w;
        if ((s16)w > 19)
            t->frame = 4;
        (*c2)->unk2C = 2;
    }
    t3 = *c2;
    t3->unk2C = t3->unk2C - 1;
}

s32 sub_080b4524(void)
{
    struct PointPair box;
    struct Task *t = gCurTask;
    s32 r;

    box.x0 = t->pixelX - 640;
    box.y0 = t->pixelY - 640;
    box.x1 = t->pixelX + 640;
    box.y1 = t->pixelY + 640;
    if (TaskIsInRectSlot(&box, t->parent) != 0)
        r = 0;
    else
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 sub_080b45c0(void)
{
    struct Task *t;
    u8 *p;
    s32 r;

    t = gCurTask;
    p = *(u8 **)((u8 *)t + 136);
    if (((gActivePlayerMask >> *(s16 *)((u8 *)t + 68)) & 1) && *(s8 *)(p + 13) == 0)
        r = 0;
    else
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        r = 1;
    }
    return r;
}

void sub_080b460c(void)
{
    u8 *q0;
    s32 w;
    s32 w2;

    q0 = (u8 *)gTerrainBoundsClamp;
    w = *q0;
    if (1 & w)
        goto docall;
    w2 = 2;
    w2 &= w;
    if (w2 == 0)
        goto skip;
docall:
    TaskToggleFacingAndReverseX();
skip:
    if (*(u8 *)gTerrainBoundsClamp & 4)
        gCurTask->velY = 0;
}

void sub_080b4648(void)
{
    struct Task *t;
    s32 w;
    s32 wl;

    if (sub_080b45c0() != 0)
        return;
    if (sub_080b4524() != 0)
        return;
    t = gCurTask;
    wl = t->unk28;
    w = wl;
    wl = wl - 1;
    t->unk28 = wl;
    if (w <= 0)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        return;
    }
    if (sub_0802294c(t) != 0)
    {
        ActorDestroy();
        return;
    }
    sub_080b44f0();
}

void Task_AbilityStar(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;
    s32 w;
    s32 w2;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)TaskMove;
    ta->drawCallback = (u32)ActorDrawWorldInView;
    *(u8 *)((u8 *)ta + 66) = 5;
    tb = *c;
    tb->frameTable = (u32 *)gAbilityStarFrames;
    tb->updateCallback = (u32)sub_080b4714;
    sub_080b447c();
    w = *(u8 *)((u8 *)*c + 123);
    w2 = 1;
    w2 &= w;
    if (w2 == 0)
        goto elsecall;
    w2 = 64;
    w2 &= w;
    if (w2 != 0)
        goto elsecall;
    ActorSetState(1);
    goto after;
elsecall:
    ActorSetState(0);
after:
    CallTableEntry(gCurTask->state, 2, gUnk_087560D0);
}

void sub_080b4714(void)
{
    struct Task *t;
    s32 w;

    if ((u8)sub_080696a0() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_087560D8);
    t = gCurTask;
    w = t->unk30;
    if (w <= 0)
        ActorCheckHits();
    else
        t->unk30 = w - 1;
    sub_08069b84();
}

void sub_080b4754(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_087560D0);
}

void sub_080b4770(void)
{
    gCurTask->updateState = 0;
    TaskSleepForever();
}

void sub_080b4788(void)
{
    sub_080b4648();
}

void sub_080b4794(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateState = 1;
    TaskStop();
    (*c)->accelY = 128 << 3;
    TaskYieldTrampoline(60);
    ActorDie();
}

void sub_080b47c0(void)
{
    sub_080b4648();
}

void Task_StarRodPiece(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gCurTask;
    ta = *c;
    ta->moveCallback = (u32)ActorMove;
    ta->drawCallback = (u32)ActorDrawWorldInView;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->frameTable = (u32 *)gStarRodPieceFrames;
    CallTableEntry(*(u8 *)((u8 *)tb + 115), 3, gStarRodPieceVariants);
}

void sub_080b480c(void)
{
    s32 i;
    s32 o;
    s32 n;
    vu16 *e;
    struct Task *t;

    i = 32;
    n = -1;
    o = 144 << 5;
    do
    {
        e = (vu16 *)gTaskSlotTypes;
        e = (vu16 *)((i << 1) + (u32)e);
        if ((s16)*e != n && *e == 68)
        {
            t = (struct Task *)((u8 *)gTasks + o);
            if (*(u8 *)(*(u8 **)((u8 *)t + 140) + 4) != 0)
            {
                t->posX = *(s16 *)((u8 *)t + 72) << 16;
                t->posY = *(s16 *)((u8 *)t + 74) << 16;
            }
            TaskSetEntry(ActorDie, i);
        }
        o += 144;
        i += 1;
    } while (i <= 62);
}

void sub_080b4878(void)
{
    s32 i;
    u8 *b5;
    s32 w;

    i = 0;
    if (i < gPlayerCount)
    {
        b5 = (u8 *)gPlayerStates;
        do
        {
            if ((gActivePlayerMask >> i) & 1)
            {
                sub_0803e68c(i);
                w = (s8)*(u8 *)(b5 + 116 * i + 13);
                if (w == 24 || w == 11)
                    SetPlayerAbilityNoHud(0, -1, i);
            }
            i++;
        } while (i < gPlayerCount);
    }
    sub_080b480c();
    DisablePause();
}

void sub_080b48e0(void)
{
    struct Task *t;

    TaskStop();
    t = gCurTask;
    t->unk1C = 0;
    t->unk18 = 0;
}

void sub_080b48f8(void)
{
    s32 i;
    s32 k;
    s32 n;
    u16 *pa;
    s32 n2;
    s32 one;
    u16 *pb;
    s32 m5;
    u8 *p2;
    u8 *pc7;

    i = 0;
    k = 0;
    n2 = gPlayerCount;
    pa = &gPlayerCount;
    pc7 = &gActivePlayerCount;
    if (k < n2)
    {
        m5 = gActivePlayerMask;
        n = n2;
        one = 1;
        p2 = (u8 *)gPlayerStates;
        do
        {
            if (((m5 >> i) & one) && *(s8 *)(p2 + 22) == 2)
                k++;
            p2 += 116;
            i++;
        } while (i < n);
    }
    if (k == *pc7)
    {
        pb = pa;
        if (*pb == 1)
            sub_0805ddb0(0);
        else
            sub_0805deac();
        ActorDestroy();
    }
}

void sub_080b4968(void)
{
    s32 i;
    s32 o;
    s32 one;
    s32 b;
    struct Task **c;
    struct Task *t;
    struct Task *t3;
    u8 *p1;
    s32 w1;
    s32 w2;

    i = 0;
    if (i >= gPlayerCount)
        return;
    o = i;
    one = 1;
    do
    {
        if ((gActivePlayerMask >> i) & one)
        {
            b = one << i;
            c = &gCurTask;
            if (!((*c)->unk18 & b))
            {
                t3 = (struct Task *)((u8 *)gTasks + o);
                w1 = i * 116;
                p1 = (u8 *)((u32)gPlayerStates + w1);
                w2 = p1[4];
                if (w2 == 0)
                {
                    if (*(s16 *)((u8 *)t3 + 74) <= 116)
                    {
                        p1[1] = 22;
                        *(u32 *)(p1 + 104) = w2;
                        *(u8 *)((u8 *)t3 + 122) = w2;
                    }
                    else
                    {
                        sub_0805e110(i);
                        t = *c;
                        t->unk18 |= b;
                        t->unk1C++;
                    }
                }
                else if (*(u32 *)(p1 + 104) == 0 && *(s16 *)((u8 *)t3 + 74) > 116)
                {
                    *(u32 *)(p1 + 104) = (u32)gPlayerDefaultTerrainBox;
                }
            }
        }
        o += 144;
        i++;
    } while (i < gPlayerCount);
}

void sub_080b4a34(void)
{
    if (gCurTask->unk1C == gActivePlayerCount)
        sub_080b48f8();
    else
        sub_080b4968();
}

void sub_080b4a5c(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateCallback = (u32)sub_080b4a8c;
    ActorSetState(0);
    CallTableEntry((*c)->state, 2, gUnk_08756150);
}

void sub_080b4a8c(void)
{
    struct Task **c;

    c = &gCurTask;
    CallTableEntry((*c)->updateState, 2, gUnk_08756158);
    if ((*c)->state == 0 && ActorCheckHits() != 0)
    {
        sub_080b4878();
        CreateBurstEffect(0, 0);
        if (gLocalPlayer == (s8)*(u8 *)((u8 *)*c + 126))
            PlaySfx(198);
        ActorSetState(1);
        TaskSetEntry(sub_080b4afc, gCurTaskIdx);
    }
}

void sub_080b4afc(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08756150);
}

void sub_080b4b18(void)
{
    struct Task **c;
    struct Task *t;
    s32 v6;
    s32 v5;
    s32 r;

    c = &gCurTask;
    (*c)->updateState = 0;
    *(u8 *)((u8 *)*c + 67) = 1;
    TaskStop();
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_087560EC);
    t = *c;
    t->unk34 = r;
    v6 = 0xFFFFC000;
    v5 = 128 << 7;
    for (;;)
    {
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = 0xFFFFA000;
        TaskYieldTrampoline(16);
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
        (*c)->velY = 192 << 7;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
    }
}

void sub_080b4b94(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk34);
    t = *c;
    t->unk34 = r;
}

void sub_080b4bb0(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateState = 1;
    sub_080b48e0();
    (*c)->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_080b4bd8(void)
{
    sub_080b4a34();
}

void sub_080b4be4(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateCallback = (u32)sub_080b4c14;
    ActorSetState(0);
    CallTableEntry((*c)->state, 3, gUnk_08756160);
}

void sub_080b4c14(void)
{
    struct Task **c;

    c = &gCurTask;
    CallTableEntry((*c)->updateState, 3, gUnk_0875616C);
    if ((*c)->state == 1 && ActorCheckHits() != 0)
    {
        sub_080b4878();
        CreateBurstEffect(0, 0);
        if (gLocalPlayer == (s8)*(u8 *)((u8 *)*c + 126))
            PlaySfx(198);
        ActorSetState(2);
        TaskSetEntry(sub_080b4c84, gCurTaskIdx);
    }
}

void sub_080b4c84(void)
{
    CallTableEntry(gCurTask->state, 3, gUnk_08756160);
}

void sub_080b4ca0(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    (*c)->updateState = 0;
    *(u8 *)((u8 *)*c + 67) = 255;
    TaskStop();
    r = ActorStartAnimNoFlip((struct AnimCmd *)gUnk_087560EC);
    t = *c;
    t->unk34 = r;
    t->velX = 0xFFFC0000;
    TaskYieldTrampoline(8);
    (*c)->velX = 0xFFFE0000;
    TaskYieldTrampoline(8);
    (*c)->velX = 0xFFFF0000;
    TaskYieldTrampoline(8);
    (*c)->velX = 0xFFFF8000;
    TaskYieldTrampoline(8);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080b4d1c(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk34);
    t = *c;
    t->unk34 = r;
    if (t->state != 0)
        TaskSetEntry(sub_080b4c84, gCurTaskIdx);
}

void sub_080b4d50(void)
{
    struct Task **c;
    struct Task *t;
    s32 v6;
    s32 v5;
    s32 r;

    c = &gCurTask;
    (*c)->updateState = 1;
    TaskStop();
    v6 = 0xFFFFC000;
    v5 = 128 << 7;
    for (;;)
    {
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = 0xFFFFA000;
        TaskYieldTrampoline(16);
        (*c)->velY = v6;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
        (*c)->velY = 192 << 7;
        TaskYieldTrampoline(16);
        (*c)->velY = v5;
        TaskYieldTrampoline(16);
    }
}

void sub_080b4db4(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gCurTask;
    r = ActorTickAnim((*c)->unk34);
    t = *c;
    t->unk34 = r;
}

void sub_080b4dd0(void)
{
    struct Task **c;

    c = &gCurTask;
    (*c)->updateState = 2;
    sub_080b48e0();
    (*c)->frame = 0xFFFF;
    TaskSleepForever();
}

void sub_080b4df8(void)
{
    sub_080b4a34();
}

void sub_080b4e04(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = 0;
    if (*(u8 *)((u8 *)t + 116) != 2)
        TaskYieldTrampoline(60);
    if (sub_08066394() != 0)
    {
        FreezeStage(15);
        ExitClearedStage();
    }
    TaskSleepForever();
}

void sub_080b4e40(void)
{
    u8 *p1;
    u8 *q;
    s32 w;
    s32 m5;
    s32 z4;
    s32 z3;
    s32 k2;
    s32 i2;

    m5 = 255;
    z4 = 0;
    z3 = 0;
    p1 = (u8 *)gRoomObjectGfxSlots;
    k2 = 9;
    do
    {
        w = *p1;
        w |= m5;
        *p1 = w;
        *(u16 *)(p1 + 2) = z3;
        *(u8 *)(p1 + 1) = z4;
        p1 += 4;
        k2 -= 1;
    } while (k2 >= 0);
    i2 = 0;
    do
    {
        ((u8 *)gRoomObjectTried)[i2] = 0;
        q = (u8 *)gRoomObjectGfxSlotIds + i2;
        w = *q;
        w |= 255;
        *q = w;
        i2++;
    } while (i2 <= 47);
    i2 = 0;
    do
    {
        q = (u8 *)gUnk_02005590 + i2;
        w = *q;
        w |= 255;
        *q = w;
        i2++;
    } while (i2 <= 30);
    LoadRoomObjectGfx();
}
