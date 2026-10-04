#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "collision.h"
#include "camera.h"

/* hitbox_1b7dc.c (0x0801B7DC-0x0801BAA3, issue #84).
 *
 * The pieces the actor-vs-collider hit tests of src/hitbox_1a8c8.c share:
 * PlaceAttackBox places the actor's attack box gAttackBox (struct
 * AttackBox) at the actor's position gAttackX/gAttackY, mirrored
 * when the actor faces left, relative to the camera rectangle
 * gViewRect[]; CalcHitDamageAndDirection computes a hit's damage (gAttackHealth
 * minus the body box's defence) and knock-back direction (one of eight, from
 * ArcTan2 between the collider and the actor); HitRecordHitter copies the hit's
 * details out for the actor code. */

/* Place the actor's attack box: move the actor position by the box's
   offset (mirrored when the actor faces left) and store the box edges
   relative to the camera rectangle. */
void PlaceAttackBox(void)
{
    s32 y;

    if (gAttackBox->unk1A & 0x8000)
    {
        s32 x;
        gAttackX = x = gAttackBox->offsetX + gAttackX;
        gAttackBoxLeft = (x - (u16)gViewRect[0]) + gAttackBox->left;
        gAttackBoxRight = (x - (u16)gViewRect[0]) + gAttackBox->right;
    }
    else if (gAttackFacing == 1)
    {
        s32 x;
        gAttackX = x = gAttackBox->offsetX + gAttackX;
        gAttackBoxLeft = (x - (u16)gViewRect[0]) + gAttackBox->left;
        gAttackBoxRight = (x - (u16)gViewRect[0]) + gAttackBox->right;
    }
    else
    {
        s32 x;
        gAttackX = x = -gAttackBox->offsetX + gAttackX;
        gAttackBoxLeft = (x - (u16)gViewRect[0]) - gAttackBox->right;
        gAttackBoxRight = (x - (u16)gViewRect[0]) - gAttackBox->left;
    }
    y = gAttackY + gAttackBox->offsetY;
    gAttackY = y;
    gAttackBoxTop = (y - (u16)gViewRect[2]) + gAttackBox->top;
    gAttackBoxBottom = (y - (u16)gViewRect[2]) + gAttackBox->bottom;
}

/* Shared tail of the hit tests: the damage left after the body box's
   defence, the hit kind and the knock-back direction (one of eight, from
   the angle between the entry and the actor). */
void CalcHitDamageAndDirection(void)
{
    gHitHealthLeft = gAttackHealth - gColliderBodyBox->damage;
    if ((s16)gHitHealthLeft <= 0)
    {
        if (gAttackBox->unk0A & 2)
        {
            gHitKind = HIT_KIND_NO_DAMAGE;
            gHitEffect = 1;
            gHitHealthLeft = gAttackHealth;
            gHitDirection = ((((u16)ArcTan2(gAttackX - gColliderX, gAttackY - gColliderY) >> 7) + 32) >> 6) & 7;
            return;
        }
        gHitKind = HIT_KIND_DEFEAT;
        gHitHealthLeft = 0;
    }
    gHitDirection = ((((u16)ArcTan2(gAttackX - gColliderX, gAttackY - gColliderY) >> 7) + 32) >> 6) & 7;
    gHitEffect = gColliderBodyBox->hitEffect;
}

/* Shared tail of the hit tests: copy the hit's details out - the body
   box's flags, the entry's task and player index, the hit's duration and
   the midpoint between the entry and the actor. */
void HitRecordHitter(void)
{
    gHitterColliderClass = gColliderBodyBox->unk08 & 0xF0;
    gHitterColliderKind = (u32)(gColliderBodyBox->unk08 << 28) >> 28;
    gHitterSlot = gColliderSlot;
    gUnk_03001F24 = gColliderPlayer;
    if (gHitKind == HIT_KIND_NO_DAMAGE || gHitKind == HIT_KIND_CATCH)
        gHitTimer = gCurTask->hitTimer;
    else
        gHitTimer = gAttackHitDuration;
    /* gAttackX/gAttackY are read signed here (ldrsh) */
    gHitMidpointX = (gColliderX + (s16)gAttackX) >> 1;
    gHitMidpointY = (gColliderY + (s16)gAttackY) >> 1;
}
