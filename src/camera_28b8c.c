#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_28b8c.c (0x08028B8C-0x0802969F, issue #93).
 *
 * Camera start-up for a new room.  sub_08028b8c (camera modes other than
 * 5) copies the room bounds into the camera bounds and puts the camera,
 * its 16.16 target and the visible rectangle on the player (one player)
 * or runs M08's multi-player updates; sub_08028e3c only sets the bounds.
 * sub_08028e4c places the player at the room's start position
 * (RoomDef.unk50/unk52) unless a door already did, clamps it and records
 * the arrival for the next level change (gUnk_02008054, gUnk_0200AFF4,
 * gUnk_02008050).  sub_08029034 clamps the arrival position into the room
 * and makes it the camera target, sub_080290ac copies the current
 * player's camera position into the player cells, sub_080290dc and
 * sub_080293d8/sub_08029444/sub_08029474
 * set the camera and BG3 positions (BG3 moves by the parallax factors of
 * room_28320.c), sub_080294d0/sub_080295b8 stream the BG and BG3 maps
 * for a camera move (M08's sub_080297dc again), sub_08029110/
 * sub_08029194 start the room's BGM, sub_080291d0 ... sub_080292d0 load
 * the room palettes, tiles and BG map (sub_08029270 picks the BG layout
 * gUnk_020055B0 from the map size) and sub_08029318 builds the door
 * objects. */

struct CamPos { u16 x, y; };

struct CamRect { s16 x0, x1, y0, y1; };

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

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk4_0:4;
    /*0x04*/ u8 unk4_4:4;
    /*0x05*/ u8 filler05[3];
};

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
};

struct Unk020055D8
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk020055D8Entry *unk4;
};

extern u16 gUnk_030055C0;
extern s8 gUnk_03002444;
extern u16 gUnk_030023AC;
extern s16 gUnk_030055F8[4];
extern s16 gUnk_03005628[4];
extern struct CamRect gUnk_03005640[4];
extern u16 gUnk_03002360;
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;
extern struct CamPos gUnk_030055D0[4];
extern s32 gUnk_03005614;
extern s32 gUnk_03005634;
extern s16 gUnk_03002158[4];
extern u8 gUnk_03002340;
extern s16 gUnk_03002398;
extern s16 gUnk_03001F00;
extern u8 gUnk_02006170;
extern s16 gUnk_020055E0;
extern s16 gUnk_0200AEF0;
extern struct RoomDef *gUnk_030055EC;
extern s8 gUnk_0200B038;
extern u16 gUnk_02008054;
extern s8 gUnk_03002468;
extern u8 gUnk_020069F0;
extern u16 gUnk_0200AFF4;
extern u16 gUnk_02008050;
extern u16 gUnk_0300566C[2];
extern u16 gUnk_03005604[2];
extern u16 gUnk_03005668[2];
extern u16 gUnk_03005690[2];
extern u8 gUnk_020055C8;
extern s16 gUnk_087325A2[];
extern s8 gUnk_02007D64;
extern u8 gUnk_0200AF04;
extern u16 gUnk_030012B0[];
extern u8 gUnk_03001470[];
extern u8 gUnk_020055B0;
extern struct Unk020055D8 gUnk_020055D8;
extern s16 gUnk_02004B78[4];
extern struct Unk02004B90 gUnk_02004B90[];
extern u16 gUnk_03005600[2];
extern s32 gUnk_030055E8;
extern u16 gUnk_03005608[2];
extern s32 gUnk_03005630;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08003110(s32 songId);
s32 sub_08003194(void);
void sub_080031b8(s32 id);
void sub_080034d0(void);
s32 sub_08021b2c(u32 x, u32 y);
void sub_08028948(void);
void sub_08028b1c(void);
void sub_08029c74(void);
void sub_08029e24(void);
void sub_0802a190(void);
void sub_0802a42c(void);
void sub_0802a4ec(void);
void sub_0802a82c(s32 x0, s32 x1, s32 y0, s32 y1);
void sub_0802aa4c(s32 x0, s32 x1, s32 y);
void sub_0802aa9c(s32 x, s32 y0, s32 y1);
void sub_0802aba8(s32 x0, s32 x1, s32 y);
void sub_0802abec(s32 x, s32 y0, s32 y1);
s32 sub_080408e4(void);
void sub_080293d8(void);

void sub_08028b8c(void)
{
    s32 x;
    s32 y;
    s32 i;

    sub_08028948();
    if (gUnk_030055C0 != 5)
    {
        if (gUnk_03002444 != 0 || gUnk_030023AC == 1)
        {
            *(long long *)gUnk_030055F8 = *(long long *)gUnk_03005628;
            gUnk_03005640[gUnk_03002360].x0 = gUnk_030055F8[0] - 117;
            gUnk_03005640[gUnk_03002360].x1 = gUnk_030055F8[1] + 117;
            gUnk_03005640[gUnk_03002360].y0 = gUnk_030055F8[2] - 76;
            gUnk_03005640[gUnk_03002360].y1 = gUnk_030055F8[3] + 104;
            x = gUnk_030023CC;
            y = gUnk_03002388;
            if (x < gUnk_030055F8[0])
                x = gUnk_030055F8[0];
            if (x > gUnk_030055F8[1])
                x = gUnk_030055F8[1];
            if (y < gUnk_030055F8[2])
                y = gUnk_030055F8[2];
            if (y > gUnk_030055F8[3])
                y = gUnk_030055F8[3];
            gUnk_030055D0[gUnk_03002360].x = x;
            gUnk_030055D0[gUnk_03002360].y = y;
            gUnk_03005614 = x << 16;
            gUnk_03005634 = y << 16;
            gUnk_03002158[0] = x - 120;
            gUnk_03002158[1] = x + 120;
            gUnk_03002158[2] = y - 80;
            gUnk_03002158[3] = y + 80;
        }
        else
        {
            sub_08029c74();
            sub_08029e24();
            sub_0802a190();
            sub_0802a42c();
            if ((gUnk_03002340 >> gUnk_03002360) & 1)
            {
                x = gUnk_030023CC;
                y = gUnk_03002388;
                if (x < gUnk_030055F8[0])
                    x = gUnk_030055F8[0];
                if (x > gUnk_030055F8[1])
                    x = gUnk_030055F8[1];
                if (y < gUnk_030055F8[2])
                    y = gUnk_030055F8[2];
                if (y > gUnk_030055F8[3])
                    y = gUnk_030055F8[3];
            }
            else
            {
                x = gUnk_030055D0[gUnk_03002360].x;
                y = gUnk_030055D0[gUnk_03002360].y;
            }
            gUnk_03005614 = x << 16;
            gUnk_03005634 = y << 16;
            sub_0802a4ec();
        }
    }
    else
    {
        x = gUnk_03002398;
        y = gUnk_03001F00;
        if (x < gUnk_03005628[0])
            x = gUnk_03005628[0];
        if (x > gUnk_03005628[1])
            x = gUnk_03005628[1];
        if (y < gUnk_03005628[2])
            y = gUnk_03005628[2];
        if (y > gUnk_03005628[3])
            y = gUnk_03005628[3];
        gUnk_030055F8[0] = gUnk_030055F8[1] = x;
        gUnk_030055F8[2] = gUnk_030055F8[3] = y;
        for (i = 0; i < gUnk_030023AC; i++)
        {
            gUnk_03005640[i].x0 = x - 117;
            gUnk_03005640[i].x1 = x + 117;
            gUnk_03005640[i].y0 = y - 76;
            gUnk_03005640[i].y1 = y + 104;
            gUnk_030055D0[i].x = x;
            gUnk_030055D0[i].y = y;
        }
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        gUnk_03002158[0] = x - 120;
        gUnk_03002158[1] = x + 120;
        gUnk_03002158[2] = y - 80;
        gUnk_03002158[3] = y + 80;
    }
}

void sub_08028e3c(void)
{
    sub_08028948();
    sub_08028b1c();
}

void sub_08028e4c(void)
{
    s32 v;

    if (gUnk_02006170 == 0)
    {
        gUnk_020055E0 = gUnk_030055EC->unk50;
        gUnk_0200AEF0 = gUnk_030055EC->unk52 + 0xFFFD;
        gUnk_02006170 = 0;
    }
    v = gUnk_03005628[0] - 117;
    if (gUnk_020055E0 < v)
        gUnk_020055E0 = v;
    v = gUnk_03005628[1] + 117;
    if (v < gUnk_020055E0)
        gUnk_020055E0 = v;
    v = gUnk_03005628[2] - 76;
    if (gUnk_0200AEF0 < v)
        gUnk_0200AEF0 = v;
    v = gUnk_03005628[3] + 104;
    if (v < gUnk_0200AEF0)
        gUnk_0200AEF0 = v;
    if (gUnk_0200B038 == 0)
    {
        gUnk_02008054 = gUnk_03002468;
        if (gUnk_020069F0 == 2)
        {
            gUnk_0200AFF4 = gUnk_030055EC->unk50;
            gUnk_02008050 = gUnk_030055EC->unk52;
        }
        else
        {
            gUnk_0200AFF4 = gUnk_020055E0;
            gUnk_02008050 = gUnk_0200AEF0;
        }
        gUnk_0200B038 = 0;
    }
    gUnk_030023CC = gUnk_020055E0;
    gUnk_03002388 = gUnk_0200AEF0;
    if (gUnk_03002444 != 0)
    {
        if (gUnk_030055C0 == 2 || gUnk_030055C0 == 4)
        {
            gUnk_03002398 = gUnk_030023CC;
            gUnk_03001F00 = gUnk_03002388;
            if (gUnk_03002398 < gUnk_03005628[0])
                gUnk_03002398 = gUnk_03005628[0];
            if (gUnk_03002398 > gUnk_03005628[1])
                gUnk_03002398 = gUnk_03005628[1];
            if (gUnk_03001F00 < gUnk_03005628[2])
                gUnk_03001F00 = gUnk_03005628[2];
            if (gUnk_03001F00 > gUnk_03005628[3])
                gUnk_03001F00 = gUnk_03005628[3];
        }
    }
    else if (gUnk_030055C0 == 5)
    {
        gUnk_03002398 = gUnk_030023CC;
        gUnk_03001F00 = gUnk_03002388;
        if (gUnk_03002398 < gUnk_03005628[0])
            gUnk_03002398 = gUnk_03005628[0];
        if (gUnk_03002398 > gUnk_03005628[1])
            gUnk_03002398 = gUnk_03005628[1];
        if (gUnk_03001F00 < gUnk_03005628[2])
            gUnk_03001F00 = gUnk_03005628[2];
        if (gUnk_03001F00 > gUnk_03005628[3])
            gUnk_03001F00 = gUnk_03005628[3];
    }
}

void sub_08029034(void)
{
    s32 v;

    v = gUnk_03005628[0] - 117;
    if (gUnk_020055E0 < v)
        gUnk_020055E0 = v;
    v = gUnk_03005628[1] + 117;
    if (v < gUnk_020055E0)
        gUnk_020055E0 = v;
    v = gUnk_03005628[2] - 76;
    if (gUnk_0200AEF0 < v)
        gUnk_0200AEF0 = v;
    v = gUnk_03005628[3] + 104;
    if (v < gUnk_0200AEF0)
        gUnk_0200AEF0 = v;
    gUnk_03002398 = gUnk_020055E0;
    gUnk_03001F00 = gUnk_0200AEF0;
}

void sub_080290ac(void)
{
    gUnk_030023CC = gUnk_030055D0[gUnk_03002360].x;
    gUnk_03002388 = gUnk_030055D0[gUnk_03002360].y;
}

void sub_080290dc(void)
{
    sub_080293d8();
    gUnk_0300566C[0] = gUnk_03005604[0];
    gUnk_0300566C[1] = gUnk_03005604[1];
    gUnk_03005668[0] = gUnk_03005690[0];
    gUnk_03005668[1] = gUnk_03005690[1];
}

void sub_08029110(void)
{
    s32 bgm;
    s32 cur;

    if (sub_080408e4() == 0)
    {
        bgm = gUnk_030055EC->unk04;
        if (bgm == -1)
        {
            sub_080034d0();
        }
        else if (gUnk_020055C8 == 0)
        {
            sub_08003110(bgm);
            gUnk_020055C8 = 1;
        }
        else
        {
            cur = sub_08003194();
            if (cur == -1 || cur != bgm)
            {
                if (gUnk_087325A2[bgm] != -1)
                    sub_08003110(gUnk_087325A2[bgm]);
                else
                    sub_08003110(bgm);
            }
        }
    }
    if (gUnk_02007D64 == 4)
        sub_080031b8(249);
}

void sub_08029194(void)
{
    if (gUnk_0200AF04 == 0)
    {
        if (gUnk_030055EC->unk04 == -1)
            sub_080034d0();
        else
            sub_08003110(gUnk_030055EC->unk04);
    }
    else
    {
        gUnk_0200AF04 = 0;
    }
}

void sub_080291d0(void)
{
    sub_080017e4(8, (u32)gUnk_030055EC->unk1C, 0x06004000, 0);
    sub_080017e4(2, (u32)(gUnk_030055EC->unk18 + 1), (u32)gUnk_030012B0, gUnk_030055EC->unk18[0]);
}

void sub_08029204(void)
{
    sub_080017e4(8, (u32)gUnk_030055EC->unk2C, 0x06008000, 0);
    sub_080017e4(2, (u32)(gUnk_030055EC->unk28 + 1), (u32)(gUnk_03001470 - gUnk_030055EC->unk28[0]), gUnk_030055EC->unk28[0]);
}

void sub_0802923c(void)
{
    u32 a;
    u32 b;

    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    b = 0;
    CpuFastSet(&b, (u32 *)0x06003000, 0x01000400);
}

void sub_08029270(void)
{
    s32 w;
    s32 h;

    w = gUnk_030055EC->unk30->unk2;
    h = gUnk_030055EC->unk30->unk4;
    if (w <= 64 && h <= 32)
        gUnk_020055B0 = 0;
    else if (w <= 32 && h <= 64)
        gUnk_020055B0 = 1;
    else
        gUnk_020055B0 = 2;
}

void sub_080292b0(void)
{
    sub_080017e4(8, (u32)gUnk_030055EC->unk30 + 8, 0x06003000, 0);
}

void sub_080292d0(void)
{
    if (gUnk_020055D8.unk0 != 0)
    {
        sub_0802a82c(gUnk_03002158[0] - 36, gUnk_03002158[1] + 36, gUnk_03002158[2] - 40, gUnk_03002158[3] + 40);
        *(long long *)gUnk_02004B78 = *(long long *)gUnk_03002158;
    }
}

void sub_08029318(void)
{
    struct Door *d;
    s32 i;
    s32 a;
    s32 b;

    d = gUnk_030055EC->unk44;
    for (i = 0; i < gUnk_030055EC->unk3A; i++)
    {
        gUnk_02004B90[i].unk1 = 1;
        /* The whole byte at +4 (unk4_0/unk4_4) cleared with one strb
           through the struct: two bit-field stores leave the -16 mask and a
           zero in the loop and change what loop.c hoists (0x22B8 then goes
           to sl). */
        gUnk_02004B90[i].filler02[2] = 0;
        a = sub_08021b2c(d->unk2, d->unk4);
        b = sub_08021b2c(d->unk2 + 1, d->unk4);
        if ((a == 16 && b == 55) || (a == 144 && b == 183))
        {
            gUnk_02004B90[i].unk0 = 1;
            gUnk_02004B90[i].unk4_0 = 8;
            gUnk_02004B90[i].unk4_4 = 3;
        }
        else if (d->unk0 == 0x22B8)
        {
            gUnk_02004B90[i].unk0 = 2;
            gUnk_02004B90[i].unk4_0 = 2;
            gUnk_02004B90[i].unk4_4 = 3;
        }
        else
        {
            gUnk_02004B90[i].unk0 = 0;
            gUnk_02004B90[i].unk4_0 = 0;
            gUnk_02004B90[i].unk4_4 = 3;
        }
        d++;
    }
}

void sub_080293d8(void)
{
    gUnk_03005604[0] = (gUnk_03005614 >> 16) - 120;
    gUnk_03005604[1] = (gUnk_03005634 >> 16) - 80;
    gUnk_03005690[0] = (((gUnk_03005604[0] - gUnk_03005600[0]) * gUnk_030055E8) >> 16) + gUnk_03005608[0];
    gUnk_03005690[1] = (((gUnk_03005604[1] - gUnk_03005600[1]) * gUnk_03005630) >> 16) + gUnk_03005608[1];
}

void sub_08029444(void)
{
    gUnk_03005690[0] = gUnk_03005604[0] = (gUnk_03005614 >> 16) - 120;
    gUnk_03005690[1] = gUnk_03005604[1] = (gUnk_03005634 >> 16) - 80;
}

void sub_08029474(void)
{
    gUnk_03005604[0] = (gUnk_03005614 >> 16) - 120;
    gUnk_03005604[1] = (gUnk_03005634 >> 16) - 80;
    gUnk_03005690[0] += 0xFFFF;
    gUnk_03005690[1] = (((gUnk_03005604[1] - gUnk_03005600[1]) * gUnk_03005630) >> 16) + gUnk_03005608[1];
}

void sub_080294d0(void)
{
    s32 x, x0, x1, y, y0, y1, d, i;

    x = gUnk_03005604[0] >> 3;
    x0 = x - 3;
    x1 = x + 32;
    y = gUnk_03005604[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = x - (gUnk_0300566C[0] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802aa9c(x1 - i, y0, y1);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802aa9c(x0 + i, y0, y1);
        }
        gUnk_0300566C[0] = gUnk_03005604[0];
    }
    d = (gUnk_03005604[1] >> 3) - (gUnk_0300566C[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802aa4c(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802aa4c(x0, x1, y0 + i);
        }
        gUnk_0300566C[1] = gUnk_03005604[1];
    }
}

void sub_080295b8(void)
{
    s32 x, x0, x1, y, y0, y1, d, i;

    x = gUnk_03005690[0] >> 3;
    x0 = x - 3;
    x1 = x + 32;
    y = gUnk_03005690[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = x - (gUnk_03005668[0] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802abec(x1 - i, y0, y1);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802abec(x0 + i, y0, y1);
        }
        gUnk_03005668[0] = gUnk_03005690[0];
    }
    d = (gUnk_03005690[1] >> 3) - (gUnk_03005668[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802aba8(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802aba8(x0, x1, y0 + i);
        }
        gUnk_03005668[1] = gUnk_03005690[1];
    }
}
