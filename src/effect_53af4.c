#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "effect.h"
#include "enemy.h"

/* effect_53af4.c (0x08053AF4-0x0805432F, issue #89).
 *
 * Task type #7, the player's effect objects: the body and variants 0-6.
 * Task type #7 (class 1) is started by M16's CreatePlayerEffect/CreatePlayerEffectHighSlot
 * (src/effect_5afac.c), and by M14's CreatePlayerObject when its type-6 bands are
 * full, with Task.unk18 = variant << 24 | arg and the spawner's position,
 * facing Task.facing and PlayerState Task.player copied in.  The body
 * Task_PlayerEffect links the task to its spawner the first time (Task.u8C.parentTask =
 * &gTasks[Task.parent], read back as a struct Task) and dispatches the
 * variant, the top byte of Task.unk18, through the 49 entries of
 * gPlayerEffectVariants, which fill this file and the eleven effect_*.c files after
 * it.  A variant installs a motion hook in Task.moveCallback (TaskMove moves in
 * world space, TaskMoveRelativeToParent keeps the position relative to the spawner's
 * task, TaskUpdatePixelPos stays put), a draw hook in Task.drawCallback, often a
 * per-frame callback in Task.updateCallback and an animation table in Task.frameTable,
 * then runs a TaskYieldTrampoline script and ends in TaskExitTrampoline;
 * the functions after a body are the callbacks only it installs.  Variant 0
 * (sub_08053b40, spawned by M10) rides on its spawner and cycles frames
 * 0-11, hidden every other frame; sub_08053be0 copies the spawner's
 * Task.skipMask with bit 2 cleared and kills it once PlayerState.unk40 bit 2
 * clears, and its draw hook sub_08053c1c draws through M11's sub_0803dfc8 in
 * player mode 10 and kills it otherwise.  Variants 1 and 2 (M10) are short
 * puffs launched from 12 pixels behind the point they face, the sub-state
 * picking the facing; 3 (M13's ability get) shows for three frames at a
 * random offset from its spawner (sub_08053e34 is its empty Task.updateCallback
 * stub); 4 and 5 fly one of eight random trajectories of the s16 rows
 * gUnk_0873B9EC[6][8] (offsets and 8.8 velocities), 5 with TaskDrawScreen's
 * draw when PlayerState.unk37 == 2.  Variant 6 has two forms picked by the
 * third byte of Task.unk18: a loop that places a puff 6 pixels behind the
 * spawner and 8 below it and spawns the other form (a rising puff) each
 * round until PlayerEffectSkidDustUpdate sets Task.unk28 - when the player's mode differs
 * from the one saved at the start or is 16, or, by the second byte of
 * Task.unk18, when the spawner's Task.onGround clears, a countdown in
 * Task.unk30 runs out or M11's PlayerGetFacingSlope no longer returns 4. */

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 RandomRange(u32 range);                       /* RNG: 0 .. range-1 */
void TaskFree(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void TaskSetPosXFacing(u16 a);
u16 RandomSpread(s32 base, u8 scale, u8 amount);   /* base + ((rand(256) * amount) >> 8) * scale */
s16 RandomSpreadFacing(s32 base, u8 scale, u8 amount);   /* the same, negated when Task.facing != 1 */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);          /* M16's effect spawner (spawns task type #7) */

void Task_PlayerEffect(void)
{
    if (gCurTask->u8C.parentTask == NULL)
    {
        gCurTask->u80.attackAbility = 0;
        gCurTask->u8C.parentTask = &gTasks[gCurTask->parent];
    }
    CallTableEntry(((u8 *)gCurTask)[27], 49, gPlayerEffectVariants);
}

void sub_08053b40(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    gCurTask->drawCallback = (u32)sub_08053c1c;
    gCurTask->updateCallback = (u32)sub_08053be0;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_08751C44;
    t->tileWord = ((t->u8C.parentTask)->tileWord + 0x1800) | 4;
    TaskSetPosXFacing(4);
    gCurTask->posY = 0x40000;
    for (;;)
    {
        gCurTask->unk28 = 0;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame((s16)gCurTask->unk28++);
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 11);
    }
}

void sub_08053be0(void)
{
    gCurTask->skipMask = (gCurTask->u8C.parentTask)->skipMask & 0xFB;
    if (!(gCurTask->player->unk40 & 4))
        TaskFree(gCurTaskIdx);
}

void sub_08053c1c(void)
{
    if (gCurTask->player->mode == 10)
        sub_0803dfc8();
    else
        TaskFree(gCurTaskIdx);
}

void sub_08053c48(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    if ((t->unk18 & 15) == 0)
        t->facing = -1;
    else
        t->facing = 1;
    TaskStepForward(-12);
    gCurTask->posY = (gCurTask->pixelY + 6) << 16;
    TaskSetMotionXFacing(-0x24000, 0x1800);
    gCurTask->velY = -0x4000;
    gCurTask->accelY = -0x2000;
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

void sub_08053d08(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    if ((t->unk18 & 15) == 0)
        t->facing = -1;
    else
        t->facing = 1;
    TaskStepForward(-12);
    gCurTask->posY = (gCurTask->pixelY + 6) << 16;
    TaskSetMotionXFacing(-0x60000, 0xC000);
    gCurTask->velY = -0x20000;
    gCurTask->accelY = 0x4000;
    TaskSetFrameByFacing(0);
    TaskYieldTrampoline(4);
    TaskSetFrameByFacing(10);
    TaskYieldTrampoline(2);
    gCurTask->frame += 2;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void PlayerEffectAbilityGetSparkle(void)
{
    struct Task *t = gCurTask;
    s32 s;

    t->updateCallback = (u32)sub_08053e34;
    s = t->unk18 & 15;
    if (s == 0)
    {
        t->moveCallback = (u32)TaskMoveRelativeToParent;
        t->drawCallback = (u32)TaskDrawWorld;
        t->layer = 5;
        gCurTask->frameTable = gUnk_08751CEC;
        gCurTask->posX = RandomSpreadFacing(-16, 1, 16) << 16;
        gCurTask->posY = (RandomSpread(-4, 1, 16) << 16) - 0x180000;
        gCurTask->frame = 0;
        TaskYieldTrampoline(3);
    }
    TaskExitTrampoline();
}

void sub_08053e34(void)
{
}

void PlayerEffectImpactStar(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 c;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    gCurTask->frameTable = gUnk_0874C500;
    n = RandomRange(8);
    TaskStepForward(gUnk_0873B9EC[n]);
    t = gCurTask;
    t->posY = (t->pixelY + (gUnk_0873B9EC + 8)[n] + 4) << 16;
    a = (gUnk_0873B9EC + 16)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->velX = b;
    a = (gUnk_0873B9EC + 24)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->velY = b;
    t->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    v = gCurTask;
    c = (gUnk_0873B9EC + 32)[n];
    b = c << 8;
    if (c & 0x8000)
        b |= 0xFF000000;
    v->velX = b;
    a = (gUnk_0873B9EC + 40)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    v->velY = b;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->velX = 0;
    w->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void PlayerEffectDeathStar(void)
{
    struct Task *u;
    struct Task *t;
    struct Task *v;
    struct Task *w;
    s32 n;
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
    s32 f;

    u = gCurTask;
    u->moveCallback = (u32)TaskMove;
    if (u->player->unk37 == 2)
        u->drawCallback = (u32)TaskDrawScreen;
    else
        u->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 8;
    gCurTask->frameTable = gUnk_0874C500;
    n = RandomRange(8);
    TaskStepForward(gUnk_0873B9EC[n]);
    t = gCurTask;
    t->posY = (t->pixelY + (gUnk_0873B9EC + 8)[n] + 4) << 16;
    a = (gUnk_0873B9EC + 16)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->velX = b;
    a = (gUnk_0873B9EC + 24)[n];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    t->velY = b;
    t->frame = 0;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    v = gCurTask;
    c = (gUnk_0873B9EC + 32)[n];
    d = c << 8;
    if (c & 0x8000)
        d |= 0xFF000000;
    v->velX = d;
    a = (gUnk_0873B9EC + 40)[n];
    d = a << 8;
    if (a & 0x8000)
        d |= 0xFF000000;
    v->velY = d;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    w = gCurTask;
    w->velX = 0;
    w->velY = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void PlayerEffectSkidDust(void)
{
    struct Task *t;

    gCurTask->moveCallback = (u32)TaskMove;
    gCurTask->drawCallback = (u32)TaskDrawWorld;
    gCurTask->layer = 5;
    t = gCurTask;
    t->frameTable = gUnk_0874C600;
    switch (t->unk18 & 0xFF0000)
    {
    case 0:
        t->unk28 = 0;
        t->unk2C = t->player->mode;
        t->unk30 = t->unk18 & 0xFF;
        t->updateCallback = (u32)PlayerEffectSkidDustUpdate;
        do
        {
            struct Task *u = gCurTask;
            struct Task *p;

            if (u->facing == 1)
                u->posX = ((p = u->u8C.parentTask)->pixelX - 6) << 16;
            else
                u->posX = ((p = u->u8C.parentTask)->pixelX + 6) << 16;
            u->posY = ((u->u8C.parentTask)->pixelY + 8) << 16;
            TaskStop();
            gCurTask->accelY = -0x2000;
            TaskSetMotionXFacing(-0x30000, 0x5A5A5A5A);
            TaskSetFrameByFacing(0);
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(2);
            gCurTask->frame += 2;
            TaskYieldTrampoline(1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 6, 0x10000);
            TaskYieldTrampoline(1);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(2);
            gCurTask->frame -= 2;
            TaskYieldTrampoline(1);
            gCurTask->frame = 0xFFFF;
            TaskYieldTrampoline(1);
            TaskStop();
        } while (gCurTask->unk28 == 0);
        break;
    case 0x10000:
        gCurTask->posX = (gCurTask->pixelX + RandomSpreadFacing(-8, 1, 8)) << 16;
        gCurTask->posY = (gCurTask->pixelY + RandomSpread(-8, 1, 8)) << 16;
        TaskSetMotionXFacing(0x5A5A5A5A, 0x4000);
        gCurTask->accelY = -0x4000;
        TaskSetFrameByFacing(4);
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(2);
        gCurTask->frame -= 2;
        TaskYieldTrampoline(1);
        break;
    }
    TaskExitTrampoline();
}

void PlayerEffectSkidDustUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 == 0)
    {
        u8 m = t->player->mode;

        if (m != t->unk2C || m == 16)
        {
            t->unk28 = 1;
            return;
        }
        switch (t->unk18 & 0xFF00)
        {
        case 0:
            if ((t->u8C.parentTask)->onGround == 0)
                t->unk28 = 1;
        case 0x100:
        {
            struct Task *u = gCurTask;

            if (((u8 *)u)[24] != 0 && --u->unk30 == 0)
                u->unk28++;
            break;
        }
        case 0x200:
            if (PlayerGetFacingSlope(t->parent) != 4)
                gCurTask->unk28++;
            break;
        }
    }
}
