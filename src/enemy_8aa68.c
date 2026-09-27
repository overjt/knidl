/* game_code_and_rodata 0x0808AA68-0x0808CCE8 (issue #80, module M23 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0808AA68 0x0808CCE8 src/enemy_8aa68.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct Task *gCurTask;
extern struct Task gTasks[];

/* ROM tables */
extern s32 gUnk_087428E8[];
extern s32 gUnk_08742930[];
extern s8 gUnk_08732FF0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_087337F0[];
extern s8 gUnk_087339F0[];
extern struct AnimCmd gUnk_08742894[];
extern struct AnimCmd gUnk_087428A8[];
extern u32 gCollisionTilePushRight[];
extern u32 gCollisionTilePushLeft[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_087428C0[];
extern u32 gUnk_087428D8[];
extern u32 gUnk_087428E0[];
extern u32 gUnk_08742908[];
extern u32 gUnk_0874290C[];
extern u32 gUnk_08742910[];
extern u32 gUnk_08742918[];
extern u32 gUnk_08742920[];
extern u32 gUnk_08742928[];
extern u32 gUnk_08742940[];
extern u32 gUnk_08742948[];
extern u32 gUnk_08742978[];
extern u32 gUnk_087522B4[];
extern u32 gUnk_08752858[];
extern u32 gUnk_087528C8[];
extern u32 gUnk_08752BD4[];
extern u8 *gUnk_08742998[];
extern u8 gCollisionTileSlope[];
extern u8 gUnk_08742938[];

/* Externals */
extern s32 Div(s32 numerator, s32 denominator);
extern s32 GetCollisionTileAtPixel(u16 x, u16 y);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetNearestPlayerDy(void);
extern s32 TaskGetFacingTowardNearestPlayer(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorStepAnim(void);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s8 TaskGetParentFacing(void);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern u32 ActorReactToHit(void);
extern u8 IsWaterAtPixel(s16 x, s16 y);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaySfx(s32 id);
extern void TaskMove(void);
extern void TaskDrawWorld(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *fn, u32 i);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStop(void);
extern void TaskUpdateFlip(void);
extern void TaskSetFrame(s32 a);
extern void TaskInitWaterFlags(void);
extern void ActorSetState(u8 v);
extern void ActorSetAttackBox(u32 v);
extern void TaskFaceNearestPlayer(void);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskAccelerateInDir(s32 step, s32 limit, u16 dir);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void ActorMove(void);
extern void sub_0806a0f0(s32 a);
extern void ActorDie(void);
void sub_0808a84c(u8 *p, s32 b);

/* Defined below */
void sub_0808ab38(void);
void sub_0808ae48(void);
void sub_0808afd0(void);
void sub_0808b234(void);
void sub_0808b4d0(void);
void sub_0808b5b4(void);
void sub_0808b650(void);
void sub_0808b704(void);
void sub_0808b79c(void);
void sub_0808b830(void);
void sub_0808b8c4(void);
void sub_0808b99c(s32 a);
void sub_0808bb24(void);
void sub_0808bb5c(void);
void sub_0808be58(void);
void sub_0808c708(void);
s32 sub_0808c82c(void);
void sub_0808c8bc(void);
void sub_0808c934(void);
void sub_0808c980(void);
s32 sub_0808ca00(s32 a, s32 b);
s32 sub_0808cab8(s32 a, s32 b);
void sub_0808cc14(void);

void sub_0808aa68(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08752858;
    gCurTask->onGround = 0;
    if (gCurTask->unk73 == 5)
        sub_0808bb24();
    TaskInitWaterFlags();
    if (gCurTask->waterFlags == 3)
        CallTableEntry(gCurTask->unk73, 6, gUnk_087428C0);
    sub_0808b5b4();
}

void sub_0808aad8(void)
{
    gCurTask->updateCallback = (u32)sub_0808ab38;
    gCurTask->onGround = 0;
    gCurTask->unk34 = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_087428D8);
}

void sub_0808ab14(void)
{
    gCurTask->updateCallback = (u32)sub_0808ab38;
    CallTableEntry(gCurTask->state, 2, gUnk_087428D8);
}

void sub_0808ab38(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_087428E0);
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void sub_0808ab70(void)
{
    gCurTask->updateState = 0;
    while (gCurTask->unk34 <= 0x6FF)
    {
        if (gCurTask->unk28 == 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(8);
            TaskYieldTrampoline(4);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
            TaskSetFrame(4);
            TaskYieldTrampoline(4);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
        }
    }
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0808abf4(void)
{
    if (gCurTask->state != 0)
    {
        TaskSetEntry(sub_0808ab14, gCurTaskIdx);
        return;
    }
    gCurTask->unk34++;
    if ((gCurTask->unk34 & 7) == 0)
    {
        u8 c = TaskGetAngleToNearestPlayer(1) + 1;
        u16 d = (c & 0xF) >> 1;

        TaskAccelerateInDir(gUnk_087428E8[gCurTask->unk74],
                     gUnk_087428E8[gCurTask->unk74 + 4], d);
        if (gUnk_030023B4 > 0)
            gCurTask->facing = 1;
        if (gUnk_030023B4 < 0)
            gCurTask->facing = -1;
        gCurTask->unk28 = gUnk_030023B4 | gUnk_030023D4;
    }
    if (IsWaterAtPixel(gCurTask->pixelX, (u16)gCurTask->pixelY - 8) != 1)
    {
        if (gCurTask->velY < 0)
            gCurTask->velY = -gCurTask->velY;
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
    }
}

void sub_0808acdc(void)
{
    gCurTask->updateState = 1;
    while (1)
    {
        if (gCurTask->unk28 == 0)
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
        }
        else
        {
            TaskSetFrame(6);
            TaskYieldTrampoline(6);
            TaskSetFrame(8);
            TaskYieldTrampoline(6);
        }
    }
}

void sub_0808ad20(void)
{
    gCurTask->unk34++;
    if ((gCurTask->unk34 & 7) == 0)
    {
        u8 c = TaskGetAngleToNearestPlayer(1) + 9;
        u16 d = (c & 0xF) >> 1;

        TaskAccelerateInDir(gUnk_087428E8[gCurTask->unk74],
                     gUnk_087428E8[gCurTask->unk74 + 4], d);
        if (gCurTask->velX > 0)
            gCurTask->facing = 1;
        if (gCurTask->velX < 0)
            gCurTask->facing = -1;
        gCurTask->unk28 = gUnk_030023B4 | gUnk_030023D4;
    }
    if (IsWaterAtPixel(gCurTask->pixelX, (u16)gCurTask->pixelY - 8) != 1)
    {
        if (gCurTask->velY < 0)
            gCurTask->velY = -gCurTask->velY;
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
    }
}

void sub_0808adec(void)
{
    gCurTask->updateCallback = (u32)sub_0808ae48;
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gUnk_08742908);
}

void sub_0808ae24(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)sub_0808ae48;
    CallTableEntry(t->state, 1, gUnk_08742908);
}

void sub_0808ae48(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 1, gUnk_0874290C);
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void sub_0808ae80(void)
{
    gCurTask->updateState = 0;
    TaskStop();
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->unk28 = ActorStartAnim(gUnk_08742894);
    while (1)
    {
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0x4000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFF8000;
        TaskYieldTrampoline(10);
        gCurTask->velY = 0xFFFFC000;
        TaskYieldTrampoline(10);
    }
}

void sub_0808aeec(void)
{
    if (gCurTask->unk8C->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
    if (gCurTask->unk73 == 3 || gCurTask->unk73 == 4)
        sub_0808b4d0();
}

void sub_0808af34(void)
{
    s16 *p;

    gCurTask->updateCallback = (u32)sub_0808afd0;
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    gCurTask->unk34 = gCurTask->facing;
    p = &gCurTask->pixelY;
    if (*p < gTasks[TaskFindNearestPlayer()].pixelY)
        ActorSetState(1);
    else
        ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08742910);
}

void sub_0808afac(void)
{
    gCurTask->updateCallback = (u32)sub_0808afd0;
    CallTableEntry(gCurTask->state, 2, gUnk_08742910);
}

void sub_0808afd0(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 2, gUnk_08742918);
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void sub_0808b008(void)
{
    gCurTask->updateState = 0;
    gCurTask->facing = gCurTask->unk34;
    gCurTask->velY = 0xFFFF8000;
    gCurTask->unk28 = ActorStartAnim(gUnk_087428A8);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velX = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0x4000;
        TaskYieldTrampoline(15);
        gCurTask->velX = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->velX = 0xFFFFC000;
        TaskYieldTrampoline(15);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0808b094(void)
{
    if (gCurTask->state == 0)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX,
                             (u16)gCurTask->pixelY - 8) != 0)
            goto anim;
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
        ActorSetState(1);
    }
    TaskSetEntry(sub_0808afac, gCurTaskIdx);
    return;
anim:
    if (gCurTask->unk8C->animScript != 0)
    {
        if (gCurTask->unk28 == 0)
            gCurTask->unk28 = ActorStepAnim();
        gCurTask->unk28--;
    }
}

void sub_0808b120(void)
{
    gCurTask->updateState = 1;
    gCurTask->facing = gCurTask->unk34;
    gCurTask->velX = 0;
    gCurTask->velY = 0x8000;
    TaskSetFrame(12);
    TaskYieldTrampoline(6);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(19);
    TaskYieldTrampoline(119);
    TaskStop();
    TaskSetFrame(13);
    TaskYieldTrampoline(6);
    ActorSetState(0);
    TaskSleepForever();
}

void sub_0808b1a8(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(sub_0808afac, gCurTaskIdx);
}

void sub_0808b1d0(void)
{
    gCurTask->updateCallback = (u32)sub_0808b234;
    gCurTask->unk2C = 1;
    gCurTask->onGround = 0;
    TaskFaceNearestPlayer();
    gCurTask->unk34 = 1;
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08742920);
}

void sub_0808b210(void)
{
    gCurTask->updateCallback = (u32)sub_0808b234;
    CallTableEntry(gCurTask->state, 2, gUnk_08742920);
}

void sub_0808b234(void)
{
    if (gCurTask->unk2C != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 2, gUnk_08742928);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 2, gUnk_08742928);
    }
    ActorCheckHits();
    ActorReactToHit();
    gCurTask->onGround = 0;
}

void sub_0808b28c(void)
{
    gCurTask->updateState = 0;
    gCurTask->unk30 = 192;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->accelY = 0xFFFFCD00;
    gCurTask->speedLimitY = 0x30000;
    while (1)
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(6);
        TaskSetFrame(10);
        TaskYieldTrampoline(5);
        TaskSetFrame(8);
        TaskYieldTrampoline(5);
        TaskSetFrame(6);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(10);
    }
}

void sub_0808b2fc(void)
{
    if (gCurTask->velY < 0
        && (u8)IsWaterAtPixel(gCurTask->pixelX,
                            (u16)gCurTask->pixelY - 8) == 0)
    {
        gCurTask->pixelY = (gCurTask->pixelY & -16) + 8;
        gCurTask->posY = (s16)gCurTask->pixelY << 16;
        ActorSetState(0);
        TaskSetEntry(sub_0808ae24, gCurTaskIdx);
    }
    else
    {
        sub_0808b4d0();
    }
}

void sub_0808b368(void)
{
    gCurTask->updateState = 1;
    if (gCurTask->unk73 == 3)
        gCurTask->unk2C = 0;
    else
        gCurTask->unk2C = 1;
    TaskFaceNearestPlayer();
    gCurTask->velX = 0;
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(2);
    gCurTask->unk34 = 90;
    TaskSetMotionXFacing(gUnk_08742930[gCurTask->unk74], 0x5A5A5A5A);
    TaskSetMotionY(0xFFFC0000, 0x1500, 0x30000);
    while (gCurTask->velY < 0)
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(3);
        TaskSetFrame(10);
        TaskYieldTrampoline(3);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
    }
    TaskSetFrame(10);
    TaskYieldTrampoline(3);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(22);
    TaskSleepForever();
}

void sub_0808b468(void)
{
    if (gCurTask->unk34 > 0 && gCurTask->velY > 0
        && (u8)IsWaterAtPixel(gCurTask->pixelX,
                            (u16)gCurTask->pixelY
                                + ((s8 *)gCurTask->unk8C->terrainBox)[2]) != 0)
    {
        gCurTask->velY >>= 2;
        ActorSetState(0);
        TaskSetEntry(sub_0808b210, gCurTaskIdx);
    }
}

void sub_0808b4d0(void)
{
    if (--gCurTask->unk30 <= 0)
    {
        gCurTask->unk30 = 192;
        TaskTurnAroundAndReverseX();
    }
    if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
    {
        ActorSetState(1);
        TaskSetEntry(sub_0808b210, gCurTaskIdx);
        return;
    }
    if (abs(TaskGetNearestPlayerDx()) > 64)
        return;
    if (--gCurTask->unk34 > 0)
        return;
    if (RandomRange(3) == 0)
    {
        gCurTask->unk34 = 16;
        return;
    }
    if (TaskGetNearestPlayerDx() < 0)
    {
        if (-TaskGetNearestPlayerDx() <= 32)
            goto one;
        goto zero;
    }
    if (TaskGetNearestPlayerDx() > 32)
        goto zero;
one:
    gCurTask->unk74 = 1;
    goto done;
zero:
    gCurTask->unk74 = 0;
done:
    ActorSetState(1);
    TaskSetEntry(sub_0808b210, gCurTaskIdx);
}

void sub_0808b5b4(void)
{
    gCurTask->updateCallback = (u32)sub_0808b8c4;
    TaskStop();
    if (gCurTask->onGround == 0)
    {
        TaskFaceNearestPlayer();
        TaskSetFrame(20);
        TaskSetMotionY(0, 0x2500, 0x30000);
        TaskSleepForever();
    }
    if (RandomRange(8) == 0)
        gCurTask->facing = -gCurTask->facing;
    switch (gUnk_08742938[RandomRange(8)])
    {
    case 0:
        sub_0808b650();
        break;
    case 1:
        sub_0808b704();
        break;
    case 2:
        sub_0808b79c();
        break;
    case 3:
        sub_0808b830();
        break;
    }
}

void sub_0808b650(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFC0000, 0x4000, 0x30000);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(21);
    TaskYieldTrampoline(3);
    sub_0808b99c(2);
    TaskSetFrame(22);
    TaskYieldTrampoline(3);
    TaskSetFrame(21);
    TaskYieldTrampoline(3);
    sub_0808b99c(3);
    TaskSetFrame(20);
    TaskYieldTrampoline(4);
    sub_0808b99c(2);
    TaskSetFrame(21);
    TaskYieldTrampoline(4);
    sub_0808b99c(3);
    TaskSetFrame(18);
    TaskYieldTrampoline(3);
    TaskSetFrame(8);
    TaskYieldTrampoline(3);
    TaskSetFrame(9);
    TaskYieldTrampoline(3);
    TaskSetFrame(19);
    TaskSleepForever();
}

void sub_0808b704(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionY(0xFFFE0000, 0x4000, 0x30000);
    sub_0808b99c(0);
    TaskSetFrame(22);
    TaskYieldTrampoline(4);
    sub_0808b99c(1);
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    sub_0808b99c(0);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(8);
    TaskYieldTrampoline(4);
    TaskSetFrame(19);
    TaskYieldTrampoline(4);
    TaskSetFrame(23);
    TaskSleepForever();
}

void sub_0808b79c(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE0000, 0x4000, 0x30000);
    TaskSetFrame(18);
    TaskYieldTrampoline(4);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(9);
    TaskSleepForever();
}

void sub_0808b830(void)
{
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFE0000, 0x4000, 0x30000);
    TaskSetFrame(9);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(17);
    TaskYieldTrampoline(4);
    TaskSetFrame(15);
    TaskYieldTrampoline(4);
    TaskSetFrame(4);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(16);
    TaskYieldTrampoline(4);
    TaskSetFrame(18);
    TaskSleepForever();
}

void sub_0808b8c4(void)
{
    if ((u8)IsWaterAtPixel(gCurTask->pixelX,
                         (u16)gCurTask->pixelY
                             + ((s8 *)gCurTask->unk8C->terrainBox)[2]) != 0)
    {
        switch (gCurTask->unk73)
        {
        case 0:
            TaskStop();
            TaskSetEntry(sub_0808aad8, gCurTaskIdx);
            break;
        case 1:
            TaskStop();
            TaskSetEntry(sub_0808adec, gCurTaskIdx);
            break;
        case 2:
            TaskStop();
            TaskSetEntry(sub_0808af34, gCurTaskIdx);
            break;
        case 3:
        case 4:
            gCurTask->velY >>= 2;
            gCurTask->unk34 = 90;
            ActorSetState(0);
            gCurTask->updateCallback = (u32)sub_0808b234;
            TaskSetEntry(sub_0808b28c, gCurTaskIdx);
            break;
        }
    }
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808b99c(s32 a)
{
    gCurTask->unk46 = CreateChildTaskHere(218, 1);
    gTasks[gCurTask->unk46].unk73 = a;
}

void sub_0808b9d0(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 12;
    gCurTask->frameTable = gUnk_087528C8;
    gCurTask->facing = TaskGetParentFacing();
    t = gCurTask;
    switch (t->unk73)
    {
    case 0:
        t->posX = (t->pixelX - t->facing * 12) << 16;
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFE0000;
        gCurTask->accelY = 0x8000;
        break;
    case 1:
        t->posX = (t->pixelX + t->facing * 12) << 16;
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFE0000;
        gCurTask->accelY = 0x8000;
        break;
    case 2:
        t->posX = (t->pixelX - t->facing * 8) << 16;
        t->posY = (t->pixelY - 12) << 16;
        TaskSetMotionXFacing(0xFFFE0000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFD8000;
        gCurTask->accelY = 0x8000;
        break;
    case 3:
        t->posX = (t->pixelX + t->facing * 8) << 16;
        t->posY = (t->pixelY - 12) << 16;
        TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
        gCurTask->velY = 0xFFFD8000;
        gCurTask->accelY = 0x8000;
        break;
    }
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void sub_0808bb24(void)
{
    gCurTask->updateCallback = (u32)sub_0808bb5c;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    TaskSetFrame(4);
    TaskSleepForever();
}

void sub_0808bb5c(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_0808bb70(void)
{
    if (gCurTask->unk73 != 5)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
        {
            TaskSetEntry(sub_0808b5b4, gCurTaskIdx);
            return 1;
        }
        switch (gCurTask->unk73)
        {
        case 0:
            gCurTask->velY = -gCurTask->velY;
        case 1:
            return 0;
        case 2:
            ActorSetState(0);
            TaskSetEntry(sub_0808afac, gCurTaskIdx);
            return 1;
        case 3:
        case 4:
            gCurTask->velY = 0;
            return 0;
        }
    }
}

s32 sub_0808bc18(void)
{
    if (gCurTask->unk73 != 5)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
        {
            TaskSetEntry(sub_0808b5b4, gCurTaskIdx);
            return 1;
        }
    }
}

s32 sub_0808bc60(void)
{
    if (gCurTask->unk73 != 5)
    {
        if ((u8)IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 0)
            goto stop;
        switch (gCurTask->unk73)
        {
        case 0:
            gCurTask->velX = -gCurTask->velX;
        case 2:
        zero:
            return 0;
        case 3:
            gCurTask->unk30 = 192;
        case 1:
        stop:
            TaskTurnAroundAndReverseX();
            goto zero;
        case 4:
            if (gCurTask->state == 0)
                gCurTask->unk30 = 192;
            TaskTurnAroundAndReverseX();
            return 0;
        }
    }
}

s32 sub_0808bd04(void)
{
    u8 r;

    if (gCurTask->unk73 != 5)
    {
        r = IsWaterAtPixel(gCurTask->pixelX, gCurTask->pixelY);
        if (r == 0)
        {
            gCurTask->velY = r;
            return 0;
        }
        switch (gCurTask->unk73)
        {
        case 0:
            gCurTask->velY = -gCurTask->velY;
        case 1:
            return 0;
        case 2:
            ActorSetState(1);
            TaskSetEntry(sub_0808afac, gCurTaskIdx);
            return 1;
        case 3:
        case 4:
            TaskSetEntry(sub_0808adec, gCurTaskIdx);
            return 1;
        }
    }
}

void sub_0808bdb4(void)
{
    gCurTask->moveCallback = (u32)ActorMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 11;
    gCurTask->frameTable = gUnk_08752BD4;
    CallTableEntry(gCurTask->unk73, 2, gUnk_08742940);
}

void sub_0808bdf4(void)
{
    gCurTask->updateCallback = (u32)sub_0808be58;
    gCurTask->unk28 = 120;
    if (sub_0808c82c() != 0)
        ActorSetState(3);
    else
        ActorSetState(1);
    CallTableEntry(gCurTask->state, 12, gUnk_08742948);
}

void sub_0808be3c(void)
{
    CallTableEntry(gCurTask->state, 12, gUnk_08742948);
}

void sub_0808be58(void)
{
    if (gCurTask->unk18 != 0)
    {
        if ((u8)ActorCollideTerrain() == 0)
            CallTableEntry(gCurTask->updateState, 8, gUnk_08742978);
    }
    else
    {
        CallTableEntry(gCurTask->updateState, 8, gUnk_08742978);
    }
    ActorCheckHits();
    ActorReactToHit();
}

void sub_0808bea0(void)
{
    gCurTask->updateState = 0;
    gCurTask->unk18 = 1;
    gCurTask->onGround = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    if (gCurTask->state == 2)
        gCurTask->facing = -gCurTask->facing;
    ActorSetState(0);
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    gCurTask->unk2C = 16;
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
    }
}

void sub_0808bf1c(void)
{
    if (--gCurTask->unk28 == 0)
    {
        ActorSetState(11);
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
        return;
    }
    if (--gCurTask->unk2C != 0)
        return;
    if (TaskGetNearestPlayerDx() < 0)
    {
        if (-TaskGetNearestPlayerDx() <= 63)
            goto near;
        goto far;
    }
    if (TaskGetNearestPlayerDx() > 63)
        goto far;
near:
    TaskFaceNearestPlayer();
    gCurTask->facing = -gCurTask->facing;
    TaskUpdateFlip();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    goto done;
far:
    TaskFaceNearestPlayer();
    TaskUpdateFlip();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
done:
    gCurTask->unk2C = 16;
}

void sub_0808bfc4(void)
{
    gCurTask->updateState = 1;
    gCurTask->unk18 = 1;
    gCurTask->onGround = 0;
    TaskStop();
    do
    {
        TaskSetFrame(8);
        TaskYieldTrampoline(29);
        sub_0808c8bc();
    } while (gCurTask->state == 3);
    TaskSleepForever();
}

void sub_0808c004(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c02c(void)
{
    gCurTask->updateState = 2;
    gCurTask->unk18 = 1;
    gCurTask->onGround = 0;
    TaskStop();
    do
    {
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velY = 0xFFFF0000;
            TaskSetFrame(8);
            TaskYieldTrampoline(3);
            TaskSetFrame(9);
            TaskYieldTrampoline(3);
            TaskSetFrame(10);
            TaskYieldTrampoline(4);
            gCurTask->velY = 0xFFFF8000;
            TaskSetFrame(11);
            TaskYieldTrampoline(3);
            TaskSetFrame(12);
            TaskYieldTrampoline(3);
            gCurTask->velY = 0xFFFFC000;
            TaskSetFrame(11);
            TaskYieldTrampoline(3);
            TaskSetFrame(10);
            TaskYieldTrampoline(3);
            TaskStop();
            TaskSetFrame(9);
            TaskYieldTrampoline(6);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        sub_0808c8bc();
    } while (gCurTask->state == 4);
    TaskSleepForever();
}

void sub_0808c0fc(void)
{
    ActorSetState(4);
    gCurTask->updateState = 2;
    gCurTask->unk18 = 1;
    gCurTask->onGround = 0;
    TaskStop();
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0xFFFE0000;
        TaskSetFrame(8);
        TaskYieldTrampoline(2);
        TaskSetFrame(9);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF0000;
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFF8000;
        TaskSetFrame(12);
        TaskYieldTrampoline(2);
        TaskSetFrame(11);
        TaskYieldTrampoline(2);
        gCurTask->velY = 0xFFFFC000;
        TaskSetFrame(10);
        TaskYieldTrampoline(2);
        TaskSetFrame(9);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_0808c1d0(void)
{
    if (gCurTask->state != 4)
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
    if (gCurTask->facing == 1)
    {
        if (sub_0808ca00(12, ((s8 *)gCurTask->unk8C->terrainBox)[3]) < 0)
        {
            ActorSetState(8);
            TaskSetEntry(sub_0808be3c, gCurTaskIdx);
        }
    }
    else if (sub_0808cab8(12, ((s8 *)gCurTask->unk8C->terrainBox)[3]) < 0)
    {
        ActorSetState(8);
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
    }
}

void sub_0808c260(void)
{
    gCurTask->updateState = 3;
    gCurTask->unk18 = 1;
    gCurTask->onGround = 0;
    TaskStop();
    do
    {
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->velY = 0x10000;
            TaskSetFrame(16);
            TaskYieldTrampoline(4);
            TaskSetFrame(13);
            TaskYieldTrampoline(6);
            gCurTask->velY = 0x8000;
            TaskSetFrame(16);
            TaskYieldTrampoline(4);
            TaskSetFrame(13);
            TaskYieldTrampoline(2);
            gCurTask->velY = 0x4000;
            TaskSetFrame(13);
            TaskYieldTrampoline(4);
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            TaskStop();
            TaskSetFrame(16);
            TaskYieldTrampoline(2);
            TaskSetFrame(13);
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        sub_0808c8bc();
    } while (gCurTask->state == 5);
    TaskSleepForever();
}

void sub_0808c32c(void)
{
    ActorSetState(5);
    gCurTask->updateState = 3;
    gCurTask->unk18 = 0;
    gCurTask->onGround = 0;
    TaskStop();
    gCurTask->facing = -gCurTask->facing;
    gCurTask->velY = 0x20000;
    TaskSetFrame(16);
    TaskYieldTrampoline(2);
    gCurTask->unk18 = 1;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->velY = 0x20000;
        TaskSetFrame(16);
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x10000;
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x8000;
        TaskSetFrame(16);
        TaskYieldTrampoline(4);
        gCurTask->velY = 0x4000;
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    ActorSetState(3);
    TaskSleepForever();
}

void sub_0808c3e8(void)
{
    if (gCurTask->state != 5)
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
    if (gCurTask->facing == 1)
    {
        if (sub_0808ca00(12, ((s8 *)gCurTask->unk8C->terrainBox)[3]) < 0)
        {
            ActorSetState(9);
            TaskSetEntry(sub_0808be3c, gCurTaskIdx);
        }
    }
    else if (sub_0808cab8(12, ((s8 *)gCurTask->unk8C->terrainBox)[3]) < 0)
    {
        ActorSetState(9);
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
    }
}

void sub_0808c478(void)
{
    gCurTask->updateState = 4;
    gCurTask->unk18 = 0;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
    TaskSetFrame(4);
    TaskYieldTrampoline(10);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0808c4bc(void)
{
    if (gCurTask->state != 8)
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c4e4(void)
{
    gCurTask->updateState = 5;
    gCurTask->unk18 = 1;
    gCurTask->onGround = 0;
    TaskStop();
    TaskSetFrame(4);
    gCurTask->facing = -gCurTask->facing;
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->accelY = 0x1500;
    gCurTask->speedLimitY = 0x30000;
    TaskSleepForever();
}

void sub_0808c538(void)
{
}

void sub_0808c53c(void)
{
    gCurTask->updateState = 6;
    gCurTask->unk18 = 0;
    gCurTask->onGround = 0;
    TaskStop();
    if (TaskGetFacingTowardNearestPlayer() == gCurTask->facing)
    {
        ActorSetState(3);
        TaskSleepForever();
    }
    TaskSetFrame(8);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetMotionXFacing(0xFFFF0000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        TaskSetMotionXFacing(0x10000, 0x5A5A5A5A);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk18 = 1;
    gCurTask->facing = -gCurTask->facing;
    sub_0808c980();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(4);
    }
}

void sub_0808c5e8(void)
{
    if (gCurTask->state != 10)
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c610(void)
{
    gCurTask->updateState = 7;
    gCurTask->unk18 = 0;
    gCurTask->onGround = 1;
    TaskStop();
    TaskFaceNearestPlayer();
    TaskSetFrame(14);
    TaskYieldTrampoline(16);
    PlaySfx(233);
    sub_0808c934();
    TaskSetFrame(15);
    TaskYieldTrampoline(12);
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    TaskSetFrame(7);
    TaskYieldTrampoline(8);
    gCurTask->unk28 = 120;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_0808c684(void)
{
    if (gCurTask->state != 11)
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
}

void sub_0808c6ac(void)
{
    gCurTask->updateCallback = (u32)sub_0808c708;
    ActorSetAttackBox((u32)gUnk_0873F500);
    gCurTask->health = 2;
    TaskFaceNearestPlayer();
    while (1)
    {
        TaskSetFrame(6);
        TaskYieldTrampoline(8);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(4);
        TaskYieldTrampoline(8);
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
    }
}

void sub_0808c708(void)
{
    ActorCollideTerrain();
    ActorCheckHits();
    ActorReactToHit();
}

s32 sub_0808c71c(void)
{
    if (gCurTask->unk73 != 1)
    {
        ActorSetState(0);
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808c74c(void)
{
    if (gCurTask->unk73 != 1)
    {
        ActorSetState(7);
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808c77c(void)
{
    if (gCurTask->unk73 != 1)
    {
        sub_0806a0f0(-2);
        return 1;
    }
}

s32 sub_0808c79c(void)
{
    if (gCurTask->unk73 != 1)
    {
        switch (gCurTask->state)
        {
        case 0:
            ActorSetState(6);
            TaskSetEntry(sub_0808be3c, gCurTaskIdx);
            return 1;
        case 9:
        case 10:
            ActorSetState(3);
            TaskSetEntry(sub_0808be3c, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_0808c7ec(void)
{
    if (gCurTask->unk73 != 1)
    {
        if (gCurTask->state != 4)
        {
            gCurTask->velY = 0;
            return 0;
        }
        ActorSetState(10);
        TaskSetEntry(sub_0808be3c, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808c82c(void)
{
    s32 r;

    r = sub_0808ca00(12, 0);
    if (r >= 0)
    {
        gCurTask->pixelX = (u16)gCurTask->pixelX
            + (r - ((s8 *)gCurTask->unk8C->terrainBox)[5]);
        gCurTask->posX = (s16)gCurTask->pixelX << 16;
        gCurTask->facing = 1;
        return 1;
    }
    r = sub_0808cab8(12, 0);
    if (r >= 0)
    {
        gCurTask->pixelX = (u16)gCurTask->pixelX
            - (((s8 *)gCurTask->unk8C->terrainBox)[4] + r);
        gCurTask->posX = (s16)gCurTask->pixelX << 16;
        gCurTask->facing = -1;
        return 1;
    }
    return 0;
}

void sub_0808c8bc(void)
{
    sub_0808a84c(gUnk_08742998[(u16)TaskGetAngleToNearestPlayer(1)], 100);
    switch (gUnk_030023D4)
    {
    case 0:
        ActorSetState(4);
        break;
    case 1:
        ActorSetState(5);
        break;
    case 2:
        ActorSetState(9);
        break;
    case 3:
        ActorSetState(10);
        break;
    case 4:
        ActorSetState(3);
        break;
    }
}

void sub_0808c934(void)
{
    struct ActorSpawn sp;
    u8 zero;

    sp.subtype = 34;
    sp.taskType = 137;
    sp.unk08 = gCurTask->unk73;
    sp.unk09 = gCurTask->unk74;
    zero = 0;
    sp.x = 6;
    sp.y = -6;
    sp.checkTerrain = zero;
    gCurTask->unk46 = CreateActorFromDescAtOffsetFacing(&sp, 0);
}

void sub_0808c980(void)
{
    s32 a;
    s32 d;

    TaskSetMotionXFacing(0x20000, 0x5A5A5A5A);
    a = abs(TaskGetNearestPlayerDx()) >> 1;
    d = -TaskGetNearestPlayerDy() + 32;
    if (d < 0)
        d = -d;
    if (a == 0)
        a = 1;
    d = Div(d << 16, a);
    if ((u8)TaskGetYDirBitToNearestPlayer() == 2)
        d = -d;
    d -= a << 16;
    if (d > 0)
        d = 0;
    if (d < (s32)0xFFFD0000)
        d = 0xFFFD0000;
    TaskSetMotionY(d, 0x2000, 0x30000);
}

s32 sub_0808ca00(s32 a, s32 b)
{
    struct Task *t;
    s16 *pa;
    s32 x;
    s32 p;
    s32 q;
    s32 i;
    s32 v;
    s32 w;
    s32 u;

    t = gCurTask;
    pa = &t->pixelX;
    x = (s8)a;
    p = (u16)*pa + x;
    q = (u16)t->pixelY + (s8)b;
    i = GetCollisionTileAtPixel(p, q);
    if (gUnk_087339F0[i] == 0 || gUnk_087336F0[i] != 0
     || gUnk_087337F0[i] != 0 || gCollisionTileSlope[i] != 0
     || gUnk_08732FF0[i] != 0)
        goto minus1;
    u = ((p << 16) >> 16) & 15;
    w = (u << 4) | u;
    v = (s8)((u8 *)gCollisionTilePushLeft[i])[w];
    if (v < 0)
        v = -v;
    return x - v;
minus1:
    return -1;
}

s32 sub_0808cab8(s32 a, s32 b)
{
    struct Task *t;
    s16 *pa;
    s32 x;
    s32 p;
    s32 q;
    s32 i;
    s32 v;
    s32 w;
    s32 u;

    t = gCurTask;
    pa = &t->pixelX;
    x = (s8)a;
    p = (u16)*pa - x;
    q = (u16)t->pixelY + (s8)b;
    i = GetCollisionTileAtPixel(p, q);
    if (gUnk_087339F0[i] == 0 || gUnk_087336F0[i] != 0
     || gUnk_087337F0[i] != 0 || gCollisionTileSlope[i] != 0
     || gUnk_08732FF0[i] != 0)
        goto minus1;
    u = ((p << 16) >> 16) & 15;
    w = (u << 4) | u;
    v = (s8)((u8 *)gCollisionTilePushRight[i])[w];
    if (v < 0)
        v = -v;
    return x - v;
minus1:
    return -1;
}

void sub_0808cb70(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    gCurTask->layer = 12;
    t = gCurTask;
    t->frameTable = gUnk_087522B4;
    t->tileWord = (t->tileWord & 0xFFF) | 0xF000;
    t->updateCallback = (u32)sub_0808cc14;
    t->unk30 = 0;
    t->unk34 = 0;
    t->unk6C = 0;
    do
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(1);
        TaskSetFrame(-1);
        TaskYieldTrampoline(1);
        TaskSetFrame(1);
        TaskYieldTrampoline(1);
        TaskSetFrame(-1);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 59);
    ActorDestroy();
}

void sub_0808cc14(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    if (gCurTask->unk34 <= 0)
    {
        PlaySfx(185);
        gCurTask->unk34 = 5;
    }
    t = gCurTask;
    t->unk34--;
    u = &gTasks[t->parent];
    if (u->hitKind != 0 || u->onGround == 0)
    {
        TaskSetEntry(ActorDie, gCurTaskIdx);
        return;
    }
    n = t->unk30;
    if (n == 0)
    {
        gCurTask->unk46 = CreateChildTaskHere(215, 1);
        gTasks[gCurTask->unk46].state = 0;
        gCurTask->unk30++;
    }
    else if (n == 10)
    {
        gCurTask->unk46 = CreateChildTaskHere(215, 1);
        gTasks[gCurTask->unk46].state = 1;
        gCurTask->unk30++;
    }
    else if (n > 19)
        t->unk30 = 0;
    else
        t->unk30 = n + 1;
    ActorCheckHits();
    ActorReactToHit();
}
