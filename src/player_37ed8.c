#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_37ed8.c (0x08037ED8-0x0803919B, issue #91).
 *
 * Player action body, part 6: action 16 and per-frame handler 16.
 * sub_08037ed8 (action 16, mode 17, 4368 bytes) is the twin of M11's
 * sub_08042580: a goto loop around a seven-state switch over Task.unk73.
 * State 5, the entry, picks the next state from Task.unk82 (its low
 * nibble, or 4 when bit 7 is set) and, for a player holding an ability
 * (PlayerState.unk0D) with bit 1 of PlayerState.unk42 clear, releases it
 * through M17's sub_08064eb8(PlayerState.unk30) and M02's HUD
 * (SetPlayerAbility); states 0-4 play the ability's animations and state 6
 * leaves.  sub_08038fe8, handler 16, steers every state into state 6 and
 * re-binds the coroutine. */

extern u16 gLocalPlayer;
extern u16 gFrameCount;

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
void TaskSetFrameNoFlip(s32 a);
void TaskSetFrameFlip(s32 a);
s32 SetPlayerAbility(s32 a, s32 b, u32 c);
void HudShowAbility(s32 a, s32 id);
void RequestScreenShake(u16 a);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerSetWaterMotionY(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
void sub_08040710(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 sub_08064eb8(u8 p2);

void sub_08037ed8(void)
{
    struct Task *t;
    struct PlayerState *p;
    struct Task *u;
    s32 anim;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 17;
    gCurTask->unk15 = 16;
    t = gCurTask;
    p = t->unk88;
    if (p->unk05 != 17)
    {
        p->unk42 &= 0xFEEF;
        t->unk73 = 5;
        if (gCurTask->unk88->unk06 == 2)
            gCurTask->unk88->unk06 = 0;
    }
loop:
    switch (gCurTask->unk73)
    {
    case 5:
        gCurTask->unk28 = 0;
        gCurTask->unk88->unk3D = 0;
        if (gCurTask->unk88->unk37 == 2 || gCurTask->unk88->unk37 == 3)
        {
            gCurTask->unk73 = gCurTask->unk82 & 15;
        }
        else
        {
            if (gCurTask->unk88->unk0D == 11)
            {
                gCurTask->unk88->unk42 &= 0xFFFD;
                SetPlayerAbility(0, -1, gCurTask->unk88->unk00);
            }
            u = gCurTask;
            if (u->unk82 & 128)
                u->unk73 = 4;
            else
                u->unk73 = u->unk82 & 15;
            if (!(gCurTask->unk88->unk42 & 2) && gCurTask->unk88->unk37 == 0)
            {
                if (gCurTask->unk88->unk0D != 0)
                {
                    gCurTask->unk88->unk42 &= 0xFFFB;
                    sub_08064eb8(gCurTask->unk88->unk30);
                    SetPlayerAbility(0, -1, gCurTask->unk88->unk00);
                }
            }
            else
            {
                HudShowAbility(gCurTask->unk88->unk0D, gCurTask->unk88->unk00);
            }
            if (gLocalPlayer == gCurTask->unk88->unk00)
                RequestScreenShake(2);
        }
        gCurTask->unk88->unk3F = 1;
        gCurTask->unk88->unk12 = 0x8000;
        goto loop;
    case 0:
        PlayerStopAxes(2);
        if (gCurTask->unk88->unk06 == 1)
        {
            PlaySfxIfLocalPlayer(158, gCurTask->unk88->unk00);
            if (!(gCurTask->unk7B & 1))
                anim = 0x171;
            else
                anim = 0x19B;
            if ((s8)gCurTask->unk7D == 0)
                PlayerSetMotionXPreset(10, 24);
            else
                PlayerSetMotionXPreset(10, 26);
            TaskSetFrame(anim + 1);
            TaskYieldTrampoline(3);
            if ((s8)gCurTask->unk7D == 0)
                PlayerSetMotionXPreset(10, 25);
            else
                PlayerSetMotionXPreset(10, 27);
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
            if (gFrameCount & 2)
                PlaySfxIfLocalPlayer(111, gCurTask->unk88->unk00);
            else
                PlaySfxIfLocalPlayer(112, gCurTask->unk88->unk00);
            switch (gCurTask->unk88->unk0D)
            {
            case 0:
            default:
                if (!(gCurTask->unk7B & 1))
                    anim = 243;
                else
                    anim = 0x11D;
                break;
            case 4:
                anim = 0x4A1;
                break;
            case 25:
                anim = 0x1036;
                break;
            }
            if (((s8)gCurTask->unk7D == 0 && gCurTask->unk43 == -1)
                || ((s8)gCurTask->unk7D == 4 && gCurTask->unk43 == 1))
            {
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 18);
                else
                    PlayerSetMotionXPreset(10, 21);
                TaskSetFrame(anim);
                TaskYieldTrampoline(3);
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 19);
                else
                    PlayerSetMotionXPreset(10, 22);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 20);
                else
                    PlayerSetMotionXPreset(10, 23);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(3);
                }
            }
            else
            {
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 18);
                else
                    PlayerSetMotionXPreset(10, 21);
                if (gCurTask->unk43 == 1)
                {
                    TaskSetFrameFlip(anim);
                    TaskYieldTrampoline(3);
                }
                else
                {
                    TaskSetFrameNoFlip(anim);
                    TaskYieldTrampoline(3);
                }
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 19);
                else
                    PlayerSetMotionXPreset(10, 22);
                TaskSetFrame(anim + 8);
                TaskYieldTrampoline(2);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 20);
                else
                    PlayerSetMotionXPreset(10, 23);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C--;
                    TaskYieldTrampoline(2);
                }
            }
        }
        PlayerStopAxes(1);
        gCurTask->unk73 = 6;
        goto loop;
    case 1:
    case 2:
        if (gCurTask->unk28 == 0)
        {
            if (gCurTask->unk73 == 1)
            {
                if (gCurTask->unk88->unk06 == 0)
                    PlaySfxIfLocalPlayer(0x125, gCurTask->unk88->unk00);
                else
                    PlaySfxIfLocalPlayer(0x126, gCurTask->unk88->unk00);
            }
            else if (gCurTask->unk88->unk06 == 0)
            {
                if (gFrameCount & 2)
                    PlaySfxIfLocalPlayer(111, gCurTask->unk88->unk00);
                else
                    PlaySfxIfLocalPlayer(112, gCurTask->unk88->unk00);
            }
            else
            {
                PlaySfxIfLocalPlayer(158, gCurTask->unk88->unk00);
            }
            if ((s8)gCurTask->unk7D == 0)
                PlayerSetMotionXPreset(10, 28);
            else
                PlayerSetMotionXPreset(10, 30);
            PlayerSetMotionYPreset(24);
        }
        gCurTask->unk88->unk14 = 120;
        if (gCurTask->unk88->unk06 == 0)
        {
            switch (gCurTask->unk28)
            {
            case 0:
                gCurTask->unk28 = 1;
                if (gCurTask->unk73 == 1)
                {
                    CreatePlayerEffect(gCurTask->unk88->unk00, 22, 0);
                    while (1)
                    {
                        TaskSetFrame(252);
                        TaskYieldTrampoline(2);
                        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
                        {
                            gCurTask->unk3C++;
                            TaskYieldTrampoline(2);
                        }
                    }
                }
                CreatePlayerEffect(gCurTask->unk88->unk00, 23, 0);
                while (1)
                {
                    TaskSetFrame(260);
                    TaskYieldTrampoline(4);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(4);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
            case 1:
                if (gCurTask->unk73 == 1)
                    CreatePlayerEffect(gCurTask->unk88->unk00, 22, 1);
                else
                    CreatePlayerEffect(gCurTask->unk88->unk00, 23, 1);
                CreatePlayerEffect(gCurTask->unk88->unk00, 25, 0);
                gCurTask->unk28++;
                TaskSetFrame(264);
                TaskYieldTrampoline(3);
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 29);
                else
                    PlayerSetMotionXPreset(10, 31);
                PlayerSetMotionYPreset(25);
                while (1)
                {
                    TaskSetFrame(0x109);
                    TaskYieldTrampoline(3);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C++;
                        TaskYieldTrampoline(3);
                    }
                }
            case 2:
                gCurTask->unk28++;
                TaskSetFrame(43);
                TaskYieldTrampoline(2);
                PlayerSetMotionYPreset(26);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->unk88->unk14 = 1;
                break;
            default:
                gCurTask->unk88->unk14 = 1;
                break;
            }
        }
        else
        {
            switch (gCurTask->unk28)
            {
            case 0:
                gCurTask->unk28++;
                if (gCurTask->unk73 == 1)
                {
                    CreatePlayerEffect(gCurTask->unk88->unk00, 22, 0);
                    while (1)
                    {
                        TaskSetFrame(372);
                        TaskYieldTrampoline(2);
                        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
                        {
                            gCurTask->unk3C++;
                            TaskYieldTrampoline(2);
                        }
                    }
                }
                CreatePlayerEffect(gCurTask->unk88->unk00, 23, 0);
                while (1)
                {
                    TaskSetFrame(380);
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
            case 1:
                if (gCurTask->unk73 == 1)
                    CreatePlayerEffect(gCurTask->unk88->unk00, 22, 1);
                else
                    CreatePlayerEffect(gCurTask->unk88->unk00, 23, 1);
                CreatePlayerEffect(gCurTask->unk88->unk00, 25, 0);
                gCurTask->unk28++;
                TaskSetFrame(0x183);
                TaskYieldTrampoline(3);
                if ((s8)gCurTask->unk7D == 0)
                    PlayerSetMotionXPreset(10, 29);
                else
                    PlayerSetMotionXPreset(10, 31);
                PlayerSetMotionYPreset(25);
                while (1)
                {
                    TaskSetFrame(0x181);
                    TaskYieldTrampoline(4);
                    gCurTask->unk3C--;
                    TaskYieldTrampoline(2);
                    TaskSetFrame(386);
                    TaskYieldTrampoline(4);
                    TaskSetFrame(384);
                    TaskYieldTrampoline(2);
                }
            case 2:
                gCurTask->unk28++;
                TaskSetFrame(0x183);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                PlayerSetMotionYPreset(26);
                while (1)
                {
                    TaskSetFrame(0x183);
                    TaskYieldTrampoline(2);
                    TaskSetFrame(0x181);
                    TaskYieldTrampoline(4);
                    gCurTask->unk3C--;
                    TaskYieldTrampoline(2);
                    TaskSetFrame(386);
                    TaskYieldTrampoline(4);
                    TaskSetFrame(384);
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(1);
                }
            case 3:
                gCurTask->unk28++;
                TaskSetFrame(0x183);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                TaskSetFrame(384);
                TaskYieldTrampoline(2);
                gCurTask->unk88->unk14 = 1;
                break;
            case 4:
            default:
                gCurTask->unk88->unk14 = 1;
                break;
            }
        }
        break;
    case 3:
        if (gCurTask->unk28 == 0)
        {
            if (gCurTask->unk88->unk06 == 0)
                PlaySfxIfLocalPlayer(121, gCurTask->unk88->unk00);
            else
                PlaySfxIfLocalPlayer(0x127, gCurTask->unk88->unk00);
            if ((s8)gCurTask->unk7D == 0)
                PlayerSetMotionXPreset(10, 28);
            else
                PlayerSetMotionXPreset(10, 30);
            PlayerSetMotionYPreset(24);
        }
        gCurTask->unk88->unk14 = 120;
        if (gCurTask->unk88->unk06 == 0)
        {
            switch (gCurTask->unk28)
            {
            case 0:
                gCurTask->unk28 = 1;
                TaskSetFrame(0x111);
                TaskYieldTrampoline(4);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(4);
                }
                gCurTask->unk3C++;
                CreatePlayerEffect(gCurTask->unk88->unk00, 24, 0);
                break;
            case 1:
                gCurTask->unk28 = 2;
                TaskSetFrame(280);
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                TaskSetFrame(0x117);
                TaskYieldTrampoline(48);
                CreatePlayerEffect(gCurTask->unk88->unk00, 26, 0);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
                PlayerSetMotionYPreset(26);
                gCurTask->unk88->unk14 = 120;
            case 2:
                gCurTask->unk28++;
                TaskSetFrame(250);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                TaskSetFrame(244);
                TaskYieldTrampoline(3);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk88->unk14 = 1;
                break;
            default:
                gCurTask->unk88->unk14 = 1;
                break;
            }
        }
        else
        {
            switch (gCurTask->unk28)
            {
            case 0:
                gCurTask->unk28++;
                TaskSetFrame(0x185);
                TaskYieldTrampoline(4);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(4);
                }
                gCurTask->unk3C++;
                break;
            case 1:
                gCurTask->unk28++;
                PlayerSetMotionYPreset(27);
                gCurTask->unk88->unk14 = 120;
            case 2:
                gCurTask->unk28++;
                TaskSetFrame(0x191);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x18B);
                TaskYieldTrampoline(16);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                TaskSetFrame(0x18B);
                TaskYieldTrampoline(20);
                TaskSetFrame(0x191);
                TaskYieldTrampoline(4);
                TaskSetFrame(0x18B);
                TaskYieldTrampoline(2);
                TaskSetFrame(402);
                TaskYieldTrampoline(4);
                TaskSetFrame(0x18B);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x191);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x18B);
                TaskYieldTrampoline(1);
                TaskSetFrame(402);
                TaskYieldTrampoline(2);
                PlayerSetMotionYPreset(28);
                gCurTask->unk88->unk14 = 120;
            case 3:
                gCurTask->unk28++;
                TaskSetFrame(398);
                TaskYieldTrampoline(2);
                CreatePlayerEffect(gCurTask->unk88->unk00, 26, 0);
                TaskSetFrame(0x193);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
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
                TaskYieldTrampoline(2);
                if (gCurTask->unk7A & 1)
                {
                    PlayerSetMotionYPreset(29);
                    gCurTask->unk88->unk14 = 120;
                    TaskSetFrame(410);
                    while (!(gCurTask->unk7A & 1))
                        TaskYieldTrampoline(1);
                }
            case 4:
                gCurTask->unk88->unk14 = 1;
                break;
            }
        }
        break;
    case 4:
        if (gCurTask->unk88->unk06 == 0)
        {
            if (gFrameCount & 2)
                PlaySfxIfLocalPlayer(111, gCurTask->unk88->unk00);
            else
                PlaySfxIfLocalPlayer(112, gCurTask->unk88->unk00);
        }
        else
        {
            PlaySfxIfLocalPlayer(158, gCurTask->unk88->unk00);
        }
        PlayerStopAxes(3);
        gCurTask->unk7A = 0;
        if (gCurTask->unk88->unk06 == 0)
        {
            PlayerSetMotionYPreset(30);
            if (!(gCurTask->unk7B & 1))
            {
                TaskSetFrame(244);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(0x11D);
                TaskYieldTrampoline(2);
            }
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            }
        }
        else
        {
            PlayerSetMotionYPreset(31);
            if (!(gCurTask->unk7B & 1))
            {
                TaskSetFrame(370);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(412);
                TaskYieldTrampoline(2);
            }
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(4);
        }
        gCurTask->unk73 = 6;
        goto loop;
    case 6:
        if (gCurTask->unk7B & 1)
            PlayerSetWaterMotionY();
        SetPlayerInvulnerability(1, 96, gCurTask->unk88->unk00);
        gCurTask->unk88->unk42 |= 0x200;
        break;
    }
    TaskSleepForever();
}

void sub_08038fe8(void)
{
    struct Task *t;

    switch (gCurTask->unk73)
    {
    case 0:
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            gCurTask->unk73 = 6;
            TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        }
        break;
    case 1:
    case 2:
        if (gCurTask->unk58 > 0)
            PlayerSetMotionYPreset(2);
        if (PlayerHasCrossedWaterSurface(0) == 0)
        {
            if (PlayerCheckLanding() != 0)
                goto e1;
            t = gCurTask;
            if (--t->unk88->unk14 != 0)
                break;
            t->unk73 = 6;
            TaskSetEntry(sub_08037ed8, gCurTaskIdx);
            break;
        }
        gCurTask->unk73 = 6;
        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        break;
    case 3:
        if (gCurTask->unk58 > 0)
            PlayerSetMotionYPreset(2);
        if (PlayerHasCrossedWaterSurface(0) == 0)
        {
            t = gCurTask;
            if (t->unk58 != 0 && (t->unk7A & 1))
            {
                if (t->unk88->unk06 != 1)
                {
                    if (t->unk28 <= 2)
                    {
                        PlayerStopAxes(3);
                        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
                        break;
                    }
                }
                else if (t->unk28 <= 1)
                    goto e3;
            }
            t = gCurTask;
            if (--t->unk88->unk14 != 0)
                break;
            if (t->unk88->unk06 != 1)
            {
                if (t->unk28 <= 2)
                    goto s2;
                t->unk73 = 6;
                TaskSetEntry(sub_08037ed8, gCurTaskIdx);
                break;
            }
            if (t->unk28 <= 3)
                goto s3;
            t->unk73 = 6;
            TaskSetEntry(sub_08037ed8, gCurTaskIdx);
            break;
        }
        /* An empty loop (a compiled-out macro?): its NOTE_INSN_LOOP_END
           stops the first cse pass from following the PlayerHasCrossedWaterSurface jump
           into this block, so the task-address reload here is not chained
           to case 3's PRE copy; without it that copy outlives gcse's
           r4 pseudo and takes r5 (push {r4, r5}). */
        do
        {
        } while (0);
        gCurTask->unk73 = 6;
        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        break;
    case 4:
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            gCurTask->unk73 = 6;
            TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        }
        PlayerSetMotionXPreset(7, 72);
        break;
    e1:
        PlayerStopAxes(1);
        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        break;
    e3:
        PlayerStopAxes(3);
        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        break;
    s2:
        t->unk28 = 2;
        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        break;
    s3:
        t->unk28 = 3;
        TaskSetEntry(sub_08037ed8, gCurTaskIdx);
        break;
    case 6:
        sub_08040710();
        break;
    }
    PlayerStopAtCeilingAndWall();
}
