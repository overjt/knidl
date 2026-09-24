#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_296a0.c (0x080296A0-0x08029C73, issue #86).
 *
 * Per-frame camera -> BG glue.  gUnk_03005604 is the camera position in
 * pixels and gUnk_0300566C the position the BG maps were last streamed at.
 * sub_080296a0, sub_08029708, sub_080297dc and sub_080298c4 compare the two
 * in 8-pixel tiles and stream the tile columns and rows that scrolled into
 * view (bgmap_2a9cc.c), one per room layout; sub_08029b30 streams the whole
 * view.  sub_08029930, sub_080299e8, sub_08029a4c and sub_08029ab4 write
 * the 16.16 BG scroll shadows and the sprite camera
 * gUnk_03002348/gUnk_030023E4 from the camera plus the screen-shake offset
 * gUnk_03005670 (BG3 follows gUnk_03005690, at half the shake unless
 * gUnk_02000000 is set).  sub_08029bb8 calls sub_0802a82c for every edge of
 * the visible rectangle gUnk_03002158 that moved past the previous one,
 * gUnk_02004B78, while gUnk_020055D8 is set, then remembers it. */

struct Unk03005670
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 filler01;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ u8 unk6;
};

struct Unk03004B00
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u32 unk4;
    /*0x08*/ u32 unk8;
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

extern u16 gUnk_03005604[2];
extern u16 gUnk_0300566C[2];
extern s32 gUnk_03001F2C;
extern s32 gUnk_03002448;
extern struct Unk03005670 gUnk_03005670;
extern u16 gUnk_03005690[2];
extern s8 gUnk_02000000;
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03001E94;
extern s32 gUnk_02016C30;
extern vs32 gUnk_03001EE0;
extern vs32 gUnk_03000FC0;
extern struct Unk03004B00 gUnk_03004B00;
extern struct Unk020055D8 gUnk_020055D8;
extern s16 gUnk_02004B78[4];
extern s16 gUnk_03002158[4];

void sub_08023f18(void);
void sub_0802a82c(s32 x0, s32 x1, s32 y0, s32 y1);
void sub_0802aae8(s32 x);
void sub_0802acbc(s32 x0, s32 x1, s32 y);
void sub_0802ad1c(s32 x, s32 y0, s32 y1);
void sub_0802ae00(s32 x0, s32 x1, s32 y);
void sub_0802ae58(s32 x, s32 y0, s32 y1);
void sub_0802af1c(s32 y);
void sub_0802b25c(s32 x);

void sub_080296a0(void)
{
    s32 x, x0, x1, d;

    x = gUnk_03005604[0] >> 3;
    x0 = x - 1;
    x1 = x + 30;
    d = x - (gUnk_0300566C[0] >> 3);
    if (d > 0)
    {
        gUnk_03001F2C = x0 - 8;
        gUnk_03002448 = x + 38;
        if (!(x1 & 1))
        {
            sub_0802b25c(gUnk_03001F2C >> 1);
            sub_0802b25c(gUnk_03002448 >> 1);
        }
        sub_0802aae8(x1);
        gUnk_0300566C[0] = gUnk_03005604[0];
    }
    else if (d < 0)
    {
        sub_0802aae8(x0);
        gUnk_0300566C[0] = gUnk_03005604[0];
    }
}

void sub_08029708(void)
{
    s32 x, x0, x1, y, y0, y1, d, i;

    x = gUnk_03005604[0] >> 3;
    x0 = x - 1;
    x1 = x + 30;
    y = gUnk_03005604[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = x - (gUnk_0300566C[0] >> 3);
    if (d > 0)
    {
        sub_0802ad1c(x1, y0, y1);
        gUnk_0300566C[0] = gUnk_03005604[0];
    }
    else if (d < 0)
    {
        sub_0802ad1c(x0, y0, y1);
        gUnk_0300566C[0] = gUnk_03005604[0];
    }
    d = (gUnk_03005604[1] >> 3) - (gUnk_0300566C[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802acbc(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802acbc(x0, x1, y0 + i);
        }
        gUnk_0300566C[1] = gUnk_03005604[1];
    }
}

void sub_080297dc(void)
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
                sub_0802ae58(x1 - i, y0, y1);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802ae58(x0 + i, y0, y1);
        }
        gUnk_0300566C[0] = gUnk_03005604[0];
    }
    d = (gUnk_03005604[1] >> 3) - (gUnk_0300566C[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802ae00(x0, x1, y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802ae00(x0, x1, y0 + i);
        }
        gUnk_0300566C[1] = gUnk_03005604[1];
    }
}

void sub_080298c4(void)
{
    s32 y, y0, y1, d, i;

    y = gUnk_03005604[1] >> 3;
    y0 = y - 3;
    y1 = y + 22;
    d = y - (gUnk_0300566C[1] >> 3);
    if (d != 0)
    {
        if (d > 0)
        {
            for (i = 0; i < d; i++)
                sub_0802af1c(y1 - i);
        }
        else
        {
            for (i = 0; i < abs(d); i++)
                sub_0802af1c(y0 + i);
        }
        gUnk_0300566C[1] = gUnk_03005604[1];
    }
}

void sub_08029930(void)
{
    gUnk_03000F8C = (gUnk_03005604[0] + gUnk_03005670.unk2) << 16;
    gUnk_03001E94 = (gUnk_03005604[1] + gUnk_03005670.unk4) << 16;
    if (gUnk_02000000 != 0)
    {
        gUnk_03000B78 = (gUnk_03005690[0] + gUnk_03005670.unk2) << 16;
        gUnk_03000FA8 = (gUnk_03005690[1] + gUnk_03005670.unk4) << 16;
    }
    else
    {
        gUnk_03000B78 = (gUnk_03005690[0] + (gUnk_03005670.unk2 >> 1)) << 16;
        gUnk_03000FA8 = gUnk_03005690[1] << 16;
    }
    gUnk_03002348 = gUnk_03005604[0] + gUnk_03005670.unk2;
    gUnk_030023E4 = gUnk_03005604[1] + gUnk_03005670.unk4;
}

void sub_080299e8(void)
{
    gUnk_03001E94 = (gUnk_03005604[1] + gUnk_03005670.unk4) << 16;
    gUnk_03000FA8 = (gUnk_03005690[1] + gUnk_03005670.unk4) << 16;
    gUnk_03002348 = gUnk_03005604[0] + gUnk_03005670.unk2;
    gUnk_030023E4 = gUnk_03005604[1] + gUnk_03005670.unk4;
    gUnk_02016C30 = gUnk_03005604[0];
}

void sub_08029a4c(void)
{
    gUnk_03000B78 = gUnk_03000F8C = (gUnk_03005604[0] + gUnk_03005670.unk2) << 16;
    gUnk_03000FA8 = gUnk_03001E94 = (gUnk_03005604[1] + gUnk_03005670.unk4) << 16;
    gUnk_03002348 = gUnk_03005604[0] + gUnk_03005670.unk2;
    gUnk_030023E4 = gUnk_03005604[1] + gUnk_03005670.unk4;
}

void sub_08029ab4(void)
{
    gUnk_03001EE0 = gUnk_03000B78 = gUnk_03000F8C = (gUnk_03005604[0] + gUnk_03005670.unk2) << 16;
    gUnk_03000FC0 = gUnk_03000FA8 = gUnk_03001E94 = (gUnk_03005604[1] + gUnk_03005670.unk4) << 16;
    gUnk_03002348 = gUnk_03005604[0] + gUnk_03005670.unk2;
    gUnk_030023E4 = gUnk_03005604[1] + gUnk_03005670.unk4;
}

void sub_08029b30(void)
{
    s32 x, x1, y0, y1;

    y0 = (gUnk_03005604[1] >> 3) - 3;
    y1 = (gUnk_03005604[1] >> 3) + 22;
    x1 = (gUnk_03005604[0] >> 3) - 1;
    for (x = (gUnk_03005604[0] >> 3) - 3; x <= x1; x++)
        sub_0802ae58(x, y0, y1);
    x1 = (gUnk_03005604[0] >> 3) + 32;
    for (x = (gUnk_03005604[0] >> 3) + 30; x <= x1; x++)
        sub_0802ae58(x, y0, y1);
    gUnk_0300566C[0] = gUnk_03005604[0];
    gUnk_0300566C[1] = gUnk_03005604[1];
    gUnk_03004B00.unk8 = (u32)sub_08023f18;
    gUnk_03001EE0 = gUnk_03000FC0 = 0;
}

void sub_08029bb8(void)
{
    if (gUnk_020055D8.unk0 != 0)
    {
        if (gUnk_03002158[0] < gUnk_02004B78[0])
            sub_0802a82c(gUnk_02004B78[0] - 36, gUnk_02004B78[0] - 28, gUnk_02004B78[2] - 40, gUnk_02004B78[3] + 40);
        if (gUnk_02004B78[1] < gUnk_03002158[1])
            sub_0802a82c(gUnk_02004B78[1] + 28, gUnk_02004B78[1] + 36, gUnk_02004B78[2] - 40, gUnk_02004B78[3] + 40);
        if (gUnk_03002158[2] < gUnk_02004B78[2])
            sub_0802a82c(gUnk_02004B78[0] - 36, gUnk_02004B78[1] + 36, gUnk_02004B78[2] - 40, gUnk_02004B78[2] - 32);
        if (gUnk_02004B78[3] < gUnk_03002158[3])
            sub_0802a82c(gUnk_02004B78[0] - 36, gUnk_02004B78[1] + 36, gUnk_02004B78[3] + 32, gUnk_02004B78[3] + 40);
        *(long long *)gUnk_02004B78 = *(long long *)gUnk_03002158;
    }
}
