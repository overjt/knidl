#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hitbox_1a8c8.c (0x0801A8C8-0x0801B24B, issue #84).
 *
 * The actor-vs-collider hit tests M17/M18's actors run (src/actor_673ec.c
 * calls them after sub_0801b7dc has placed the actor's attack box): each walks
 * one of the collider lists M05's sub_0801a828 fills, places the collider's
 * body box the same way, tests the overlap and, on a hit, writes the hit
 * result (gUnk_03002380 = hit kind, gUnk_03002450, gUnk_03002394) through
 * the shared tails sub_0801b8e4/sub_0801b9e4 and returns 1.
 * sub_0801a8c8 tests the players' list gUnk_030054B0 (by the attack's class
 * gUnk_0300236C->unk06 & 7: 0 a damaging hit, 1 a hit the body box can
 * block, 2/3 a touch that marks the player in gUnk_03001F24);
 * sub_0801af14 tests the second list gUnk_030053A0.  The third list's test,
 * sub_0801b24c (0x0801B24C-0x0801B7DC), is still asm.
 * 
 * Matching notes: the class 2/3 case is the last case in the source, because
 * merge_blocks moves its head up behind the dispatch; hit paths end at an
 * in-loop `sub_0801b9e4(); return 1;` (loop.c moves lone exit blocks out of
 * the loop); the attack's flags are read from gUnk_0300236C at every test. */

/* An actor's attack box (ROM), pointed to by gUnk_0300236C during the
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
   gUnk_030054B0 and cached in gUnk_030054E8: the same six signed offsets,
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
   filled by M05's sub_0801a828): the owner's task index, its position and
   its body box.  The three lists are gUnk_030054B0[gUnk_03005290] (up to 4,
   the players), gUnk_030053A0[gUnk_030054A8] and
   gUnk_030052A0[gUnk_030054F4] (up to 20 each), picked by the high nibble
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
extern u8 gUnk_03001F24;
extern s8 gUnk_03002140;
extern s16 gUnk_03002158[];         /* camera rectangle: left, right, top, bottom */
extern u8 gUnk_03002354;
extern u16 gUnk_03002358;           /* actor x */
extern u16 gUnk_03002368;
extern struct AttackBox *gUnk_0300236C; /* the actor's attack box (s32 in actor_673ec.c) */
extern u8 gUnk_03002380;            /* hit result */
extern u8 gUnk_03002390;
extern u16 gUnk_03002394;
extern u8 gUnk_030023A4;
extern u8 gUnk_030023DC;
extern u8 gUnk_03002440;
extern u8 gUnk_03002450;
extern u8 gUnk_03002460;
extern s16 gUnk_03005294;           /* attack box bottom */
extern s16 gUnk_030054E0;           /* attack box top */
extern s16 gUnk_03005490;           /* attack box right */
extern s16 gUnk_030054A4;           /* attack box left */
extern u8 gUnk_03005290;            /* number of hit-list entries */
extern struct HitEntry gUnk_030054B0[];
extern struct BodyBox *gUnk_030054E8;   /* the current entry's body box */
extern struct PlayerState *gUnk_030054F0; /* the current entry's player */
extern u8 gUnk_03005394;            /* the current entry's player index */
extern u8 gUnk_03005498;            /* the current entry's task index */
extern s16 gUnk_0300549C;           /* the current entry's x */
extern s16 gUnk_030054A0;           /* the current entry's y */
extern s16 gUnk_030054EC;           /* body box left */
extern s16 gUnk_030054E4;           /* body box right */
extern s16 gUnk_03005390;           /* body box top */
extern s16 gUnk_03005494;           /* body box bottom */
extern s16 gUnk_02005588[];
extern struct HitEntry gUnk_030053A0[];
extern u8 gUnk_030054A8;
extern u16 gUnk_08732218[];
extern u16 gUnk_08732224[];

s32 sub_08009ee8(s32 a, u32 b);
void sub_0801b8e4(void);
void sub_0801b9e4(void);

/* Hit test of the actor's attack box against the players' hit list
   gUnk_030054B0.  By the attack's class (unk06 & 7): 0 = a damaging hit
   (hit kind 2/6, health and knock-back through sub_0801b8e4), 1 = hit kind
   8 unless the body box blocks it, 2/3 = a touch (kind 7) that marks the
   player in gUnk_03001F24 (class 2 returns at the first one, class 3 after
   the list).  Class 2/3 comes last in the switch: merge_blocks moves its
   head up behind the dispatch, which is why its tail sits after class 1. */
u8 sub_0801a8c8(void)
{
    struct HitEntry *e;
    struct Task *t;
    struct PlayerState *ps;
    s32 i;
    u8 s;

    gUnk_03002380 = 0;
    gUnk_03001F24 = 0;
    e = gUnk_030054B0;
    for (i = 0; i < gUnk_03005290; i++)
    {
        gUnk_03005498 = e->unk00;
        /* gUnk_03002460 is compared signed (lsls/asrs) */
        if (gUnk_03005498 == (s8)gUnk_03002390 && *(s8 *)&gUnk_03002460 == 0)
        {
            e++;
            continue;
        }
        t = &gUnk_03002790[gUnk_03005498];
        gUnk_030054F0 = t->unk88;
        if (gUnk_030054F0->unk45 != 0)
        {
            e++;
            continue;
        }
        gUnk_03005394 = gUnk_030054F0->unk00;
        gUnk_030054E8 = e->unk08;
        /* the list entry's position is unsigned (ldrh) */
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
        switch (gUnk_0300236C->unk06 & 7)
        {
        case 0:
            ps = gUnk_030054F0;
            s = ps->unk3F;
            if (s == 2)
                continue;
            if (gUnk_030054E4 < gUnk_030054A4)
                continue;
            if (gUnk_03005490 < gUnk_030054EC)
                continue;
            if (gUnk_03005494 < gUnk_030054E0)
                continue;
            if (gUnk_03005294 < gUnk_03005390)
                continue;
            if (ps->unk17 == 1)
            {
                if (gUnk_0300236C->unk0A & 0x8000)
                    continue;
                if (gUnk_0300236C->unk0A & 8)
                {
                    gUnk_03002380 = 6;
                    gUnk_03002450 = 12;
                    gUnk_03002394 = gUnk_03002368;
                }
                else if (gUnk_0300236C->unk0A & 4)
                {
                    gUnk_03002380 = 6;
                    gUnk_03002450 = 11;
                    gUnk_03002394 = gUnk_03002368;
                }
                else
                {
                    gUnk_03002380 = 2;
                    sub_0801b8e4();
                    t->unk76 = (t->unk76 & 0x4000) | 1;
                }
            }
            else if (s == 0)
            {
                if (!(gUnk_0300236C->unk0C & 0x4005)
                    && !((gUnk_0300236C->unk1A & 0x40) && (t->unk76 & 0x4000)))
                {
                    t->unk82 = gUnk_0300236C->unk09;
                    /* the actor's x is read signed here (ldrsh) */
                    if ((s16)gUnk_03002358 < gUnk_0300549C)
                        t->unk7D = 0;
                    else
                        t->unk7D = 4;
                    gUnk_030054F0->unk45++;
                    sub_08009ee8(-gUnk_0300236C->unk08, gUnk_03005394);
                    if (gUnk_02005588[gUnk_03005394] <= 0)
                    {
                        t->unk7C = 1;
                        t->unk82 = gUnk_0300236C->unk1A & 0x300;
                    }
                    else
                    {
                        t->unk7C = 2;
                    }
                }
                if (gUnk_0300236C->unk0A & 0x8000)
                    continue;
                if (gUnk_0300236C->unk0A & 4)
                {
                    gUnk_03002380 = 6;
                    gUnk_03002450 = 11;
                    gUnk_03002394 = gUnk_03002368;
                }
                else if (gUnk_0300236C->unk0A & 1)
                {
                    gUnk_03002380 = 6;
                    gUnk_03002450 = 1;
                    gUnk_03002394 = gUnk_03002368;
                }
                else
                {
                    gUnk_03002380 = 2;
                    sub_0801b8e4();
                }
            }
            else if (s == 1 || s == 3)
            {
                if (gUnk_0300236C->unk0A & 0x8000)
                    continue;
                if (gUnk_0300236C->unk0A & 4)
                {
                    gUnk_03002380 = 6;
                    gUnk_03002450 = 11;
                    gUnk_03002394 = gUnk_03002368;
                }
                else
                {
                    gUnk_03002380 = 2;
                    sub_0801b8e4();
                }
            }
            sub_0801b9e4();
            return 1;
        case 1:
            ps = gUnk_030054F0;
            if (ps->unk3F >= 1 && ps->unk3F <= 3)
                continue;
            if (gUnk_030054E4 < gUnk_030054A4)
                continue;
            if (gUnk_03005490 < gUnk_030054EC)
                continue;
            if (gUnk_03005494 < gUnk_030054E0)
                continue;
            if (gUnk_03005294 < gUnk_03005390)
                continue;
            gUnk_03002450 = 0;
            /* the body box's halfword at 0x0E (ldrh) */
            if (!(gUnk_030054E8->unk0E & 6))
            {
                ps->unk45++;
                gUnk_03002380 = 8;
                gUnk_03002394 = gUnk_03002368;
                sub_0801b9e4();
                return 1;
            }
            gUnk_03002380 = 6;
            continue;
        default:
            continue;
        case 2:
        case 3:
            if (gUnk_030054F0->unk3F == 2)
                continue;
            if (gUnk_030054E4 < gUnk_030054A4)
                continue;
            break;
        }
        if (gUnk_03005490 < gUnk_030054EC)
            continue;
        if (gUnk_03005494 < gUnk_030054E0)
            continue;
        if (gUnk_03005294 < gUnk_03005390)
            continue;
        gUnk_03002380 = 7;
        gUnk_030023A4 = 0;
        gUnk_03002394 = gUnk_03002368;
        gUnk_030023DC = gUnk_03005498;
        gUnk_03002354 = gUnk_03002140;
        if ((gUnk_0300236C->unk06 & 7) == 2)
        {
            gUnk_03001F24 = gUnk_03005394;
            return 1;
        }
        gUnk_03001F24 |= 1 << gUnk_03005394;
    }
    if ((gUnk_0300236C->unk06 & 7) == 3 && gUnk_03002380 == 7)
        return 1;
    return 0;
}

/* Hit test of the actor's attack box against the second hit list. */
u8 sub_0801af14(void)
{
    struct HitEntry *e;
    struct Task *t;
    struct Task *u;
    s32 i;
    u32 k;
    u16 mask;

    e = gUnk_030053A0;
    for (i = 0; i < gUnk_030054A8; i++)
    {
        gUnk_03005498 = e->unk00;
        t = &gUnk_03002790[gUnk_03005498];
        gUnk_030054F0 = t->unk88;
        gUnk_03005394 = gUnk_030054F0->unk00;
        gUnk_030054E8 = e->unk08;
        if (gUnk_03005394 == (s8)gUnk_03002440)
        {
            e++;
            continue;
        }
        /* the list entry's position is unsigned (ldrh) */
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
        if (gUnk_0300236C->unk1A & 0x3E)
        {
            if (!((gUnk_0300236C->unk1A >> gUnk_030054E8->unk0D) & 1))
                continue;
            gUnk_03002380 = 7;
            gUnk_03002394 = gUnk_03002368;
            sub_0801b9e4();
            return 1;
        }
        k = (u32)(gUnk_030054E8->unk08 << 28) >> 28;
        mask = gUnk_08732224[k] | 0x4000;
        /* the body box's halfword at 0x0E (ldrh) */
        if (!(gUnk_030054E8->unk0E & 0x8000) && !(mask & gUnk_0300236C->unk10))
        {
            t->unk82 = gUnk_0300236C->unk09;
            t->unk78 -= gUnk_0300236C->unk08;
            if (t->unk78 <= 0)
                t->unk7C = 1;
            else
                t->unk7C = 2;
            u = &gUnk_03002790[gUnk_03005394];
            if (!(gUnk_0300236C->unk1A & 1))
            {
                u->unk76 &= 0x4000;
                u->unk76 |= gUnk_030054E8->unk10 & 0x3FFF;
            }
        }
        if (!(gUnk_0300236C->unk0E & 0x8000))
        {
            mask = gUnk_08732224[k];
            if (!(mask & gUnk_0300236C->unk0E))
            {
                gUnk_03002380 = 2;
                sub_0801b8e4();
            }
            else
            {
                gUnk_03002380 = 6;
                gUnk_03002450 = gUnk_08732218[k];
                gUnk_03002394 = gUnk_03002368;
            }
            sub_0801b9e4();
            return 1;
        }
    }
    return 0;
}
