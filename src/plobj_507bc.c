#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"

/* plobj_507bc.c (0x080507BC-0x080509EB, issue #90).
 *
 * Task type #6 (the objects the player's actions spawn through
 * CreatePlayerObject/CreatePlayerObjectLowSlot): the body and two shared callbacks.  The body
 * Task_PlayerObject links the task to its spawner (Task.u8C.parentTask =
 * &gTasks[Task.parent], the first time only) and dispatches the
 * variant, the top byte of Task.unk18, through the 13 variant bodies
 * gPlayerObjectVariants.  PlayerObjectVanish is the common exit most variants re-bind on
 * contact: it installs the sprite (TaskDrawScreen for PlayerState.unk37 == 2,
 * TaskDrawWorldInViewOrFree otherwise, animation table gPlayerObjectVanishFrames), registers the
 * collider row Task.unk24 if there is one and steps animation frames
 * 0-10 before TaskExitTrampoline.  PlayerObjectLaserBeamVanish is a second, drifting burst
 * (gSmokePuffFrames). */

/* task / sprite services (landed prototypes) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */

void Task_PlayerObject(void)
{
    if (gCurTask->u8C.parentTask == NULL)
    {
        gCurTask->u80.attackAbility = ABILITY_NORMAL;
        gCurTask->u8C.parentTask = &gTasks[gCurTask->parent];
        gCurTask->health = 1;
        gCurTask->playerObjectVanishBox = 0;
    }
    CallTableEntry(((u8 *)gCurTask)[27], 13, gPlayerObjectVariants);
}

void PlayerObjectVanish(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    if (t->player->unk37 != 2)
        t->drawCallback = (u32)TaskDrawWorldInViewOrFree;
    else
        t->drawCallback = (u32)TaskDrawScreen;
    gCurTask->updateCallback = 0;
    gCurTask->lateUpdateCallback = 0;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gPlayerObjectVanishFrames;
    t->spriteFlags = 0;
    t->tileWord = 0;
    if (t->playerObjectVanishBox != 0)
        RegisterCollider(gCurTaskIdx, t->pixelX, t->pixelY, (void *)t->playerObjectVanishBox);
    TaskStop();
    gCurTask->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 6;
    TaskYieldTrampoline(1);
    gCurTask->frame = 2;
    TaskYieldTrampoline(1);
    gCurTask->frame = 7;
    TaskYieldTrampoline(1);
    gCurTask->frame = 3;
    TaskYieldTrampoline(1);
    gCurTask->frame = 8;
    TaskYieldTrampoline(1);
    gCurTask->frame = 4;
    TaskYieldTrampoline(1);
    gCurTask->frame = 9;
    TaskYieldTrampoline(1);
    gCurTask->frame = 5;
    TaskYieldTrampoline(1);
    gCurTask->frame = 10;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void PlayerObjectLaserBeamVanish(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorldInViewOrFree;
    gCurTask->updateCallback = 0;
    gCurTask->lateUpdateCallback = 0;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gSmokePuffFrames;
    t->spriteFlags = 0;
    t->tileWord = 0;
    t->unk28 = 0;
    if (t->velY < 0)
        t->unk28 = 1;
    TaskStop();
    if (gCurTask->unk28 == 1)
        gCurTask->velY = 0x8000;
    else
        gCurTask->velY = -0x8000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(3);
    gCurTask->frame += 2;
    TaskYieldTrampoline(3);
    gCurTask->frame += 2;
    TaskYieldTrampoline(3);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
