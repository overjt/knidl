#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_343c0.c (0x080343C0-0x08034F8B, issue #92).
 *
 * Player mode bodies, part 2.  The player task starts the "enter"
 * coroutine of the requested action PlayerState.unk02 from
 * gPlayerActions[62] and every frame the "per-frame" handler Task.unk15
 * from gPlayerActionHandlers[57] (CallTableEntry(index, count, table); entry 0 of
 * both tables is NULL).  Here: actions 3-6 and 22.
 * sub_080343c0 enters mode 2 (handler 3, sub_0803469c), sub_08034874
 * mode 3 (handler 4, sub_080349b4), sub_08034a88 and sub_08034d34 mode 4
 * (handlers 5 and 6, sub_08034bec and sub_08034e60); sub_08034f70
 * (action 22) clears PlayerState.unk68 and runs sub_08034f8c, the
 * mode-5 coroutine of the next file.  The enter coroutines switch on the
 * ability PlayerState.unk0D for the animation (TaskSetFrame) and loop
 * on TaskYieldTrampoline; the handlers run M11's transition predicates
 * and write the next request into PlayerState.unk01. */

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern u16 gLatchedHeldKeys[];             /* latched state mask per player (M11) */
extern u16 gUnk_0873D31C[];
extern u16 gUnk_0873D350[];
extern struct Unk03005550 gUnk_03005550;
extern u16 gUnk_0873D384[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 sub_08022624(u16 x, u16 y);
void sub_08034f8c(void);
void PlayerPlayBump(void);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerStartSfx(s32 a0, u16 a1);
s32 PlayerLand(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerCheckBump(void);
s32 PlayerStopAtWall(void);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fd90(void);
s32 PlayerCheckJump(void);
s32 sub_0803fe68(void);
s32 PlayerCheckDuckOrSwallow(void);
s32 PlayerCheckLadder(void);
s32 PlayerCheckFloat(void);
s32 sub_08040084(void);
s32 sub_080400c0(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 band, s32 id, s32 payload);

void sub_080343c0(void)
{
    struct Task *t;
    struct PlayerState *p;
    u16 *q;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 2;
    gCurTask->unk15 = 3;
    t = gCurTask;
    t->unk28 = 0;
    t->unk2C = -1;
    PlayerSetMotionXPreset(3, 72);
    if (gCurTask->unk88->unk05 != 2)
    {
        gCurTask->unk88->unk46 = 0;
        q = gLatchedHeldKeys;
        p = gCurTask->unk88;
        if (q[p->unk00] & 48)
        {
            if (p->unk3E == 2)
                p->unk3E = 0;
        }
        PlayerPlayBump();
        PlayerStartSfx(117, gCurTask->unk88->unk00);
        CreatePlayerEffect(gCurTask->unk88->unk00, 7, 0);
        if (sub_0803fd20(gCurTask->unk88->unk00) == 4)
            gCurTask->unk73 = 1;
        else
            gCurTask->unk73 = 0;
    }
    if (gCurTask->unk73 == 0)
    {
        if (gCurTask->unk88->unk06 == 1)
        {
            while (1)
            {
                TaskSetFrame(0x153);
                TaskYieldTrampoline(2);
                gCurTask->unk6C = 0;
                do
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk6C++;
                } while ((s16)gCurTask->unk6C <= 3);
                TaskSetFrame(0x148);
                TaskYieldTrampoline(2);
                gCurTask->unk6C = 0;
                do
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk6C++;
                } while ((s16)gCurTask->unk6C <= 9);
            }
        }
        else
        {
            gCurTask->unk46 = gUnk_0873D31C[gCurTask->unk88->unk0D];
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->unk6C = 0;
                do
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(3);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk6C++;
                } while ((s16)gCurTask->unk6C <= 2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
    }
    CreatePlayerEffect(gCurTask->unk88->unk00, 6, 0x200);
    if (gCurTask->unk88->unk06 == 1)
        gCurTask->unk46 = 0x15D;
    else
        gCurTask->unk46 = gUnk_0873D350[gCurTask->unk88->unk0D];
    switch (gCurTask->unk88->unk0D)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void sub_0803469c(void)
{
    struct Task *t;
    struct Task *t3;
    struct PlayerState *p;
    struct PlayerState *p4;
    u16 *q;
    s32 x;
    s32 m;
    s32 m2;
    s32 m5;
    s32 y;
    s32 m3;

    t = gCurTask;
    if (t->unk28 == 0)
    {
        m2 = gUnk_03005550.unkD;
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
            if (sub_08022624(((u16 *)t)[36],
                             (y = ((u16 *)t)[37], m3 = -16, m3 &= y, m3 + 16)) != 0)
                gCurTask->unk7A = 1;
        }
        else
        {
            p->unk14--;
        }
    }
    t = gCurTask;
    if ((t->unk7A & 1) != 0 || (t->unk88->unk48 & 3) != 0)
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
        if (PlayerCheckFloat() != 0)
            goto end;
        if (sub_080400c0() != 0)
            goto end;
        if (PlayerCheckDropAbility() != 0)
            goto end;
        q = gLatchedHeldKeys;
        t3 = gCurTask;
        p4 = t3->unk88;
        m = q[p4->unk00] & 48;
        if (m == 0)
        {
            x = abs(t3->unk54);
            if ((u32)x <= 0x14BFF)
            {
                p4->unk3D = m;
                gCurTask->unk88->unk01 = 2;
                goto end;
            }
        }
        m5 = gUnk_03005550.unk0;
        if (m5 != 0)
        {
            PlayerCheckBump();
            gCurTask->unk88->unk01 = 1;
            goto end;
        }
        if (gCurTask->unk73 == 0)
        {
            if (sub_0803fd20(gCurTask->unk88->unk00) == 4)
            {
                gCurTask->unk73 = 1;
                TaskSetEntry(sub_080343c0, gCurTaskIdx);
            }
        }
        else if (sub_0803fd20(gCurTask->unk88->unk00) != 4)
        {
            gCurTask->unk73 = m5;
            TaskSetEntry(sub_080343c0, gCurTaskIdx);
        }
        goto end;
    }
end:
    PlayerSetMotionXPreset(3, 72);
}

void sub_08034874(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 3;
    gCurTask->unk15 = 4;
    PlayerSetMotionXPreset(4, 72);
    if (gCurTask->unk88->unk05 != 3)
    {
        PlaySfxIfLocalPlayer(119, gCurTask->unk88->unk00);
        CreatePlayerEffect(gCurTask->unk88->unk00, 6, 0);
    }
    if (gCurTask->unk88->unk06 == 1)
        gCurTask->unk46 = 0x15D;
    else
        gCurTask->unk46 = gUnk_0873D350[gCurTask->unk88->unk0D];
    switch (gCurTask->unk88->unk0D)
    {
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void sub_080349b4(void)
{
    while (PlayerCheckJump() == 0 && sub_0803fe68() == 0 && sub_080400c0() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gCurTask->unk54 == 0)
        {
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
            {
                if (gCurTask->unk43 == 1)
                {
                    if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 32)
                        gCurTask->unk43 = -1;
                }
                else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 16)
                {
                    gCurTask->unk43 = 1;
                }
                gCurTask->unk5C = 0;
                if (gCurTask->unk88->unk3D == 0)
                    gCurTask->unk88->unk01 = 2;
                else
                    gCurTask->unk88->unk01 = 3;
            }
            else
            {
                gCurTask->unk88->unk01 = 1;
            }
            if (gCurTask->unk88->unk01 != 0)
                break;
        }
        if (gUnk_03005550.unk0 != 0)
        {
            PlayerCheckBump();
            gCurTask->unk88->unk01 = 1;
        }
        break;
    }
}

void sub_08034a88(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 4;
    gCurTask->unk15 = 5;
    if (gCurTask->unk88->unk05 != 4)
    {
        if (gCurTask->unk88->unk05 == 9)
            gCurTask->unk88->unk14 = 4;
        else
            gCurTask->unk88->unk14 = 23;
        PlayerSetMotionYPreset(0);
        PlaySfxIfLocalPlayer(100, gCurTask->unk88->unk00);
        gCurTask->unk73 = 0;
    }
    PlayerPlayBump();
    if (gCurTask->unk88->unk06 == 1)
    {
        TaskSetFrame(0x158);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskSleepForever();
    }
    gCurTask->unk46 = gUnk_0873D384[gCurTask->unk88->unk0D];
    switch (gCurTask->unk88->unk0D)
    {
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    case 1:
    case 2:
    case 4:
    case 5:
    case 9:
    case 10:
    case 15:
    case 16:
    case 17:
    case 19:
    case 22:
    case 23:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    }
}

void sub_08034bec(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckLadder() == 0 && sub_080400c0() == 0 && PlayerCheckDropAbility() == 0 && PlayerCheckEnterDoor() == 0)
    {
        if (gCurTask->unk7A & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            goto end;
        }
        if (gUnk_03005550.unk1 != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(2);
            gCurTask->unk88->unk01 = 7;
            break;
        }
        switch (gCurTask->unk73)
        {
        case 0:
            if (--gCurTask->unk88->unk14 == 0
                || (gLatchedHeldKeys[gCurTask->unk88->unk00] & 1) == 0)
            {
                gCurTask->unk73 = 1;
                PlayerSetMotionYPreset(1);
                gCurTask->unk88->unk14 = 6;
            }
            break;
        case 1:
            if (sub_08040084() == 0 && --gCurTask->unk88->unk14 == 0)
            {
                PlayerStopAxes(2);
                PlayerSetMotionYPreset(2);
                gCurTask->unk88->unk01 = 7;
            }
            break;
        }
        if (gUnk_03005550.unk0 != 0)
        {
            PlayerCheckBump();
            if (gCurTask->unk88->unk3E & 7)
                TaskSetEntry(sub_08034a88, gCurTaskIdx);
        }
        break;
    }
end:
    PlayerSetMotionXPreset(7, 72);
    PlayerStopAtWall();
}

void sub_08034d34(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 4;
    gCurTask->unk15 = 6;
    if (gCurTask->unk88->unk05 != 4)
    {
        gCurTask->unk73 = 0;
        PlayerSetMotionYPreset(0);
        PlaySfxIfLocalPlayer(100, gCurTask->unk88->unk00);
    }
    gCurTask->unk46 = gUnk_0873D384[gCurTask->unk88->unk0D];
    switch (gCurTask->unk88->unk0D)
    {
    case 1:
    case 2:
    case 4:
    case 5:
    case 9:
    case 10:
    case 15:
    case 16:
    case 17:
    case 19:
    case 22:
    case 23:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 0:
    default:
        TaskSetFrame(gCurTask->unk46);
        TaskSleepForever();
    }
}

void sub_08034e60(void)
{
    PlayerTurnToHeldDirection();
    while (sub_080400c0() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gCurTask->unk7A & 1)
        {
            PlayerCheckBump();
            PlayerLand(0);
            PlayerRequestLocomotion();
            break;
        }
        if (gUnk_03005550.unk1 != 0)
        {
            PlayerCheckBump();
            PlayerStopAxes(2);
            gCurTask->unk88->unk01 = 7;
            break;
        }
        switch (gCurTask->unk73)
        {
        case 0:
            if (--gCurTask->unk88->unk14 == 0)
            {
                gCurTask->unk73 = 1;
                PlayerSetMotionYPreset(1);
                gCurTask->unk88->unk14 = 5;
            }
            break;
        case 1:
            if (--gCurTask->unk88->unk14 == 0)
            {
                PlayerStopAxes(2);
                PlayerSetMotionYPreset(2);
                gCurTask->unk88->unk01 = 7;
            }
            break;
        }
        if (gUnk_03005550.unk0 != 0)
        {
            PlayerCheckBump();
            if (gCurTask->unk88->unk3E & 7)
                TaskSetEntry(sub_08034a88, gCurTaskIdx);
        }
        break;
    }
    PlayerSetMotionXPreset(7, 72);
    PlayerStopAtWall();
}

void sub_08034f70(void)
{
    gCurTask->unk88->unk68 = 0;
    sub_08034f8c();
}
