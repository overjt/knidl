#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_273a0.c (0x080273A0-0x08027A6B, issue #93).
 *
 * Stage helpers, part 3.  sub_080273a0 picks the arrival door after a
 * level change (the stage door kind in gUnk_02008054), sub_08027548 and
 * sub_08027588/sub_080275cc keep the two-player race record
 * gUnk_02006098 (flags|0x80, lo, hi, previous, direction),
 * sub_080276ac/sub_080276cc/sub_08027750/sub_08027a30 the per-player
 * camera modes gUnk_0300560C, and sub_08027798, sub_080277f0,
 * sub_08027850 and sub_08027908 set the camera mode and target
 * (gUnk_03002398/gUnk_03001F00) for one or all players. */

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

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

struct CamPos { u16 x, y; };

extern struct RoomDef *gUnk_030055EC;
extern struct RoomDef **gUnk_087E1D58[][8];
extern u16 gUnk_02008054;
extern s8 gUnk_030023EC;
extern s8 gUnk_0300238C;
extern s8 gUnk_03002468;
extern u16 gUnk_02007FF0;
extern u16 gUnk_030023D8;
extern s16 gUnk_020055E0;
extern s16 gUnk_0200AEF0;
extern u8 gUnk_02006170;
extern u8 gUnk_0200AF00;
extern u8 gUnk_020069F0;
extern s8 gUnk_030023B8;
extern u16 gUnk_0200AFF4;
extern u16 gUnk_02008050;
extern s8 gUnk_02007D64;
extern u16 gUnk_02007D60;
extern u8 gUnk_0200B078;
extern s8 gUnk_02006098[];
extern u8 gUnk_0300560C[];
extern u16 gUnk_030023AC;
extern u8 gUnk_03002340;
extern u16 gUnk_030055C0;
extern u16 gUnk_03002360;
extern s16 gUnk_02007D48[];
extern s16 gUnk_03002398;
extern s16 gUnk_03001F00;
extern s32 gUnk_03005614;
extern s32 gUnk_03005634;
extern s8 gUnk_03002444;
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;
extern u8 gUnk_02007D38;
extern s16 gUnk_030055F8[4];
extern struct CamPos gUnk_030055D0[4];

void sub_08009e2c(s32 a);
void sub_0800a0dc(s32 a, s32 b);
void sub_08028b8c(void);
void sub_08028e3c(void);
s32 sub_0802d344(s32 a);

void sub_080273a0(void)
{
    struct RoomDef *r;
    struct Door *d;
    s32 i;
    s32 k;

    if (gUnk_02008054 & 0xFF00)
    {
        if (gUnk_02008054 == 0x100)
        {
            gUnk_030023EC = gUnk_0300238C;
            gUnk_0300238C = 8;
            gUnk_03002468 = 0;
            k = 2;
            gUnk_02007FF0++;
            if (gUnk_02007FF0 > 5)
                gUnk_02007FF0 = 5;
        }
        else
        {
            gUnk_03002468 = 0;
            k = gUnk_030023D8 - 11;
        }
        r = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
        d = r->unk44;
        for (i = 0; i < r->unk3A; d++, i++)
        {
            if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == k)
                break;
        }
        gUnk_020055E0 = (d->unk2 << 4) + 22;
        gUnk_0200AEF0 = (d->unk4 << 4) + 5;
        gUnk_02006170 = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 1;
        gUnk_030023D8 = 5;
    }
    else
    {
        if (gUnk_030023B8 == 7)
        {
            gUnk_02007FF0++;
            if (gUnk_02007FF0 > 5)
                gUnk_02007FF0 = 5;
        }
        gUnk_03002468 = gUnk_02008054;
        gUnk_020055E0 = gUnk_0200AFF4;
        gUnk_0200AEF0 = gUnk_02008050;
        gUnk_02006170 = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 0;
    }
}

void sub_08027548(void)
{
    if (gUnk_02007D64 != 4)
    {
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 &= 0x7FFF;
        gUnk_02007D60++;
        if (gUnk_02007D60 > 5)
            gUnk_02007D60 = 5;
    }
}

s32 sub_08027588(void)
{
    if (gUnk_0200B078 != 4 || gUnk_02006098[0] != 0 || sub_0802d344(1) == -1)
        return 0;
    gUnk_02006098[0] |= 0x80;
    return 1;
}

s32 sub_080275cc(s32 a)
{
    s32 lo;
    s32 hi;
    s32 prev;
    u8 dir;
    s8 *p;

    if (gUnk_0200B078 != 5)
        return 0;
    p = gUnk_02006098;
    if (p[0] & 0x80)
    {
        if (p[4] == 1)
        {
            if (p[2] == a)
                ((u8 *)p)[3] = 0xFF;
            else
            {
                if (p[1] == a)
                {
                    p[4] = -1 * p[4];
                    ((u8 *)p)[3] = 0xFF;
                }
                else
                {
                    if (a < p[1])
                        p[4] = -1 * p[4];
                    p[3] = a;
                }
            }
        }
        else
        {
            if (p[1] == a)
                ((u8 *)p)[3] = 0xFF;
            else
            {
                if (p[2] == a)
                {
                    p[4] = -1 * p[4];
                    ((u8 *)p)[3] = 0xFF;
                }
                else
                {
                    if (p[2] < a)
                        p[4] = -1 * p[4];
                    p[3] = a;
                }
            }
        }
        return;
    }
    if (p[0] == a)
        return 0;
    if (sub_0802d344(4) != -1)
    {
        if (gUnk_02006098[0] != 1 && a != 1)
        {
            lo = gUnk_02006098[0];
            hi = 1;
            prev = a;
        }
        else
        {
            lo = gUnk_02006098[0];
            hi = a;
            prev = -1;
        }
        if (gUnk_02006098[0] < a)
        {
            gUnk_02006098[1] = lo;
            gUnk_02006098[2] = hi;
            dir = 1;
        }
        else
        {
            gUnk_02006098[1] = hi;
            gUnk_02006098[2] = lo;
            dir = 0xFF;
        }
        gUnk_02006098[4] = dir;
        gUnk_02006098[3] = prev;
        gUnk_02006098[0] |= 0x80;
        return 1;
    }
    return 0;
}

s32 sub_080276ac(s32 a)
{
    gUnk_0300560C[gUnk_03002490->unk88->unk00] = 1;
    return a;
}

s32 sub_080276cc(s32 i)
{
    if (gUnk_030023AC > 1 && gUnk_03002340 != 0)
    {
        if (gUnk_030055C0 != 5)
            gUnk_0300560C[i] = 2;
        else
            gUnk_0300560C[i] = 3;
        if (i == gUnk_03002360)
        {
            if (gUnk_02007D48[i] != 0)
                sub_0800a0dc(26, gUnk_03002490->unk88->unk00);
            else
                sub_08009e2c(i);
        }
    }
}

s32 sub_08027750(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (((gUnk_03002340 >> i) & 1) == 0 && gUnk_0300560C[i] != 3)
            return 0;
    }
    return 1;
}

void sub_08027798(s32 x, s32 y)
{
    gUnk_03002398 = x;
    gUnk_03001F00 = y;
    gUnk_03005614 = gUnk_03002398 << 16;
    gUnk_03005634 = gUnk_03001F00 << 16;
    if (gUnk_03002444 != 0)
        gUnk_030055C0 = 4;
    else
        gUnk_030055C0 = 5;
}

void sub_080277f0(s32 x, s32 y)
{
    gUnk_030023CC = x;
    gUnk_03002388 = y;
    if (gUnk_03002444 != 0)
        gUnk_030055C0 = 0;
    else
        gUnk_030055C0 = 0;
    if (gUnk_02007D64 != 2)
    {
        if (gUnk_03002444 != 0)
            sub_08028e3c();
        else
            sub_08028b8c();
    }
}

void sub_08027850(s32 a)
{
    s32 i;
    s32 x;
    s32 y;

    if (gUnk_030023AC == 1)
    {
        gUnk_02007D38 = gUnk_03002360;
        gUnk_030055C0 = 0;
    }
    else
    {
        gUnk_02007D38 = a;
        gUnk_030055C0 = 1;
        for (i = 0; i < gUnk_030023AC; i++)
        {
            x = gUnk_03002398;
            y = gUnk_03001F00;
            if (x < gUnk_030055F8[0])
                x = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < x)
                x = gUnk_030055F8[1];
            if (y < gUnk_030055F8[2])
                y = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < y)
                y = gUnk_030055F8[3];
            gUnk_030055D0[i].x = x;
            gUnk_030055D0[i].y = y;
        }
    }
}

void sub_08027908(void)
{
    s32 i;
    s32 x;
    s32 y;
    u16 *m;

    if (gUnk_030023AC == 1)
    {
        x = gUnk_03002398;
        y = gUnk_03001F00;
        if (x < gUnk_030055F8[0])
            x = gUnk_030055F8[0];
        if (gUnk_030055F8[1] < x)
            x = gUnk_030055F8[1];
        if (y < gUnk_030055F8[2])
            y = gUnk_030055F8[2];
        if (gUnk_030055F8[3] < y)
            y = gUnk_030055F8[3];
        gUnk_030055D0[gUnk_03002360].x = x;
        gUnk_030055D0[gUnk_03002360].y = y;
        gUnk_030055C0 = 0;
    }
    else
    {
        i = 0;
        m = &gUnk_030055C0;
        for (; i < gUnk_030023AC; i++)
        {
            x = gUnk_03002398;
            y = gUnk_03001F00;
            if (x < gUnk_030055F8[0])
                x = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < x)
                x = gUnk_030055F8[1];
            if (y < gUnk_030055F8[2])
                y = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < y)
                y = gUnk_030055F8[3];
            gUnk_030055D0[i].x = x;
            gUnk_030055D0[i].y = y;
            if (i == gUnk_02007D38)
                gUnk_0300560C[i] = 0;
            else
                gUnk_0300560C[i] = 2;
        }
        *m = 3;
    }
}

s32 sub_08027a30(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_0300560C[i] == 2)
            return 0;
    }
    return 1;
}

void sub_08027a60(void)
{
    gUnk_030055C0 = 0;
}
