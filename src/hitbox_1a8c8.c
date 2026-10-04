#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "hud.h"
#include "room.h"
#include "camera.h"

/* hitbox_1a8c8.c (0x0801A8C8-0x0801B24B, issue #84).
 *
 * The actor-vs-collider hit tests M17/M18's actors run (src/actor_673ec.c
 * calls them after PlaceAttackBox has placed the actor's attack box): each walks
 * one of the collider lists M05's RegisterCollider fills, places the collider's
 * body box the same way, tests the overlap and, on a hit, writes the hit
 * result (gHitKind = hit kind, gHitEffect, gHitHealthLeft) through
 * the shared tails CalcHitDamageAndDirection/sub_0801b9e4 and returns 1.
 * HitTestPlayerColliders tests the players' list gPlayerColliders (by the attack's class
 * gAttackBox->unk06 & 7: 0 a damaging hit, 1 a hit the body box can
 * block, 2/3 a touch that marks the player in gUnk_03001F24);
 * HitTestColliderClass10 tests the second list gColliderClass10.  The third list's test,
 * HitTestColliderClass20, is src/hitbox_1b24c.c.
 * 
 * Matching notes: the class 2/3 case is the last case in the source, because
 * merge_blocks moves its head up behind the dispatch; hit paths end at an
 * in-loop `sub_0801b9e4(); return 1;` (loop.c moves lone exit blocks out of
 * the loop); the attack's flags are read from gAttackBox at every test. */

/* An actor's attack box (ROM), pointed to by gAttackBox during the
   actor-vs-player hit tests: signed offsets from the actor's position
   (unk00/unk01) and the box edges relative to that point (left unk02, top
   unk03, right unk04, bottom unk05), then the attack's kind and flags. */
struct AttackBox
{
    /*0x00*/ s8 unk00;
    /*0x01*/ s8 unk01;
    /*0x02*/ s8 unk02;
    /*0x03*/ s8 unk03;
    /*0x04*/ s8 unk04;
    /*0x05*/ s8 unk05;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 unk08;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u16 unk0A;
    /*0x0C*/ u16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12;
    /*0x14*/ u32 unk14;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
};

/* A player's body box, pointed to by each entry of the hit list
   gPlayerColliders and cached in gColliderBodyBox: the same six signed offsets,
   then per-box bytes. */
struct BodyBox
{
    /*0x00*/ s8 unk00;
    /*0x01*/ s8 unk01;
    /*0x02*/ s8 unk02;
    /*0x03*/ s8 unk03;
    /*0x04*/ s8 unk04;
    /*0x05*/ s8 unk05;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 unk08;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u16 unk10;
};

/* One entry of a collider list (src/player_1a76c.c's struct Collider,
   filled by M05's RegisterCollider): the owner's task index, its position and
   its body box.  The three lists are gPlayerColliders[gPlayerColliderCount] (up to 4,
   the players), gColliderClass10[gColliderClass10Count] and
   gColliderClass20[gColliderClass20Count] (up to 20 each), picked by the high nibble
   of the body box's byte 8. */
struct HitEntry
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 filler01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ u16 filler06;
    /*0x08*/ struct BodyBox *unk08;
};

/* Actor-vs-player hit test cells (M17's src/actor_673ec.c widths). */
/* Not from collision.h: this file's view of gPlayerColliders differs (lesson
   3.517). */
extern u8 gUnk_03001F24;
extern s8 gAttackHitDuration;
extern u8 gHitTimer;
extern u16 gAttackX;           /* actor x */
extern u16 gAttackHealth;
extern struct AttackBox *gAttackBox; /* the actor's attack box (s32 in actor_673ec.c) */
extern u8 gHitKind;            /* hit result */
extern u8 gAttackLastHitterSlot;
extern u16 gHitHealthLeft;
extern u8 gHitterColliderClass;
extern u8 gHitterSlot;
extern u8 gAttackLastHitter;
extern u8 gHitEffect;
extern u8 gAttackLastHitterClass;
extern s16 gAttackBoxBottom;           /* attack box bottom */
extern s16 gAttackBoxTop;           /* attack box top */
extern s16 gAttackBoxRight;           /* attack box right */
extern s16 gAttackBoxLeft;           /* attack box left */
extern u8 gPlayerColliderCount;            /* number of hit-list entries */
extern struct HitEntry gPlayerColliders[];
extern struct BodyBox *gColliderBodyBox;   /* the current entry's body box */
extern struct PlayerState *gColliderPlayerState; /* the current entry's player */
extern u8 gColliderPlayer;            /* the current entry's player index */
extern u8 gColliderSlot;            /* the current entry's task index */
extern s16 gColliderX;           /* the current entry's x */
extern s16 gColliderY;           /* the current entry's y */
extern s16 gColliderLeft;           /* body box left */
extern s16 gColliderRight;           /* body box right */
extern s16 gColliderTop;           /* body box top */
extern s16 gColliderBottom;           /* body box bottom */
extern struct HitEntry gColliderClass10[];
extern u8 gColliderClass10Count;
extern u16 gUnk_08732218[];
extern u16 gUnk_08732224[];

void CalcHitDamageAndDirection(void);
void sub_0801b9e4(void);

/* Hit test of the actor's attack box against the players' hit list
   gPlayerColliders.  By the attack's class (unk06 & 7): 0 = a damaging hit
   (hit kind 2/6, health and knock-back through CalcHitDamageAndDirection), 1 = hit kind
   8 unless the body box blocks it, 2/3 = a touch (kind 7) that marks the
   player in gUnk_03001F24 (class 2 returns at the first one, class 3 after
   the list).  Class 2/3 comes last in the switch: merge_blocks moves its
   head up behind the dispatch, which is why its tail sits after class 1. */
u8 HitTestPlayerColliders(void)
{
    struct HitEntry *e;
    struct Task *t;
    struct PlayerState *ps;
    s32 i;
    u8 s;

    gHitKind = 0;
    gUnk_03001F24 = 0;
    e = gPlayerColliders;
    for (i = 0; i < gPlayerColliderCount; i++)
    {
        gColliderSlot = e->unk00;
        /* gAttackLastHitterClass is compared signed (lsls/asrs) */
        if (gColliderSlot == (s8)gAttackLastHitterSlot && *(s8 *)&gAttackLastHitterClass == 0)
        {
            e++;
            continue;
        }
        t = &gTasks[gColliderSlot];
        gColliderPlayerState = t->player;
        if (gColliderPlayerState->hitsThisFrame != 0)
        {
            e++;
            continue;
        }
        gColliderPlayer = gColliderPlayerState->playerIndex;
        gColliderBodyBox = e->unk08;
        /* the list entry's position is unsigned (ldrh) */
        if (gColliderBodyBox->unk10 & 0x8000)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->unk00 + e->unk02;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk02;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk04;
        }
        else if (t->facing == 1)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->unk00 + e->unk02;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk02;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk04;
        }
        else
        {
            s32 x;
            gColliderX = x = -gColliderBodyBox->unk00 + e->unk02;
            gColliderLeft = (x - (u16)gViewRect[0]) - gColliderBodyBox->unk04;
            gColliderRight = (x - (u16)gViewRect[0]) - gColliderBodyBox->unk02;
        }
        gColliderY = gColliderBodyBox->unk01 + e->unk04;
        e++;
        gColliderTop = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->unk03;
        gColliderBottom = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->unk05;
        switch (gAttackBox->unk06 & 7)
        {
        case 0:
            ps = gColliderPlayerState;
            s = ps->invulnerability;
            if (s == 2)
                continue;
            if (gColliderRight < gAttackBoxLeft)
                continue;
            if (gAttackBoxRight < gColliderLeft)
                continue;
            if (gColliderBottom < gAttackBoxTop)
                continue;
            if (gAttackBoxBottom < gColliderTop)
                continue;
            if (ps->invincible == 1)
            {
                if (gAttackBox->unk0A & 0x8000)
                    continue;
                if (gAttackBox->unk0A & 8)
                {
                    gHitKind = 6;
                    gHitEffect = 12;
                    gHitHealthLeft = gAttackHealth;
                }
                else if (gAttackBox->unk0A & 4)
                {
                    gHitKind = 6;
                    gHitEffect = 11;
                    gHitHealthLeft = gAttackHealth;
                }
                else
                {
                    gHitKind = 2;
                    CalcHitDamageAndDirection();
                    t->unk76 = (t->unk76 & 0x4000) | 1;
                }
            }
            else if (s == 0)
            {
                if (!(gAttackBox->unk0C & 0x4005)
                    && !((gAttackBox->unk1A & 0x40) && (t->unk76 & 0x4000)))
                {
                    t->hitEffect = gAttackBox->unk09;
                    /* the actor's x is read signed here (ldrsh) */
                    if ((s16)gAttackX < gColliderX)
                        t->hitDirection = 0;
                    else
                        t->hitDirection = 4;
                    gColliderPlayerState->hitsThisFrame++;
                    AddPlayerHealth(-gAttackBox->unk08, gColliderPlayer);
                    if (gPlayerHealth[gColliderPlayer] <= 0)
                    {
                        t->hitKind = 1;
                        t->hitEffect = gAttackBox->unk1A & 0x300;
                    }
                    else
                    {
                        t->hitKind = 2;
                    }
                }
                if (gAttackBox->unk0A & 0x8000)
                    continue;
                if (gAttackBox->unk0A & 4)
                {
                    gHitKind = 6;
                    gHitEffect = 11;
                    gHitHealthLeft = gAttackHealth;
                }
                else if (gAttackBox->unk0A & 1)
                {
                    gHitKind = 6;
                    gHitEffect = 1;
                    gHitHealthLeft = gAttackHealth;
                }
                else
                {
                    gHitKind = 2;
                    CalcHitDamageAndDirection();
                }
            }
            else if (s == 1 || s == 3)
            {
                if (gAttackBox->unk0A & 0x8000)
                    continue;
                if (gAttackBox->unk0A & 4)
                {
                    gHitKind = 6;
                    gHitEffect = 11;
                    gHitHealthLeft = gAttackHealth;
                }
                else
                {
                    gHitKind = 2;
                    CalcHitDamageAndDirection();
                }
            }
            sub_0801b9e4();
            return 1;
        case 1:
            ps = gColliderPlayerState;
            if (ps->invulnerability >= 1 && ps->invulnerability <= 3)
                continue;
            if (gColliderRight < gAttackBoxLeft)
                continue;
            if (gAttackBoxRight < gColliderLeft)
                continue;
            if (gColliderBottom < gAttackBoxTop)
                continue;
            if (gAttackBoxBottom < gColliderTop)
                continue;
            gHitEffect = 0;
            /* the body box's halfword at 0x0E (ldrh) */
            if (!(gColliderBodyBox->unk0E & 6))
            {
                ps->hitsThisFrame++;
                gHitKind = 8;
                gHitHealthLeft = gAttackHealth;
                sub_0801b9e4();
                return 1;
            }
            gHitKind = 6;
            continue;
        default:
            continue;
        case 2:
        case 3:
            if (gColliderPlayerState->invulnerability == 2)
                continue;
            if (gColliderRight < gAttackBoxLeft)
                continue;
            break;
        }
        if (gAttackBoxRight < gColliderLeft)
            continue;
        if (gColliderBottom < gAttackBoxTop)
            continue;
        if (gAttackBoxBottom < gColliderTop)
            continue;
        gHitKind = 7;
        gHitterColliderClass = 0;
        gHitHealthLeft = gAttackHealth;
        gHitterSlot = gColliderSlot;
        gHitTimer = gAttackHitDuration;
        if ((gAttackBox->unk06 & 7) == 2)
        {
            gUnk_03001F24 = gColliderPlayer;
            return 1;
        }
        gUnk_03001F24 |= 1 << gColliderPlayer;
    }
    if ((gAttackBox->unk06 & 7) == 3 && gHitKind == 7)
        return 1;
    return 0;
}

/* Hit test of the actor's attack box against the second hit list. */
u8 HitTestColliderClass10(void)
{
    struct HitEntry *e;
    struct Task *t;
    struct Task *u;
    s32 i;
    u32 k;
    u16 mask;

    e = gColliderClass10;
    for (i = 0; i < gColliderClass10Count; i++)
    {
        gColliderSlot = e->unk00;
        t = &gTasks[gColliderSlot];
        gColliderPlayerState = t->player;
        gColliderPlayer = gColliderPlayerState->playerIndex;
        gColliderBodyBox = e->unk08;
        if (gColliderPlayer == (s8)gAttackLastHitter)
        {
            e++;
            continue;
        }
        /* the list entry's position is unsigned (ldrh) */
        if (gColliderBodyBox->unk10 & 0x8000)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->unk00 + e->unk02;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk02;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk04;
        }
        else if (t->facing == 1)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->unk00 + e->unk02;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk02;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->unk04;
        }
        else
        {
            s32 x;
            gColliderX = x = -gColliderBodyBox->unk00 + e->unk02;
            gColliderLeft = (x - (u16)gViewRect[0]) - gColliderBodyBox->unk04;
            gColliderRight = (x - (u16)gViewRect[0]) - gColliderBodyBox->unk02;
        }
        gColliderY = gColliderBodyBox->unk01 + e->unk04;
        e++;
        gColliderTop = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->unk03;
        gColliderBottom = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->unk05;
        if (gColliderRight < gAttackBoxLeft)
            continue;
        if (gAttackBoxRight < gColliderLeft)
            continue;
        if (gColliderBottom < gAttackBoxTop)
            continue;
        if (gAttackBoxBottom < gColliderTop)
            continue;
        if (gAttackBox->unk1A & 0x3E)
        {
            if (!((gAttackBox->unk1A >> gColliderBodyBox->unk0D) & 1))
                continue;
            gHitKind = 7;
            gHitHealthLeft = gAttackHealth;
            sub_0801b9e4();
            return 1;
        }
        k = (u32)(gColliderBodyBox->unk08 << 28) >> 28;
        mask = gUnk_08732224[k] | 0x4000;
        /* the body box's halfword at 0x0E (ldrh) */
        if (!(gColliderBodyBox->unk0E & 0x8000) && !(mask & gAttackBox->unk10))
        {
            t->hitEffect = gAttackBox->unk09;
            t->health -= gAttackBox->unk08;
            if (t->health <= 0)
                t->hitKind = 1;
            else
                t->hitKind = 2;
            u = &gTasks[gColliderPlayer];
            if (!(gAttackBox->unk1A & 1))
            {
                u->unk76 &= 0x4000;
                u->unk76 |= gColliderBodyBox->unk10 & 0x3FFF;
            }
        }
        if (!(gAttackBox->unk0E & 0x8000))
        {
            mask = gUnk_08732224[k];
            if (!(mask & gAttackBox->unk0E))
            {
                gHitKind = 2;
                CalcHitDamageAndDirection();
            }
            else
            {
                gHitKind = 6;
                gHitEffect = gUnk_08732218[k];
                gHitHealthLeft = gAttackHealth;
            }
            sub_0801b9e4();
            return 1;
        }
    }
    return 0;
}
