#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* door_26b60.c (0x08026B60-0x080270CF, issue #93).
 *
 * The door objects: one gUnk_02004B90 record per RoomDef door that is not
 * one of the special ids 0x1A0A, 0x1E61 or 0x15B3.  sub_08026b60 and
 * sub_08026ca4 (the per-frame body, flag 16 of gUnk_03005624) decide
 * whether a door is usable - in multi-player every present player must be
 * within 128 pixels - and step its animation (frame in the low nibble of
 * byte 4, timer in the high one, the star doors from gUnk_0873264C);
 * sub_08026eec draws the visible ones with sub_08001a94. */

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

extern struct RoomDef *gUnk_030055EC;
extern struct Unk02004B90 gUnk_02004B90[];
extern s16 gUnk_0200AEF0;
extern s32 gUnk_03001F2C;
extern s32 gUnk_03002448;
extern u16 gUnk_030023AC;
extern u8 gUnk_03002350;
extern s32 gUnk_03002344;
extern s16 gUnk_020055E0;
extern s8 gUnk_08733AF0[];
extern u32 gUnk_03002160;
extern u8 gUnk_03002340;
extern u8 gUnk_0873264C[][2];
extern s8 gUnk_03002444;
extern u32 gUnk_03001F10;
extern u8 gUnk_02007CF0;
extern u32 gUnk_0874CDF8[];
extern s16 gUnk_03002348;
extern u16 gUnk_030023E4;

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 sub_0800641c(s16 x, s16 y);
s32 sub_08021b18(u16 x, u16 y);

void sub_08026b60(void)
{
    struct Door *d = gUnk_030055EC->unk44;
    s16 i;

    for (i = 0; i < gUnk_030055EC->unk3A; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s32 lim;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gUnk_02004B90[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        p->unk1 = 0;
        if (gUnk_030023AC > 1 && gUnk_03002350 > 1)
        {
            gUnk_03002344 = 128;
            gUnk_03001F2C = x - gUnk_020055E0;
            gUnk_03002448 = y - gUnk_0200AEF0;
            lim = 0x4000;
            if (lim < gUnk_03001F2C * gUnk_03001F2C + gUnk_03002448 * gUnk_03002448)
                p->unk1 = 0;
            else
                p->unk1 = 1;
        }
        else
        {
            p->unk1 = 1;
        }
        if (--p->unk4_4 == 0)
        {
            if (++p->unk4_0 > 3)
                p->unk4_0 = 0;
            p->unk4_4 = 3;
        }
    }
}

void sub_08026ca4(void)
{
    struct Door *d = gUnk_030055EC->unk44;
    s16 i;

    for (i = 0; i < gUnk_030055EC->unk3A; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s16 j;
        s16 k;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gUnk_02004B90[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        p->unk1 = 0;
        k = sub_08021b18(x, y);
        if (gUnk_08733AF0[k] == 0)
            continue;
        if (gUnk_030023AC > 1 && gUnk_03002350 > 1)
        {
            gUnk_03002160 = 0;
            gUnk_03002344 = 128;
            for (j = 0; j < gUnk_030023AC; j++)
            {
                if ((gUnk_03002340 >> j) & 1)
                {
                    struct Task *t = &gUnk_03002790[j];

                    gUnk_03001F2C = x - t->unk48;
                    gUnk_03002448 = y - t->unk4A;
                    if (gUnk_03002344 * gUnk_03002344 >= gUnk_03001F2C * gUnk_03001F2C + gUnk_03002448 * gUnk_03002448)
                        gUnk_03002160++;
                }
                else
                {
                    gUnk_03002160++;
                }
            }
            if (gUnk_03002160 == gUnk_030023AC)
                p->unk1 = 1;
            else
                p->unk1 = 0;
        }
        else
        {
            p->unk1 = 1;
        }
        if (p->unk0 != 2)
        {
            if (--p->unk4_4 == 0)
            {
                if (++p->unk4_0 > 3)
                    p->unk4_0 = 0;
                p->unk4_4 = 3;
            }
        }
        else
        {
            if (--p->unk4_4 == 0)
            {
                if (++p->unk4_0 > 5)
                    p->unk4_0 = 0;
                p->unk4_4 = gUnk_0873264C[p->unk4_0][1];
            }
        }
    }
}

void sub_08026eec(void)
{
    struct Door *d;
    s16 i;

    if (gUnk_03002444 != 0)
        return;
    d = gUnk_030055EC->unk44;
    for (i = 0; i < gUnk_030055EC->unk3A; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s16 k;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gUnk_02004B90[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        if (sub_0800641c(x, y) == 0)
            continue;
        k = sub_08021b18(x, y);
        if (k != 16 && k != 144)
            continue;
        gUnk_03001F10 = -1;
        switch (p->unk0)
        {
        case 1:
            gUnk_03001F2C = 16;
            if ((gUnk_02007CF0 != 2 || gUnk_03002350 <= 1) && p->unk1 != 0)
                gUnk_03001F10 = 8;
            else
                gUnk_03001F10 = 12;
            gUnk_03001F10 += p->unk4_0;
            break;
        default:
        case 0:
            gUnk_03001F2C = 8;
            if ((gUnk_02007CF0 != 2 || gUnk_03002350 <= 1) && p->unk1 != 0)
                gUnk_03001F10 = 0;
            else
                gUnk_03001F10 = 4;
            gUnk_03001F10 += p->unk4_0;
            break;
        case 2:
            gUnk_03001F2C = 8;
            if (p->unk1 != 0)
                gUnk_03001F10 = 0;
            else
                gUnk_03001F10 = 6;
            gUnk_03001F10 += gUnk_0873264C[p->unk4_0][0];
            break;
        }
        sub_08001a94(15, gUnk_0874CDF8[gUnk_03001F10], 0, 0, x + gUnk_03001F2C - gUnk_03002348,
                     -gUnk_030023E4 + y);
    }
}
