/* game_code_and_rodata_080653ec_0806ef5c 0x080BB528-0x080BC0CC
 * (issue #95, module M35, file 3 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BB528 0x080BC0CC src/subgame_bb528.c --newpb
 *
 * The duel's results screen: a seven-state task (sub_080bbd9c) whose
 * <entry, check> pairs are the anchor tables 0x08756378 / 0x08756394, and
 * the sprite spawners and placement helpers it calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s32 gCurTaskIdx;
extern s8 gUnk_02006168;
extern u8 gUnk_02006184;
extern u8 gUnk_0200B03C[];
extern u8 gUnk_0200B048;
extern u8 gUnk_0200B07C[];
extern vs16 gUnk_03000FB8;
extern vu16 gUnk_03001EB8[];
extern u16 gUnk_03002150;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u32 gUnk_08755BAC[];
extern s16 gUnk_0875636C[];
extern u32 gUnk_08756378[];
extern u32 gUnk_08756394[];

void TaskYieldTrampoline(u32 frames);
void sub_08002e98(u32 a, u32 b, u32 *c);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08005ca0(void);
void sub_08006138(void);
void sub_08006148(void *fn, u32 i);
void sub_080b9d0c(s32 a0);
void sub_080b9d24(void);
u8 sub_080b9d68(void);
void sub_080b9e30(void);
void sub_080ba5bc(s32 a0);

void sub_080bbb70(void);
void sub_080bbc04(void);
void sub_080bba1c(void);
void sub_080bbbb8(void);
void sub_080bbc70(void);
void sub_080bbcdc(void);
void sub_080bbd4c(void);
void sub_080bbde4(void);

void sub_080bb528(void)
{
    struct Task *t;

    if (gUnk_030023AC != 1)
        sub_080bbb70();
    else
        sub_080bbc04();
    t = gUnk_03002490;
    t->unk24 = 40;
    t->unk14 = 0;
}

void sub_080bb554(void)
{
    struct Task *t;

    gUnk_03002490->unk38 = gUnk_08755BAC;
    gUnk_03002490->unk42 = 4;
    t = gUnk_03002490;
    t->unk40 |= 0x800;
    t->unk3C = 0;
    t->unk48 = 120;
    t->unk4A = 152;
    t->unk1C = 0;
}

void sub_080bb59c(s32 a0)
{
    if (gUnk_03002360 == 0)
        sub_080031b8(a0);
}

void sub_080bb5b8(void)
{
    struct Task *t;

    if (gUnk_03001EB8[0] & 9)
    {
        sub_080bb59c(102);
        t = gUnk_03002490;
        t->unk28 = 0;
        if (t->unk1C == 0)
            t->unk14 = 5;
        else
            sub_080b9d24();
    }
    else if (gUnk_03001EB8[0] & 0x30)
    {
        if (gUnk_03001EB8[0] & 0x20)
        {
            t = gUnk_03002490;
            if (t->unk1C != 0)
            {
                t->unk1C = 0;
                t->unk3C = 0;
                sub_080bb59c(101);
            }
        }
        else
        {
            t = gUnk_03002490;
            if (t->unk1C == 0)
            {
                t->unk1C = 1;
                t->unk3C = 1;
                sub_080bb59c(101);
            }
        }
    }
}

void sub_080bb63c(void)
{
    struct Task *t = gUnk_03002490;
    s32 v;

    t->unk48 = 120;
    t->unk4A = 152;
    v = gUnk_02006168;
    t->unk1C = v;
    t->unk24 = 10;
    t->unk3C = v + 2;
}

void sub_080bb66c(void)
{
    struct Task *t;

    if (gUnk_03001EB8[0] & 9)
    {
        sub_080bb59c(102);
        sub_080b9d0c(gUnk_03002490->unk1C);
        gUnk_03002490->unk28 = 0;
    }
    else if (gUnk_03001EB8[0] & 2)
    {
        sub_080bb59c(215);
        gUnk_03002490->unk14 = 4;
        gUnk_03002490->unk28 = 0;
    }
    else if (gUnk_03001EB8[0] & 0x30)
    {
        if (gUnk_03001EB8[0] & 0x20)
        {
            t = gUnk_03002490;
            if (t->unk1C > 0)
            {
                t->unk1C--;
                sub_080bb59c(101);
            }
        }
        else
        {
            t = gUnk_03002490;
            if (t->unk1C <= 1)
            {
                t->unk1C++;
                sub_080bb59c(101);
            }
        }
        gUnk_03002490->unk3C = gUnk_03002490->unk1C + 2;
    }
}

void sub_080bb718(u16 a0, u16 a1, u16 a2)
{
    s32 i = sub_080058e4(94, 32);

    if (i != -1)
    {
        struct Task *t = &gUnk_03002790[i];

        t->unk73 = 7;
        t->unk3C = a0;
        t->unk48 = a1;
        t->unk4A = a2;
    }
}

void sub_080bb760(void)
{
    if (gUnk_030023AC != 1)
        gUnk_03002490->unk20 = 3;
    else
        gUnk_03002490->unk20 = gUnk_0875636C[gUnk_0200B048];
}

void sub_080bb7a0(void)
{
    if (gUnk_030023AC != 1)
        sub_080bba1c();
    else
        sub_080bbbb8();
    gUnk_03002490->unk20--;
}

void sub_080bb7cc(u8 a0, s16 a1, s16 a2, s8 a3)
{
    s32 i = sub_080058e4(94, 32);

    if (i != -1)
    {
        struct Task *t = &gUnk_03002790[i];

        t->unk73 = 8;
        t->unk18 = a0;
        t->unk48 = a1;
        t->unk4A = a2;
        t->unk1C = a3;
    }
}

void sub_080bb820(u8 a0, s16 a1, s16 a2, s8 a3)
{
    s32 i = sub_080058e4(94, 32);

    if (i != -1)
    {
        struct Task *t = &gUnk_03002790[i];

        t->unk48 = a1;
        t->unk4A = a2;
        t->unk73 = 9;
        t->unk18 = a0;
        t->unk1C = a3;
        t->unk3C = a0;
    }
}

void sub_080bb874(u8 a0, s8 a1)
{
    struct Task *t = &gUnk_03002790[a1];
    s16 x = t->unk48 - 24;
    s16 y = t->unk4A - 32;

    /* The ROM keeps a dead `ldrsh` of t->unk48: the switch sub_080bb930 has
       on unk4A, with every arm reduced to a no-op.  Merging the arms into
       one loses the load, so the four labels stay (the values are
       sub_080bb930's; the ROM cannot show them). */
    switch (t->unk48)
    {
    case 120:
        x = x;
        break;
    case 56:
        x = x;
        break;
    case 104:
        x = x;
        break;
    case 72:
        x = x;
        break;
    }
    if (gUnk_0200B03C[a1] == 0)
    {
        sub_080bb820(gUnk_0200B048 + 1, x, y, a1);
    }
    else
    {
        sub_080bb820(a0, x, y, a1);
        gUnk_0200B048 = a0;
    }
}

void sub_080bb8f8(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
        sub_080bb874(i, gUnk_0200B07C[i]);
}

void sub_080bb930(u8 a0, s8 a1)
{
    struct Task *base = gUnk_03002790;
    struct Task *t = &base[a1];
    s16 x = t->unk48;
    s16 y = t->unk4A;

    switch (t->unk4A)
    {
    case 120:
        y = 96;
        break;
    case 56:
        y = 40;
        break;
    case 104:
        y = 84;
        break;
    case 72:
        y = 52;
        break;
    }
    if (gUnk_0200B03C[a1] == 0)
        sub_080bb7cc(3, x, y, a1);
    else
        sub_080bb7cc(a0, x, y, a1);
}

void sub_080bb9b4(void)
{
    sub_080bb930(2, gUnk_0200B07C[1]);
}

void sub_080bb9cc(void)
{
    sub_080bb930(1, gUnk_0200B07C[1]);
    sub_080bb930(2, gUnk_0200B07C[2]);
}

void sub_080bb9f0(void)
{
    sub_080bb930(1, gUnk_0200B07C[1]);
    sub_080bb930(2, gUnk_0200B07C[2]);
    sub_080bb930(3, gUnk_0200B07C[3]);
}

void sub_080bba1c(void)
{
    struct Task *base = gUnk_03002790;
    struct Task *t = &base[gUnk_0200B07C[0]];
    s16 x = t->unk48 + (2 - gUnk_03002490->unk20) * 24;
    s16 y = t->unk4A;

    switch (t->unk4A)
    {
    case 120:
        y = 96;
        break;
    case 56:
        y = 40;
        break;
    case 104:
        y = 84;
        break;
    case 72:
        y = 52;
        break;
    }
    sub_080bb7cc(0, x, y, gUnk_0200B07C[0]);
    if (gUnk_03002490->unk20 == 3)
    {
        switch (gUnk_030023AC)
        {
        case 4:
            sub_080bb9f0();
            break;
        case 3:
            sub_080bb9cc();
            break;
        case 2:
            sub_080bb9b4();
            break;
        }
    }
}

void sub_080bbad4(void)
{
    s32 i = 0;
    s32 found = 0;

    for (; i < gUnk_030023AC; i++)
    {
        if (gUnk_0200B07C[i] == gUnk_03002360)
        {
            found = i;
            break;
        }
    }
    if ((gUnk_030023AC == 2 && found != 0) || gUnk_0200B03C[gUnk_03002360] == 0)
        found = 3;
    switch (found)
    {
    case 0:
        gUnk_03002490->unk2C = 29;
        break;
    case 1:
    case 2:
        gUnk_03002490->unk2C = 28;
        break;
    case 3:
        gUnk_03002490->unk2C = 23;
        break;
    }
}

void sub_080bbb70(void)
{
    if (gUnk_03002150 == 5)
        sub_080bb718(0, 120, 16);
    sub_080ba5bc(1);
    sub_080bbad4();
    if (gUnk_03002150 == 5)
        gUnk_03002490->unk30 = 2;
    else
        gUnk_03002490->unk30 = 3;
}

void sub_080bbbb8(void)
{
    u16 y;

    switch (gUnk_0200B048)
    {
    default:
        y = 152;
        break;
    case 4:
        y = 144;
        break;
    case 5:
        y = (3 - gUnk_03002490->unk20) * 18 + 144;
        break;
    }
    sub_080bb7cc(gUnk_0200B048, y, 64, 0);
}

void sub_080bbc04(void)
{
    sub_080bbc70();
    sub_080bbcdc();
    if (gUnk_03002150 != 5)
        sub_080bbd4c();
    switch (gUnk_0200B048)
    {
    case 5:
        gUnk_03002490->unk2C = 29;
        break;
    case 1 ... 4:
        gUnk_03002490->unk2C = 28;
        break;
    case 0:
        gUnk_03002490->unk2C = 23;
        break;
    }
    gUnk_03002490->unk30 = 1;
}

void sub_080bbc70(void)
{
    s32 id = sub_080058e4(94, 32);

    if (id != -1)
    {
        struct Task *t = &gUnk_03002790[id];

        t->unk73 = 1;
        t->unk18 = gUnk_0200B048;
        t->unk1C = 4;
        t->unk20 = -1;
        t->unk24 = 6;
        t->unk48 = 80;
        t->unk4A = 16;
        t->unk28 = -12;
        t->unk2C = 80;
        t->unk30 = 0x08755A88;
        t->unk34 = 0;
    }
}

void sub_080bbcdc(void)
{
    s32 id = sub_080058e4(94, 32);

    if (id != -1)
    {
        struct Task *t = &gUnk_03002790[id];
        s32 v;
        s32 n;

        t->unk73 = 1;
        t->unk1C = 5;
        t->unk20 = -1;
        t->unk48 = 172;
        t->unk4A = 96;
        t->unk28 = -8;
        t->unk2C = 0;
        t->unk30 = 0x08755A34;
        t->unk34 = 0;
        v = gUnk_02006184;
        n = 0;
        while (v > 9)
        {
            v -= 10;
            n++;
        }
        t->unk24 = n;
        t->unk18 = v;
    }
}

void sub_080bbd4c(void)
{
    s32 id = sub_080058e4(94, 0);

    if (id != -1)
    {
        struct Task *t = &gUnk_03002790[id];

        t->unk44 = gCurTaskIdx;
        t->unk18 = id;
        t->unk73 = 0;
        t->unk74 = 2;
        t->unk28 = 0;
    }
}

void sub_080bbd9c(void)
{
    struct Task *t;

    sub_080bb528();
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    t = gUnk_03002490;
    t->unk04 = (u32)sub_080bbde4;
    sub_08002e98(t->unk14, 7, gUnk_08756378);
    sub_08006138();
}

void sub_080bbde4(void)
{
    sub_08002e98(gUnk_03002490->unk15, 7, gUnk_08756394);
    sub_080b9e30();
}

void sub_080bbe04(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08756378);
}

void sub_080bbe20(void)
{
    gUnk_03002490->unk15 = 0;
    TaskYieldTrampoline(30);
    sub_08003110(gUnk_03002490->unk2C | 0x800);
    TaskYieldTrampoline(180);
    gUnk_03002490->unk14 = gUnk_03002490->unk30;
    sub_08006138();
}

void sub_080bbe58(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080bbe04, gCurTaskIdx);
}

void sub_080bbe80(void)
{
    gUnk_03002490->unk15 = 1;
    TaskYieldTrampoline(16);
    if (gUnk_03002150 == 5)
    {
        if (gUnk_0200B048 != 0)
        {
            sub_080bb718(0, 96, 64);
            gUnk_03002490->unk14 = 2;
        }
        else
        {
            sub_080bb718(1, 120, 64);
            gUnk_03002490->unk14 = 6;
        }
    }
    else
        gUnk_03002490->unk14 = 4;
    sub_08006138();
}

void sub_080bbedc(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080bbe04, gCurTaskIdx);
}

void sub_080bbf04(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080bb760();
    TaskYieldTrampoline(16);
    while (gUnk_03002490->unk20 != 0)
    {
        sub_080bb7a0();
        TaskYieldTrampoline(20);
    }
    gUnk_03002490->unk14 = 6;
    sub_08006138();
}

void sub_080bbf44(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080bbe04, gCurTaskIdx);
}

void sub_080bbf6c(void)
{
    gUnk_03002490->unk15 = 3;
    TaskYieldTrampoline(16);
    sub_080bb8f8();
    TaskYieldTrampoline(20);
    gUnk_03002490->unk14 = 4;
    sub_08006138();
}

void sub_080bbf9c(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080bbe04, gCurTaskIdx);
}

void sub_080bbfc4(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk28 = 0;
    TaskYieldTrampoline(gUnk_03002490->unk24);
    if (gUnk_03002360 == 0)
        gUnk_03002490->unk0C = (u32)sub_08005ca0;
    sub_080bb554();
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_080bc008(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        sub_080bb5b8();
        if (gUnk_03002490->unk14 != 4)
            sub_08006148(sub_080bbe04, gCurTaskIdx);
    }
}

void sub_080bc03c(void)
{
    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk28 = 0;
    TaskYieldTrampoline(8);
    sub_080bb63c();
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_080bc06c(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        sub_080bb66c();
        if (gUnk_03002490->unk14 != 5)
            sub_08006148(sub_080bbe04, gCurTaskIdx);
    }
}

void sub_080bc0a0(void)
{
    gUnk_03002490->unk15 = 6;
    sub_08006138();
}

void sub_080bc0b8(void)
{
    if (sub_080b9d68())
        sub_080b9d24();
}
