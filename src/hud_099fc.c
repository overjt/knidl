#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_099fc.c (0x080099FC-0x0800A12F, issue #96).
 *
 * Task type #237 and the HUD/score interface. */

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

extern u16 gUnk_02004B50[];
extern s8 gUnk_02004B58;
extern s16 gUnk_02004B68;
extern s16 gUnk_02005580;
extern s16 gUnk_02005588[];
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
extern s16 gUnk_02007D48[];
extern s16 gUnk_02007FBC[];
extern s16 gUnk_0200801C;
extern u16 gUnk_0200AF18[];
extern u8 gUnk_0200B028;
extern u8 gUnk_02020000[];
extern void (*gUnk_03000014)(void);
extern u8 gUnk_03001470[];
extern u8 gUnk_03001F34;
extern struct PlayerState gUnk_03002170[];
extern u8 gUnk_03002340;
extern u16 gUnk_03002360;
extern s8 gUnk_03002444;
extern u8 gUnk_0300560C[];
extern u8 gUnk_06010000[];
extern u16 gUnk_08731CE6[];
extern u32 gUnk_087555D8[];
extern struct GfxHeader *gUnk_087555FC[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_080031b8(s32 id);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08008ebc(void);
void sub_08008ed4(s32 a);
void sub_0800a130(s32 a, s32 id);
void sub_0800a19c(s32 a);
void sub_0800a280(void);
void sub_0800aa94(s32 idx);
void sub_0800aad0(void);
void sub_0800ab3c(void);
void sub_0800ab64(s32 a);
void sub_0800abc0(s32 n);
void sub_0800ac38(s32 n);
void sub_0800acbc(s32 a, s32 b);
void sub_0800ad68(s32 v);
void sub_0800af40(u16 *time);
void sub_0800b0a4(s32 n);
void sub_0800b130(s32 x);
void sub_0800b230(s32 a, s32 b);
void sub_0800b37c(void);
void sub_0800b3a8(void);
void sub_0800b3f8(void);
void sub_0800b428(void);
void sub_0800a0dc(s32 a, s32 b);

void sub_080099fc(void)
{
    struct GfxHeader *h;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_087555D8;
    gUnk_03002490->unk40 = 0x800;
    gUnk_03002490->unk4C = 0x300000;
    gUnk_03002490->unk50 = 0x780000;
    h = gUnk_087555FC[gUnk_03002490->unk18];
    sub_080017e4(2, (u32)h->unk08, (u32)gUnk_03001470, h->unk00 << 5);
    LZ77UnCompVram(h->unk0C, gUnk_02020000);
    sub_080017e4(3, (u32)gUnk_02020000, (u32)gUnk_06010000, h->unk02 << 5);
    gUnk_03002490->unk3C = gUnk_03002490->unk18;
    TaskYieldTrampoline(gUnk_08731CE6[gUnk_03002490->unk18] + 67);
    TaskDispatchTrampoline();
}

void sub_08009aa0(void)
{
    gUnk_02006030 = 0;
    gUnk_03000014 = 0;
}

void sub_08009ab8(void)
{
    gUnk_02006030 = 1;
    gUnk_03000014 = sub_0800aad0;
    gUnk_03000014();
}

void sub_08009adc(void)
{
    s32 i;

    gUnk_02006014 = 0;
    gUnk_0200617C = 0;
    for (i = 0; i < 2; i++) {
        gUnk_02007FBC[i] = 0;
        sub_0800aa94(i);
    }
    gUnk_02004B68 = gUnk_02007D30 = 0;
    gUnk_020055F0[0] = 0;
    gUnk_020055F0[1] = 0;
}

void sub_08009b2c(s32 i)
{
    s32 j;

    sub_0800b37c();
    if (gUnk_03002444 != 0)
        sub_0800ab3c();
    if (gUnk_02007D48[i] != 0 || gUnk_02005588[i] != 0) {
        gUnk_02006014 = 1;
        gUnk_0200617C = 1;
        sub_0800b428();
        sub_0800ab64(i);
        sub_0800abc0(gUnk_02007D48[i]);
        sub_0800ac38(gUnk_02005588[i] >> 3);
        if ((gUnk_03002340 >> i) & 1) {
            sub_0800a130((s16)gUnk_02004B50[i], i);
            if ((s16)gUnk_02004B50[i] != 0) {
                gUnk_020061D4 = 1;
                gUnk_0200801C = 16;
                sub_0800b0a4(2);
            } else {
                gUnk_020061D4 = 0;
                gUnk_0200801C = 0;
                sub_0800b0a4(0);
            }
        } else if (gUnk_0300560C[i] != 1) {
            sub_0800a130(26, i);
            gUnk_020061D4 = 1;
            gUnk_0200801C = 16;
            sub_0800b0a4(2);
        } else {
            gUnk_020061D4 = 0;
            gUnk_0200801C = 0;
            sub_0800b0a4(0);
        }
        if (gUnk_02006030 == 0)
            sub_0800ad68(gUnk_02006020[i]);
        else
            sub_0800af40(gUnk_02006068);
        gUnk_0200B028 = 0;
        for (j = 0; j < 2; j++) {
            gUnk_02007FBC[j] = 0;
            sub_0800aa94(j);
        }
        gUnk_02004B68 = gUnk_02007D30 = 0;
        gUnk_020055F0[0] = 0;
        gUnk_020055F0[1] = 0;
    } else {
        gUnk_02006014 = 2;
        gUnk_0200617C = 0;
    }
    sub_0800b3f8();
}

void sub_08009cb0(s32 i)
{
    gUnk_02006014 = 1;
    gUnk_0200617C = 1;
    sub_0800b3a8();
    sub_0800b428();
    if (gUnk_03002444 != 0)
        sub_0800ab3c();
    sub_0800ab64(i);
    sub_0800abc0(gUnk_02007D48[i]);
    sub_0800ac38(gUnk_02005588[i] >> 3);
    if ((gUnk_03002340 >> i) & 1) {
        sub_0800a130((s16)gUnk_02004B50[i], i);
        if ((s16)gUnk_02004B50[i] != 0) {
            gUnk_020061D4 = 1;
            gUnk_0200801C = 16;
            sub_0800b0a4(2);
        } else {
            gUnk_020061D4 = 0;
            gUnk_0200801C = 0;
            sub_0800b0a4(0);
        }
    } else {
        sub_0800a0dc(26, i);
    }
    if (gUnk_0200B028 == 0) {
        if (gUnk_02006030 == 0)
            sub_0800ad68(gUnk_02006020[i]);
        else
            sub_0800af40(gUnk_02006068);
    } else {
        sub_0800a280();
        sub_0800b130(gUnk_02006A00[gUnk_02004B58].unk4);
    }
    if (gUnk_020055F0[0] != 0) {
        if (gUnk_020055F0[0] == 2)
            sub_0800b230(gUnk_02005E00.unk04[i] >> 4, gUnk_020055F0[0]);
        else
            sub_0800b230(i, gUnk_020055F0[0]);
    }
    sub_0800b3f8();
}

void sub_08009e14(void)
{
    gUnk_0200617C = 0;
}

void sub_08009e20(void)
{
    gUnk_0200617C = 1;
}

void sub_08009e2c(void)
{
    gUnk_02006014 = 2;
    gUnk_0200617C = 0;
    sub_0800b3a8();
    if (gUnk_03002444 != 0)
        sub_0800ab3c();
}

s32 sub_08009e60(s32 a, u32 b)
{
    if (b < 4) {
        gUnk_02007D48[b] = gUnk_02007D48[b] + a;
        if (gUnk_02007D48[b] >= 100)
            gUnk_02007D48[b] = 99;
        else if (gUnk_02007D48[b] < 0)
            gUnk_02007D48[b] = 0;
        if (b == gUnk_03002360 && gUnk_02006014 == 1)
            sub_0800abc0(gUnk_02007D48[b]);
    }
}

s32 sub_08009eb8(s32 a, u32 b)
{
    if (b < 4) {
        gUnk_02007D48[b] = gUnk_02007D48[b] + a;
        if (gUnk_02007D48[b] >= 100)
            gUnk_02007D48[b] = 99;
        else if (gUnk_02007D48[b] < 0)
            gUnk_02007D48[b] = 0;
    }
}

s32 sub_08009ee8(s32 a, u32 b)
{
    s32 old, delta;

    if (b < 4) {
        old = gUnk_02005588[b];
        if (a > 0) {
            if (old + a > gUnk_02005580) {
                delta = gUnk_02005580 - old;
                gUnk_02005588[b] = gUnk_02005580;
            } else {
                delta = a;
                gUnk_02005588[b] += delta;
            }
        } else {
            if (old + a < 0) {
                delta = -old;
                gUnk_02005588[b] = 0;
            } else {
                delta = a;
                gUnk_02005588[b] += delta;
            }
            if (gUnk_02005588[b] == 0)
                gUnk_03001F34 = 1;
            else if (gUnk_02005588[b] == 8 && gUnk_03002360 == b)
                sub_080031b8(262);
        }
        gUnk_03002790[b].unk78 = gUnk_02005588[b];
        if (b == gUnk_03002360 && gUnk_02006014 == 1)
            sub_0800acbc(old >> 3, delta >> 3);
        return gUnk_03002790[b].unk78;
    }
}

s32 sub_08009fcc(s32 a, s32 b, u32 c)
{
    if (c < 4) {
        struct PlayerState *p;
        gUnk_02004B50[c] = a;
        gUnk_0200AF18[c] = b;
        p = &gUnk_03002170[c];
        p->unk0D = a;
        p->unk0E = b;
        return p->unk0D;
    }
}

s32 sub_0800a008(s32 a, s32 b, u32 c)
{
    if (c < 4) {
        struct PlayerState *p;
        gUnk_02004B50[c] = a;
        gUnk_0200AF18[c] = b;
        p = &gUnk_03002170[c];
        p->unk0D = a;
        p->unk0E = b;
        sub_0800a130(a, c);
        return p->unk0D;
    }
}

void sub_0800a04c(s32 a, u32 b)
{
    if (b < 4) {
        if (gUnk_02006020[b] < 99999999) {
            gUnk_02006020[b] = gUnk_02006020[b] + a;
            if (gUnk_02006020[b] > 99999999)
                gUnk_02006020[b] = 99999999;
        }
        if (gUnk_03002360 == b && gUnk_0200B028 == 0 && gUnk_02006030 == 0)
            sub_0800ad68(gUnk_02006020[b]);
    }
}

void sub_0800a0ac(s32 a, u32 b)
{
    if (b < 4) {
        if (gUnk_02006020[b] < 99999999) {
            gUnk_02006020[b] = gUnk_02006020[b] + a;
            if (gUnk_02006020[b] > 99999999)
                gUnk_02006020[b] = 99999999;
        }
    }
}

void sub_0800a0dc(s32 a, s32 b)
{
    if (b == gUnk_03002360 && gUnk_02006014 == 1) {
        if (a == 0) {
            sub_08008ebc();
            gUnk_020061D4 = a;
            gUnk_0200801C = a;
            sub_0800b0a4(0);
        } else {
            sub_08008ed4(a);
            sub_0800a19c(b);
        }
    }
}
