#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x08021130-0x0802136C.
 *
 * The room probe for the player: sub_08021130 is sub_0802136c (still in asm)
 * with a tile-set special case in front (tile sets 64..79 and 192..207 pick a
 * pair of signed offsets from the gUnk_0300558C table into gUnk_030055A8 /
 * gUnk_03005580) and the gUnk_08733AF0 attribute copied to unkA behind.
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
s32 sub_080216d8(u32 x, u32 y);

void sub_08021130(void)
{
    u8 prev;
    u8 prev2;
    u32 zero;
    s32 y;
    s32 h;
    u16 f;
    u8 v;
    u16 tile;
    u16 t;

    sub_080216d8(gUnk_03005560, gUnk_03005570);
    tile = gUnk_03005578;
    t = tile - 64;
    if (t <= 15)
    {
        gUnk_030055A8 = gUnk_0300558C[(tile - 48) * 2];
        gUnk_03005580 = gUnk_0300558C[(gUnk_03005578 - 48) * 2 + 1];
    }
    else
    {
        t = tile - 192;
        if (t <= 15)
        {
            gUnk_030055A8 = gUnk_0300558C[(tile - 192) * 2];
            gUnk_03005580 = gUnk_0300558C[(gUnk_03005578 - 192) * 2 + 1];
        }
    }
    prev = gUnk_03005530.unk7;
    prev2 = prev;
    gUnk_03005530.unk8 = 0xFFFF;
    gUnk_03005530.unk7 = 0;
    zero = 0;
    y = gUnk_03005570;
    h = gUnk_0300561C << 4;
    if (y >= h)
    {
        sub_08021634(gUnk_03005560, h - 16);
        if (gUnk_03005578 & 0x80)
            gUnk_03005530.unk7 = 11;
    }
    else
    {
        f = gUnk_03005578 & 0x80;
        if (f)
        {
            gUnk_03005530.unk7 = 1;
            sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_0300557C);
            if (gUnk_03005578 & 0x80)
            {
                v = gUnk_03005530.unk7 | 2 | zero;
                gUnk_03005530.unk7 = v;
                if ((prev & 2) == 0)
                {
                    gUnk_03005530.unk7 = v | 0x80;
                    gUnk_03005530.unk8 = (gUnk_03005570 + gUnk_0300557C) & 0xFFF0;
                }
                else
                {
                    gUnk_03005530.unk7 = v | 8;
                }
            }
            else
            {
                gUnk_03005530.unk7 |= 0x48;
                gUnk_03005530.unk8 = ((gUnk_03005570 + gUnk_0300557C) & 0xFFF0) + 16;
            }
        }
        else
        {
            gUnk_03005530.unk7 = f;
            sub_08021634(gUnk_03005560, gUnk_03005570 + gUnk_03005584);
            if (gUnk_03005578 & 0x80)
            {
                gUnk_03005530.unk7 |= 0x48;
                gUnk_03005530.unk8 = (gUnk_03005570 + gUnk_03005584) & 0xFFF0;
            }
            else if (prev2 & 8)
            {
                gUnk_03005530.unk7 |= 0x80;
                gUnk_03005530.unk8 = ((gUnk_03005570 + gUnk_03005584) & 0xFFF0) + 16;
            }
        }
    }
    gUnk_03005530.unkA = 0;
    if (gUnk_08733AF0[tile] != 0)
        gUnk_03005530.unkA = 1;
}
