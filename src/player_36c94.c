#include "gba/gba.h"
#include "global.h"
#include "task.h"

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
 * sub_08030898 (spawning the debris with M17's sub_08065100 at the
 * broken block) and M09's collision registry RegisterCollider.  Actions 14
 * and 15 (PlayerActionSpit, PlayerActionSwallow) are short animation scripts, and
 * handlers 14 and 15 pick the next velocity preset from the ground flags
 * Task.onGround/unk7B. */

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 ceilingHits;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 slope;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 atDoor;
    /*0x0B*/ u8 onSlipperyFloor;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

/* gUnk_02007E90[4][3]: M04's per-player spark records (src/player_10358.c) */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
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

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern s16 gUnk_0873D880[];
extern u16 gUnk_0873D8B4[];
extern u16 gUnk_0873D908[];
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gTerrainResult;
extern s16 gUnk_0300244C;
extern struct M04Spark gUnk_02007E90[][3];
extern u8 gUnk_02007CF4[];
extern s32 gUnk_03001F2C;
extern struct HitBoxSet gUnk_0873CC54;
extern u16 gUnk_02007FA0;
extern u16 gUnk_02004B6C;
extern u8 gUnk_0873BEC4[];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);
u16 sub_08030898(struct HitBoxSet *p, s32 e);
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
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 sub_08065100(s16 x, s16 y, u32 p2, u8 p3, u8 p4);

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
    gCurTask->updateState = 12;
    if (gCurTask->player->prevMode != 9)
    {
        PlayerStopAxes(3);
        gCurTask->facing = 1;
        h1 = gCurTask;
        h1->spriteFlags &= 0x7FFF;
        h1->player->running = 0;
        gCurTask->player->unk50 = 1;
        h3 = gCurTask;
        h3->unk2C = h3->facing;
        h3->posX = ((h3->pixelX & 0xFFF0) | 8) << 16;
        q = gLatchedHeldKeys;
        if (q[h3->player->playerIndex] & 64)
            h3->variant = 1;
        else
            h3->variant = 2;
        switch (gCurTask->player->ability)
        {
        case 0:
        default:
            h4 = gCurTask;
            if (h4->variant == 1)
                h4->unk28 = 0;
            else
                h4->unk28 = 10;
            break;
        case 1:
        case 2:
        case 5:
        case 19:
            h4 = gCurTask;
            if (h4->variant == 1)
                h4->unk28 = 0;
            else
                h4->unk28 = 16;
            break;
        }
        h5 = gCurTask;
        h5->unk30 = h5->variant;
    }
    d = gCurTask;
    d->unk46 = gUnk_0873D880[d->player->ability];
    k = d->variant;
    switch (k)
    {
    case 1:
        d->unk30 = k;
        switch (d->player->ability)
        {
        case 0:
        default:
            for (;;)
            {
                if (gCurTask->unk28 == 0 || gCurTask->unk28 == 6)
                    PlaySfxIfLocalPlayer(123, gCurTask->player->playerIndex);
                a1 = gCurTask;
                r = &gUnk_0873D8B4[a1->unk28 * 3];
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
                a1->frame = r[0] + a1->unk46;
                TaskYieldTrampoline(r[1]);
                b1 = gCurTask;
                b1->unk28++;
                if (b1->unk28 > 9)
                    b1->unk28 = 0;
            }
        case 1:
        case 2:
        case 5:
        case 19:
            for (;;)
            {
                if (gCurTask->unk28 == 0 || gCurTask->unk28 == 9)
                    PlaySfxIfLocalPlayer(123, gCurTask->player->playerIndex);
                a3 = gCurTask;
                r = &gUnk_0873D908[a3->unk28 * 3];
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
                a3->frame = r[0] + a3->unk46;
                TaskYieldTrampoline(r[1]);
                b3 = gCurTask;
                b3->unk28++;
                if (b3->unk28 > 15)
                    b3->unk28 = 0;
            }
        }
    case 2:
        c2 = gCurTask;
        c2->unk30 = c2->variant;
        c2->player->unk14 = 0;
        PlaySfxIfLocalPlayer(124, c2->player->playerIndex);
        switch (gCurTask->player->ability)
        {
        case 0:
        default:
            for (;;)
            {
                a2 = gCurTask;
                r = &gUnk_0873D8B4[a2->unk28 * 3];
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
                a2->frame = r[0] + a2->unk46;
                TaskYieldTrampoline(r[1]);
                b2 = gCurTask;
                b2->unk28++;
                if (b2->unk28 > 13)
                    b2->unk28 = 10;
            }
        case 1:
        case 2:
        case 5:
        case 19:
            for (;;)
            {
                a4 = gCurTask;
                i = a4->unk28;
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
                a4->frame = r[0] + a4->unk46;
                TaskYieldTrampoline((&gUnk_0873D908[1])[i * 3]);
                b4 = gCurTask;
                b4->unk28++;
                if (b4->unk28 > 20)
                    b4->unk28 = 16;
            }
        }
    case 0:
        PlayerStopAxes(2);
        switch (gCurTask->player->ability)
        {
        case 0:
        default:
            TaskSleepForever();
        case 1:
        case 2:
        case 5:
        case 19:
            e = gCurTask;
            e->unk34 = gUnk_0873D908[e->unk28 * 3];
            for (;;)
            {
                gCurTask->unk34 += 13;
                if (gCurTask->unk34 > 51)
                    gCurTask->unk34 -= 52;
                gCurTask->frame = gCurTask->unk34 + gCurTask->unk46;
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
            gCurTask->player->requestedAction = 0;
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
        k &= p->unk14;
        if (k != 0)
        {
            PlaySfxIfLocalPlayer(124, p->playerIndex);
            gCurTask->player->unk14 = 0;
        }
        else
        {
            p->unk14++;
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
        case 0:
        default:
            th = gCurTask;
            n = th->unk30;
            if (n == 1)
            {
                if ((gLatchedPressedKeys[th->player->playerIndex] & 64) != 0)
                {
                    th->variant = n;
                    tc = gCurTask;
                    tc->unk28++;
                    if (tc->unk28 > 9)
                        tc->unk28 = 0;
                }
                else
                {
                    th->variant = 2;
                    gCurTask->unk28 = 10;
                }
            }
            else if ((gLatchedPressedKeys[th->player->playerIndex] & 64) == 0)
            {
                th->variant = 2;
                ti = gCurTask;
                ti->unk28++;
                if (ti->unk28 > 13)
                    ti->unk28 = 10;
            }
            else
            {
                th->variant = 1;
                gCurTask->unk28 = 0;
            }
            goto callit;
        case 1:
        case 2:
        case 5:
        case 19:
            tj = gCurTask;
            m = tj->unk30;
            if (m == 1)
            {
                if ((gLatchedPressedKeys[tj->player->playerIndex] & 64) != 0)
                {
                    tj->variant = m;
                    tc = gCurTask;
                    tc->unk28++;
                    if (tc->unk28 > 15)
                        tc->unk28 = 0;
                }
                else
                {
                    tj->variant = 2;
                    gCurTask->unk28 = 16;
                }
            }
            else if ((gLatchedPressedKeys[tj->player->playerIndex] & 64) != 0)
            {
                tj->variant = 1;
                gCurTask->unk28 = 0;
            }
            else
                goto arm3;
            goto callit;
        set5:
            tg->player->requestedAction = 5;
            return;
        set1:
            tg->player->requestedAction = 1;
            return;
        arm3:
            tj->variant = 2;
            td = gCurTask;
            td->unk28++;
            if (td->unk28 > 20)
                td->unk28 = 16;
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
        te->facing = te->unk2C;
        tf = gCurTask;
        tf->spriteFlags &= 0x7FFF;
        if (tf->velY != 0)
            PlayerStopAxes(2);
        PlayerRequestLocomotion();
    }
    else if ((gTerrainResult.unk6 & 3) == 0)
    {
        te->facing = te->unk2C;
        tg = gCurTask;
        if (tg->variant == 1)
            goto set5;
        if (tg->onGround & 1)
            goto set1;
        tg->player->requestedAction = 7;
    }
}

void PlayerActionInhale(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 10;
    gCurTask->updateState = 13;
    t = gCurTask;
    if (t->player->prevMode != 10)
    {
        t->unk30 = 0;
        t->variant = 0;
    }
    u = gCurTask;
    switch (u->variant)
    {
    case 0:
        u->unk28 = 1;
        u->unk2C = 30;
        u->unk34 = -1;
        {
            struct PlayerState *p = u->player;

            p->unk0A = 0;
            p->unk09 = 0;
            p->heldCount = 0;
            p->attachedCount = 0;
        }
        {
            struct PlayerState *p = gCurTask->player;

            p->ability = 0;
            p->pendingAbility = 0;
        }
        {
            struct PlayerState *p = gCurTask->player;

            p->abilityUses = 255;
            p->pendingAbilityUses = -1;
        }
        if (gUnk_0300244C != 0)
            gUnk_02007CF4[gCurTask->player->playerIndex] = 0;
        gUnk_03001F2C = 0;
        do
        {
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk00 = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk04 = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk08 = 0;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk0C = 1;
            gUnk_02007E90[gCurTask->player->playerIndex][gUnk_03001F2C].unk0D = 0;
            gUnk_03001F2C++;
        } while (gUnk_03001F2C <= 2);
        LoadAbilityTiles();
        gCurTask->variant = 1;
        TaskSetFrame(57);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        PlayerStartSfx(103, gCurTask->player->playerIndex);
        CreatePlayerEffect(gCurTask->player->playerIndex, 0, 0);
        gCurTask->player->unk40 |= 4;
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->variant = 1;
    case 1:
        while (1)
        {
            TaskSetFrame(60);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
            gCurTask->frame++;
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                if ((s8)gCurTask->player->attachedCount != 0 && (s8)gCurTask->player->attachedCount == (s8)gCurTask->player->heldCount)
                    goto hit;
                TaskYieldTrampoline(1);
            }
        }
    hit:
        gCurTask->variant = 2;
    case 2:
        gCurTask->player->unk40 &= 0xFFFB;
        PlayerStopSfx();
        {
            struct PlayerState *q = gCurTask->player;

            if ((s8)q->attachedCount == 0 || q->unk09 == 3)
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
                    CreatePlayerEffect(w->player->playerIndex, 1, 0);
                    CreatePlayerEffect(gCurTask->player->playerIndex, 1, 1);
                }
                TaskSetFrame(62);
                TaskYieldTrampoline(1);
                PlayerStartOffsetScript(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
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
                gCurTask->player->requestedAction = 23;
                break;
            }
        }
        else
        {
            t = gCurTask;
            if ((t->waterFlags & 1) && (s8)t->player->attachedCount == 0)
            {
                PlayerSetWaterMotionY();
                gCurTask->player->requestedAction = 23;
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
                if (t->unk2C == 0)
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
                    t->unk2C--;
                }
            }
            u = gCurTask;
            if (u->player->unk09 == 0)
            {
                if (u->unk28 == 0)
                {
                    if (sub_08030898(&gUnk_0873CC54, u->player->playerIndex) != 0)
                    {
                        gCurTask->player->unk09 = 2;
                        sub_08065100(gUnk_02007FA0 + 8, gUnk_02004B6C + 8, gCurTaskIdx, 3, 1);
                    }
                }
                else
                {
                    u->unk28--;
                }
                u = gCurTask;
                if (u->player->unk09 == 0)
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BEC4);
            }
            else if (u->player->unk09 == 1)
            {
                if (u->unk34 == -1)
                {
                    if (!(u->onGround & 1))
                        u->unk34 = 8;
                    if (gCurTask->unk34 == -1)
                        goto skip;
                }
                u = gCurTask;
                if (u->unk34 != 0)
                {
                    u->unk34--;
                    RegisterCollider(gCurTaskIdx, u->pixelX, u->pixelY, gUnk_0873BEC4);
                }
            skip:;
            }
            if ((s8)gCurTask->player->attachedCount != 0 && gCurTask->unk30 == 0)
            {
                PlayerStartOffsetScript(1);
                gCurTask->unk30++;
            }
            break;
        }
        case 0:
        case 2:
            break;
        case 3:
            if (t->waterFlags & 1)
                t->player->requestedAction = 23;
            else if (t->onGround & 1)
                t->player->requestedAction = 1;
            else
                t->player->requestedAction = 7;
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
    gCurTask->updateState = 14;
    gCurTask->unk28 = 0;
    gCurTask->player->mouthState = 0;
    PlayerStartOffsetScript(3);
    TaskSetFrame(79);
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->player->pendingAbility = 0;
    if ((s8)gCurTask->player->attachedCount > 1)
        CreatePlayerObject(gCurTask->player->playerIndex, 2, 0);
    else
        CreatePlayerObject(gCurTask->player->playerIndex, 1, 0);
    if (gCurTask->onGround & 1)
    {
        CreatePlayerEffect(gCurTask->player->playerIndex, 2, 0);
        CreatePlayerEffect(gCurTask->player->playerIndex, 2, 1);
    }
    gCurTask->frame++;
    TaskYieldTrampoline(11);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    gCurTask->unk28++;
    TaskSleepForever();
}

void PlayerActionSpitUpdate(void)
{
    struct Task *t;

    if (gCurTask->unk28 != 0)
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
    gCurTask->updateState = 15;
    gCurTask->unk28 = 0;
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
    gCurTask->unk28++;
    TaskSleepForever();
}

void PlayerActionSwallowUpdate(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->unk28 != 0)
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
