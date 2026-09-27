#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4f948.c (0x0804F948-0x0804FFDB, issue #90).
 *
 * Player action bodies, part 26: actions 56-58 and per-frame handlers
 * 53-55.  sub_0804f948 (action 56, mode 13) installs the collider row
 * gUnk_0873C2C8, plays sound 155 and animations 0x1026/0x102A and spawns
 * task type #6's variant 11 through sub_08053a44; its handler sub_0804fab0
 * re-binds it and requests action 2 once the animation ends.
 * sub_0804fba4 (action 57, mode 13) plays animation 0xFE5 with the collider
 * row gUnk_0873C304; its handler sub_0804fc98 is a `switch (Task.unk73)`
 * with M11's steering and camera presets.  Action 58 is a second move set
 * one level down: sub_0804fe68 (mode 13) dispatches Task.unk73 through its
 * four sub-actions gUnk_0873B6AC and sub_0804fee8 is the re-entry callback;
 * its handler sub_0804ff1c runs the sub-handler Task.unk73 of
 * gUnk_0873B6BC, the steering helper sub_080506dc, clamps the player to
 * 16-224 x 18-132 and, once gSpriteCameraY passes 888, subtracts the
 * player's whole health (AddPlayerHealth) and requests action 17. */

struct M11R20 { u32 w[5]; };

extern struct M11R20 gPlayerBodyBoxes[];
extern u32 gUnk_0873C2C8[];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern u32 gUnk_0873C2DC[];
extern u32 gUnk_0873C304[];
extern u32 gUnk_0873C318[];
extern void (*gUnk_0873B6AC[])(void);   /* enter 58's sub-actions [4] */
extern void (*gUnk_0873B6BC[])(void);   /* handler 55's per-frame sub-handlers [4] */
extern s16 gSpriteCameraY;               /* scalar, read with ldrsh (32 landed files) */
extern s16 gPlayerHealth[];             /* per-player health (M02's HUD) */

void TaskYieldTrampoline(s32 frames);
/* task / sprite services (landed prototypes) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 AddPlayerHealth(s32 a, s32 b);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerLand(s32 a0);
s32 sub_0803e55c(void);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
void PlayerTurnToHeldDirection(void);
void PlayerCheckBump(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerCheckFloat(void);
s32 sub_080400c0(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void sub_080506dc(void);
s32 sub_08053a44(s8 player, u8 variant, s32 arg);

void sub_0804f948(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 53;
    gCurTask->unk80 = 25;
    {
        struct Task *t = gCurTask;
        t->unk70 = 0;
        t->unk2C = -1;
        gPlayerBodyBoxes[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873C2C8;
    }
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        PlaySfxIfLocalPlayer(155, (u16)t->unk88->unk00);
    }
    TaskSetFrame(0x1026);
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskSetFrame(0x102A);
    TaskYieldTrampoline(1);
    sub_08053a44(gCurTask->unk88->unk00, 11, 0);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
    {
        struct Task *t = gCurTask;
        t->unk2C++;
        t->unk3C++;
        TaskYieldTrampoline(1);
    }
    {
        struct Task *t = gCurTask;
        t->unk2C = -1;
        t->unk3C++;
    }
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gCurTask;
        t->unk28++;
        t->unk3C++;
    }
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk70++;
    TaskSleepForever();
}

void sub_0804fab0(void)
{
    struct Task *t = gCurTask;

    if ((s16)t->unk70 != 0)
    {
        PlayerRequestLocomotion();
    }
    else
    {
        if (t->unk28 != 0)
        {
            if (gLatchedPressedKeys[t->unk88->unk00] & 2)
            {
                TaskSetEntry(sub_0804f948, gCurTaskIdx);
            }
            else if ((t->unk7A & 1) && (gLatchedHeldKeys[t->unk88->unk00] & 0x30))
            {
                PlayerTurnToHeldDirection();
                gCurTask->unk88->unk01 = 2;
            }
        }
        {
            struct Task *u = gCurTask;
            if (u->unk2C != -1)
            {
                LoadPlayerBodyBoxRect(u->unk88->unk00, (u8 *)gUnk_0873C2DC + u->unk2C * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                             &gPlayerBodyBoxes[gCurTask->unk88->unk00]);
            }
        }
    }
    sub_0803e55c();
}

void sub_0804fba4(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 54;
    gCurTask->unk73 = 0;
    gCurTask->unk88->unk14 = 23;
    PlayerSetMotionYPreset(0);
    PlaySfxIfLocalPlayer(100, (u16)gCurTask->unk88->unk00);
    {
        struct Task *t = gCurTask;
        t->unk2C = t->unk43;
        t->unk80 = 25;
    }
    {
        struct Task *t = gCurTask;
        t->unk28 = -1;
        gPlayerBodyBoxes[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873C304;
    }
    while (1)
    {
        {
            struct Task *t = gCurTask;
            t->unk28++;
            PlaySfxIfLocalPlayer(0x11D, (u16)t->unk88->unk00);
        }
        TaskSetFrame(0xFE5);
        TaskYieldTrampoline(1);
        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 6; gCurTask->unk6E++)
        {
            struct Task *t = gCurTask;
            t->unk28++;
            t->unk3C++;
            TaskYieldTrampoline(1);
        }
        gCurTask->unk28 = -1;
    }
}

void sub_0804fc98(void)
{
    struct Task *t = gCurTask;

    switch (t->unk73)
    {
    case 0:
        if (--t->unk88->unk14 == 0 || !(gLatchedHeldKeys[t->unk88->unk00] & 1))
        {
            t->unk73 = 1;
            PlayerSetMotionYPreset(1);
            gCurTask->unk88->unk14 = 5;
        }
        break;
    case 1:
        if (--t->unk88->unk14 == 0)
        {
            PlayerStopAxes(2);
            PlayerSetMotionYPreset(2);
            gCurTask->unk73 = 2;
        }
        break;
    case 2:
        break;
    }
    if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x30)
    {
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x10)
            gCurTask->unk2C = 1;
        else
            gCurTask->unk2C = -1;
    }
    if (PlayerCheckFloat() == 0)
    {
        if (sub_080400c0() == 0)
        {
            if (gCurTask->unk7A & 1)
            {
                PlayerCheckBump();
                PlayerRequestLocomotion();
                gCurTask->unk43 = gCurTask->unk2C;
            }
        }
        else
        {
            gCurTask->unk43 = gCurTask->unk2C;
        }
    }
    {
        struct Task *u = gCurTask;
        if (u->unk28 != -1)
        {
            LoadPlayerBodyBoxRect(u->unk88->unk00, (u8 *)gUnk_0873C318 + u->unk28 * 8);
            RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                         &gPlayerBodyBoxes[gCurTask->unk88->unk00]);
        }
    }
    if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x30)
    {
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x10)
            PlayerSetMotionXPreset(10, 70);
        else
            PlayerSetMotionXPreset(10, 71);
    }
    else
    {
        PlayerSetMotionXPreset(11, 2);
    }
    if (!(gCurTask->unk7A & 1))
        PlayerSetMotionYPreset(2);
    else
        PlayerLand(1);
    PlayerStopAtCeilingAndWall();
}

void sub_0804fe68(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 55;
    if (gCurTask->unk88->unk05 != 13)
    {
        gCurTask->unk88->unk3D = 0;
        gCurTask->unk88->unk14 = 0;
        gCurTask->unk88->unk10 = 0;
        PlayerStopAxes(3);
        gCurTask->unk73 = 0;
        gCurTask->unk80 = 0;
        gCurTask->unk08 = 0;
        gCurTask->unk88->unk64 = 0;
        gCurTask->unk88->unk68 = 0;
        gCurTask->unk88->unk6C = 0;
    }
    CallTableEntry(gCurTask->unk73, 4, gUnk_0873B6AC);
}

void sub_0804fee8(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    CallTableEntry(gCurTask->unk73, 4, gUnk_0873B6AC);
}

void sub_0804ff1c(void)
{
    CallTableEntry(gCurTask->unk73, 4, gUnk_0873B6BC);
    sub_080506dc();
    {
        struct Task *t = gCurTask;
        if (t->unk48 < 16)
        {
            t->unk48 = 16;
            t->unk4C = 16 << 16;
        }
        else if (t->unk48 > 224)
        {
            t->unk48 = 224;
            t->unk4C = 224 << 16;
        }
    }
    {
        struct Task *t = gCurTask;
        if (t->unk4A < 18)
        {
            t->unk4A = 18;
            t->unk50 = 18 << 16;
        }
        else if (t->unk4A > 132)
        {
            t->unk4A = 132;
            t->unk50 = 132 << 16;
        }
    }
    if (gSpriteCameraY > 888)
    {
        gCurTask->unk7C = 1;
        AddPlayerHealth(-gPlayerHealth[gCurTask->unk88->unk00], gCurTask->unk88->unk00);
        gCurTask->unk88->unk01 = 17;
    }
}
