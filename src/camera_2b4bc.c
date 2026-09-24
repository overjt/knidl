#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_2b4bc.c (0x0802B4BC-0x0802C42B, issue #86).
 *
 * The per-frame camera updates of the room modes (called from M07).  With
 * one player, sub_0802b4bc, sub_0802b62c, sub_0802be80 and sub_0802bff4
 * clamp the player position gUnk_030023CC/gUnk_03002388 to the camera
 * bounds gUnk_030055F8 into that player's camera gUnk_030055D0[i]; with
 * several they run the multiplayer helpers of camera_29c74.c.  While the
 * scroll lock of gUnk_03005680 is armed (sub_0802cda0), sub_0802b62c and
 * sub_0802bff4 slide the camera and its bounds towards the lock line by
 * gUnk_03005610 (x) / gUnk_03005664 (y) pixels a frame and stop on
 * arrival.  All of them end by writing the 16.16 target
 * gUnk_03005614/gUnk_03005634 and the visible rectangle gUnk_03002158. */

struct CamRect { s16 x0, x1, y0, y1; };

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

extern u8 gUnk_03002350;
extern u16 gUnk_030023AC;
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;
extern u8 gUnk_0200B078;
extern s16 gUnk_030055F8[4];
extern u16 gUnk_03002360;
extern struct CamRect gUnk_03005640[4];
extern struct CamPos gUnk_030055D0[4];
extern s32 gUnk_03005614;
extern s32 gUnk_03005634;
extern s16 gUnk_03002158[4];
extern u8 gUnk_03002340;
extern struct Unk03005680 gUnk_03005680;
extern s32 gUnk_03005610;
extern s32 gUnk_03005664;
extern u16 gUnk_030055C0;
extern s16 gUnk_03005628[4];

void sub_08029c74(void);
void sub_08029e24(void);
void sub_08029ef4(void);
void sub_0802a190(void);
void sub_0802a260(void);
void sub_0802a42c(void);
void sub_0802a484(void);
void sub_0802a4ec(void);
void sub_0802a568(void);
void sub_0802a63c(void);

void sub_0802b4bc(void)
{
    s32 x, y;
    struct CamPos *c;

    if (gUnk_03002350)
    {
        if (gUnk_030023AC == 1)
        {
            x = gUnk_030023CC;
            y = gUnk_03002388;
            if (gUnk_0200B078 == 2 && y < gUnk_030055F8[3])
            {
                struct CamRect *p;

                gUnk_030055F8[3] = y;
                p = gUnk_03005640;
                p[gUnk_03002360].y1 = y + 104;
            }
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
                c = gUnk_030055D0;
                x = c[gUnk_03002360].x;
                y = c[gUnk_03002360].y;
            }
            gUnk_03005614 = x << 16;
            gUnk_03005634 = y << 16;
            sub_0802a4ec();
        }
    }
}

void sub_0802b62c(void)
{
    s32 x, y, i;
    u32 done;

    if (gUnk_03002350 == 0)
        return;
    if (gUnk_030023AC == 1)
    {
        done = 0;
        if (gUnk_03005680.unk1 & 1)
        {
            s32 d = gUnk_03005610;

            if (d == 0)
            {
                done = 1;
            }
            else
            {
                gUnk_030055D0[gUnk_03002360].x += d;
                if (d > 0)
                {
                    if (gUnk_030055D0[gUnk_03002360].x >= gUnk_03005680.unk2)
                    {
                        gUnk_030055D0[gUnk_03002360].x = gUnk_03005680.unk2;
                        gUnk_03005610 = 0;
                        done = 1;
                    }
                    gUnk_030055F8[0] = gUnk_030055D0[gUnk_03002360].x;
                    gUnk_03005640[gUnk_03002360].x0 = gUnk_030055D0[gUnk_03002360].x + -117;
                }
                else
                {
                    if (gUnk_03005680.unk4 >= gUnk_030055D0[gUnk_03002360].x)
                    {
                        gUnk_030055D0[gUnk_03002360].x = gUnk_03005680.unk4;
                        gUnk_03005610 = 0;
                        done = 1;
                    }
                    gUnk_030055F8[1] = gUnk_030055D0[gUnk_03002360].x;
                    gUnk_03005640[gUnk_03002360].x1 = gUnk_030055D0[gUnk_03002360].x + 117;
                }
            }
            x = gUnk_030055D0[gUnk_03002360].x;
        }
        else
        {
            x = gUnk_030023CC;
            if (x < gUnk_030055F8[0])
                x = gUnk_030055F8[0];
            if (x > gUnk_030055F8[1])
                x = gUnk_030055F8[1];
            gUnk_030055D0[gUnk_03002360].x = x;
        }
        if (gUnk_03005680.unk1 & 2)
        {
            s32 d = gUnk_03005664;

            if (d == 0)
            {
                done |= 2;
            }
            else
            {
                gUnk_030055D0[gUnk_03002360].y += d;
                if (d > 0)
                {
                    if (gUnk_030055D0[gUnk_03002360].y >= gUnk_03005680.unk6)
                    {
                        gUnk_030055D0[gUnk_03002360].y = gUnk_03005680.unk6;
                        gUnk_03005664 = 0;
                        done |= 2;
                    }
                    gUnk_030055F8[2] = gUnk_030055D0[gUnk_03002360].y;
                    gUnk_03005640[gUnk_03002360].y0 = gUnk_030055D0[gUnk_03002360].y + -76;
                }
                else
                {
                    if (gUnk_03005680.unk8 >= gUnk_030055D0[gUnk_03002360].y)
                    {
                        gUnk_030055D0[gUnk_03002360].y = gUnk_03005680.unk8;
                        gUnk_03005664 = 0;
                        done |= 2;
                    }
                    gUnk_030055F8[3] = gUnk_030055D0[gUnk_03002360].y;
                    gUnk_03005640[gUnk_03002360].y1 = gUnk_030055D0[gUnk_03002360].y + 104;
                }
            }
            y = gUnk_030055D0[gUnk_03002360].y;
        }
        else
        {
            y = gUnk_03002388;
            if (y < gUnk_030055F8[2])
                y = gUnk_030055F8[2];
            if (y > gUnk_030055F8[3])
                y = gUnk_030055F8[3];
            gUnk_030055D0[gUnk_03002360].y = y;
        }
        if (done == gUnk_03005680.unk1)
            gUnk_030055C0 = 3;
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
        sub_08029ef4();
        done = 0;
        if (gUnk_03005680.unk1 & 1)
        {
            if (gUnk_03005610 == 0)
            {
                done = 1;
                gUnk_030055F8[0] = gUnk_03005680.unk2;
                gUnk_030055F8[1] = gUnk_03005680.unk4;
            }
            else if (gUnk_03005610 > 0)
            {
                for (i = 0; i < gUnk_030023AC; i++)
                {
                    if (((gUnk_03005680.unk0 >> i) & 1) && ((gUnk_03002340 >> i) & 1))
                    {
                        gUnk_030055D0[i].x += gUnk_03005610;
                        if (gUnk_03005680.unk2 <= gUnk_030055D0[i].x)
                            gUnk_030055D0[i].x = gUnk_03005680.unk2;
                        gUnk_03005640[i].x0 = gUnk_030055D0[i].x + -117;
                        gUnk_03005640[i].x1 = gUnk_03005680.unk4 + 117;
                    }
                }
                if (gUnk_030055D0[gUnk_030023AC].x < gUnk_03005680.unk4)
                {
                    gUnk_030055D0[gUnk_030023AC].x += gUnk_03005610;
                    if (gUnk_03005680.unk4 < gUnk_030055D0[gUnk_030023AC].x)
                        gUnk_030055D0[gUnk_030023AC].x = gUnk_03005680.unk4;
                }
                if ((gUnk_03005680.unk0 >> gUnk_030023AC) & 1)
                {
                    gUnk_030055F8[0] += gUnk_03005610;
                    if (gUnk_03005680.unk2 <= gUnk_030055F8[0])
                    {
                        done |= 1;
                        gUnk_03005610 = 0;
                        gUnk_030055F8[0] = gUnk_03005680.unk2;
                    }
                }
                gUnk_030055F8[1] = gUnk_03005680.unk4;
            }
            else
            {
                for (i = 0; i < gUnk_030023AC; i++)
                {
                    if (((gUnk_03005680.unk0 >> i) & 1) && ((gUnk_03002340 >> i) & 1))
                    {
                        gUnk_030055D0[i].x += gUnk_03005610;
                        if (gUnk_030055D0[i].x <= gUnk_03005680.unk4)
                            gUnk_030055D0[i].x = gUnk_03005680.unk4;
                        gUnk_03005640[i].x0 = gUnk_03005680.unk2 + -117;
                        gUnk_03005640[i].x1 = gUnk_030055D0[i].x + 117;
                    }
                }
                if (gUnk_03005680.unk2 < gUnk_030055D0[gUnk_030023AC].x)
                {
                    gUnk_030055D0[gUnk_030023AC].x += gUnk_03005610;
                    if (gUnk_030055D0[gUnk_030023AC].x < gUnk_03005680.unk2)
                        gUnk_030055D0[gUnk_030023AC].x = gUnk_03005680.unk2;
                }
                if ((gUnk_03005680.unk0 >> gUnk_030023AC) & 1)
                {
                    gUnk_030055F8[1] += gUnk_03005610;
                    if (gUnk_030055F8[1] <= gUnk_03005680.unk4)
                    {
                        done |= 1;
                        gUnk_03005610 = 0;
                        gUnk_030055F8[1] = gUnk_03005680.unk4;
                    }
                }
                gUnk_030055F8[0] = gUnk_03005680.unk2;
            }
        }
        if (gUnk_03005680.unk1 & 2)
        {
            if (gUnk_03005664 == 0)
            {
                done |= 2;
                gUnk_030055F8[2] = gUnk_03005680.unk6;
                gUnk_030055F8[3] = gUnk_03005680.unk8;
            }
            else if (gUnk_03005664 > 0)
            {
                for (i = 0; i < gUnk_030023AC; i++)
                {
                    if (((gUnk_03005680.unk0 >> i) & 1) && ((gUnk_03002340 >> i) & 1))
                    {
                        gUnk_030055D0[i].y += gUnk_03005664;
                        if (gUnk_03005680.unk6 <= gUnk_030055D0[i].y)
                            gUnk_030055D0[i].y = gUnk_03005680.unk6;
                        gUnk_03005640[i].y0 = gUnk_030055D0[i].y + -76;
                        gUnk_03005640[i].y1 = gUnk_03005680.unk8 + 104;
                    }
                }
                if (gUnk_030055D0[gUnk_030023AC].y < gUnk_03005680.unk8)
                {
                    gUnk_030055D0[gUnk_030023AC].y += gUnk_03005664;
                    if (gUnk_03005680.unk8 < gUnk_030055D0[gUnk_030023AC].y)
                        gUnk_030055D0[gUnk_030023AC].y = gUnk_03005680.unk8;
                }
                if ((gUnk_03005680.unk0 >> gUnk_030023AC) & 1)
                {
                    gUnk_030055F8[2] += gUnk_03005664;
                    if (gUnk_03005680.unk6 <= gUnk_030055F8[2])
                    {
                        done |= 2;
                        gUnk_03005664 = 0;
                        gUnk_030055F8[2] = gUnk_03005680.unk6;
                    }
                }
                gUnk_030055F8[3] = gUnk_03005680.unk8;
            }
            else
            {
                for (i = 0; i < gUnk_030023AC; i++)
                {
                    if (((gUnk_03005680.unk0 >> i) & 1) && ((gUnk_03002340 >> i) & 1))
                    {
                        gUnk_030055D0[i].y += gUnk_03005664;
                        if (gUnk_030055D0[i].y <= gUnk_03005680.unk8)
                            gUnk_030055D0[i].y = gUnk_03005680.unk8;
                        gUnk_03005640[i].y1 = gUnk_030055D0[i].y + 104;
                        gUnk_03005640[i].y0 = gUnk_03005680.unk6 + -76;
                    }
                }
                if (gUnk_03005680.unk6 < gUnk_030055D0[gUnk_030023AC].y)
                {
                    gUnk_030055D0[gUnk_030023AC].y += gUnk_03005664;
                    if (gUnk_030055D0[gUnk_030023AC].y < gUnk_03005680.unk6)
                        gUnk_030055D0[gUnk_030023AC].y = gUnk_03005680.unk6;
                }
                if ((gUnk_03005680.unk0 >> gUnk_030023AC) & 1)
                {
                    gUnk_030055F8[3] += gUnk_03005664;
                    if (gUnk_030055F8[3] <= gUnk_03005680.unk8)
                    {
                        done |= 2;
                        gUnk_03005664 = 0;
                        gUnk_030055F8[3] = gUnk_03005680.unk8;
                    }
                }
                gUnk_030055F8[2] = gUnk_03005680.unk6;
            }
        }
        sub_0802a260();
        sub_0802a484();
        if ((gUnk_03002340 >> gUnk_03002360) & 1)
        {
            if ((gUnk_03005680.unk0 >> gUnk_03002360) & 1)
            {
                if (gUnk_03005680.unk1 & 1)
                {
                    x = gUnk_030055D0[gUnk_03002360].x;
                }
                else
                {
                    x = gUnk_030023CC;
                    if (x < gUnk_030055F8[0])
                        x = gUnk_030055F8[0];
                    if (x > gUnk_030055F8[1])
                        x = gUnk_030055F8[1];
                }
                if (gUnk_03005680.unk1 & 2)
                {
                    y = gUnk_030055D0[gUnk_03002360].y;
                }
                else
                {
                    y = gUnk_03002388;
                    if (y < gUnk_030055F8[2])
                        y = gUnk_030055F8[2];
                    if (y > gUnk_030055F8[3])
                        y = gUnk_030055F8[3];
                }
            }
            else
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
        }
        else
        {
            x = gUnk_030055D0[gUnk_03002360].x;
            y = gUnk_030055D0[gUnk_03002360].y;
        }
        gUnk_03005614 = x << 16;
        gUnk_03005634 = y << 16;
        sub_0802a568();
        if (done == gUnk_03005680.unk1)
            gUnk_030055C0 = 3;
        else
            sub_0802a63c();
    }
}

void sub_0802be80(void)
{
    s32 x, y;

    if (gUnk_03002350)
    {
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
            sub_08029e24();
            if (gUnk_03005680.unk1 & 1)
            {
                gUnk_030055F8[0] = gUnk_03005680.unk2;
                gUnk_030055F8[1] = gUnk_03005680.unk4;
            }
            if (gUnk_03005680.unk1 & 2)
            {
                gUnk_030055F8[2] = gUnk_03005680.unk6;
                gUnk_030055F8[3] = gUnk_03005680.unk8;
            }
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
                struct CamPos *c = gUnk_030055D0;

                x = c[gUnk_03002360].x;
                y = c[gUnk_03002360].y;
            }
            gUnk_03005614 = x << 16;
            gUnk_03005634 = y << 16;
            sub_0802a4ec();
        }
    }
}

/* Single-player path: animate the camera bounds toward the room bounds by
   gUnk_03005610 / gUnk_03005664 per frame and stop once the clamped player
   position no longer differs.  u/v must be their own locals (reusing x/y or
   one shared temp permutes the whole allocation). */
void sub_0802bff4(void)
{
    s32 x, y;
    s32 u, v;
    struct CamRect *p;

    if (gUnk_03002350)
    {
        if (gUnk_030023AC == 1)
        {
            if (gUnk_03005680.unk1 & 1)
            {
                gUnk_030055F8[0] -= gUnk_03005610;
                gUnk_030055F8[1] += gUnk_03005610;
                if (gUnk_030055F8[0] < gUnk_03005628[0])
                    gUnk_030055F8[0] = gUnk_03005628[0];
                if (gUnk_03005628[1] < gUnk_030055F8[1])
                    gUnk_030055F8[1] = gUnk_03005628[1];
                u = gUnk_030023CC;
                v = u;
                if (u < gUnk_03005628[0])
                    v = gUnk_03005628[0];
                if (gUnk_03005628[1] < v)
                    v = gUnk_03005628[1];
                if (u < gUnk_030055F8[0])
                    u = gUnk_030055F8[0];
                if (gUnk_030055F8[1] < u)
                    u = gUnk_030055F8[1];
                if (v == u)
                {
                    gUnk_03005610 = 0;
                    gUnk_03005680.unk1 &= ~1;
                    gUnk_030055F8[0] = gUnk_03005628[0];
                    gUnk_030055F8[1] = gUnk_03005628[1];
                }
                p = gUnk_03005640;
                p[gUnk_03002360].x0 = gUnk_030055F8[0] - 117;
                p[gUnk_03002360].x1 = gUnk_030055F8[1] + 117;
                /* bit 0 again, not bit 1: the ROM tests the x flag twice, so
                   the y bounds only animate while the x animation runs */
                if (gUnk_03005680.unk1 & 1)
                {
                    gUnk_030055F8[2] -= gUnk_03005664;
                    gUnk_030055F8[3] += gUnk_03005664;
                    if (gUnk_030055F8[2] < gUnk_03005628[2])
                        gUnk_030055F8[2] = gUnk_03005628[2];
                    if (gUnk_03005628[3] < gUnk_030055F8[3])
                        gUnk_030055F8[3] = gUnk_03005628[3];
                    u = gUnk_03002388;
                    v = u;
                    if (u < gUnk_03005628[2])
                        v = gUnk_03005628[2];
                    if (gUnk_03005628[3] < v)
                        v = gUnk_03005628[3];
                    if (u < gUnk_030055F8[2])
                        u = gUnk_030055F8[2];
                    if (gUnk_030055F8[3] < u)
                        u = gUnk_030055F8[3];
                    if (v == u)
                    {
                        gUnk_03005664 = 0;
                        gUnk_03005680.unk1 &= ~2;
                        gUnk_030055F8[2] = gUnk_03005628[2];
                        gUnk_030055F8[3] = gUnk_03005628[3];
                    }
                    p[gUnk_03002360].y0 = gUnk_030055F8[2] - 76;
                    p[gUnk_03002360].y1 = gUnk_030055F8[3] + 104;
                }
            }
            if (gUnk_03005680.unk1 == 0)
                gUnk_030055C0 = 0;
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
        }
        else
        {
            sub_08029c74();
            sub_08029e24();
            if (gUnk_03005680.unk1 & 1)
            {
                gUnk_03005680.unk2 -= gUnk_03005610;
                gUnk_03005680.unk4 += gUnk_03005610;
                if (gUnk_03005680.unk2 < gUnk_03005628[0])
                    gUnk_03005680.unk2 = gUnk_03005628[0];
                if (gUnk_03005628[1] < gUnk_03005680.unk4)
                    gUnk_03005680.unk4 = gUnk_03005628[1];
                if (gUnk_03005680.unk2 <= gUnk_030055F8[0] && gUnk_030055F8[1] <= gUnk_03005680.unk4)
                {
                    gUnk_03005610 = 0;
                    gUnk_03005680.unk1 &= ~1;
                }
                else
                {
                    if (gUnk_030055F8[0] < gUnk_03005680.unk2)
                        gUnk_030055F8[0] = gUnk_03005680.unk2;
                    if (gUnk_03005680.unk4 < gUnk_030055F8[1])
                        gUnk_030055F8[1] = gUnk_03005680.unk4;
                }
            }
            if (gUnk_03005680.unk1 & 2)
            {
                gUnk_03005680.unk6 -= gUnk_03005664;
                gUnk_03005680.unk8 += gUnk_03005664;
                if (gUnk_03005680.unk6 < gUnk_03005628[2])
                    gUnk_03005680.unk6 = gUnk_03005628[2];
                if (gUnk_03005628[3] < gUnk_03005680.unk8)
                    gUnk_03005680.unk8 = gUnk_03005628[3];
                if (gUnk_03005680.unk6 <= gUnk_030055F8[2] && gUnk_030055F8[3] <= gUnk_03005680.unk8)
                {
                    gUnk_03005664 = 0;
                    gUnk_03005680.unk1 &= ~2;
                }
                else
                {
                    if (gUnk_030055F8[2] < gUnk_03005680.unk6)
                        gUnk_030055F8[2] = gUnk_03005680.unk6;
                    if (gUnk_03005680.unk8 < gUnk_030055F8[3])
                        gUnk_030055F8[3] = gUnk_03005680.unk8;
                }
            }
            if (gUnk_03005680.unk1 == 0)
                gUnk_030055C0 = 0;
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
                struct CamPos *c = gUnk_030055D0;

                x = c[gUnk_03002360].x;
                y = c[gUnk_03002360].y;
            }
            gUnk_03005614 = x << 16;
            gUnk_03005634 = y << 16;
            sub_0802a4ec();
        }
    }
}
