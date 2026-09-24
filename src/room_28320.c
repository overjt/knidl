#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* room_28320.c (0x08028320-0x08028B8B, issue #93).
 *
 * Room start-up services.  sub_08028320 clears the door-object slots
 * gUnk_02006A20[32][3], finds the door the player entered by and spawns
 * an M08 stage object for every locked or special door (a 9-way switch on
 * the door kind, gated by the save flags gUnk_08732348/gUnk_03002400);
 * sub_0802885c computes the BG3 parallax factors, sub_08028948 the room
 * bounds, sub_08028990 the multi-player group bounds and sub_08028b1c
 * copies the room bounds into the camera and per-player bounds. */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
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

struct CamRect { s16 x0, x1, y0, y1; };

extern s8 gUnk_0200B034;
extern s8 gUnk_02006A20[][3];
extern s32 gUnk_03005610;
extern s32 gUnk_03005664;
extern s32 gUnk_030055F0;
extern s32 gUnk_03005618;
extern s16 gUnk_020055D4;
extern u8 gUnk_020069F0;
extern s16 gUnk_020055E0;
extern s16 gUnk_0200AEF0;
extern struct RoomDef *gUnk_030055EC;
extern u16 gUnk_08732348[][9];
extern s8 gUnk_030023B8;
extern u32 gUnk_030023C8[];
extern u8 gUnk_03002400[8][7];
extern s8 gUnk_030023E0;
extern s8 gUnk_03002384;
extern s8 gUnk_030023EC;
extern u8 gUnk_0200AF00;
extern u8 gUnk_02007D58[];
extern u8 gUnk_0200AF08;
extern u8 gUnk_0200B04C;
extern s8 gUnk_03002444;
extern s32 gUnk_030055E8;
extern s32 gUnk_03005630;
extern u16 gUnk_03005608[2];
extern u16 gUnk_03005600[2];
extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern s16 gUnk_03005628[4];
extern u16 gUnk_030023AC;
extern u8 gUnk_03002340;
extern u16 gUnk_030055F4[2];
extern s16 gUnk_030055F8[4];
extern struct CamRect gUnk_03005640[4];

void sub_08008c64(u16 a0);
s32 sub_08021b18(u16 x, u16 y);
void sub_0802a190(void);
s32 sub_0802eac8(s32 x, s32 y, s32 a);
s32 sub_0802ec1c(s32 x, s32 y, s32 a);
s32 sub_0802ef90(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f05c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f1fc(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f31c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f420(s32 x, s32 y, s32 a);
s32 sub_0802f4c8(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f53c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f5b4(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802f7dc(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802fa3c(s32 x, s32 y, s32 a, s32 b);
s32 sub_0802fdf4(s32 x, s32 y, s32 a, s32 b);

void sub_08028320(void)
{
    s32 i, x, y, k, m, r;
    u32 v;
    struct Door *d;

    sub_08008c64(2);
    gUnk_0200B034 = -1;
    for (i = 0; i < 32; i++)
    {
        gUnk_02006A20[i][0] = -1;
        gUnk_02006A20[i][1] = -1;
        gUnk_02006A20[i][2] = -1;
    }
    gUnk_03005610 = 0;
    gUnk_03005664 = 0;
    gUnk_030055F0 = 0;
    gUnk_03005618 = 0;
    gUnk_020055D4 = 0x4000;
    if (gUnk_020069F0 == 1 || gUnk_020069F0 == 4)
    {
        r = sub_08021b18(gUnk_020055E0, gUnk_0200AEF0);
        if (r == 55 || r == 183)
            x = (gUnk_020055E0 >> 4) - 1;
        else
            x = gUnk_020055E0 >> 4;
        y = gUnk_0200AEF0 >> 4;
        d = gUnk_030055EC->unk44;
        for (i = 0; i < gUnk_030055EC->unk3A; d++, i++)
        {
            if (d->unk2 == x && d->unk4 == y)
            {
                gUnk_0200B034 = i;
                break;
            }
        }
    }
    d = gUnk_030055EC->unk44;
    for (i = 0; i < gUnk_030055EC->unk3A; d++, i++)
    {
        if (d->unk0 != 0x270F)
            continue;
        x = (d->unk2 << 4) + 16;
        y = (d->unk4 << 4) + 8;
        k = d->unk6 & 0xFF;
        v = gUnk_08732348[gUnk_030023B8][k];
        if (v != 0xFFFF)
        {
            if (v & 0x100)
            {
                if (!(gUnk_030023C8[0] & (1 << (v & 0xFF))))
                    continue;
            }
            else
            {
                if (!gUnk_03002400[gUnk_030023B8][v])
                    continue;
            }
        }
        switch (k)
        {
        case 0:
            if (gUnk_030023E0 > gUnk_030023B8 || gUnk_03002384 >= d->unk8)
            {
                switch (gUnk_03002400[gUnk_030023EC][d->unk8])
                {
                default:
                case 0:
                    gUnk_02006A20[i][0] = sub_0802f4c8(x, y, d->unk8, i);
                    break;
                case 1:
                    if (i == gUnk_0200B034 && gUnk_0200AF00 == 1)
                    {
                        gUnk_02006A20[i][0] = sub_0802f4c8(x, y, d->unk8, i);
                    }
                    else
                    {
                        gUnk_02006A20[i][0] = sub_0802f53c(x, y, d->unk8, i);
                        gUnk_02006A20[i][1] = sub_0802ef90(x, y, 15, 0x4000);
                    }
                    break;
                case 2:
                    if (i == gUnk_0200B034)
                    {
                        if (gUnk_020069F0 == 4)
                        {
                            gUnk_02006A20[i][0] = sub_0802f53c(x, y, d->unk8, i);
                            gUnk_02006A20[i][1] = sub_0802ef90(x, y, 15, 0x4000);
                            break;
                        }
                        if (gUnk_0200AF00 == 1)
                        {
                            gUnk_02006A20[i][0] = sub_0802f4c8(x, y, d->unk8, i);
                            break;
                        }
                    }
                    gUnk_02006A20[i][0] = sub_0802f5b4(x, y, d->unk8, i);
                    gUnk_02006A20[i][1] = sub_0802ef90(x, y, 15, 0x4000);
                    break;
                }
            }
            break;
        case 1:
            gUnk_02006A20[i][0] = sub_0802fdf4(x, y, 0, i);
            break;
        case 2:
            if (gUnk_030023B8 >= gUnk_030023E0)
                gUnk_02006A20[i][0] = sub_0802ec1c(x, y, i);
            else
                gUnk_02006A20[i][0] = sub_0802fdf4(x, y, 1, i);
            break;
        case 3:
            if (!(gUnk_02007D58[gUnk_030023EC] & 1))
                gUnk_02006A20[i][0] = sub_0802f1fc(x, y, 1, i);
            else if (i == gUnk_0200B034 && gUnk_0200AF00 == 2)
                gUnk_02006A20[i][0] = sub_0802f1fc(x, y, 1, i);
            else
                gUnk_02006A20[i][0] = sub_0802f1fc(x, y, 0, i);
            break;
        case 4:
            if (!(gUnk_02007D58[gUnk_030023EC] & 2))
                gUnk_02006A20[i][0] = sub_0802f31c(x, y, 1, i);
            else if (i == gUnk_0200B034 && gUnk_0200AF00 == 2)
                gUnk_02006A20[i][0] = sub_0802f31c(x, y, 1, i);
            else
                gUnk_02006A20[i][0] = sub_0802f31c(x, y, 0, i);
            break;
        case 5:
            if (!(gUnk_02007D58[gUnk_030023EC] & 4))
                gUnk_02006A20[i][0] = sub_0802f05c(x, y, 1, i);
            else if (i == gUnk_0200B034 && gUnk_0200AF00 == 2)
                gUnk_02006A20[i][0] = sub_0802f05c(x, y, 1, i);
            else
                gUnk_02006A20[i][0] = sub_0802f05c(x, y, 0, i);
            break;
        case 6:
            gUnk_020055D4 = 0x4000;
            m = 0;
            if (gUnk_0200AF08 != 0)
            {
                if (gUnk_0200AF08 & 16)
                    m = 1;
                else
                    gUnk_020055D4 = 0x2000;
            }
            if (gUnk_0200B04C & ~(1 << gUnk_030023EC))
                gUnk_02006A20[i][0] = sub_0802f7dc(x, y, 0, i);
            else
                gUnk_02006A20[i][0] = sub_0802f7dc(x, y, 1, i);
            gUnk_02006A20[i][1] = sub_0802fa3c(x, y, 0, m);
            gUnk_02006A20[i][2] = sub_0802fa3c(x, y, 1, m);
            break;
        case 8:
            gUnk_02006A20[i][0] = sub_0802eac8(x, y, i);
            break;
        case 7:
            gUnk_02006A20[i][0] = sub_0802f420(x, y, i);
            break;
        }
    }
}

void sub_0802885c(void)
{
    s32 num;
    s32 den;
    s32 a;
    s32 k;

    if (gUnk_03002444 != 0)
    {
        gUnk_030055E8 = 0x10000;
        gUnk_03005630 = 0x10000;
        return;
    }
    a = gUnk_030055EC->unk30->unk2;
    a <<= 3;
    k = gUnk_03005608[0] * 2 + 240;
    num = a - k;
    a = gUnk_03005620;
    a <<= 4;
    k = gUnk_03005600[0] * 2 + 240;
    den = a - k;
    if (den > 0)
    {
        gUnk_030055E8 = Div(num << 16, den);
        if (gUnk_030055E8 > 0x10000)
            gUnk_030055E8 = 0x10000;
    }
    else
    {
        gUnk_030055E8 = 0x10000;
    }
    a = gUnk_030055EC->unk30->unk4;
    a <<= 3;
    k = gUnk_03005608[1] * 2 + 160;
    num = a - k;
    a = gUnk_0300561C;
    a <<= 4;
    k = gUnk_03005600[1] * 2 + 160;
    den = a - k;
    if (den > 0)
    {
        gUnk_03005630 = Div(num << 16, den);
        if (gUnk_03005630 > 0x10000)
            gUnk_03005630 = 0x10000;
    }
    else
    {
        gUnk_03005630 = 0x10000;
    }
}

void sub_08028948(void)
{
    gUnk_03005628[0] = gUnk_03005600[0] + 120;
    gUnk_03005628[1] = gUnk_03005620 * 16 - gUnk_03005600[0] - 120;
    gUnk_03005628[2] = gUnk_03005600[1] + 80;
    gUnk_03005628[3] = gUnk_0300561C * 16 - gUnk_03005600[1] - 80;
}

void sub_08028990(void)
{
    s32 x0, x1, y0, y1, v, cx, cy, i;
    s16 t;

    x0 = gUnk_03005620 << 4;
    x1 = 0;
    y0 = gUnk_0300561C << 4;
    y1 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            v = gUnk_03002790[i].unk48;
            if (v < gUnk_03005628[0])
                v = gUnk_03005628[0];
            if (gUnk_03005628[1] < v)
                v = gUnk_03005628[1];
            if (v < x0)
                x0 = v;
            if (x1 < v)
                x1 = v;
            v = gUnk_03002790[i].unk4A;
            if (v < gUnk_03005628[2])
                v = gUnk_03005628[2];
            if (gUnk_03005628[3] < v)
                v = gUnk_03005628[3];
            if (v < y0)
                y0 = v;
            if (y1 < v)
                y1 = v;
        }
    }
    gUnk_030055F4[0] = cx = (x0 + x1) >> 1;
    gUnk_030055F4[1] = cy = (y0 + y1) >> 1;
    gUnk_030055F8[0] = t = cx - 80;
    gUnk_030055F8[1] = cx + 80;
    gUnk_030055F8[2] = cy - 120;
    gUnk_030055F8[3] = cy + 120;
    if (t < gUnk_03005628[0])
        gUnk_030055F8[0] = gUnk_03005628[0];
    if (gUnk_03005628[1] < gUnk_030055F8[1])
        gUnk_030055F8[1] = gUnk_03005628[1];
    if (gUnk_030055F8[2] < gUnk_03005628[2])
        gUnk_030055F8[2] = gUnk_03005628[2];
    if (gUnk_03005628[3] < gUnk_030055F8[3])
        gUnk_030055F8[3] = gUnk_03005628[3];
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            gUnk_03005640[i].x0 = gUnk_030055F8[0] - 117;
            gUnk_03005640[i].x1 = gUnk_030055F8[1] + 117;
            gUnk_03005640[i].y0 = gUnk_030055F8[2] - 76;
            gUnk_03005640[i].y1 = gUnk_030055F8[3] + 104;
        }
    }
    sub_0802a190();
}

void sub_08028b1c(void)
{
    s32 i;

    gUnk_030055F8[0] = gUnk_03005628[0];
    gUnk_030055F8[1] = gUnk_03005628[1];
    gUnk_030055F8[2] = gUnk_03005628[2];
    gUnk_030055F8[3] = gUnk_03005628[3];
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            gUnk_03005640[i].x0 = gUnk_030055F8[0] - 117;
            gUnk_03005640[i].x1 = gUnk_030055F8[1] + 117;
            gUnk_03005640[i].y0 = gUnk_030055F8[2] - 76;
            gUnk_03005640[i].y1 = gUnk_030055F8[3] + 104;
        }
    }
}
