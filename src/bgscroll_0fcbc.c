#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* bgscroll_0fcbc.c (0x0800FCBC-0x080100AB, issue #99).
 *
 * The menus' BG scroll animator and small shared helpers.  Up to eight
 * scrolls run at once, one per BG and axis: gUnk_02004B74 holds the
 * running bits (bit bg = x, bit 4 + bg = y), gUnk_02006070[axis][bg] the
 * speed and gUnk_020061B0[axis][bg] the target, and the BGnHOFS/BGnVOFS
 * shadow cells are reached through the pointer tables gUnk_08731DB0 and
 * gUnk_08731DA0.  sub_0800fcbc resets it, sub_0800fcfc/sub_0800fd24/
 * sub_0800fdb8/sub_0800fe54 start scrolls, sub_0800fe94 snaps them to
 * their targets and task type #257 (sub_0800ff00) steps them every frame.
 * Also: sub_0800ffd8 (link work byte 1 minus one, floored at 0),
 * sub_0800ffe8 (is the current task on screen), sub_08010020 (the four
 * blend shadow bytes) and sub_08010048 (window 0/1 setup). */

extern u8 gUnk_02004B74;
extern s32 gUnk_02006070[2][4];
extern s32 gUnk_020061B0[2][4];
extern vu8 gUnk_0200EBC0[];
extern vu8 gUnk_03000040;
extern vu16 gUnk_03000044;
extern vu16 gUnk_03000AFC;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03000B18;
extern vu8 gUnk_03000F7C;
extern vs32 gUnk_03000FA8;
extern vu16 gUnk_03000FD4;
extern vu16 gUnk_03001018;
extern vu8 gUnk_0300118C;
extern vu8 gUnk_03001EAC;
extern vu8 gUnk_03001ED0;
extern vu16 gUnk_03001ED8;
extern vs32 *const gUnk_08731DA0[4];
extern vs32 *const gUnk_08731DB0[4];

void TaskYieldTrampoline(s32 frames);
void sub_0800fd24(s32 speed, s32 dist, s32 bg);
void sub_0800fdb8(s32 speed, s32 dist, s32 bg);

void sub_0800fcbc(void)
{
    s32 i;

    gUnk_02004B74 = 0;
    for (i = 0; i <= 3; i++) {
        gUnk_02006070[0][i] = 0;
        gUnk_020061B0[0][i] = 0;
        *gUnk_08731DA0[i] = 0;
        *gUnk_08731DB0[i] = 0;
    }
}

void sub_0800fcfc(s32 xspeed, s32 yspeed, s32 xdist, s32 ydist, s32 bg)
{
    if (xspeed != 0)
        sub_0800fd24(xspeed, xdist, bg);
    if (yspeed != 0)
        sub_0800fdb8(yspeed, ydist, bg);
}

void sub_0800fd24(s32 speed, s32 dist, s32 bg)
{
    s32 v;

    gUnk_02006070[0][bg] = speed;
    if ((gUnk_02004B74 >> bg) & 1) {
        if (speed > 0)
            gUnk_020061B0[0][bg] += dist << 16;
        else
            gUnk_020061B0[0][bg] -= dist << 16;
    } else {
        gUnk_02004B74 |= 1 << bg;
        v = *gUnk_08731DB0[bg] & 0x01FF0000;
        *gUnk_08731DB0[bg] = v;
        if (speed > 0)
            gUnk_020061B0[0][bg] = v + (dist << 16);
        else
            gUnk_020061B0[0][bg] = v - (dist << 16);
    }
}

void sub_0800fdb8(s32 speed, s32 dist, s32 bg)
{
    s32 v;

    gUnk_02006070[1][bg] = speed;
    if (gUnk_02004B74 & (16 << bg)) {
        if (speed > 0)
            gUnk_020061B0[1][bg] += dist << 16;
        else
            gUnk_020061B0[1][bg] -= dist << 16;
    } else {
        gUnk_02004B74 |= 16 << bg;
        v = *gUnk_08731DA0[bg] & 0x01FF0000;
        *gUnk_08731DA0[bg] = v;
        if (speed > 0)
            gUnk_020061B0[1][bg] = v + (dist << 16);
        else
            gUnk_020061B0[1][bg] = v - (dist << 16);
    }
}

s32 sub_0800fe54(s32 speed)
{
    if (speed > 0)
        gUnk_03000FA8 = 0;
    else
        gUnk_03000FA8 = 144 << 16;
    gUnk_020061B0[1][3] &= 0x01FF0000;
    sub_0800fdb8(speed, 144, 3);
}

void sub_0800fe94(void)
{
    s32 i;

    for (i = 0; i <= 3; i++) {
        if ((gUnk_02004B74 >> i) & 1) {
            *gUnk_08731DB0[i] = gUnk_020061B0[0][i];
            gUnk_02004B74 &= ~(1 << i);
        }
        if (gUnk_02004B74 & (16 << i)) {
            *gUnk_08731DA0[i] = gUnk_020061B0[1][i];
            gUnk_02004B74 &= ~(16 << i);
        }
    }
}

void sub_0800ff00(void)
{
    s32 i;
    s32 v;

    for (;;) {
        for (i = 0; i <= 3; i++) {
            if ((gUnk_02004B74 >> i) & 1) {
                v = *gUnk_08731DB0[i] + gUnk_02006070[0][i];
                *gUnk_08731DB0[i] = v;
                if ((gUnk_02006070[0][i] > 0 && v >= gUnk_020061B0[0][i])
                    || (gUnk_02006070[0][i] < 0 && v <= gUnk_020061B0[0][i])) {
                    *gUnk_08731DB0[i] = gUnk_020061B0[0][i];
                    gUnk_02004B74 &= ~(1 << i);
                }
            }
            if (gUnk_02004B74 & (16 << i)) {
                v = *gUnk_08731DA0[i] + gUnk_02006070[1][i];
                *gUnk_08731DA0[i] = v;
                if ((gUnk_02006070[1][i] > 0 && v >= gUnk_020061B0[1][i])
                    || (gUnk_02006070[1][i] < 0 && v <= gUnk_020061B0[1][i])) {
                    *gUnk_08731DA0[i] = gUnk_020061B0[1][i];
                    gUnk_02004B74 &= ~(16 << i);
                }
            }
        }
        TaskYieldTrampoline(1);
    }
}

s32 sub_0800ffd8(void)
{
    s32 n = gUnk_0200EBC0[1];

    if (n != 0)
        n--;
    return n;
}

u8 sub_0800ffe8(void)
{
    struct Task *t = gUnk_03002490;
    s16 x = t->unk48;
    s16 y = t->unk4A;

    if ((u16)(x + 63) > 366)
        return 0;
    if (y <= -64)
        return 0;
    if (y > 223)
        return 0;
    return 1;
}

void sub_08010020(s32 a, s32 b, s32 c, s32 d)
{
    gUnk_0300118C = a;
    gUnk_03000040 = b;
    gUnk_03000B08 = c;
    gUnk_03001EAC = d;
}

void sub_08010048(s32 in, s32 out, s32 h, s32 v, s32 win)
{
    if (win == 0x2000) {
        gUnk_03000B18 = in;
        gUnk_03000FD4 = h;
        gUnk_03000044 = v;
    }
    if (win == 0x4000) {
        gUnk_03001ED0 = in;
        gUnk_03001018 = h;
        gUnk_03000AFC = v;
    }
    gUnk_03000F7C = out;
    gUnk_03001ED8 |= win;
}
