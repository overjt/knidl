#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menutask_0ea0c.c (0x0800EA0C-0x0800F17F, issue #99).
 *
 * Menu sprite and palette tasks, second part.  sub_0800ea0c is the
 * per-frame body of task type #243 (two sprites plus a palette
 * cross-fade); task type #244 (sub_0800eae4, body sub_0800ec08) slides a
 * sprite pair in, bobs it and slides it out; task types #254
 * (sub_0800ec70, body sub_0800ecb8) and #255 (sub_0800ed78) animate the
 * sound-test screen (menu screen 7: the cursor sprites and the palette
 * pulse of the selected column, which stays lit while its song plays);
 * task types #249 (sub_0800ef30) and #250 (sub_0800f084) cycle the
 * palettes of the link-play screen (menu screen 8) through sub_08003014
 * blends. */

extern s8 gUnk_020060D0;
extern s8 gUnk_02007D34;
extern s8 gUnk_02007E88;
extern s16 gUnk_0200A6E0[];
extern s8 gUnk_0200B074;
extern vu16 gUnk_03000038;
extern vu8 gUnk_03000B08;
extern u16 gUnk_030012F0[][16];
extern u16 gUnk_03001310[][16];
extern u16 gUnk_03001372[];
extern u16 gUnk_030015D0[];
extern u16 gUnk_030015F4[];
extern vu8 gUnk_03001EAC;
extern u16 gUnk_08559BA4[][16];
extern u16 gUnk_08560DBC[][16];
extern u16 gUnk_08560F9C[];
extern u16 gUnk_08561224[][10];
extern u16 gUnk_0856342C[][10];
extern u16 gUnk_085634D8[][16];
extern u16 gUnk_08564F38[][5];
extern u32 gUnk_08755650[];
extern u32 gUnk_087556D4[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080059d8(void);
u8 sub_0800ffe8(void);
void sub_08010020(s32 a, s32 b, s32 c, s32 d);
void sub_0800ec08(void);
void sub_0800ecb8(void);

void sub_0800ea0c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    s32 n;
    s32 k;

    if (sub_0800ffe8()) {
        struct Task *s = gUnk_03002490;
        u32 *tbl = s->unk38;

        sub_08001a94(3, tbl[8], 0, 0, s->unk48, s->unk4A);
        sub_08001a94(2, tbl[9], 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    }
    t = gUnk_03002490;
    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gUnk_03002490;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gUnk_03002490->unk34 = 0;
    }
    v = gUnk_03002490;
    n = v->unk34 + 32;
    v->unk34 = n;
    if (n > 256)
        v->unk34 = 256;
    k = gUnk_02007D34 * 2;
    x = gUnk_03002490;
    sub_08003014(gUnk_08559BA4[k + x->unk2C], gUnk_08559BA4[k + x->unk30], (u16)x->unk34, 16, gUnk_030015D0);
}

void sub_0800eae4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059d8;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0800ec08;
    t->unk38 = gUnk_08755650;
    t->unk28 = 0;
    t->unk2C = 0;
    for (t->unk6C = 0; (s16)gUnk_03002490->unk6C <= 9; gUnk_03002490->unk6C++) {
        struct Task *u = gUnk_03002490;

        u->unk28 += ((s16)u->unk6C + 1) * 9 << 15;
        TaskYieldTrampoline(1);
    }
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++) {
        for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 1; gUnk_03002490->unk6E++) {
            gUnk_03002490->unk2C += 0x60000;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 1; gUnk_03002490->unk6E++) {
            gUnk_03002490->unk2C -= 0x60000;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(16);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 9; gUnk_03002490->unk6C++) {
        struct Task *u = gUnk_03002490;

        u->unk28 -= ((s16)u->unk6C + 1) * 9 << 15;
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_0800ec08(void)
{
    if (sub_0800ffe8()) {
        struct Task *t = gUnk_03002490;
        u32 *tbl = t->unk38;

        sub_08001a94(1, tbl[10], 0, 0, (t->unk28 >> 16) - 127, (t->unk2C >> 16) + 144);
        sub_08001a94(1, tbl[11], 0, 0, 0x16F - (gUnk_03002490->unk28 >> 16), (gUnk_03002490->unk2C >> 16) + 144);
    }
}

void sub_0800ec70(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0800ecb8;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    while (gUnk_020060D0 == 7)
        TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0800ecb8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 v;

    if (sub_0800ffe8()) {
        u32 *tbl = gUnk_087556D4;

        sub_08001a94(9, tbl[0], 0, 0, gUnk_0200B074 * 52 + 94, 136);
        sub_08001a94(8, tbl[1], 0, 0, 94, 136);
        sub_08001a94(8, tbl[2], 0, 0, 146, 136);
    }
    t = gUnk_03002490;
    if (t->unk34 == 256) {
        t->unk2C ^= 1;
        t->unk30 ^= 1;
        t->unk34 = 0;
    }
    u = gUnk_03002490;
    v = u->unk34 + 32;
    u->unk34 = v;
    if (v > 256)
        u->unk34 = 256;
    w = gUnk_03002490;
    sub_08003014(gUnk_08564F38[w->unk2C], gUnk_08564F38[w->unk30], (u16)w->unk34, 5, gUnk_030015F4);
}

void sub_0800ed78(void)
{
    struct Task *t = gUnk_03002490;

    t->unk28 = 0;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    t->unk18 = 0;
    while (gUnk_020060D0 == 7) {
        struct Task *u;
        struct Task *v;
        struct Task *w;
        s32 n;

        u = gUnk_03002490;
        if (u->unk34 == 256) {
            if (++u->unk2C > 5) {
                u->unk2C = 0;
                u->unk28 = 4;
                u->unk18 = 0;
            }
            v = gUnk_03002490;
            if (++v->unk30 > 5)
                v->unk30 = 0;
            gUnk_03002490->unk34 = 0;
        }
        v = gUnk_03002490;
        if (v->unk28 != 0) {
            v->unk28--;
        } else {
            n = v->unk34 + 128;
            v->unk34 = n;
            if (n > 256)
                v->unk34 = 256;
        }
        if (gUnk_0200B074 == 0) {
            if ((s32)gMPlayTable[gSongTable[gUnk_0200A6E0[gUnk_0200B074]].ms].info->status >= 0) {
                w = gUnk_03002490;
                sub_08003014(gUnk_085634D8[w->unk2C], gUnk_085634D8[w->unk30], (u16)w->unk34, 16, gUnk_03001310[gUnk_0200B074]);
            } else {
                sub_08003014(gUnk_085634D8[5], gUnk_085634D8[5], (u16)gUnk_03002490->unk34, 16, gUnk_03001310[gUnk_0200B074]);
            }
        } else {
            if (gUnk_03000038 & 1) {
                struct Task *x = gUnk_03002490;

                x->unk18 = 1;
                x->unk28 = 0;
                x->unk2C = 0;
                x->unk30 = 1;
                x->unk34 = 0;
            }
            w = gUnk_03002490;
            if (w->unk18 != 0)
                sub_08003014(gUnk_085634D8[w->unk2C], gUnk_085634D8[w->unk30], (u16)w->unk34, 16, gUnk_03001310[gUnk_0200B074]);
            else
                sub_08003014(gUnk_085634D8[5], gUnk_085634D8[5], (u16)w->unk34, 16, gUnk_03001310[gUnk_0200B074]);
        }
        sub_080017e4(2, (u32)&gUnk_085634D8[0][(gUnk_0200B074 + 6) * 16], (u32)gUnk_03001310[(s8)(gUnk_0200B074 ^ 1)], 32);
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_0800ef30(void)
{
    struct Task *t = gUnk_03002490;

    t->unk28 = 0;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    t->unk18 = 0;
    sub_08010020(68, 8, 0, 16);
    while (gUnk_020060D0 == 8 || gUnk_020060D0 == 9) {
        struct Task *u;
        struct Task *v;
        s32 n;

        {
            struct Task *s = gUnk_03002490;

            if (s->unk18 <= 19) {
                s->unk18++;
                gUnk_03000B08 = s->unk18 >> 2;
                gUnk_03001EAC = 16 - gUnk_03000B08;
            }
        }
        u = gUnk_03002490;
        if (u->unk34 == 256) {
            if (++u->unk2C > 15) {
                u->unk2C = 0;
                if (++u->unk28 > 3)
                    u->unk28 = 0;
            }
            v = gUnk_03002490;
            if (++v->unk30 > 15)
                v->unk30 = 0;
            gUnk_03002490->unk34 = 0;
        }
        v = gUnk_03002490;
        n = v->unk34 + 64;
        v->unk34 = n;
        if (n > 256)
            v->unk34 = 256;
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 3; gUnk_03002490->unk6C++) {
            struct Task *w = gUnk_03002490;
            s32 i = (s16)w->unk6C;

            if (i == w->unk28)
                sub_08003014(gUnk_08560DBC[w->unk2C], gUnk_08560DBC[w->unk30], (u16)w->unk34, 16, gUnk_030012F0[i]);
            else
                sub_080017e4(2, (u32)gUnk_08560F9C, (u32)gUnk_030012F0[i], 32);
        }
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_0800f084(void)
{
    struct Task *t = gUnk_03002490;

    t->unk28 = gUnk_02007E88;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    while (gUnk_020060D0 == 8 || gUnk_020060D0 == 9) {
        struct Task *u;
        struct Task *v;
        struct Task *w;
        s32 n;

        u = gUnk_03002490;
        if (u->unk34 == 256) {
            if (++u->unk2C > 6)
                u->unk2C = 0;
            v = gUnk_03002490;
            if (++v->unk30 > 6)
                v->unk30 = 0;
            gUnk_03002490->unk34 = 0;
        }
        v = gUnk_03002490;
        n = v->unk34 + 16;
        v->unk34 = n;
        if (n > 256)
            v->unk34 = 256;
        w = gUnk_03002490;
        if (w->unk28 == 3)
            sub_08003014(gUnk_08561224[w->unk2C], gUnk_08561224[w->unk30], (u16)w->unk34, 10, gUnk_03001372);
        else
            sub_08003014(gUnk_0856342C[w->unk2C], gUnk_0856342C[w->unk30], (u16)w->unk34, 10, gUnk_03001372);
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}
