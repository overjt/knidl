#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C8DC-0x0801C930.
 *
 * sub_0801c8dc: count a step when the cell below the actor's box is solid.
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
void sub_080214e0(void);
u32 sub_0802069c(void);
s32 sub_08021990(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_08021634(u32 x, u32 y);

s32 sub_08021634(u32 x, u32 y);

void sub_0801c8dc(void)
{
    if (sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C) != 0 && gUnk_087336F0[gUnk_03005578] == 0)
        gUnk_03005530.unk0++;
}
