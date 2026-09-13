#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801BCAC-0x0801C30C.
 *
 * Six per-frame entry points sharing one body: load the actor's terrain box
 * (sub_0801c51c), derive the actor's room-relative position from
 * Task.unk4C/unk50 (16.16 fixed point) and the room origin in
 * Task.unk54/unk58, form the box corners in gUnk_0300550C/gUnk_03005590
 * (x) and gUnk_030055A4/gUnk_030055B0 (y), then dispatch the probe set by
 * the actor's movement direction (the sign of gUnk_03005598) and finish with
 * the room probe sub_0802136c and the write-back sub_0801c5c8.
 *
 * Matching note: the corner sums read gUnk_03005518/gUnk_03005520 back
 * rather than keeping the position in a local - cse folds the re-read into
 * the register just stored and that is the only spelling that keeps the
 * position as the FIRST operand of the narrowed adds (lesson 3.359).
 */


/* ROM pointer tables: one entry per tile set, each pointing at a byte table. */
extern u8 *const gUnk_08734BF0[];
extern u8 *const gUnk_08733BF0[];
extern u8 *const gUnk_08733FF0[];
extern u8 *const gUnk_087343F0[];
extern u8 *const gUnk_087347F0[];
extern s8 *const gUnk_087330F0[];
extern s8 *const gUnk_087328F0[];

/* ROM byte tables indexed by tile set. */
extern u8 gUnk_08732CF0[];
extern u8 gUnk_087337F0[];
extern u8 gUnk_087334F0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_08732FF0[];
extern s8 gUnk_08733AF0[];
extern s8 gUnk_08732EF0[];
extern s8 gUnk_087338F0[];
extern u8 gUnk_08732DF0[];

/* IWRAM room descriptor cells. */
extern u16 gUnk_03005504;
extern s16 gUnk_0300550C;
extern u16 gUnk_03005508;
extern u16 gUnk_03005510;
extern s32 gUnk_03005514;
extern s16 gUnk_03005518;
extern s16 gUnk_03005520;
extern s16 gUnk_0300551C;
extern s16 gUnk_03005560;
extern u8 gUnk_03005564;
extern u16 gUnk_0300556C;
extern s16 gUnk_03005570;
extern u16 gUnk_03005574;
extern u16 gUnk_03005578;
extern s16 gUnk_0300557C;
extern s16 gUnk_03005584;
extern u16 gUnk_03005588;
extern s16 gUnk_03005590;
extern u16 gUnk_03005594;
extern s16 gUnk_0300559C;
extern s8 *gUnk_030055A0;
extern s16 gUnk_030055A4;
extern u16 gUnk_030055AC;
extern s16 gUnk_030055B0;
extern s16 gUnk_030055E4;
extern s32 gUnk_03005580;
extern s32 gUnk_03005598;
extern s32 gUnk_030055A8;
extern s16 gUnk_0300561C;
extern s16 gUnk_03005620;

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};
extern struct MapCell *gUnk_03005660;

extern s16 *gUnk_0300558C;

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
extern struct Unk03005530 gUnk_03005530;

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
extern struct Unk03005550 gUnk_03005550;

s32 sub_080217dc(u32 x, u32 y);
void sub_0801c51c(const s8 *p);
void sub_080207a0(void);
void sub_0801c690(void);
void sub_0801c8dc(void);
void sub_0801d394(void);
void sub_0801dc88(void);
void sub_0801e178(void);
void sub_0801f540(void);
void sub_0801f800(void);
void sub_08022810(void);
void sub_0801fc48(void);
void sub_0801ff84(void);
void sub_0802136c(void);
void sub_080222b0(s16 x, s16 y);
s32 sub_080219b0(u16 a);
s32 sub_080219f0(u16 a);

void sub_0801c51c(const s8 *p);
void sub_0801c5c8(const s8 *p);
void sub_0801c690(void);
void sub_0801c7cc(void);
void sub_0801c8dc(void);
void sub_0801c930(void);
void sub_0801d394(void);
void sub_0801d9c8(void);
void sub_0801dc88(void);
void sub_0801dee8(void);
void sub_0801e178(void);
void sub_0801ecd0(void);
void sub_0801f540(void);
void sub_0801f6b0(void);
void sub_0801f800(void);
void sub_0801f9b8(void);
void sub_0801fc48(void);
void sub_0801fe2c(void);
void sub_0801ff84(void);
void sub_080214e0(void);
void sub_08021564(void);
void sub_0802136c(void);
void sub_08022810(void);

void sub_0801bcac(const s8 *p)
{
    s32 v;

    sub_0801c51c(p);
    gUnk_03005598 = gUnk_03002490->unk54;
    gUnk_03005514 = gUnk_03002490->unk58;
    gUnk_03005518 = ((gUnk_03005560 << 16) + (gUnk_03002490->unk4C & 0xFFFF) - gUnk_03005598) >> 16;
    gUnk_03005520 = ((gUnk_03005570 << 16) + (gUnk_03002490->unk50 & 0xFFFF) - gUnk_03005514) >> 16;
    gUnk_0300550C = gUnk_03005518 + gUnk_0300551C;
    gUnk_03005590 = gUnk_03005518 + gUnk_0300559C;
    gUnk_030055A4 = gUnk_03005520 + gUnk_0300557C;
    gUnk_030055B0 = gUnk_03005520 + gUnk_03005584;
    if (gUnk_03005530.unkB & 0x80)
    {
        gUnk_03005530.unkB = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584) >> 4;
    }
    if (gUnk_03005530.unk6 != 0)
    {
        v = gUnk_03005598;
        if (v != 0)
        {
            if (v < 0)
                sub_0801c7cc();
            else
                sub_0801c690();
        }
        sub_0801c8dc();
        sub_0801d394();
    }
    else
    {
        v = gUnk_03005598;
        if (v != 0)
        {
            if (v < 0)
                sub_0801dc88();
            else
                sub_0801d9c8();
        }
        sub_0801dee8();
        sub_0801ecd0();
    }
    sub_0802136c();
    sub_0801c5c8(p);
}

void sub_0801bde0(const s8 *p)
{
    s32 v;

    sub_0801c51c(p);
    gUnk_03005598 = gUnk_03002490->unk54;
    gUnk_03005514 = gUnk_03002490->unk58;
    gUnk_03005518 = ((gUnk_03005560 << 16) + (gUnk_03002490->unk4C & 0xFFFF) - gUnk_03005598) >> 16;
    gUnk_03005520 = ((gUnk_03005570 << 16) + (gUnk_03002490->unk50 & 0xFFFF) - gUnk_03005514) >> 16;
    gUnk_0300550C = gUnk_03005518 + gUnk_0300551C;
    gUnk_03005590 = gUnk_03005518 + gUnk_0300559C;
    gUnk_030055A4 = gUnk_03005520 + gUnk_0300557C;
    gUnk_030055B0 = gUnk_03005520 + gUnk_03005584;
    if (gUnk_03005530.unkB & 0x80)
    {
        gUnk_03005530.unkB = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584) >> 4;
    }
    sub_08022810();
    if (gUnk_03005530.unk6 != 0)
    {
        v = gUnk_03005598;
        if (v != 0)
        {
            if (v < 0)
                sub_0801c7cc();
            else
                sub_0801c690();
        }
        sub_0801c8dc();
        sub_0801c930();
    }
    else
    {
        v = gUnk_03005598;
        if (v != 0)
        {
            if (v < 0)
                sub_0801dc88();
            else
                sub_0801d9c8();
        }
        sub_0801dee8();
        sub_0801e178();
    }
    sub_0802136c();
    sub_08021564();
    sub_0801c5c8(p);
}

void sub_0801bf1c(const s8 *p)
{
    s32 v;

    sub_0801c51c(p);
    gUnk_03005598 = gUnk_03002490->unk54;
    gUnk_03005514 = gUnk_03002490->unk58;
    gUnk_03005518 = ((gUnk_03005560 << 16) + (gUnk_03002490->unk4C & 0xFFFF) - gUnk_03005598) >> 16;
    gUnk_03005520 = ((gUnk_03005570 << 16) + (gUnk_03002490->unk50 & 0xFFFF) - gUnk_03005514) >> 16;
    gUnk_0300550C = gUnk_03005518 + gUnk_0300551C;
    gUnk_03005590 = gUnk_03005518 + gUnk_0300559C;
    gUnk_030055A4 = gUnk_03005520 + gUnk_0300557C;
    gUnk_030055B0 = gUnk_03005520 + gUnk_03005584;
    if (gUnk_03005530.unkB & 0x80)
    {
        gUnk_03005530.unkB = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 16) >> 4;
    }
    v = gUnk_03005598;
    if (v != 0)
    {
        if (v < 0)
            sub_0801f6b0();
        else
            sub_0801f540();
    }
    sub_0802136c();
    sub_0801c5c8(p);
}

void sub_0801c030(const s8 *p)
{
    sub_0801c51c(p);
    gUnk_03005598 = gUnk_03002490->unk54;
    gUnk_03005514 = gUnk_03002490->unk58;
    gUnk_03005518 = ((gUnk_03005560 << 16) + (gUnk_03002490->unk4C & 0xFFFF) - gUnk_03005598) >> 16;
    gUnk_03005520 = ((gUnk_03005570 << 16) + (gUnk_03002490->unk50 & 0xFFFF) - gUnk_03005514) >> 16;
    gUnk_0300550C = gUnk_03005518 + gUnk_0300551C;
    gUnk_03005590 = gUnk_03005518 + gUnk_0300559C;
    gUnk_030055A4 = gUnk_03005520 + gUnk_0300557C;
    gUnk_030055B0 = gUnk_03005520 + gUnk_03005584;
    if (gUnk_03005530.unkB & 0x80)
    {
        gUnk_03005530.unkB = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 16) >> 4;
    }
    sub_0801f800();
    sub_0801f9b8();
    sub_0802136c();
    sub_0801c5c8(p);
}

void sub_0801c12c(const s8 *p)
{
    sub_0801c51c(p);
    gUnk_03005598 = gUnk_03002490->unk54;
    gUnk_03005514 = gUnk_03002490->unk58;
    gUnk_03005518 = ((gUnk_03005560 << 16) + (gUnk_03002490->unk4C & 0xFFFF) - gUnk_03005598) >> 16;
    gUnk_03005520 = ((gUnk_03005570 << 16) + (gUnk_03002490->unk50 & 0xFFFF) - gUnk_03005514) >> 16;
    gUnk_0300550C = gUnk_03005518 + gUnk_0300551C;
    gUnk_03005590 = gUnk_03005518 + gUnk_0300559C;
    gUnk_030055A4 = gUnk_03005520 + gUnk_0300557C;
    gUnk_030055B0 = gUnk_03005520 + gUnk_03005584;
    if (gUnk_03005530.unkB & 0x80)
    {
        gUnk_03005530.unkB = 1;
        gUnk_03005530.unkC = (gUnk_03005570 + gUnk_03005584 + 16) >> 4;
    }
    if (gUnk_03005530.unk6 != 0)
        sub_0801fc48();
    else
        sub_0801fe2c();
    sub_0802136c();
    sub_0801c5c8(p);
}

void sub_0801c230(const s8 *p)
{
    sub_0801c51c(p);
    gUnk_03005598 = gUnk_03002490->unk54;
    gUnk_03005514 = gUnk_03002490->unk58;
    gUnk_03005518 = ((gUnk_03005560 << 16) + (gUnk_03002490->unk4C & 0xFFFF) - gUnk_03005598) >> 16;
    gUnk_03005520 = ((gUnk_03005570 << 16) + (gUnk_03002490->unk50 & 0xFFFF) - gUnk_03005514) >> 16;
    gUnk_0300550C = gUnk_03005518 + gUnk_0300551C;
    gUnk_03005590 = gUnk_03005518 + gUnk_0300559C;
    gUnk_030055A4 = gUnk_03005520 + gUnk_0300557C;
    gUnk_030055B0 = gUnk_03005520 + gUnk_03005584;
    if (gUnk_03005530.unkB & 0x80)
        gUnk_03005530.unkB = 0;
    sub_0801ff84();
    sub_080214e0();
    sub_0801c5c8(p);
}
