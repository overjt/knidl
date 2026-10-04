#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_37ed8.c (0x08037ED8-0x0803919B, issue #91).
 *
 * Player action body, part 6: action 16 and per-frame handler 16.
 * PlayerActionHurt (action 16, mode 17, 4368 bytes) is the twin of M11's
 * MetaKnightActionHurt: a goto loop around a seven-state switch over Task.variant.
 * State 5, the entry, picks the next state from Task.hitEffect (its low
 * nibble, or 4 when bit 7 is set) and, for a player holding an ability
 * (PlayerState.ability) with bit 1 of PlayerState.unk42 clear, releases it
 * through M17's CreateAbilityStar(PlayerState.ownStarSwallowCount) and M02's HUD
 * (SetPlayerAbility); states 0-4 play the ability's animations and state 6
 * leaves.  PlayerActionHurtUpdate, handler 16, steers every state into state 6 and
 * re-binds the coroutine. */

void TaskSetEntry(void *a, u32 i);
void RequestScreenShake(u16 a);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */

void PlayerActionHurt(void)
{
    struct Task *t;
    struct PlayerState *p;
    struct Task *u;
    s32 anim;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 17;
    gCurTask->updateState = 16;
    t = gCurTask;
    p = t->player;
    if (p->prevMode != 17)
    {
        p->unk42 &= 0xFEEF;
        t->variant = 5;
        if (gCurTask->player->mouthState == 2)
            gCurTask->player->mouthState = 0;
    }
loop:
    switch (gCurTask->variant)
    {
    case 5:
        gCurTask->playerHurtPhase = 0;
        gCurTask->player->running = 0;
        if (gCurTask->player->unk37 == 2 || gCurTask->player->unk37 == 3)
        {
            gCurTask->variant = gCurTask->hitEffect & 15;
        }
        else
        {
            if (gCurTask->player->ability == 11)
            {
                gCurTask->player->unk42 &= 0xFFFD;
                SetPlayerAbility(0, -1, gCurTask->player->playerIndex);
            }
            u = gCurTask;
            if (u->hitEffect & 128)
                u->variant = 4;
            else
                u->variant = u->hitEffect & 15;
            if (!(gCurTask->player->unk42 & 2) && gCurTask->player->unk37 == 0)
            {
                if (gCurTask->player->ability != 0)
                {
                    gCurTask->player->unk42 &= 0xFFFB;
                    CreateAbilityStar(gCurTask->player->ownStarSwallowCount);
                    SetPlayerAbility(0, -1, gCurTask->player->playerIndex);
                }
            }
            else
            {
                HudShowAbility(gCurTask->player->ability, gCurTask->player->playerIndex);
            }
            if (gLocalPlayer == gCurTask->player->playerIndex)
                RequestScreenShake(2);
        }
        gCurTask->player->invulnerability = 1;
        gCurTask->player->invulnerabilityTimer = 0x8000;
        goto loop;
    case 0:
        PlayerStopAxes(2);
        if (gCurTask->player->mouthState == 1)
        {
            PlaySfxIfLocalPlayer(158, gCurTask->player->playerIndex);
            if (!(gCurTask->waterFlags & 1))
                anim = 0x171;
            else
                anim = 0x19B;
            if ((s8)gCurTask->hitDirection == 0)
                PlayerSetMotionXPreset(10, 24);
            else
                PlayerSetMotionXPreset(10, 26);
            TaskSetFrame(anim + 1);
            TaskYieldTrampoline(3);
            if ((s8)gCurTask->hitDirection == 0)
                PlayerSetMotionXPreset(10, 25);
            else
                PlayerSetMotionXPreset(10, 27);
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
            if (gFrameCount & 2)
                PlaySfxIfLocalPlayer(111, gCurTask->player->playerIndex);
            else
                PlaySfxIfLocalPlayer(112, gCurTask->player->playerIndex);
            switch (gCurTask->player->ability)
            {
            case 0:
            default:
                if (!(gCurTask->waterFlags & 1))
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
            if (((s8)gCurTask->hitDirection == 0 && gCurTask->facing == -1)
                || ((s8)gCurTask->hitDirection == 4 && gCurTask->facing == 1))
            {
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 18);
                else
                    PlayerSetMotionXPreset(10, 21);
                TaskSetFrame(anim);
                TaskYieldTrampoline(3);
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 19);
                else
                    PlayerSetMotionXPreset(10, 22);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 20);
                else
                    PlayerSetMotionXPreset(10, 23);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(3);
                }
            }
            else
            {
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 18);
                else
                    PlayerSetMotionXPreset(10, 21);
                if (gCurTask->facing == 1)
                {
                    TaskSetFrameFlip(anim);
                    TaskYieldTrampoline(3);
                }
                else
                {
                    TaskSetFrameNoFlip(anim);
                    TaskYieldTrampoline(3);
                }
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 19);
                else
                    PlayerSetMotionXPreset(10, 22);
                TaskSetFrame(anim + 8);
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 20);
                else
                    PlayerSetMotionXPreset(10, 23);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame--;
                    TaskYieldTrampoline(2);
                }
            }
        }
        PlayerStopAxes(1);
        gCurTask->variant = 6;
        goto loop;
    case 1:
    case 2:
        if (gCurTask->playerHurtPhase == 0)
        {
            if (gCurTask->variant == 1)
            {
                if (gCurTask->player->mouthState == 0)
                    PlaySfxIfLocalPlayer(0x125, gCurTask->player->playerIndex);
                else
                    PlaySfxIfLocalPlayer(0x126, gCurTask->player->playerIndex);
            }
            else if (gCurTask->player->mouthState == 0)
            {
                if (gFrameCount & 2)
                    PlaySfxIfLocalPlayer(111, gCurTask->player->playerIndex);
                else
                    PlaySfxIfLocalPlayer(112, gCurTask->player->playerIndex);
            }
            else
            {
                PlaySfxIfLocalPlayer(158, gCurTask->player->playerIndex);
            }
            if ((s8)gCurTask->hitDirection == 0)
                PlayerSetMotionXPreset(10, 28);
            else
                PlayerSetMotionXPreset(10, 30);
            PlayerSetMotionYPreset(24);
        }
        gCurTask->player->unk14 = 120;
        if (gCurTask->player->mouthState == 0)
        {
            switch (gCurTask->playerHurtPhase)
            {
            case 0:
                gCurTask->playerHurtPhase = 1;
                if (gCurTask->variant == 1)
                {
                    CreatePlayerEffect(gCurTask->player->playerIndex, 22, 0);
                    while (1)
                    {
                        TaskSetFrame(252);
                        TaskYieldTrampoline(2);
                        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
                        {
                            gCurTask->frame++;
                            TaskYieldTrampoline(2);
                        }
                    }
                }
                CreatePlayerEffect(gCurTask->player->playerIndex, 23, 0);
                while (1)
                {
                    TaskSetFrame(260);
                    TaskYieldTrampoline(4);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(4);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
            case 1:
                if (gCurTask->variant == 1)
                    CreatePlayerEffect(gCurTask->player->playerIndex, 22, 1);
                else
                    CreatePlayerEffect(gCurTask->player->playerIndex, 23, 1);
                CreatePlayerEffect(gCurTask->player->playerIndex, 25, 0);
                gCurTask->playerHurtPhase++;
                TaskSetFrame(264);
                TaskYieldTrampoline(3);
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 29);
                else
                    PlayerSetMotionXPreset(10, 31);
                PlayerSetMotionYPreset(25);
                while (1)
                {
                    TaskSetFrame(0x109);
                    TaskYieldTrampoline(3);
                    for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
                    {
                        gCurTask->frame++;
                        TaskYieldTrampoline(3);
                    }
                }
            case 2:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(43);
                TaskYieldTrampoline(2);
                PlayerSetMotionYPreset(26);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->player->unk14 = 1;
                break;
            default:
                gCurTask->player->unk14 = 1;
                break;
            }
        }
        else
        {
            switch (gCurTask->playerHurtPhase)
            {
            case 0:
                gCurTask->playerHurtPhase++;
                if (gCurTask->variant == 1)
                {
                    CreatePlayerEffect(gCurTask->player->playerIndex, 22, 0);
                    while (1)
                    {
                        TaskSetFrame(372);
                        TaskYieldTrampoline(2);
                        for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
                        {
                            gCurTask->frame++;
                            TaskYieldTrampoline(2);
                        }
                    }
                }
                CreatePlayerEffect(gCurTask->player->playerIndex, 23, 0);
                while (1)
                {
                    TaskSetFrame(380);
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
            case 1:
                if (gCurTask->variant == 1)
                    CreatePlayerEffect(gCurTask->player->playerIndex, 22, 1);
                else
                    CreatePlayerEffect(gCurTask->player->playerIndex, 23, 1);
                CreatePlayerEffect(gCurTask->player->playerIndex, 25, 0);
                gCurTask->playerHurtPhase++;
                TaskSetFrame(0x183);
                TaskYieldTrampoline(3);
                if ((s8)gCurTask->hitDirection == 0)
                    PlayerSetMotionXPreset(10, 29);
                else
                    PlayerSetMotionXPreset(10, 31);
                PlayerSetMotionYPreset(25);
                while (1)
                {
                    TaskSetFrame(0x181);
                    TaskYieldTrampoline(4);
                    gCurTask->frame--;
                    TaskYieldTrampoline(2);
                    TaskSetFrame(386);
                    TaskYieldTrampoline(4);
                    TaskSetFrame(384);
                    TaskYieldTrampoline(2);
                }
            case 2:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(0x183);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                PlayerSetMotionYPreset(26);
                while (1)
                {
                    TaskSetFrame(0x183);
                    TaskYieldTrampoline(2);
                    TaskSetFrame(0x181);
                    TaskYieldTrampoline(4);
                    gCurTask->frame--;
                    TaskYieldTrampoline(2);
                    TaskSetFrame(386);
                    TaskYieldTrampoline(4);
                    TaskSetFrame(384);
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                }
            case 3:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(0x183);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                TaskSetFrame(384);
                TaskYieldTrampoline(2);
                gCurTask->player->unk14 = 1;
                break;
            case 4:
            default:
                gCurTask->player->unk14 = 1;
                break;
            }
        }
        break;
    case 3:
        if (gCurTask->playerHurtPhase == 0)
        {
            if (gCurTask->player->mouthState == 0)
                PlaySfxIfLocalPlayer(121, gCurTask->player->playerIndex);
            else
                PlaySfxIfLocalPlayer(0x127, gCurTask->player->playerIndex);
            if ((s8)gCurTask->hitDirection == 0)
                PlayerSetMotionXPreset(10, 28);
            else
                PlayerSetMotionXPreset(10, 30);
            PlayerSetMotionYPreset(24);
        }
        gCurTask->player->unk14 = 120;
        if (gCurTask->player->mouthState == 0)
        {
            switch (gCurTask->playerHurtPhase)
            {
            case 0:
                gCurTask->playerHurtPhase = 1;
                TaskSetFrame(0x111);
                TaskYieldTrampoline(4);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(4);
                }
                gCurTask->frame++;
                CreatePlayerEffect(gCurTask->player->playerIndex, 24, 0);
                break;
            case 1:
                gCurTask->playerHurtPhase = 2;
                TaskSetFrame(280);
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                TaskSetFrame(0x117);
                TaskYieldTrampoline(48);
                CreatePlayerEffect(gCurTask->player->playerIndex, 26, 0);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                PlayerSetMotionYPreset(26);
                gCurTask->player->unk14 = 120;
            case 2:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(250);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                TaskSetFrame(244);
                TaskYieldTrampoline(3);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->player->unk14 = 1;
                break;
            default:
                gCurTask->player->unk14 = 1;
                break;
            }
        }
        else
        {
            switch (gCurTask->playerHurtPhase)
            {
            case 0:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(0x185);
                TaskYieldTrampoline(4);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(4);
                }
                gCurTask->frame++;
                break;
            case 1:
                gCurTask->playerHurtPhase++;
                PlayerSetMotionYPreset(27);
                gCurTask->player->unk14 = 120;
            case 2:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(0x191);
                TaskYieldTrampoline(2);
                TaskSetFrame(0x18B);
                TaskYieldTrampoline(16);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                gCurTask->frame++;
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
                gCurTask->player->unk14 = 120;
            case 3:
                gCurTask->playerHurtPhase++;
                TaskSetFrame(398);
                TaskYieldTrampoline(2);
                CreatePlayerEffect(gCurTask->player->playerIndex, 26, 0);
                TaskSetFrame(0x193);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
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
                TaskYieldTrampoline(2);
                if (gCurTask->onGround & 1)
                {
                    PlayerSetMotionYPreset(29);
                    gCurTask->player->unk14 = 120;
                    TaskSetFrame(410);
                    while (!(gCurTask->onGround & 1))
                        TaskYieldTrampoline(1);
                }
            case 4:
                gCurTask->player->unk14 = 1;
                break;
            }
        }
        break;
    case 4:
        if (gCurTask->player->mouthState == 0)
        {
            if (gFrameCount & 2)
                PlaySfxIfLocalPlayer(111, gCurTask->player->playerIndex);
            else
                PlaySfxIfLocalPlayer(112, gCurTask->player->playerIndex);
        }
        else
        {
            PlaySfxIfLocalPlayer(158, gCurTask->player->playerIndex);
        }
        PlayerStopAxes(3);
        gCurTask->onGround = 0;
        if (gCurTask->player->mouthState == 0)
        {
            PlayerSetMotionYPreset(30);
            if (!(gCurTask->waterFlags & 1))
            {
                TaskSetFrame(244);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(0x11D);
                TaskYieldTrampoline(2);
            }
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 6; gCurTask->playerLoopCount++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            }
        }
        else
        {
            PlayerSetMotionYPreset(31);
            if (!(gCurTask->waterFlags & 1))
            {
                TaskSetFrame(370);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(412);
                TaskYieldTrampoline(2);
            }
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(4);
        }
        gCurTask->variant = 6;
        goto loop;
    case 6:
        if (gCurTask->waterFlags & 1)
            PlayerSetWaterMotionY();
        SetPlayerInvulnerability(1, 96, gCurTask->player->playerIndex);
        gCurTask->player->unk42 |= 0x200;
        break;
    }
    TaskSleepForever();
}

void PlayerActionHurtUpdate(void)
{
    struct Task *t;

    switch (gCurTask->variant)
    {
    case 0:
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            gCurTask->variant = 6;
            TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        }
        break;
    case 1:
    case 2:
        if (gCurTask->velY > 0)
            PlayerSetMotionYPreset(2);
        if (PlayerHasCrossedWaterSurface(0) == 0)
        {
            if (PlayerCheckLanding() != 0)
                goto e1;
            t = gCurTask;
            if (--t->player->unk14 != 0)
                break;
            t->variant = 6;
            TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
            break;
        }
        gCurTask->variant = 6;
        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        break;
    case 3:
        if (gCurTask->velY > 0)
            PlayerSetMotionYPreset(2);
        if (PlayerHasCrossedWaterSurface(0) == 0)
        {
            t = gCurTask;
            if (t->velY != 0 && (t->onGround & 1))
            {
                if (t->player->mouthState != 1)
                {
                    if (t->playerHurtPhase <= 2)
                    {
                        PlayerStopAxes(3);
                        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
                        break;
                    }
                }
                else if (t->playerHurtPhase <= 1)
                    goto e3;
            }
            t = gCurTask;
            if (--t->player->unk14 != 0)
                break;
            if (t->player->mouthState != 1)
            {
                if (t->playerHurtPhase <= 2)
                    goto s2;
                t->variant = 6;
                TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
                break;
            }
            if (t->playerHurtPhase <= 3)
                goto s3;
            t->variant = 6;
            TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
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
        gCurTask->variant = 6;
        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        break;
    case 4:
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            gCurTask->variant = 6;
            TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        }
        PlayerSetMotionXPreset(7, 72);
        break;
    e1:
        PlayerStopAxes(1);
        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        break;
    e3:
        PlayerStopAxes(3);
        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        break;
    s2:
        t->playerHurtPhase = 2;
        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        break;
    s3:
        t->playerHurtPhase = 3;
        TaskSetEntry(PlayerActionHurt, gCurTaskIdx);
        break;
    case 6:
        PlayerRequestStandOrFall();
        break;
    }
    PlayerStopAtCeilingAndWall();
}
