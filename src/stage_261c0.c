#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_261c0.c (0x080261C0-0x08026B5F, issue #93).
 *
 * Stage helpers the rest of the game calls.  sub_080261d4 starts a screen
 * shake (gUnk_03005670, levels 1-4 only upgrade), sub_0802621c spawns a task
 * in one of the free high slots 32-62, sub_08026264/sub_08026278 place the
 * player or the camera target, sub_080262e8 spawns a map event at a
 * tabled position, sub_08026308 wraps every camera, object and task
 * coordinate back by 0x200 pixels in a looping room, sub_08026900 clamps
 * the player to the room bounds and sub_0802695c starts the next stage.
 * sub_080264b0, sub_0802651c, sub_0802653c, sub_08026584 and sub_08026704
 * spawn and adjust the M08 stage objects of the door the player entered by
 * (gUnk_0200B034, its slots in gUnk_02006A20); sub_0802672c/sub_08026834
 * spawn a map-event task and put the camera on the player or a partner.
 * sub_08026a0c, sub_08026a80 and sub_08026aec arm the scroll lock of one
 * room each.  sub_08026994 is an empty dead export. */

struct Unk03005670
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 filler01;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ u8 unk6;
};

struct CamRect { s16 x0, x1, y0, y1; };

struct Unk020061F0
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u8 filler08[0x18];
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

struct Unk03005680
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
};

extern struct Unk03005670 gUnk_03005670;
extern vs16 gUnk_03004CA0[];
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;
extern s8 gUnk_03002444;
extern u16 gUnk_030055C0;
extern s16 gUnk_03002398;
extern s16 gUnk_03001F00;
extern u8 gUnk_0200D080;
extern u16 gUnk_08732638[][2];
extern u16 gUnk_0300566C[2];
extern u16 gUnk_03005604[2];
extern s32 gUnk_03005614;
extern u16 gUnk_03005690[2];
extern u16 gUnk_03005668[2];
extern s16 gUnk_03002158[4];
extern s16 gUnk_030055F8[4];
extern s16 gUnk_020055B8[4];
extern u16 gUnk_030023AC;
extern struct CamRect gUnk_03005640[4];
extern s16 gUnk_03002348;
extern u16 gUnk_02007FA0;
extern struct Unk020061F0 gUnk_020061F0[];
extern s16 gUnk_03005620;
extern struct RoomDef *gUnk_030055EC;
extern s8 gUnk_02006A20[][3];
extern s8 gUnk_0200B034;
extern u8 gUnk_0200AF00;
extern u8 gUnk_03002400[8][7];
extern s8 gUnk_030023EC;
extern u8 gUnk_0200AF08;
extern s16 gUnk_03005628[4];
extern u8 gUnk_020055E8;
extern u8 gUnk_02007D38;
extern s16 gUnk_020055E0;
extern s16 gUnk_0200AEF0;
extern s8 gUnk_030023E0;
extern s8 gUnk_030023B8;
extern s8 gUnk_03002384;
extern u8 gUnk_03001F20;
extern u16 gUnk_02007FB0;
extern u8 gUnk_03001F30;
extern s32 gUnk_030023E8;
extern u8 gUnk_02005578;
extern s8 gUnk_02007FF8;
extern struct Unk03005680 gUnk_03005680;

void sub_080031b8(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08006148(void *a, u32 i);
void sub_08029930(void);
void sub_0802cda0(s32 x0, s32 x1, s32 y0, s32 y1);
void sub_0802d074(void);
s32 sub_0802d344(s32 type);
s32 sub_0802d478(s32 x, s32 y);
s32 sub_0802ed94(s32 x, s32 y, s32 a);
s32 sub_0802ef90(s32 x, s32 y, s32 a, s32 b);
void sub_0802f1dc(void);
void sub_0802f2fc(void);
void sub_0802f400(void);
void sub_0802f6c0(void);
void sub_0802f6f4(void);
s32 sub_080301e8(s32 a, s32 x, s32 y);
s32 sub_08030f78(u32 x, u32 y);
void sub_08031738(u32 x, u32 y);
void sub_080b7b7c(s32 a);

s32 sub_080261c0(s32 x, s32 y)
{
    return sub_080301e8(0, x, y);
}

void sub_080261d4(u16 a)
{
    if (a > 7)
        return;
    if (a == 0)
    {
        gUnk_03005670.unk0 = 0;
        gUnk_03005670.unk2 = 0;
        gUnk_03005670.unk4 = 0;
        gUnk_03005670.unk6 = 0;
    }
    else if (gUnk_03005670.unk0 == 0 || gUnk_03005670.unk0 > 4 || gUnk_03005670.unk0 <= a)
    {
        gUnk_03005670.unk0 = a;
        gUnk_03005670.unk2 = 0;
        gUnk_03005670.unk4 = 0;
        gUnk_03005670.unk6 = 0;
    }
}

s32 sub_0802621c(s32 type)
{
    s32 i = 62;

    while (gUnk_03004CA0[i] != -1)
    {
        i--;
        if (i <= 31)
            return -1;
    }
    return sub_080058e4(type, i);
}

void sub_08026264(s32 x, s32 y)
{
    gUnk_030023CC = x;
    gUnk_03002388 = y;
}

void sub_08026278(s32 x, s32 y)
{
    if (gUnk_03002444 != 0)
    {
        if (gUnk_030055C0 != 2 && gUnk_030055C0 != 4)
        {
            gUnk_030023CC = x;
            gUnk_03002388 = y;
            return;
        }
    }
    else if (gUnk_030055C0 != 5)
    {
        gUnk_030023CC = x;
        gUnk_03002388 = y;
        return;
    }
    gUnk_03002398 = x;
    gUnk_03001F00 = y;
}

void sub_080262dc(void)
{
    gUnk_0200D080 = 2;
}

void sub_080262e8(s32 a)
{
    sub_0802d478(gUnk_08732638[a][0], gUnk_08732638[a][1]);
}

void sub_08026308(void)
{
    s32 i;

    if (gUnk_0300566C[0] > 0x2FF && gUnk_03005604[0] > 0x300)
    {
        gUnk_03005614 &= 0x1FFFFFF;
        gUnk_03005604[0] &= 0x1FF;
        gUnk_0300566C[0] &= 0x1FF;
        gUnk_03005690[0] &= 0x1FF;
        gUnk_03005668[0] &= 0x1FF;
        sub_08029930();
        gUnk_03002158[0] &= 0x1FF;
        gUnk_03002158[1] &= 0x1FF;
        gUnk_030055F8[0] &= 0x1FF;
        gUnk_030055F8[1] &= 0x1FF;
        gUnk_020055B8[0] &= 0x1FF;
        gUnk_020055B8[1] &= 0x1FF;
        for (i = 0; i < gUnk_030023AC; i++)
        {
            gUnk_03005640[i].x0 &= 0x1FF;
            gUnk_03005640[i].x1 &= 0x1FF;
        }
        gUnk_030023CC &= 0x1FF;
        gUnk_03002398 &= 0x1FF;
        gUnk_03002348 &= 0x1FF;
        gUnk_02007FA0 &= 31;
        for (i = 0; i < 64; i++)
        {
            if (gUnk_020061F0[i].unk6 != 0xFFFF)
            {
                gUnk_020061F0[i].unk0 &= 31;
                gUnk_020061F0[i].unk4 = gUnk_020061F0[i].unk0 + gUnk_020061F0[i].unk2 * gUnk_03005620;
            }
        }
        for (i = 0; i < 64; i++)
        {
            if (gUnk_03004CA0[i] != -1)
            {
                gUnk_03002790[i].unk4C &= 0x1FFFFFF;
                gUnk_03002790[i].unk48 &= 0x1FF;
            }
        }
    }
}

s32 sub_080264b0(void)
{
    s32 r = -1;
    struct Door *d;

    if (gUnk_02006A20[gUnk_0200B034][0] != -1 && gUnk_0200B034 != -1)
    {
        d = &gUnk_030055EC->unk44[gUnk_0200B034];
        r = sub_0802ed94((d->unk2 << 4) + 16, (d->unk4 << 4) + 8, gUnk_03002790[gUnk_02006A20[gUnk_0200B034][0]].unk20);
    }
    return r;
}

void sub_0802651c(s32 i)
{
    if (i != -1)
        gUnk_03002790[i].unk1C = 1;
}

s32 sub_0802653c(void)
{
    s32 r = -1;
    struct Door *d;

    if (gUnk_0200B034 != -1)
    {
        d = &gUnk_030055EC->unk44[gUnk_0200B034];
        r = sub_0802ef90((d->unk2 << 4) + 16, (d->unk4 << 4) + 8, 3, 0x2000);
    }
    return r;
}

s32 sub_08026584(void)
{
    s32 n = 0;
    struct Door *d;
    struct Door *doors;

    switch (gUnk_0200AF00)
    {
    case 0:
        break;
    case 1:
    case 3:
        if (gUnk_02006A20[gUnk_0200B034][0] == -1 || gUnk_0200B034 == -1)
            break;
        doors = gUnk_030055EC->unk44;
        d = (struct Door *)(gUnk_0200B034 * sizeof(struct Door) + (u32)doors);
        switch (gUnk_03002400[gUnk_030023EC][d->unk8])
        {
        case 0:
            break;
        case 1:
            sub_08006148(sub_0802f6c0, gUnk_02006A20[gUnk_0200B034][0]);
            break;
        case 2:
            sub_080301e8(4, (d->unk2 << 4) + 16, (d->unk4 << 4) + 8);
            n = 1;
            sub_08006148(sub_0802f6f4, gUnk_02006A20[gUnk_0200B034][0]);
            break;
        }
        break;
    case 2:
        if (gUnk_02006A20[gUnk_0200B034][0] == -1 || gUnk_0200B034 == -1)
            break;
        switch (*(u8 *)&gUnk_030055EC->unk44[gUnk_0200B034].unk6)
        {
        case 3:
            sub_08006148(sub_0802f2fc, gUnk_02006A20[gUnk_0200B034][0]);
            break;
        case 4:
            sub_08006148(sub_0802f400, gUnk_02006A20[gUnk_0200B034][0]);
            break;
        case 5:
            sub_08006148(sub_0802f1dc, gUnk_02006A20[gUnk_0200B034][0]);
            break;
        default:
            return;
        }
        sub_080301e8(4, (gUnk_030055EC->unk44[gUnk_0200B034].unk2 << 4) + 16, (gUnk_030055EC->unk44[gUnk_0200B034].unk4 << 4) + 8);
        n++;
        sub_080031b8(0x11B);
        break;
    }
    return n;
}

void sub_08026704(s32 i)
{
    struct Task *t;

    if (i != -1)
    {
        t = &gUnk_03002790[i];
        t->unk42 = 15;
        t->unk3E = 0x4000;
    }
}

s32 sub_0802672c(void)
{
    s32 id = sub_0802d344(5);
    struct Task *t;
    u8 f;

    if (id != -1)
    {
        t = &gUnk_03002790[id];
        f = gUnk_0200AF08 & 16;
        if (f != 0)
            t->unk24 = 1;
        else
            t->unk24 = 0;
        if (gUnk_030023AC == 1)
        {
            gUnk_03002398 = gUnk_030023CC;
            gUnk_03001F00 = gUnk_03002388;
        }
        else
        {
            gUnk_03002398 = gUnk_03002790[gUnk_02007D38].unk48;
            gUnk_03001F00 = gUnk_03002790[gUnk_02007D38].unk4A;
        }
        if (gUnk_03002398 < gUnk_03005628[0])
            gUnk_03002398 = gUnk_03005628[0];
        if (gUnk_03002398 > gUnk_03005628[1])
            gUnk_03002398 = gUnk_03005628[1];
        if (gUnk_03001F00 < gUnk_03005628[2])
            gUnk_03001F00 = gUnk_03005628[2];
        if (gUnk_03001F00 > gUnk_03005628[3])
            gUnk_03001F00 = gUnk_03005628[3];
        sub_0802d074();
        gUnk_020055E8 = 0;
    }
    return id;
}

s32 sub_08026834(void)
{
    s32 id = sub_0802d344(6);
    struct Task *t;
    u8 f;

    if (id != -1)
    {
        t = &gUnk_03002790[id];
        f = gUnk_0200AF08 & 16;
        if (f != 0)
            t->unk24 = 1;
        else
            t->unk24 = 0;
        gUnk_03002398 = gUnk_030023CC = gUnk_020055E0;
        gUnk_03001F00 = gUnk_03002388 = gUnk_0200AEF0;
        if (gUnk_03002398 < gUnk_03005628[0])
            gUnk_03002398 = gUnk_03005628[0];
        if (gUnk_03002398 > gUnk_03005628[1])
            gUnk_03002398 = gUnk_03005628[1];
        if (gUnk_03001F00 < gUnk_03005628[2])
            gUnk_03001F00 = gUnk_03005628[2];
        if (gUnk_03001F00 > gUnk_03005628[3])
            gUnk_03001F00 = gUnk_03005628[3];
        sub_0802d074();
        gUnk_020055E8 = 0;
    }
    return id;
}

void sub_08026900(void)
{
    if (gUnk_030023CC < gUnk_03005628[0])
        gUnk_030023CC = gUnk_03005628[0];
    if (gUnk_030023CC > gUnk_03005628[1])
        gUnk_030023CC = gUnk_03005628[1];
    if (gUnk_03002388 < gUnk_03005628[2])
        gUnk_03002388 = gUnk_03005628[2];
    if (gUnk_03002388 > gUnk_03005628[3])
        gUnk_03002388 = gUnk_03005628[3];
}

void sub_0802695c(void)
{
    gUnk_030023E0 = gUnk_030023B8 + 1;
    gUnk_03002384 = 0;
    gUnk_030023B8 = gUnk_030023E0;
    gUnk_03001F20 = 16;
    gUnk_02007FB0 |= 1;
}

void sub_08026994(void)
{
}

void sub_08026998(void)
{
    if (gUnk_03001F30 == 0)
        sub_080b7b7c(gUnk_030023E8);
    gUnk_02005578 = gUnk_030023B8;
    gUnk_02007FF8 = gUnk_03001F20;
}

void sub_080269d8(u32 x, u32 y)
{
    sub_08031738(x >> 4, y >> 4);
}

void sub_080269e8(void)
{
    s32 i;
    s32 j;

    for (j = 2; j <= 4; j++)
        for (i = 5; i <= 8; i++)
            sub_08030f78(i, j);
}

u32 sub_08026a0c(void)
{
    switch (gUnk_030055C0)
    {
    case 0:
        if (gUnk_03002158[3] > 0x167)
        {
            gUnk_03005680.unkC = 280;
            sub_0802cda0(0xFFFF, 0xFFFF, 200, 360);
        }
        break;
    case 3:
        return 1;
    case 1:
    case 2:
    case 4:
    case 5:
        break;
    }
    return 0;
}

u32 sub_08026a80(void)
{
    switch (gUnk_030055C0)
    {
    case 0:
        if (gUnk_03002158[2] <= 69)
        {
            gUnk_03005680.unkC = 149;
            sub_0802cda0(0xFFFF, 0xFFFF, 16, 176);
        }
        break;
    case 3:
        return 1;
    case 1:
    case 2:
    case 4:
    case 5:
        break;
    }
    return 0;
}

u32 sub_08026aec(void)
{
    switch (gUnk_030055C0)
    {
    case 0:
        if (gUnk_03002158[3] > 0x147)
        {
            gUnk_03005680.unkC = 248;
            sub_0802cda0(0xFFFF, 0xFFFF, 168, 328);
        }
        break;
    case 3:
        return 1;
    case 1:
    case 2:
    case 4:
    case 5:
        break;
    }
    return 0;
}
