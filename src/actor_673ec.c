/* game_code_and_rodata 0x080673EC-0x080692FC (issue #65, module M17 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080673EC 0x080692FC src/actor_673ec.c --newpb
 *
 * Task bodies for the carried/helper actor states (unk15 = 3..10) plus the
 * player-record plumbing: spawn/teardown (HoldPlayer/DropHeldPlayer), the
 * per-character animation-offset switch (HeldPlayerAddAbilityFrameOffset), and the
 * gPlayerStates[] save/restore used when a player is picked up or dropped.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "actor.h"

/* Not from collision.h: this file's view of gAttackBox differs (lesson
   3.517). */
extern u16 gAttackX;
extern u16 gAttackY;
extern u16 gAttackHealth;
extern u8 gAttackLastHitterSlot;
extern u8 gAttackLastHitter;
extern s16 gAttackFacing;
extern s8 gAttackHitDuration;
extern u8 gAttackLastHitterClass;
extern s32 gAttackBox;
extern u8 gHitKind;
extern u8 gHitEffect;
extern u16 gHitHealthLeft;
extern u8 gHitDirection;
extern u8 gHitTimer;
extern u8 gHitterSlot;
extern u8 gUnk_03001F24;
extern u8 gHitterColliderClass;
extern u8 gHitterColliderKind;

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void PlaceAttackBox(void);
extern u8 HitTestColliderClass20(void);
extern u8 HitTestColliderClass10(void);
extern u8 HitTestPlayerColliders(void);
extern void PlaySfx(u32 a);
extern u8 gTerrainResult;
extern void ClampTaskToRoom(struct Task *t);
extern void TerrainCollideBox(u32 *p);
extern void RequestScreenShake(u32 a);
extern void TaskSetEntry(void *fn, s32 i);

void HeldPlayerEnterState(void)
{
    CallTableEntry(gCurTask->state, 11, gHeldPlayerStates);
}

void HeldPlayerUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 11, gHeldPlayerStateUpdates);
    PlayerUpdateInvulnerability();
    if ((gCurTask->player->unk42 & 32) == 0)
        sub_0803e080();
    if (gLocalPlayer == gCurTask->player->playerIndex)
        SetCameraFocus(gCurTask->pixelX, gCurTask->pixelY);
}

void HeldPlayerSwallow(void)
{
    gCurTask->updateState = 0;
    sub_08068690();
    while (HeldPlayerIsNearCaptor() == 0)
    {
        HeldPlayerPullTowardCaptor();
        TaskYieldTrampoline(1);
    }
    TaskStop();
    HeldPlayerEnterMouth();
    TaskSleepForever();
}

void HeldPlayerSwallowUpdate(void)
{
    struct Task *t;

    if (gMetaKnightmareMode != 0)
        return;
    t = gCurTask;
    if (t->playerHeldSwallowFrameCount <= 0)
        return;
    if (t->frame == -1)
        return;
    sub_080687fc();
}

void HeldPlayerSpitFlight(void)
{
    gCurTask->updateState = 1;
    sub_08068828();
    TaskSleepForever();
}

void HeldPlayerSpitFlightUpdate(void)
{
}

void HeldPlayerSpitBounceOff(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->updateState = 2;
    TaskStop();
    sub_08068840();
    TaskSleepForever();
}

void HeldPlayerSpitBounceOffUpdate(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->playerHeldSpitTimer <= 0)
    {
        TaskStop();
        DropHeldPlayer(gCurTaskIdx);
    }
    else
    {
        t->playerHeldSpitTimer--;
        sub_0806896c();
    }
}

void HeldPlayerBackdropHeld(void)
{
    struct Task *u;
    struct PlayerState *p;

    gCurTask->updateState = 3;
    gCurTask->facing = gTasks[gCurTask->parent].facing;
    if (gMetaKnightmareMode == 0)
    {
        u = gCurTask;
        p = u->player;
        if (p->mouthState == 1)
        {
            u->playerHeldBaseFrame = 0x11C1;
            u->playerHeldPoseFrameOffset = 3;
        }
        else
        {
            u->playerHeldBaseFrame = 0x133;
            u->playerHeldPoseFrameOffset = -8;
        }
        TaskSetFrame(*(s16 *)&gCurTask->playerHeldBaseFrame);
        HeldPlayerAddAbilityFrameOffset();
    }
    else
    {
        TaskSetFrame(0x123B);
    }
    TaskSleepForever();
}
void HeldPlayerBackdropHeldUpdate(void)
{
    HeldPlayerFollowCaptorPose();
}

/* Follow the carried task's frame offsets out of gUnk_0873E1F8. */
void HeldPlayerFollowCaptorPose(void)
{
    struct Task *t;
    struct Task *u;
    s8 *p;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u16)(u->frame - 36) > 25)
        return;
    p = &gUnk_0873E1F8[u->frame * 4];
    t->pixelX = u->pixelX + *p * t->facing;
    p++;
    t->pixelY = *p + u->pixelY;
    p++;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    if (gMetaKnightmareMode == 0)
    {
        if (*p++ != 0)
            t->frame = t->playerHeldBaseFrame + t->playerHeldPoseFrameOffset;
        else
            t->frame = t->playerHeldBaseFrame;
        HeldPlayerAddAbilityFrameOffset();
    }
    else
    {
        if (*p++ != 0)
            t->frame = 0x1243;
        else
            TaskSetFrame(0x123B);
    }
    gCurTask->layer = *p;
}

/* Task body: the carried task's "thrown" arc. */
void HeldPlayerBackdropBounceOff(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    u = gCurTask;
    u->moveCallback = (u32)PlayerMove;
    u->updateState = 4;
    TaskStop();
    HeldPlayerDamage(-8, 512);
    if (gMetaKnightmareMode == 0)
        HeldPlayerFollowCaptorPose();
    t = gCurTask;
    t->unk28 = gTasks[t->parent].unk28;
    t->playerHeldBounceEnded = 0;
    if (t->health == 0)
        t->playerHeldBounceEnded = 1;
    if (gCurTask->unk28 != 3)
        TaskYieldTrampoline(28);
    else
        TaskYieldTrampoline(1);
    if (gCurTaskIdx == gLocalPlayer)
    {
        if (gMetaKnightmareMode == 0)
        {
            if (gCurTask->player->mouthState == 1)
                PlaySfx(158);
            else if (gFrameCount & 1)
                PlaySfx(111);
            else
                PlaySfx(112);
        }
        else
        {
            PlaySfx(0x107);
        }
    }
    SetPlayerInvulnerability(1, 96, gCurTaskIdx);
    v = gCurTask;
    if (v->unk28 == 2)
        v->facing = -v->facing;
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2500, 0x30000);
    if (gMetaKnightmareMode == 0 && gCurTask->player->mouthState == 1)
    {
        TaskSetFrame(0x11C2);
        TaskYieldTrampoline(3);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame--;
        TaskYieldTrampoline(3);
        gCurTask->frame--;
        TaskYieldTrampoline(5);
        gCurTask->frame++;
        TaskYieldTrampoline(3);
    }
    else
    {
        w = gCurTask;
        w->playerHeldFrameStep = w->facing;
        w->unk28 = 8;
        if (gMetaKnightmareMode == 0)
            TaskSetFrameFlip(0x133);
        else
            TaskSetFrame(0x123B);
        gCurTask->playerHeldLoopCount = 0;
        do
        {
            sub_0806896c();
            TaskYieldTrampoline(1);
        } while ((s16)(++gCurTask->playerHeldLoopCount) <= 29);
    }
    x = gCurTask;
    x->playerHeldBounceEnded = x->playerHeldBounceEnded + 1;
    TaskSleepForever();
}
void HeldPlayerBackdropBounceOffUpdate(void)
{
    ClampTaskToRoom(gCurTask);
    TerrainCollideBox(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
        gCurTask->velX = 0;
    if (gCurTask->playerHeldBounceEnded != 0)
    {
        TaskStop();
        DropHeldPlayer(gCurTaskIdx);
    }
}

/* Task body: the carried task wobbling in the player's hands. */
void HeldPlayerState5(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    gCurTask->updateState = 5;
    u = gCurTask;
    u->facing = gTasks[u->parent].facing;
    v = gCurTask;
    v->playerHeldWobbleStep = 0;
    v->layer = 7;
    TaskStop();
    t = gCurTask;
    t->posX = (t->facing * 3) << 19;
    t->posY = 0;
    while (gTasks[gCurTask->parent].frame == 34)
        TaskYieldTrampoline(1);
    gCurTask->layer = 12;
    if (gCurTask->player->mouthState == 1)
    {
        TaskSleepForever();
        return;
    }
    while (1)
    {
        TaskSetMotionXFacing(gUnk_0873E348[gCurTask->playerHeldWobbleStep], 0x5A5A5A5A);
        w = gCurTask;
        w->velY = gUnk_0873E388[w->playerHeldWobbleStep];
        TaskYieldTrampoline(2);
        x = gCurTask;
        x->playerHeldWobbleStep++;
        if (x->playerHeldWobbleStep > 14)
            x->playerHeldWobbleStep = 0;
    }
}
void HeldPlayerState5Update(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 i;

    if (gMetaKnightmareMode == 0)
    {
        t = gCurTask;
        p = t->player;
        if (p->mouthState == 1)
        {
            i = (s16)(gTasks[t->parent].frame - 30);
            switch (i)
            {
            case 1:
                TaskSetFrame(0x173);
                break;
            case 0:
            case 2:
                TaskSetFrame(370);
                break;
            case 3:
            case 4:
                TaskSetFrame(0x171);
                break;
            }
        }
        else
        {
            TaskSetFrame(0x12D);
            HeldPlayerAddAbilityFrameOffset();
        }
    }
    else
    {
        TaskSetFrame(0x1243);
        u = gCurTask;
        if (u->facing == 1)
            u->spriteFlags |= 0x8000;
        else
            u->spriteFlags &= 0x7FFF;
        TaskSleepForever();
    }
}
void HeldPlayerState6(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->updateState = 6;
    TaskStop();
    t = gCurTask;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    HeldPlayerDamage(-8, 512);
    t = gCurTask;
    t->playerHeldBounceEnded = 0;
    if (t->health == 0)
        t->playerHeldBounceEnded = 1;
    if (gMetaKnightmareMode == 0)
    {
        if (gLocalPlayer == gCurTaskIdx)
        {
            p = gCurTask->player;
            if (p->mouthState == 1)
                PlaySfx(158);
            else if (gFrameCount & 1)
                PlaySfx(111);
            else
                PlaySfx(112);
        }
    }
    else
    {
        PlaySfx(0x107);
    }
    SetPlayerInvulnerability(1, 96, gCurTaskIdx);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2500, 0x30000);
    if (gMetaKnightmareMode == 0)
    {
        t = gCurTask;
        p = t->player;
        if (p->mouthState == 1)
        {
            TaskSetFrame(370);
            TaskYieldTrampoline(3);
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(4);
            t = gCurTask;
            t->frame--;
            TaskYieldTrampoline(2);
            t = gCurTask;
            t->frame--;
            TaskYieldTrampoline(4);
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(2);
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(4);
            t = gCurTask;
            t->frame--;
            TaskYieldTrampoline(3);
            t = gCurTask;
            t->frame--;
            TaskYieldTrampoline(5);
            t = gCurTask;
            t->frame++;
            TaskYieldTrampoline(3);
        }
        else
        {
            t->playerHeldFrameStep = t->facing;
            t->playerHeldFrameTimer = 8;
            TaskSetFrameFlip(0x133);
            gCurTask->playerHeldLoopCount = 0;
            do
            {
                sub_0806896c();
                TaskYieldTrampoline(1);
                t = gCurTask;
                t->playerHeldLoopCount++;
            } while ((s16)t->playerHeldLoopCount <= 29);
        }
    }
    else
    {
        TaskSetFrame(0x123B);
        TaskYieldTrampoline(6);
        TaskSetFrame(0x1241);
        TaskYieldTrampoline(26);
    }
    t = gCurTask;
    t->playerHeldBounceEnded++;
    TaskSleepForever();
}

void HeldPlayerState6Update(void)
{
    ClampTaskToRoom(gCurTask);
    TerrainCollideBox(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
        gCurTask->velX = 0;
    if (gCurTask->playerHeldBounceEnded != 0)
    {
        TaskStop();
        DropHeldPlayer(gCurTaskIdx);
    }
}

void HeldPlayerThrowHeld(void)
{
    struct Task *t;

    gCurTask->updateState = 7;
    t = gCurTask;
    t->facing = gTasks[t->parent].facing;
    TaskSleepForever();
}

/* Track the carrier's frame through the 5-word gUnk_0873E3C8 table. */
void HeldPlayerThrowHeldUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 i;

    t = gCurTask;
    u = &gTasks[t->parent];
    i = (u->frame - 30) * 5;
    if (gMetaKnightmareMode == 0)
    {
        p = t->player;
        if (p->mouthState == 1)
            i += 75;
    }
    else
    {
        i += 150;
    }
    t = gCurTask;
    t->pixelX = u->pixelX + (u16)u->facing * gUnk_0873E3C8[i];
    t->pixelY = u->pixelY + gUnk_0873E3C8[i + 1];
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    t->facing = u->facing * (s16)gUnk_0873E3C8[i + 2];
    TaskSetFrame((s16)gUnk_0873E3C8[i + 3]);
    if (gMetaKnightmareMode == 0)
        HeldPlayerAddAbilityFrameOffset();
    gCurTask->layer = gUnk_0873E3C8[i + 4];
}

/* Task body: the carried task struggling in the player's hands. */
void HeldPlayerThrowFlightForward(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PlayerState *p;
    s32 f;

    gCurTask->updateState = 8;
    gCurTask->moveCallback = (u32)PlayerMove;
    gCurTask->layer = 7;
    gCurTask->facing = gTasks[gCurTask->parent].facing;
    t = gCurTask;
    u = &gTasks[t->parent];
    t->posX = (u->pixelX + (t->facing << 4)) << 16;
    t->posY = (gTasks[t->parent].pixelY - 24) << 16;
    TaskSetMotionXFacing(0x50000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x1200, 0x30000);
    if (gMetaKnightmareMode == 0)
    {
        p = gCurTask->player;
        if (p->mouthState == 1)
        {
            f = 370;
            while (1)
            {
                TaskSetFrame(0x171);
                TaskYieldTrampoline(4);
                TaskSetFrame(f);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x173);
                TaskYieldTrampoline(4);
                TaskSetFrame(f);
                TaskYieldTrampoline(2);
            }
        }
        while (1)
        {
            TaskSetFrame(0x135);
            HeldPlayerAddAbilityFrameOffset();
            TaskYieldTrampoline(1);
            gCurTask->playerHeldLoopCount = 0;
            do
            {
                w = gCurTask;
                w->frame--;
                TaskYieldTrampoline(1);
                v = gCurTask;
                v->playerHeldLoopCount++;
            } while ((s16)v->playerHeldLoopCount <= 14);
        }
    }
    TaskSetFrame(0x123C);
    x = gCurTask;
    if (x->facing == 1)
        x->spriteFlags |= 0x8000;
    else
        x->spriteFlags &= 0x7FFF;
    TaskSleepForever();
    TaskSleepForever();
}

void HeldPlayerThrowFlightForwardUpdate(void)
{
    struct Task *t;

    TerrainCollideBox(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
    {
        CreateChildTaskHere(TASK_STAR_SCATTER, 0);
        PlaySfx(153);
        RequestScreenShake(2);
        t = gCurTask;
        t->facing = -t->facing;
        TaskStop();
        TaskSetEntry(HeldPlayerThrowBounceOff, gCurTaskIdx);
    }
    else if (gCurTask->onGround & 1)
    {
        TaskStop();
        TaskSetEntry(HeldPlayerThrowBounceOff, gCurTaskIdx);
    }
}

/* Task body: the carried task struggling, mirrored variant. */
void HeldPlayerThrowFlightBackward(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 f;

    gCurTask->updateState = 9;
    gCurTask->moveCallback = (u32)PlayerMove;
    gCurTask->layer = 7;
    gCurTask->facing = -gTasks[gCurTask->parent].facing;
    u = gCurTask;
    u->posX = gTasks[u->parent].pixelX << 16;
    u->posY = (gTasks[u->parent].pixelY - 40) << 16;
    TaskSetMotionXFacing(0x50000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x1200, 0x30000);
    if (gMetaKnightmareMode == 0)
    {
        p = gCurTask->player;
        if (p->mouthState == 1)
        {
            f = 370;
            while (1)
            {
                TaskSetFrame(0x171);
                TaskYieldTrampoline(4);
                TaskSetFrame(f);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x173);
                TaskYieldTrampoline(4);
                TaskSetFrame(f);
                TaskYieldTrampoline(2);
            }
        }
        while (1)
        {
            TaskSetFrame(0x135);
            HeldPlayerAddAbilityFrameOffset();
            TaskYieldTrampoline(1);
            gCurTask->playerHeldLoopCount = 0;
            do
            {
                gCurTask->frame--;
                TaskYieldTrampoline(1);
            } while ((s16)(++gCurTask->playerHeldLoopCount) <= 14);
        }
    }
    TaskSetFrame(0x123C);
    t = gCurTask;
    if (t->facing == 1)
        t->spriteFlags |= 0x8000;
    else
        t->spriteFlags &= 0x7FFF;
    TaskSleepForever();
    TaskSleepForever();
}

void HeldPlayerThrowFlightBackwardUpdate(void)
{
    struct Task *t;

    TerrainCollideBox(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
    {
        CreateChildTaskHere(TASK_STAR_SCATTER, 0);
        PlaySfx(153);
        RequestScreenShake(2);
        t = gCurTask;
        t->facing = -t->facing;
        TaskStop();
        TaskSetEntry(HeldPlayerThrowBounceOff, gCurTaskIdx);
    }
    else if (gCurTask->onGround & 1)
    {
        TaskStop();
        TaskSetEntry(HeldPlayerThrowBounceOff, gCurTaskIdx);
    }
}
void HeldPlayerThrowBounceOff(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->updateState = 10;
    TaskStop();
    HeldPlayerDamage(-8, 512);
    u = gCurTask;
    u->playerHeldBounceEnded = 0;
    if (u->health == 0)
        u->playerHeldBounceEnded = 1;
    if (gMetaKnightmareMode == 0)
    {
        if (gLocalPlayer == gCurTaskIdx)
        {
            if (gCurTask->player->mouthState == 1)
                PlaySfx(158);
            else if (gFrameCount & 1)
                PlaySfx(111);
            else
                PlaySfx(112);
        }
    }
    else
    {
        PlaySfx(0x107);
    }
    SetPlayerInvulnerability(1, 96, gCurTaskIdx);
    gCurTask->onGround = 0;
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2500, 0x30000);
    if (gMetaKnightmareMode == 0)
    {
        if (gCurTask->player->mouthState == 1)
        {
            TaskSetFrame(370);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
        }
        else
        {
            v = gCurTask;
            v->playerHeldFrameStep = v->facing;
            v->playerHeldFrameTimer = 8;
            TaskSetFrameFlip(0x133);
            gCurTask->playerHeldLoopCount = 0;
            do
            {
                sub_0806896c();
                TaskYieldTrampoline(1);
            } while ((s16)(++gCurTask->playerHeldLoopCount) <= 14);
        }
    }
    else
    {
        TaskSetFrame(0x123B);
        TaskYieldTrampoline(6);
        TaskSetFrame(0x1241);
        TaskYieldTrampoline(7);
    }
    w = gCurTask;
    w->playerHeldBounceEnded = w->playerHeldBounceEnded + 1;
    TaskSleepForever();
}
void HeldPlayerThrowBounceOffUpdate(void)
{
    ClampTaskToRoom(gCurTask);
    TerrainCollideBox(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
        gCurTask->velX = 0;
    if (gCurTask->playerHeldBounceEnded != 0)
        DropHeldPlayer(gCurTaskIdx);
}
s32 HeldPlayerAddAbilityFrameOffset(void)
{
    s32 d;

    switch (gCurTask->player->ability)
    {
    case ABILITY_NORMAL:
    case ABILITY_MIKE:
    case ABILITY_SLEEP:
    case ABILITY_CRASH:
    case ABILITY_LIGHT:
    case ABILITY_UFO:
        d = 0;
        break;
    case ABILITY_FIRE:
        d = 0x143;
        break;
    case ABILITY_SPARK:
        d = 0x234;
        break;
    case ABILITY_CUTTER:
        d = 0x2B3;
        break;
    case ABILITY_SWORD:
        d = 0x384;
        break;
    case ABILITY_BURNING:
        d = 0x495;
        break;
    case ABILITY_LASER:
        d = 0x522;
        break;
    case ABILITY_WHEEL:
        d = 0x5CB;
        break;
    case ABILITY_HAMMER:
        d = 0x69C;
        break;
    case ABILITY_PARASOL:
        d = 0x79C;
        break;
    case ABILITY_NEEDLE:
        d = 0x838;
        break;
    case ABILITY_ICE:
        d = 0x8C4;
        break;
    case ABILITY_FREEZE:
        d = 0x950;
        break;
    case ABILITY_HI_JUMP:
        d = 0x9F5;
        break;
    case ABILITY_BEAM:
        d = 0xA87;
        break;
    case ABILITY_STONE:
        d = 0xB0A;
        break;
    case ABILITY_BALL:
        d = 0xB94;
        break;
    case ABILITY_TORNADO:
        d = 0xC8F;
        break;
    case ABILITY_BACKDROP:
        d = 0xD60;
        break;
    case ABILITY_THROW:
        d = 0xE38;
        break;
    case ABILITY_STAR_ROD:
        d = 0xF3A;
        break;
    default:
        while (1)
            ;
    }
    gCurTask->frame += d;
}
void HoldPlayer(s32 i, s32 j, u8 c)
{
    struct Task *t;
    struct Task *u;

    t = &gTasks[i];
    u = &gTasks[j];
    t->parent = j;
    t->taskClass = 4;
    t->state = c;
    t->actorKind = gCurTask->actorKind;
    if (u->actorKind == ACTOR_KIND_BOSS)
        PlayerSuspendControl(i, 1);
    else
        PlayerSuspendControl(i, 0);
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = i;
    TaskSetEntry(HeldPlayerInit, i);
}
void DropHeldPlayer(s32 i)
{
    struct Task *t;

    t = &gTasks[i];
    PlayerResumeControl(i, 0, 0, 1);
    gUnk_02007D00[1] = -1;
    t->actorKind = ACTOR_KIND_ENEMY;
}
void sub_08068690(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->posX = (t->pixelX - u->pixelX) << 16;
    t->posY = (t->pixelY - u->pixelY) << 16;
    t->playerHeldFrameTimer = 8;
    t->playerHeldSwallowFrameCount = 2;
    if (gMetaKnightmareMode == 0)
    {
        if (t->player->mouthState == 1)
        {
            t->facing = u->facing;
            gCurTask->playerHeldFrameStep = 1;
            TaskSetFrame(0x171);
        }
        else
        {
            t->facing = -u->facing;
            v = gCurTask;
            v->playerHeldFrameStep = -v->facing;
            v->frame = 0x133;
            HeldPlayerAddAbilityFrameOffset();
        }
    }
    else
    {
        t->facing = u->facing;
        TaskSetFrame(0x123B);
    }
}
void HeldPlayerPullTowardCaptor(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;

    t = gCurTask;
    if (t->posX <= 0)
        t->accelX = 10752;
    else
        t->accelX = -10752;
    u = gCurTask;
    v = u->posY;
    if (v < 0)
        v = -v;
    v >>= 3;
    if (u->posY <= 0)
        u->velY = v;
    else
        u->velY = -v;
}
u32 HeldPlayerIsNearCaptor(void)
{
    struct Task *t;
    struct Task *u;
    s32 d;

    t = gCurTask;
    u = &gTasks[t->parent];
    d = u->pixelX - t->pixelX;
    if (d < 0)
        d = -d;
    if (d <= 23)
        return 1;
    else
        return 0;
}
void HeldPlayerEnterMouth(void)
{
    gUnk_02007D00[0] = 1;
    gCurTask->frame = 0xFFFF;
}
void sub_080687fc(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->playerHeldFrameTimer <= 0)
    {
        t->frame += t->playerHeldFrameStep;
        t->playerHeldFrameTimer = 8;
        t->playerHeldSwallowFrameCount--;
    }
    else
    {
        t->playerHeldFrameTimer--;
    }
}
void sub_08068828(void)
{
    struct Task *t;

    t = gCurTask;
    t->parent = t->playerHeldNextCaptor;
    t->posX = 0;
    t->posY = 0;
}
void sub_08068840(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->pixelX = gUnk_02007D00[2];
    t->pixelY = gUnk_02007D00[3];
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    t->facing = gUnk_02007D00[4];
    u = gCurTask;
    u->playerHeldFrameStep = -u->facing;
    u->playerHeldFrameTimer = 8;
    if (gMetaKnightmareMode == 0)
        u->frame = 0x133;
    else
        TaskSetFrame(0x123B);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x2000, 0x30000);
    gCurTask->onGround = 0;
    if (HeldPlayerDamage(-8, 256) != 0)
        SetPlayerInvulnerability(1, 96, gCurTaskIdx);
    v = gCurTask;
    if (v->health != 0)
        v->playerHeldSpitTimer = 48;
    else
        v->playerHeldSpitTimer = 32;
}
void SetHeldPlayerState(s32 i, u8 c)
{
    struct Task *t;

    t = &gTasks[i];
    t->playerHeldNextCaptor = gCurTaskIdx;
    t->state = c;
    TaskSetEntry(HeldPlayerEnterState, i);
}
void sub_08068950(s16 x, s16 y, s16 d)
{
    gUnk_02007D00[2] = x;
    gUnk_02007D00[3] = y;
    gUnk_02007D00[4] = d;
}
void sub_0806896c(void)
{
    struct Task *t;
    struct Task *u;

    if (gMetaKnightmareMode != 0)
        return;
    t = gCurTask;
    if (t->playerHeldFrameTimer <= 0)
    {
        t->frame += t->playerHeldFrameStep;
        t->playerHeldFrameTimer = 8;
        if (t->frame > 0x135)
            t->frame = 294;
        u = gCurTask;
        if (u->frame <= 0x125)
            u->frame = 0x135;
    }
    else
    {
        t->playerHeldFrameTimer--;
    }
}
void ReleaseHeldPlayer(s32 i, u8 d)
{
    struct Task *t;
    struct PlayerState *p;

    t = &gTasks[i];
    p = &gPlayerStates[i];
    t->facing = d;
    t->onGround = 0;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    t->actorKind = ACTOR_KIND_ENEMY;
    TaskStopSlot(i);
    PlayerResumeControl(i, 6, 0, 0);
    p->unk14 = 4;
}
s32 HeldPlayerDamage(s32 a, s32 b)
{
    struct Task *t;
    s32 r;

    r = AddPlayerHealth(a, gCurTaskIdx);
    if (gCurTask->player->ability != ABILITY_NORMAL)
    {
        CreateAbilityStar(0);
        SetPlayerAbility(ABILITY_NORMAL, -1, gCurTask->player->playerIndex);
    }
    t = gCurTask;
    if (t->health == 0)
        t->hitEffect = b;
    return r;
}
void PlayerSuspendControl(s32 i, u8 flag)
{
    struct Task *t;
    struct PlayerState *p;
    u8 a;
    u8 b;

    t = &gTasks[i];
    p = &gPlayerStates[i];
    if (p->sfxPlayer != -1)
        StopSfxOnPlayer(p->sfxPlayer, p->sfxId);
    t->lateUpdateCallback = 0;
    t->updateCallback = 0;
    t->updateState = 0;
    t->sleepFrames = 0;
    t->u76.unk76 = 0;
    t->variant = 0;
    t->hitKind = HIT_KIND_NONE;
    if (p->unk40 & 1)
    {
        gCurTask->player->pixelOffsetY = 0;
        p->pixelOffsetX = 0;
        p->blockBreakCooldown = 0;
        p->offsetScriptDelay = 0;
        p->offsetScriptStep = 0;
        p->unk40 &= 0xFFFE;
        t->skipMask = 0;
    }
    p->unk42 &= 0xFFEF;
    t->accelY = 0;
    t->accelX = 0;
    t->velY = 0;
    t->velX = 0;
    t->speedLimitY = 0x80000000;
    t->speedLimitX = 0x80000000;
    a = p->mode;
    b = p->hitsThisFrame;
    if (flag != 0)
        sub_0803d1c4(i);
    else
        sub_0803d2d4(i);
    p->prevMode = a;
    p->mode = 16;
    gPlayerStates[i].hitsThisFrame = b;
    p->unk16 = 255;
    DisablePause();
}
void PlayerResumeControl(s32 i, u16 b, u8 c, u8 d)
{
    struct Task *t;
    struct PlayerState *p;

    t = &gTasks[i];
    p = &gPlayerStates[i];
    if (c != 0)
        sub_0803d1c4(i);
    else
        sub_0803d2d4(i);
    t->taskClass = 1;
    t->layer = 7;
    t->parent = i;
    t->moveCallback = (u32)PlayerMove;
    t->updateCallback = (u32)PlayerUpdate;
    t->lateUpdateCallback = (u32)sub_0803332c;
    if (b == 0)
    {
        if (t->waterFlags == 0)
        {
            if (t->onGround != 0)
                p->requestedAction = PLAYER_ACTION_STAND;
            else
                p->requestedAction = PLAYER_ACTION_FALL;
        }
        else
        {
            if (t->onGround != 0)
                p->requestedAction = PLAYER_ACTION_STAND_IN_WATER;
            else
                p->requestedAction = PLAYER_ACTION_SWIM;
        }
    }
    else
    {
        p->requestedAction = b;
    }
    switch (p->ability)
    {
    case ABILITY_MIKE:
    case ABILITY_CRASH:
    case ABILITY_LIGHT:
        p->paletteFlashMode = 2;
        break;
    case ABILITY_UFO:
        p->requestedAction = PLAYER_ACTION_UFO;
        break;
    }
    TaskSetEntry(PlayerStartRequestedAction, i);
    if (d != 0)
        SetPlayerInvulnerability(1, 96, i);
    p->prevPixelX = t->pixelX;
    p->prevPixelY = t->pixelY;
    if (gPlayerHealth[i] != 0)
        EnablePause();
}
u32 ActorTestColliders(u8 a)
{
    PlaceAttackBox();
    if (HitTestColliderClass20() != 0)
    {
        ActorStoreHit(a);
        return 1;
    }
    if (HitTestColliderClass10() != 0)
    {
        ActorStoreHit(a);
        return 1;
    }
    if (HitTestPlayerColliders() != 0)
    {
        ActorStoreHit(a);
        return 1;
    }
    return 0;
}
u32 ActorCheckHitsWithBox(s32 a)
{
    struct Task *t;
    struct Task *u;
    u16 saved;
    u32 r;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    gCurTask->hitKind = HIT_KIND_NONE;
    t = gCurTask;
    saved = t->health;
    r = 0;
    if (a != 0)
    {
        if (t->hitterSlot != -1)
        {
            if (--t->hitTimer <= 0)
            {
                gCurTask->hitTimer = 0;
                gCurTask->hitterSlot = 255;
                gCurTask->hitterPlayer = -1;
            }
        }
        gAttackX = gCurTask->pixelX;
        u = gCurTask;
        gAttackY = u->pixelY;
        gAttackHealth = u->health;
        gAttackLastHitterSlot = u->hitterSlot;
        gAttackLastHitter = u->hitterPlayer;
        gAttackHitTimer = u->hitTimer;
        gAttackFacing = u->facing;
        gAttackHitDuration = 30;
        gAttackBox = a;
        r = ActorTestColliders(0);
    }
    gCurTask->health = saved;
    return r;
}
u32 ActorCheckHits(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Actor *a;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->u8C.actor;
    t->hitKind = HIT_KIND_NONE;
    if (a->attackBox == 0)
        return 0;
    u = gCurTask;
    if (u->hitterSlot != -1)
    {
        if (--u->hitTimer <= 0)
        {
            gCurTask->hitTimer = 0;
            gCurTask->hitterSlot = 255;
            gCurTask->hitterPlayer = -1;
            if (a->hitState != 2)
                a->hitState = 0;
        }
    }
    gAttackX = gCurTask->pixelX;
    v = gCurTask;
    gAttackY = v->pixelY;
    gAttackHealth = v->health;
    gAttackLastHitterSlot = v->hitterSlot;
    gAttackLastHitter = v->hitterPlayer;
    gAttackLastHitterClass = a->hitterClass;
    gAttackHitTimer = v->hitTimer;
    gAttackFacing = v->facing;
    if (a->unk60 != NULL)
    {
        gAttackHitDuration = a->unk60->hitDuration;
        if (v->hitTimer > (a->unk60->hitDuration >> 1))
            gAttackBox = a->unk60->altAttackBox;
        else
            gAttackBox = a->attackBox;
    }
    else
    {
        gAttackHitDuration = 30;
        gAttackBox = a->attackBox;
    }
    ActorUpdateAttachedEffect();
    if (gAttackHitDuration - gCurTask->hitTimer <= 5)
        return 0;
    return ActorTestColliders(1);
}
u32 ActorCheckHitsWithExtraBox(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Actor *a;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->u8C.actor;
    t->hitKind = HIT_KIND_NONE;
    if (a->attackBox != 0)
    {
        u = gCurTask;
        if (u->hitterSlot != -1)
        {
            if (--u->hitTimer <= 0)
            {
                gCurTask->hitTimer = 0;
                gCurTask->hitterSlot = 255;
                gCurTask->hitterPlayer = -1;
                if (a->hitState != 2)
                    a->hitState = 0;
            }
        }
        gAttackX = gCurTask->pixelX;
        v = gCurTask;
        gAttackY = v->pixelY;
        gAttackHealth = v->health;
        gAttackLastHitterSlot = v->hitterSlot;
        gAttackLastHitter = v->hitterPlayer;
        gAttackLastHitterClass = a->hitterClass;
        gAttackHitTimer = v->hitTimer;
        gAttackFacing = v->facing;
        if (a->unk60 != NULL)
        {
            gAttackHitDuration = a->unk60->hitDuration;
            if (v->hitTimer > (a->unk60->hitDuration >> 1))
                gAttackBox = a->unk60->altAttackBox;
            else
                gAttackBox = a->attackBox;
        }
        else
        {
            gAttackHitDuration = 30;
            gAttackBox = a->attackBox;
        }
        if (gAttackHitDuration - gCurTask->hitTimer <= 5)
            return 0;
        if (ActorTestColliders(1) == 1)
            return 1;
    }
    if (a->extraAttackBox == 0)
        return 0;
    gAttackBox = a->extraAttackBox;
    gAttackX = gCurTask->pixelX;
    w = gCurTask;
    gAttackY = w->pixelY;
    PlaceAttackBox();
    if (a->unk60 != NULL)
    {
        gAttackHitDuration = a->unk60->hitDuration;
        if (gCurTask->hitTimer > (a->unk60->hitDuration >> 1))
            gAttackBox = a->unk60->altAttackBox;
        else
            gAttackBox = a->extraAttackBox;
    }
    else
    {
        gAttackHitDuration = 30;
        gAttackBox = a->extraAttackBox;
    }
    if (HitTestPlayerColliders() == 0)
        return 0;
    ActorStoreHit(1);
    return 1;
}
u32 ActorCheckPlayerHitsWithBox(s32 a)
{
    struct Task *t;
    struct Task *u;
    struct Actor *b;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    b = t->u8C.actor;
    t->hitKind = HIT_KIND_NONE;
    if (a == 0)
        return 0;
    gAttackX = gCurTask->pixelX;
    u = gCurTask;
    gAttackY = u->pixelY;
    gAttackHealth = u->health;
    gAttackLastHitterSlot = u->hitterSlot;
    gAttackLastHitter = u->hitterPlayer;
    gAttackHitTimer = u->hitTimer;
    gAttackFacing = u->facing;
    if (b->unk60 != NULL)
        gAttackHitDuration = b->unk60->hitDuration;
    else
        gAttackHitDuration = 30;
    gAttackBox = a;
    PlaceAttackBox();
    if (HitTestPlayerColliders() == 0)
        return 0;
    ActorStoreHit(0);
    return 1;
}
void ActorStoreHit(u8 a)
{
    struct Task *t;
    struct Actor *b;
    struct Task *u;

    t = gCurTask;
    b = t->u8C.actor;
    t->hitKind = gHitKind;
    gCurTask->hitEffect = gHitEffect;
    gCurTask->health = gHitHealthLeft;
    gCurTask->hitDirection = gHitDirection;
    gCurTask->hitTimer = gHitTimer;
    gCurTask->hitterSlot = gHitterSlot;
    gCurTask->hitterPlayer = gUnk_03001F24;
    if (a != 0)
    {
        if ((u8)(gHitKind - 3) > 1)
        {
            if (b->hitState != 2)
                b->hitState = 1;
        }
        b->hitterClass = gHitterColliderClass;
        b->hitterKind = gHitterColliderKind;
        u = &gTasks[gCurTask->hitterSlot];
        b->hitterParent = u->parent;
    }
}
