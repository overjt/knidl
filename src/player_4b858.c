#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4b858.c (0x0804B858-0x0804C64B, issue #88).
 *
 * Player action bodies, part 21: action 53 and per-frame handler 50.
 * sub_0804b858 (action 53, mode 13) is `loop: switch (Task.unk73)` over
 * eight states, a stance with six moves.  A fresh entry starts in
 * state 6 with the 120-frame timer PlayerState.unk14 and Task.unk80 =
 * 22: the stance (animation 0xE84, effect 28 on the ground) picks the
 * next move from the input - up 0, back 1 or forward 2 (PlayerGetHeldDirection,
 * kept in gUnk_03001F2C), down 3, A 4, leaving the ground 5 - or a
 * random one (gUnk_0873B65E[RandomRange(4)]) when the timer runs out,
 * and goes to state 7 once PlayerState.unk08 is 0.  States 0-5 are the
 * moves (animations 0xE97-0xEE4 with the frame index PlayerState.unk16,
 * velocity presets 2 and 56-68, sounds 178/179, a landing with effect
 * 27 and RequestScreenShake(2)), each back to state 7, which clears
 * PlayerState.unk42 bit 9, calls SetPlayerInvulnerability(255, 0, player) and ends
 * the action (TaskSleepForever, falling into state 0, lesson 3.403).  The
 * long `bl`s at 0x0804C49E and 0x0804C488 are cross-jumped `goto loop`
 * tails.  Its handler sub_0804c4ac lets PlayerRequestLocomotion end state 7,
 * steers state 4 in the air (TaskSetMotionXFacing and the 8.8 speed Task.unk64
 * for the direction in gUnk_03001F2C), re-binds state 7 from the other
 * moves once PlayerState.unk16 >= 0 and unk08 == 0, and requests action
 * 23 through PlayerHasCrossedWaterSurface. */

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern s32 gUnk_03001F2C;               /* boot_091ac.c spelling */
extern u8 gUnk_0873B65E[];

void TaskYieldTrampoline(s32 frames);
u32 RandomRange(u32 range);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskSetFrame(s32 a);
void RequestScreenShake(u16 a);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerTurnToHeldDirection(void);
s32 PlayerGetHeldDirection(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_0804b858(void)
{
    struct Task *h;
    s32 k;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 50;
    h = gCurTask;
    if (h->unk88->unk05 != 13)
    {
        struct Task *u;

        h->unk73 = 6;
        u = gCurTask;
        u->unk88->unk14 = 120;
        u->unk28 = 0;
        u->unk80 = 22;
    }
loop:
    switch (gCurTask->unk73)
    {
    case 6:
        {
            struct Task *a;

            SetPlayerInvulnerability(3, 0, gCurTask->unk88->unk00);
            gCurTask->unk88->unk16 = 0;
            TaskSetFrame(0xE84);
            PlayerStartOffsetScript(12);
            a = gCurTask;
            if (a->unk7A & 1)
            {
                CreatePlayerEffect(a->unk88->unk00, 28, 3);
                TaskYieldTrampoline(10);
            }
            else
            {
                PlayerStopAxes(3);
                TaskYieldTrampoline(10);
            }
        }
        for (;;)
        {
            struct Task *b;
            struct Task *c;
            struct PlayerState *p;

            b = gCurTask;
            if (--b->unk88->unk14 == 0)
            {
                gCurTask->unk73 = gUnk_0873B65E[RandomRange(4)];
                goto loop;
            }
            if (!(b->unk7A & 1))
            {
                b->unk73 = 5;
                goto loop;
            }
            if (gLatchedHeldKeys[b->unk88->unk00] & 1)
            {
                b->unk73 = 4;
                goto loop;
            }
            k = PlayerGetHeldDirection();
            gUnk_03001F2C = k;
            if (k != 0)
            {
                if (k == 1)
                {
                    gCurTask->unk73 = 2;
                    goto loop;
                }
                gCurTask->unk73 = 1;
                goto loop;
            }
            c = gCurTask;
            p = c->unk88;
            if (gLatchedHeldKeys[p->unk00] & 0xC0)
            {
                if (gLatchedHeldKeys[p->unk00] & 0x40)
                {
                    c->unk73 = 0;
                    goto loop;
                }
                c->unk73 = 3;
                goto loop;
            }
            if ((s8)p->unk08 == 0)
                c->unk73 = 7;
            TaskYieldTrampoline(1);
        }
    case 7:
        {
            struct Task *d = gCurTask;

            d->unk88->unk42 &= 0xFDFF;
            SetPlayerInvulnerability(255, 0, d->unk88->unk00);
        }
        TaskSleepForever();
    case 0:
        gCurTask->unk88->unk16 = 1;
        TaskSetFrame(0xE97);
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 2;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 3;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 4;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 5;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 6;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerSetMotionYPreset(58);
        PlaySfxIfLocalPlayer(178, gCurTask->unk88->unk00);
        gCurTask->unk88->unk16 = 7;
        gCurTask->unk3C++;
        TaskYieldTrampoline(17);
        PlayerSetMotionYPreset(59);
        gCurTask->unk88->unk16 = 24;
        TaskSetFrame(0xEBA);
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->unk88->unk00);
        PlayerStopAxes(2);
        gCurTask->unk88->unk16 = 25;
        TaskSetFrame(0xEBB);
        TaskYieldTrampoline(20);
        gCurTask->unk88->unk16 = 255;
        gCurTask->unk73 = 7;
        goto loop;
    case 1:
        gCurTask->unk88->unk16 = 1;
        TaskSetFrame(0xE97);
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 2;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 3;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 4;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 5;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 6;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerSetMotionXPreset(11, 66);
        PlayerSetMotionYPreset(60);
        PlaySfxIfLocalPlayer(178, gCurTask->unk88->unk00);
        gCurTask->unk88->unk16 = 7;
        gCurTask->unk3C++;
        TaskYieldTrampoline(13);
        gCurTask->unk88->unk16 = 8;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 9;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(61);
        gCurTask->unk88->unk16 = 10;
        gCurTask->unk3C++;
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->unk88->unk00);
        PlayerStopAxes(3);
        gCurTask->unk88->unk16 = 11;
        TaskSetFrame(0xEA1);
        TaskYieldTrampoline(20);
        gCurTask->unk88->unk16 = 255;
        PlayerSetMotionYPreset(56);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 7;
        goto loop;
    case 3:
        gCurTask->unk88->unk16 = 1;
        TaskSetFrame(0xE97);
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 2;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 3;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 4;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 5;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 6;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerSetMotionYPreset(62);
        PlaySfxIfLocalPlayer(178, gCurTask->unk88->unk00);
        gCurTask->unk88->unk16 = 7;
        gCurTask->unk3C++;
        TaskYieldTrampoline(11);
        gCurTask->unk88->unk16 = 12;
        TaskSetFrame(0xEA8);
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 13;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 14;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(63);
        do
        {
            gCurTask->unk88->unk16 = 15;
            TaskSetFrame(0xEAB);
            TaskYieldTrampoline(1);
            if (gCurTask->unk7A & 1)
                goto done3;
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
            {
                gCurTask->unk88->unk16++;
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                if (gCurTask->unk7A & 1)
                    goto done3;
            }
        } while (!(gCurTask->unk7A & 1));
    done3:
        CreatePlayerEffect(gCurTask->unk88->unk00, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->unk88->unk00);
        PlayerStopAxes(2);
        gCurTask->unk88->unk16 = 23;
        TaskSetFrame(0xEB3);
        TaskYieldTrampoline(20);
        gCurTask->unk88->unk16 = 255;
        PlayerSetMotionYPreset(56);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 7;
        goto loop;
    case 4:
        gCurTask->unk28 = 0;
        PlayerSetMotionXPreset(11, 67);
        gCurTask->unk88->unk16 = 37;
        TaskSetFrame(0xECE);
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 38;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 39;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 40;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 41;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerStopAxes(1);
        gCurTask->unk88->unk16 = 42;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerSetMotionYPreset(64);
        PlaySfxIfLocalPlayer(178, gCurTask->unk88->unk00);
        gCurTask->unk88->unk16 = 43;
        gCurTask->unk3C++;
        TaskYieldTrampoline(11);
        gCurTask->unk88->unk16 = 44;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 45;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 46;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(65);
        gCurTask->unk88->unk16 = 47;
        TaskSetFrame(0xED8);
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->unk88->unk00);
        PlayerStopAxes(3);
        gCurTask->unk28++;
        gCurTask->unk88->unk16 = 48;
        TaskSetFrame(0xED9);
        TaskYieldTrampoline(20);
        gCurTask->unk88->unk16 = 255;
        PlayerSetMotionYPreset(57);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 7);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 7;
        goto loop;
    case 2:
        PlayerSetMotionXPreset(11, 68);
        gCurTask->unk88->unk16 = 26;
        TaskSetFrame(0xEBD);
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 27;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerStopAxes(1);
        gCurTask->unk88->unk16 = 28;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        PlayerSetMotionXPreset(11, 69);
        PlayerSetMotionYPreset(66);
        PlaySfxIfLocalPlayer(178, gCurTask->unk88->unk00);
        gCurTask->unk88->unk16 = 29;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 30;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 31;
        gCurTask->unk3C++;
        TaskYieldTrampoline(9);
        gCurTask->unk88->unk16 = 32;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 33;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(67);
        gCurTask->unk88->unk16 = 34;
        TaskSetFrame(0xEC5);
        while (!(gCurTask->unk7A & 1))
            TaskYieldTrampoline(1);
        CreatePlayerEffect(gCurTask->unk88->unk00, 27, 0);
        RequestScreenShake(2);
        PlaySfxIfLocalPlayer(179, gCurTask->unk88->unk00);
        PlayerStopAxes(3);
        gCurTask->unk88->unk16 = 35;
        TaskSetFrame(0xEC6);
        TaskYieldTrampoline(20);
        gCurTask->unk88->unk16 = 36;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16 = 255;
        PlayerSetMotionYPreset(56);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 7;
        goto loop;
    case 5:
        PlayerStopAxes(3);
        PlayerSetMotionYPreset(68);
        gCurTask->unk88->unk16 = 49;
        TaskSetFrame(0xEE4);
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16++;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16++;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk88->unk16++;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerSetMotionYPreset(2);
        gCurTask->unk88->unk16++;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16++;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16++;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk16 = 254;
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(8);
        gCurTask->unk88->unk16 = 255;
        gCurTask->unk73 = 7;
        goto loop;
    }
    goto loop;
}

void sub_0804c4ac(void)
{
    switch (gCurTask->unk73) {
    case 7:
        if (gCurTask->unk7A & 1)
            PlayerStopAxes(2);
        PlayerRequestLocomotion();
        return;
    case 4:
        {
            struct Task *t = gCurTask;
            if (t->unk28 == 0 && !(t->unk7A & 1) && (s8)t->unk88->unk16 != -1) {
                s32 k, x, v;
                k = PlayerGetHeldDirection();
                gUnk_03001F2C = k;
                if (k != 0) {
                    if (k == 1)
                        x = 0x300;
                    else
                        x = 0x100;
                } else {
                    x = 0x200;
                }
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(v, 0);
                {
                    struct Task *u = gCurTask;
                    s32 w = x << 8;
                    if (x & 0x8000)
                        w |= 0xFF000000;
                    u->unk64 = w;
                }
            }
        }
        /* fallthrough */
    case 0:
    case 1:
    case 2:
    case 3:
    case 5:
        {
            struct Task *u = gCurTask;
            struct PlayerState *p = u->unk88;
            if ((s8)p->unk16 >= 0 && (s8)p->unk08 == 0) {
                if (!(u->unk7A & 1)) {
                    PlayerStopAxes(2);
                    PlayerSetMotionYPreset(2);
                }
                PlayerStopAxes(1);
                gCurTask->unk73 = 7;
                TaskSetEntry(sub_0804b858, gCurTaskIdx);
            }
        }
        break;
    }
    PlayerStopAtCeilingAndWall();
    if (PlayerHasCrossedWaterSurface(0) != 0) {
        gCurTask->unk88->unk01 = 23;
        gCurTask->unk88->unk16 = 255;
        SetPlayerInvulnerability(255, 0, gCurTask->unk88->unk00);
    } else if (gCurTask->unk7A & 1) {
        PlayerLand(0);
    } else if ((s8)gCurTask->unk88->unk16 == -1) {
        PlayerTurnToHeldDirection();
        PlayerSetMotionXPreset(7, 72);
    }
}
