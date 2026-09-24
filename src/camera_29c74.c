#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_29c74.c (0x08029C74-0x0802A9CB, issue #86).
 *
 * Multiplayer camera targets.  Player i has a camera position
 * gUnk_030055D0[i], bounds gUnk_03005640[i] and a mode gUnk_0300560C[i]
 * (0 follows its task, 2 glides to the group centre, 3 is parked there);
 * gUnk_030023AC is the player count and gUnk_03002340 the mask of the
 * players present.  sub_08029c74, sub_08029ef4 and sub_0802a340 compute
 * the group centre gUnk_030055F4 (the midpoint of the players' extremes,
 * clamped to the camera bounds gUnk_030055F8), sub_08029e24 re-centres
 * those bounds on it inside the room bounds gUnk_03005628,
 * sub_0802a190/sub_0802a260 update the per-player positions,
 * sub_0802a42c/sub_0802a484 their bounds and sub_0802a4ec/sub_0802a568
 * the visible rectangle gUnk_03002158 around them.  sub_0802a63c moves
 * the bounds once every player has crossed the scroll line held in the
 * camera control block gUnk_03005680.  sub_0802a82c spawns the entries
 * of the room's object list gUnk_020055D8 (sorted along one axis) that
 * lie inside a rectangle, through sub_080b5338. */

struct CamPos { u16 x, y; };

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

struct CamRect { s16 x0, x1, y0, y1; };

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

extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern u16 gUnk_030023AC;
extern s16 gUnk_030055F8[4];
extern u8 gUnk_0300560C[];
extern u16 gUnk_030055F4[2];
extern struct CamPos gUnk_030055D0[4];
extern u8 gUnk_0200B078;
extern s16 gUnk_03005628[4];
extern struct Unk03005680 gUnk_03005680;
extern s32 gUnk_03005610;
extern s32 gUnk_03005664;
extern u8 gUnk_03002340;
extern struct CamRect gUnk_03005640[4];
extern s16 gUnk_03002158[4];
extern struct Unk020055D8 gUnk_020055D8;

s32 sub_080b5338(s32 i);

void sub_08029c74(void)
{
    s32 x0, x1, y0, y1, x, y, cx, cy, i;

    x0 = gUnk_03005620 << 4;
    x1 = 0;
    y0 = gUnk_0300561C << 4;
    y1 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        switch (gUnk_0300560C[i])
        {
        case 0:
            y = gUnk_03002790[i].unk48;
            if (y < gUnk_030055F8[0])
                y = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < y)
                y = gUnk_030055F8[1];
            if (y < x0)
                x0 = y;
            if (x1 < y)
                x1 = y;
            y = gUnk_03002790[i].unk4A;
            if (y < gUnk_030055F8[2])
                y = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < y)
                y = gUnk_030055F8[3];
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 1:
            x = gUnk_030055D0[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gUnk_030055D0[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 2:
            break;
        }
    }
    cx = (x0 + x1) >> 1;
    cy = (y0 + y1) >> 1;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_0300560C[i] == 2)
        {
            if (cx < gUnk_030055D0[i].x)
            {
                gUnk_030055D0[i].x -= 6;
                if (gUnk_030055D0[i].x < cx)
                    gUnk_030055D0[i].x = cx;
            }
            else
            {
                gUnk_030055D0[i].x += 6;
                if (cx < gUnk_030055D0[i].x)
                    gUnk_030055D0[i].x = cx;
            }
            if (cy < gUnk_030055D0[i].y)
            {
                gUnk_030055D0[i].y -= 6;
                if (gUnk_030055D0[i].y < cy)
                    gUnk_030055D0[i].y = cy;
            }
            else
            {
                gUnk_030055D0[i].y += 6;
                if (cy < gUnk_030055D0[i].y)
                    gUnk_030055D0[i].y = cy;
            }
            x = gUnk_030055D0[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gUnk_030055D0[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            if (cx == x && cy == y)
                gUnk_0300560C[i] = 3;
        }
    }
    gUnk_030055F4[0] = (x0 + x1) >> 1;
    gUnk_030055F4[1] = (y0 + y1) >> 1;
}

void sub_08029e24(void)
{
    s32 y, i;

    gUnk_030055F8[0] = gUnk_030055F4[0] - 80;
    gUnk_030055F8[1] = gUnk_030055F4[0] + 80;
    gUnk_030055F8[2] = gUnk_030055F4[1] - 120;
    if (gUnk_0200B078 == 2)
    {
        y = 0;
        for (i = 0; i < gUnk_030023AC; i++)
        {
            if (gUnk_0300560C[i] != 3 && y < gUnk_030055D0[i].y)
                y = gUnk_030055D0[i].y;
        }
        if (y < gUnk_030055F8[3])
            gUnk_030055F8[3] = y;
    }
    else
    {
        gUnk_030055F8[3] = gUnk_030055F4[1] + 120;
    }
    if (gUnk_030055F8[0] < gUnk_03005628[0])
        gUnk_030055F8[0] = gUnk_03005628[0];
    if (gUnk_03005628[1] < gUnk_030055F8[1])
        gUnk_030055F8[1] = gUnk_03005628[1];
    if (gUnk_030055F8[2] < gUnk_03005628[2])
        gUnk_030055F8[2] = gUnk_03005628[2];
    if (gUnk_03005628[3] < gUnk_030055F8[3])
        gUnk_030055F8[3] = gUnk_03005628[3];
}

void sub_08029ef4(void)
{
    s32 x0, x1, y0, y1, x, y, cx, cy, i;
    s16 t;

    x0 = gUnk_03005620 << 4;
    x1 = 0;
    y0 = gUnk_0300561C << 4;
    y1 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        switch (gUnk_0300560C[i])
        {
        case 0:
            if ((gUnk_03005680.unk0 >> i) & 1)
                break;
            x = gUnk_03002790[i].unk48;
            if (x < gUnk_030055F8[0])
                x = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < x)
                x = gUnk_030055F8[1];
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gUnk_03002790[i].unk4A;
            if (y < gUnk_030055F8[2])
                y = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < y)
                y = gUnk_030055F8[3];
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 1:
            x = gUnk_030055D0[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gUnk_030055D0[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        case 2:
            x = gUnk_030055D0[i].x;
            if (x < x0)
                x0 = x;
            if (x1 < x)
                x1 = x;
            y = gUnk_030055D0[i].y;
            if (y < y0)
                y0 = y;
            if (y1 < y)
                y1 = y;
            break;
        }
    }
    if (gUnk_03005680.unk1 & 1)
    {
        if (gUnk_03005610 > 0)
            x1 = gUnk_03005680.unkA;
        else
            x0 = gUnk_03005680.unkA;
    }
    if (gUnk_03005680.unk1 & 2)
    {
        if (gUnk_03005664 > 0)
            y1 = gUnk_03005680.unkC;
        else
            y0 = gUnk_03005680.unkC;
    }
    cx = (x0 + x1) >> 1;
    cy = (y0 + y1) >> 1;
    if (!((gUnk_03005680.unk0 >> gUnk_030023AC) & 1) || !(gUnk_03005680.unk1 & 1))
    {
        gUnk_030055F8[0] = t = cx - 80;
        gUnk_030055F8[1] = cx + 80;
        if (t < gUnk_03005628[0])
            gUnk_030055F8[0] = gUnk_03005628[0];
        if (gUnk_03005628[1] < gUnk_030055F8[1])
            gUnk_030055F8[1] = gUnk_03005628[1];
    }
    if (!((gUnk_03005680.unk0 >> gUnk_030023AC) & 1) || !(gUnk_03005680.unk1 & 2))
    {
        gUnk_030055F8[2] = cy - 120;
        if (gUnk_0200B078 == 2)
        {
            y1 = 0;
            for (i = 0; i < gUnk_030023AC; i++)
            {
                if (gUnk_0300560C[i] != 3 && !(((gUnk_03005680.unk0 & gUnk_03002340) >> i) & 1)
                    && y1 < gUnk_030055D0[i].y)
                    y1 = gUnk_030055D0[i].y;
            }
            if (y1 < gUnk_030055F8[3])
                gUnk_030055F8[3] = y1;
        }
        else
        {
            gUnk_030055F8[3] = cy + 120;
        }
        if (gUnk_030055F8[2] < gUnk_03005628[2])
            gUnk_030055F8[2] = gUnk_03005628[2];
        if (gUnk_03005628[3] < gUnk_030055F8[3])
            gUnk_030055F8[3] = gUnk_03005628[3];
    }
}

void sub_0802a190(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        switch (gUnk_0300560C[i])
        {
        case 0:
            gUnk_030055D0[i].x = gUnk_03002790[i].unk48;
            if (gUnk_030055D0[i].x < gUnk_030055F8[0])
                gUnk_030055D0[i].x = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < gUnk_030055D0[i].x)
                gUnk_030055D0[i].x = gUnk_030055F8[1];
            gUnk_030055D0[i].y = gUnk_03002790[i].unk4A;
            if (gUnk_030055D0[i].y < gUnk_030055F8[2])
                gUnk_030055D0[i].y = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < gUnk_030055D0[i].y)
                gUnk_030055D0[i].y = gUnk_030055F8[3];
            break;
        case 1:
        case 2:
            break;
        case 3:
            gUnk_030055D0[i].x = gUnk_030055F4[0];
            gUnk_030055D0[i].y = gUnk_030055F4[1];
            break;
        }
    }
}

void sub_0802a260(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        switch (gUnk_0300560C[i])
        {
        case 0:
            if ((gUnk_03005680.unk0 >> i) & 1)
                break;
            gUnk_030055D0[i].x = gUnk_03002790[i].unk48;
            if (gUnk_030055D0[i].x < gUnk_030055F8[0])
                gUnk_030055D0[i].x = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < gUnk_030055D0[i].x)
                gUnk_030055D0[i].x = gUnk_030055F8[1];
            gUnk_030055D0[i].y = gUnk_03002790[i].unk4A;
            if (gUnk_030055D0[i].y < gUnk_030055F8[2])
                gUnk_030055D0[i].y = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < gUnk_030055D0[i].y)
                gUnk_030055D0[i].y = gUnk_030055F8[3];
            break;
        case 1:
        case 2:
            break;
        case 3:
            gUnk_030055D0[i].x = gUnk_030055F4[0];
            gUnk_030055D0[i].y = gUnk_030055F4[1];
            break;
        }
    }
}

void sub_0802a340(void)
{
    s32 x0, x1, y0, y1, v, i;

    x0 = gUnk_03005620 << 4;
    x1 = 0;
    y0 = gUnk_0300561C << 4;
    y1 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            v = gUnk_03002790[i].unk48;
            if (v < gUnk_030055F8[0])
                v = gUnk_030055F8[0];
            if (gUnk_030055F8[1] < v)
                v = gUnk_030055F8[1];
            if (v < x0)
                x0 = v;
            if (x1 < v)
                x1 = v;
            v = gUnk_03002790[i].unk4A;
            if (v < gUnk_030055F8[2])
                v = gUnk_030055F8[2];
            if (gUnk_030055F8[3] < v)
                v = gUnk_030055F8[3];
            if (v < y0)
                y0 = v;
            if (y1 < v)
                y1 = v;
        }
    }
    gUnk_030055F4[0] = (x0 + x1) >> 1;
    gUnk_030055F4[1] = (y0 + y1) >> 1;
}

void sub_0802a42c(void)
{
    s32 i;

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

void sub_0802a484(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (((gUnk_03002340 >> i) & 1) && !((gUnk_03005680.unk0 >> i) & 1))
        {
            gUnk_03005640[i].x0 = gUnk_030055F8[0] - 117;
            gUnk_03005640[i].x1 = gUnk_030055F8[1] + 117;
            gUnk_03005640[i].y0 = gUnk_030055F8[2] - 76;
            gUnk_03005640[i].y1 = gUnk_030055F8[3] + 104;
        }
    }
}

void sub_0802a4ec(void)
{
    s32 x0, x1, y0, y1, v, i;

    x0 = gUnk_03005620 << 4;
    x1 = 0;
    y0 = gUnk_0300561C << 4;
    y1 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        v = gUnk_030055D0[i].x;
        if (v < x0)
            x0 = v;
        if (x1 < v)
            x1 = v;
        v = gUnk_030055D0[i].y;
        if (v < y0)
            y0 = v;
        if (y1 < v)
            y1 = v;
    }
    gUnk_03002158[0] = x0 - 120;
    gUnk_03002158[1] = x1 + 120;
    gUnk_03002158[2] = y0 - 80;
    gUnk_03002158[3] = y1 + 80;
}

void sub_0802a568(void)
{
    s32 x0, x1, y0, y1, v, i;

    x0 = gUnk_03005620 << 4;
    x1 = 0;
    y0 = gUnk_0300561C << 4;
    y1 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        v = gUnk_030055D0[i].x;
        if (v < x0)
            x0 = v;
        if (x1 < v)
            x1 = v;
        v = gUnk_030055D0[i].y;
        if (v < y0)
            y0 = v;
        if (y1 < v)
            y1 = v;
    }
    if (gUnk_03005680.unk1 & 1)
    {
        v = gUnk_030055D0[gUnk_030023AC].x;
        if (v < x0)
            x0 = v;
        if (x1 < v)
            x1 = v;
    }
    if (gUnk_03005680.unk1 & 2)
    {
        v = gUnk_030055D0[gUnk_030023AC].y;
        if (v < y0)
            y0 = v;
        if (y1 < v)
            y1 = v;
    }
    gUnk_03002158[0] = x0 - 120;
    gUnk_03002158[1] = x1 + 120;
    gUnk_03002158[2] = y0 - 80;
    gUnk_03002158[3] = y1 + 80;
}

void sub_0802a63c(void)
{
    s32 i;

    if ((gUnk_03005680.unk0 >> gUnk_030023AC) & 1)
        return;
    if (gUnk_03005680.unk1 & 1)
    {
        for (i = 0; i < gUnk_030023AC; i++)
        {
            if (((gUnk_03002340 >> i) & 1) && !((gUnk_03005680.unk0 >> i) & 1))
            {
                if (gUnk_03005610 > 0 && gUnk_03005680.unkA <= gUnk_030055D0[i].x)
                {
                    gUnk_03005680.unk0 |= 1 << i;
                    gUnk_03005640[i].x0 = gUnk_030055D0[i].x + 0xFF8B;
                    gUnk_03005640[i].x1 = gUnk_03005680.unk4 + 117;
                }
                else if (gUnk_03005610 < 0 && gUnk_030055D0[i].x <= gUnk_03005680.unkA)
                {
                    gUnk_03005680.unk0 |= 1 << i;
                    gUnk_03005640[i].x0 = gUnk_03005680.unk2 + 0xFF8B;
                    gUnk_03005640[i].x1 = gUnk_030055D0[i].x + 117;
                }
            }
        }
    }
    if (gUnk_03005680.unk1 & 2)
    {
        for (i = 0; i < gUnk_030023AC; i++)
        {
            if (((gUnk_03002340 >> i) & 1) && !((gUnk_03005680.unk0 >> i) & 1))
            {
                if (gUnk_03005664 > 0 && gUnk_03005680.unkC <= gUnk_030055D0[i].y)
                {
                    gUnk_03005680.unk0 |= 1 << i;
                    gUnk_03005640[i].y0 = gUnk_030055D0[i].y + 0xFFB4;
                    gUnk_03005640[i].y1 = gUnk_03005680.unk8 + 104;
                }
                else if (gUnk_03005664 < 0 && gUnk_030055D0[i].y <= gUnk_03005680.unkC)
                {
                    gUnk_03005680.unk0 |= 1 << i;
                    gUnk_03005640[i].y0 = gUnk_03005680.unk6 + 0xFFB4;
                    gUnk_03005640[i].y1 = gUnk_030055D0[i].y + 104;
                }
            }
        }
    }
    if ((gUnk_03005680.unk0 & gUnk_03002340) == gUnk_03002340)
    {
        gUnk_03005680.unk0 |= 1 << gUnk_030023AC;
        if (gUnk_03005680.unk1 & 1)
        {
            if (gUnk_03005610 > 0)
                gUnk_030055F8[0] = gUnk_03005680.unkA;
            else
                gUnk_030055F8[1] = gUnk_03005680.unkA;
        }
        if (gUnk_03005680.unk1 & 2)
        {
            if (gUnk_03005664 > 0)
                gUnk_030055F8[2] = gUnk_03005680.unkC;
            else
                gUnk_030055F8[3] = gUnk_03005680.unkC;
        }
    }
}

void sub_0802a82c(s32 x0, s32 x1, s32 y0, s32 y1)
{
    s32 i;
    struct Unk020055D8Entry *e;

    if (x0 < 0)
        x0 = 0;
    if (x1 < 0)
        x1 = 0;
    if (x0 > gUnk_03005620 << 4)
        x0 = gUnk_03005620 << 4;
    if (x1 > gUnk_03005620 << 4)
        x1 = gUnk_03005620 << 4;
    if (y0 < 0)
        y0 = 0;
    if (y1 < 0)
        y1 = 0;
    if (y0 > gUnk_0300561C << 4)
        y0 = gUnk_0300561C << 4;
    if (y1 > gUnk_0300561C << 4)
        y1 = gUnk_0300561C << 4;
    if (gUnk_020055D8.unk2 != 0)
    {
        if ((y1 + y0) >> 1 >= gUnk_0300561C << 3)
        {
            for (i = gUnk_020055D8.unk0 - 1; i >= 0; i--)
            {
                e = &gUnk_020055D8.unk4[i];
                if (e->unk6 >= y1)
                    continue;
                if (e->unk6 < y0)
                    break;
                if (e->unk4 >= x0 && e->unk4 < x1)
                    sub_080b5338(i);
            }
        }
        else
        {
            for (i = 0; i < gUnk_020055D8.unk0; i++)
            {
                e = &gUnk_020055D8.unk4[i];
                if (e->unk6 < y0)
                    continue;
                if (e->unk6 >= y1)
                    break;
                if (e->unk4 >= x0 && e->unk4 < x1)
                    sub_080b5338(i);
            }
        }
    }
    else
    {
        if ((x1 + x0) >> 1 >= gUnk_03005620 << 3)
        {
            for (i = gUnk_020055D8.unk0 - 1; i >= 0; i--)
            {
                e = &gUnk_020055D8.unk4[i];
                if (e->unk4 >= x1)
                    continue;
                if (e->unk4 < x0)
                    break;
                if (e->unk6 >= y0 && e->unk6 < y1)
                    sub_080b5338(i);
            }
        }
        else
        {
            for (i = 0; i < gUnk_020055D8.unk0; i++)
            {
                e = &gUnk_020055D8.unk4[i];
                if (e->unk4 < x0)
                    continue;
                if (e->unk4 >= x1)
                    break;
                if (e->unk6 >= y0 && e->unk6 < y1)
                    sub_080b5338(i);
            }
        }
    }
}
