#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_0a130.c (0x0800A130-0x0800AACF, issue #96).
 *
 * HUD/score state updates. */

struct HudBar
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
    /*0x06*/ s16 unk6;
};

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern u8 gUnk_02000034;
extern s8 gUnk_02004B58;
extern s16 gUnk_02004B68;
extern s8 gUnk_02005590[];
extern s8 gUnk_020055D0;
extern s8 gUnk_020055F0[];
extern struct Unk02005E00 gUnk_02005E00;
extern u8 gUnk_02006014;
extern s32 gUnk_02006020[];
extern u8 gUnk_02006030;
extern u16 gUnk_02006068[];
extern s8 gUnk_0200617C;
extern s8 gUnk_020061D4;
extern struct HudBar gUnk_02006A00[];
extern s16 gUnk_02007D30;
extern s16 gUnk_02007FBC[];
extern s16 gUnk_02008014[];
extern s16 gUnk_0200801C;
extern u8 gUnk_0200AFF8;
extern u8 gUnk_0200B028;
extern void (*gUnk_03000014)(void);
extern u16 gUnk_03002360;
extern u8 gUnk_030023B0;
extern s8 gUnk_03002444;
extern s32 gUnk_03002448;

void sub_080031b8(s32 id);
void sub_08008ebc(void);
void sub_08008ed4(s32 a0);
void sub_0800ab08(void);
void sub_0800ad68(s32 v);
void sub_0800af40(u16 *time);
void sub_0800b0a4(s32 n);
void sub_0800b0fc(void);
void sub_0800b130(s32 x);
void sub_0800b190(s32 from, s32 to);
void sub_0800b230(s32 a, s32 b);
void sub_0800b34c(s32 x, s32 y, s32 n);
void sub_0800b3f8(void);
void sub_0800a9a0(s32 from, s32 to, s32 i);
s32 sub_0800aa18(s32 from, s32 to);
void sub_0800aa74(s32 x, s32 i);
void sub_0800aa94(s32 i);
void sub_0800aaac(s32 i);

void sub_0800a130(s32 a, s32 id)
{
    if (id == gUnk_03002360 && gUnk_02006014 == 1) {
        if (a == 0) {
            sub_08008ebc();
            gUnk_020061D4 = 0;
            gUnk_0200801C = 0;
            sub_0800b0a4(0);
        } else {
            sub_08008ed4(a);
        }
    }
}

void sub_0800a178(s32 a, s32 id)
{
    if (id == gUnk_03002360 && gUnk_02006014 == 1)
        gUnk_0200801C = a;
}

void sub_0800a19c(s32 id)
{
    if (id == gUnk_03002360 && gUnk_02006014 == 1) {
        switch (gUnk_020061D4) {
        case 0:
            gUnk_020061D4 = 2;
            gUnk_0200801C = 0;
            break;
        case 1:
            gUnk_020061D4 = 2;
            gUnk_0200801C = 16;
            break;
        case 2:
            break;
        case 3:
            gUnk_0200801C = 180;
            break;
        case 4:
            gUnk_020061D4 = 2;
            break;
        }
    }
}

void sub_0800a21c(s32 id)
{
    if (id == gUnk_03002360 && gUnk_02006014 == 1) {
        switch (gUnk_020061D4) {
        case 0:
        case 1:
            break;
        case 2:
            gUnk_020061D4 = 4;
            break;
        case 3:
            gUnk_020061D4 = 4;
            gUnk_0200801C = 48;
            break;
        case 4:
            break;
        }
    }
}

void sub_0800a280(void)
{
    gUnk_0200B028 = 1;
    sub_0800b0fc();
}

void sub_0800a294(s32 max, s32 cur)
{
    if (gUnk_030023B0 != 0) {
        gUnk_02004B68 = max;
        gUnk_02007D30 = 32;
        gUnk_02007FBC[0] = 32;
        gUnk_0200AFF8 = 1;
        sub_0800b130(32);
        gUnk_02004B58 = 0;
        gUnk_02006A00[0].unk0 = 0;
        gUnk_02006A00[0].unk4 = gUnk_02007FBC[0];
        gUnk_02006A00[0].unk2 = gUnk_02007FBC[0];
        gUnk_02006A00[0].unk6 = 0;
    } else {
        gUnk_02004B68 = max;
        gUnk_02007D30 = 32;
        gUnk_02007FBC[0] = Div(cur << 5, gUnk_02004B68);
        if (gUnk_02007FBC[0] > gUnk_02007D30)
            gUnk_02007FBC[0] = gUnk_02007D30;
        sub_0800aa94(0);
        sub_0800b130(0);
        sub_0800aa18(0, gUnk_02007FBC[0]);
    }
}

void sub_0800a340(s32 max, s32 cur)
{
    s32 idx;
    struct HudBar *p;

    if (gUnk_020055D0 == 1)
        idx = 0;
    else
        idx = gUnk_02005590[gCurTaskIdx - 32];
    if (gUnk_030023B0 != 0) {
        if (gUnk_02004B68 == 0) {
            gUnk_02004B68 = max;
            gUnk_02007D30 = 32;
        }
        gUnk_02007FBC[idx] = gUnk_02007D30;
        gUnk_0200AFF8 = 1;
        sub_0800b130(32);
        gUnk_02004B58 = idx;
        p = &gUnk_02006A00[idx];
        p->unk0 = 0;
        p->unk4 = gUnk_02007FBC[idx];
        p->unk2 = gUnk_02007FBC[idx];
        p->unk6 = 0;
    } else {
        if (gUnk_02004B68 == 0) {
            gUnk_02004B68 = max;
            gUnk_02007D30 = 32;
        }
        gUnk_02007FBC[idx] = Div(cur << 5, gUnk_02004B68);
        sub_0800aa94(idx);
        sub_0800b130(0);
        sub_0800a9a0(0, gUnk_02007FBC[idx], idx);
    }
}

void sub_0800a42c(void)
{
    s32 idx;
    s32 v;

    if (gUnk_02004B68 != 0) {
        if (gUnk_020055D0 == 1)
            idx = 0;
        else
            idx = gUnk_02005590[gCurTaskIdx - 32];
        if (gUnk_02008014[idx] != -1 && gUnk_03002490->unk78 > 0) {
            v = Div(gUnk_03002490->unk78 << 5, gUnk_02004B68);
            if (v != gUnk_02007FBC[idx])
                sub_0800a9a0(gUnk_02007FBC[idx], v, idx);
            gUnk_02007FBC[idx] = v;
        }
    }
}

void sub_0800a4c0(void)
{
    s32 idx;
    s32 v;

    if (gUnk_02004B68 != 0) {
        if (gUnk_020055D0 == 1)
            idx = 0;
        else
            idx = gUnk_02005590[gCurTaskIdx - 32];
        if (gUnk_02008014[idx] != -1) {
            if (gUnk_03002490->unk78 > 0)
                v = Div(gUnk_03002490->unk78 << 5, gUnk_02004B68);
            else
                v = 0;
            gUnk_02007FBC[idx] = v;
            sub_0800aa74(v, idx);
        }
    }
}

void sub_0800a554(void)
{
    s32 idx;
    s32 i;

    if (gUnk_020055D0 == 1)
        idx = 0;
    else
        idx = gUnk_02005590[gCurTaskIdx - 32];
    if (gUnk_02008014[idx] != -1) {
        gUnk_02008014[idx] = -1;
        if (--gUnk_02000034 != 0) {
            if (gUnk_02004B58 == idx) {
                for (i = 0; i < 2; i++) {
                    if (i != idx && gUnk_02005590[gUnk_02008014[i] - 32] != -1)
                        break;
                }
                if (gUnk_02006014 == 1)
                    sub_0800aa74(gUnk_02007FBC[i], idx);
            }
        } else {
            if (gUnk_02006014 == 1) {
                sub_0800b34c(20, 18, 2);
                sub_0800b34c(20, 19, 2);
                if (gUnk_02006030 == 0)
                    sub_0800ad68(gUnk_02006020[gUnk_03002360]);
                else
                    sub_0800af40(gUnk_02006068);
            }
            gUnk_0200B028 = 0;
            gUnk_02004B68 = gUnk_02007D30 = 0;
        }
    }
}

void sub_0800a698(void)
{
    gUnk_03000014 = 0;
}

void sub_0800a6a4(void)
{
    u8 mode = gUnk_02006014;
    s32 row;

    if (mode != 0) {
        if (mode == 1) {
            if (gUnk_020055F0[0] == 0) {
                if (gUnk_02005E00.unk04[gUnk_03002360] != 0) {
                    gUnk_020055F0[0] = mode;
                    gUnk_020055F0[1] = 0;
                }
                if (gUnk_020055F0[0] == 0)
                    goto done;
            }
            if (gUnk_02005E00.unk04[gUnk_03002360] == 0) {
                gUnk_020055F0[0] = 0;
                row = gUnk_03002444 ? 2 : 0;
                sub_0800b34c(12, row, 16);
                sub_0800b34c(12, row + 1, 16);
            } else {
                if (gUnk_020055F0[1] == 0) {
                    if (gUnk_020055F0[0] == 2) {
                        gUnk_020055F0[0] = 1;
                        sub_0800b230(gUnk_02005E00.unk04[gUnk_03002360] >> 4, 1);
                    } else {
                        gUnk_020055F0[0] = 2;
                        sub_0800b230(gUnk_03002360, 2);
                    }
                    gUnk_020055F0[1] = 90;
                }
                gUnk_020055F0[1]--;
            }
        }
    done:
        sub_0800ab08();
        sub_0800b3f8();
    }
}

void sub_0800a778(void)
{
    if (gUnk_02006014 != 0 && gUnk_0200617C != 0) {
        switch (gUnk_020061D4) {
        case 0:
        case 1:
            break;
        case 2:
            gUnk_0200801C += 8;
            gUnk_03002448 = gUnk_0200801C;
            if (gUnk_03002448 > 47) {
                gUnk_03002448 = 48;
                gUnk_020061D4 = 3;
                gUnk_0200801C = 180;
            }
            sub_0800b0a4(gUnk_03002448 >> 3);
            break;
        case 3:
            if (--gUnk_0200801C <= 0)
                sub_0800a21c(gUnk_03002360);
            break;
        case 4:
            gUnk_0200801C -= 8;
            gUnk_03002448 = gUnk_0200801C;
            if (gUnk_03002448 <= 16) {
                gUnk_03002448 = 16;
                gUnk_020061D4 = 1;
                gUnk_0200801C = 0;
            }
            sub_0800b0a4(gUnk_03002448 >> 3);
            break;
        }
    }
}

void sub_0800a854(void)
{
    s32 i;
    struct HudBar *p;
    s32 v;

    if (gUnk_02006014 != 0 && gUnk_0200B028 == 1) {
        for (i = 0; i < 2; i++) {
            p = &gUnk_02006A00[i];
            switch (p->unk0) {
            case 0:
                break;
            case 1:
                if (--p->unk6 > 0)
                    break;
                sub_080031b8(221);
                v = p->unk4 + 1;
                if (v > p->unk2)
                    v = p->unk2;
                if (gUnk_02004B58 == i)
                    sub_0800b190(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2) {
                    p->unk0 = 0;
                    gUnk_0200AFF8 = 1;
                } else {
                    p->unk6 = 4;
                }
                break;
            case 2:
                if (--p->unk6 > 0)
                    break;
                sub_080031b8(221);
                v = p->unk4 + 1;
                if (v > p->unk2)
                    v = p->unk2;
                if (gUnk_02004B58 == i)
                    sub_0800b190(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2)
                    p->unk0 = 0;
                else
                    p->unk6 = 4;
                break;
            case 3:
                if (--p->unk6 > 0)
                    break;
                v = p->unk4 - 1;
                if (v < p->unk2)
                    v = p->unk2;
                if (gUnk_02004B58 == i)
                    sub_0800b190(p->unk4, v);
                p->unk4 = v;
                if (p->unk4 == p->unk2)
                    p->unk0 = 0;
                else
                    p->unk6 = 4;
                break;
            }
        }
    }
}

void sub_0800a9a0(s32 from, s32 to, s32 i)
{
    struct HudBar *p;
    s32 d;

    if (gUnk_02006014 != 0 && gUnk_0200B028 == 1) {
        p = &gUnk_02006A00[i];
        if (p->unk0 != 1) {
            sub_0800b130(p->unk4);
            if (p->unk0 != 0) {
                d = to - p->unk4;
                p->unk2 = to;
                if (d > 0)
                    p->unk0 = 2;
                else
                    p->unk0 = 3;
            } else {
                d = to - from;
                if (d != 0) {
                    p->unk4 = from;
                    p->unk2 = to;
                    p->unk6 = 4;
                    if (d > 0)
                        p->unk0 = 2;
                    else
                        p->unk0 = 3;
                }
            }
            gUnk_02004B58 = i;
        }
    }
}

s32 sub_0800aa18(s32 from, s32 to)
{
    s32 i;

    if (gUnk_02006014 != 0 && gUnk_0200B028 == 1 && to > from) {
        for (i = 0; i < 2; i++) {
            gUnk_02006A00[i].unk4 = from;
            gUnk_02006A00[i].unk2 = to;
            gUnk_02006A00[i].unk6 = 4;
            gUnk_02006A00[i].unk0 = 1;
        }
        gUnk_02004B58 = 0;
        gUnk_0200AFF8 = 0;
    }
}

void sub_0800aa74(s32 x, s32 i)
{
    sub_0800aaac(i);
    sub_0800b130(x);
    gUnk_02004B58 = i;
}

void sub_0800aa94(s32 i)
{
    struct HudBar *p = &gUnk_02006A00[i];

    p->unk0 = 0;
    p->unk4 = 0;
    p->unk2 = 0;
    p->unk6 = 0;
}

void sub_0800aaac(s32 i)
{
    struct HudBar *p = &gUnk_02006A00[i];

    p->unk0 = 0;
    p->unk4 = gUnk_02007FBC[i];
    p->unk2 = gUnk_02007FBC[i];
    p->unk6 = 0;
}
