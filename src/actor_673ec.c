/* game_code_and_rodata 0x080673EC-0x080692FC (issue #65, module M17 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080673EC 0x080692FC src/actor_673ec.c --newpb
 *
 * Task bodies for the carried/helper actor states (unk15 = 3..10) plus the
 * player-record plumbing: spawn/teardown (sub_080685EC/sub_0806865C), the
 * per-character animation-offset switch (sub_080684A4), and the
 * gPlayerStates[] save/restore used when a player is picked up or dropped.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
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
extern u8 gUnk_03002460;
extern s32 gAttackBox;
extern u8 gHitKind;
extern u8 gUnk_03002450;
extern u16 gHitHealthLeft;
extern u8 gHitDirection;
extern u8 gHitTimer;
extern u8 gHitterSlot;
extern u8 gUnk_03001F24;
extern u8 gUnk_030023A4;
extern u8 gUnk_030023D0;

extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskStopSlot(s32 i);
extern s32 AddPlayerHealth(s32 a, s32 b);
extern void CreateAbilityStar(u32 a);
extern void SetPlayerAbility(u32 a, s32 b, s32 c);
extern void StopSfxOnPlayer(s32 a, s32 b);
extern void sub_0803d1c4(s32 i);
extern void sub_0803d2d4(s32 i);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern void PlayerUpdate(void);
extern void sub_0803332c(void);
extern void PlayerStartRequestedAction(void);
extern void sub_0801b7dc(void);
extern u8 sub_0801b24c(void);
extern u8 sub_0801af14(void);
extern u8 sub_0801a8c8(void);
extern void ActorStoreHit(u8 a);
extern u32 ActorTestColliders(u8 a);
extern void ActorUpdateAttachedEffect(void);
extern void sub_08068a8c(s32 i, u8 flag);
extern void sub_08068b88(s32 i, u16 b, u8 c, u8 d);
extern void sub_0806737c(void);
extern void PlayerUpdateInvulnerability(void);
extern void sub_0803e080(void);
extern void SetCameraFocus(s16 x, s16 y);
extern void sub_08068690(void);
extern void sub_08068760(void);
extern u32 sub_080687a0(void);
extern void TaskStop(void);
extern void sub_080687e0(void);
extern void TaskSleepForever(void);
extern void sub_080687fc(void);
extern void sub_08068828(void);
extern void PlayerMove(void);
extern void sub_08068840(void);
extern void sub_0806865c(s32 i);
extern void sub_0806896c(void);
extern void TaskSetFrame(s32 a);
extern s32 sub_080684a4(void);
extern void TaskYieldTrampoline(u32 a);
extern s32 sub_08068a2c(s32 a, s32 b);
extern void SetPlayerInvulnerability(u32 a, u32 b, s32 c);
extern void PlaySfx(u32 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskSetFrameFlip(s32 a);
extern u8 gTerrainResult;
extern void ClampTaskToRoom(struct Task *t);
extern void sub_0801bcac(u32 *p);
extern void RequestScreenShake(u32 a);
extern void sub_080682a8(void);
extern void TaskSetEntry(void *fn, s32 i);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);

void sub_080675e4(void);

void sub_080673ec(void)
{
    CallTableEntry(gCurTask->state, 11, gUnk_0873E2F0);
}

void sub_08067408(void)
{
    CallTableEntry(gCurTask->updateState, 11, gUnk_0873E31C);
    PlayerUpdateInvulnerability();
    if ((gCurTask->player->unk42 & 32) == 0)
        sub_0803e080();
    if (gLocalPlayer == gCurTask->player->playerIndex)
        SetCameraFocus(gCurTask->pixelX, gCurTask->pixelY);
}

void sub_08067470(void)
{
    gCurTask->updateState = 0;
    sub_08068690();
    while (sub_080687a0() == 0)
    {
        sub_08068760();
        TaskYieldTrampoline(1);
    }
    TaskStop();
    sub_080687e0();
    TaskSleepForever();
}

void sub_080674a8(void)
{
    struct Task *t;

    if (gUnk_03001F30 != 0)
        return;
    t = gCurTask;
    if (t->unk30 <= 0)
        return;
    if (t->frame == -1)
        return;
    sub_080687fc();
}

void sub_080674d8(void)
{
    gCurTask->updateState = 1;
    sub_08068828();
    TaskSleepForever();
}

void sub_080674f4(void)
{
}

void sub_080674f8(void)
{
    struct Task *t;

    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->updateState = 2;
    TaskStop();
    sub_08068840();
    TaskSleepForever();
}

void sub_08067520(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk34 <= 0)
    {
        TaskStop();
        sub_0806865c(gCurTaskIdx);
    }
    else
    {
        t->unk34--;
        sub_0806896c();
    }
}

void sub_08067550(void)
{
    struct Task *u;
    struct PlayerState *p;

    gCurTask->updateState = 3;
    gCurTask->facing = gTasks[gCurTask->parent].facing;
    if (gUnk_03001F30 == 0)
    {
        u = gCurTask;
        p = u->player;
        if (p->mouthState == 1)
        {
            u->unk28 = 0x11C1;
            u->unk2C = 3;
        }
        else
        {
            u->unk28 = 0x133;
            u->unk2C = -8;
        }
        TaskSetFrame(*(s16 *)&gCurTask->unk28);
        sub_080684a4();
    }
    else
    {
        TaskSetFrame(0x123B);
    }
    TaskSleepForever();
}
void sub_080675d8(void)
{
    sub_080675e4();
}

/* Follow the carried task's frame offsets out of gUnk_0873E1F8. */
void sub_080675e4(void)
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
    if (gUnk_03001F30 == 0)
    {
        if (*p++ != 0)
            t->frame = t->unk28 + t->unk2C;
        else
            t->frame = t->unk28;
        sub_080684a4();
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
void sub_080676c0(void)
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
    sub_08068a2c(-8, 512);
    if (gUnk_03001F30 == 0)
        sub_080675e4();
    t = gCurTask;
    t->unk28 = gTasks[t->parent].unk28;
    t->unk34 = 0;
    if (t->health == 0)
        t->unk34 = 1;
    if (gCurTask->unk28 != 3)
        TaskYieldTrampoline(28);
    else
        TaskYieldTrampoline(1);
    if (gCurTaskIdx == gLocalPlayer)
    {
        if (gUnk_03001F30 == 0)
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
    if (gUnk_03001F30 == 0 && gCurTask->player->mouthState == 1)
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
        w->unk2C = w->facing;
        w->unk28 = 8;
        if (gUnk_03001F30 == 0)
            TaskSetFrameFlip(0x133);
        else
            TaskSetFrame(0x123B);
        gCurTask->unk6C = 0;
        do
        {
            sub_0806896c();
            TaskYieldTrampoline(1);
        } while ((s16)(++gCurTask->unk6C) <= 29);
    }
    x = gCurTask;
    x->unk34 = x->unk34 + 1;
    TaskSleepForever();
}
void sub_08067908(void)
{
    ClampTaskToRoom(gCurTask);
    sub_0801bcac(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
        gCurTask->velX = 0;
    if (gCurTask->unk34 != 0)
    {
        TaskStop();
        sub_0806865c(gCurTaskIdx);
    }
}

/* Task body: the carried task wobbling in the player's hands. */
void sub_08067950(void)
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
    v->unk34 = 0;
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
        TaskSetMotionXFacing(gUnk_0873E348[gCurTask->unk34], 0x5A5A5A5A);
        w = gCurTask;
        w->velY = gUnk_0873E388[w->unk34];
        TaskYieldTrampoline(2);
        x = gCurTask;
        x->unk34++;
        if (x->unk34 > 14)
            x->unk34 = 0;
    }
}
void sub_08067a48(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 i;

    if (gUnk_03001F30 == 0)
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
            sub_080684a4();
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
void sub_08067b24(void)
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
    sub_08068a2c(-8, 512);
    t = gCurTask;
    t->unk34 = 0;
    if (t->health == 0)
        t->unk34 = 1;
    if (gUnk_03001F30 == 0)
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
    if (gUnk_03001F30 == 0)
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
            t->unk2C = t->facing;
            t->unk28 = 8;
            TaskSetFrameFlip(0x133);
            gCurTask->unk6C = 0;
            do
            {
                sub_0806896c();
                TaskYieldTrampoline(1);
                t = gCurTask;
                t->unk6C++;
            } while ((s16)t->unk6C <= 29);
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
    t->unk34++;
    TaskSleepForever();
}

void sub_08067d30(void)
{
    ClampTaskToRoom(gCurTask);
    sub_0801bcac(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
        gCurTask->velX = 0;
    if (gCurTask->unk34 != 0)
    {
        TaskStop();
        sub_0806865c(gCurTaskIdx);
    }
}

void sub_08067d78(void)
{
    struct Task *t;

    gCurTask->updateState = 7;
    t = gCurTask;
    t->facing = gTasks[t->parent].facing;
    TaskSleepForever();
}

/* Track the carrier's frame through the 5-word gUnk_0873E3C8 table. */
void sub_08067db0(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *p;
    s32 i;

    t = gCurTask;
    u = &gTasks[t->parent];
    i = (u->frame - 30) * 5;
    if (gUnk_03001F30 == 0)
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
    if (gUnk_03001F30 == 0)
        sub_080684a4();
    gCurTask->layer = gUnk_0873E3C8[i + 4];
}

/* Task body: the carried task struggling in the player's hands. */
void sub_08067ea4(void)
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
    if (gUnk_03001F30 == 0)
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
            sub_080684a4();
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do
            {
                w = gCurTask;
                w->frame--;
                TaskYieldTrampoline(1);
                v = gCurTask;
                v->unk6C++;
            } while ((s16)v->unk6C <= 14);
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

void sub_08068028(void)
{
    struct Task *t;

    sub_0801bcac(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
    {
        CreateChildTaskHere(148, 0);
        PlaySfx(153);
        RequestScreenShake(2);
        t = gCurTask;
        t->facing = -t->facing;
        TaskStop();
        TaskSetEntry(sub_080682a8, gCurTaskIdx);
    }
    else if (gCurTask->onGround & 1)
    {
        TaskStop();
        TaskSetEntry(sub_080682a8, gCurTaskIdx);
    }
}

/* Task body: the carried task struggling, mirrored variant. */
void sub_080680ac(void)
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
    if (gUnk_03001F30 == 0)
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
            sub_080684a4();
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do
            {
                gCurTask->frame--;
                TaskYieldTrampoline(1);
            } while ((s16)(++gCurTask->unk6C) <= 14);
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

void sub_08068224(void)
{
    struct Task *t;

    sub_0801bcac(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
    {
        CreateChildTaskHere(148, 0);
        PlaySfx(153);
        RequestScreenShake(2);
        t = gCurTask;
        t->facing = -t->facing;
        TaskStop();
        TaskSetEntry(sub_080682a8, gCurTaskIdx);
    }
    else if (gCurTask->onGround & 1)
    {
        TaskStop();
        TaskSetEntry(sub_080682a8, gCurTaskIdx);
    }
}
void sub_080682a8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t = gCurTask;
    t->moveCallback = (u32)PlayerMove;
    t->updateState = 10;
    TaskStop();
    sub_08068a2c(-8, 512);
    u = gCurTask;
    u->unk34 = 0;
    if (u->health == 0)
        u->unk34 = 1;
    if (gUnk_03001F30 == 0)
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
    if (gUnk_03001F30 == 0)
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
            v->unk2C = v->facing;
            v->unk28 = 8;
            TaskSetFrameFlip(0x133);
            gCurTask->unk6C = 0;
            do
            {
                sub_0806896c();
                TaskYieldTrampoline(1);
            } while ((s16)(++gCurTask->unk6C) <= 14);
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
    w->unk34 = w->unk34 + 1;
    TaskSleepForever();
}
void sub_08068460(void)
{
    ClampTaskToRoom(gCurTask);
    sub_0801bcac(gPlayerDefaultTerrainBox);
    if (gTerrainResult != 0)
        gCurTask->velX = 0;
    if (gCurTask->unk34 != 0)
        sub_0806865c(gCurTaskIdx);
}
s32 sub_080684a4(void)
{
    s32 d;

    switch (gCurTask->player->ability)
    {
    case 0:
    case 7:
    case 11:
    case 20:
    case 21:
    case 24:
        d = 0;
        break;
    case 1:
        d = 0x143;
        break;
    case 2:
        d = 0x234;
        break;
    case 3:
        d = 0x2B3;
        break;
    case 4:
        d = 0x384;
        break;
    case 5:
        d = 0x495;
        break;
    case 6:
        d = 0x522;
        break;
    case 8:
        d = 0x5CB;
        break;
    case 9:
        d = 0x69C;
        break;
    case 10:
        d = 0x79C;
        break;
    case 12:
        d = 0x838;
        break;
    case 13:
        d = 0x8C4;
        break;
    case 14:
        d = 0x950;
        break;
    case 15:
        d = 0x9F5;
        break;
    case 16:
        d = 0xA87;
        break;
    case 17:
        d = 0xB0A;
        break;
    case 18:
        d = 0xB94;
        break;
    case 19:
        d = 0xC8F;
        break;
    case 22:
        d = 0xD60;
        break;
    case 23:
        d = 0xE38;
        break;
    case 25:
        d = 0xF3A;
        break;
    default:
        while (1)
            ;
    }
    gCurTask->frame += d;
}
void sub_080685ec(s32 i, s32 j, u8 c)
{
    struct Task *t;
    struct Task *u;

    t = &gTasks[i];
    u = &gTasks[j];
    t->parent = j;
    t->taskClass = 4;
    t->state = c;
    t->actorKind = gCurTask->actorKind;
    if (u->actorKind == 2)
        sub_08068a8c(i, 1);
    else
        sub_08068a8c(i, 0);
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = i;
    TaskSetEntry(sub_0806737c, i);
}
void sub_0806865c(s32 i)
{
    struct Task *t;

    t = &gTasks[i];
    sub_08068b88(i, 0, 0, 1);
    gUnk_02007D00[1] = -1;
    t->actorKind = 0;
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
    t->unk28 = 8;
    t->unk30 = 2;
    if (gUnk_03001F30 == 0)
    {
        if (t->player->mouthState == 1)
        {
            t->facing = u->facing;
            gCurTask->unk2C = 1;
            TaskSetFrame(0x171);
        }
        else
        {
            t->facing = -u->facing;
            v = gCurTask;
            v->unk2C = -v->facing;
            v->frame = 0x133;
            sub_080684a4();
        }
    }
    else
    {
        t->facing = u->facing;
        TaskSetFrame(0x123B);
    }
}
void sub_08068760(void)
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
u32 sub_080687a0(void)
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
void sub_080687e0(void)
{
    gUnk_02007D00[0] = 1;
    gCurTask->frame = 0xFFFF;
}
void sub_080687fc(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk28 <= 0)
    {
        t->frame += t->unk2C;
        t->unk28 = 8;
        t->unk30--;
    }
    else
    {
        t->unk28--;
    }
}
void sub_08068828(void)
{
    struct Task *t;

    t = gCurTask;
    t->parent = t->unk18;
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
    u->unk2C = -u->facing;
    u->unk28 = 8;
    if (gUnk_03001F30 == 0)
        u->frame = 0x133;
    else
        TaskSetFrame(0x123B);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x2000, 0x30000);
    gCurTask->onGround = 0;
    if (sub_08068a2c(-8, 256) != 0)
        SetPlayerInvulnerability(1, 96, gCurTaskIdx);
    v = gCurTask;
    if (v->health != 0)
        v->unk34 = 48;
    else
        v->unk34 = 32;
}
void sub_08068920(s32 i, u8 c)
{
    struct Task *t;

    t = &gTasks[i];
    t->unk18 = gCurTaskIdx;
    t->state = c;
    TaskSetEntry(sub_080673ec, i);
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

    if (gUnk_03001F30 != 0)
        return;
    t = gCurTask;
    if (t->unk28 <= 0)
    {
        t->frame += t->unk2C;
        t->unk28 = 8;
        if (t->frame > 0x135)
            t->frame = 294;
        u = gCurTask;
        if (u->frame <= 0x125)
            u->frame = 0x135;
    }
    else
    {
        t->unk28--;
    }
}
void sub_080689c8(s32 i, u8 d)
{
    struct Task *t;
    struct PlayerState *p;

    t = &gTasks[i];
    p = &gPlayerStates[i];
    t->facing = d;
    t->onGround = 0;
    t->posX = t->pixelX << 16;
    t->posY = t->pixelY << 16;
    t->actorKind = 0;
    TaskStopSlot(i);
    sub_08068b88(i, 6, 0, 0);
    p->unk14 = 4;
}
s32 sub_08068a2c(s32 a, s32 b)
{
    struct Task *t;
    s32 r;

    r = AddPlayerHealth(a, gCurTaskIdx);
    if (gCurTask->player->ability != 0)
    {
        CreateAbilityStar(0);
        SetPlayerAbility(0, -1, gCurTask->player->playerIndex);
    }
    t = gCurTask;
    if (t->health == 0)
        t->unk82 = b;
    return r;
}
void sub_08068a8c(s32 i, u8 flag)
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
    t->unk76 = 0;
    t->variant = 0;
    t->hitKind = 0;
    if (p->unk40 & 1)
    {
        gCurTask->player->pixelOffsetY = 0;
        p->pixelOffsetX = 0;
        p->unk2B = 0;
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
    sub_08067108();
}
void sub_08068b88(s32 i, u16 b, u8 c, u8 d)
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
                p->requestedAction = 1;
            else
                p->requestedAction = 7;
        }
        else
        {
            if (t->onGround != 0)
                p->requestedAction = 24;
            else
                p->requestedAction = 23;
        }
    }
    else
    {
        p->requestedAction = b;
    }
    switch (p->ability)
    {
    case 7:
    case 20:
    case 21:
        p->unk22 = 2;
        break;
    case 24:
        p->requestedAction = 55;
        break;
    }
    TaskSetEntry(PlayerStartRequestedAction, i);
    if (d != 0)
        SetPlayerInvulnerability(1, 96, i);
    p->prevPixelX = t->pixelX;
    p->prevPixelY = t->pixelY;
    if (gPlayerHealth[i] != 0)
        sub_08067114();
}
u32 ActorTestColliders(u8 a)
{
    sub_0801b7dc();
    if (sub_0801b24c() != 0)
    {
        ActorStoreHit(a);
        return 1;
    }
    if (sub_0801af14() != 0)
    {
        ActorStoreHit(a);
        return 1;
    }
    if (sub_0801a8c8() != 0)
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
    gCurTask->hitKind = 0;
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
        gUnk_030023F0 = u->hitTimer;
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
    a = t->unk8C;
    t->hitKind = 0;
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
    gUnk_03002460 = a->unk06;
    gUnk_030023F0 = v->hitTimer;
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
u32 sub_08068f68(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Actor *a;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    a = t->unk8C;
    t->hitKind = 0;
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
        gUnk_03002460 = a->unk06;
        gUnk_030023F0 = v->hitTimer;
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
    if (a->unk4C == 0)
        return 0;
    gAttackBox = a->unk4C;
    gAttackX = gCurTask->pixelX;
    w = gCurTask;
    gAttackY = w->pixelY;
    sub_0801b7dc();
    if (a->unk60 != NULL)
    {
        gAttackHitDuration = a->unk60->hitDuration;
        if (gCurTask->hitTimer > (a->unk60->hitDuration >> 1))
            gAttackBox = a->unk60->altAttackBox;
        else
            gAttackBox = a->unk4C;
    }
    else
    {
        gAttackHitDuration = 30;
        gAttackBox = a->unk4C;
    }
    if (sub_0801a8c8() == 0)
        return 0;
    ActorStoreHit(1);
    return 1;
}
u32 sub_0806914c(s32 a)
{
    struct Task *t;
    struct Task *u;
    struct Actor *b;

    if (gTaskSlotTypes[gCurTaskIdx] == -1)
        return 0;
    t = gCurTask;
    b = t->unk8C;
    t->hitKind = 0;
    if (a == 0)
        return 0;
    gAttackX = gCurTask->pixelX;
    u = gCurTask;
    gAttackY = u->pixelY;
    gAttackHealth = u->health;
    gAttackLastHitterSlot = u->hitterSlot;
    gAttackLastHitter = u->hitterPlayer;
    gUnk_030023F0 = u->hitTimer;
    gAttackFacing = u->facing;
    if (b->unk60 != NULL)
        gAttackHitDuration = b->unk60->hitDuration;
    else
        gAttackHitDuration = 30;
    gAttackBox = a;
    sub_0801b7dc();
    if (sub_0801a8c8() == 0)
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
    b = t->unk8C;
    t->hitKind = gHitKind;
    gCurTask->unk82 = gUnk_03002450;
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
        b->unk06 = gUnk_030023A4;
        b->unk07 = gUnk_030023D0;
        u = &gTasks[gCurTask->hitterSlot];
        b->unk0E = u->parent;
    }
}
