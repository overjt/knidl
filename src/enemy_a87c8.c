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

void sub_080a87c8(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)sub_080a8850;
    t->unk28 = 0;
    t->unk2C = 0;
    t->unk34 = 0;
    t->unk18 = 0;
    gUnk_02007D00[2] = 0;
    TaskGetScreenPosSlot(gCurTaskIdx);
    if (gUnk_030023B4 <= 127)
        gCurTask->unk30 = 0;
    else
        gCurTask->unk30 = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 7, gUnk_08749168);
}

void sub_080a8834(void)
{
    CallTableEntry(gCurTask->state, 7, gUnk_08749168);
}

void sub_080a8850(void)
{
    sub_080a9738();
    CallTableEntry(gCurTask->updateState, 7, gUnk_08749184);
    sub_08068f68();
    ActorReactToHit();
}

void sub_080a8878(void)
{
    struct Task *t;

    gCurTask->updateState = 0;
    t = gCurTask;
    t->posY = (gViewRect[2] - 16) << 16;
    t->velY = 128 << 8;
    TaskYieldTrampoline(8);
    sub_08066544();
    gUnk_02007D00[0] = 0;
    TaskYieldTrampoline(76);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(3);
        gCurTask->velY = 0;
        TaskYieldTrampoline(1);
        gCurTask->velY = 128 << 8;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    gCurTask->velY = 0;
    gCurTask->posY = (gViewRect[2] + 48) << 16;
    gUnk_02007D00[0] = 0;
    while (gHudHpBarFilled == 0)
        TaskYieldTrampoline(1);
    sub_08066564();
    ActorSetAttackBox((u32)gUnk_08749720);
    sub_08063a00((u32)gUnk_08749758);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a8948(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
}

void sub_080a8970(void)
{
    s32 idx;

    gCurTask->updateState = 1;
    for (;;)
    {
        switch (gCurTask->unk2C)
        {
        case 0:
            if (gCurTask->unk18 == 0)
            {
                gCurTask->unk18 = 4;
                ActorSetState(4);
            }
            else
            {
                gCurTask->unk18 = 5;
                ActorSetState(5);
                gCurTask->unk2C = 3;
            }
            break;
        case 1:
            if (gCurTask->unk28 & 128)
            {
                gCurTask->unk28 = 0;
                goto pick;
            }
            if ((RandomRange(2) != 0 || gCurTask->unk28 > 4)
                && gUnk_02007D00[2] <= 1)
            {
                gCurTask->unk28 = 128;
                gCurTask->unk2C = 3;
                ActorSetState(6);
                break;
            }
pick:
            gCurTask->unk28++;
            idx = TaskFindNearestPlayer();
            gUnk_03001F2C = idx;
            gUnk_03002448 = gPlayerStates[idx].mode;
            TaskGetPosSlot(idx);
            if (gUnk_03002448 == 14)
            {
                if (gUnk_030023D4 <= 95)
                {
                    ActorSetState(5);
                    goto st3;
                }
                if (gCurTask->unk30 == 0)
                {
                    if (gUnk_030023B4 <= 71)
                    {
                        gUnk_03002344 = 0;
                        goto sel;
                    }
                    gUnk_03002344 = 1;
                }
                else if (gUnk_030023B4 > 168)
                    gUnk_03002344 = 0;
                else
                    gUnk_03002344 = 1;
            }
            else if (gUnk_030023B4 <= 71)
                gUnk_03002344 = gUnk_087490B0[gCurTask->unk30];
            else if (gUnk_030023B4 > 168)
                gUnk_03002344 = gUnk_087490B2[gCurTask->unk30];
            else
                gUnk_03002344 = 2;
sel:
            gUnk_03002160 = gUnk_087490B4[gUnk_03002344][RandomRange(8)];
            if (gUnk_03002160 == 4 && gCurTask->unk18 == 4)
                gUnk_03002160 = gUnk_087490DC[RandomRange(2)];
            ActorSetState((u16)gUnk_03002160);
            gCurTask->unk18 = gUnk_03002160;
st3:
            gCurTask->unk2C = 3;
            break;
        case 2:
            ActorSetState(2);
            gCurTask->posX = (gUnk_087490E4[gCurTask->unk30] + gViewRect[0]) << 16;
            gCurTask->posY = (gViewRect[2] + 48) << 16;
            gCurTask->unk2C = 3;
            break;
        }
        TaskYieldTrampoline(1);
    }
}

void sub_080a8b44(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
}

void sub_080a8b6c(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = gUnk_087490E8[(s16)gCurTask->unk6C];
        TaskYieldTrampoline(8);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    gCurTask->unk2C = 1;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a8bcc(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
}

void sub_080a8bf4(void)
{
    gCurTask->updateState = 3;
    TaskStop();
    TaskGetScreenPosSlot(gCurTaskIdx);
    if (gUnk_030023B4 <= 127)
    {
        gCurTask->unk30 = 0;
        gCurTask->velX = 160 << 9;
    }
    else
    {
        gCurTask->unk30 = 1;
        gCurTask->velX = -0x14000;
    }
    gCurTask->unk24 = 0;
    for (;;)
    {
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velY = gUnk_087490E8[(s16)gCurTask->unk6C];
            TaskYieldTrampoline(6);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 5);
    }
}

void sub_080a8c84(void)
{
    struct Task *t;

    TaskGetScreenPosSlot(gCurTaskIdx);
    t = gCurTask;
    if (t->unk30 == 0)
    {
        if (gUnk_030023B4 > 167)
        {
            t->posX = (gViewRect[0] + 168) << 16;
            t->unk24++;
        }
    }
    else if (gUnk_030023B4 <= 72)
    {
        t->posX = (gViewRect[0] + 72) << 16;
        t->unk24++;
    }
    if (gCurTask->unk24 != 0)
    {
        struct Task *u = gCurTask;

        u->velX = 0;
        u->velY = 0;
        u->unk2C = 2;
        u->unk30 ^= 1;
        ActorSetState(1);
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
    }
}

void sub_080a8d1c(void)
{
    gCurTask->updateState = 4;
    gUnk_02007D00[0] = 1;
    TaskStop();
    StartBgPaletteBlend(8, 128);
    if (gCurTask->unk30 == 0)
        gCurTask->facing = 1;
    else
        gCurTask->facing = 255;
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(16);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(12);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(0, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(15);
    TaskYieldTrampoline(1);
    gCurTask->unk24 = 1;
    TaskSetMotionXFacing(160 << 9, 0x5A5A5A5A);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 8; gCurTask->unk6C++)
    {
        for (gCurTask->unk6E = 0; (s16)gCurTask->unk6E <= 3; gCurTask->unk6E++)
        {
            CreateChildTaskAtOffsetFacing(197, 0, 32, 1);
            gCurTask->velY = gUnk_08749100[(s16)gCurTask->unk6E];
            for (gCurTask->unk70 = 0; (s16)gCurTask->unk70 <= 3; gCurTask->unk70++)
            {
                if (--gCurTask->unk24 == 0)
                {
                    PlaySfx(138 << 2);
                    gCurTask->unk24 = 10;
                }
                TaskYieldTrampoline(1);
            }
        }
    }
    CreateChildTaskAtOffsetFacing(197, 0, 32, 1);
    gCurTask->velY = 0;
    TaskYieldTrampoline(6);
    EndBgPaletteBlend(8);
    TaskSetMotionXFacing(144 << 9, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(12);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(16);
    gCurTask->velX = 0;
    gCurTask->velY = 0;
    gUnk_02007D00[0] = 0;
    gCurTask->unk30 ^= 1;
    if (gCurTask->unk2C != 0)
        gCurTask->unk2C = 2;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a8f18(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
}

void sub_080a8f40(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 6;
    TaskStop();
    gUnk_02007D00[0] = 1;
    TaskYieldTrampoline(36);
    sp.subtype = 25;
    sp.taskType = 128;
    sp.variant = 0;
    sp.spawnArg = 0;
    sp.x = 0;
    sp.y = 16;
    sp.checkTerrain = 1;
    CreateActorFromDescAtOffsetFacing(&sp, 0);
    PlaySfx(506);
    gUnk_02007D00[0] = 0;
    gCurTask->unk2C = 2;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a8fb4(void)
{
    if (gCurTask->state != 6)
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
}

void sub_080a8fdc(void)
{
    gCurTask->updateState = 5;
    gUnk_02007D00[0] = 1;
    if (gCurTask->unk30 == 0)
        gCurTask->facing = 1;
    else
        gCurTask->facing = 255;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = -0x2000;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(0, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(10);
    PlaySfx(0x229);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    gCurTask->velY = 192 << 10;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = 192 << 10;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(192 << 11, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(10);
    TaskSetMotionXFacing(128 << 11, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->velY = -0x60000;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = -0x40000;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x40000, 0x5A5A5A5A);
    gCurTask->velY = -0x10000;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
    gCurTask->velY = -0x8000;
    TaskYieldTrampoline(8);
    TaskSetMotionXFacing(-0x20000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x10000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(-0x8000, 0x5A5A5A5A);
    gCurTask->velY = 128 << 10;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = 128 << 9;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->velY = 128 << 8;
    TaskYieldTrampoline(4);
    TaskSetMotionXFacing(128 << 6, 0x5A5A5A5A);
    gCurTask->velY = 0;
    TaskYieldTrampoline(6);
    gUnk_02007D00[0] = 0;
    if (gCurTask->unk2C != 0)
        gCurTask->unk2C = 2;
    gCurTask->unk30 ^= 1;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a9304(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(sub_080a8834, gCurTaskIdx);
}
