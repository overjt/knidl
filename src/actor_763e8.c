/* game_code_and_rodata 0x080763E8-0x08077AE0 (issue #79, module M19 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080763E8 0x08077AE0 src/actor_763e8.c --newpb
 *
 * M19 batch 4: task type #99's cutscene director tail plus type #75
 * (Task_Cannon) and type #76 (Task_CannonFuse) - the ending-pose and
 * script-walker families over struct CannonFusePiece.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "cutscene.h"
#include "collision.h"
#include "room.h"
#include "actor.h"

/* callees */
extern s32 PlaySfx(u32 a);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern s32 sub_080269d8();
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *a, u32 i);
extern void TerrainCollideBox(u32 *p);
extern void RequestScreenShake(u32 a);
extern void ActorSetState(u8 v);
extern void AngleToVector(s16 t, s16 mag);

void PlayerCannonStepPose(void)
{
    struct Task *t = gCurTask;

    if (t->playerCannonPoseTimer <= 0)
    {
        if (t->playerCannonPoseDir != 0)
        {
            if (t->playerCannonPoseRow > 0)
                t->playerCannonPoseRow--;
        }
        else
        {
            if (t->playerCannonPoseRow <= 3)
                t->playerCannonPoseRow++;
        }
        if (gMetaKnightmareMode == 0)
            gCurTask->frame = gUnk_0874009C[gCurTask->playerCannonPoseRow];
        else
            gCurTask->frame = gUnk_087400A6[gCurTask->playerCannonPoseRow];
        gCurTask->playerCannonPoseTimer = 1;
    }
    gCurTask->playerCannonPoseTimer--;
}

void sub_08076454(void)
{
    struct Task *t = gCurTask;

    t->posX = 0;
    t->posY = -0x180000;
    t->playerCannonCameraFollow = 0;
    t->playerCannonPoseTimer = 1;
    t->playerCannonPoseDir = 1;
    t->playerCannonPoseRow = 2;
    if (gUnk_0300244C != 0 && gMetaKnightmareMode == 1)
        t->frame = 0x1265;
    else
        t->frame = 0x11C7;
    {
        u16 id = gLocalPlayer;
        struct Task *u = gCurTask;

        if (id == u->player->playerIndex)
        {
            s32 x = u->posX + gTasks[u->parent].posX;
            s32 y = u->posY + gTasks[u->parent].posY;

            SetCameraFocus(x >> 16, y >> 16);
        }
    }
}

void sub_080764f8(void)
{
    vu16 *p = gPlayerPressedKeys;
    struct Task *t = gCurTask;

    if (p[t->player->playerIndex] & 1)
    {
        PlayerJumpOutOfCannon(gCurTaskIdx);
    }
    else
    {
        if (gPlayerHeldKeys[t->player->playerIndex] & 0x80)
            t->playerCannonPoseDir = 0;
        else
            t->playerCannonPoseDir = 1;
        PlayerCannonStepPose();
    }
}

void PlayerCannonSetLaunchVelocityFacing(u16 a)
{
    struct Task *t = gCurTask;

    switch (t->playerCannonRiderIndex)
    {
    case 0:
        if (t->playerCannonRiderCount == 2 || t->playerCannonRiderCount == 4)
            a = a + 8;
        break;
    case 1:
        a = a + 8;
        break;
    case 2:
    case 3:
        a = a + 8;
        if (t->playerCannonRiderCount == 4)
            a = a + 16;
        break;
    }
    if (gCurTask->playerCannonRiderIndex & 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    AngleToVector((s16)a, 1024);
    TaskSetMotionXFacing(gUnk_030023B4, 0x5A5A5A5A);
    gCurTask->velY = gUnk_030023D4;
}

void PlayerCannonSetLaunchVelocity(u16 a)
{
    struct Task *t = gCurTask;

    switch (t->playerCannonRiderIndex)
    {
    case 0:
        if (t->playerCannonRiderCount == 2 || t->playerCannonRiderCount == 4)
            a = a + 8;
        break;
    case 1:
        a = a - 8;
        break;
    case 2:
    case 3:
        if (t->playerCannonRiderIndex & 1)
            a = a - 8;
        else
            a = a + 8;
        if (gCurTask->playerCannonRiderCount == 4)
        {
            if (gCurTask->playerCannonRiderIndex & 1)
                a = a - 16;
            else
                a = a + 16;
        }
        break;
    }
    if (gCurTask->playerCannonRiderIndex & 1)
        gCurTask->facing = 255;
    else
        gCurTask->facing = 1;
    AngleToVector((s16)a, 1024);
    {
        struct Task *u = gCurTask;

        u->velX = gUnk_030023B4;
        u->velY = gUnk_030023D4;
    }
}

void sub_080766ac(void)
{
    {
        struct Task *t = gCurTask;

        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->frame = 0xFFFF;
    }
    TaskYieldTrampoline(25);
    PlayerCannonSetLaunchVelocityFacing(384);
    PlayerStartTumble();
    TaskGetScreenPos();
    if (gUnk_030023D4 > 63)
        gCurTask->playerCannonCameraFollow = 1;
    {
        struct Task *t = gCurTask;

        t->playerCannonExited = 0;
        t->playerCannonSmokeTimer = 8;
    }
}

void sub_08076710(void)
{
    {
        struct Task *t = gCurTask;

        t->pixelX += 13;
        t->pixelY += 8;
        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        t->facing = 1;
    }
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(25);
    PlayerCannonSetLaunchVelocity(448);
    PlayerStartTumble();
    TaskGetScreenPos();
    if (gUnk_030023B4 > 95 || gUnk_030023D4 > 63)
        gCurTask->playerCannonCameraFollow = 1;
    {
        struct Task *t = gCurTask;

        t->playerCannonExited = 0;
        t->playerCannonSmokeTimer = 8;
    }
}

void sub_08076798(void)
{
    PlayerCannonSetLaunchVelocityFacing(384);
    {
        struct Task *t = gCurTask;

        t->posX = t->pixelX << 16;
        t->posY = t->pixelY << 16;
        if (gMetaKnightmareMode == 0)
            TaskSetFrame(gPlayerJumpFrames[t->player->ability]);
        else
            TaskSetFrame(0x11E4);
    }
    TaskGetScreenPos();
    if (gUnk_030023D4 > 63)
        gCurTask->playerCannonCameraFollow = 1;
    {
        struct Task *t = gCurTask;

        t->playerCannonExited = 0;
        t->playerCannonEndTimer = 10;
    }
}

void PlayerCannonCheckExit(void)
{
    TaskGetScreenPos();
    if (gCurTask->playerCannonExited == 0 && gUnk_030023D4 <= 0)
    {
        ExitByCannon();
        {
            struct Task *t = gCurTask;

            t->playerCannonCameraFollow = 0;
            t->playerCannonExited = 1;
        }
    }
}

void PlayerCannonStepFlight(s32 a)
{
    struct Task *t = gCurTask;

    t->playerCannonFocusX = t->pixelX;
    t->playerCannonFocusY = t->pixelY;
    if (t->frame != -1)
    {
        if (gMetaKnightmareMode == 0)
            PlayerStepTumble();
        {
            struct Task *u = gCurTask;

            if ((s16)u->playerCannonSmokeTimer <= 0)
            {
                u->playerCannonSmokeTimer = 8;
                CreateCannonSmoke(0, a);
            }
        }
        gCurTask->playerCannonSmokeTimer--;
        PlayerCannonCheckExit();
    }
}

void sub_080768c8(void)
{
    gCurTask->facing = 1;
    gCurTask->playerCannonCameraFollow = 1;
    TaskSetMotionY(0x18000, 0x8000, 0x40000);
    {
        struct Task *t = gCurTask;
        u16 x;

        t->spriteFlags |= SPRITE_FLAG_FLIP_X;
        t->playerRideLandCount = 0;
        t->playerRideIsCannon = 1;
        x = t->posX >> 16;
        switch (t->player->playerIndex)
        {
        case 0:
            break;
        case 3:
            x = ((x << 16) + 0x80000) >> 16;
        case 2:
            x = ((x << 16) + 0x80000) >> 16;
        case 1:
            x = ((x << 16) + 0x80000) >> 16;
            break;
        }
        gCurTask->posX = x << 16;
    }
    DisablePause();
}

void sub_08076958(void)
{
    u8 k = gCurTask->player->ability;

    if (k == 1 || k == 2 || (s8)k == 4 || (s8)k == 5 || (s8)k == 9
     || (s8)k == 10 || (s8)k == 15 || (s8)k == 16 || (s8)k == 17
     || (s8)k == 19 || (s8)k == 22 || (s8)k == 23)
    {
        s32 v = (u16)gCurTask->frame << 16;

    loop:
        TaskSetFrame(v >> 16);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        goto loop;
    }
    TaskSleepForever();
}

void PlayerCannonAnimateMetaKnightLaunch(void)
{
    while (1)
    {
        TaskSetFrame(0x124D);
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount++;
        } while ((s16)gCurTask->playerLoopCount <= 6);
    }
}

void PlayerCannonAnimateMetaKnightArrival(void)
{
    while (1)
    {
        TaskSetFrame(0x124D);
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount++;
        } while ((s16)gCurTask->playerLoopCount <= 6);
    }
}

void sub_08076a58(void)
{
    TaskSetMotionXFacing(0x40000, -0x10000);
    TaskSetFrame(0x1255);
    TaskYieldTrampoline(4);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x10000, 0);
    {
        struct Task *t = gCurTask;

        t->velY = -0x49800;
        t->accelY = 0x3800;
        t->playerLoopCount = 0;
    }
    do
    {
        TaskSetFrame(0x1256);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->playerLoopCount++;
    } while ((s16)gCurTask->playerLoopCount <= 2);
    TaskSetMotionXFacing(0x50000, -0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x30000, 0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(0x50000, -0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(-0x30000, 0x20000);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    TaskSetMotionXFacing(0x30000, 0);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    if (gCurTask->playerRideLandCount == 1)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->playerRideLandCount == 1);
    }
    TaskYieldTrampoline(3);
    PlayerEndRideLanding(gCurTaskIdx);
    TaskSleepForever();
}

void sub_08076c00(void)
{
    struct Task *t = gCurTask;

    if (t->velY != 0 && (t->onGround & 1))
    {
        t->playerRideLandCount++;
        TaskStop();
        gCurTask->onGround = 0;
        {
            struct Task *u = gCurTask;

            switch (u->playerRideLandCount)
            {
            case 1:
                PlaySfx(153);
                RequestScreenShake(4);
                CreateBurstEffect(0, 0);
                {
                    struct Task *v = gCurTask;

                    v->frame = 0x1255;
                    v->state = PLAYER_CANNON_STATE_5;
                }
                TaskSetEntry(PlayerCannonEnterState, gCurTaskIdx);
                break;
            case 2:
                u->frame = 0x1264;
                break;
            }
        }
    }
}

void PlayerCannonWait(void)
{
    gCurTask->updateState = PLAYER_CANNON_STATE_WAIT;
    gCurTask->moveCallback = (u32)TaskMoveRelativeToParent;
    sub_08076454();
    TaskSleepForever();
}

void PlayerCannonWaitUpdate(void)
{
    struct Task *t = gCurTask;

    t->playerCannonFocusX = t->pixelX;
    t->playerCannonFocusY = t->pixelY;
    sub_080764f8();
}

void PlayerCannonLaunchUp(void)
{
    gCurTask->updateState = PLAYER_CANNON_STATE_LAUNCH_UP;
    sub_080766ac();
    if (gMetaKnightmareMode == 1)
        PlayerCannonAnimateMetaKnightLaunch();
    TaskSleepForever();
}

void PlayerCannonLaunchUpUpdate(void)
{
    PlayerCannonStepFlight(0);
}

void PlayerCannonLaunchUpRight(void)
{
    gCurTask->updateState = PLAYER_CANNON_STATE_LAUNCH_UP_RIGHT;
    sub_08076710();
    if (gMetaKnightmareMode == 1)
        PlayerCannonAnimateMetaKnightLaunch();
    TaskSleepForever();
}

void PlayerCannonLaunchUpRightUpdate(void)
{
    PlayerCannonStepFlight(1);
}

void PlayerCannonLaunchShort(void)
{
    gCurTask->updateState = PLAYER_CANNON_STATE_LAUNCH_SHORT;
    sub_08076798();
    if (gMetaKnightmareMode == 0)
        sub_08076958();
    else
        TaskSleepForever();
}

void PlayerCannonLaunchShortUpdate(void)
{
    struct Task *t = gCurTask;

    t->playerCannonFocusX = t->pixelX;
    t->playerCannonFocusY = t->pixelY;
    if (t->playerCannonEndTimer <= 0)
        PlayerEndCannonLaunch(gCurTaskIdx);
    else
        t->playerCannonEndTimer--;
}

void PlayerCannonArrive(void)
{
    gCurTask->updateState = PLAYER_CANNON_STATE_ARRIVE;
    sub_080768c8();
    if (gMetaKnightmareMode == 0)
    {
        PlayerStartHighFallPose();
        PlayerWaitForRideLanding();
    }
    else
    {
        PlayerCannonAnimateMetaKnightArrival();
    }
}

void PlayerCannonArriveUpdate(void)
{
    TerrainCollideBox(gPlayerCannonTerrainBox);
    if (gMetaKnightmareMode == 0)
    {
        PlayerStepRideBounces();
        if (gCurTask->playerRideLandCount == 0)
            PlayerStepHighFallPose();
    }
    else
    {
        sub_08076c00();
    }
    {
        struct Task *t = gCurTask;

        t->playerCannonFocusX = t->pixelX;
        t->playerCannonFocusY = t->pixelY;
    }
}

void PlayerCannonState5(void)
{
    gCurTask->updateState = PLAYER_CANNON_STATE_5;
    sub_08076a58();
}

void PlayerCannonState5Update(void)
{
    if ((gCurTask->onGround & 1) == 0)
        TerrainCollideBox(gPlayerCannonTerrainBox);
    sub_08076c00();
    {
        struct Task *t = gCurTask;

        t->playerCannonFocusX = t->pixelX;
        t->playerCannonFocusY = t->pixelY;
    }
}

void PlayerEnterCannon(s32 id, s32 v)
{
    struct Task *t = &gTasks[id];

    PlayerSuspendControl(id, 1);
    t->state = 0;
    t->parent = v;
    t->waterFlags = 0;
    TaskSetEntry(PlayerCannonInit, id);
}

void PlayerLeaveCannon(s32 id)
{
    struct Task *u = &gTasks[id];
    struct Task *t = &gTasks[u->parent];
    t->cannonLoadTimer = 60;
    t->cannonRiderCount--;
    t->cannonRiderMask &= ~(1 << gCurTaskIdx);
}

void PlayerEndCannonLaunch(s32 id)
{
    struct Task *t = &gTasks[id];
    struct PlayerState *p = &gPlayerStates[id];

    PlayerLeaveCannon(id);
    t->onGround = 0;
    TaskStopY();
    PlayerResumeControl(id, 0, 1, 0);
    HudShowAbility(p->ability, id);
}

void PlayerJumpOutOfCannon(s32 id)
{
    struct Task *t = &gTasks[id];
    struct PlayerState *p = &gPlayerStates[id];
    struct Task *u = &gTasks[t->parent];

    PlayerLeaveCannon(id);
    t->facing = 1;
    t->onGround = 0;
    {
        struct Task *v = gCurTask;

        t->posX = v->pixelX << 16;
        t->posY = v->pixelY << 16;
    }
    TaskStopSlot(id);
    PlayerResumeControl(id, 6, 1, 0);
    p->playerJumpPhaseTimer = 8;
    HudShowAbility(p->ability, id);
    if (u->unk1C > 0)
        DisablePause();
}

void CannonLaunchPlayers(int a)
{
    u16 v = a;
    s32 i = 0;
    s32 n = 0;

    for (; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            struct Task *t = &gTasks[i];

            if ((gCurTask->cannonRiderMask >> i) & 1)
            {
                t->playerCannonRiderIndex = n;
                n++;
                t->playerCannonRiderCount = gCurTask->cannonRiderCount;
                t->state = v;
                TaskSetEntry(PlayerCannonEnterState, i);
            }
        }
    }
}

void Task_Cannon(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 13;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gCannonFrames;
        t->cannonLoadTimer = 0;
        CallTableEntry(t->variant, 1, gCannonVariants);
    }
}

u16 CannonPickLaunchState(void)
{
    s32 i = gLevelIndex * 2 + gCurTask->actorSpawnArg;
    u16 v;

    if (i > 13)
        i = 0;
    v = gUnk_087400E4[i];
    switch (v)
    {
    case 1:
        ActorSetState(CANNON_STATE_LAUNCH_UP);
        break;
    case 2:
        ActorSetState(CANNON_STATE_LAUNCH_UP_RIGHT);
        break;
    }
    return v;
}

void CannonLoadPlayer(s32 id)
{
    if (gUnk_0300244C == 0 || gPlayerHealth[id] > 0)
    {
        struct Task *t = &gTasks[id];
        struct PlayerState *p = &gPlayerStates[id];

        if (p->mouthState != 1 && t->velY > 0)
        {
            PlayerEnterCannon(id, gCurTaskIdx);
            {
                struct Task *u = gCurTask;

                u->cannonRiderCount++;
                u->cannonLoadTimer = 0;
                u->cannonRiderMask |= 1 << id;
            }
        }
        else
        {
            gCurTask->hitTimer = 0;
        }
    }
}

void sub_0807717c(void)
{
    if (gCurTask->cannonRiderCount == 0)
        CreateCannonSmoke(1, 0);
    CreateCannonSmoke(2, 2);
    RequestScreenShake(4);
    PlaySfx(296);
}

void sub_080771b0(void)
{
    CreateCannonSmoke(2, 2);
    RequestScreenShake(4);
}

void sub_080771c4(void)
{
    CreateCannonSmoke(3, 3);
    RequestScreenShake(4);
}

void CannonFire(void)
{
    if (gCurTask->cannonRiderCount == gActivePlayerCount)
        CannonLaunchPlayers((s16)CannonPickLaunchState());
    else
    {
        CannonLaunchPlayers(3);
        ActorSetState(CANNON_STATE_LAUNCH_SHORT);
    }
    TaskSetEntry(CannonEnterState, gCurTaskIdx);
}

void CannonInit(void)
{
    gCurTask->updateCallback = (u32)CannonUpdate;
    ActorSetState(CANNON_STATE_WAIT);
    CallTableEntry(gCurTask->state, 4, gCannonStates);
}

void CannonUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 4, gCannonStateUpdates);
}

void CannonEnterState(void)
{
    CallTableEntry(gCurTask->state, 4, gCannonStates);
}

void CannonWait(void)
{
    gCurTask->updateState = CANNON_STATE_WAIT;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->cannonRiderCount = 0;
        t->cannonRiderMask = 0;
        t->frame = 0;
    }
    TaskSleepForever();
}

void CannonWaitUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
    {
        if (gCannonFuseState == 1)
        {
            CannonFire();
        }
        else
        {
            struct Task *t = gCurTask;

            if (t->cannonLoadTimer <= 0)
            {
                if (ActorCheckHits())
                    CannonLoadPlayer(gCurTask->hitterSlot);
            }
            else
            {
                t->cannonLoadTimer--;
            }
        }
    }
}

void CannonLaunchShort(void)
{
    gCurTask->updateState = CANNON_STATE_LAUNCH_SHORT;
    TaskStop();
    sub_0807717c();
    gCurTask->frame = 12;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gCurTask;

        t->frame = 0;
        t->velY = -0x60000;
    }
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x30000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x60000;
    TaskYieldTrampoline(2);
    gCurTask->velY = -0x20000;
    TaskYieldTrampoline(2);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    gCannonFuseState = 0;
    ActorSetState(CANNON_STATE_WAIT);
    TaskSleepForever();
}

void CannonLaunchShortUpdate(void)
{
    if (gCurTask->state != CANNON_STATE_LAUNCH_SHORT)
        TaskSetEntry(CannonEnterState, gCurTaskIdx);
}

void CannonLaunchUp(void)
{
    gCurTask->updateState = CANNON_STATE_LAUNCH_UP;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velY = -0x24000;
        t->accelY = 0x4000;
        t->frame = 1;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
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
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 12;
    TaskYieldTrampoline(8);
    PlaySfx(225);
    sub_080771b0();
    {
        struct Task *t = gCurTask;

        t->frame = 0;
        t->cannonLoopCount = 0;
    }
    gCurTask->cannonLoopCount = 0;
    do
    {
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->cannonLoopCount++;
    } while ((s16)gCurTask->cannonLoopCount <= 3);
    gCurTask->cannonLoopCount = 0;
    do
    {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->cannonLoopCount++;
    } while ((s16)gCurTask->cannonLoopCount <= 3);
    gCurTask->cannonLoopCount = 0;
    do
    {
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->cannonLoopCount++;
    } while ((s16)gCurTask->cannonLoopCount <= 3);
    TaskStop();
    TaskSleepForever();
}

void CannonLaunchUpUpdate(void)
{
}

void CannonLaunchUpRight(void)
{
    gCurTask->updateState = CANNON_STATE_LAUNCH_UP_RIGHT;
    TaskStop();
    {
        struct Task *t = gCurTask;

        t->velY = -0x24000;
        t->accelY = 0x4000;
        t->frame = 1;
    }
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame = 0;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->frame = 14;
    TaskYieldTrampoline(8);
    PlaySfx(225);
    sub_080771c4();
    {
        struct Task *t = gCurTask;

        t->frame--;
        t->cannonLoopCount = 0;
    }
    gCurTask->cannonLoopCount = 0;
    do
    {
        gCurTask->velY = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->cannonLoopCount++;
    } while ((s16)gCurTask->cannonLoopCount <= 3);
    gCurTask->cannonLoopCount = 0;
    do
    {
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->cannonLoopCount++;
    } while ((s16)gCurTask->cannonLoopCount <= 3);
    gCurTask->cannonLoopCount = 0;
    do
    {
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->velY = -0x8000;
        TaskYieldTrampoline(2);
        gCurTask->cannonLoopCount++;
    } while ((s16)gCurTask->cannonLoopCount <= 3);
    TaskStop();
    TaskSleepForever();
}

void CannonLaunchUpRightUpdate(void)
{
}

void Task_CannonFuse(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 13;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gCannonFuseFrames;
        CallTableEntry(t->variant, 1, gCannonFuseVariants);
    }
}

void CannonFuseInitBurn(void)
{
    struct Task *t = gCurTask;

    t->frame = 0xFFFF;
    t->cannonFuseStepTimer = 0;
    t->cannonFusePieceKind = 0;
    t->cannonFuseFrameStep = 3;
    t->cannonFuseExit = 0;
}

s32 CannonFuseGetPieceFrame(struct CannonFusePiece *p)
{
    struct Task *t = gCurTask;
    u8 *q;
    s32 i;
    u16 v;

    if (t->cannonFuseBurnDir == 1)
    {
        if (t->cannonFuseExit != 0)
        {
            i = t->cannonFuseFrameStep;
            i *= 2;
            q = (u8 *)p + 18;
            q += i;
            v = *(vu16 *)q;
            return (s16)v;
        }
    }
    else
    {
        if (t->cannonFuseExit == 0)
        {
            i = t->cannonFuseFrameStep;
            i *= 2;
            q = (u8 *)p + 18;
            q += i;
            v = *(vu16 *)q;
            return (s16)v;
        }
    }
    i = t->cannonFuseFrameStep;
    i *= 2;
    q = (u8 *)p + 6;
    q += i;
    v = *(vu16 *)q;
    return (s16)v;
}

void CannonFuseEnterPiece(s32 x, s32 y, s32 d)
{
    struct CannonFusePiece *p = gCannonFusePieces[gCurTask->cannonFusePieceKind];
    struct Task *t = gCurTask;

    {
        s32 k = d + t->cannonFusePieceKind * 4;

        t->cannonFuseExit = gUnk_087401CC[k];
    }
    if (t->cannonFuseBurnDir == 1)
    {
        sub_080269d8(x, y, 0);
        {
            struct Task *u = gCurTask;

            u->cannonFuseFrameStep = 0;
            u->frame = p->unk04;
        }
    }
    else
    {
        sub_080269d8(t->pixelX, t->pixelY, 1);
        {
            struct Task *u = gCurTask;

            u->cannonFuseFrameStep = p->unk00[2] - 1;
            u->frame = 0xFFFF;
        }
    }
}

void CannonFuseBurnStep(void)
{
    struct CannonFusePiece *p = gCannonFusePieces[gCurTask->cannonFusePieceKind];
    struct Task *t = gCurTask;

    if (t->cannonFuseStepTimer <= 0)
    {
        gCurTask->frame = CannonFuseGetPieceFrame(p);
        if (gCurTask->frame != -1)
        {
            struct Task *u = gCurTask;

            u->cannonFuseFrameStep++;
            CannonFuseMoveSpark();
        }
        else
        {
            CannonFuseStepCell(p);
        }
        gCurTask->cannonFuseStepTimer = 2;
    }
    gCurTask->cannonFuseStepTimer--;
}

void CannonFuseStepCell(struct CannonFusePiece *p)
{
    struct Task *t = gCurTask;
    u8 d = p->unk00[t->cannonFuseExit];
    s32 x = t->pixelX;
    s32 y = t->pixelY;
    s32 id;

    switch (d)
    {
    case 0:
        y -= 16;
        break;
    case 1:
        y += 16;
        break;
    case 2:
        x -= 16;
        break;
    case 3:
        x += 16;
        break;
    }
    id = sub_08022540(x, y);
    gCurTask->cannonFusePieceKind = id;
    if (id != -1)
    {
        CannonFuseEnterPiece(x, y, d);
    }
    else
    {
        struct Task *u = gCurTask;

        if (u->cannonFuseBurnDir == 1)
        {
            gCannonFuseState = u->cannonFuseBurnDir;
            TaskFree(u->cannonFuseSparkSlot);
        }
        else
        {
            sub_080269d8(u->pixelX, u->pixelY, 1);
            gCannonFuseState = id;
            gCurTask->frame = 42;
            ActorSetState(CANNON_FUSE_STATE_WAIT);
        }
    }
    {
        struct Task *v = gCurTask;

        v->pixelX = x;
        v->pixelY = y;
        v->posX = v->pixelX << 16;
        v->posY = v->pixelY << 16;
    }
}

void CannonFuseRestoreStep(void)
{
    struct CannonFusePiece *p = gCannonFusePieces[gCurTask->cannonFusePieceKind];
    struct Task *t = gCurTask;

    if (t->cannonFuseStepTimer <= 0)
    {
        if (t->cannonFuseFrameStep < 0)
        {
            CannonFuseStepCell(p);
        }
        else
        {
            struct Task *u;

            gCurTask->frame = CannonFuseGetPieceFrame(p);
            u = gCurTask;
            u->cannonFuseFrameStep--;
        }
        gCurTask->cannonFuseStepTimer = 2;
    }
    gCurTask->cannonFuseStepTimer--;
}

void CannonFuseMoveSpark(void)
{
    struct Task *t = gCurTask;
    s32 i = t->frame;
    s32 j = i * 2;
    struct Task *u = &gTasks[t->cannonFuseSparkSlot];

    u->pixelX = t->pixelX + gUnk_08740124[i * 2];
    u->pixelY = t->pixelY + gUnk_08740124[j + 1];
    u->posX = u->pixelX << 16;
    u->posY = u->pixelY << 16;
}

void CreateCannonFuseSpark(void)
{
    gCurTask->cannonFuseSparkSlot = CreateChildTaskHere(TASK_CANNON_FUSE_SPARK, 1);
}

void CannonFuseInit(void)
{
    struct Task *t = gCurTask;

    t->updateCallback = (u32)CannonFuseUpdate;
    t->cannonFuseStartX = t->pixelX;
    t->cannonFuseStartY = t->pixelY;
    ActorSetState(CANNON_FUSE_STATE_WAIT);
    CallTableEntry(gCurTask->state, 3, gCannonFuseStates);
}

void CannonFuseUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 3, gCannonFuseStateUpdates);
}

void CannonFuseEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gCannonFuseStates);
}
