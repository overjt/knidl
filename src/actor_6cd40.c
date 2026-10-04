/* game_code_and_rodata 0x0806CD40-0x0806D22C (issue #64, module M18 batch 4c).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806CD40 0x0806D22C src/actor_6cd40.c --newpb
 *
 * The tail of the vehicle/ride block: Task_DustTrail's per-frame integrator
 * over the gTasks[] task table, the two sprite-list players
 * Task_DustPuff / Task_BackwardDustPuff, and the spawn/teardown helpers
 * CreateLandingDust / sub_0806d08c / Task_LandingDust / CreateDustBurst.  Every literal
 * pool in this range ends exactly on the next function's entry, so any
 * symbols.csv boundary here is a valid carve point.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "actor.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);
extern u16 RandomSpread(s32 base, u8 scale, u8 amount);

void Task_DustTrail(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 j;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CB90;
    while (u->dustTrailPuffCount != 0 && gTaskSlotTypes[u->parent] != -1)
    {
        if (TaskHasSameSerial(gCurTask->parent) != 1)
            break;
        if ((s8)gTasks[j = gCurTask->parent].hitKind == HIT_KIND_INHALE
         && gTasks[j].hitEffect == HIT_EFFECT_INHALE)
            break;
        if ((s8)gTasks[j].hitKind == HIT_KIND_GRAB
         && (u16)(gTasks[j].hitEffect - 2) <= 1)
            break;
        v = gCurTask;
        v->posX = (gTasks[j].pixelX + v->dustTrailOffsetX) << 16;
        v->posY = (gTasks[j].pixelY + v->dustTrailOffsetY) << 16;
        v->accelY = -0x2000;
        TaskSetMotionXFacing(0xFFFD0000, 0x5A5A5A5A);
        TaskSetFrameByFacing(0);
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(2);
        gCurTask->frame += 2;
        TaskYieldTrampoline(1);
        CreateDustPuff();
        TaskYieldTrampoline(1);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        gCurTask->frame = 0xFFFF;
        TaskYieldTrampoline(1);
        TaskStop();
        u = gCurTask;
        u->dustTrailPuffCount--;
    }
    TaskExitTrampoline();
}

void Task_DustPuff(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CB90;
    TaskFaceLikeParent();
    gCurTask->posX = (RandomSpreadFacing(-8, 1, 8) + gCurTask->pixelX) << 16;
    gCurTask->posY = (RandomSpread(-8, 1, 8) + gCurTask->pixelY) << 16;
    TaskSetMotionXFacing(0x5A5A5A5A, 0x4000);
    gCurTask->accelY = -0x4000;
    TaskSetFrameByFacing(4);
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(2);
    gCurTask->frame -= 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void Task_BackwardDustPuff(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CB90;
    TaskFaceLikeParent();
    TaskSetMotionXFacing(0xFFFDC000, 0x1800);
    v = gCurTask;
    v->velY = -0x4000;
    v->accelY = -0x2000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    TaskSetFrameByFacing(6);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
void CreateLandingDust(s16 dx, s16 dy)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    i = (s16)CreateChildTask(TASK_LANDING_DUST, (s16)(dx + t->pixelX), (s16)(dy + t->pixelY), 0);
    if (i != -1)
        gTasks[i].facing = 1;
    u = gCurTask;
    i = (s16)CreateChildTask(TASK_LANDING_DUST, (s16)(u->pixelX - dx), (s16)(dy + u->pixelY), 0);
    if (i != -1)
        gTasks[i].facing = 0xFF;
}

void sub_0806d08c(s16 a, s16 b, s16 c)
{
    struct Task *t;
    struct Task *u;
    s32 i;

    t = gCurTask;
    i = (s16)CreateChildTask(TASK_LANDING_DUST, (s16)(t->pixelX + t->facing * a),
                          (s16)(c + t->pixelY), 0);
    if (i != -1)
        gTasks[i].facing = gCurTask->facing;
    u = gCurTask;
    i = (s16)CreateChildTask(TASK_LANDING_DUST, (s16)(u->pixelX - b * u->facing),
                          (s16)(c + u->pixelY), 0);
    if (i != -1)
        gTasks[i].facing = -gCurTask->facing;
}

void Task_LandingDust(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->moveCallback = (u32)ActorMove;
    t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
    t->layer = 10;
    u = gCurTask;
    u->frameTable = gUnk_0874CB90;
    if (u->facing != 1 && u->facing != -1)
        u->facing = 1;
    TaskSetMotionXFacing(0x24000, 0xFFFFE800);
    v = gCurTask;
    v->velY = -0x4000;
    v->accelY = -0x2000;
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

s32 CreateDustBurst(s16 a, s16 b)
{
    struct Task *p;
    s32 i;

    i = CreateChildTaskAt(TASK_DUST_BURST, 0, 0, 0);
    if (i != -1)
    {
        p = &gTasks[i];
        p->dustBurstOffsetX = a;
        p->dustBurstOffsetY = b;
    }
    return i;
}
