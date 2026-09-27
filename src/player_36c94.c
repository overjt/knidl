#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_36c94.c (0x08036C94-0x08037ED7, issue #91).
 *
 * Player action bodies, part 5: actions 12-15 and per-frame handlers
 * 12-15.  sub_08036c94 (action 12, mode 9) is a four-state machine over
 * Task.unk73 that plays the ability's rows of gUnk_0873D880,
 * gUnk_0873D8B4 and gUnk_0873D908; its handler sub_080371f0 re-binds it
 * when the keys change.  sub_080375e0 (action 13, mode 10) clears the
 * ability (PlayerState.unk0D = 0) and the player's three spark records
 * gUnk_02007E90[player][] before its animation; its handler sub_08037914
 * runs the block-breaking hit box gUnk_0873CC54 through M09's
 * sub_08030898 (spawning the debris with M17's sub_08065100 at the
 * broken block) and M09's collision registry sub_0801a828.  Actions 14
 * and 15 (sub_08037bd4, sub_08037d64) are short animation scripts, and
 * handlers 14 and 15 pick the next velocity preset from the ground flags
 * Task.unk7A/unk7B. */

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
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ s8 (*unk4)[4];
};

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern s16 gUnk_0873D880[];
extern u16 gUnk_0873D8B4[];
extern u16 gUnk_0873D908[];
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */
extern struct Unk03005550 gUnk_03005550;
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
u32 sub_0801a828(u8 idx, s16 x, s16 y, u8 *p);
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

void sub_08036c94(void)
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
        gCurTask->unk88->unk50 = 1;
        h3 = gCurTask;
        h3->unk2C = h3->unk43;
        h3->unk4C = ((h3->unk48 & 0xFFF0) | 8) << 16;
        q = gLatchedHeldKeys;
        if (q[h3->unk88->unk00] & 64)
            h3->unk73 = 1;
        else
            h3->unk73 = 2;
        switch (gCurTask->unk88->unk0D)
        {
        case 0:
        default:
            h4 = gCurTask;
            if (h4->unk73 == 1)
                h4->unk28 = 0;
            else
                h4->unk28 = 10;
            break;
        case 1:
        case 2:
        case 5:
        case 19:
            h4 = gCurTask;
            if (h4->unk73 == 1)
                h4->unk28 = 0;
            else
                h4->unk28 = 16;
            break;
        }
        h5 = gCurTask;
        h5->unk30 = h5->unk73;
    }
    d = gCurTask;
    d->unk46 = gUnk_0873D880[d->unk88->unk0D];
    k = d->unk73;
    switch (k)
    {
    case 1:
        d->unk30 = k;
        switch (d->unk88->unk0D)
        {
        case 0:
        default:
            for (;;)
            {
                if (gCurTask->unk28 == 0 || gCurTask->unk28 == 6)
                    PlaySfxIfLocalPlayer(123, gCurTask->unk88->unk00);
                a1 = gCurTask;
                r = &gUnk_0873D8B4[a1->unk28 * 3];
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
                a1->unk3C = r[0] + a1->unk46;
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
                    PlaySfxIfLocalPlayer(123, gCurTask->unk88->unk00);
                a3 = gCurTask;
                r = &gUnk_0873D908[a3->unk28 * 3];
                x = -r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a3->unk58 = v;
                x = r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a3->unk68 = v;
                a3->unk3C = r[0] + a3->unk46;
                TaskYieldTrampoline(r[1]);
                b3 = gCurTask;
                b3->unk28++;
                if (b3->unk28 > 15)
                    b3->unk28 = 0;
            }
        }
    case 2:
        c2 = gCurTask;
        c2->unk30 = c2->unk73;
        c2->unk88->unk14 = 0;
        PlaySfxIfLocalPlayer(124, c2->unk88->unk00);
        switch (gCurTask->unk88->unk0D)
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
                a2->unk58 = v;
                x = r[2];
                v = x << 8;
                if (x & 0x8000)
                    v |= 0xFF000000;
                a2->unk68 = v;
                a2->unk3C = r[0] + a2->unk46;
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
                a4->unk58 = w;
                x = r[2];
                w = x << 8;
                if (x & 0x8000)
                    w |= 0xFF000000;
                a4->unk68 = w;
                a4->unk3C = r[0] + a4->unk46;
                TaskYieldTrampoline((&gUnk_0873D908[1])[i * 3]);
                b4 = gCurTask;
                b4->unk28++;
                if (b4->unk28 > 20)
                    b4->unk28 = 16;
            }
        }
    case 0:
        PlayerStopAxes(2);
        switch (gCurTask->unk88->unk0D)
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
                gCurTask->unk3C = gCurTask->unk34 + gCurTask->unk46;
                TaskYieldTrampoline(2);
            }
        }
    }
}

void sub_080371f0(void)
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
        t->unk3C = gUnk_0873D880[t->unk88->unk0D];
        TaskSetEntry(sub_08036c94, gCurTaskIdx);
        if (gUnk_0300244C != 0)
            gCurTask->unk88->unk01 = 0;
        return;
    }
    t = gCurTask;
    k = t->unk73;
    switch (k)
    {
    case 1:
        if (gUnk_03005550.unk1 != 0 || (t->unk88->unk48 & 4) != 0)
            t->unk58 = 0;
        qa = gLatchedHeldKeys;
        ta = gCurTask;
        if ((qa[ta->unk88->unk00] & 192) == 0)
            ta->unk73 = 0;
        else if ((qa[ta->unk88->unk00] & 128) != 0)
            ta->unk73 = 2;
        if (gCurTask->unk73 == 1)
            break;
        TaskSetEntry(sub_08036c94, gCurTaskIdx);
        break;
    case 2:
        p = t->unk88;
        k &= p->unk14;
        if (k != 0)
        {
            PlaySfxIfLocalPlayer(124, p->unk00);
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
        TaskSetEntry(sub_08036c94, gCurTaskIdx);
        break;
    case 0:
        if ((gLatchedPressedKeys[t->unk88->unk00] & 192) == 0)
            break;
        switch (t->unk88->unk0D)
        {
        case 0:
        default:
            th = gCurTask;
            n = th->unk30;
            if (n == 1)
            {
                if ((gLatchedPressedKeys[th->unk88->unk00] & 64) != 0)
                {
                    th->unk73 = n;
                    tc = gCurTask;
                    tc->unk28++;
                    if (tc->unk28 > 9)
                        tc->unk28 = 0;
                }
                else
                {
                    th->unk73 = 2;
                    gCurTask->unk28 = 10;
                }
            }
            else if ((gLatchedPressedKeys[th->unk88->unk00] & 64) == 0)
            {
                th->unk73 = 2;
                ti = gCurTask;
                ti->unk28++;
                if (ti->unk28 > 13)
                    ti->unk28 = 10;
            }
            else
            {
                th->unk73 = 1;
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
                if ((gLatchedPressedKeys[tj->unk88->unk00] & 64) != 0)
                {
                    tj->unk73 = m;
                    tc = gCurTask;
                    tc->unk28++;
                    if (tc->unk28 > 15)
                        tc->unk28 = 0;
                }
                else
                {
                    tj->unk73 = 2;
                    gCurTask->unk28 = 16;
                }
            }
            else if ((gLatchedPressedKeys[tj->unk88->unk00] & 64) != 0)
            {
                tj->unk73 = 1;
                gCurTask->unk28 = 0;
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
            tj->unk73 = 2;
            td = gCurTask;
            td->unk28++;
            if (td->unk28 > 20)
                td->unk28 = 16;
        callit:
            TaskSetEntry(sub_08036c94, gCurTaskIdx);
            break;
        }
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
    else if ((gUnk_03005550.unk6 & 3) == 0)
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

void sub_080375e0(void)
{
    struct Task *t;
    struct Task *u;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 10;
    gCurTask->unk15 = 13;
    t = gCurTask;
    if (t->unk88->unk05 != 10)
    {
        t->unk30 = 0;
        t->unk73 = 0;
    }
    u = gCurTask;
    switch (u->unk73)
    {
    case 0:
        u->unk28 = 1;
        u->unk2C = 30;
        u->unk34 = -1;
        {
            struct PlayerState *p = u->unk88;

            p->unk0A = 0;
            p->unk09 = 0;
            p->unk08 = 0;
            p->unk07 = 0;
        }
        {
            struct PlayerState *p = gCurTask->unk88;

            p->unk0D = 0;
            p->unk0B = 0;
        }
        {
            struct PlayerState *p = gCurTask->unk88;

            p->unk0E = 255;
            p->unk0C = -1;
        }
        if (gUnk_0300244C != 0)
            gUnk_02007CF4[gCurTask->unk88->unk00] = 0;
        gUnk_03001F2C = 0;
        do
        {
            gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk00 = 0;
            gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk04 = 0;
            gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk08 = 0;
            gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk0C = 1;
            gUnk_02007E90[gCurTask->unk88->unk00][gUnk_03001F2C].unk0D = 0;
            gUnk_03001F2C++;
        } while (gUnk_03001F2C <= 2);
        LoadAbilityTiles();
        gCurTask->unk73 = 1;
        TaskSetFrame(57);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerStartSfx(103, gCurTask->unk88->unk00);
        CreatePlayerEffect(gCurTask->unk88->unk00, 0, 0);
        gCurTask->unk88->unk40 |= 4;
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 1;
    case 1:
        while (1)
        {
            TaskSetFrame(60);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                if ((s8)gCurTask->unk88->unk07 != 0 && (s8)gCurTask->unk88->unk07 == (s8)gCurTask->unk88->unk08)
                    goto hit;
                TaskYieldTrampoline(1);
            }
            gCurTask->unk3C++;
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
            {
                if ((s8)gCurTask->unk88->unk07 != 0 && (s8)gCurTask->unk88->unk07 == (s8)gCurTask->unk88->unk08)
                    goto hit;
                TaskYieldTrampoline(1);
            }
        }
    hit:
        gCurTask->unk73 = 2;
    case 2:
        gCurTask->unk88->unk40 &= 0xFFFB;
        PlayerStopSfx();
        {
            struct PlayerState *q = gCurTask->unk88;

            if ((s8)q->unk07 == 0 || q->unk09 == 3)
            {
                TaskSetFrame(57);
                TaskYieldTrampoline(2);
            }
            else
            {
                struct Task *w;
                struct PlayerState *r;

                PlaySfxIfLocalPlayer(104, q->unk00);
                w = gCurTask;
                if (w->unk7A & 1)
                {
                    CreatePlayerEffect(w->unk88->unk00, 1, 0);
                    CreatePlayerEffect(gCurTask->unk88->unk00, 1, 1);
                }
                TaskSetFrame(62);
                TaskYieldTrampoline(1);
                PlayerStartOffsetScript(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
                r = gCurTask->unk88;
                if ((s8)r->unk08 == 0)
                    r->unk06 = 0;
            }
        }
        gCurTask->unk73 = 3;
    }
    TaskSleepForever();
}

void sub_08037914(void)
{
    struct Task *t;

    while (1)
    {
        if (PlayerHasCrossedWaterSurface(0) != 0)
        {
            PlayerSetWaterMotionY();
            if ((s8)gCurTask->unk88->unk07 == 0)
            {
                gCurTask->unk88->unk01 = 23;
                break;
            }
        }
        else
        {
            t = gCurTask;
            if ((t->unk7B & 1) && (s8)t->unk88->unk07 == 0)
            {
                PlayerSetWaterMotionY();
                gCurTask->unk88->unk01 = 23;
                break;
            }
        }
        t = gCurTask;
        switch (t->unk73)
        {
        case 1:
        {
            struct Task *u;
            struct PlayerState *p = t->unk88;

            if ((s8)p->unk07 == 0)
            {
                if (t->unk2C == 0)
                {
                    if (!(gLatchedHeldKeys[p->unk00] & 2))
                    {
                        t->unk73 = 2;
                        TaskSetEntry(sub_080375e0, gCurTaskIdx);
                        break;
                    }
                }
                else
                {
                    t->unk2C--;
                }
            }
            u = gCurTask;
            if (u->unk88->unk09 == 0)
            {
                if (u->unk28 == 0)
                {
                    if (sub_08030898(&gUnk_0873CC54, u->unk88->unk00) != 0)
                    {
                        gCurTask->unk88->unk09 = 2;
                        sub_08065100(gUnk_02007FA0 + 8, gUnk_02004B6C + 8, gCurTaskIdx, 3, 1);
                    }
                }
                else
                {
                    u->unk28--;
                }
                u = gCurTask;
                if (u->unk88->unk09 == 0)
                    sub_0801a828(gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873BEC4);
            }
            else if (u->unk88->unk09 == 1)
            {
                if (u->unk34 == -1)
                {
                    if (!(u->unk7A & 1))
                        u->unk34 = 8;
                    if (gCurTask->unk34 == -1)
                        goto skip;
                }
                u = gCurTask;
                if (u->unk34 != 0)
                {
                    u->unk34--;
                    sub_0801a828(gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873BEC4);
                }
            skip:;
            }
            if ((s8)gCurTask->unk88->unk07 != 0 && gCurTask->unk30 == 0)
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
            if (t->unk7B & 1)
                t->unk88->unk01 = 23;
            else if (t->unk7A & 1)
                t->unk88->unk01 = 1;
            else
                t->unk88->unk01 = 7;
            break;
        }
        break;
    }
    t = gCurTask;
    if (t->unk7A & 1)
    {
        if (t->unk54 != 0)
        {
            if (t->unk7B & 1)
                PlayerSetMotionXPreset(8, 72);
            else
                PlayerSetMotionXPreset(0, 72);
        }
        PlayerLand(1);
    }
    else if (!(t->unk7B & 1))
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

void sub_08037bd4(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 11;
    gCurTask->unk15 = 14;
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk06 = 0;
    PlayerStartOffsetScript(3);
    TaskSetFrame(79);
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk88->unk0B = 0;
    if ((s8)gCurTask->unk88->unk07 > 1)
        CreatePlayerObject(gCurTask->unk88->unk00, 2, 0);
    else
        CreatePlayerObject(gCurTask->unk88->unk00, 1, 0);
    if (gCurTask->unk7A & 1)
    {
        CreatePlayerEffect(gCurTask->unk88->unk00, 2, 0);
        CreatePlayerEffect(gCurTask->unk88->unk00, 2, 1);
    }
    gCurTask->unk3C++;
    TaskYieldTrampoline(11);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08037cc8(void)
{
    struct Task *t;

    if (gCurTask->unk28 != 0)
        PlayerRequestLocomotion();
    if (PlayerHasCrossedWaterSurface(0) != 0)
        PlayerSetWaterMotionY();
    t = gCurTask;
    if (t->unk7A & 1)
    {
        if (t->unk54 != 0)
        {
            if (!(t->unk7B & 1))
                PlayerSetMotionXPreset(0, 72);
            else
                PlayerSetMotionXPreset(8, 72);
        }
        PlayerLand(1);
    }
    else if (!(t->unk7B & 1))
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

void sub_08037d64(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 12;
    gCurTask->unk15 = 15;
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk06 = 0;
    TaskSetFrame(68);
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
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(8);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08037e28(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->unk28 != 0)
    {
        gLatchedHeldKeys[t->unk88->unk00] = gLatchedPressedKeys[t->unk88->unk00] = 0;
        PlayerRequestLocomotion();
    }
    u = gCurTask;
    if (u->unk7A & 1)
    {
        if (u->unk54 != 0)
        {
            if (!(u->unk7B & 1))
                PlayerSetMotionXPreset(8, 72);
            else
                PlayerSetMotionXPreset(0, 72);
        }
        PlayerLand(1);
    }
    else if (!(u->unk7B & 1))
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
