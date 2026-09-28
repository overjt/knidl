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
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
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
extern void sub_08030db8();
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
void sub_080b54a4();
s32 sub_080b5670();
s32 sub_080b5840();
s32 sub_080b590c();
void sub_080b59d8();
s32 sub_080b5a94();
s32 sub_080b5bdc();
s32 sub_080b5d84();

void MetaKnightInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)MetaKnightUpdate;
    t->unk28 = 0;
    t->unk2C = -1;
    t->unk30 = -1;
    t->unk34 = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[3] = (s16)ActorComputeHealth();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 24, gMetaKnightStates);
}

void MetaKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 24, gMetaKnightStates);
}

void MetaKnightUpdate(void)
{
    struct Task *t;
    u32 rr;
    u32 *q;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 24, gMetaKnightStateUpdates);
    if (gCurTask->unk30 != -1)
    {
        ActorSetAttackBox(gUnk_08748D38[gCurTask->unk30]);
        gUnk_02007D00[1] = gCurTask->health;
        sub_08068f68();
        switch (gCurTask->hitKind)
        {
        case 1:
        case 2:
            if ((u8)sub_080a720c() == 1)
            {
                gCurTask->hitKind = 0;
                t = gCurTask;
                t->health = gUnk_02007D00[1];
                t->unk30 = 2;
                ActorSetState(22);
                sub_080a7168();
            }
            break;
        case 6:
            gUnk_02007D00[0] = gCurTask->hitterPlayer;
            switch (gCurTask->hitEffect)
            {
            case 4:
                gUnk_03001F2C = sub_080a6e1c();
                q = &gUnk_03002448;
                rr = RandomRange(16);
                *q = rr;
                if ((s32)rr < gUnk_08748D6C[gUnk_03001F2C])
                {
                    gCurTask->unk30 = 2;
                    ActorSetState(22);
                    sub_080a7168();
                }
                else if ((s32)rr < gUnk_08748D70[gUnk_03001F2C])
                {
                    gCurTask->unk30 = 2;
                    ActorSetState(16);
                    sub_080a7168();
                }
                break;
            case 2:
            case 3:
                gCurTask->unk30 = 2;
                ActorSetState(23);
                sub_080a7168();
                break;
            }
            break;
        }
        ActorReactToHit();
    }
}

void sub_080a57d4(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s32 i;

    gCurTask->updateState = 0;
    gCurTask->posX = (gViewRect[0] + 216) << 16;
    gCurTask->posY = (gViewRect[2] + 44) << 16;
    if (gMetaKnightmareMode == 0)
    {
        for (i = 0; i < gActivePlayerCount; i++)
        {
            sp.subtype = 18;
            sp.taskType = 120;
            sp.variant = 0;
            sp.spawnArg = i;
            sp.x = gUnk_08748D28[i + (gActivePlayerCount - 1) * 4] + gViewRect[0];
            sp.y = gViewRect[2];
            sp.tileWord = gCurTask->u8C.actor->savedTileWord;
            sp.checkTerrain = 0;
            CreateActorFromDesc(&sp, 1);
        }
    }
    else
        gUnk_02007D00[2] = gPlayerCount;
    gCurTask->facing = 1;
    TaskSetFrame(4);
    while (gUnk_02007D00[2] != gActivePlayerCount)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(1);
    gCurTask->unk30 = 2;
    sub_08066544();
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(185, (s16)(t->pixelX + 8), t->pixelY, t->u8C.actor->savedTileWord);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(18);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(188, (s16)(t->pixelX - 32), t->pixelY, t->u8C.actor->savedTileWord | (240 << 8));
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    TaskStopY();
    TaskYieldTrampoline(20);
    gCurTask->facing = 255;
    TaskSetFrame(60);
    TaskYieldTrampoline(10);
    CreateChildTaskHere(184, 0);
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(9);
    TaskSleepForever();
}

void sub_080a5a78(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5aa0(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = 1;
    t = gCurTask;
    t->unk28 &= ~128;
    t->unk30 = 0;
    v = TaskFindNearestPlayer();
    gUnk_02007D00[5] = v;
    if (gUnk_0300244C != 0)
    {
        if (gCurTask->unk2C != v)
        {
            gCurTask->unk20 = 0;
            gCurTask->unk24 = 0;
            gCurTask->unk2C = gUnk_02007D00[5];
        }
    }
    sub_080a6e98();
}

void sub_080a5af4(void)
{
    struct Task *pt;
    s16 *px;
    vu16 *pv;
    vu16 *f9;
    s32 d;

    gCurTask->unk1C--;
    if (gCurTask->unk1C == 0)
    {
        sub_080a7190();
        return;
    }
    pt = &gTasks[gUnk_02007D00[5]];
    px = &pt->pixelX;
    d = (*px + gCurTask->unk18) - gCurTask->pixelX;
    switch (gCurTask->unk24)
    {
    case 0:
        if (gCurTask->unk34 & 2)
        {
            if (d < 0)
                gCurTask->facing = 255;
            else
                gCurTask->facing = 1;
        }
        else
            TaskFaceNearestPlayer();
        if (abs(d) <= 1)
        {
            gCurTask->unk24 = 1;
            gCurTask->velX = 0;
            gUnk_02007D00[7] = 0;
        }
        else if (abs(d) <= 27)
        {
            gCurTask->unk20 &= ~2;
            if (d > 0)
                gCurTask->velX = 128 << 9;
            else
                gCurTask->velX = -0x10000;
        }
        else
        {
            gCurTask->unk20 |= 2;
            if (d > 0)
                gCurTask->velX = 128 << 10;
            else
                gCurTask->velX = -0x20000;
        }
        gUnk_02007D00[6] = pt->pixelX;
        break;
    case 1:
        gCurTask->unk34 &= ~6;
        if ((u8)sub_080a6f38(gCurTask->pixelX, gCurTask->pixelY) == 1)
        {
            gCurTask->unk24 = 2;
            gCurTask->velX = 0;
        }
        else
        {
            gCurTask->pixelX = *px + gCurTask->unk18;
            gCurTask->posX = gCurTask->pixelX << 16;
            TaskFaceNearestPlayer();
            f9 = gPlayerHeldKeys;
            pv = &f9[gUnk_02007D00[5]];
            if ((*pv & 48) && *px != gUnk_02007D00[6])
            {
                if ((u16)(*pv & 16) != 0)
                {
                    if (gUnk_02007D00[7] < 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]++;
                }
                else
                {
                    if (gUnk_02007D00[7] > 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]--;
                }
                if (abs(gUnk_02007D00[7]) == 15)
                    gCurTask->unk24 = 0;
            }
            else if (pt->velX == 0)
            {
                gCurTask->unk24 = 2;
                gCurTask->velX = 0;
            }
        }
        gUnk_02007D00[6] = pt->pixelX;
        break;
    case 2:
        TaskFaceNearestPlayer();
        if (*px != gUnk_02007D00[6])
        {
            if ((u8)sub_080a6f38((s16)(*px + gCurTask->unk18), gCurTask->pixelY) == 0)
            {
                gCurTask->unk24 = 0;
                gCurTask->unk20 &= ~1;
                break;
            }
        }
        if (gCurTask->frame == 65)
            gCurTask->unk20 |= 1;
        break;
    }
    if ((u8)sub_080a6f74() == 1)
    {
        gCurTask->velX = 0;
        gCurTask->unk24 = 2;
        if (!(gCurTask->unk34 & 1))
        {
            gCurTask->unk34 |= 1;
            gCurTask->unk1C = RandomRange(30) + 30;
        }
    }
    TaskUpdateFlip();
    if (!(gCurTask->unk34 & 6))
    {
        if (abs(TaskGetNearestPlayerDx()) <= 31)
        {
            ActorSetState(gUnk_08748DA8[pt->onGround][RandomRange(8)]);
            sub_080a7168();
        }
    }
}

void sub_080a5dd0(void)
{
    gCurTask->updateState = 2;
    if (!(gCurTask->unk34 & 2))
    {
        if (gCurTask->facing == 1)
            gCurTask->unk18 = 64;
        else
            gCurTask->unk18 = -64;
    }
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk34 &= 2;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5e30(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 2)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5e60(void)
{
    gCurTask->updateState = 3;
    gCurTask->unk18 = -gCurTask->unk18;
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk34 = 2;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5e9c(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 3)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5ecc(void)
{
    gCurTask->updateState = 4;
    gCurTask->unk1C = 48;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5ef0(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 4)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5f20(void)
{
    struct Task *t;

    gCurTask->updateState = 5;
    gCurTask->unk34 = 4;
    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->facing == 1)
        t->unk18 = -64;
    else
        t->unk18 = 64;
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5f7c(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 5)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5fac(void)
{
    gCurTask->updateState = 6;
    if (gCurTask->unk18 < 0)
        gUnk_02007D00[6] = 1;
    else
        gUnk_02007D00[6] = 0;
    {
        struct Task *t2 = &gTasks[TaskFindNearestPlayer()];

        gUnk_02007D00[7] = t2->pixelX;
    }
    gCurTask->unk20 = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskSetFrame(61);
    sub_080a6e98();
}

void sub_080a6020(void)
{
    s32 d;
    s32 x;

    ClampTaskToRoom(gCurTask);
    x = gCurTask->pixelX;
    d = x - gUnk_02007D00[7];
    if (d >= 0 ? d <= 5 : gUnk_02007D00[7] - x <= 5)
    {
        if (abs(TaskGetNearestPlayerDy()) <= 47)
            ActorSetState(gUnk_08748E48[RandomRange(16)]);
        else
            ActorSetState(gUnk_08748E68[RandomRange(16)]);
        if (gCurTask->state == 3)
            gUnk_02007D00[6]++;
        gCurTask->unk18 = gUnk_08748D60[gUnk_02007D00[6]];
        gCurTask->unk34 = 2;
        sub_080a7168();
    }
}

void MetaKnightRun(void)
{
    gCurTask->updateState = 7;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(224 << 9, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(82);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
    }
}

void MetaKnightRunUpdate(void)
{
    ClampTaskToRoom(gCurTask);
    if (abs(TaskGetNearestPlayerDx()) <= 43)
    {
        gCurTask->unk34 = 0;
        if (gCurTask->health >= gUnk_02007D00[3] >> 1)
            ActorSetState(gMetaKnightNearStates[RandomRange(8)]);
        else
            ActorSetState(gMetaKnightNearStatesLowHealth[RandomRange(8)]);
        sub_080a7168();
    }
}

void sub_080a61b4(void)
{
    gCurTask->updateState = 9;
    gCurTask->unk30 = 0;
    gUnk_02007D00[7] = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x26000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    if (gCurTask->onGround == 0)
    {
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(10);
    TaskSleepForever();
}

void sub_080a6264(void)
{
    sub_080a6fb4();
    if (gCurTask->state != 9)
        sub_080a7168();
}

void sub_080a6280(void)
{
    gCurTask->updateState = 8;
    gUnk_02007D00[7] = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x33000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    if (gCurTask->onGround == 0)
    {
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(10);
    TaskSleepForever();
}

void sub_080a6330(void)
{
    sub_080a6fb4();
    if (gCurTask->state != 8)
        sub_080a7168();
}

void sub_080a634c(void)
{
    gCurTask->updateState = 10;
    TaskStop();
    gCurTask->unk30 = 0;
    TaskSetFrame(81);
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a638c(void)
{
    if (gCurTask->state != 10)
        sub_080a7168();
}

void sub_080a63a4(void)
{
    gCurTask->updateState = 11;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a63d8(void)
{
    if (gCurTask->state != 11)
        sub_080a7168();
}

void sub_080a63f0(void)
{
    gCurTask->updateState = 12;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 0;
    gUnk_02007D00[6] = 0;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6420(void)
{
    if (gCurTask->state != 12)
        sub_080a7168();
}

void sub_080a6438(void)
{
    struct Task *t;
    s32 i;

    gCurTask->updateState = 13;
    t = gCurTask;
    t->accelY = 144 << 7;
    t->speedLimitY = 128 << 11;
    TaskGetScreenPosSlot(gCurTaskIdx);
    if (gUnk_030023D4 <= 43)
        gUnk_02007D00[7] = 4;
    else
        gUnk_02007D00[7] = 2;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_02007D00[7])
    {
        PlaySfx(0x225);
        TaskSetFrame(114);
        TaskYieldTrampoline(1);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk6E++;
        } while ((s16)gCurTask->unk6E <= 6);
        gCurTask->unk6C++;
    }
    TaskSetFrame(122);
    TaskYieldTrampoline(2);
    TaskSetFrame(80);
    TaskSleepForever();
}

void sub_080a650c(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(10);
        sub_080a7168();
    }
}

void sub_080a6544(void)
{
    gCurTask->updateState = 16;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 15;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6574(void)
{
    if (gCurTask->state != 16)
        sub_080a7168();
}

void sub_080a658c(void)
{
    gCurTask->updateState = 15;
    TaskStop();
    TaskSetFrame(111);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskSetMotionY(128 << 10, 144 << 7, 128 << 11);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(2);
    PlaySfx(504);
    CreateLandingImpact(0, 0, 3);
    gCurTask->unk30 = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a6628(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->state != 15)
        sub_080a7168();
}

void sub_080a6650(void)
{
    gCurTask->updateState = 14;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 17;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6680(void)
{
    if (gCurTask->state != 14)
        sub_080a7168();
}

void sub_080a6698(void)
{
    gCurTask->updateState = 17;
    sub_080a7080();
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    TaskSetFrame(80);
    TaskSleepForever();
}

void sub_080a66c8(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->onGround != 0)
    {
        StopSfxOnPlayer(gUnk_02007D00[4], 137 << 2);
        TaskStop();
        ActorSetState(10);
        sub_080a7168();
    }
}

void sub_080a6710(void)
{
    gCurTask->updateState = 18;
    TaskStop();
    sub_080a7080();
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a6734(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 18)
        sub_080a7168();
}

void sub_080a6754(void)
{
    struct Task *t;

    gCurTask->updateState = 19;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(12);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk30 = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(24);
    TaskSetFrame(60);
    TaskYieldTrampoline(28);
    ActorSetState(5);
    TaskSleepForever();
}

void sub_080a680c(void)
{
    s32 v;

    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->velX = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->state != 19)
        sub_080a7168();
}

void sub_080a6868(void)
{
    struct Task *t;

    gCurTask->updateState = 20;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskSetFrame(97);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    TaskSetFrame(88);
    TaskYieldTrampoline(10);
    ActorSetState(21);
    TaskSleepForever();
}

void sub_080a6988(void)
{
    s32 v;

    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->velX = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->state != 20)
        sub_080a7168();
}

void sub_080a69e4(void)
{
    struct Task *t;

    gCurTask->updateState = 21;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(105);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(20);
    gCurTask->unk30 = 0;
    TaskSetFrame(60);
    TaskYieldTrampoline(48);
    ActorSetState(5);
    TaskSleepForever();
}

void sub_080a6aac(void)
{
    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 25)
    {
        gUnk_02007D00[7]++;
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2
             && gUnk_02007D00[7] >= gUnk_08748D4C[(s16)gCurTask->unk6C]; gCurTask->unk6C++)
            ;
        TaskSetMotionXFacing(gUnk_08748D50[(s16)gCurTask->unk6C], 0x5A5A5A5A);
    }
    if (gCurTask->state != 21)
        sub_080a7168();
}

void sub_080a6b3c(void)
{
    struct Task *t;

    gCurTask->updateState = 22;
    gUnk_02007D00[6] = gCurTask->onGround;
    gUnk_02007D00[7] = 20;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    t = gCurTask;
    if (t->onGround == 0)
    {
        CreateChildTask(188, (s16)(t->pixelX + t->facing * 4), (s16)(t->pixelY - 4),
                     t->u8C.actor->savedTileWord | (240 << 8));
        gCurTask->accelY = 168 << 5;
        gCurTask->speedLimitY = 192 << 10;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(70);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
    }
    else
    {
        CreateChildTask(188, (s16)(t->pixelX + t->facing * 16), (s16)(t->pixelY + 4),
                     t->u8C.actor->savedTileWord | (240 << 8));
        TaskStop();
        TaskSetFrame(69);
    }
    TaskSleepForever();
}

void sub_080a6c3c(void)
{
    s32 *pD;
    u8 *b3v;
    s32 *pE;
    s32 w;
    s32 v;

    ClampTaskToRoom(gCurTask);
    pD = gUnk_02007D00;
    v = pD[7];
    if (v == 0)
    {
        gCurTask->unk30 = v;
        if (gCurTask->onGround != 0)
        {
            if (pD[6] == 0)
            {
                TaskStop();
                ActorSetState(10);
                TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
            }
            else
            {
                TaskStop();
                gCurTask->unk24 = v;
                sub_080a7190();
            }
        }
    }
    else
    {
        w = v - 1;
        pD[7] = w;
        pD[5] = 0;
        b3v = (u8 *)gUnk_08748D78;
        pE = pD;
        while (w >= *(u8 *)(pD[5] + (u32)b3v))
            pD[5]++;
        TaskSetMotionXFacing(gUnk_08748D80[pE[5]], 0x5A5A5A5A);
        if (gCurTask->onGround != 0)
        {
            TaskStopY();
            pE[6] = gCurTask->onGround;
            TaskSetFrame(69);
        }
    }
}

void sub_080a6d08(void)
{
    struct Task *t;

    gCurTask->updateState = 23;
    TaskStop();
    gUnk_02007D00[6] = gCurTask->onGround;
    t = gCurTask;
    if (t->onGround == 0)
    {
        t->accelY = 168 << 5;
        t->speedLimitY = 192 << 10;
    }
    gUnk_02007D00[7] = 60;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    TaskSetFrame(69);
    TaskSleepForever();
}

void sub_080a6d60(void)
{
    u16 v;

    ClampTaskToRoom(gCurTask);
    if (gPlayerStates[gUnk_02007D00[0]].unk40 & 4) {
        if (gCurTask->onGround != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                gUnk_02007D00[6] = gCurTask->onGround;
            }
            if (--gUnk_02007D00[7] == 0) {
                gCurTask->unk30 = 1;
                v = gUnk_08748EA8[RandomRange(2)];
                ActorSetState(v);
                TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
            }
        }
    } else {
        if (gCurTask->onGround != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                v = 10;
            } else {
                v = 2;
            }
            ActorSetState(v);
            TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
        } else {
            TaskSetFrame(70);
        }
    }
}

s32 sub_080a6e1c(void)
{
    s32 x = gCurTask->health;
    s32 q = gUnk_02007D00[3] >> 2;

    if (x < q)
        return 0;
    if (x < gUnk_02007D00[3] >> 1)
        return 1;
    if (x < q * 3)
        return 2;
    return 3;
}

void sub_080a6e58(void)
{
    if (gCurTask->health >= gUnk_02007D00[3] >> 1)
        gCurTask->unk1C = RandomRange(54) + 48;
    else
        gCurTask->unk1C = RandomRange(84) + 96;
}

void sub_080a6e98(void)
{
    struct Task **c = &gCurTask;
    struct Task **d;
    u8 *a = gUnk_08748D98;
    u8 *b = a + 8;
    struct Task *t;
    struct Task *u;
    s32 ix;
    s32 n;

    for (;;)
    {
        t = *c;
        if (t->unk20 & 2)
        {
            ix = t->frame - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)b));
        }
        else
        {
            ix = t->frame - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)a));
        }
        t = *c;
        n = t->unk20 & 1;
        d = &gCurTask;
        if (n == 0)
        {
            if ((t->facing == 1 && t->velX >= 0) || (t->facing == -1 && t->velX <= 0))
            {
                u = *c;
                u->frame++;
                if ((s16)u->frame > 68)
                    u->frame = 61;
            }
            else
            {
                u = *d;
                u->frame--;
                if ((s16)u->frame <= 60)
                    u->frame = 68;
            }
        }
    }
}

s32 sub_080a6f38(s16 x, s16 y)
{
    if ((u16)GetCollisionTileAtOffset(x, y, 1, 0) == 0 && (u16)GetCollisionTileAtOffset(x, y, -1, 0) == 0)
        return 0;
    return 1;
}

s32 sub_080a6f74(void)
{
    u8 v = ClampTaskToRoom(gCurTask);

    if (((v & 1) && gCurTask->velX < 0) || ((v & 2) && gCurTask->velX > 0))
        return 1;
    return 0;
}

void sub_080a6fb4(void)
{
    s32 k;
    s32 g;

    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gUnk_02007D00[7] == 0 && gCurTask->onGround == 0 && gCurTask->velY > 0)
    {
        if (gUnk_02007D00[6] == 0)
        {
            gCurTask->unk34 = 0;
            gUnk_02007D00[7]++;
            gUnk_02007D00[4] = TaskGetNearestPlayerDx();
            if (TaskGetNearestPlayerDy() < 0)
                k = 0;
            else
            {
                g = gUnk_02007D00[4];
                if (abs(g) <= 15)
                    k = 1;
                else if ((g > 0 && gCurTask->facing == 1) ||
                         (g < 0 && gCurTask->facing == -1))
                    k = 2;
                else
                    k = 3;
            }
            gUnk_02007D00[6] = gUnk_08748E28[k][RandomRange(4)];
            if (gUnk_02007D00[6] == 0)
                return;
        }
        ActorSetState((u16)gUnk_02007D00[6]);
    }
}

void sub_080a7080(void)
{
    gUnk_02007D00[4] = -1;
    TaskSetFrame(98);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gUnk_02007D00[4] = PlaySfx(137 << 2);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x40000;
    gCurTask->accelY = 128 << 8;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskStopY();
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->velY = 128 << 11;
    gCurTask->accelY = -0x8000;
    TaskSetFrame(101);
    TaskYieldTrampoline(2);
    TaskStopY();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
}

void sub_080a7168(void)
{
    gCurTask->unk28 |= 128;
    TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a7190(void)
{
    struct Task *pt = &gTasks[TaskFindNearestPlayer()];

    if (gCurTask->unk34 & 1)
        ActorSetState(gUnk_08748DC8[RandomRange(16)]);
    else
        ActorSetState(gUnk_08748DE8[pt->onGround][RandomRange(16)]);
    gCurTask->unk34 = 0;
    sub_080a7168();
}

s32 sub_080a720c(void)
{
    u8 k;

    if (gCurTask->unk28 & 128)
        gCurTask->unk28++;
    if (gCurTask->unk28 & 3)
    {
        gCurTask->unk28--;
        k = sub_080a6e1c();
        if (RandomRange(16) < gUnk_08748D74[k])
            return 1;
    }
    return 0;
}

s32 sub_080a7260(void)
{
    gBg2Cnt |= 64;
    gBg3Cnt |= 64;
    gCurTask->unk28 |= 2;
    sub_0806619c(13, (u32)sub_080a72b0, (u32)gUnk_082F427C, 16, 1);
    return 0;
}

void sub_080a72b0(void)
{
    gBgMosaic = gUnk_08748EAC[gUnk_02006190[3] >> 1];
    if (gUnk_02006190[3] == 0)
    {
        gBg2Cnt &= 0xFFBF;
        gBg3Cnt &= 0xFFBF;
        sub_0806621c();
    }
}

s32 sub_080a72fc(void)
{
    TaskStop();
    ActorSetHitReactions((u32)gUnk_08749B30);
    gCurTask->unk30 = -1;
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 sub_080a7334(void)
{
    struct Task *t;

    switch (gCurTask->state)
    {
    case 1:
        t = gCurTask;
        t->unk24 = 1;
        t->velX = 0;
        break;
    case 8:
    case 13:
    case 15:
    case 17:
        TaskTurnAroundAndReverseX();
        break;
    }
    return 0;
}

void sub_080a73b4(void)
{
    struct Task *t = gCurTask;

    t->drawCallback = (u32)sub_080653ec;
    t->updateCallback = (u32)sub_080a73fc;
    t->frameTable = gMetaKnightFrames;
    t->layer = 4;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08748F7C);
}

void sub_080a73fc(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->updateState, 2, gUnk_08748F84);
}

void sub_080a741c(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08748F7C);
}

void sub_080a7438(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->onGround = 0;
    gCurTask->accelY = 168 << 5;
    gCurTask->speedLimitY = 192 << 10;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    sp.subtype = 18;
    sp.taskType = 120;
    sp.variant = 1;
    sp.spawnArg = 0;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = gCurTask->u8C.actor->savedTileWord + (128 << 5);
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 1);
    TaskGetScreenPosSlot(gCurTaskIdx);
    gCurTask->unk30 = gUnk_030023B4;
    TaskGetNearestPlayerScreenPos();
    if ((u32)(gUnk_030023B4 - 88) > 64)
        gCurTask->unk28 = 128;
    else if (gCurTask->unk30 <= 127)
        gCurTask->unk28 = 64;
    else
        gCurTask->unk28 = 176;
    if (gCurTask->unk30 < gCurTask->unk28)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -1;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskSetFrame(21);
    gCurTask->unk2C = abs(gCurTask->unk30 - gCurTask->unk28) >> 1;
    TaskSetMotionY(-((gCurTask->unk2C >> 1) << 13), 128 << 6, 192 << 10);
    if (gCurTask->velY == 0)
        gCurTask->velY = -0x10000;
    gCurTask->onGround = 0;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a75a0(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_080a741c, gCurTaskIdx);
}

void sub_080a75c8(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(14);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    t = gCurTask;
    CreateChildTask(186, (s16)(t->pixelX - t->facing * 2), (s16)(t->pixelY - 1), t->u8C.actor->savedTileWord);
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x30000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x20000;
    gCurTask->accelY = 128 << 9;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x44000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    TaskYieldTrampoline(9);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetFrame(45);
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(57);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    gCurTask->onGround = 0;
    gCurTask->velY = -0x70000;
    while (gCurTask->pixelY > 10)
        TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(30);
    FadeOutBgm(8);
    TaskYieldTrampoline(32);
    CreateStarRodPiece(0, 128, 104);
    ActorDestroy();
}

void sub_080a787c(void)
{
}

void Task_MetaKnightSwordHitBox(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)MetaKnightSwordHitBoxUpdate;
    TaskSleepForever();
}
