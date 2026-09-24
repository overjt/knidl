#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_2c42c.c (0x0802C42C-0x0802D01B, issue #86).
 *
 * Camera modes, the screen shake and the scroll-lock setup.
 * sub_0802c42c, sub_0802cab0 and sub_0802cbb8 snap the camera, and the
 * per-player cameras and bounds, to the spawn point
 * gUnk_03002398/gUnk_03001F00 clamped to the room bounds gUnk_03005628
 * (when gUnk_03002350 is set).  sub_0802c550, sub_0802c680, sub_0802c7f4
 * and sub_0802cc90 follow the player (gUnk_030023CC/gUnk_03002388, the
 * task gUnk_02007D38 or the multiplayer group of camera_29c74.c) inside
 * the bounds gUnk_030055F8 and write the 16.16 target
 * gUnk_03005614/gUnk_03005634 and the visible rectangle gUnk_03002158.
 * sub_0802cd24 clears and sub_0802cd38 steps the screen shake
 * gUnk_03005670 through the offset lists of gUnk_08732880 (0x8000 ends a
 * list, 0x9999 restarts it).  sub_0802cda0(x0, x1, y0, y1) arms the scroll
 * lock of gUnk_03005680 for a room region, 0xFFFF meaning no limit on
 * that axis (also called from M33, src/hud_b5840.c). */

struct CamPos { u16 x, y; };

struct CamRect { s16 x0, x1, y0, y1; };

struct Unk03005670
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 filler01;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ u8 unk6;
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

extern u8 gUnk_03002350;
extern s16 gUnk_03002398;
extern s16 gUnk_03001F00;
extern s16 gUnk_03005628[4];
extern s16 gUnk_030055F8[4];
extern u16 gUnk_030023AC;
extern s16 gUnk_03002158[4];
extern struct CamRect gUnk_03005640[4];
extern struct CamPos gUnk_030055D0[4];
extern s32 gUnk_03005614;
extern s32 gUnk_03005634;
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;
extern u16 gUnk_03002360;
extern u8 gUnk_03002340;
extern u8 gUnk_02007D38;
extern u8 gUnk_0300560C[];
extern u16 gUnk_030055F4[2];
extern struct Unk03005670 gUnk_03005670;
extern u16 *gUnk_08732880[];
extern struct Unk03005680 gUnk_03005680;
extern s32 gUnk_03005610;
extern s32 gUnk_03005664;
extern u16 gUnk_030055C0;

void sub_08029c74(void);
void sub_0802a190(void);
void sub_0802a340(void);
void sub_0802a4ec(void);
void sub_0802a63c(void);

void sub_0802c42c(void)
{
    s32 x, y, i;

    if (gUnk_03002350)
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

void sub_0802c550(void)
{
    s32 x, y;

    if (gUnk_030023AC == 1)
    {
        struct CamPos *c;

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
        c = gUnk_030055D0;
        c[gUnk_03002360].x = x;
        c[gUnk_03002360].y = y;
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        gUnk_03002158[0] = x - 120;
        gUnk_03002158[1] = x - 120 + 240;
        gUnk_03002158[2] = y - 80;
        gUnk_03002158[3] = y - 80 + 160;
    }
    else
    {
        sub_08029c74();
        sub_0802a190();
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
            struct CamPos *c = gUnk_030055D0;

            x = c[gUnk_03002360].x;
            y = c[gUnk_03002360].y;
        }
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        sub_0802a4ec();
    }
}

void sub_0802c680(void)
{
    s32 x, y;

    if (gUnk_030023AC == 1)
    {
        struct CamPos *c;

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
        c = gUnk_030055D0;
        c[gUnk_03002360].x = x;
        c[gUnk_03002360].y = y;
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        gUnk_03002158[0] = x - 120;
        gUnk_03002158[1] = x - 120 + 240;
        gUnk_03002158[2] = y - 80;
        gUnk_03002158[3] = y - 80 + 160;
    }
    else
    {
        s32 i;

        x = gUnk_03002790[gUnk_02007D38].unk48;
        y = gUnk_03002790[gUnk_02007D38].unk4A;
        if (x < gUnk_030055F8[0])
            x = gUnk_030055F8[0];
        if (x > gUnk_030055F8[1])
            x = gUnk_030055F8[1];
        if (y < gUnk_030055F8[2])
            y = gUnk_030055F8[2];
        if (y > gUnk_030055F8[3])
            y = gUnk_030055F8[3];
        /* indexed as a u16[][2] here: the struct spelling (.x/.y) gives the
           loop one address giv and reorders the hoisted rectangle sums */
        for (i = 0; i < gUnk_030023AC; i++)
        {
            ((u16 (*)[2])gUnk_030055D0)[i][0] = x;
            ((u16 (*)[2])gUnk_030055D0)[i][1] = y;
        }
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        gUnk_03002158[0] = x - 120;
        gUnk_03002158[1] = x + 120;
        gUnk_03002158[2] = y - 80;
        gUnk_03002158[3] = y + 80;
    }
}

void sub_0802c7f4(void)
{
    s32 x, y;

    if (gUnk_030023AC == 1)
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
        if (gUnk_0300560C[gUnk_03002360] == 2)
        {
            if (x == gUnk_030055D0[gUnk_03002360].x && y == gUnk_030055D0[gUnk_03002360].y)
            {
                gUnk_0300560C[gUnk_03002360] = 0;
            }
            else
            {
                if (x < gUnk_030055D0[gUnk_03002360].x)
                {
                    gUnk_030055D0[gUnk_03002360].x -= 3;
                    if (gUnk_030055D0[gUnk_03002360].x < x)
                        gUnk_030055D0[gUnk_03002360].x = x;
                }
                else
                {
                    gUnk_030055D0[gUnk_03002360].x += 3;
                    if (x < gUnk_030055D0[gUnk_03002360].x)
                        gUnk_030055D0[gUnk_03002360].x = x;
                }
                if (y < gUnk_030055D0[gUnk_03002360].y)
                {
                    gUnk_030055D0[gUnk_03002360].y -= 3;
                    if (gUnk_030055D0[gUnk_03002360].y < y)
                        gUnk_030055D0[gUnk_03002360].y = y;
                }
                else
                {
                    gUnk_030055D0[gUnk_03002360].y += 3;
                    if (y < gUnk_030055D0[gUnk_03002360].y)
                        gUnk_030055D0[gUnk_03002360].y = y;
                }
            }
            x = gUnk_030055D0[gUnk_03002360].x;
            y = gUnk_030055D0[gUnk_03002360].y;
        }
        else
        {
            gUnk_030055D0[gUnk_03002360].x = x;
            gUnk_030055D0[gUnk_03002360].y = y;
        }
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        gUnk_03002158[0] = x - 120;
        gUnk_03002158[1] = x - 120 + 240;
        gUnk_03002158[2] = y - 80;
        gUnk_03002158[3] = y - 80 + 160;
    }
    else
    {
        s32 i;

        sub_0802a340();
        for (i = 0; i < gUnk_030023AC; i++)
        {
            if ((gUnk_03002340 >> i) & 1)
            {
                x = gUnk_03002790[i].unk48;
                y = gUnk_03002790[i].unk4A;
                if (x < gUnk_030055F8[0])
                    x = gUnk_030055F8[0];
                if (gUnk_030055F8[1] < x)
                    x = gUnk_030055F8[1];
                if (y < gUnk_030055F8[2])
                    y = gUnk_030055F8[2];
                if (gUnk_030055F8[3] < y)
                    y = gUnk_030055F8[3];
                if (gUnk_0300560C[i] == 2)
                {
                    if (x == gUnk_030055D0[i].x && y == gUnk_030055D0[i].y)
                    {
                        gUnk_0300560C[i] = 0;
                    }
                    else
                    {
                        if (x < gUnk_030055D0[i].x)
                        {
                            gUnk_030055D0[i].x -= 3;
                            if (gUnk_030055D0[i].x < x)
                                gUnk_030055D0[i].x = x;
                        }
                        else
                        {
                            gUnk_030055D0[i].x += 3;
                            if (x < gUnk_030055D0[i].x)
                                gUnk_030055D0[i].x = x;
                        }
                        if (y < gUnk_030055D0[i].y)
                        {
                            gUnk_030055D0[i].y -= 3;
                            if (gUnk_030055D0[i].y < y)
                                gUnk_030055D0[i].y = y;
                        }
                        else
                        {
                            gUnk_030055D0[i].y += 3;
                            if (y < gUnk_030055D0[i].y)
                                gUnk_030055D0[i].y = y;
                        }
                    }
                }
                else
                {
                    gUnk_030055D0[i].x = x;
                    gUnk_030055D0[i].y = y;
                }
            }
            else
            {
                x = gUnk_030055F4[0];
                if (x < gUnk_030055F8[0])
                    x = gUnk_030055F8[0];
                if (gUnk_030055F8[1] < x)
                    x = gUnk_030055F8[1];
                if (gUnk_0300560C[i] == 2)
                {
                    if (x == gUnk_030055D0[i].x)
                    {
                        gUnk_0300560C[i] = 3;
                    }
                    else if (x < gUnk_030055D0[i].x)
                    {
                        gUnk_030055D0[i].x -= 3;
                        if (gUnk_030055D0[i].x < x)
                            gUnk_030055D0[i].x = x;
                    }
                    else
                    {
                        gUnk_030055D0[i].x += 3;
                        if (x < gUnk_030055D0[i].x)
                            gUnk_030055D0[i].x = x;
                    }
                }
                else
                {
                    gUnk_030055D0[i].x = x;
                }
            }
        }
        x = gUnk_030055D0[gUnk_03002360].x;
        y = gUnk_030055D0[gUnk_03002360].y;
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        sub_0802a4ec();
    }
}

void sub_0802cab0(void)
{
    s32 x, y, i;

    if (gUnk_03002350)
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
            struct CamRect *p = &gUnk_03005640[i];
            p->x0 = x - 117;
            p->x1 = x + 117;
            p->y0 = y - 76;
            p->y1 = y + 104;
        }
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        gUnk_03002158[0] = x - 120;
        gUnk_03002158[1] = x + 120;
        gUnk_03002158[2] = y - 80;
        gUnk_03002158[3] = y + 80;
    }
}

void sub_0802cbb8(void)
{
    s32 x, y, i;

    if (gUnk_03002350)
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
        for (i = 0; i < gUnk_030023AC; i++)
        {
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

void sub_0802cc90(void)
{
    s32 x = gUnk_030023CC;
    s32 y = gUnk_03002388;
    s32 t;
    struct CamPos *c;

    if (x < gUnk_030055F8[0])
        x = gUnk_030055F8[0];
    if (x > gUnk_030055F8[1])
        x = gUnk_030055F8[1];
    if (y < gUnk_030055F8[2])
        y = gUnk_030055F8[2];
    if (y > gUnk_030055F8[3])
        y = gUnk_030055F8[3];
    c = gUnk_030055D0;
    c[gUnk_03002360].x = x;
    c[gUnk_03002360].y = y;
    gUnk_03005614 = x << 16;
    gUnk_03005634 = y << 16;
    gUnk_03002158[0] = x - 120;
    gUnk_03002158[1] = x - 120 + 240;
    gUnk_03002158[2] = y - 80;
    gUnk_03002158[3] = y - 80 + 160;
}

void sub_0802cd24(void)
{
    gUnk_03005670.unk0 = 0;
    gUnk_03005670.unk2 = 0;
    gUnk_03005670.unk4 = 0;
    gUnk_03005670.unk6 = 0;
}

void sub_0802cd38(void)
{
    u16 *p;

    if (gUnk_03005670.unk0 != 0)
    {
        p = gUnk_08732880[gUnk_03005670.unk0];
        switch (p[gUnk_03005670.unk6 * 2])
        {
        case 0x8000:
            gUnk_03005670.unk0 = 0;
            gUnk_03005670.unk2 = 0;
            gUnk_03005670.unk4 = 0;
            gUnk_03005670.unk6 = 0;
            return;
        case 0x9999:
            gUnk_03005670.unk6 = 0;
        }
        gUnk_03005670.unk2 = p[gUnk_03005670.unk6 * 2];
        gUnk_03005670.unk4 = p[gUnk_03005670.unk6 * 2 + 1];
        gUnk_03005670.unk6++;
    }
}

void sub_0802cda0(s32 x0, s32 x1, s32 y0, s32 y1)
{
    u8 flags = 0;
    s32 mid;

    if (x0 != 0xFFFF && x1 != 0xFFFF)
    {
        gUnk_03005680.unk2 = x0 + 120;
        gUnk_03005680.unk4 = x1 - 120;
        if (gUnk_03005680.unk2 < gUnk_03005628[0])
            gUnk_03005680.unk2 = gUnk_03005628[0];
        if (gUnk_03005628[1] < gUnk_03005680.unk4)
            gUnk_03005680.unk4 = gUnk_03005628[1];
        flags = 1;
        mid = (gUnk_03005680.unk2 + gUnk_03005680.unk4) >> 1;
        if (gUnk_03005680.unkA <= mid)
        {
            gUnk_03005610 = 2;
            gUnk_030055F8[1] = gUnk_03005680.unk4;
        }
        else
        {
            gUnk_03005610 = -2;
            gUnk_030055F8[0] = gUnk_03005680.unk2;
        }
    }
    else
    {
        gUnk_03005680.unk2 = 0xFFFF;
        gUnk_03005680.unk4 = -1;
        gUnk_03005610 = 0;
    }
    if (y0 != 0xFFFF && y1 != 0xFFFF)
    {
        gUnk_03005680.unk6 = y0 + 80;
        gUnk_03005680.unk8 = y1 - 80;
        if (gUnk_03005680.unk6 < gUnk_03005628[2])
            gUnk_03005680.unk6 = gUnk_03005628[2];
        if (gUnk_03005680.unk8 > gUnk_03005628[3])
            gUnk_03005680.unk8 = gUnk_03005628[3];
        flags |= 2;
        mid = (gUnk_03005680.unk6 + gUnk_03005680.unk8) >> 1;
        if (gUnk_03005680.unkC <= mid)
        {
            gUnk_03005664 = 1;
            gUnk_030055F8[3] = gUnk_03005680.unk8;
        }
        else
        {
            gUnk_03005664 = -1;
            gUnk_030055F8[2] = gUnk_03005680.unk6;
        }
    }
    else
    {
        gUnk_03005680.unk6 = 0xFFFF;
        gUnk_03005680.unk8 = -1;
        gUnk_03005664 = 0;
    }
    if (flags != 0)
    {
        gUnk_030055C0 = 2;
        gUnk_03005680.unk1 = flags;
        gUnk_03005680.unk0 = 0;
        if (gUnk_030023AC == 1)
        {
            if (flags & 1)
            {
                struct CamRect *b;

                if (gUnk_03005610 > 0)
                {
                    gUnk_030055F8[0] = gUnk_030055D0[gUnk_03002360].x;
                    gUnk_030055F8[1] = gUnk_03005680.unk4;
                }
                else
                {
                    gUnk_030055F8[0] = gUnk_03005680.unk2;
                    gUnk_030055F8[1] = gUnk_030055D0[gUnk_03002360].x;
                }
                b = gUnk_03005640;
                b[gUnk_03002360].x0 = gUnk_030055F8[0] - 117;
                b[gUnk_03002360].x1 = gUnk_030055F8[1] + 117;
            }
            if (gUnk_03005680.unk1 & 2)
            {
                struct CamRect *b;

                if (gUnk_03005664 > 0)
                {
                    s16 *d = gUnk_030055F8;
                    struct CamPos *c = gUnk_030055D0;

                    d[2] = c[gUnk_03002360].y;
                    gUnk_030055F8[3] = gUnk_03005680.unk8;
                }
                else
                {
                    struct CamPos *c;

                    gUnk_030055F8[2] = gUnk_03005680.unk6;
                    c = gUnk_030055D0;
                    gUnk_030055F8[3] = c[gUnk_03002360].y;
                }
                b = gUnk_03005640;
                b[gUnk_03002360].y0 = gUnk_030055F8[2] - 76;
                b[gUnk_03002360].y1 = gUnk_030055F8[3] + 104;
            }
        }
        else
        {
            sub_0802a63c();
            if (gUnk_03005680.unk1 & 1)
                gUnk_030055D0[gUnk_030023AC].x = gUnk_03005680.unkA;
            if (gUnk_03005680.unk1 & 2)
            {
                struct CamPos *c = gUnk_030055D0;

                c[gUnk_030023AC].y = gUnk_03005680.unkC;
            }
        }
    }
}
