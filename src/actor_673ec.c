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

extern u8 gUnk_03001F30;
extern u16 gLocalPlayer;
extern s8 gUnk_0873E1F8[];
extern u32 gUnk_0873E2F0[];
extern u32 gUnk_0873E31C[];
extern vs16 gTaskSlotTypes[];
extern u32 gUnk_02007D00[];
extern struct PlayerState gPlayerStates[];
extern s16 gPlayerHealth[];
extern u16 gAttackX;
extern u16 gAttackY;
extern u16 gAttackPower;
extern u8 gUnk_03002390;
extern u8 gAttackLastHitter;
extern u8 gUnk_030023F0;
extern s16 gAttackFacing;
extern s8 gAttackHitDuration;
extern u8 gUnk_03002460;
extern s32 gAttackBox;
extern u8 gHitKind;
extern u8 gUnk_03002450;
extern u16 gHitDamage;
extern u8 gHitDirection;
extern u8 gUnk_03002354;
extern u8 gUnk_030023DC;
extern u8 gUnk_03001F24;
extern u8 gUnk_030023A4;
extern u8 gUnk_030023D0;

extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskStopSlot(s32 i);
extern s32 AddPlayerHealth(s32 a, s32 b);
extern void sub_08064eb8(u32 a);
extern void SetPlayerAbility(u32 a, s32 b, s32 c);
extern void StopSfxOnPlayer(s32 a, s32 b);
extern void sub_0803d1c4(s32 i);
extern void sub_0803d2d4(s32 i);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern void sub_08032d48(void);
extern void sub_0803332c(void);
extern void sub_08032bd0(void);
extern void sub_0801b7dc(void);
extern u8 sub_0801b24c(void);
extern u8 sub_0801af14(void);
extern u8 sub_0801a8c8(void);
extern void sub_08069234(u8 a);
extern u32 sub_08068cb4(u8 a);
extern void sub_0806dedc(void);
extern void sub_08068a8c(s32 i, u8 flag);
extern void sub_08068b88(s32 i, u16 b, u8 c, u8 d);
extern void sub_0806737c(void);
extern void sub_0803e2d4(void);
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
extern u16 gFrameCount;
extern s32 sub_08068a2c(s32 a, s32 b);
extern void SetPlayerInvulnerability(u32 a, u32 b, s32 c);
extern void PlaySfx(u32 a);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskSetFrameFlip(s32 a);
extern u32 gPlayerDefaultTerrainBox[];
extern u8 gTerrainResult;
extern s32 gUnk_0873E348[];
extern s32 gUnk_0873E388[];
extern void ClampTaskToRoom(struct Task *t);
extern void sub_0801bcac(u32 *p);
extern u16 gUnk_0873E3C8[];
extern void RequestScreenShake(u32 a);
extern void sub_080682a8(void);
extern void TaskSetEntry(void *fn, s32 i);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);

void sub_080675e4(void);

void sub_080673ec(void)
{
    CallTableEntry(gCurTask->unk14, 11, gUnk_0873E2F0);
}

void sub_08067408(void)
{
    CallTableEntry(gCurTask->unk15, 11, gUnk_0873E31C);
    sub_0803e2d4();
    if ((gCurTask->unk88->unk42 & 32) == 0)
        sub_0803e080();
    if (gLocalPlayer == gCurTask->unk88->unk00)
        SetCameraFocus(gCurTask->unk48, gCurTask->unk4A);
}

void sub_08067470(void)
{
    gCurTask->unk15 = 0;
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
    if (t->unk3C == -1)
        return;
    sub_080687fc();
}

void sub_080674d8(void)
{
    gCurTask->unk15 = 1;
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
    t->unk00 = (u32)PlayerMove;
    t->unk15 = 2;
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

    gCurTask->unk15 = 3;
    gCurTask->unk43 = gTasks[gCurTask->unk44].unk43;
    if (gUnk_03001F30 == 0)
    {
        u = gCurTask;
        p = u->unk88;
        if (p->unk06 == 1)
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
    u = &gTasks[t->unk44];
    if ((u16)(u->unk3C - 36) > 25)
        return;
    p = &gUnk_0873E1F8[u->unk3C * 4];
    t->unk48 = u->unk48 + *p * t->unk43;
    p++;
    t->unk4A = *p + u->unk4A;
    p++;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    if (gUnk_03001F30 == 0)
    {
        if (*p++ != 0)
            t->unk3C = t->unk28 + t->unk2C;
        else
            t->unk3C = t->unk28;
        sub_080684a4();
    }
    else
    {
        if (*p++ != 0)
            t->unk3C = 0x1243;
        else
            TaskSetFrame(0x123B);
    }
    gCurTask->unk42 = *p;
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
    u->unk00 = (u32)PlayerMove;
    u->unk15 = 4;
    TaskStop();
    sub_08068a2c(-8, 512);
    if (gUnk_03001F30 == 0)
        sub_080675e4();
    t = gCurTask;
    t->unk28 = gTasks[t->unk44].unk28;
    t->unk34 = 0;
    if (t->unk78 == 0)
        t->unk34 = 1;
    if (gCurTask->unk28 != 3)
        TaskYieldTrampoline(28);
    else
        TaskYieldTrampoline(1);
    if (gCurTaskIdx == gLocalPlayer)
    {
        if (gUnk_03001F30 == 0)
        {
            if (gCurTask->unk88->unk06 == 1)
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
        v->unk43 = -v->unk43;
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2500, 0x30000);
    if (gUnk_03001F30 == 0 && gCurTask->unk88->unk06 == 1)
    {
        TaskSetFrame(0x11C2);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(3);
        gCurTask->unk3C--;
        TaskYieldTrampoline(5);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
    }
    else
    {
        w = gCurTask;
        w->unk2C = w->unk43;
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
        gCurTask->unk54 = 0;
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

    gCurTask->unk15 = 5;
    u = gCurTask;
    u->unk43 = gTasks[u->unk44].unk43;
    v = gCurTask;
    v->unk34 = 0;
    v->unk42 = 7;
    TaskStop();
    t = gCurTask;
    t->unk4C = (t->unk43 * 3) << 19;
    t->unk50 = 0;
    while (gTasks[gCurTask->unk44].unk3C == 34)
        TaskYieldTrampoline(1);
    gCurTask->unk42 = 12;
    if (gCurTask->unk88->unk06 == 1)
    {
        TaskSleepForever();
        return;
    }
    while (1)
    {
        TaskSetMotionXFacing(gUnk_0873E348[gCurTask->unk34], 0x5A5A5A5A);
        w = gCurTask;
        w->unk58 = gUnk_0873E388[w->unk34];
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
        p = t->unk88;
        if (p->unk06 == 1)
        {
            i = (s16)(gTasks[t->unk44].unk3C - 30);
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
        if (u->unk43 == 1)
            u->unk3E |= 0x8000;
        else
            u->unk3E &= 0x7FFF;
        TaskSleepForever();
    }
}
void sub_08067b24(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    t->unk00 = (u32)PlayerMove;
    t->unk15 = 6;
    TaskStop();
    t = gCurTask;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    sub_08068a2c(-8, 512);
    t = gCurTask;
    t->unk34 = 0;
    if (t->unk78 == 0)
        t->unk34 = 1;
    if (gUnk_03001F30 == 0)
    {
        if (gLocalPlayer == gCurTaskIdx)
        {
            p = gCurTask->unk88;
            if (p->unk06 == 1)
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
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2500, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        t = gCurTask;
        p = t->unk88;
        if (p->unk06 == 1)
        {
            TaskSetFrame(370);
            TaskYieldTrampoline(3);
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(4);
            t = gCurTask;
            t->unk3C--;
            TaskYieldTrampoline(2);
            t = gCurTask;
            t->unk3C--;
            TaskYieldTrampoline(4);
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(2);
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(4);
            t = gCurTask;
            t->unk3C--;
            TaskYieldTrampoline(3);
            t = gCurTask;
            t->unk3C--;
            TaskYieldTrampoline(5);
            t = gCurTask;
            t->unk3C++;
            TaskYieldTrampoline(3);
        }
        else
        {
            t->unk2C = t->unk43;
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
        gCurTask->unk54 = 0;
    if (gCurTask->unk34 != 0)
    {
        TaskStop();
        sub_0806865c(gCurTaskIdx);
    }
}

void sub_08067d78(void)
{
    struct Task *t;

    gCurTask->unk15 = 7;
    t = gCurTask;
    t->unk43 = gTasks[t->unk44].unk43;
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
    u = &gTasks[t->unk44];
    i = (u->unk3C - 30) * 5;
    if (gUnk_03001F30 == 0)
    {
        p = t->unk88;
        if (p->unk06 == 1)
            i += 75;
    }
    else
    {
        i += 150;
    }
    t = gCurTask;
    t->unk48 = u->unk48 + (u16)u->unk43 * gUnk_0873E3C8[i];
    t->unk4A = u->unk4A + gUnk_0873E3C8[i + 1];
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk43 = u->unk43 * (s16)gUnk_0873E3C8[i + 2];
    TaskSetFrame((s16)gUnk_0873E3C8[i + 3]);
    if (gUnk_03001F30 == 0)
        sub_080684a4();
    gCurTask->unk42 = gUnk_0873E3C8[i + 4];
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

    gCurTask->unk15 = 8;
    gCurTask->unk00 = (u32)PlayerMove;
    gCurTask->unk42 = 7;
    gCurTask->unk43 = gTasks[gCurTask->unk44].unk43;
    t = gCurTask;
    u = &gTasks[t->unk44];
    t->unk4C = (u->unk48 + (t->unk43 << 4)) << 16;
    t->unk50 = (gTasks[t->unk44].unk4A - 24) << 16;
    TaskSetMotionXFacing(0x50000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x1200, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        p = gCurTask->unk88;
        if (p->unk06 == 1)
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
                w->unk3C--;
                TaskYieldTrampoline(1);
                v = gCurTask;
                v->unk6C++;
            } while ((s16)v->unk6C <= 14);
        }
    }
    TaskSetFrame(0x123C);
    x = gCurTask;
    if (x->unk43 == 1)
        x->unk3E |= 0x8000;
    else
        x->unk3E &= 0x7FFF;
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
        t->unk43 = -t->unk43;
        TaskStop();
        TaskSetEntry(sub_080682a8, gCurTaskIdx);
    }
    else if (gCurTask->unk7A & 1)
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

    gCurTask->unk15 = 9;
    gCurTask->unk00 = (u32)PlayerMove;
    gCurTask->unk42 = 7;
    gCurTask->unk43 = -gTasks[gCurTask->unk44].unk43;
    u = gCurTask;
    u->unk4C = gTasks[u->unk44].unk48 << 16;
    u->unk50 = (gTasks[u->unk44].unk4A - 40) << 16;
    TaskSetMotionXFacing(0x50000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x1200, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        p = gCurTask->unk88;
        if (p->unk06 == 1)
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
                gCurTask->unk3C--;
                TaskYieldTrampoline(1);
            } while ((s16)(++gCurTask->unk6C) <= 14);
        }
    }
    TaskSetFrame(0x123C);
    t = gCurTask;
    if (t->unk43 == 1)
        t->unk3E |= 0x8000;
    else
        t->unk3E &= 0x7FFF;
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
        t->unk43 = -t->unk43;
        TaskStop();
        TaskSetEntry(sub_080682a8, gCurTaskIdx);
    }
    else if (gCurTask->unk7A & 1)
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
    t->unk00 = (u32)PlayerMove;
    t->unk15 = 10;
    TaskStop();
    sub_08068a2c(-8, 512);
    u = gCurTask;
    u->unk34 = 0;
    if (u->unk78 == 0)
        u->unk34 = 1;
    if (gUnk_03001F30 == 0)
    {
        if (gLocalPlayer == gCurTaskIdx)
        {
            if (gCurTask->unk88->unk06 == 1)
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
    gCurTask->unk7A = 0;
    TaskSetMotionXFacing(0x18000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD8000, 0x2500, 0x30000);
    if (gUnk_03001F30 == 0)
    {
        if (gCurTask->unk88->unk06 == 1)
        {
            TaskSetFrame(370);
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
        else
        {
            v = gCurTask;
            v->unk2C = v->unk43;
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
        gCurTask->unk54 = 0;
    if (gCurTask->unk34 != 0)
        sub_0806865c(gCurTaskIdx);
}
s32 sub_080684a4(void)
{
    s32 d;

    switch (gCurTask->unk88->unk0D)
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
    gCurTask->unk3C += d;
}
void sub_080685ec(s32 i, s32 j, u8 c)
{
    struct Task *t;
    struct Task *u;

    t = &gTasks[i];
    u = &gTasks[j];
    t->unk44 = j;
    t->unk12 = 4;
    t->unk14 = c;
    t->unk72 = gCurTask->unk72;
    if (u->unk72 == 2)
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
    t->unk72 = 0;
}
void sub_08068690(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    u = &gTasks[t->unk44];
    t->unk4C = (t->unk48 - u->unk48) << 16;
    t->unk50 = (t->unk4A - u->unk4A) << 16;
    t->unk28 = 8;
    t->unk30 = 2;
    if (gUnk_03001F30 == 0)
    {
        if (t->unk88->unk06 == 1)
        {
            t->unk43 = u->unk43;
            gCurTask->unk2C = 1;
            TaskSetFrame(0x171);
        }
        else
        {
            t->unk43 = -u->unk43;
            v = gCurTask;
            v->unk2C = -v->unk43;
            v->unk3C = 0x133;
            sub_080684a4();
        }
    }
    else
    {
        t->unk43 = u->unk43;
        TaskSetFrame(0x123B);
    }
}
void sub_08068760(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;

    t = gCurTask;
    if (t->unk4C <= 0)
        t->unk5C = 10752;
    else
        t->unk5C = -10752;
    u = gCurTask;
    v = u->unk50;
    if (v < 0)
        v = -v;
    v >>= 3;
    if (u->unk50 <= 0)
        u->unk58 = v;
    else
        u->unk58 = -v;
}
u32 sub_080687a0(void)
{
    struct Task *t;
    struct Task *u;
    s32 d;

    t = gCurTask;
    u = &gTasks[t->unk44];
    d = u->unk48 - t->unk48;
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
    gCurTask->unk3C = 0xFFFF;
}
void sub_080687fc(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk28 <= 0)
    {
        t->unk3C += t->unk2C;
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
    t->unk44 = t->unk18;
    t->unk4C = 0;
    t->unk50 = 0;
}
void sub_08068840(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk48 = gUnk_02007D00[2];
    t->unk4A = gUnk_02007D00[3];
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk43 = gUnk_02007D00[4];
    u = gCurTask;
    u->unk2C = -u->unk43;
    u->unk28 = 8;
    if (gUnk_03001F30 == 0)
        u->unk3C = 0x133;
    else
        TaskSetFrame(0x123B);
    TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
    TaskSetMotionY(0xFFFD0000, 0x2000, 0x30000);
    gCurTask->unk7A = 0;
    if (sub_08068a2c(-8, 256) != 0)
        SetPlayerInvulnerability(1, 96, gCurTaskIdx);
    v = gCurTask;
    if (v->unk78 != 0)
        v->unk34 = 48;
    else
        v->unk34 = 32;
}
void sub_08068920(s32 i, u8 c)
{
    struct Task *t;

    t = &gTasks[i];
    t->unk18 = gCurTaskIdx;
    t->unk14 = c;
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
        t->unk3C += t->unk2C;
        t->unk28 = 8;
        if (t->unk3C > 0x135)
            t->unk3C = 294;
        u = gCurTask;
        if (u->unk3C <= 0x125)
            u->unk3C = 0x135;
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
    t->unk43 = d;
    t->unk7A = 0;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk72 = 0;
    TaskStopSlot(i);
    sub_08068b88(i, 6, 0, 0);
    p->unk14 = 4;
}
s32 sub_08068a2c(s32 a, s32 b)
{
    struct Task *t;
    s32 r;

    r = AddPlayerHealth(a, gCurTaskIdx);
    if (gCurTask->unk88->unk0D != 0)
    {
        sub_08064eb8(0);
        SetPlayerAbility(0, -1, gCurTask->unk88->unk00);
    }
    t = gCurTask;
    if (t->unk78 == 0)
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
    if (p->unk2C != -1)
        StopSfxOnPlayer(p->unk2C, p->unk2E);
    t->unk08 = 0;
    t->unk04 = 0;
    t->unk15 = 0;
    t->unk10 = 0;
    t->unk76 = 0;
    t->unk73 = 0;
    t->unk7C = 0;
    if (p->unk40 & 1)
    {
        gCurTask->unk88->unk26 = 0;
        p->unk24 = 0;
        p->unk2B = 0;
        p->unk29 = 0;
        p->unk28 = 0;
        p->unk40 &= 0xFFFE;
        t->unk13 = 0;
    }
    p->unk42 &= 0xFFEF;
    t->unk60 = 0;
    t->unk5C = 0;
    t->unk58 = 0;
    t->unk54 = 0;
    t->unk68 = 0x80000000;
    t->unk64 = 0x80000000;
    a = p->unk04;
    b = p->unk45;
    if (flag != 0)
        sub_0803d1c4(i);
    else
        sub_0803d2d4(i);
    p->unk05 = a;
    p->unk04 = 16;
    gPlayerStates[i].unk45 = b;
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
    t->unk12 = 1;
    t->unk42 = 7;
    t->unk44 = i;
    t->unk00 = (u32)PlayerMove;
    t->unk04 = (u32)sub_08032d48;
    t->unk08 = (u32)sub_0803332c;
    if (b == 0)
    {
        if (t->unk7B == 0)
        {
            if (t->unk7A != 0)
                p->unk01 = 1;
            else
                p->unk01 = 7;
        }
        else
        {
            if (t->unk7A != 0)
                p->unk01 = 24;
            else
                p->unk01 = 23;
        }
    }
    else
    {
        p->unk01 = b;
    }
    switch (p->unk0D)
    {
    case 7:
    case 20:
    case 21:
        p->unk22 = 2;
        break;
    case 24:
        p->unk01 = 55;
        break;
    }
    TaskSetEntry(sub_08032bd0, i);
    if (d != 0)
        SetPlayerInvulnerability(1, 96, i);
    p->unk5E = t->unk48;
    p->unk60 = t->unk4A;
    if (gPlayerHealth[i] != 0)
        sub_08067114();
}
u32 sub_08068cb4(u8 a)
{
    sub_0801b7dc();
    if (sub_0801b24c() != 0)
    {
        sub_08069234(a);
        return 1;
    }
    if (sub_0801af14() != 0)
    {
        sub_08069234(a);
        return 1;
    }
    if (sub_0801a8c8() != 0)
    {
        sub_08069234(a);
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
    gCurTask->unk7C = 0;
    t = gCurTask;
    saved = t->unk78;
    r = 0;
    if (a != 0)
    {
        if (t->unk7E != -1)
        {
            if (--t->unk75 <= 0)
            {
                gCurTask->unk75 = 0;
                gCurTask->unk7E = 255;
                gCurTask->unk7F = -1;
            }
        }
        gAttackX = gCurTask->unk48;
        u = gCurTask;
        gAttackY = u->unk4A;
        gAttackPower = u->unk78;
        gUnk_03002390 = u->unk7E;
        gAttackLastHitter = u->unk7F;
        gUnk_030023F0 = u->unk75;
        gAttackFacing = u->unk43;
        gAttackHitDuration = 30;
        gAttackBox = a;
        r = sub_08068cb4(0);
    }
    gCurTask->unk78 = saved;
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
    t->unk7C = 0;
    if (a->unk48 == 0)
        return 0;
    u = gCurTask;
    if (u->unk7E != -1)
    {
        if (--u->unk75 <= 0)
        {
            gCurTask->unk75 = 0;
            gCurTask->unk7E = 255;
            gCurTask->unk7F = -1;
            if (a->unk05 != 2)
                a->unk05 = 0;
        }
    }
    gAttackX = gCurTask->unk48;
    v = gCurTask;
    gAttackY = v->unk4A;
    gAttackPower = v->unk78;
    gUnk_03002390 = v->unk7E;
    gAttackLastHitter = v->unk7F;
    gUnk_03002460 = a->unk06;
    gUnk_030023F0 = v->unk75;
    gAttackFacing = v->unk43;
    if (a->unk60 != NULL)
    {
        gAttackHitDuration = a->unk60->unk00;
        if (v->unk75 > (a->unk60->unk00 >> 1))
            gAttackBox = a->unk60->unk04;
        else
            gAttackBox = a->unk48;
    }
    else
    {
        gAttackHitDuration = 30;
        gAttackBox = a->unk48;
    }
    sub_0806dedc();
    if (gAttackHitDuration - gCurTask->unk75 <= 5)
        return 0;
    return sub_08068cb4(1);
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
    t->unk7C = 0;
    if (a->unk48 != 0)
    {
        u = gCurTask;
        if (u->unk7E != -1)
        {
            if (--u->unk75 <= 0)
            {
                gCurTask->unk75 = 0;
                gCurTask->unk7E = 255;
                gCurTask->unk7F = -1;
                if (a->unk05 != 2)
                    a->unk05 = 0;
            }
        }
        gAttackX = gCurTask->unk48;
        v = gCurTask;
        gAttackY = v->unk4A;
        gAttackPower = v->unk78;
        gUnk_03002390 = v->unk7E;
        gAttackLastHitter = v->unk7F;
        gUnk_03002460 = a->unk06;
        gUnk_030023F0 = v->unk75;
        gAttackFacing = v->unk43;
        if (a->unk60 != NULL)
        {
            gAttackHitDuration = a->unk60->unk00;
            if (v->unk75 > (a->unk60->unk00 >> 1))
                gAttackBox = a->unk60->unk04;
            else
                gAttackBox = a->unk48;
        }
        else
        {
            gAttackHitDuration = 30;
            gAttackBox = a->unk48;
        }
        if (gAttackHitDuration - gCurTask->unk75 <= 5)
            return 0;
        if (sub_08068cb4(1) == 1)
            return 1;
    }
    if (a->unk4C == 0)
        return 0;
    gAttackBox = a->unk4C;
    gAttackX = gCurTask->unk48;
    w = gCurTask;
    gAttackY = w->unk4A;
    sub_0801b7dc();
    if (a->unk60 != NULL)
    {
        gAttackHitDuration = a->unk60->unk00;
        if (gCurTask->unk75 > (a->unk60->unk00 >> 1))
            gAttackBox = a->unk60->unk04;
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
    sub_08069234(1);
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
    t->unk7C = 0;
    if (a == 0)
        return 0;
    gAttackX = gCurTask->unk48;
    u = gCurTask;
    gAttackY = u->unk4A;
    gAttackPower = u->unk78;
    gUnk_03002390 = u->unk7E;
    gAttackLastHitter = u->unk7F;
    gUnk_030023F0 = u->unk75;
    gAttackFacing = u->unk43;
    if (b->unk60 != NULL)
        gAttackHitDuration = b->unk60->unk00;
    else
        gAttackHitDuration = 30;
    gAttackBox = a;
    sub_0801b7dc();
    if (sub_0801a8c8() == 0)
        return 0;
    sub_08069234(0);
    return 1;
}
void sub_08069234(u8 a)
{
    struct Task *t;
    struct Actor *b;
    struct Task *u;

    t = gCurTask;
    b = t->unk8C;
    t->unk7C = gHitKind;
    gCurTask->unk82 = gUnk_03002450;
    gCurTask->unk78 = gHitDamage;
    gCurTask->unk7D = gHitDirection;
    gCurTask->unk75 = gUnk_03002354;
    gCurTask->unk7E = gUnk_030023DC;
    gCurTask->unk7F = gUnk_03001F24;
    if (a != 0)
    {
        if ((u8)(gHitKind - 3) > 1)
        {
            if (b->unk05 != 2)
                b->unk05 = 1;
        }
        b->unk06 = gUnk_030023A4;
        b->unk07 = gUnk_030023D0;
        u = &gTasks[gCurTask->unk7E];
        b->unk0E = u->unk44;
    }
}
