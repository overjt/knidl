#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_3aa64.c (0x0803AA64-0x0803BDE7, issue #91).
 *
 * Player action bodies, part 9: actions 18 and 23-28, per-frame handlers
 * 17 and 20-25.  gLatchedHeldKeys[player] is the held-keys mask the bodies
 * test (0x30 left/right, 0x41 A or up, 0x80 down; gLatchedPressedKeys[] is the
 * newly-pressed one).  sub_0803aa64 (action 23) is the twin of M11's
 * sub_08043014: a four-state machine over Task.unk73 picked from the keys
 * (left/right = 3, A/up = 1, down = 2, else 0) that plays a row of
 * gUnk_0873D9DA[4][4] chosen by the ability; its per-frame handler 20,
 * sub_0803afcc, re-picks the state and re-binds the coroutine when the
 * keys change.  Actions 24-28 (sub_0803b3c4, sub_0803b4f8, sub_0803b768,
 * sub_0803b87c, sub_0803b9a0) are short animation scripts, the last a
 * four-way directional pick; handlers 21-25 run M11's predicates and
 * request the next action through PlayerState.unk01.  sub_0803bd90
 * (action 18) installs handler 17, the leaf sub_0803bdd4. */

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern s16 gUnk_0873D9DA[4][4];
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern s16 gUnk_0300244C;

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetFrame(s32 a);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerCheckDuckOrSwallow(void);
s32 sub_080400c0(void);
s32 PlayerCheckEnterDoor(void);
s32 PlayerCheckDropAbility(void);
s32 sub_080403e4(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void sub_0803aa64(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s16 *anim;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 15;
    gCurTask->unk15 = 20;
    t = gCurTask;
    if (t->unk88->unk06 != 0)
    {
        if (t->unk88->unk05 != 15)
        {
            t->unk73 = 0;
            u = gCurTask;
            u->unk88->unk14 = 1;
            if (gLatchedHeldKeys[u->unk88->unk00] & 65)
                u->unk28 = 2;
            else
                u->unk28 = 5;
            PlayerSetWaterMotionY();
        }
        while (1)
        {
            TaskSetFrame(364);
            TaskYieldTrampoline(gCurTask->unk28);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(gCurTask->unk28);
            }
            TaskSetFrame(0x161);
            TaskYieldTrampoline(gCurTask->unk28);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 9; gCurTask->unk6C++)
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(gCurTask->unk28);
                if (gCurTask->unk3C == 356)
                    PlaySfxIfLocalPlayer(135, gCurTask->unk88->unk00);
            }
            PlaySfxIfLocalPlayer(135, gCurTask->unk88->unk00);
        }
    }
    v = gCurTask;
    if (v->unk88->unk05 != 15)
    {
        if (gLatchedHeldKeys[v->unk88->unk00] & 48)
            v->unk73 = 3;
        else if (gLatchedHeldKeys[v->unk88->unk00] & 65)
            v->unk73 = 1;
        else if (gLatchedHeldKeys[v->unk88->unk00] & 128)
            v->unk73 = 2;
        else
            v->unk73 = 0;
        PlayerSetWaterMotionY();
        gCurTask->unk2C = 0;
    }
    gCurTask->unk88->unk3D = 0;
    switch (gCurTask->unk88->unk0D)
    {
    default:
        anim = gUnk_0873D9DA[0];
        break;
    case 4:
        anim = gUnk_0873D9DA[1];
        break;
    case 9:
        anim = gUnk_0873D9DA[2];
        break;
    case 10:
        anim = gUnk_0873D9DA[3];
        break;
    }
    switch (gCurTask->unk73)
    {
    case 0:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        if (gCurTask->unk2C != 0)
        {
            gCurTask->unk2C = 0;
            TaskSetFrame(anim[3]);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C--;
            TaskYieldTrampoline(3);
        }
        TaskSetFrame(anim[0]);
        break;
    case 1:
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
        gCurTask->unk46 = anim[1];
        while (1)
        {
        lab1:
            PlaySfxIfLocalPlayer(120, gCurTask->unk88->unk00);
            gCurTask->unk28 = 0;
            if (gCurTask->unk88->unk5C & 1)
            {
                PlayerSetMotionYPreset(15);
                TaskSetFrame((s16)(gCurTask->unk46 + 1));
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->unk6C = 0;
                do
                {
                    gCurTask->unk3C++;
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                    if (gCurTask->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                } while ((s16)++gCurTask->unk6C <= 4);
                gCurTask->unk68 = 0x10000;
            }
            TaskSetFrame(gCurTask->unk46);
            gCurTask->unk6C = 0;
            do
            {
                if (gCurTask->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 14);
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 65)
                goto lab1;
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
        TaskSetFrame(anim[3]);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        break;
    case 3:
        PlayerSetMotionXPreset(11, 3);
        gCurTask->unk28 = 0;
        if (gCurTask->unk88->unk5C & 1)
        {
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 65)
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
        gCurTask->unk46 = anim[2];
        while (1)
        {
            PlaySfxIfLocalPlayer(120, gCurTask->unk88->unk00);
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
        }
    }
    TaskSleepForever();
}

void sub_0803afcc(void)
{
    struct Task *t;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct PlayerState *q;
    u8 *st;

    if (PlayerCheckDropAbility() != 0)
        return;
    PlayerTurnToHeldDirection();
    t = gCurTask;
    if (t->unk88->unk06 != 0)
    {
        if (t->unk58 < 0)
        {
            t->unk28 = 2;
        }
        else
        {
            t->unk28 = 5;
            t->unk88->unk14 = 1;
        }
        if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 65) && --gCurTask->unk88->unk14 == 0)
        {
            gCurTask->unk88->unk14 = 15;
            PlayerSetMotionYPreset(18);
        }
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128)
            PlayerSetMotionYPreset(17);
        else
            PlayerSetMotionYPreset(16);
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
            PlayerSetMotionXPreset(11, 3);
        else
            PlayerSetMotionXPreset(11, 4);
    }
    else
    {
        st = &t->unk73;
        switch (*st)
        {
        case 0:
            if (gLatchedHeldKeys[t->unk88->unk00] & 48)
                goto set3a;
            if (gLatchedHeldKeys[t->unk88->unk00] & 65)
                goto set1a;
            if (!(gLatchedHeldKeys[t->unk88->unk00] & 128))
                break;
            *st = 2;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        case 1:
            /* The do { } while (0) changes no code: it counts case 1's
               references one loop level deeper (lessons 3.383, 3.412),
               which lets the key-mask value win its register ahead of
               the switch value, as in the ROM. */
            do
            {
                if ((gLatchedHeldKeys[t->unk88->unk00] & 193) == 128)
                    goto set2b;
                if (gLatchedPressedKeys[t->unk88->unk00] & 48)
                    goto set3b;
                if (t->unk28 == -1)
                    goto set0b;
                if (gLatchedPressedKeys[t->unk88->unk00] & 65)
                    t->unk28 = 1;
            } while (0);
            break;
        case 2:
            t->unk2C = 0;
            if (gLatchedHeldKeys[t->unk88->unk00] & 65)
                goto set1c;
            if (gLatchedHeldKeys[t->unk88->unk00] & 48)
                goto set3c;
            if (!(gLatchedHeldKeys[t->unk88->unk00] & 240))
                goto set0c;
            if ((s16)t->unk88->unk14 != 0)
                t->unk88->unk14--;
            break;
        set1d:
            *st = 1;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            goto keys;
        set2d:
            *st = 2;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            goto keys;
        dec2:
            t->unk88->unk14--;
            goto keys;
        case 3:
            if (gLatchedHeldKeys[t->unk88->unk00] & 48)
                goto keys3;
            if (gLatchedHeldKeys[t->unk88->unk00] & 65)
                goto set1d;
            if (gLatchedHeldKeys[t->unk88->unk00] & 128)
                goto set2d;
            if ((s16)t->unk88->unk14 != 0)
                goto dec2;
            if (gLatchedHeldKeys[t->unk88->unk00] & 241)
                goto keys;
            if (t->unk58 < 0)
                goto keys;
            *st = 0;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
        keys:
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
            {
            keys3:
                PlayerSetMotionXPreset(11, 3);
            }
            else
                PlayerSetMotionXPreset(11, 4);
            v = gCurTask;
            if (v->unk28 != 0)
                goto dec;
            if (gLatchedHeldKeys[v->unk88->unk00] & 65)
                PlayerSetMotionYPreset(12);
            else
                PlayerSetMotionYPreset(13);
            break;
        set3a:
            *st = 3;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set1a:
            *st = 1;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set2b:
            *st = 2;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set3b:
            *st = 3;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set0b:
            *st = 0;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set1c:
            *st = 1;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set3c:
            *st = 3;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set0c:
            *st = 0;
            gCurTask->unk2C = 1;
            TaskSetEntry(sub_0803aa64, gCurTaskIdx);
            break;
        set25:
            w->unk88->unk01 = 25;
            goto out;
        dec:
            v->unk28--;
            PlayerSetMotionYPreset(13);
        }
    }
    if (sub_080400c0() == 0 && PlayerCheckEnterDoor() == 0)
    {
        x = gCurTask;
        if (!(x->unk7B & 1))
        {
            if ((gLatchedHeldKeys[(q = x->unk88)->unk00] & 64) && q->unk06 != 1)
                q->unk01 = 9;
            else
                q->unk01 = 5;
            gCurTask->unk88->unk3D = 0;
            gCurTask->unk88->unk0F = 0;
        }
        else
        {
            q = x->unk88;
            if (q->unk06 != 0 || x->unk73 != 1 || !(gLatchedHeldKeys[q->unk00] & 65))
            {
                w = gCurTask;
                if (w->unk58 != 0 && (w->unk7A & 1))
                {
                    if (w->unk54 != 0)
                        goto set25;
                    w->unk88->unk01 = 24;
                }
            }
        }
    }
out:
    PlayerStopAtCeilingAndWall();
    if (gCurTask->unk7A & 1)
        PlayerLand(0);
}

void sub_0803b3c4(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 0;
    gCurTask->unk15 = 21;
    if (gCurTask->unk88->unk05 != 0)
    {
        PlayerStopAxes(3);
        gCurTask->unk28 = gCurTask->unk88->unk4A;
    }
    if (gCurTask->unk88->unk06 == 1)
    {
        gCurTask->unk46 = 352;
    }
    else
    {
        switch (gCurTask->unk88->unk0D)
        {
        default:
            gCurTask->unk46 = 221;
            break;
        case 4:
            gCurTask->unk46 = 1184;
            break;
        case 9:
            gCurTask->unk46 = 0x7C1;
            break;
        case 10:
            gCurTask->unk46 = 0x8C1;
            break;
        }
    }
    TaskSetFrame(gCurTask->unk46);
    TaskSleepForever();
}

void sub_0803b47c(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterDoor() == 0 && sub_080403e4() == 0 && PlayerCheckDuckOrSwallow() == 0
           && sub_080400c0() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
        {
            if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 16) && gCurTask->unk28 == 1)
                break;
            if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 32) && gCurTask->unk28 == 2)
                break;
            gCurTask->unk88->unk01 = 25;
        }
        break;
    }
}

void sub_0803b4f8(void)
{
    struct Task *t;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 1;
    gCurTask->unk15 = 22;
    t = gCurTask;
    if ((u32)abs(t->unk54) > 0x9900)
    {
        if (t->unk54 < 0)
            t->unk54 = -0x9900;
        else
            t->unk54 = 0x9900;
    }
    PlayerSetMotionXPreset(9, 72);
    if (gCurTask->unk88->unk06 == 1)
    {
        while (1)
        {
            TaskSetFrame(364);
            TaskYieldTrampoline(5);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(5);
            }
            TaskSetFrame(0x161);
            TaskYieldTrampoline(5);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 9; gCurTask->unk6C++)
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(5);
            }
        }
    }
    switch (gCurTask->unk88->unk0D)
    {
    default:
        gCurTask->unk46 = 207;
        break;
    case 4:
        gCurTask->unk46 = 0x492;
        break;
    case 9:
        gCurTask->unk46 = 0x7B3;
        break;
    case 10:
        gCurTask->unk46 = 0x8B3;
        break;
    }
    while (1)
    {
        TaskSetFrame(gCurTask->unk46);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        TaskSetFrame((s16)(gCurTask->unk46 - 6));
        TaskYieldTrampoline(10);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
        }
    }
}

void sub_0803b6ec(void)
{
    PlayerTurnToHeldDirection();
    while (PlayerCheckEnterDoor() == 0 && sub_080403e4() == 0 && PlayerCheckDuckOrSwallow() == 0
           && sub_080400c0() == 0 && PlayerCheckDropAbility() == 0)
    {
        if (gCurTask->unk54 == 0 && gCurTask->unk64 == 0)
        {
            gCurTask->unk88->unk01 = 24;
            break;
        }
        if (!(gCurTask->unk7A & 1))
            gCurTask->unk88->unk01 = 23;
        break;
    }
    PlayerSetMotionXPreset(9, 72);
}

void sub_0803b768(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 12;
    gCurTask->unk15 = 25;
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk06 = 0;
    TaskSetFrame(225);
    TaskYieldTrampoline(2);
    PlaySfxIfLocalPlayer(113, gCurTask->unk88->unk00);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetFrame(227);
    TaskYieldTrampoline(2);
    TaskSetFrame(229);
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_0803b828(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0)
    {
        gLatchedHeldKeys[t->unk88->unk00] = gLatchedPressedKeys[t->unk88->unk00] = 0;
        if (t->unk7A & 1)
            t->unk88->unk01 = 24;
        else
            t->unk88->unk01 = 23;
    }
}

void sub_0803b87c(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 11;
    gCurTask->unk15 = 24;
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk06 = 0;
    TaskSetFrame(224);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    if ((s8)gCurTask->unk88->unk07 > 1)
        CreatePlayerObject(gCurTask->unk88->unk00, 2, 0);
    else
        CreatePlayerObject(gCurTask->unk88->unk00, 1, 0);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_0803b914(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0)
    {
        if (!(t->unk7A & 1) || (gLatchedHeldKeys[t->unk88->unk00] & 65))
            t->unk88->unk01 = 23;
        else
            t->unk88->unk01 = 24;
    }
    if (gCurTask->unk7A & 1)
    {
        if (gCurTask->unk54 != 0)
            PlayerSetMotionXPreset(8, 72);
        PlayerLand(1);
    }
    else
    {
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
    }
    PlayerStopAtCeilingAndWall();
}

void sub_0803b9a0(void)
{
    struct Task *t;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 11;
    gCurTask->unk15 = 23;
    t = gCurTask;
    if (t->unk88->unk05 != 11)
    {
        if (gLatchedHeldKeys[t->unk88->unk00] & 240)
        {
            if (gLatchedHeldKeys[t->unk88->unk00] & 64)
                t->unk28 = 3;
            else if (gLatchedHeldKeys[t->unk88->unk00] & 128)
                t->unk28 = 1;
            else if (gLatchedHeldKeys[t->unk88->unk00] & 16)
                t->unk28 = 0;
            else if (gLatchedHeldKeys[t->unk88->unk00] & 32)
                t->unk28 = 2;
        }
        else if (t->unk43 == 1)
        {
            t->unk28 = 0;
        }
        else
        {
            t->unk28 = 2;
        }
        gCurTask->unk73 = 0;
        {
            struct Task *u = gCurTask;

            u->unk2C = 0;
            u->unk88->unk14 = 15;
            if ((u32)abs(u->unk54) > 0x10C00)
                TaskSetMotionXFacing(0x10C00, 0x5A5A5A5A);
        }
        if (gUnk_0300244C != 0)
        {
            struct Task *v = gCurTask;

            if ((u32)abs(v->unk58) > 0xE000)
            {
                v->unk58 = 0xE000;
                PlayerSetMotionYPreset(13);
            }
        }
    }
    switch (gCurTask->unk28)
    {
    case 3:
        if (gCurTask->unk73 == 0)
        {
            CreatePlayerObject(gCurTask->unk88->unk00, 3, 0);
            while (1)
            {
                TaskSetFrame(239);
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->unk28 = -1;
        TaskSetFrame(241);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk2C++;
        break;
    case 1:
        if (gCurTask->unk73 == 0)
        {
            CreatePlayerObject(gCurTask->unk88->unk00, 3, 0);
            while (1)
            {
                TaskSetFrame(235);
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->unk28 = -1;
        TaskSetFrame(237);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk2C++;
        break;
    case 0:
        if (gCurTask->unk73 == 0)
        {
            gCurTask->unk43 = 1;
            CreatePlayerObject(gCurTask->unk88->unk00, 3, 0);
            while (1)
            {
                TaskSetFrame(231);
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->unk28 = -1;
        TaskSetFrame(233);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk2C++;
        break;
    case 2:
        if (gCurTask->unk73 == 0)
        {
            gCurTask->unk43 = -1;
            CreatePlayerObject(gCurTask->unk88->unk00, 3, 0);
            while (1)
            {
                TaskSetFrame(231);
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            }
        }
        gCurTask->unk28 = -1;
        TaskSetFrame(233);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk2C++;
        break;
    }
    TaskSleepForever();
}

void sub_0803bbf0(void)
{
    struct Task *t = gCurTask;
    u8 *st = &t->unk73;

    if (*st == 0)
    {
        if (!(gLatchedHeldKeys[t->unk88->unk00] & 2) && (s16)t->unk88->unk14 == 0)
        {
            *st = 1;
            TaskSetEntry(sub_0803b9a0, gCurTaskIdx);
        }
        else
        {
            s32 d = gCurTask->unk28;

            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 240)
            {
                if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 64)
                    d = 3;
                else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128)
                    d = 1;
                else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 16)
                    d = 0;
                else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 32)
                    d = 2;
            }
            if (d != gCurTask->unk28)
            {
                gCurTask->unk28 = d;
                TaskSetEntry(sub_0803b9a0, gCurTaskIdx);
            }
        }
        if ((s16)gCurTask->unk88->unk14 != 0)
            gCurTask->unk88->unk14--;
    }
    else if (t->unk2C != 0 && sub_080403e4() == 0)
    {
        if (!(gCurTask->unk7A & 1) || (gLatchedHeldKeys[gCurTask->unk88->unk00] & 65))
            gCurTask->unk88->unk01 = 23;
        else
            gCurTask->unk88->unk01 = 24;
    }
    if (!(gCurTask->unk7B & 1))
    {
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 64)
            gCurTask->unk88->unk01 = 9;
        else
            gCurTask->unk88->unk01 = 5;
    }
    if (gCurTask->unk7A & 1)
    {
        PlayerLand(1);
        PlayerSetMotionXPreset(8, 72);
    }
    else
    {
        PlayerSetMotionXPreset(11, 4);
        PlayerSetMotionYPreset(13);
    }
    PlayerStopAtCeilingAndWall();
}

void sub_0803bd90(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 5;
    gCurTask->unk15 = 17;
    PlayerSetMotionXPreset(11, 16);
    PlayerSetMotionYPreset(22);
    gCurTask->unk7A = 0;
    TaskSleepForever();
}

void sub_0803bdd4(void)
{
    gCurTask->unk88->unk01 = 7;
}
