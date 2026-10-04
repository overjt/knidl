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
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066c3c(u32 *p);

void CannonFuseWait(void)
{
    gCurTask->updateState = 0;
    gCurTask->facing = 1;
    TaskStop();
    gCannonFuseState = -1;
    {
        struct Task *t = gCurTask;

        t->posX = t->unk30 << 16;
        t->posY = t->unk2C << 16;
        t->frame = 42;
    }
    TaskSleepForever();
}

void CannonFuseWaitUpdate(void)
{
    if (ActorCheckHits())
    {
        gCannonFuseState = 0;
        gCurTask->unk24 = 1;
        ActorSetState(1);
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
    }
}

void CannonFuseBurn(void)
{
    gCurTask->updateState = 1;
    TaskStop();
    sub_0807775c();
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
    if (gCurTask->unk20 != -1)
    {
        CannonFuseBurnStep();
    }
    else if (gCannonFuseState == 0)
    {
        ActorSetState(2);
        TaskSetEntry(CannonFuseEnterState, gCurTaskIdx);
    }
}

void CannonFuseState2(void)
{
    gCurTask->updateState = 2;
    TaskStop();
    gCurTask->unk24 = -1;
    sub_0807775c();
    {
        struct Task *t = gCurTask;

        t->pixelY += 16;
        t->posY = t->pixelY << 16;
    }
    TaskSleepForever();
}

void CannonFuseState2Update(void)
{
    if (gCurTask->unk20 != -1)
        sub_08077980();
    if (gCurTask->state != 2)
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

s32 sub_08077ca4(void)
{
    struct PlayerState *p = &gPlayerStates[gCurTask->hitterSlot];

    if (p->ability == 7 && p->mode == 13)
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
    ActorSetState(0);
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
    gCurTask->updateState = 0;
    gCurTask->facing = 1;
    TaskStop();
    gCurTask->frame = 0;
    TaskSleepForever();
}

void BigSwitchWaitUpdate(void)
{
    if (gBigSwitchPressActive == 0 && ActorCheckHits() && (u8)sub_08077ca4())
        BigSwitchStartPress();
}

void BigSwitchState1(void)
{
    gCurTask->updateState = 1;
    TaskYieldTrampoline(8);
    FadeOutSfx(16);
    PressBigSwitch(gCurTaskIdx);
    TaskSleepForever();
}

void BigSwitchState1Update(void)
{
}

void BigSwitchRefill(void)
{
    gCurTask->updateState = 2;
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
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 1, gStakeStates);
}

void StakeUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 1, gStakeStateUpdates);
}

void StakeState0(void)
{
    gCurTask->updateState = 0;
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
    gCurTask->unk28 = 0;
    do
    {
        sub_080781fc(&gRoomParticles[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
}

void RoomParticlesDrawBelowLine(void)
{
    if (gSpriteCameraY > gCurTask->unk34)
    {
        for (gCurTask->unk28 = 0;
             gCurTask->unk28 < gCurTask->unk30;
             gCurTask->unk28++)
        {
            struct M19Particle *base = gRoomParticles;
            struct M19Particle *p = &base[gCurTask->unk28];

            if (p->unk03 > 8)
                sub_080782b4(p);
        }
    }
    else
    {
        for (gCurTask->unk28 = 0;
             gCurTask->unk28 < gCurTask->unk30;
             gCurTask->unk28++)
        {
            struct M19Particle *base = gRoomParticles;
            struct M19Particle *p = &base[gCurTask->unk28];

            if (p->unk03 > gCurTask->unk34 - gSpriteCameraY)
                sub_080782b4(p);
        }
    }
}

void RoomParticlesDrawRepeated(void)
{
    gCurTask->unk28 = 0;
    do
    {
        sub_0807831c(&gRoomParticles[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    gCurTask->unk28 = 3;
    do
    {
        sub_0807831c(&gRoomParticles[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 5);
}

void RoomParticleStepX(struct M19Particle *p)
{
    p->unk01++;
    if (gUnk_08740320[p->unk00][p->unk01].unk00 == 255)
        p->unk01 = 0;
    {
        s32 j = p->unk01 * 4;
        s32 k = p->unk00 * 96;

        p->unk02 += ((u8 *)gUnk_08740320)[j + k + 2];
    }
    if (p->unk02 > 240)
    {
        p->unk02 = 0;
        p->unk03 = RandomRange(132) + 8;
    }
}

void RoomParticleInit(struct M19Particle *p, u8 a, u8 b)
{
    switch (a)
    {
    case 0:
    case 1:
        p->unk02 = RandomRange(64) + 100;
        break;
    case 2:
        p->unk02 = RandomRange(48) + 170;
        break;
    case 3:
        switch (b)
        {
        case 0:
            p->unk02 = RandomRange(48) + 120;
            break;
        case 1:
            p->unk02 = RandomRange(48) + 40;
            break;
        }
        break;
    }
    p->unk03 = RandomRange(132) + 8;
    p->unk01 = RandomRange(10);
    p->unk00 = gUnk_08740620[gCurTask->unk28];
}

void sub_080781fc(struct M19Particle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752DB8[gUnk_08740320[p->unk00][p->unk01].unk00],
                 t->spriteFlags, t->tileWord, p->unk02, p->unk03);
}

void sub_08078258(struct M19Particle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                 t->spriteFlags, t->tileWord, p->unk02, p->unk03);
}

void sub_080782b4(struct M19Particle *p)
{
    struct Task *t = gCurTask;

    QueueSprite(t->layer,
                 gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                 t->spriteFlags, t->tileWord, p->unk02 - gSpriteCameraX, p->unk03);
}

void sub_0807831c(struct M19Particle *p)
{
    gCurTask->unk2C = 0;
    do
    {
        if (sub_080783e0(p->unk02 - gSpriteCameraX + gCurTask->unk2C * 192,
                         p->unk03))
        {
            struct Task *t = gCurTask;

            QueueSprite(t->layer,
                         gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                         t->spriteFlags, t->tileWord,
                         p->unk02 - gSpriteCameraX + t->unk2C * 192,
                         p->unk03);
        }
        gCurTask->unk2C++;
    } while (gCurTask->unk2C <= 4);
}

u8 sub_080783e0(s16 x, s16 y)
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

void RoomParticleStepY(struct M19Particle *p, u8 a)
{
    p->unk01++;
    if (gUnk_087404A0[p->unk00][p->unk01].unk00 == 255)
        p->unk01 = 0;
    switch (a)
    {
    case 0:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            p->unk02 = RandomRange(250);
            p->unk03 = 160;
        }
        break;
    case 2:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->unk02 = v + (u8)gCurTask->pixelX;
            }
            p->unk03 = 160;
        }
        break;
    case 1:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = RandomRange(64) + 224;

                p->unk02 = v + (u8)gCurTask->pixelX;
            }
            p->unk03 = 160;
        }
        break;
    case 3:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 -= q[i];
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->unk02 = v + (u8)gCurTask->pixelX + 96;
            }
            p->unk03 = 160;
        }
        break;
    case 4:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;
            u8 *q = (u8 *)gUnk_087404A0;

            q += 3;
            p->unk03 += q[i];
        }
        if (p->unk03 > 160)
        {
            {
                s32 v = RandomRange(48) + 232;

                p->unk02 = v + (u8)gCurTask->pixelX;
            }
            p->unk03 = 0;
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
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->unk28], 0, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
    TaskSleepForever();
}

void sub_080786b4(void)
{
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleStepX(&gRoomParticles[gCurTask->unk28]);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
}

void RoomParticlesVariant1(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_08078734;
        t->unk30 = 8;
        t->unk34 = 0;
    }
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->unk28], 0, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
    TaskSleepForever();
}

void sub_08078734(void)
{
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->unk28], 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 7);
}

void RoomParticlesVariant2(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_080787b8;
        t->unk30 = 3;
        t->unk34 = 208;
    }
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->unk28], 2, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    TaskSleepForever();
}

void sub_080787b8(void)
{
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->unk28], 2);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
}

void RoomParticlesVariant3(void)
{
    {
        struct Task *t = gCurTask;

        t->updateCallback = (u32)sub_0807883c;
        t->unk30 = 4;
        t->unk34 = 64;
    }
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->unk28], 1, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 3);
    TaskSleepForever();
}

void sub_0807883c(void)
{
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->unk28], 1);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 3);
}

void RoomParticlesVariant4(void)
{
    gCurTask->updateCallback = (u32)sub_080788e0;
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->unk28], 3, 0);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    gCurTask->unk28 = 3;
    do
    {
        RoomParticleInit(&gRoomParticles[gCurTask->unk28], 3, 1);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 5);
    TaskSleepForever();
}

void sub_080788e0(void)
{
    gCurTask->unk28 = 0;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->unk28], 3);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 2);
    gCurTask->unk28 = 3;
    do
    {
        RoomParticleStepY(&gRoomParticles[gCurTask->unk28], 4);
        gCurTask->unk28++;
    } while (gCurTask->unk28 <= 5);
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
    sub_08066c08(gWaddleDeeDef, 0);
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

            if (t->unk28 <= 0)
                t->unk28 = 30;
        }
        gCurTask->unk34 = ActorStartAnim(gUnk_087406A0);
        ActorSetState(0);
        TaskSetEntry(WaddleDeeJumpEnterState, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        sub_08066c3c(gWaddleDeeDef);
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
        sub_08066c08(gWaddleDeeDef, 0);
    ActorStartDrown(-2);
    return 1;
}

s32 WaddleDeeHitWall(void)
{
    struct Task *t = gCurTask;

    if (t->variant == 3 && t->state == 1)
        sub_08066b70();
    else
        TaskTurnAroundAndReverseX();
    return 0;
}

s32 WaddleDeeHitCeiling(void)
{
    return 0;
}
