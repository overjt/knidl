#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "collision.h"
#include "room.h"
#include "effect.h"
#include "actor.h"

/* player_36c94.c (0x08036C94-0x08037ED7, issue #91).
 *
 * Player action bodies, part 5: actions 12-15 and per-frame handlers
 * 12-15.  PlayerActionLadder (action 12, mode 9) is a four-state machine over
 * Task.variant that plays the ability's rows of gUnk_0873D880,
 * gUnk_0873D8B4 and gUnk_0873D908; its handler PlayerActionLadderUpdate re-binds it
 * when the keys change.  PlayerActionInhale (action 13, mode 10) clears the
 * ability (PlayerState.ability = 0) and the player's three spark records
 * gUnk_02007E90[player][] before its animation; its handler PlayerActionInhaleUpdate
 * runs the block-breaking hit box gUnk_0873CC54 through M09's
 * TaskBreakFirstBlock (spawning the debris with M17's CreateBlockStar at the
 * broken block) and M09's collision registry RegisterCollider.  Actions 14
 * and 15 (PlayerActionSpit, PlayerActionSwallow) are short animation scripts, and
 * handlers 14 and 15 pick the next velocity preset from the ground flags
 * Task.onGround/unk7B. */

/* gUnk_02007E90[4][3]: M04's per-player spark records (src/player_10358.c) */
struct M04Spark
{
    /*0x00*/ s32 offsetX;
    /*0x04*/ s32 offsetY;
    /*0x08*/ s32 velX;
    /*0x0C*/ u8 frameTimer;
    /*0x0D*/ u8 frame;
    /*0x0E*/ u16 unk0E;
};

/* block_30804.c's hit-box set (M09) */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 offsetX;
    /*0x03*/ s8 offsetY;
    /*0x04*/ s8 (*boxes)[4];
};

/* Not from player.h: this file's view of gBrokenBlockY differs (lesson
   3.517). */
extern s16 gUnk_0873D880[];
extern u16 gUnk_0873D8B4[];
extern u16 gUnk_0873D908[];
extern struct M04Spark gUnk_02007E90[][3];
extern struct HitBoxSet gUnk_0873CC54;
extern u16 gBrokenBlockY;
extern u8 gUnk_0873BEC4[];

void TaskSetEntry(void *a, u32 i);
/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);
u16 TaskBreakFirstBlock(struct HitBoxSet *p, s32 e);
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void PlayerStartSfx(s32 a0, u16 a1);
void PlayerStopSfx(void);
void PlayerSetWaterMotionY(void);
s32 PlayerLand(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
s32 PlayerCheckDropAbility(void);
s32 PlayerRequestLocomotion(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
void LoadAbilityTiles(void);                     /* M13, src/player_49738.c */
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */
s32 CreateBlockStar(s16 x, s16 y, u32 p2, u8 p3, u8 p4);

void PlayerActionLadder(void)
{
    struct Task *h1;
    struct Task *h3;
    struct Task *h4;
    struct Task *h5;
    struct Task *d;
    struct Task *a1;
    struct Task *b1;
    struct Task *a3;
    struct Task *b3;
    struct Task *c2;
    struct Task *a2;
    struct Task *b2;
    struct Task *a4;
    struct Task *b4;
    struct Task *e;
    u16 *q;
    u16 *r;
    s32 v;
    s32 w;
    s32 x;
    s32 xa;
    s32 xb;
    s32 k;
    s32 i;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 9;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_LADDER;
    if (gCurTask->player->prevMode != 9)
    {
        PlayerStopAxes(3);
        gCurTask->facing = 1;
        h1 = gCurTask;
        h1->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
        h1->player->running = 0;
        gCurTask->player->unk50 = 1;
        h3 = gCurTask;
        h3->playerLadderSavedFacing = h3->facing;
        h3->posX = ((h3->pixelX & 0xFFF0) | 8) << 16;
        q = gLatchedHeldKeys;
        if (q[h3->player->playerIndex] & 64)
            h3->variant = 1;
        else
            h3->variant = 2;
        switch (gCurTask->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            h4 = gCurTask;
            if (h4->variant == 1)
                h4->playerLadderStep = 0;
            else
                h4->playerLadderStep = 10;
            break;
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_TORNADO:
            h4 = gCurTask;
            if (h4->variant == 1)
                h4->playerLadderStep = 0;
            else
                h4->playerLadderStep = 16;
            break;
        }
        h5 = gCurTask;
        h5->playerLadderDir = h5->variant;
    }
    d = gCurTask;
    d->playerBaseFrame = gUnk_0873D880[d->player->ability];
    k = d->variant;
    switch (k)
    {
    case 1:
        d->playerLadderDir = k;
        switch (d->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            for (;;)
            {
                if (gCurTask->playerLadderStep == 0 || gCurTask->playerLadderStep == 6)
                    PlaySfxIfLocalPlayer(123, gCurTask->player->playerIndex);
                a1 = gCurTask;
                r = &gUnk_0873D8B4[a1->playerLadderStep * 3];
                x = -r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a1->velY = v;
                x = r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a1->speedLimitY = v;
                a1->frame = r[0] + a1->playerBaseFrame;
                TaskYieldTrampoline(r[1]);
                b1 = gCurTask;
                b1->playerLadderStep++;
                if (b1->playerLadderStep > 9)
                    b1->playerLadderStep = 0;
            }
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_TORNADO:
            for (;;)
            {
                if (gCurTask->playerLadderStep == 0 || gCurTask->playerLadderStep == 9)
                    PlaySfxIfLocalPlayer(123, gCurTask->player->playerIndex);
                a3 = gCurTask;
                r = &gUnk_0873D908[a3->playerLadderStep * 3];
                x = -r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a3->velY = v;
                x = r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a3->speedLimitY = v;
                a3->frame = r[0] + a3->playerBaseFrame;
                TaskYieldTrampoline(r[1]);
                b3 = gCurTask;
                b3->playerLadderStep++;
                if (b3->playerLadderStep > 15)
                    b3->playerLadderStep = 0;
            }
        }
    case 2:
        c2 = gCurTask;
        c2->playerLadderDir = c2->variant;
        c2->player->playerLadderSfxTimer = 0;
        PlaySfxIfLocalPlayer(124, c2->player->playerIndex);
        switch (gCurTask->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            for (;;)
            {
                a2 = gCurTask;
                r = &gUnk_0873D8B4[a2->playerLadderStep * 3];
                xa = r[2];
                v = xa << 8;
                if (xa & 0x8000)
                    v |= 0xFF000000;
                a2->velY = v;
                x = r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a2->speedLimitY = v;
                a2->frame = r[0] + a2->playerBaseFrame;
                TaskYieldTrampoline(r[1]);
                b2 = gCurTask;
                b2->playerLadderStep++;
                if (b2->playerLadderStep > 13)
                    b2->playerLadderStep = 10;
            }
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_TORNADO:
            for (;;)
            {
                a4 = gCurTask;
                i = a4->playerLadderStep;
                r = &gUnk_0873D908[i * 3];
                xb = r[2];
                w = xb << 8;
                if (xb & 0x8000)
                    w |= 0xFF000000;
                a4->velY = w;
                x = r[2];
                w = x << 8;
                if (x & 0x8000)
                    w |= 0xFF000000;
                a4->speedLimitY = w;
                a4->frame = r[0] + a4->playerBaseFrame;
                TaskYieldTrampoline((&gUnk_0873D908[1])[i * 3]);
                b4 = gCurTask;
                b4->playerLadderStep++;
                if (b4->playerLadderStep > 20)
                    b4->playerLadderStep = 16;
            }
        }
    case 0:
        PlayerStopAxes(2);
        switch (gCurTask->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            TaskSleepForever();
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_TORNADO:
            e = gCurTask;
            e->playerLadderFrameOffset = gUnk_0873D908[e->playerLadderStep * 3];
            for (;;)
            {
                gCurTask->playerLadderFrameOffset += 13;
                if (gCurTask->playerLadderFrameOffset > 51)
                    gCurTask->playerLadderFrameOffset -= 52;
                gCurTask->frame = gCurTask->playerLadderFrameOffset + gCurTask->playerBaseFrame;
                TaskYieldTrampoline(2);
            }
        }
    }
}

void PlayerActionLadderUpdate(void)
{
    struct Task *t;
    struct Task *ta;
    struct Task *tb;
    struct Task *tc;
    struct Task *td;
    struct Task *te;
    struct Task *tf;
    struct Task *tg;
    struct Task *th;
    struct Task *ti;
    struct Task *tj;
    struct PlayerState *p;
    u16 *qa;
    u16 *qb;
    u16 *qd;
    s32 k;
    s32 n;
    s32 m;

    if (PlayerCheckDropAbility() != 0)
    {
        t = gCurTask;
        t->frame = gUnk_0873D880[t->player->ability];
        TaskSetEntry(PlayerActionLadder, gCurTaskIdx);
        if (gUnk_0300244C != 0)
            gCurTask->player->requestedAction = PLAYER_ACTION_NONE;
        return;
    }
    t = gCurTask;
    k = t->variant;
    switch (k)
    {
    case 1:
        if (gTerrainResult.ceilingHits != 0 || (t->player->boundsClamp & 4) != 0)
            t->velY = 0;
        qa = gLatchedHeldKeys;
        ta = gCurTask;
        if ((qa[ta->player->playerIndex] & 192) == 0)
            ta->variant = 0;
        else if ((qa[ta->player->playerIndex] & 128) != 0)
            ta->variant = 2;
        if (gCurTask->variant == 1)
            break;
        TaskSetEntry(PlayerActionLadder, gCurTaskIdx);
        break;
    case 2:
        p = t->player;
        k &= p->playerLadderSfxTimer;
        if (k != 0)
        {
            PlaySfxIfLocalPlayer(124, p->playerIndex);
            gCurTask->player->playerLadderSfxTimer = 0;
        }
        else
        {
            p->playerLadderSfxTimer++;
        }
        qb = gLatchedHeldKeys;
        tb = gCurTask;
        if ((qb[tb->player->playerIndex] & 192) == 0)
            tb->variant = 0;
        else if ((qb[tb->player->playerIndex] & 64) != 0)
            tb->variant = 1;
        if (gCurTask->variant == 2)
            break;
        TaskSetEntry(PlayerActionLadder, gCurTaskIdx);
        break;
    case 0:
        if ((gLatchedPressedKeys[t->player->playerIndex] & 192) == 0)
            break;
        switch (t->player->ability)
        {
        case ABILITY_NORMAL:
        default:
            th = gCurTask;
            n = th->playerLadderDir;
            if (n == 1)
            {
                if ((gLatchedPressedKeys[th->player->playerIndex] & 64) != 0)
                {
                    th->variant = n;
                    tc = gCurTask;
                    tc->playerLadderStep++;
                    if (tc->playerLadderStep > 9)
                        tc->playerLadderStep = 0;
                }
                else
                {
                    th->variant = 2;
                    gCurTask->playerLadderStep = 10;
                }
            }
            else if ((gLatchedPressedKeys[th->player->playerIndex] & 64) == 0)
            {
                th->variant = 2;
                ti = gCurTask;
                ti->playerLadderStep++;
                if (ti->playerLadderStep > 13)
                    ti->playerLadderStep = 10;
            }
            else
            {
                th->variant = 1;
                gCurTask->playerLadderStep = 0;
            }
            goto callit;
        case ABILITY_FIRE:
        case ABILITY_SPARK:
        case ABILITY_BURNING:
        case ABILITY_TORNADO:
            tj = gCurTask;
            m = tj->playerLadderDir;
            if (m == 1)
            {
                if ((gLatchedPressedKeys[tj->player->playerIndex] & 64) != 0)
                {
                    tj->variant = m;
                    tc = gCurTask;
                    tc->playerLadderStep++;
                    if (tc->playerLadderStep > 15)
                        tc->playerLadderStep = 0;
                }
                else
                {
                    tj->variant = 2;
                    gCurTask->playerLadderStep = 16;
                }
            }
            else if ((gLatchedPressedKeys[tj->player->playerIndex] & 64) != 0)
            {
                tj->variant = 1;
                gCurTask->playerLadderStep = 0;
            }
            else
                goto arm3;
            goto callit;
        set5:
            tg->player->requestedAction = PLAYER_ACTION_JUMP;
            return;
        set1:
            tg->player->requestedAction = PLAYER_ACTION_STAND;
            return;
        arm3:
            tj->variant = 2;
            td = gCurTask;
            td->playerLadderStep++;
            if (td->playerLadderStep > 20)
                td->playerLadderStep = 16;
        callit:
            TaskSetEntry(PlayerActionLadder, gCurTaskIdx);
            break;
        }
        break;
    }
    qd = gLatchedPressedKeys;
    te = gCurTask;
    if ((qd[te->player->playerIndex] & 48) != 0)
    {
        te->facing = te->playerLadderSavedFacing;
        tf = gCurTask;
        tf->spriteFlags &= ~SPRITE_FLAG_FLIP_X;
        if (tf->velY != 0)
            PlayerStopAxes(2);
        PlayerRequestLocomotion();
    }
    else if ((gTerrainResult.unk6 & 3) == 0)
    {
        te->facing = te->playerLadderSavedFacing;
        tg = gCurTask;
        if (tg->variant == 1)
            goto set5;
        if (tg->onGround & 1)
            goto set1;
        tg->player->requestedAction = PLAYER_ACTION_FALL;
    }
}

void PlayerActionInhale(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 10;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_INHALE;
    t = gCurTask;
    if (t->player->prevMode != 10)
    {
        t->playerInhaleCaught = 0;
        t->variant = 0;
    }
    u = gCurTask;
    switch (u->variant)
    {
    case 0:
        u->playerCatchBlockDelay = 1;
        u->playerInhaleHoldTimer = 30;
        u->playerInhaleAirColliderTimer = -1;
        {
            struct PlayerState *p = u->player;

            p->abilitySwallowCount = 0;
            p->catchKind = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
        {
            struct PlayerState *p = gCurTask->player;

            p->ability = ABILITY_NORMAL;
            p->pendingAbility = ABILITY_NORMAL;
        }
        {
            struct PlayerState *p = gCurTask->player;

            p->abilityUses = 255;
            p->pendingAbilityUses = -1;
        }
        if (gUnk_0300244C != 0)
            gAbilityStarInMouth[gCurTask->player->playerIndex] = 0;
        gUnk_03001F2C = 0;
        do
        {
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].offsetX = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].offsetY = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].velX = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].frameTimer = 1;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].frame = 0;
            gUnk_03001F2C++;
        } while (gUnk_03001F2C <= 2);
        LoadAbilityTiles();
        gCurTask->variant = 1;
        TaskSetFrame(57);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStartSfx(103, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_INHALE_AIR, 0);
        gCurTask->player->actionFlags |= PLAYER_ACTION_FLAG_CATCHING;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        while (1)
        {
            TaskSetFrame(60);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
            gCurTask->frame++;
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 2; gCurTask->playerLoopCount++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
        }
    hit:
        gCurTask->variant = 2;
    case 2:
        gCurTask->player->actionFlags &= ~PLAYER_ACTION_FLAG_CATCHING;
        PlayerStopSfx();
        {
            struct PlayerState *q = gCurTask->player;

            if ((s8)q->attachedCount == 0 || q->catchKind == 3)
            {
                TaskSetFrame(57);
                TaskYieldTrampoline(2);
            }
            else
            {
                struct Task *w;
                struct PlayerState *r;

                PlaySfxIfLocalPlayer(104, q->playerIndex);
                w = gCurTask;
                if (w->onGround & 1)
                {
                    CreatePlayerEffect(w->player->playerIndex, PLAYER_EFFECT_VARIANT_CATCH_DUST, 0);
                    CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_CATCH_DUST, 1);
                }
                TaskSetFrame(62);
                TaskYieldTrampoline(1);
                PlayerStartOffsetScript(2);
                for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 4; gCurTask->playerLoopCount++)
                {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
                r = gCurTask->player;
                if ((s8)r->heldCount == 0)
                    r->mouthState = 0;
            }
        }
        gCurTask->variant = 3;
    }
    TaskSleepForever();
}

void PlayerActionInhaleUpdate(void)
{
    struct Task *t;

    while (1)
    {
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            PlayerSetWaterMotionY();
            if ((s8)gCurTask->player->attachedCount == 0)
            {
                gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
                break;
            }
        }
        else
        {
            t = gCurTask;
            if ((t->waterFlags & 1) && (s8)t->player->attachedCount == 0)
            {
                PlayerSetWaterMotionY();
                gCurTask->player->requestedAction = PLAYER_ACTION_SWIM;
                break;
            }
        }
        t = gCurTask;
        switch (t->variant)
        {
        case 1:
        {
            struct Task *u;
            struct PlayerState *p = t->player;

            if ((s8)p->attachedCount == 0)
            {
                if (t->playerInhaleHoldTimer == 0)
                {
                    if (!(gLatchedHeldKeys[p->playerIndex] & 2))
                    {
                        t->variant = 2;
                        TaskSetEntry(PlayerActionInhale, gCurTaskIdx);
                        break;
                    }
                }
                else
                {
                    t->playerInhaleHoldTimer--;
                }
            }
            u = gCurTask;
            if (u->player->catchKind == 0)
            {
                if (u->playerCatchBlockDelay == 0)
                {
                    if (TaskBreakFirstBlock(&gUnk_0873CC54, u->player->playerIndex) != 0)
                    {
                        gCurTask->player->catchKind = 2;
                        CreateBlockStar(gBrokenBlockX + 8, gBrokenBlockY + 8, gCurTaskIdx, HIT_KIND_INHALE, HIT_EFFECT_INHALE);
                    }
                }
                else
                {
                    u->playerCatchBlockDelay--;
                }
                u = gCurTask;
                if (u->player->catchKind == 0)
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BEC4);
            }
            else if (u->player->catchKind == 1)
            {
                if (u->playerInhaleAirColliderTimer == -1)
                {
                    if (!(u->onGround & 1))
                        u->playerInhaleAirColliderTimer = 8;
                    if (gCurTask->playerInhaleAirColliderTimer == -1)
                        goto skip;
                }
                u = gCurTask;
                if (u->playerInhaleAirColliderTimer != 0)
                {
                    u->playerInhaleAirColliderTimer--;
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BEC4);
                }
            skip:;
            }
            if ((s8)gCurTask->player->attachedCount != 0 && gCurTask->playerInhaleCaught == 0)
            {
                PlayerStartOffsetScript(1);
                gCurTask->playerInhaleCaught++;
            }
            break;
        }
        case 0:
        case 2:
            break;
        case 3:
            if (t->waterFlags & 1)
                t->player->requestedAction = PLAYER_ACTION_SWIM;
            else if (t->onGround & 1)
                t->player->requestedAction = PLAYER_ACTION_STAND;
            else
                t->player->requestedAction = PLAYER_ACTION_FALL;
            break;
        }
        break;
    }
    t = gCurTask;
    if (t->onGround & 1)
    {
        if (t->velX != 0)
        {
            if (t->waterFlags & 1)
                PlayerSetMotionXPreset(8, 72);
            else
                PlayerSetMotionXPreset(0, 72);
        }
        PlayerLand(1);
    }
    else if (!(t->waterFlags & 1))
    {
        PlayerSetMotionYPreset(2);
        PlayerSetMotionXPreset(11, 2);
    }
    else
    {
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
    }
    PlayerStopAtCeilingAndWall();
}

void PlayerActionSpit(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 11;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SPIT;
    gCurTask->playerActionDone28 = 0;
    gCurTask->player->mouthState = 0;
    PlayerStartOffsetScript(3);
    TaskSetFrame(79);
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->player->pendingAbility = ABILITY_NORMAL;
    if ((s8)gCurTask->player->attachedCount > 1)
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_SPIT_MULTI_STAR, 0);
    else
        CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_SPIT_STAR, 0);
    if (gCurTask->onGround & 1)
    {
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SPIT_DUST, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SPIT_DUST, 1);
    }
    gCurTask->frame++;
    TaskYieldTrampoline(11);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionSpitUpdate(void)
{
    struct Task *t;

    if (gCurTask->playerActionDone28 != 0)
        PlayerRequestLocomotion();
    if (PlayerHasCrossedWaterSurface(0) != 0)
        PlayerSetWaterMotionY();
    t = gCurTask;
    if (t->onGround & 1)
    {
        if (t->velX != 0)
        {
            if (!(t->waterFlags & 1))
                PlayerSetMotionXPreset(0, 72);
            else
                PlayerSetMotionXPreset(8, 72);
        }
        PlayerLand(1);
    }
    else if (!(t->waterFlags & 1))
    {
        PlayerSetMotionYPreset(2);
        PlayerSetMotionXPreset(11, 2);
    }
    else
    {
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
    }
    PlayerStopAtCeilingAndWall();
}

void PlayerActionSwallow(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 12;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_SWALLOW;
    gCurTask->playerActionDone28 = 0;
    gCurTask->player->mouthState = 0;
    TaskSetFrame(68);
    TaskYieldTrampoline(2);
    PlaySfxIfLocalPlayer(113, gCurTask->player->playerIndex);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->playerActionDone28++;
    TaskSleepForever();
}

void PlayerActionSwallowUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->playerActionDone28 != 0)
    {
        gLatchedHeldKeys[t->player->playerIndex] = gLatchedPressedKeys[t->player->playerIndex] = 0;
        PlayerRequestLocomotion();
    }
    u = gCurTask;
    if (u->onGround & 1)
    {
        if (u->velX != 0)
        {
            if (!(u->waterFlags & 1))
                PlayerSetMotionXPreset(8, 72);
            else
                PlayerSetMotionXPreset(0, 72);
        }
        PlayerLand(1);
    }
    else if (!(u->waterFlags & 1))
    {
        PlayerSetMotionYPreset(2);
        PlayerSetMotionXPreset(11, 2);
    }
    else
    {
        PlayerSetMotionYPreset(13);
        PlayerSetMotionXPreset(11, 4);
    }
    PlayerStopAtCeilingAndWall();
}
