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

void Task_MetaKnightCape(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)TaskMove;
    t->drawCallback = (u32)ActorDrawWorldInView;
    t->layer = 12;
    gCurTask->frameTable = gUnk_08753DA0;
    gCurTask->facing = 1;
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, -0xC000, 128 << 5, 0x5A5A5A5A);
    TaskSetFrame(0);
    TaskYieldTrampoline(3);
    gCurTask->unk6E = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(3);
        gCurTask->unk6E++;
    } while ((s16)gCurTask->unk6E <= 2);
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(1);
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetFrame(2);
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetFrame(2);
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetFrame(3);
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(1);
    TaskSetFrame(0);
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(1);
    gCurTask->frame = -1;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void Task_MetaKnightMask(void)
{
    struct Task *t;
    s32 v;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 3;
    gCurTask->frameTable = gUnk_08753DA0;
    TaskFaceLikeParent();
    gCurTask->velY = -0x10000;
    gCurTask->accelY = -0x10000;
    TaskSetFrame(26);
    TaskYieldTrampoline(3);
    gCurTask->unk6C = 0;
    do
    {
        t = gCurTask;
        v = CreateChildTask(187, t->pixelX, t->pixelY, t->tileWord);
        gTasks[v].unk18 = (s16)gCurTask->unk6C;
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->frame = 0xFFFF;
    TaskSleepForever();
}

void Task_MetaKnightMaskHalf(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 3;
    gCurTask->frameTable = gUnk_08753DA0;
    TaskFaceLikeParent();
    if (gCurTask->unk18 == 0)
    {
        TaskSetMotionXFacing(-0xC000, 0);
        gCurTask->velY = -0x1F800;
        gCurTask->accelY = 192 << 5;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(31);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
    }
    else
    {
        TaskSetMotionXFacing(192 << 8, 0);
        gCurTask->velY = -0x1F800;
        gCurTask->accelY = 192 << 5;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(27);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
    }
    TaskExitTrampoline();
}

void Task_MetaKnightSparkle(void)
{
    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 3;
    gCurTask->frameTable = gUnk_08753DA0;
    TaskSetFrame(13);
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetFrame(36);
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

s32 sub_080a7d20(void)
{
    TaskSetFrame(0);
}

void Task_Kracko(void)
{
    struct Task *t;

    sub_08066088(0);
    sub_08066144();
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInView;
    gCurTask->layer = 10;
    t = gCurTask;
    t->frameTable = gKrackoFrames;
    t->u8C.actor->defeatSweepCallback = (u32)sub_080a9e88;
    t->tileWord |= 128 << 4;
    CallTableEntry(t->variant, 2, gKrackoVariants);
}

void KrackoJrInit(void)
{
    struct Task *t;

    gCurTask->updateCallback = (u32)KrackoJrUpdate;
    ActorSetAttackBox((u32)gUnk_08749704);
    t = gCurTask;
    t->unk28 = 0;
    t->unk34 = 0;
    t->unk30 = 0;
    t->unk2C = 0;
    gUnk_02007D00[0] = 1;
    gCurTask->unk46 = CreateChildTaskHere(195, 1);
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gKrackoJrStates);
}

void KrackoJrEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gKrackoJrStates);
}

void KrackoJrUpdate(void)
{
    sub_080a9738();
    CallTableEntry(gCurTask->updateState, 2, gKrackoJrStateUpdates);
    if (gCurTask->updateState == 0)
        ActorCheckHits();
    ActorReactToHit();
}

void sub_080a7e44(void)
{
    gCurTask->updateState = 0;
    for (;;)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 != 10);
        TaskStop();
        gCurTask->velX = 0;
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velX = -0x10000;
        gCurTask->velY = 0;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0;
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(4);
        gCurTask->velX = 128 << 9;
        gCurTask->velY = 0;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0x10000;
        gCurTask->velY = 0;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(4);
        gCurTask->velX = 128 << 9;
        gCurTask->velY = 0;
        TaskYieldTrampoline(4);
        gCurTask->velX = 0;
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(4);
        gCurTask->velX = -0xC000;
        gCurTask->velY = 128 << 7;
        TaskYieldTrampoline(4);
        sub_08030db8((u32)gUnk_08749B84);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velX = 0;
            gCurTask->velY = 192 << 8;
            TaskYieldTrampoline(1);
            gCurTask->velX = 192 << 8;
            gCurTask->velY = 0;
            TaskYieldTrampoline(1);
            gCurTask->velX = 0;
            gCurTask->velY = -0xC000;
            TaskYieldTrampoline(1);
            gCurTask->velX = -0xC000;
            gCurTask->velY = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 5);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velX = 0;
            gCurTask->velY = 128 << 9;
            TaskYieldTrampoline(2);
            gCurTask->velX = 128 << 9;
            gCurTask->velY = 0;
            TaskYieldTrampoline(2);
            gCurTask->velX = 0;
            gCurTask->velY = -0x10000;
            TaskYieldTrampoline(2);
            gCurTask->velX = -0x10000;
            gCurTask->velY = 0;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        gCurTask->velX = 0;
        gCurTask->velY = 192 << 8;
        TaskYieldTrampoline(1);
        gCurTask->velX = 192 << 8;
        gCurTask->velY = 0;
        TaskYieldTrampoline(1);
        gCurTask->velX = 0;
        gCurTask->velY = 128 << 9;
        TaskYieldTrampoline(1);
        TaskStop();
        TaskYieldTrampoline(16);
        gUnk_02007D00[0] = 130;
        TaskYieldTrampoline(8);
        gUnk_02007D00[0] = 129;
        gCurTask->unk28 = 9;
    }
}

void sub_080a8038(void)
{
    struct PointPair pp;
    struct Task *pt;
    struct Task *u;
    s32 v;
    s32 w;
    s32 va;
    s32 z5;
    s32 n;
    s32 r;
    s32 d;

    if (gViewRect[2] <= 0x16F)
    {
        TaskStop();
        ActorSetState(1);
        TaskSetEntry(KrackoJrEnterState, gCurTaskIdx);
        return;
    }
    gUnk_02007D00[5] = TaskFindNearestPlayer();
    pt = &gTasks[gUnk_02007D00[5]];
    gUnk_02007D00[6] = pt->onGround;
    if ((u32)(gCurTask->unk28 - 8) > 2)
        sub_080a932c();
    if (!(gCurTask->unk34 & (128 << 8)))
    {
        if (gUnk_02007D00[6] != 0)
            gCurTask->unk34 = 0;
        else
        {
            gCurTask->unk34++;
            if (gCurTask->unk34 > 119)
                gCurTask->unk34 = 128 << 8;
        }
    }
    if (gCurTask->unk28 == 0)
        goto next;
    if (gCurTask->unk28 < 0)
        goto sw;
    if (gCurTask->unk28 > 2)
        goto sw;
    gCurTask->unk2C++;
    if (gCurTask->unk2C > 63)
    {
        gCurTask->unk2C = 0;
        r = RandomRange(4);
        if (r == 0)
        {
            gCurTask->velX = r;
            gCurTask->velY = r;
            gCurTask->unk28 = r;
        }
    }
next:
    w = gCurTask->unk30;
    if (w & (128 << 8))
    {
        if (sub_080a93ec() != 0)
            goto sw;
        goto retreat;
    }
    else
    {
        w++;
        gCurTask->unk30 = w;
        if (w > 31 && !(w & 15))
        {
            r = RandomRange(32);
            gUnk_02007D00[7] = r;
            if (gCurTask->unk30 == 128)
            {
                if (pt->onGround != 0)
                    goto retreat;
                if (r > 16)
                    goto retreat;
                gCurTask->unk30 = 32;
                goto sw;
            }
            else
            {
                if (pt->onGround == 0)
                {
                    if (r <= 0)
                        goto chk;
                    goto sw;
                }
                else if (r > 1)
                    goto sw;
chk:
                if (sub_080a93ec() != 0)
                    goto strong;
retreat:
                sub_080a9434(pt);
                goto sw;
strong:
                gCurTask->unk30 = 128 << 8;
                goto sw;
            }
        }
        goto sw;
    }
sw:
    switch (gCurTask->unk28)
    {
    case 0:
        gCurTask->unk2C++;
        if (gCurTask->unk2C > 32)
        {
            gCurTask->unk2C = 0;
            gUnk_02007D00[0] = 129;
            gCurTask->unk1C = 4;
            gCurTask->unk28 = 1;
            sub_080a94d4();
        }
        sub_080a96a4();
        break;
    case 1:
        if (sub_080a95dc() == 1)
        {
            gCurTask->unk28 = 2;
            gCurTask->unk1C = 4;
            sub_080a96dc();
            AngleToVector((s16)gUnk_02007D00[4], 320);
            gCurTask->velX = gUnk_030023B4;
            gCurTask->velY = gUnk_030023D4;
        }
        sub_080a96a4();
        break;
    case 2:
        sub_080a95dc();
        gCurTask->unk1C--;
        if (gCurTask->unk1C == 0)
        {
            gCurTask->unk1C = 4;
            sub_080a96dc();
            AngleToVector((s16)gUnk_02007D00[4], 320);
            gCurTask->velX = gUnk_030023B4;
            gCurTask->velY = gUnk_030023D4;
        }
        sub_080a96a4();
        break;
    case 3:
        if (pt->onGround != 0)
        {
            if (RandomRange(8) <= 2)
                goto retreat2;
            gCurTask->velX = 0;
            gCurTask->velY = 0;
            gCurTask->unk34 = 0;
            gCurTask->unk30 = 0;
            gCurTask->unk2C = 0;
            gCurTask->unk28 = 0;
        }
        else
        {
            gCurTask->unk24--;
            if (gCurTask->unk24 == 0)
            {
                gCurTask->unk24 = 4;
                TaskAccelerateTowardNearestPlayer(192 << 6, 192 << 9);
            }
        }
        break;
    case 4:
        gCurTask->unk30--;
        if (gCurTask->unk30 == 0)
        {
            gCurTask->unk28 = 5;
            gUnk_02007D00[1] = 128 << 3;
            gUnk_02007D00[2] = -1;
        }
        break;
    case 5:
        if (gUnk_02007D00[2] != -1)
        {
            gUnk_02007D00[2]--;
            if (gUnk_02007D00[2] == 0)
            {
                u = gCurTask;
                pp.x0 = u->pixelX - 32;
                pp.y0 = u->pixelY - 32;
                pp.x1 = u->pixelX + 32;
                pp.y1 = u->pixelY + 32;
                if (TaskIsInRectSlot((struct Rect *)&pp, gUnk_02007D00[5]) == 0)
                {
                    gUnk_02007D00[2]++;
                    {
                        struct Rect *rc = (struct Rect *)&pp;

                        rc->left = gCurTask->pixelX - 64;
                        rc->top = gCurTask->pixelY - 64;
                        rc->right = gCurTask->pixelX + 64;
                        rc->bottom = gCurTask->pixelY + 64;
                    }
                    if (TaskIsInRectSlot((struct Rect *)&pp, gUnk_02007D00[5]) == 0)
                        gUnk_02007D00[2]++;
                }
                gUnk_02007D00[2] = gUnk_087490A4[gUnk_02007D00[2]];
                gUnk_02007D00[4] = (gUnk_02007D00[4] + 256) & 511;
                gCurTask->unk28 = 6;
            }
        }
        else
        {
            AngleToVector((s16)gUnk_02007D00[4], (s16)gUnk_02007D00[1]);
            gCurTask->velX = gUnk_030023B4;
            gCurTask->velY = gUnk_030023D4;
            gUnk_02007D00[1] -= 64;
            z5 = gUnk_02007D00[1];
            if (z5 == 0)
            {
                gCurTask->velX = z5;
                gCurTask->velY = z5;
                gUnk_02007D00[2] = 16;
            }
        }
        break;
    case 6:
        gUnk_02007D00[2]--;
        if (gUnk_02007D00[2] != 0)
        {
            gUnk_02007D00[1] += 24;
            if (gUnk_02007D00[1] > 768)
                gUnk_02007D00[1] = 768;
            AngleToVector((s16)gUnk_02007D00[4], (s16)gUnk_02007D00[1]);
            gCurTask->velX = gUnk_030023B4;
            gCurTask->velY = gUnk_030023D4;
        }
        else
        {
            gUnk_02007D00[2] = -1;
            gCurTask->unk1C = 4;
            gCurTask->unk28 = gUnk_02007D00[3] + 7;
        }
        break;
    case 7:
        if (gUnk_02007D00[2] != -1)
        {
            if (gUnk_02007D00[2] == 24)
                gUnk_02007D00[0] = 130;
            else if (gUnk_02007D00[2] == 16)
                gUnk_02007D00[0] = 129;
            gUnk_02007D00[2]--;
            if (gUnk_02007D00[2] == 0)
            {
                sub_080a96a4();
                v = gCurTask->unk34 & (128 << 8);
                if (v == 0)
                {
                    if ((u32)RandomRange(16) <= 2)
                    {
retreat2:
                        sub_080a9434(pt);
                    }
                    else
                    {
                        gCurTask->unk30 = v;
                        gCurTask->unk2C = v;
                        gCurTask->unk28 = v;
                    }
                }
            }
        }
        else
        {
            gUnk_02007D00[1] -= 30;
            if (gUnk_02007D00[1] < 0)
            {
                gCurTask->velX = 0;
                gCurTask->velY = 0;
                gUnk_02007D00[2] = 32;
            }
            else
            {
                gCurTask->unk1C--;
                if (gCurTask->unk1C == 0)
                {
                    gCurTask->unk1C = 4;
                    sub_080a96dc();
                    AngleToVector((s16)gUnk_02007D00[4], (s16)gUnk_02007D00[1]);
                    gCurTask->velX = gUnk_030023B4;
                    gCurTask->velY = gUnk_030023D4;
                }
            }
        }
        break;
    case 8:
        d = gCurTask->pixelX - gCurTask->unk18;
        if (d >= 0 ? d <= 2 : gCurTask->unk18 - gCurTask->pixelX <= 2)
        {
            d = gCurTask->pixelY - gCurTask->unk20;
            if (d >= 0 ? d <= 2 : gCurTask->unk20 - gCurTask->pixelY <= 2)
            {
                gCurTask->velX = 0;
                gCurTask->velY = 0;
                gCurTask->unk28 = 10;
                break;
            }
        }
        va = ArcTan2((s16)(gCurTask->unk18 - gCurTask->pixelX), (s16)(gCurTask->unk20 - gCurTask->pixelY));
        gUnk_02007D00[4] = (va <<= 16, va = (u32)va >> 23, va += 32, va &= 511, va >>= 6);
        TaskAccelerateInDir(192 << 6, 192 << 9, va);
        break;
    case 9:
        sub_080a96a4();
        n = gCurTask->unk34 & (128 << 8);
        if (n == 0)
        {
            gCurTask->unk30 = n;
            gCurTask->unk2C = n;
            gCurTask->unk28 = n;
        }
        break;
    case 10:
        break;
    }
}

void sub_080a85e4(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    t = gCurTask;
    t->velY = 0;
    t->accelY = -0x2000;
    t->unk28 = 0;
    TaskSleepForever();
}
