#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_270d0.c (0x080270D0-0x0802739F, issue #93).
 *
 * Stage helpers, part 2.  sub_080270d0 restarts the room's BGM (with
 * the 0x800 flag), sub_08027128/sub_08027178/sub_08027198 tear the level
 * down before a state change (flags in gUnk_02007FB0), sub_080271ec/
 * sub_08027204/sub_08027210 set the per-frame flags gUnk_03005624, and
 * sub_08027228/sub_08027240 save and restore the OBJ palette and tiles
 * around M02's pause screen.  sub_080272dc picks the hub door the player
 * returns to (gUnk_020055E0/gUnk_0200AEF0).  The file stops before
 * sub_080273a0 because that function only matches without these nine in
 * front of it in the translation unit (lesson 4.79). */

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

struct RoomDef
{
    /*0x00*/ u8 filler00[4];
    /*0x04*/ s8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 *unk18;
    /*0x1C*/ void *unk1C;
    /*0x20*/ void *unk20;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u16 *unk28;
    /*0x2C*/ void *unk2C;
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u16 unk34;
    /*0x36*/ u16 unk36;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 unk3A;
    /*0x3C*/ u16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *unk44;
    /*0x48*/ void *unk48;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 unk50;
    /*0x52*/ u16 unk52;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

extern struct RoomDef *gUnk_030055EC;
extern s16 gUnk_087325A2[];
extern u16 gUnk_02007FB0;
extern s8 gUnk_02006A20[][3];
extern u16 gUnk_03005624;
extern u8 gUnk_03001370[];
extern u16 gUnk_02008060[];
extern vu16 gUnk_03001ED8;
extern u8 gUnk_020055B0;
extern u8 gUnk_0200B078;
extern u8 gUnk_0200B040;
extern struct RoomDef **gUnk_087E1D58[][8];
extern s8 gUnk_0300238C;
extern s8 gUnk_030023EC;
extern s8 gUnk_030023B8;
extern s8 gUnk_03002468;
extern s16 gUnk_020055E0;
extern s16 gUnk_0200AEF0;
extern u8 gUnk_02006170;
extern u8 gUnk_0200AF00;
extern u8 gUnk_020069F0;
extern u8 gUnk_03001F20;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08003110(s32 songId);
void sub_080034d0(void);
void sub_08003688(void);
void sub_080055c4(u16 val, s32 idx);
void sub_08005654(s32 id);
void sub_0800ab08(void);
void sub_08026998(void);
void sub_08029930(void);
void sub_0802b460(u16 a);
void sub_0802b49c(u16 a);
void sub_08030758(void);
void sub_080307a4(void);
void sub_080b6e60(void);
void sub_080b6ed4(void);
void sub_080b6f04(void);
void sub_080b6f20(void);

void sub_080270d0(void)
{
    s32 bgm = gUnk_030055EC->unk04;
    s16 v;

    if (bgm == -1)
    {
        sub_080034d0();
    }
    else if (gUnk_087325A2[bgm] != -1)
    {
        v = gUnk_087325A2[bgm] | 0x800;
        sub_08003110(v);
    }
    else
    {
        sub_08003110(bgm | 0x800);
    }
}

void sub_08027128(void)
{
    sub_080307a4();
    sub_080b6e60();
    sub_08005654(63);
    if (gUnk_02007FB0 & 1)
        sub_08026998();
    if (gUnk_02007FB0 & 2)
        sub_08003688();
    if (gUnk_02007FB0 & 4)
        sub_080055c4(31, 63);
    gUnk_02007FB0 = 0;
}

void sub_08027178(void)
{
    sub_080307a4();
    sub_080b6e60();
    sub_08005654(63);
    gUnk_02007FB0 = 0;
}

void sub_08027198(void)
{
    s32 i;
    s32 j;

    sub_08005654(63);
    for (j = 0; j <= 31; j++)
        for (i = 0; i <= 2; i++)
            if (gUnk_02006A20[j][i] != -1)
                sub_08005654(gUnk_02006A20[j][i]);
    sub_080307a4();
}

void sub_080271ec(void)
{
    gUnk_03005624 = 0;
    sub_080307a4();
    sub_080b6ed4();
}

void sub_08027204(u32 a)
{
    gUnk_03005624 = a;
}

void sub_08027210(void)
{
    gUnk_03005624 = 31;
    sub_08030758();
    sub_080b6f20();
}

void sub_08027228(void)
{
    CpuSet(gUnk_03001370, gUnk_02008060, 128);
}

void sub_08027240(void)
{
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1F00;
    sub_080017e4(8, (u32)gUnk_030055EC->unk2C, 0x06008000, 0);
    CpuSet(gUnk_02008060, gUnk_03001370, 128);
    if (gUnk_020055B0 == 1)
        sub_0802b49c(0x8000);
    if (gUnk_0200B078 == 6)
        sub_0802b460(0);
    else if (gUnk_0200B078 == 1)
        sub_0802b460(0x8000);
    sub_08029930();
    if (gUnk_0200B040 != 0)
        sub_080b6f04();
    sub_0800ab08();
}

void sub_080272dc(void)
{
    struct RoomDef *r;
    struct Door *d;
    s32 i;

    gUnk_0300238C = 8;
    gUnk_030023EC = gUnk_030023B8;
    gUnk_03002468 = 0;
    r = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][0];
    d = r->unk44;
    for (i = 0; i < r->unk3A; d++, i++)
    {
        if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
            break;
    }
    gUnk_020055E0 = (d->unk2 << 4) + 22;
    gUnk_0200AEF0 = (d->unk4 << 4) + 5;
    gUnk_02006170 = 1;
    gUnk_0200AF00 = 0;
    gUnk_020069F0 = 1;
}
