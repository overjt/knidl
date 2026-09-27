#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hitbox_1b7dc.c (0x0801B7DC-0x0801BAA3, issue #84).
 *
 * The pieces the actor-vs-collider hit tests of src/hitbox_1a8c8.c share:
 * sub_0801b7dc places the actor's attack box gAttackBox (struct
 * AttackBox) at the actor's position gAttackX/gAttackY, mirrored
 * when the actor faces left, relative to the camera rectangle
 * gViewRect[]; sub_0801b8e4 computes a hit's damage (gAttackPower
 * minus the body box's defence) and knock-back direction (one of eight, from
 * ArcTan2 between the collider and the actor); sub_0801b9e4 copies the hit's
 * details out for the actor code. */

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

/* Actor-vs-player hit test cells (M17's src/actor_673ec.c widths). */
extern u8 gUnk_03001F24;
extern s8 gAttackHitDuration;
extern u8 gHitDirection;
extern u16 gAttackY;           /* actor y */
extern s16 gViewRect[];         /* camera rectangle: left, right, top, bottom */
extern u8 gUnk_03002354;
extern u16 gAttackX;           /* actor x */
extern u16 gAttackPower;
extern struct AttackBox *gAttackBox; /* the actor's attack box (s32 in actor_673ec.c) */
extern u8 gHitKind;            /* hit result */
extern u16 gHitDamage;
extern s16 gAttackFacing;
extern u8 gUnk_030023A4;
extern u8 gUnk_030023D0;
extern u8 gUnk_030023DC;
extern u8 gUnk_03002450;
extern s16 gAttackBoxBottom;           /* attack box bottom */
extern s16 gAttackBoxTop;           /* attack box top */
extern s16 gAttackBoxRight;           /* attack box right */
extern s16 gAttackBoxLeft;           /* attack box left */
extern struct BodyBox *gColliderBodyBox;   /* the current entry's body box */
extern u8 gColliderPlayer;            /* the current entry's player index */
extern u8 gColliderSlot;            /* the current entry's task index */
extern s16 gColliderX;           /* the current entry's x */
extern s16 gColliderY;           /* the current entry's y */
extern s16 gUnk_03001F04;
extern s16 gUnk_03002148;

/* Place the actor's attack box: move the actor position by the box's
   offset (mirrored when the actor faces left) and store the box edges
   relative to the camera rectangle. */
void sub_0801b7dc(void)
{
    s32 y;

    if (gAttackBox->unk1A & 0x8000)
    {
        s32 x;
        gAttackX = x = gAttackBox->unk00 + gAttackX;
        gAttackBoxLeft = (x - (u16)gViewRect[0]) + gAttackBox->unk02;
        gAttackBoxRight = (x - (u16)gViewRect[0]) + gAttackBox->unk04;
    }
    else if (gAttackFacing == 1)
    {
        s32 x;
        gAttackX = x = gAttackBox->unk00 + gAttackX;
        gAttackBoxLeft = (x - (u16)gViewRect[0]) + gAttackBox->unk02;
        gAttackBoxRight = (x - (u16)gViewRect[0]) + gAttackBox->unk04;
    }
    else
    {
        s32 x;
        gAttackX = x = -gAttackBox->unk00 + gAttackX;
        gAttackBoxLeft = (x - (u16)gViewRect[0]) - gAttackBox->unk04;
        gAttackBoxRight = (x - (u16)gViewRect[0]) - gAttackBox->unk02;
    }
    y = gAttackY + gAttackBox->unk01;
    gAttackY = y;
    gAttackBoxTop = (y - (u16)gViewRect[2]) + gAttackBox->unk03;
    gAttackBoxBottom = (y - (u16)gViewRect[2]) + gAttackBox->unk05;
}

/* Shared tail of the hit tests: the damage left after the body box's
   defence, the hit kind and the knock-back direction (one of eight, from
   the angle between the entry and the actor). */
void sub_0801b8e4(void)
{
    gHitDamage = gAttackPower - gColliderBodyBox->unk0C;
    if ((s16)gHitDamage <= 0)
    {
        if (gAttackBox->unk0A & 2)
        {
            gHitKind = 6;
            gUnk_03002450 = 1;
            gHitDamage = gAttackPower;
            gHitDirection = ((((u16)ArcTan2(gAttackX - gColliderX, gAttackY - gColliderY) >> 7) + 32) >> 6) & 7;
            return;
        }
        gHitKind = 1;
        gHitDamage = 0;
    }
    gHitDirection = ((((u16)ArcTan2(gAttackX - gColliderX, gAttackY - gColliderY) >> 7) + 32) >> 6) & 7;
    gUnk_03002450 = gColliderBodyBox->unk0D;
}

/* Shared tail of the hit tests: copy the hit's details out - the body
   box's flags, the entry's task and player index, the hit's duration and
   the midpoint between the entry and the actor. */
void sub_0801b9e4(void)
{
    gUnk_030023A4 = gColliderBodyBox->unk08 & 0xF0;
    gUnk_030023D0 = (u32)(gColliderBodyBox->unk08 << 28) >> 28;
    gUnk_030023DC = gColliderSlot;
    gUnk_03001F24 = gColliderPlayer;
    if (gHitKind == 6 || gHitKind == 8)
        gUnk_03002354 = gCurTask->unk75;
    else
        gUnk_03002354 = gAttackHitDuration;
    /* gAttackX/gAttackY are read signed here (ldrsh) */
    gUnk_03001F04 = (gColliderX + (s16)gAttackX) >> 1;
    gUnk_03002148 = (gColliderY + (s16)gAttackY) >> 1;
}
