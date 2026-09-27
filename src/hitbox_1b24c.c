#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hitbox_1b24c.c (0x0801B24C-0x0801B7DB, issue #84).
 *
 * The third actor-vs-collider hit test (the first two, sub_0801a8c8 and
 * sub_0801af14, are src/hitbox_1a8c8.c; the shared tails sub_0801b8e4 and
 * sub_0801b9e4 are src/hitbox_1b7dc.c).  sub_0801b24c walks the third
 * collider list gUnk_030052A0 (gUnk_030054F4 entries) that M05's
 * sub_0801a828 fills, places each entry's body box (mirrored by its task's
 * facing, unk43) against the camera rectangle, tests it against the actor's
 * attack box and, on an overlap, sorts the hit by the body box's kind k
 * (low nibble of unk08) through the per-kind mask tables gUnk_08732254,
 * gUnk_08732278 and gUnk_0873229C against the attack's flags: it may mark
 * the collider's task (unk7C = 6/7) or its player (unk76), writes the hit
 * result gUnk_03002380 (3/4 with a knock-back direction from ArcTan2 into
 * gUnk_03002144, 6 or 7, or gUnk_08732230[k] with the damage of
 * sub_0801b8e4) and returns 1; it returns 0 when nothing is hit.
 *
 * Matching notes (#84's final campaign, lesson 3.492): parked by #84 at 44
 * differing bytes; the player index is read from gUnk_03005394 at every
 * test and as u's index (no `p` local, which made cse1 and cse2 disagree on
 * the canonical register, lesson 3.478), and every table mask goes through
 * the one `u32 m`. */

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

struct HitEntry
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 filler01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ u16 filler06;
    /*0x08*/ struct BodyBox *unk08;
};

extern u8 gUnk_03002144;
extern u16 gUnk_0300214C;           /* actor y */
extern s16 gUnk_03002158[];         /* camera rectangle: left, right, top, bottom */
extern u16 gUnk_03002358;           /* actor x */
extern u16 gUnk_03002368;
extern struct AttackBox *gUnk_0300236C;
extern u8 gUnk_03002380;            /* hit result */
extern u8 gUnk_03002390;
extern u16 gUnk_03002394;
extern u8 gUnk_03002450;
extern u8 gUnk_03002460;
extern s16 gUnk_03005294;           /* attack box bottom */
extern s16 gUnk_030054E0;           /* attack box top */
extern s16 gUnk_03005490;           /* attack box right */
extern s16 gUnk_030054A4;           /* attack box left */
extern struct BodyBox *gUnk_030054E8;
extern struct PlayerState *gUnk_030054F0;
extern u8 gUnk_03005394;
extern u8 gUnk_03005498;
extern s16 gUnk_0300549C;
extern s16 gUnk_030054A0;
extern s16 gUnk_030054EC;
extern s16 gUnk_030054E4;
extern s16 gUnk_03005390;
extern s16 gUnk_03005494;
extern struct HitEntry gUnk_030052A0[];
extern u8 gUnk_030054F4;
extern u16 gUnk_08732230[];
extern u16 gUnk_08732242[];
extern u32 gUnk_08732254[];
extern u32 gUnk_08732278[];
extern u32 gUnk_0873229C[];

void sub_0801b8e4(void);
void sub_0801b9e4(void);

/* Hit test of the actor's attack box against the third collider list.
   There is no local for the player index: cse gives the byte load of
   gUnk_03005394 one pseudo (the ROM's r7) that every test reuses, and u's
   index a copy (`adds r1, r7, #0`).  All four table masks go through the
   one `u32 m`, which does not tie to the table address (`ldr r2, [r0];
   ands r3, r2`). */
u8 sub_0801b24c(void)
{
    s32 i;
    struct HitEntry *e;
    struct Task *t;
    struct Task *u;
    struct BodyBox *b;
    struct AttackBox *a;
    s32 k;
    u32 m;

    e = gUnk_030052A0;
    for (i = 0; i < gUnk_030054F4; i++)
    {
        gUnk_03005498 = e->unk00;
        t = &gUnk_03002790[gUnk_03005498];
        gUnk_030054F0 = t->unk88;
        gUnk_030054E8 = e->unk08;
        if (gUnk_030054F0 == NULL)
            gUnk_03005394 = 4;
        else
            gUnk_03005394 = gUnk_030054F0->unk00;
        if (gUnk_030054E8->unk10 & 0x8000)
        {
            s32 x;
            gUnk_0300549C = x = gUnk_030054E8->unk00 + e->unk02;
            gUnk_030054EC = (x - (u16)gUnk_03002158[0]) + gUnk_030054E8->unk02;
            gUnk_030054E4 = (x - (u16)gUnk_03002158[0]) + gUnk_030054E8->unk04;
        }
        else if (t->unk43 == 1)
        {
            s32 x;
            gUnk_0300549C = x = gUnk_030054E8->unk00 + e->unk02;
            gUnk_030054EC = (x - (u16)gUnk_03002158[0]) + gUnk_030054E8->unk02;
            gUnk_030054E4 = (x - (u16)gUnk_03002158[0]) + gUnk_030054E8->unk04;
        }
        else
        {
            s32 x;
            gUnk_0300549C = x = -gUnk_030054E8->unk00 + e->unk02;
            gUnk_030054EC = (x - (u16)gUnk_03002158[0]) - gUnk_030054E8->unk04;
            gUnk_030054E4 = (x - (u16)gUnk_03002158[0]) - gUnk_030054E8->unk02;
        }
        gUnk_030054A0 = gUnk_030054E8->unk01 + e->unk04;
        e++;
        gUnk_03005390 = (gUnk_030054A0 - (u16)gUnk_03002158[2]) + gUnk_030054E8->unk03;
        gUnk_03005494 = (gUnk_030054A0 - (u16)gUnk_03002158[2]) + gUnk_030054E8->unk05;
        if (gUnk_030054E4 < gUnk_030054A4)
            continue;
        if (gUnk_03005490 < gUnk_030054EC)
            continue;
        if (gUnk_03005494 < gUnk_030054E0)
            continue;
        if (gUnk_03005294 < gUnk_03005390)
            continue;
        if (gUnk_03005394 != 4)
            u = &gUnk_03002790[gUnk_03005394];
        b = gUnk_030054E8;
        k = (u32)(b->unk08 << 28) >> 28;
        a = gUnk_0300236C;
        if ((s32)a->unk14 < 0)
        {
            if (gUnk_03005498 == (s8)gUnk_03002390 && (s8)gUnk_03002460 == 32)
                continue;
            m = gUnk_08732278[k] | 0x4000;
            if (!(a->unk18 & m) && !(b->unk10 & 8))
                t->unk7C = 6;
            if (k == 4)
                continue;
            if (k == 5 && (gUnk_0300236C->unk1A & 0x80))
                continue;
            if (gUnk_0300236C->unk1A & 1)
                continue;
            if (gUnk_03005394 == 4)
                continue;
            u->unk76 &= 0x4000;
            u->unk76 |= gUnk_030054E8->unk10 & 0x3FFF;
            continue;
        }
        m = gUnk_08732254[k];
        if (!(a->unk14 & m))
        {
            if (gUnk_03005498 == (s8)gUnk_03002390 && (s8)gUnk_03002460 == 32)
                continue;
            if ((a->unk1A & 0x40) && (b->unk10 & 0x4000))
            {
                if (gUnk_03005394 == 4)
                    continue;
                u->unk76 |= 0x4000;
                continue;
            }
            if (gUnk_0300236C->unk1A & 0x3E)
            {
                if (!((gUnk_0300236C->unk1A >> gUnk_030054E8->unk0D) & 1))
                    continue;
                gUnk_03002380 = 7;
                gUnk_03002394 = gUnk_03002368;
                sub_0801b9e4();
                return 1;
            }
            if (!(gUnk_0300236C->unk1A & 1) && !(k == 5 && (gUnk_0300236C->unk1A & 0x80))
                && gUnk_03005394 != 4)
            {
                u->unk76 &= 0x4000;
                u->unk76 |= gUnk_030054E8->unk10 & 0x3FFF;
            }
            switch (k)
            {
            case 1:
                gUnk_03002380 = 3;
                gUnk_03002450 = k;
                gUnk_03002394 = gUnk_03002368;
                gUnk_03002144 = ((((u16)ArcTan2(gUnk_03002358 - gUnk_0300549C, gUnk_0300214C - gUnk_030054A0) >> 7) + 32) >> 6) & 7;
                break;
            case 2:
            case 3:
                gUnk_03002380 = 4;
                gUnk_03002450 = k;
                gUnk_03002394 = gUnk_03002368;
                gUnk_03002144 = ((((u16)ArcTan2(gUnk_03002358 - gUnk_0300549C, gUnk_0300214C - gUnk_030054A0) >> 7) + 32) >> 6) & 7;
                break;
            default:
                if (!(gUnk_030054E8->unk10 & 0x10))
                    t->unk7C = 7;
                gUnk_03002380 = gUnk_08732230[k];
                sub_0801b8e4();
                break;
            }
            sub_0801b9e4();
            return 1;
        }
        if (!(a->unk1A & 1) && !(k == 5 && (a->unk1A & 0x80)) && gUnk_03005394 != 4)
        {
            u->unk76 &= 0x4000;
            u->unk76 |= b->unk10 & 0x3FFF;
        }
        if (gUnk_03005498 != (s8)gUnk_03002390 && (s8)gUnk_03002460 != 32)
        {
            m = gUnk_08732278[k] | 0x4000;
            if (!(gUnk_0300236C->unk18 & m) && !(gUnk_030054E8->unk10 & 8))
                t->unk7C = 6;
        }
        m = gUnk_0873229C[k];
        if (gUnk_0300236C->unk14 & m)
            continue;
        gUnk_03002380 = 6;
        gUnk_03002450 = gUnk_08732242[k];
        gUnk_03002394 = gUnk_03002368;
        sub_0801b9e4();
        return 1;
    }
    return 0;
}
