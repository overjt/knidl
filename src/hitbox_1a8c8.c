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
 * the shared tails CalcHitDamageAndDirection/HitRecordHitter and returns 1.
 * HitTestPlayerColliders tests the players' list gPlayerColliders (by the attack's class
 * gAttackBox->unk06 & 7: 0 a damaging hit, 1 a hit the body box can
 * block, 2/3 a touch that marks the player in gUnk_03001F24);
 * HitTestColliderClass10 tests the second list gColliderClass10.  The third list's test,
 * HitTestColliderClass20, is src/hitbox_1b24c.c.
 * 
 * Matching notes: the class 2/3 case is the last case in the source, because
 * merge_blocks moves its head up behind the dispatch; hit paths end at an
 * in-loop `HitRecordHitter(); return 1;` (loop.c moves lone exit blocks out of
 * the loop); the attack's flags are read from gAttackBox at every test. */

/* An actor's attack box (ROM), pointed to by gAttackBox during the
   actor-vs-player hit tests: signed offsets from the actor's position
   (unk00/unk01) and the box edges relative to that point (left unk02, top
   unk03, right unk04, bottom unk05), then the attack's kind and flags. */
struct AttackBox
{
    /*0x00*/ s8 offsetX;
    /*0x01*/ s8 offsetY;
    /*0x02*/ s8 left;
    /*0x03*/ s8 top;
    /*0x04*/ s8 right;
    /*0x05*/ s8 bottom;
    /*0x06*/ u8 playerHitMode;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 damage;
    /*0x09*/ u8 hitEffect;
    /*0x0A*/ u16 immunityFlags;
    /*0x0C*/ u16 playerHarmlessMask;
    /*0x0E*/ u16 class10ImmuneMask;
    /*0x10*/ u16 class10HarmlessMask;
    /*0x12*/ u16 unk12;
    /*0x14*/ u32 class20ImmuneMask;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 attackFlags;
};

/* A player's body box, pointed to by each entry of the hit list
   gPlayerColliders and cached in gColliderBodyBox: the same six signed offsets,
   then per-box bytes. */
struct BodyBox
{
    /*0x00*/ s8 offsetX;
    /*0x01*/ s8 offsetY;
    /*0x02*/ s8 left;
    /*0x03*/ s8 top;
    /*0x04*/ s8 right;
    /*0x05*/ s8 bottom;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 classKind;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ u8 damage;
    /*0x0D*/ u8 hitEffect;
    /*0x0E*/ u16 guardFlags;
    /*0x10*/ u16 bodyFlags;
};

/* One entry of a collider list (src/player_1a76c.c's struct Collider,
   filled by M05's RegisterCollider): the owner's task index, its position and
   its body box.  The three lists are gPlayerColliders[gPlayerColliderCount] (up to 4,
   the players), gColliderClass10[gColliderClass10Count] and
   gColliderClass20[gColliderClass20Count] (up to 20 each), picked by the high nibble
   of the body box's byte 8. */
struct HitEntry
{
    /*0x00*/ u8 slot;
    /*0x01*/ u8 filler01;
    /*0x02*/ u16 x;
    /*0x04*/ u16 y;
    /*0x06*/ u16 filler06;
    /*0x08*/ struct BodyBox *bodyBox;
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
extern u16 gColliderClass10KindBits[];

void CalcHitDamageAndDirection(void);
void HitRecordHitter(void);

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

    gHitKind = HIT_KIND_NONE;
    gUnk_03001F24 = 0;
    e = gPlayerColliders;
    for (i = 0; i < gPlayerColliderCount; i++)
    {
        gColliderSlot = e->slot;
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
        gColliderBodyBox = e->bodyBox;
        /* the list entry's position is unsigned (ldrh) */
        if (gColliderBodyBox->bodyFlags & BODY_BOX_FLAG_NO_MIRROR)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->offsetX + e->x;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->left;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->right;
        }
        else if (t->facing == 1)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->offsetX + e->x;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->left;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->right;
        }
        else
        {
            s32 x;
            gColliderX = x = -gColliderBodyBox->offsetX + e->x;
            gColliderLeft = (x - (u16)gViewRect[0]) - gColliderBodyBox->right;
            gColliderRight = (x - (u16)gViewRect[0]) - gColliderBodyBox->left;
        }
        gColliderY = gColliderBodyBox->offsetY + e->y;
        e++;
        gColliderTop = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->top;
        gColliderBottom = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->bottom;
        switch (gAttackBox->playerHitMode & 7)
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
                if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_NO_RESULT)
                    continue;
                if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_INVINCIBLE)
                {
                    gHitKind = HIT_KIND_NO_DAMAGE;
                    gHitEffect = 12;
                    gHitHealthLeft = gAttackHealth;
                }
                else if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_PLAYERS)
                {
                    gHitKind = HIT_KIND_NO_DAMAGE;
                    gHitEffect = 11;
                    gHitHealthLeft = gAttackHealth;
                }
                else
                {
                    gHitKind = HIT_KIND_DAMAGE;
                    CalcHitDamageAndDirection();
                    t->u76.unk76 = (t->u76.unk76 & PLAYER_HIT_SHIELDED) | PLAYER_HIT_LANDED;
                }
            }
            else if (s == 0)
            {
                if (!(gAttackBox->playerHarmlessMask & 0x4005)
                    && !((gAttackBox->attackFlags & ATTACK_BOX_FLAG_SHIELDABLE) && (t->u76.unk76 & PLAYER_HIT_SHIELDED)))
                {
                    t->hitEffect = gAttackBox->hitEffect;
                    /* the actor's x is read signed here (ldrsh) */
                    if ((s16)gAttackX < gColliderX)
                        t->hitDirection = 0;
                    else
                        t->hitDirection = 4;
                    gColliderPlayerState->hitsThisFrame++;
                    AddPlayerHealth(-gAttackBox->damage, gColliderPlayer);
                    if (gPlayerHealth[gColliderPlayer] <= 0)
                    {
                        t->hitKind = HIT_KIND_DEFEAT;
                        t->hitEffect = gAttackBox->attackFlags & 0x300;
                    }
                    else
                    {
                        t->hitKind = HIT_KIND_DAMAGE;
                    }
                }
                if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_NO_RESULT)
                    continue;
                if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_PLAYERS)
                {
                    gHitKind = HIT_KIND_NO_DAMAGE;
                    gHitEffect = 11;
                    gHitHealthLeft = gAttackHealth;
                }
                else if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_TOUCH)
                {
                    gHitKind = HIT_KIND_NO_DAMAGE;
                    gHitEffect = 1;
                    gHitHealthLeft = gAttackHealth;
                }
                else
                {
                    gHitKind = HIT_KIND_DAMAGE;
                    CalcHitDamageAndDirection();
                }
            }
            else if (s == 1 || s == 3)
            {
                if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_NO_RESULT)
                    continue;
                if (gAttackBox->immunityFlags & ATTACK_BOX_IMMUNITY_PLAYERS)
                {
                    gHitKind = HIT_KIND_NO_DAMAGE;
                    gHitEffect = 11;
                    gHitHealthLeft = gAttackHealth;
                }
                else
                {
                    gHitKind = HIT_KIND_DAMAGE;
                    CalcHitDamageAndDirection();
                }
            }
            HitRecordHitter();
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
            if (!(gColliderBodyBox->guardFlags & 6))
            {
                ps->hitsThisFrame++;
                gHitKind = HIT_KIND_CATCH;
                gHitHealthLeft = gAttackHealth;
                HitRecordHitter();
                return 1;
            }
            gHitKind = HIT_KIND_NO_DAMAGE;
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
        if ((gAttackBox->playerHitMode & 7) == 2)
        {
            gUnk_03001F24 = gColliderPlayer;
            return 1;
        }
        gUnk_03001F24 |= 1 << gColliderPlayer;
    }
    if ((gAttackBox->playerHitMode & 7) == 3 && gHitKind == 7)
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
        gColliderSlot = e->slot;
        t = &gTasks[gColliderSlot];
        gColliderPlayerState = t->player;
        gColliderPlayer = gColliderPlayerState->playerIndex;
        gColliderBodyBox = e->bodyBox;
        if (gColliderPlayer == (s8)gAttackLastHitter)
        {
            e++;
            continue;
        }
        /* the list entry's position is unsigned (ldrh) */
        if (gColliderBodyBox->bodyFlags & BODY_BOX_FLAG_NO_MIRROR)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->offsetX + e->x;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->left;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->right;
        }
        else if (t->facing == 1)
        {
            s32 x;
            gColliderX = x = gColliderBodyBox->offsetX + e->x;
            gColliderLeft = (x - (u16)gViewRect[0]) + gColliderBodyBox->left;
            gColliderRight = (x - (u16)gViewRect[0]) + gColliderBodyBox->right;
        }
        else
        {
            s32 x;
            gColliderX = x = -gColliderBodyBox->offsetX + e->x;
            gColliderLeft = (x - (u16)gViewRect[0]) - gColliderBodyBox->right;
            gColliderRight = (x - (u16)gViewRect[0]) - gColliderBodyBox->left;
        }
        gColliderY = gColliderBodyBox->offsetY + e->y;
        e++;
        gColliderTop = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->top;
        gColliderBottom = (gColliderY - (u16)gViewRect[2]) + gColliderBodyBox->bottom;
        if (gColliderRight < gAttackBoxLeft)
            continue;
        if (gAttackBoxRight < gColliderLeft)
            continue;
        if (gColliderBottom < gAttackBoxTop)
            continue;
        if (gAttackBoxBottom < gColliderTop)
            continue;
        if (gAttackBox->attackFlags & 0x3E)
        {
            if (!((gAttackBox->attackFlags >> gColliderBodyBox->hitEffect) & 1))
                continue;
            gHitKind = 7;
            gHitHealthLeft = gAttackHealth;
            HitRecordHitter();
            return 1;
        }
        k = (u32)(gColliderBodyBox->classKind << 28) >> 28;
        mask = gColliderClass10KindBits[k] | 0x4000;
        /* the body box's halfword at 0x0E (ldrh) */
        if (!(gColliderBodyBox->guardFlags & BODY_BOX_GUARD_NO_DAMAGE) && !(mask & gAttackBox->class10HarmlessMask))
        {
            t->hitEffect = gAttackBox->hitEffect;
            t->health -= gAttackBox->damage;
            if (t->health <= 0)
                t->hitKind = HIT_KIND_DEFEAT;
            else
                t->hitKind = HIT_KIND_DAMAGE;
            u = &gTasks[gColliderPlayer];
            if (!(gAttackBox->attackFlags & ATTACK_BOX_FLAG_NO_HIT_REACTION))
            {
                u->u76.unk76 &= PLAYER_HIT_SHIELDED;
                u->u76.unk76 |= gColliderBodyBox->bodyFlags & ~(BODY_BOX_FLAG_SHIELD | BODY_BOX_FLAG_NO_MIRROR);
            }
        }
        if (!(gAttackBox->class10ImmuneMask & 0x8000))
        {
            mask = gColliderClass10KindBits[k];
            if (!(mask & gAttackBox->class10ImmuneMask))
            {
                gHitKind = HIT_KIND_DAMAGE;
                CalcHitDamageAndDirection();
            }
            else
            {
                gHitKind = HIT_KIND_NO_DAMAGE;
                gHitEffect = gUnk_08732218[k];
                gHitHealthLeft = gAttackHealth;
            }
            HitRecordHitter();
            return 1;
        }
    }
    return 0;
}
