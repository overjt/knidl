#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* terrain_1baa4.c (0x0801BAA4-0x0801BCAB, issue #84).
 *
 * The player's per-frame terrain entry point.  M09's player task
 * (src/player_32688.c) calls it with the player's box record; it is the body
 * of src/terrain_1bcac.c's entry points, but the room-relative position comes
 * from PlayerState.unk5E/unk60 and the box offsets from PlayerState.unk70,
 * both wall probes run (in the order the facing gUnk_03005564 picks), and
 * the probes' 8.8 push gUnk_030055A8/gUnk_03005580 is written back to
 * PlayerState.unk54/unk58 on top of gUnk_030055F0/gUnk_03005618. */

/* The probe result block, filled by the terrain probes and mirrored into
   gUnk_03005550 by sub_0801c5c8. */
struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};

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
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern s16 gUnk_0300550C;           /* box left (room-relative) */
extern s32 gUnk_03005514;           /* Task.unk58 */
extern s16 gUnk_03005518;           /* actor x (room-relative) */
extern s16 gUnk_03005520;           /* actor y (room-relative) */
extern struct Unk03005530 gUnk_03005530;
extern struct Unk03005550 gUnk_03005550;
extern s16 gUnk_03005560;           /* probe x */
extern u8 gUnk_03005564;            /* Task.unk43 */
extern s16 gUnk_03005570;           /* probe y */
extern s32 gUnk_03005580;
extern s16 gUnk_03005584;           /* box bottom offset */
extern s16 gUnk_03005590;           /* box right (room-relative) */
extern s32 gUnk_03005598;           /* Task.unk54 */
extern s16 gUnk_030055A4;           /* box top (room-relative) */
extern s32 gUnk_030055A8;
extern s16 gUnk_030055B0;           /* box bottom (room-relative) */
extern s32 gUnk_030055F0;
extern s32 gUnk_03005618;

void sub_0801c51c(const s8 *p);
void sub_0801c5c8(const s8 *p);
void sub_0801c690(void);
void sub_0801c7cc(void);
void sub_0801c8dc(void);
void sub_0801c930(void);
void sub_0801d9c8(void);
void sub_0801dc88(void);
void sub_0801dee8(void);
void sub_0801e178(void);
void sub_08021130(void);
void sub_08021564(void);
void sub_08022650(void);

/* The player's per-frame terrain entry point (M09's player task passes the
   player's box record): the position comes from PlayerState.unk5E/unk60
   and the box offsets from PlayerState.unk70 instead of the task. */
void sub_0801baa4(u32 p)
{
    s8 *box;
    s32 zero;
    struct PlayerState *ps;
    s32 r;

    sub_0801c51c((const s8 *)p);
    gUnk_03005530.unkD = gUnk_03002490->unk88->unk50 << 4;
    zero = 0;
    gUnk_03005518 = gUnk_03002490->unk88->unk5E + (box = (s8 *)gUnk_03002490->unk88->unk70)[0];
    gUnk_03005520 = box[1] + gUnk_03002490->unk88->unk60;
    gUnk_0300550C = gUnk_03005518 + box[4];
    gUnk_03005590 = gUnk_03005518 + box[5];
    gUnk_030055A4 = gUnk_03005520 + box[2];
    gUnk_030055B0 = gUnk_03005520 + box[3];
    gUnk_03005598 = gUnk_03005560 - gUnk_03005518;
    gUnk_03005514 = gUnk_03005570 - gUnk_03005520;
    gUnk_030055A8 = gUnk_03005580 = zero;
    if (gUnk_03005530.unkB & 0x80)
    {
        gUnk_03005530.unkB = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 16) >> 4;
    }
    sub_08022650();
    if (gUnk_03005530.unk6 != 0)
    {
        if ((s8)gUnk_03005564 == 1)
        {
            sub_0801c7cc();
            sub_0801c690();
        }
        else
        {
            sub_0801c690();
            sub_0801c7cc();
        }
        sub_0801c8dc();
        sub_0801c930();
    }
    else
    {
        if ((s8)gUnk_03005564 == 1)
        {
            sub_0801dc88();
            sub_0801d9c8();
        }
        else
        {
            sub_0801d9c8();
            sub_0801dc88();
        }
        sub_0801dee8();
        sub_0801e178();
    }
    sub_08021130();
    sub_08021564();
    ps = gUnk_03002490->unk88;
    if (gUnk_030055A8 & 0x8000)
        r = ((gUnk_030055A8 << 8) | 0xFF000000) + gUnk_030055F0;
    else
        r = (gUnk_030055A8 << 8) + gUnk_030055F0;
    ps->unk54 = r;
    ps = gUnk_03002490->unk88;
    if (gUnk_03005580 & 0x8000)
        r = ((gUnk_03005580 << 8) | 0xFF000000) + gUnk_03005618;
    else
        r = (gUnk_03005580 << 8) + gUnk_03005618;
    ps->unk58 = r;
    gUnk_03005550.unkA = gUnk_03005530.unkA;
    gUnk_03005550.unk6 = gUnk_03005530.unkD & 15;
    sub_0801c5c8((const s8 *)p);
}
