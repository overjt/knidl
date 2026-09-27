#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
struct M11R20 { u32 w[5]; };

extern u8 gUnk_020055E8;
extern u8 gUnk_0200AF00;
extern vu16 gFadeSteps;
extern vu16 gFrameCount;
extern vu16 gDispCnt;
extern u8 gUnk_03001F34;
extern u8 gUnk_03002340;
extern s16 gSpriteCameraX;
extern u8 gUnk_03002350;
extern u16 gLatchedPressedKeys[];
extern s16 gSpriteCameraY;
extern u8 gUnk_03002438;
extern s8 gUnk_03002444;
extern u16 gLatchedHeldKeys[];
extern struct Task *gCurTask;
extern u8 gTerrainResult[];
extern u32 gPlayerMotionYPresets[];
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873CA68[];
extern u32 gPlayerDefaultTerrainBox[];
extern u32 gUnk_0873CB24[];
extern u32 gUnk_0873D03C[];
extern s16 gUnk_0873D206[];
extern s16 gUnk_0873D5C0[];
extern u32 gUnk_0873D986[];

void TaskYieldTrampoline(s32 frames);
u32 RandomRange(u32 range);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);
void StopAllSound(void);
void StopAllSfx(void);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskFree(s32 id);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrameByFacing(s16 a);
void TaskSetFrame(s32 a);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);
void sub_08021c74(s32 a, s32 b);
void TaskInitWaterFlags(void);
s32 IsFullBlockAtPixel(u16 a, u16 b);
s32 sub_08022760(struct Task *t);
void sub_08025024(void);
void sub_080261d4(u32 a);
s32 sub_080264b0(void);
void sub_0802651c(s32 a);
s32 sub_0802653c(void);
void sub_08026584(void);
void sub_08026704(s32 a);
void sub_0802672c(void);
void sub_08027204(u32 a);
void sub_08027548(void);
void sub_080276ac(s32 a);
void sub_08027a60(void);
void PlayerPlayBump(void);
void PlayerStopAxes(s32 a0);
void sub_0803e080(void);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
void PlayerStartSfx(s32 a, u16 b);
void FreezeOtherTasks(s32 a0);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerCheckBump(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerStopAtWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fd90(void);
s32 PlayerCheckJump(void);
s32 sub_0803fe68(void);
s32 PlayerCheckDuckOrSwallow(void);
s32 PlayerCheckLadder(void);
s32 sub_080400c0(void);
s32 sub_08040264(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerRequestLocomotion(void);
void sub_08040710(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void sub_08042c50(void);
s32 CreatePlayerEffect(s32 band, s32 id, s32 payload);

/* gPlayerMotionYPresets is a table of 8-byte records; the three used halfwords are
   signed 8.8 velocities for Task.unk58 / unk60 / unk68, and 0x9999 is the
   "leave this axis alone" sentinel.  Field 0's sign bit additionally clears
   Task.unk7A.  Same unpack as PlayerSetMotionXPreset's case 10 (which writes
   unk54/unk5C/unk64), except that block 1 needs its own sentinel local: the
   `u16 c` truncation keeps cse2 from folding the copy into the shift, and the
   separate result `s` lets the loaded value die at that copy so the shifted
   result can reuse its register. */
void PlayerSetMotionYPreset(s32 a0)
{
    u16 *e = (u16 *)gPlayerMotionYPresets + a0 * 4;
    s32 v = e[0];

    if (v != 0x9999)
    {
        struct Task *t = gCurTask;
        u16 c = v;
        s32 s = c << 8;

        if (c & 0x8000)
            s |= 0xFF000000;
        t->unk58 = s;
        if (e[0] & 0x8000)
            t->unk7A = 0;
    }

    if (e[1] != 0x9999)
    {
        struct Task *t2 = gCurTask;
        s32 v2 = e[1] << 8;

        if (e[1] & 0x8000)
            v2 |= 0xFF000000;
        t2->unk60 = v2;
    }

    if (e[2] != 0x9999)
    {
        struct Task *t3 = gCurTask;
        s32 v3 = e[2] << 8;

        if (e[2] & 0x8000)
            v3 |= 0xFF000000;
        t3->unk68 = v3;
    }
}

void sub_08041438(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 0;
    gCurTask->unk15 = 1;

    if (gCurTask->unk88->unk05 != 0)
    {
        struct Task *t;
        struct Task *t2;

        PlayerStopAxes(3);
        t = gCurTask;
        t->unk28 = (u16)t->unk88->unk4E;
        t->unk2C = t->unk88->unk4B;
        if (t->unk88->unk4A != 0)
            t->unk88->unk46 = t->unk88->unk4A;
        gCurTask->unk88->unk3D = 0;
        t2 = gCurTask;
        t2->unk88->unk40 &= 0xFFEF;
        t2->unk88->unk0F = 0;
        PlayerPlayBump();
    }

    {
        s16 *p = (s16 *)gUnk_0873D206;

        TaskSetFrame(p[sub_0803fd20(gCurTask->unk88->unk00)]);
    }
    TaskSleepForever();
}

void sub_080414e8(void)
{
    struct PlayerState *p;
    struct Task *t;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 1;
    gCurTask->unk15 = 2;
    PlayerSetMotionXPreset(1, 72);
    p = gCurTask->unk88;
    if (p->unk05 != 1)
    {
        p->unk3D = 0;
        gCurTask->unk88->unk46 = 0;
        gCurTask->unk88->unk0F = 0;
        gCurTask->unk28 = 0;
        PlayerPlayBump();
    }
    while (1)
    {
        TaskSetFrame(0x11D5);
        TaskYieldTrampoline(gCurTask->unk28 + 2);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 10);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 10);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
        t = gCurTask; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
    }
}

void sub_080415c8(void)
{
    while (sub_0803fd90() == 0 && PlayerCheckJump() == 0 && sub_0803fe68() == 0
           && PlayerCheckEnterDoor() == 0 && PlayerCheckLadder() == 0 && PlayerCheckDuckOrSwallow() == 0
           && sub_080400c0() == 0)
    {
        struct Task *t = gCurTask;

        if (t->unk54 == 0 && t->unk64 == 0)
        {
            t->unk88->unk01 = 1;
        }
        else if (gTerrainResult[0] != 0)
        {
            PlayerCheckBump();
            gCurTask->unk88->unk01 = 1;
        }
        else
        {
            struct Task *t2 = gCurTask;
            struct PlayerState *p = t2->unk88;
            s32 v = p->unk3D;

            if (v != 0)
            {
                p->unk01 = 3;
            }
            else if ((gLatchedHeldKeys[p->unk00] & 48) == 0)
            {
                s32 d = t2->unk54;

                if (d < 0)
                    d = -d;
                if ((u32)d <= 0xFFFF)
                    t2->unk28 = 2;
            }
            else
            {
                t2->unk28 = v;
            }
        }
        break;
    }
    PlayerSetMotionXPreset(2, 72);
}

void sub_080416a0(void)
{
    struct PlayerState *p;
    u16 *q;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 2;
    gCurTask->unk15 = 3;
    gCurTask->unk28 = 0;
    PlayerSetMotionXPreset(3, 72);
    if (gCurTask->unk88->unk05 != 2)
    {
        ((u8 *)gCurTask->unk88)[70] = 0;
        q = gLatchedHeldKeys;
        p = gCurTask->unk88;
        if (q[p->unk00] & 48)
        {
            if (((u8 *)p)[62] == 2)
                ((u8 *)p)[62] = 0;
        }
        PlayerPlayBump();
        PlayerStartSfx(117, gCurTask->unk88->unk00);
        CreatePlayerEffect(gCurTask->unk88->unk00, 7, 0);
    }
    while (1)
    {
        TaskSetFrame(0x11DD);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
    }
}

void sub_08041778(void)
{
    struct Task *t;
    struct Task *t3;
    struct PlayerState *p;
    struct PlayerState *p4;
    u16 *q;
    s32 x;
    s32 m;
    s32 m2;
    s32 y;
    s32 m3;

    t = gCurTask;
    if (t->unk28 == 0)
    {
        m2 = gTerrainResult[13];
        if (m2 != 0)
        {
            t->unk88->unk14 = 5;
            t->unk28 = 1;
        }
        else
        {
            t->unk88->unk14 = m2;
        }
    }
    else
    {
        p = t->unk88;
        if ((s16)p->unk14 == 0)
        {
            if (IsFullBlockAtPixel(((u16 *)t)[36],
                             (y = ((u16 *)t)[37], m3 = -16, m3 &= y, m3 + 16)) != 0)
                gCurTask->unk7A = 1;
        }
        else
        {
            p->unk14--;
        }
    }
    t = gCurTask;
    if ((t->unk7A & 1) != 0 || (((u8 *)t->unk88)[72] & 3) != 0)
    {
        t->unk28 = 0;
        t->unk88->unk14 = 0;
    }
    while (sub_0803fd90() == 0 && PlayerCheckJump() == 0)
    {
        if (gCurTask->unk28 == 0 && sub_0803fe68() != 0)
            break;
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (PlayerCheckLadder() != 0)
            break;
        if (PlayerCheckDuckOrSwallow() != 0)
            goto end;
        if (sub_080400c0() != 0)
            goto end;
        q = gLatchedHeldKeys;
        t3 = gCurTask;
        p4 = t3->unk88;
        m = q[p4->unk00] & 48;
        if (m == 0)
        {
            x = t3->unk54;
            if (x < 0)
                x = -x;
            if ((u32)x <= 0x1CBFF)
            {
                p4->unk3D = m;
                gCurTask->unk88->unk01 = 2;
                goto end;
            }
        }
        if (gTerrainResult[0] == 0)
            goto end;
        PlayerCheckBump();
        gCurTask->unk88->unk01 = 1;
        goto end;
    }
end:
    PlayerSetMotionXPreset(3, 72);
}

void sub_080418dc(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 3;
    gCurTask->unk15 = 4;
    PlayerSetMotionXPreset(4, 72);
    if (gCurTask->unk88->unk05 != 3)
    {
        PlaySfx(119);
        CreatePlayerEffect(gCurTask->unk88->unk00, 6, 0);
    }
    TaskSetFrame(0x11E3);
    TaskSleepForever();
}

void sub_08041940(void)
{
    struct Task *t;
    struct PlayerState *p;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 4;
    gCurTask->unk15 = 5;
    t = gCurTask;
    if (t->unk88->unk05 != 4)
    {
        t->unk28 = 1;
        p = t->unk88;
        if (p->unk05 == 9)
        {
            p->unk14 = 4;
        }
        else
        {
            t->unk73 = 0;
            TaskSetFrame(0x11ED);
            TaskYieldTrampoline(3);
            gCurTask->unk88->unk14 = 20;
        }
        PlayerSetMotionYPreset(4);
        PlaySfx(264);
        gCurTask->unk73 = 1;
        ((s8 *)gCurTask->unk88)[16] = 10;
    }
    PlayerPlayBump();
    TaskSetFrame(0x11E4);
    TaskYieldTrampoline(8);
    gCurTask->unk28 = 0;
    TaskSetFrame(0x11E5);
    TaskYieldTrampoline(3);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_08041a2c(void)
{
    struct Task *t;
    struct PlayerState *p;
    struct PlayerState *p3;
    s32 k;

    PlayerTurnToHeldDirection();
    while (sub_08040264() == 0 && PlayerCheckLadder() == 0)
    {
        if (sub_080400c0() != 0)
            break;
        if (PlayerCheckEnterDoor() != 0)
            break;
        t = gCurTask;
        k = t->unk73;
        if (k == 0)
            return;
        if (t->unk7A & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        if (gTerrainResult[1] != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(2);
            gCurTask->unk88->unk01 = 7;
            break;
        }
        if (k == 1)
        {
            p = t->unk88;
            p->unk14--;
            if (p->unk14 == 0 || (k &= gLatchedHeldKeys[t->unk88->unk00]) == 0)
            {
                t->unk73 = 2;
                PlayerSetMotionYPreset(5);
            }
        }
        else if (t->unk28 != 0 && t->unk58 >= 0)
        {
            PlayerSetMotionYPreset(6);
            gCurTask->unk88->unk01 = 7;
        }
        p3 = gCurTask->unk88;
        if (((s8 *)p3)[16] == 0)
        {
            if (gLatchedPressedKeys[p3->unk00] & 1)
            {
                p3->unk01 = 9;
                break;
            }
        }
        if (gTerrainResult[0] == 0)
            break;
        PlayerCheckBump();
        if (((u8 *)gCurTask->unk88)[62] & 7)
            TaskSetEntry(sub_08041940, gCurTaskIdx);
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    PlayerStopAtWall();
}

void sub_08041b8c(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 4;
    gCurTask->unk15 = 6;
    if (gCurTask->unk88->unk05 != 4)
    {
        PlayerSetMotionYPreset(4);
        PlaySfx(264);
        ((s8 *)gCurTask->unk88)[16] = 10;
    }
    gCurTask->unk73 = 1;
    TaskSetFrame(0x11E4);
    TaskSleepForever();
}

void sub_08041bf0(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 5;
    gCurTask->unk15 = 7;
    PlayerSetMotionYPreset(6);
    PlayerPlayBump();
    TaskSetFrame(0x11EC);
    TaskSleepForever();
}

void sub_08041c30(void)
{
    struct Task *t;
    struct PlayerState *p;

    PlayerTurnToHeldDirection();
    while (sub_08040264() == 0 && PlayerCheckLadder() == 0)
    {
        if (PlayerCheckEnterDoor() != 0)
            break;
        if (sub_080400c0() != 0)
            break;
        t = gCurTask;
        if (t->unk7A & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        p = t->unk88;
        if (((s8 *)p)[16] == 0)
        {
            if (gLatchedPressedKeys[p->unk00] & 1)
            {
                p->unk01 = 9;
                break;
            }
        }
        if (gTerrainResult[0] == 0)
            break;
        PlayerCheckBump();
        if (((u8 *)gCurTask->unk88)[62] & 7)
            TaskSetEntry(sub_08041bf0, gCurTaskIdx);
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    if (gCurTask->unk7A & 1)
        PlayerLand(1);
    PlayerStopAtCeilingAndWall();
}

void sub_08041d14(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 14;
    gCurTask->unk15 = 9;
    gCurTask->unk88->unk3D = 0;
    ((s8 *)gCurTask->unk88)[16] = 7;
    PlayerSetMotionYPreset(11);
    PlaySfx(0x109);
    TaskSetFrame(0x11EE);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetFrame(0x11E4);
    TaskSleepForever();
}

void sub_08041dc8(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 v;

    PlayerTurnToHeldDirection();
    while (PlayerCheckLadder() == 0 && sub_080400c0() == 0)
    {
        if ((v = PlayerCheckEnterDoor()) != 0)
            break;
        t = gCurTask;
        if (t->unk7A & 1)
        {
            if (t->unk58 >= 0)
            {
                PlayerCheckBump();
                PlayerLand(0);
                PlayerRequestLocomotion();
                goto end;
            }
            t->unk7A = v;
        }
        p = gCurTask->unk88;
        if (((s8 *)p)[16] == 0)
        {
            if (gLatchedPressedKeys[p->unk00] & 1)
            {
                TaskSetEntry(sub_08041d14, gCurTaskIdx);
                break;
            }
        }
        if (gCurTask->unk58 > 0x10000)
        {
            PlayerSetMotionYPreset(6);
            gCurTask->unk88->unk01 = 7;
        }
        break;
    }
end:
    PlayerSetMotionXPreset(6, 72);
    PlayerStopAtWall();
}

void sub_08041e8c(void)
{
    struct Task *t;
    s16 *q;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 6;
    gCurTask->unk15 = 10;
    t = gCurTask;
    if (t->unk88->unk05 != 6)
    {
        ((u32 **)t->unk88)[25] = gUnk_0873BD28;
        ((u32 **)t->unk88)[26] = gUnk_0873CB24;
        t->unk2C = ((u8 *)t->unk88)[75];
        PlayerSetMotionXPreset(0, 72);
    }
    gCurTask->unk28 = 8;
    q = (s16 *)gUnk_0873D5C0;
    TaskSetFrame(q[sub_0803fd20(gCurTask->unk88->unk00)]);
    TaskSleepForever();
}

void sub_08041f10(void)
{
    struct Task *t;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 7;
    gCurTask->unk15 = 11;
    t = gCurTask;
    if (t->unk88->unk05 != 7)
    {
        t->unk28 = 0;
        t->unk73 = 0;
    }
    switch (gCurTask->unk73)
    {
    case 0:
        PlayerSetMotionXPreset(11, 1);
        TaskSetFrame(0x1201);
        TaskYieldTrampoline(1);
        PlayerStopAxes(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 1;
        /* fallthrough */
    case 1:
        gCurTask->unk80 = 4;
        gCurTask->unk88->unk14 = 10;
        PlayerSetMotionXPreset(11, 0);
        gCurTask->unk88->unk6C = gUnk_0873D03C;
        PlayerStartSfx(118, gCurTask->unk88->unk00);
        CreatePlayerEffect(gCurTask->unk88->unk00, 8, 0);
        TaskSetFrame(0x1203);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        while ((u32)abs(gCurTask->unk54) > 0x7FFF)
            TaskYieldTrampoline(1);
        gCurTask->unk73 = 2;
        /* fallthrough */
    case 2:
        gCurTask->unk88->unk6C = 0;
        gCurTask->unk80 = 0;
        break;
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08042050(void)
{
    struct Task *t;
    struct Task *t2;
    struct PlayerState *p;
    s32 v;
    s32 x;
    s32 k;

    v = sub_0803fe68();
    if (v == 0)
    {
        t = gCurTask;
        k = t->unk73;
        switch (k)
        {
        case 0:
            break;
        case 1:
            if (gTerrainResult[0] != 0)
            {
                PlayerCheckBump();
                PlayerStopAxes(1);
                gCurTask->unk88->unk01 = 1;
                break;
            }
            p = t->unk88;
            if ((s16)p->unk14 != 0)
            {
                p->unk14--;
                if (p->unk14 == 0)
                    PlayerSetMotionXPreset(5, 72);
            }
            t2 = gCurTask;
            x = t2->unk54;
            if (x < 0)
                x = -x;
            if ((u32)x > 0xA000)
                RegisterCollider((u8)gCurTaskIdx, t2->unk48, t2->unk4A, gUnk_0873CA68);
            break;
        case 2:
            if (t->unk28 != 0)
            {
                t->unk88->unk6C = (void *)v;
                t->unk88->unk01 = 1;
            }
            break;
        }
    }
    else
    {
        if (gCurTask->unk5C == 0)
            PlayerSetMotionXPreset(5, 72);
    }
}

/* MATCH (512 bytes).  `xa` is load-bearing, not cosmetic.  It is the FIRST x
   of loop 2 and is referenced only inside that loop`s entry block, so it adds
   a fourth block-local quantity there.  With three quantities (the unk28
   index; the combined i*2/+i/*2 chain; the &gUnk_0873D986 pool address)
   local-alloc takes gcc 2.9`s hand-rolled `case 3:` sort in block_alloc,
   whose comparisons use the literal qty numbers 0/1/2 while EXCHANGE permutes
   qty_order - it swaps twice and leaves the identity order, so the index is
   allocated first and takes r0 while the *6 chain takes r1.  The record
   pointer is then `(set r (plus <chain> <pool>))`, set_preference reads
   XEXP(src,0) = the chain = hard r1, so the pointer allocno prefers r1;
   prune_preferences copies that into regs_someone_prefers[v] (v conflicts
   with it and is higher priority), find_reg pass 0 refuses r1 for v, v takes
   r2 and the pointer takes r1 - the r1/r2 swap seen in BOTH loops.  A fourth
   quantity pushes block_alloc onto the qsort path, which sorts correctly:
   chain->r0, pool->r1, index->r1, xa->r0; the pointer`s preference becomes
   r0, which it conflicts with and is pruned, so v is free to take r1. */
void sub_08042128(void)
{
    struct Task *h1;
    struct Task *h2;
    struct Task *h3;
    struct Task *d;
    struct Task *a1;
    struct Task *b1;
    struct Task *c2;
    struct Task *a2;
    struct Task *b2;
    u16 *q;
    u16 *r;
    s32 v;
    s32 x;
    s32 xa;
    s32 k;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 9;
    gCurTask->unk15 = 12;
    if (gCurTask->unk88->unk05 != 9)
    {
        PlayerStopAxes(3);
        gCurTask->unk43 = 1;
        h1 = gCurTask;
        h1->unk3E &= 0x7FFF;
        h1->unk88->unk3D = 0;
        ((u8 *)gCurTask->unk88)[80] = 1;
        q = gLatchedHeldKeys;
        h2 = gCurTask;
        if (q[h2->unk88->unk00] & 64)
        {
            h2->unk73 = 1;
            gCurTask->unk28 = 0;
        }
        else
        {
            h2->unk73 = 2;
            gCurTask->unk28 = 10;
        }
        h3 = gCurTask;
        h3->unk2C = h3->unk43;
        h3->unk4C = ((h3->unk48 & 0xFFF0) | 8) << 16;
    }
    d = gCurTask;
    d->unk46 = 0x11F4;
    k = d->unk73;
    switch (k)
    {
    case 1:
        d->unk30 = k;
        for (;;)
        {
            if (gCurTask->unk28 == 0 || gCurTask->unk28 == 6)
                PlaySfx(123);
            a1 = gCurTask;
            r = &((u16 *)gUnk_0873D986)[a1->unk28 * 3];
            x = -r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a1->unk58 = v;
            x = r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a1->unk68 = v;
            a1->unk3C = r[0] + ((u16 *)a1)[35];
            TaskYieldTrampoline(r[1]);
            b1 = gCurTask;
            b1->unk28++;
            if (b1->unk28 > 9)
                b1->unk28 = 0;
        }
    case 2:
        c2 = gCurTask;
        c2->unk30 = c2->unk73;
        c2->unk88->unk14 = 0;
        PlaySfx(124);
        for (;;)
        {
            a2 = gCurTask;
            r = &((u16 *)gUnk_0873D986)[a2->unk28 * 3];
            xa = r[2];
            v = xa << 8;
            if (xa & 0x8000)
                v |= 0xFF000000;
            a2->unk58 = v;
            x = r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a2->unk68 = v;
            a2->unk3C = r[0] + ((u16 *)a2)[35];
            TaskYieldTrampoline(r[1]);
            b2 = gCurTask;
            b2->unk28++;
            if (b2->unk28 > 13)
                b2->unk28 = 10;
        }
    case 0:
        PlayerStopAxes(2);
        break;
    }
    TaskSleepForever();
}

/* MATCH (600 bytes).  The two `unk01 = 5` / `unk01 = 1` arms are NOT written
   as an if/else-if chain in the tail: they are labelled statements INSIDE
   case 0, sitting between the `else if ((...&64) != 0)` arm and the third
   arm (which is therefore reached by `goto arm3`).  The tail branches to them
   with the un-inverted conditions (`beq set5` / `bne set1`, both backward),
   and each returns.  gcc emits basic blocks in source order, so that source
   position is what puts the 11 instructions in the hole at 0x080424AE and
   makes the four literal pools land where the ROM has them - the previous
   4-byte residue was pool alignment padding caused purely by block order.
   Corroboration: at 0x08042562 the `movs r3,#1` for the `unk7A & 1` mask is
   still live inside set1 (`strb r3,[r0,#1]`), i.e. the constant 1 is CSEd
   across the goto, which only happens if set1 is a jump target. */
void sub_08042328(void)
{
    struct Task *t;
    struct Task *ta;
    struct Task *tb;
    struct Task *tc;
    struct Task *td;
    struct Task *te;
    struct Task *tf;
    struct PlayerState *p;
    u16 *qa;
    u16 *qb;
    struct Task *tg;
    u16 *qd;
    s32 k;
    s32 n;

    t = gCurTask;
    k = t->unk73;
    switch (k)
    {
    case 1:
        if (gTerrainResult[1] != 0 || (((u8 *)t->unk88)[72] & 4) != 0)
            t->unk58 = 0;
        qa = gLatchedHeldKeys;
        ta = gCurTask;
        if ((qa[ta->unk88->unk00] & 192) == 0)
            ta->unk73 = 0;
        else if ((qa[ta->unk88->unk00] & 128) != 0)
            ta->unk73 = 2;
        if (gCurTask->unk73 == 1)
            break;
        TaskSetEntry(sub_08042128, gCurTaskIdx);
        break;
    case 2:
        p = t->unk88;
        k &= p->unk14;
        if (k != 0)
        {
            PlaySfx(124);
            gCurTask->unk88->unk14 = 0;
        }
        else
        {
            p->unk14++;
        }
        qb = gLatchedHeldKeys;
        tb = gCurTask;
        if ((qb[tb->unk88->unk00] & 192) == 0)
            tb->unk73 = 0;
        else if ((qb[tb->unk88->unk00] & 64) != 0)
            tb->unk73 = 1;
        if (gCurTask->unk73 == 2)
            break;
        TaskSetEntry(sub_08042128, gCurTaskIdx);
        break;
    case 0:
        if ((gLatchedPressedKeys[t->unk88->unk00] & 192) == 0)
            break;
        n = t->unk30;
        if (n == 1)
        {
            if ((gLatchedPressedKeys[t->unk88->unk00] & 64) != 0)
            {
                t->unk73 = n;
                tc = gCurTask;
                tc->unk28++;
                if (tc->unk28 > 9)
                    tc->unk28 = k;
            }
            else
            {
                t->unk73 = 2;
                gCurTask->unk28 = 10;
            }
        }
        else if ((gLatchedPressedKeys[t->unk88->unk00] & 64) != 0)
        {
            t->unk73 = 1;
            gCurTask->unk28 = k;
        }
        else
            goto arm3;
        goto callit;
    set5:
        tg->unk88->unk01 = 5;
        return;
    set1:
        tg->unk88->unk01 = 1;
        return;
    arm3:
        t->unk73 = 2;
        td = gCurTask;
        td->unk28++;
        if (td->unk28 > 13)
            td->unk28 = 10;
    callit:
        TaskSetEntry(sub_08042128, gCurTaskIdx);
        break;
    }
    qd = gLatchedPressedKeys;
    te = gCurTask;
    if ((qd[te->unk88->unk00] & 48) != 0)
    {
        te->unk43 = te->unk2C;
        tf = gCurTask;
        tf->unk3E &= 0x7FFF;
        if (tf->unk58 != 0)
            PlayerStopAxes(2);
        PlayerRequestLocomotion();
    }
    else if ((gTerrainResult[6] & 3) == 0)
    {
        te->unk43 = te->unk2C;
        tg = gCurTask;
        if (tg->unk73 == 1)
            goto set5;
        if (tg->unk7A & 1)
            goto set1;
        tg->unk88->unk01 = 7;
    }
}

void sub_08042580(void)
{
    struct Task *t;
    struct Task *t5;
    struct Task *tj;
    struct Task *tt;
    struct PlayerState *p;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 17;
    gCurTask->unk15 = 16;
    t = gCurTask;
    p = t->unk88;
    if (p->unk05 != 17)
    {
        p->unk42 &= 0xFFEF;
        t->unk73 = 5;
    }
    while (1)
    {
        switch (gCurTask->unk73)
        {
        case 5:
            gCurTask->unk88->unk3D = 0;
            sub_080261d4(2);
            t5 = gCurTask;
            if (t5->unk82 & 128)
                t5->unk73 = 4;
            else
                t5->unk73 = t5->unk82 & 15;
            ((u8 *)gCurTask->unk88)[63] = 1;
            gCurTask->unk88->unk12 = 0x8000;
            PlayerStopAxes(3);
            continue;
        case 6:
            if (gCurTask->unk7B & 1)
                PlayerSetWaterMotionY();
            SetPlayerInvulnerability(1, 96, gCurTask->unk88->unk00);
            TaskSleepForever();
            /* fallthrough */
        case 0:
            PlaySfx(0x107);
            if ((s8)gCurTask->unk7D == 0)
                PlayerSetMotionXPreset(10, 32);
            else
                PlayerSetMotionXPreset(10, 33);
            TaskSetFrame(0x123B);
            TaskYieldTrampoline(16);
            break;
        case 1:
            PlaySfx(0x107);
            PlayerStartOffsetScript(16);
            CreatePlayerEffect(gCurTask->unk88->unk00, 22, 0);
            gCurTask->unk6C = 0;
            do
            {
                TaskSetFrame(0x1244);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk6C++;
            } while ((s16)gCurTask->unk6C <= 3);
            TaskSetFrame(0x1244);
            TaskYieldTrampoline(1);
            tj = gCurTask;
            goto spawn25;
        case 2:
            PlaySfx(0x107);
            CreatePlayerEffect(gCurTask->unk88->unk00, 23, 0);
            gCurTask->unk6C = 0;
            do
            {
                PlayerStartOffsetScript(17);
                TaskSetFrame(0x123B);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x1249);
                TaskYieldTrampoline(2);
                gCurTask->unk6C++;
            } while ((s16)gCurTask->unk6C <= 7);
            TaskSetFrame(0x123B);
            TaskYieldTrampoline(1);
            tj = gCurTask;
        spawn25:
            CreatePlayerEffect(tj->unk88->unk00, 25, 0);
            break;
        case 3:
            PlaySfx(0x107);
            CreatePlayerEffect(gCurTask->unk88->unk00, 24, 0);
            PlayerStartOffsetScript(18);
            TaskSetFrame(0x124B);
            TaskYieldTrampoline(44);
            TaskSetFrame(0x123B);
            TaskYieldTrampoline(1);
            break;
        case 4:
            PlaySfx(0x107);
            gCurTask->unk7A = 0;
            PlayerSetMotionYPreset(30);
            TaskSetFrame(0x123C);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk6C++;
            } while ((s16)gCurTask->unk6C <= 5);
            TaskSetFrame(0x11EC);
            TaskYieldTrampoline(2);
            goto again;
        }
        if ((s8)gCurTask->unk7D == 0)
            PlayerSetMotionXPreset(10, 36);
        else
            PlayerSetMotionXPreset(10, 37);
        PlayerStartOffsetScript(15);
        TaskSetFrame(0x123C);
        TaskYieldTrampoline(3);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
        if ((s8)gCurTask->unk7D == 0)
            PlayerSetMotionXPreset(10, 34);
        else
            PlayerSetMotionXPreset(10, 35);
        PlayerStartOffsetScript(15);
        tt = gCurTask;
        if (tt->unk7A & 1)
        {
            TaskSetFrame(0x11EC);
            TaskYieldTrampoline(3);
        }
        else
        {
            tt->unk3C++;
            TaskYieldTrampoline(3);
        }
        PlayerStopAxes(3);
    again:
        gCurTask->unk73 = 6;
    }
}

void sub_08042980(void)
{
    switch (gCurTask->unk73)
    {
    case 6:
        sub_08040710();
        break;
    case 5:
        break;
    case 4:
        PlayerSetMotionXPreset(7, 72);
    case 0:
    case 1:
    case 2:
    case 3:
        if (PlayerHasCrossedWaterSurface(0))
        {
            gCurTask->unk73 = 6;
            TaskSetEntry(sub_08042580, gCurTaskIdx);
        }
        break;
    }
    PlayerStopAtCeilingAndWall();
}

/* Stage entry (issue #85).  Clears the player's bit in gUnk_03002340,
   decrements gUnk_03002350, installs sub_08042c50 as Task.unk04, spawns five
   sub-tasks through CreatePlayerEffect (id 12 four times, then id 13) and picks a
   random signed 8.8 value into Task.unk54 from the camera x (gSpriteCameraX)
   and gFrameCount.

   Four shapes were load-bearing in the +/-1 chain:
     * `k` is a REAL LOCAL holding the 1 stored into PlayerState.unk22, not a
       literal.  A literal store leaves the RTL as `(set (mem:QI) (const_int 1))`
       and reload invents the register only after cse, so there is no pseudo for
       cse to reuse; with a local, cse follows the TAKEN side of the `beq` into
       this else-arm (that label has LABEL_NUSES == 1) and still knows k == 1 at
       the `& 1`, rewriting `movs r0,#1` into `adds r0,r5,#0` (lesson 3.327).
     * The store must be the FIELD spelling `->unk22 = k = 1`.  `((u8 *)p)[34]`
       or a separate `k = 1;` statement emits the `movs` before the address
       computation; the field form emits address-then-`movs` (the offset 34
       exceeds strb's #31 immediate, so it still becomes `adds r0,#34`).
     * `m` caches `gFrameCount & 1` BEFORE `sign = 1`, which is what puts the
       `movs r5,#1` between the `ands` and the `cmp`.
     * `dx` (the camera delta) must be its own local: the ROM keeps it in r0 and
       the later `x << 8` in r2, and two registers mean two variables (3.322).
       Reusing one `d` for both puts the delta in r2.
   Sharing `k` with `sign` instead lets cse delete `sign = 1` in the first arm.

   `PlayerState.unk12` (u16 at 0x12) had to exist as a real field: the ROM's
   `movs r0,#128; lsls r0,r0,#8; strh r0,[r1,#18]` only comes out of a struct
   member - every `*(u16 *)((u8 *)p + 18)` spelling adds an `adds r0,rN,#0`
   copy (lesson 3.323).  include/task.h now carries it, so the throw-away
   stand-in this body used is gone. */
void sub_080429fc(void)
{
    struct Task *t;
    u16 *q;
    s32 i;
    s32 sign;
    s32 k;
    s32 m;
    s32 dx;
    s32 r;
    s32 d;
    s32 x;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 18;
    gCurTask->unk04 = (u32)sub_08042c50;
    gCurTask->unk08 = 0;
    gUnk_03002350--;
    gUnk_03002340 &= ~(1 << gCurTask->unk88->unk00);
    if (gCurTask->unk82 == 512)
        sub_08027548();
    gCurTask->unk88->unk40 |= 8;
    gCurTask->unk88->unk42 |= 0x100;
    gCurTask->unk73 = 0;
    gCurTask->unk88->unk42 &= 0xFFEF;
    SetPlayerInvulnerability(255, 0, gCurTask->unk88->unk00);
    ((u8 *)gCurTask->unk88)[23] = 0;
    ((u16 *)gCurTask->unk88)[12] = 0;
    q = (u16 *)gCurTask->unk88;
    q[14] = 0;
    q[13] = 0;
    PlayerStopAxes(3);
    gCurTask->unk42 = 4;
    sub_080276ac(gCurTask->unk88->unk00);
    StopAllSfx();
    StopAllSound();
    gUnk_03001F34 = 1;
    TaskSetFrame(0x123B);
    FreezeOtherTasks(15);
    sub_08027204(2);
    if ((gDispCnt & 0x400) == 0)
    {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
    }
    sub_080261d4(4);
    TaskYieldTrampoline(1);
    PlaySfx(158);
    TaskYieldTrampoline(59);
    gCurTask->unk73 = 1;
    for (i = 0; i < 4; i++)
        CreatePlayerEffect(gCurTask->unk88->unk00, 12, i);
    CreatePlayerEffect(gCurTask->unk88->unk00, 13, 0);
    gCurTask->unk88->unk22 = k = 1;
    gCurTask->unk88->unk12 = 0x8000;
    PlayBgm(3);
    if (sub_08022760(gCurTask))
    {
        PlayerSetMotionYPreset(32);
    }
    else
    {
        dx = gCurTask->unk48 - gSpriteCameraX;
        if (dx <= 55)
            sign = 1;
        else if (dx > 160)
            sign = -1;
        else
        {
            m = gFrameCount & 1;
            sign = 1;
            if (m == 0)
                sign = -1;
        }
        r = RandomRange(3) << 8;
        r |= RandomRange(16) << 4;
        if (r <= 255)
            r |= 256;
        t = gCurTask;
        x = r * sign;
        d = x << 8;
        if (x & 0x8000)
            d |= 0xFF000000;
        t->unk54 = d;
        t->unk64 = 0x40000;
        PlayerSetMotionYPreset(33);
    }
    TaskSetFrame(0x124C);
    TaskSleepForever();
}

void sub_08042c50(void)
{
    switch (gCurTask->unk73)
    {
    case 0:
        break;
    case 1:
        if (gCurTask->unk58 > 0)
        {
            gCurTask->unk68 = 0x40000;
            if (gCurTask->unk4A - gSpriteCameraY > 199)
            {
                PlayerStopAxes(2);
                gCurTask->unk88->unk14 = 90;
                gCurTask->unk73 = 2;
            }
        }
        if ((gCurTask->unk88->unk42 & 32) == 0)
            sub_0803e080();
        break;
    case 2:
        if (--gCurTask->unk88->unk14 == 0)
        {
            gUnk_03002438 = 6;
            TaskFree(gCurTaskIdx);
        }
        break;
    }
}

void sub_08042cfc(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 5;
    gCurTask->unk15 = 17;
    PlayerSetMotionXPreset(11, 16);
    PlayerSetMotionYPreset(22);
    gCurTask->unk7A = 0;
    TaskSleepForever();
}

void sub_08042d40(void)
{
    gCurTask->unk88->unk01 = 7;
}

void sub_08042d54(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 19;
    gCurTask->unk04 = 0;
    gCurTask->unk08 = 0;
    gCurTask->unk88->unk3D = 0;
    gUnk_03001F34 = 1;
    PlayerStopAxes(3);
    gCurTask->unk88->unk42 |= 0x100;
    sub_080261d4(0);
    if (gUnk_03002444 == 0)
    {
        StopAllSfx();
        FreezeOtherTasks(15);
        if ((gDispCnt & 0x400) == 0)
        {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1D00;
        }
        TaskYieldTrampoline(1);
    }
    sub_08025024();
    if (gUnk_03002444 != 0)
        ((void (*)(void))sub_080264b0)();
    if (gUnk_03002444 == 0)
        PlaySfx(181);
    if ((gCurTask->unk7B & 1) == 0)
    {
        TaskSetFrame(0x1208);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
    }
    else
    {
        TaskSetFrame(0x1208);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
    }
    gCurTask->unk3C++;
    TaskSleepForever();
}

void sub_08042e98(void)
{
    s32 a;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 19;
    gCurTask->unk15 = 19;
    TaskInitWaterFlags();
    sub_08021c74((s32)gPlayerDefaultTerrainBox, gCurTaskIdx);
    gCurTask->unk28 = 0;
    gCurTask->unk3C = -1;
    if (gUnk_0200AF00 == 1)
    {
        ((void (*)(void))sub_08027a60)();
        a = ((s32 (*)(void))sub_0802653c)();
        TaskYieldTrampoline(1);
        sub_08026704(a);
    }
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    a = ((s32 (*)(void))sub_080264b0)();
    gCurTask->unk3C = -1;
    TaskYieldTrampoline(4);
    gCurTask->unk3E = 0x2000;
    PlayerSetMotionXPreset(10, 9);
    gCurTask->unk3C = 0x120B;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    sub_0802651c(a);
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    PlayerStopAxes(1);
    TaskSetFrameByFacing(0x1208);
    TaskYieldTrampoline(2);
    {
        s16 *p = (s16 *)gUnk_0873D206;
        TaskSetFrame(p[((s32 (*)(s32))sub_0803fd20)((s8)gCurTask->unk88->unk00)]);
    }
    if (gUnk_0200AF00 == 1)
    {
        TaskSetSkipMask(14, gCurTaskIdx);
        sub_0802672c();
        while (gUnk_020055E8 == 0)
            TaskYieldTrampoline(1);
        ((void (*)(void))sub_08027a60)();
        TaskSetSkipMask(0, gCurTaskIdx);
    }
    gCurTask->unk3E = 0x4000;
    ((void (*)(void))sub_08026584)();
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08043014(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 15;
    gCurTask->unk15 = 20;
    if (gCurTask->unk88->unk05 != 15)
    {
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x30)
            gCurTask->unk73 = 3;
        else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x41)
            gCurTask->unk73 = 1;
        else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x80)
            gCurTask->unk73 = 2;
        else
            gCurTask->unk73 = 0;
        gCurTask->unk2C = gCurTask->unk73;
        PlayerSetWaterMotionY();
        ((u8 *)gCurTask->unk88)[61] = 0;
    }
    switch (gCurTask->unk73)
    {
    case 0:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        if (gCurTask->unk2C == 2)
        {
            TaskSetFrame(0x1239);
            TaskYieldTrampoline(3);
        }
        TaskSetFrame(0x1238);
        break;
    case 1:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        while (1)
        {
        lab1:
            gCurTask->unk28 = 0;
            PlaySfx(120);
            if (((u8 *)gCurTask->unk88)[92] & 1)
            {
                PlayerSetMotionYPreset(15);
                TaskSetFrame(0x1234);
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                TaskSetFrame(0x1230);
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                gCurTask->unk6C = 0;
                do
                {
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                } while ((s16)++gCurTask->unk6C <= 4);
                gCurTask->unk68 = 0x10000;
            }
            TaskSetFrame(0x1233);
            gCurTask->unk6C = 0;
            do
            {
                if (gCurTask->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 14);
            {
                if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x41)
                    goto lab1;
            }
            gCurTask->unk6C = 0;
            do
            {
                if (gCurTask->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 44);
            gCurTask->unk28 = -1;
            TaskSleepForever();
        }
    case 2:
        PlayerSetMotionYPreset(14);
        PlayerSetMotionXPreset(11, 4);
        gCurTask->unk88->unk14 = 15;
        TaskSetFrame(0x1239);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        break;
    case 3:
        PlayerSetMotionXPreset(11, 3);
        gCurTask->unk28 = 0;
        if (((u8 *)gCurTask->unk88)[92] & 1)
        {
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x41)
                PlayerSetMotionYPreset(12);
            else
                PlayerSetMotionYPreset(13);
        }
        else
        {
            PlayerSetMotionYPreset(13);
            gCurTask->unk28 = 10;
        }
        gCurTask->unk88->unk14 = 15;
        while (1)
        {
            PlaySfx(120);
            TaskSetFrame(0x1228);
            TaskYieldTrampoline(5);
            gCurTask->unk6C = 0;
            do
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(5);
            } while ((s16)++gCurTask->unk6C <= 6);
        }
    }
    TaskSleepForever();
}
