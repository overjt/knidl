#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "camera.h"

/* hitbox_1b24c.c (0x0801B24C-0x0801B7DB, issue #84).
 *
 * The third actor-vs-collider hit test (the first two, HitTestPlayerColliders and
 * HitTestColliderClass10, are src/hitbox_1a8c8.c; the shared tails CalcHitDamageAndDirection and
 * HitRecordHitter are src/hitbox_1b7dc.c).  HitTestColliderClass20 walks the third
 * collider list gColliderClass20 (gColliderClass20Count entries) that M05's
 * RegisterCollider fills, places each entry's body box (mirrored by its task's
 * facing, unk43) against the camera rectangle, tests it against the actor's
 * attack box and, on an overlap, sorts the hit by the body box's kind k
 * (low nibble of unk08) through the per-kind mask tables gUnk_08732254,
 * gColliderClass20KindBits and gUnk_0873229C against the attack's flags: it may mark
 * the collider's task (unk7C = 6/7) or its player (unk76), writes the hit
 * result gHitKind (3/4 with a knock-back direction from ArcTan2 into
 * gHitDirection, 6 or 7, or gColliderClass20KindHitKinds[k] with the damage of
 * CalcHitDamageAndDirection) and returns 1; it returns 0 when nothing is hit.
 *
 * Matching notes (#84's final campaign, lesson 3.492): parked by #84 at 44
 * differing bytes; the player index is read from gColliderPlayer at every
 * test and as u's index (no `p` local, which made cse1 and cse2 disagree on
 * the canonical register, lesson 3.478), and every table mask goes through
 * the one `u32 m`. */

struct AttackBox
{
    /*0x00*/ s8 offsetX;
    /*0x01*/ s8 offsetY;
    /*0x02*/ s8 left;
    /*0x03*/ s8 top;
    /*0x04*/ s8 right;
    /*0x05*/ s8 bottom;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 damage;
    /*0x09*/ u8 hitEffect;
    /*0x0A*/ u16 unk0A;
    /*0x0C*/ u16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12;
    /*0x14*/ u32 unk14;
    /*0x18*/ u16 unk18;
    /*0x1A*/ u16 unk1A;
};

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
    /*0x08*/ u8 unk08;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ u8 damage;
    /*0x0D*/ u8 hitEffect;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u16 unk10;
};

struct HitEntry
{
    /*0x00*/ u8 slot;
    /*0x01*/ u8 filler01;
    /*0x02*/ u16 x;
    /*0x04*/ u16 y;
    /*0x06*/ u16 filler06;
    /*0x08*/ struct BodyBox *bodyBox;
};

/* Not from collision.h: this file's view of gColliderClass20 differs (lesson
   3.517). */
extern u8 gHitDirection;
extern u16 gAttackY;           /* actor y */
extern u16 gAttackX;           /* actor x */
extern u16 gAttackHealth;
extern struct AttackBox *gAttackBox;
extern u8 gHitKind;            /* hit result */
extern u8 gAttackLastHitterSlot;
extern u16 gHitHealthLeft;
extern u8 gHitEffect;
extern u8 gAttackLastHitterClass;
extern s16 gAttackBoxBottom;           /* attack box bottom */
extern s16 gAttackBoxTop;           /* attack box top */
extern s16 gAttackBoxRight;           /* attack box right */
extern s16 gAttackBoxLeft;           /* attack box left */
extern struct BodyBox *gColliderBodyBox;
extern struct PlayerState *gColliderPlayerState;
extern u8 gColliderPlayer;
extern u8 gColliderSlot;
extern s16 gColliderX;
extern s16 gColliderY;
extern s16 gColliderLeft;
extern s16 gColliderRight;
extern s16 gColliderTop;
extern s16 gColliderBottom;
extern struct HitEntry gColliderClass20[];
extern u8 gColliderClass20Count;
extern u16 gColliderClass20KindHitKinds[];
extern u16 gUnk_08732242[];
extern u32 gUnk_08732254[];
extern u32 gColliderClass20KindBits[];
extern u32 gUnk_0873229C[];

void CalcHitDamageAndDirection(void);
void HitRecordHitter(void);

/* Hit test of the actor's attack box against the third collider list.
   There is no local for the player index: cse gives the byte load of
   gColliderPlayer one pseudo (the ROM's r7) that every test reuses, and u's
   index a copy (`adds r1, r7, #0`).  All four table masks go through the
   one `u32 m`, which does not tie to the table address (`ldr r2, [r0];
   ands r3, r2`). */
u8 HitTestColliderClass20(void)
{
    s32 i;
    struct HitEntry *e;
    struct Task *t;
    struct Task *u;
    struct BodyBox *b;
    struct AttackBox *a;
    s32 k;
    u32 m;

    e = gColliderClass20;
    for (i = 0; i < gColliderClass20Count; i++)
    {
        gColliderSlot = e->slot;
        t = &gTasks[gColliderSlot];
        gColliderPlayerState = t->player;
        gColliderBodyBox = e->bodyBox;
        if (gColliderPlayerState == NULL)
            gColliderPlayer = 4;
        else
            gColliderPlayer = gColliderPlayerState->playerIndex;
        if (gColliderBodyBox->unk10 & 0x8000)
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
        if (gColliderPlayer != 4)
            u = &gTasks[gColliderPlayer];
        b = gColliderBodyBox;
        k = (u32)(b->unk08 << 28) >> 28;
        a = gAttackBox;
        if ((s32)a->unk14 < 0)
        {
            if (gColliderSlot == (s8)gAttackLastHitterSlot && (s8)gAttackLastHitterClass == 32)
                continue;
            m = gColliderClass20KindBits[k] | 0x4000;
            if (!(a->unk18 & m) && !(b->unk10 & 8))
                t->hitKind = 6;
            if (k == 4)
                continue;
            if (k == 5 && (gAttackBox->unk1A & 0x80))
                continue;
            if (gAttackBox->unk1A & 1)
                continue;
            if (gColliderPlayer == 4)
                continue;
            u->u76.unk76 &= 0x4000;
            u->u76.unk76 |= gColliderBodyBox->unk10 & 0x3FFF;
            continue;
        }
        m = gUnk_08732254[k];
        if (!(a->unk14 & m))
        {
            if (gColliderSlot == (s8)gAttackLastHitterSlot && (s8)gAttackLastHitterClass == 32)
                continue;
            if ((a->unk1A & 0x40) && (b->unk10 & 0x4000))
            {
                if (gColliderPlayer == 4)
                    continue;
                u->u76.unk76 |= 0x4000;
                continue;
            }
            if (gAttackBox->unk1A & 0x3E)
            {
                if (!((gAttackBox->unk1A >> gColliderBodyBox->hitEffect) & 1))
                    continue;
                gHitKind = 7;
                gHitHealthLeft = gAttackHealth;
                HitRecordHitter();
                return 1;
            }
            if (!(gAttackBox->unk1A & 1) && !(k == 5 && (gAttackBox->unk1A & 0x80))
                && gColliderPlayer != 4)
            {
                u->u76.unk76 &= 0x4000;
                u->u76.unk76 |= gColliderBodyBox->unk10 & 0x3FFF;
            }
            switch (k)
            {
            case 1:
                gHitKind = 3;
                gHitEffect = k;
                gHitHealthLeft = gAttackHealth;
                gHitDirection = ((((u16)ArcTan2(gAttackX - gColliderX, gAttackY - gColliderY) >> 7) + 32) >> 6) & 7;
                break;
            case 2:
            case 3:
                gHitKind = 4;
                gHitEffect = k;
                gHitHealthLeft = gAttackHealth;
                gHitDirection = ((((u16)ArcTan2(gAttackX - gColliderX, gAttackY - gColliderY) >> 7) + 32) >> 6) & 7;
                break;
            default:
                if (!(gColliderBodyBox->unk10 & 0x10))
                    t->hitKind = 7;
                gHitKind = gColliderClass20KindHitKinds[k];
                CalcHitDamageAndDirection();
                break;
            }
            HitRecordHitter();
            return 1;
        }
        if (!(a->unk1A & 1) && !(k == 5 && (a->unk1A & 0x80)) && gColliderPlayer != 4)
        {
            u->u76.unk76 &= 0x4000;
            u->u76.unk76 |= b->unk10 & 0x3FFF;
        }
        if (gColliderSlot != (s8)gAttackLastHitterSlot && (s8)gAttackLastHitterClass != 32)
        {
            m = gColliderClass20KindBits[k] | 0x4000;
            if (!(gAttackBox->unk18 & m) && !(gColliderBodyBox->unk10 & 8))
                t->hitKind = 6;
        }
        m = gUnk_0873229C[k];
        if (gAttackBox->unk14 & m)
            continue;
        gHitKind = 6;
        gHitEffect = gUnk_08732242[k];
        gHitHealthLeft = gAttackHealth;
        HitRecordHitter();
        return 1;
    }
    return 0;
}
