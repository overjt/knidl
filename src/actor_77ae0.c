/* game_code_and_rodata 0x08077AE0-0x08078B68 (issue #79, module M19 batch 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08077AE0 0x08078B68 src/actor_77ae0.c --newpb
 *
 * M19 batch 5: task types #77 (Task_BigSwitch), #78 (Task_Stake), #79
 * (Task_RoomParticles, the credits particle system over gRoomParticles) and #8
 * (Task_WaddleDee), whose states hand off to module M20.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "cutscene.h"
#include "collision.h"
#include "room.h"
#include "actor.h"
#include "enemy.h"

/* callees */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 PlaySfx(u32 a);
extern u32 RandomRange(u32 range);
extern u32 ActorCheckHits(void);
extern u32 ActorCollideTerrain(void);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *a, u32 i);
extern void RequestScreenShake(u32 a);
extern void ActorSetState(u8 v);
extern void ActorSetStateSlot(u32 i, u8 v);
extern void ActorDropParasol(u32 *p, s32 b);
extern void ActorDropParasolOnLanding(u32 *p);

void CannonFuseWait(void)
{
    gCurTask->updateState = CANNON_FUSE_STATE_WAIT;
    gCurTask->facing = 1;
    TaskStop();
    gCannonFuseState = -1;
    {
        struct Task *t = gCurTask;

        t->posX = t->cannonFuseStartX << 16;
        t->posY = t->cannonFuseStartY << 16;
        t->frame = 42;
    }
    TaskSleepForever();
}

void CannonFuseWaitUpdate(void)
{
    if (ActorCheckHits())
    {
        gCannonFuseState = 0;
        gCurTask->cannonFuseBurnDir = 1;
        ActorSetState(CANNON_FUSE_STATE_BURN);
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
    }
}

void CannonFuseBurn(void)
{
    gCurTask->updateState = CANNON_FUSE_STATE_BURN;
    TaskStop();
    CannonFuseInitBurn();
    CreateCannonFuseSpark();
    while (gCannonFuseState == 0)
    {
        PlaySfx(230);
        TaskYieldTrampoline(3);
    }
    TaskSleepForever();
}

void CannonFuseBurnUpdate(void)
{
    if (gCurTask->cannonFusePieceKind != -1)
    {
        CannonFuseBurnStep();
    }
    else if (gCannonFuseState == 0)
    {
        ActorSetState(CANNON_FUSE_STATE_2);
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
    }
}

void CannonFuseRestore(void)
{
    gCurTask->updateState = CANNON_FUSE_STATE_2;
    TaskStop();
    gCurTask->cannonFuseBurnDir = -1;
    CannonFuseInitBurn();
    {
        struct Task *t = gCurTask;

        t->pixelY += 16;
        t->posY = t->pixelY << 16;
    }
    TaskSleepForever();
}

void CannonFuseRestoreUpdate(void)
{
    if (gCurTask->cannonFusePieceKind != -1)
        CannonFuseRestoreStep();
    if (gCurTask->state != CANNON_FUSE_STATE_2)
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
}

void Task_BigSwitch(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gBigSwitchFrames;
        CallTableEntry(t->variant, 1, gBigSwitchVariants);
    }
}

s32 BigSwitchHitterCanPress(void)
{
    struct PlayerState *p = &gPlayerStates[gCurTask->hitterSlot];

    if (p->ability == ABILITY_MIKE && p->mode == 13)
        return 0;
    return 1;
}

void sub_08077cd4(void)
{
    gCurTask->frame = 1;
    FreezeStage(15);
    SetRoomUpdateFlags(2);
}

void BigSwitchStartPress(void)
{
    DisablePause();
    gBigSwitchPressActive = 1;
    PlaySfx(226);
    RequestScreenShake(4);
    sub_08077cd4();
    HudDeactivateAbilityPanel();
    ActorSetStateSlot(gCurTaskIdx, 1);
    TaskSetEntry(BigSwitchEnterState, gCurTaskIdx);
}

void BigSwitchStartRefill(s32 id)
{
    ActorSetStateSlot(id, 2);
    TaskSetEntry(BigSwitchEnterState, id);
}

void BigSwitchRefillHealth(void)
{
    u8 i;

    DisablePause();
    TaskYieldTrampoline(15);
    FadeInSfx(16);
    TaskYieldTrampoline(15);
    RequestScreenShake(4);
    gBigSwitchPressActive = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
        {
            u8 done;

            do
            {
                PlaySfx(221);
                done = HealPlayerStep(i);
                TaskYieldTrampoline(8);
            } while (done == 0);
        }
    }
    ThawStage();
    HudActivateAbilityPanel();
    EnablePause();
    ActorDie();
}

void BigSwitchInit(void)
{
    gCurTask->updateCallback = (u32)BigSwitchUpdate;
    ActorSetState(BIG_SWITCH_STATE_WAIT);
    CallTableEntry(gCurTask->state, 3, gBigSwitchStates);
}

void BigSwitchUpdate(void)
{
    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 3, gBigSwitchStateUpdates);
}

void BigSwitchEnterState(void)
{
    CallTableEntry(gCurTask->state, 3, gBigSwitchStates);
}

void BigSwitchWait(void)
{
    gCurTask->updateState = BIG_SWITCH_STATE_WAIT;
    gCurTask->facing = 1;
    TaskStop();
    gCurTask->frame = 0;
    TaskSleepForever();
}

void BigSwitchWaitUpdate(void)
{
    if (gBigSwitchPressActive == 0 && ActorCheckHits() && (u8)BigSwitchHitterCanPress())
        BigSwitchStartPress();
}

void BigSwitchPress(void)
{
    gCurTask->updateState = BIG_SWITCH_STATE_1;
    TaskYieldTrampoline(8);
    FadeOutSfx(16);
    PressBigSwitch(gCurTaskIdx);
    TaskSleepForever();
}

void BigSwitchPressUpdate(void)
{
}

void BigSwitchRefill(void)
{
    gCurTask->updateState = BIG_SWITCH_STATE_REFILL;
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    BigSwitchRefillHealth();
    TaskSleepForever();
}

void BigSwitchRefillUpdate(void)
{
}

void Task_Stake(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInView;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gStakeFrames;
        CallTableEntry(t->variant, 1, gStakeVariants);
    }
}

void StakeInit(void)
{
    gCurTask->updateCallback = (u32)StakeUpdate;
    ActorSetState(STAKE_STATE_0);
    CallTableEntry(gCurTask->state, 1, gStakeStates);
}

void StakeUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gStakeStateUpdates);
}

void StakeState0(void)
{
    gCurTask->updateState = STAKE_STATE_0;
    gCurTask->frame = 0;
    while (GetCollisionTileAtPixel(gCurTask->pixelX, gCurTask->pixelY) == 51)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(5);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame = 0xFFFF;
    ActorDestroy();
}

void StakeState0Update(void)
{
}

void RoomParticlesDrawFixed(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleDrawFixed(&gRoomParticles[gCurTask->roomParticlesIndex]);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 7);
}

void RoomParticlesDrawBelowLine(void)
{
    if (gSpriteCameraY > gCurTask->roomParticlesLineY)
    {
        for (gCurTask->roomParticlesIndex = 0;
             gCurTask->roomParticlesIndex < gCurTask->roomParticlesCount;
             gCurTask->roomParticlesIndex++)
        {
            struct RoomParticle *base = gRoomParticles;
            struct RoomParticle *p = &base[gCurTask->roomParticlesIndex];

            if (p->pixelY > 8)
                RoomParticleDrawScrolled(p);
        }
    }
    else
    {
        for (gCurTask->roomParticlesIndex = 0;
             gCurTask->roomParticlesIndex < gCurTask->roomParticlesCount;
             gCurTask->roomParticlesIndex++)
        {
            struct RoomParticle *base = gRoomParticles;
            struct RoomParticle *p = &base[gCurTask->roomParticlesIndex];

            if (p->pixelY > gCurTask->roomParticlesLineY - gSpriteCameraY)
                RoomParticleDrawScrolled(p);
        }
    }
}

void RoomParticlesDrawRepeated(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleDrawRepeated(&gRoomParticles[gCurTask->roomParticlesIndex]);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 2);
    gCurTask->roomParticlesIndex = 3;
    do
    {
        RoomParticleDrawRepeated(&gRoomParticles[gCurTask->roomParticlesIndex]);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 5);
}

void RoomParticleStepX(struct RoomParticle *p)
{
    p->animStep++;
    if (gUnk_08740320[p->animRow][p->animStep].frame == 255)
        p->animStep = 0;
    {
        s32 j = p->animStep * 4;
        s32 k = p->animRow * 96;

        p->pixelX += ((u8 *)gUnk_08740320)[j + k + 2];
    }
    if (p->pixelX > 240)
    {
        p->pixelX = 0;
        p->pixelY = RandomRange(132) + 8;
    }
}

void RoomParticleInit(struct RoomParticle *p, u8 a, u8 b)
{
    switch (a)
    {
    case 0:
    case 1:
        p->pixelX = RandomRange(64) + 100;
        break;
    case 2:
        p->pixelX = RandomRange(48) + 170;
        break;
    case 3:
        switch (b)
        {
        case 0:
            p->pixelX = RandomRange(48) + 120;
            break;
        case 1:
            p->pixelX = RandomRange(48) + 40;
            break;
        }
        break;
    }
    p->pixelY = RandomRange(132) + 8;
    p->animStep = RandomRange(10);
    p->animRow = gUnk_08740620[gCurTask->unk28];
}

void RoomParticleDrawFixed(struct RoomParticle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752DB8[gUnk_08740320[p->animRow][p->animStep].frame],
                 t->spriteFlags, t->tileWord, p->pixelX, p->pixelY);
}

void sub_08078258(struct RoomParticle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752E00[gUnk_087404A0[p->animRow][p->animStep].frame],
                 t->spriteFlags, t->tileWord, p->pixelX, p->pixelY);
}

void RoomParticleDrawScrolled(struct RoomParticle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752E00[gUnk_087404A0[p->animRow][p->animStep].frame],
                 t->spriteFlags, t->tileWord, p->pixelX - gSpriteCameraX, p->pixelY);
}

void RoomParticleDrawRepeated(struct RoomParticle *p)
{
    gCurTask->unk2C = 0;
    do
    {
        if (RoomParticleIsOnScreen(p->pixelX - gSpriteCameraX + gCurTask->unk2C * 192,
                         p->pixelY))
        {
            struct Task *t = gCurTask;

            QueueSprite(t->layer,
                         gUnk_08752E00[gUnk_087404A0[p->animRow][p->animStep].frame],
                         t->spriteFlags, t->tileWord,
                         p->pixelX - gSpriteCameraX + t->unk2C * 192,
                         p->pixelY);
        }
        gCurTask->unk2C++;
    } while (gCurTask->unk2C <= 4);
}

u8 RoomParticleIsOnScreen(s16 x, s16 y)
{
    u16 v = y;

    if ((u32)((x << 16) + 0x3F0000) > 0x16E0000)
        return 0;
    if ((s16)v <= -64)
        return 0;
    if ((s16)v > 223)
        return 0;
    return 1;
}

void RoomParticleStepY(struct RoomParticle *p, u8 a)
{
    p->animStep++;
    if (gUnk_087404A0[p->animRow][p->animStep].frame == 255)
        p->animStep = 0;
    switch (a)
    {
    case 0:
        {
            s32 i = p->animStep * 4 + p->animRow * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->pixelY -= q[i];
        }
        if (p->pixelY <= 7)
        {
            p->pixelX = RandomRange(250);
            p->pixelY = 160;
        }
        break;
    case 2:
        {
            s32 i = p->animStep * 4 + p->animRow * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->pixelY -= q[i];
        }
        if (p->pixelY <= 7)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->pixelX = v + (u8)gCurTask->pixelX;
            }
            p->pixelY = 160;
        }
        break;
    case 1:
        {
            s32 i = p->animStep * 4 + p->animRow * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->pixelY -= q[i];
        }
        if (p->pixelY <= 7)
        {
            {
                s32 v = RandomRange(64) + 224;

                p->pixelX = v + (u8)gCurTask->pixelX;
            }
            p->pixelY = 160;
        }
        break;
    case 3:
        {
            s32 i = p->animStep * 4 + p->animRow * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->pixelY -= q[i];
        }
        if (p->pixelY <= 7)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->pixelX = v + (u8)gCurTask->pixelX + 96;
            }
            p->pixelY = 160;
        }
        break;
    case 4:
        {
            s32 i = p->animStep * 4 + p->animRow * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->pixelY += q[i];
        }
        if (p->pixelY > 160)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->pixelX = v + (u8)gCurTask->pixelX;
            }
            p->pixelY = 0;
        }
        break;
    }
}

void Task_RoomParticles(void)
{
    switch (gCurTask->variant)
    {
    case 0:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)RoomParticlesDrawFixed;
            t->frameTable = gUnk_08752DB8;
        }
        break;
    case 1:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)RoomParticlesDrawBelowLine;
            t->frameTable = gUnk_08752E00;
        }
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)RoomParticlesDrawBelowLine;
            t->frameTable = gUnk_08752E00;
        }
        break;
    case 3:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)RoomParticlesDrawBelowLine;
            t->frameTable = gUnk_08752E00;
        }
        break;
    case 4:
        {
            struct Task *t = gCurTask;

            t->drawCallback = (u32)RoomParticlesDrawRepeated;
            t->frameTable = gUnk_08752E00;
        }
        break;
    default:
        sub_0806ee2c();
        break;
    }
    gCurTask->layer = 4;
    {
        struct Task *t = gCurTask;

        t->tileWord = 0xF000 | t->tileWord;
        CallTableEntry(t->variant, 5, gRoomParticlesVariants);
    }
}

void RoomParticlesVariant0(void)
{
    gCurTask->updateCallback = (u32)sub_080786b4;
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->roomParticlesIndex], 0, 0);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 7);
    TaskSleepForever();
}

void sub_080786b4(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleStepX(&gRoomParticles[gCurTask->roomParticlesIndex]);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 7);
}

void RoomParticlesVariant1(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_08078734;
        t->roomParticlesCount = 8;
        t->roomParticlesLineY = 0;
    }
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->roomParticlesIndex], 0, 0);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 7);
    TaskSleepForever();
}

void sub_08078734(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->roomParticlesIndex], 0);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 7);
}

void RoomParticlesVariant2(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_080787b8;
        t->roomParticlesCount = 3;
        t->roomParticlesLineY = 208;
    }
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->roomParticlesIndex], 2, 0);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 2);
    TaskSleepForever();
}

void sub_080787b8(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->roomParticlesIndex], 2);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 2);
}

void RoomParticlesVariant3(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_0807883c;
        t->roomParticlesCount = 4;
        t->roomParticlesLineY = 64;
    }
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->roomParticlesIndex], 1, 0);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 3);
    TaskSleepForever();
}

void sub_0807883c(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->roomParticlesIndex], 1);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 3);
}

void RoomParticlesVariant4(void)
{
    gCurTask->updateCallback = (u32)sub_080788e0;
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->roomParticlesIndex], 3, 0);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 2);
    gCurTask->roomParticlesIndex = 3;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->roomParticlesIndex], 3, 1);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 5);
    TaskSleepForever();
}

void sub_080788e0(void)
{
    gCurTask->roomParticlesIndex = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->roomParticlesIndex], 3);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 2);
    gCurTask->roomParticlesIndex = 3;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->roomParticlesIndex], 4);
        gCurTask->roomParticlesIndex++;
    } while (gCurTask->roomParticlesIndex <= 5);
}

void Task_WaddleDee(void)
{
    {
        struct Task *t = gCurTask;

        t->moveCallback = (u32)ActorMove;
        t->drawCallback = (u32)ActorDrawWorldInViewOrDestroy;
        t->layer = 11;
    }
    {
        struct Task *t = gCurTask;

        t->frameTable = gWaddleDeeFrames;
        t->u8C.actor->extraFrame = 4;
        CallTableEntry(t->variant, 6, gWaddleDeeVariants);
    }
}

s32 ParasolWaddleDeeReactToDefeat(void)
{
    ActorDropParasol(gWaddleDeeDef, 0);
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 WaddleDeeStartFall(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(1);
        TaskSetEntry(WaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        ActorSetState(1);
        TaskSetEntry(WaddleDeePaceEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        ActorSetState(2);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        ActorSetState(1);
        TaskSetEntry(ParasolWaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        ActorSetState(1);
        TaskSetEntry(ParasolWaddleDeeStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 WaddleDeeLand(void)
{
    s32 r = 0;

    switch (gCurTask->variant)
    {
    case 0:
        ActorSetState(0);
        TaskSetEntry(WaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        ActorSetState(0);
        TaskSetEntry(WaddleDeePaceEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        {
            struct Task *t = gCurTask;

            if (t->waddleDeeJumpTimer <= 0)
                t->waddleDeeJumpTimer = 30;
        }
        gCurTask->actorAnimDelay34 = ActorStartAnim(gWaddleDeeJumpAnim);
        ActorSetState(0);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        ActorDropParasolOnLanding(gWaddleDeeDef);
        ActorSetState(0);
        TaskSetEntry(ParasolWaddleDeeWalkEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        ActorSetState(0);
        TaskSetEntry(ParasolWaddleDeeStandEnterState, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 WaddleDeeEnterWater(void)
{
    u8 v = gCurTask->variant;

    if (v == 3 || v == 5)
        ActorDropParasol(gWaddleDeeDef, 0);
    ActorStartDrown(-2);
    return 1;
}

s32 WaddleDeeHitWall(void)
{
    struct Task *t = gCurTask;

    if (t->variant == 3 && t->state == 1)
        TaskBounceParasolDriftOffWall();
    else
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 WaddleDeeHitCeiling(void)
{
    return 0;
}
