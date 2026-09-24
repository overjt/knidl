#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menutask_0e314.c (0x0800E314-0x0800EA0B, issue #99).
 *
 * Menu sprite tasks, middle part.  Task types #245 (sub_0800e314, body
 * sub_0800e390) and #246 (sub_0800e46c, body sub_0800e518) show the
 * pictures and palette pulses of menu screens 2 and 3; #247
 * (sub_0800e5b0, body sub_0800e674) the mode list's picture and row
 * highlight (sub_0800e7b0); #248 (sub_0800e81c, body sub_0800e8c0) the
 * picture of screen 5; and sub_0800e9a4 is the entry of #243, whose
 * body sub_0800ea0c is in menutask_0ea0c.c. */

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern s8 gUnk_02000018;
extern s8 gUnk_02004B44;
extern s8 gUnk_020060D0;
extern u8 gUnk_0200618C;
extern s8 gUnk_02007D34;
extern s8 gUnk_0200B074;
extern struct SaveSlot gUnk_0200E600[];
extern u16 gUnk_030012F0[];
extern u16 gUnk_0300153C[];
extern u16 gUnk_03001550[];
extern u16 gUnk_03001570[];
extern s32 gUnk_030023E8;
extern u16 gUnk_08559C24[][16];
extern u16 gUnk_08559CE6[];
extern u16 gUnk_08559CEC[];
extern u16 gUnk_0855D2F8[][10];
extern u16 gUnk_0855D320[];
extern u16 gUnk_0855D334[][16];
extern u16 gUnk_08731E4C[];
extern u16 gUnk_08731E52[];
extern u32 gUnk_08755650[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08008c64(u16 a0);
s32 sub_0800e2dc(s32 id, s32 part);
void sub_0800ea0c(void);
u8 sub_0800ffe8(void);
void sub_0800e390(void);
void sub_0800e518(void);
void sub_0800e674(void);
void sub_0800e7b0(void);
void sub_0800e8c0(void);

void sub_0800e314(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = (u32)sub_0800e390;
    gUnk_03002490->unk42 = 6;
    gUnk_03002490->unk38 = gUnk_08755650;
    gUnk_03002490->unk3C = 7;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk4C = 0xA00000;
    gUnk_03002490->unk50 = 0x300000;
    while (gUnk_020060D0 == 2 || gUnk_020060D0 == 3)
        TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0800e390(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gUnk_03002490;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gUnk_03002490->unk34 = 0;
    }
    v = gUnk_03002490;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gUnk_020060D0 == 2) {
        k = gUnk_02007D34 * 3;
        w = gUnk_03002490;
        sub_08003014(gUnk_08559C24[k + w->unk2C], gUnk_08559C24[k + w->unk30], (u16)w->unk34, 16, gUnk_03001570);
        sub_0800e2dc(1, gUnk_02007D34);
    } else {
        sub_08003014((u16 *)gUnk_08559C24 + (gUnk_02007D34 * 3 + 2) * 16, (u16 *)gUnk_08559C24 + (gUnk_02007D34 * 3 + 2) * 16, (u16)gUnk_03002490->unk34, 16, gUnk_03001570);
    }
}

void sub_0800e46c(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = (u32)sub_0800e518;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_08755650;
    gUnk_03002490->unk3C = 6;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    if (gUnk_0200E600[gUnk_030023E8].unk10 & 4) {
        gUnk_03002490->unk4C = 0xB00000;
        gUnk_03002490->unk50 = (gUnk_02007D34 << 20) + 0x280000;
    } else {
        gUnk_03002490->unk4C = 0xA80000;
        gUnk_03002490->unk50 = 0x300000;
    }
    while (gUnk_020060D0 == 3)
        TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0800e518(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gUnk_03002490;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gUnk_03002490->unk34 = 0;
    }
    v = gUnk_03002490;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    k = gUnk_02004B44 * 3;
    w = gUnk_03002490;
    sub_08003014(gUnk_08559C24[k + w->unk2C], gUnk_08559C24[k + w->unk30], (u16)w->unk34, 16, gUnk_03001550);
    sub_0800e2dc(2, gUnk_02004B44);
}

void sub_0800e5b0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk04 = (u32)sub_0800e674;
    gUnk_03002490->unk38 = gUnk_08755650;
    sub_08008c64(32);
    sub_0800e7b0();
    if (gUnk_020060D0 == 4)
        sub_0800e2dc(3, gUnk_0200B074);
    gUnk_03002490->unk28 = gUnk_0200B074;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk4C = 0x680000;
    while (1) {
        gUnk_03002490->unk50 = (gUnk_08731E4C[gUnk_02000018] + gUnk_08731E52[gUnk_02000018] * gUnk_0200B074) << 16;
        if (gUnk_020060D0 == 1 || gUnk_020060D0 == 8)
            break;
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_0800e674(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u, *v, *w;
    u32 *tbl;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gUnk_03002490;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gUnk_03002490->unk34 = 0;
    }
    v = gUnk_03002490;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gUnk_020060D0 == 4) {
        w = gUnk_03002490;
        sub_08003014(gUnk_0855D2F8[w->unk2C], gUnk_0855D2F8[w->unk30], (u16)w->unk34, 10, gUnk_0300153C);
        if (gUnk_0200B074 != gUnk_03002490->unk28) {
            sub_0800e7b0();
            if (gUnk_020060D0 == 4)
                sub_0800e2dc(3, gUnk_0200B074);
            gUnk_03002490->unk28 = gUnk_0200B074;
        }
    } else {
        sub_08003014(gUnk_0855D320, gUnk_0855D320, (u16)gUnk_03002490->unk34, 10, gUnk_0300153C);
        gUnk_03002490->unk28 = -1;
    }
    if (sub_0800ffe8()) {
        tbl = gUnk_08755650;
        sub_08001a94(6, tbl[13], 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
        sub_08001a94(10, tbl[12], 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    }
}

void sub_0800e7b0(void)
{
    s32 i;
    u16 *p;

    for (i = 0; i < gUnk_02000018 + 3; i++) {
        p = gUnk_030012F0;
        if (i == gUnk_0200B074)
            sub_080017e4(2, (u32)gUnk_08559CE6, (u32)&p[i * 3 + 1], 6);
        else
            sub_080017e4(2, (u32)gUnk_08559CEC, (u32)&p[i * 3 + 1], 6);
    }
}

void sub_0800e81c(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = (u32)sub_0800e8c0;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_08755650;
    gUnk_03002490->unk3C = 6;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk4C = 0xA80000;
    gUnk_03002490->unk50 = (gUnk_08731E4C[gUnk_02000018] + gUnk_08731E52[gUnk_02000018] * gUnk_0200B074) << 16;
    while (gUnk_020060D0 == 5)
        TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0800e8c0(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gUnk_03002490;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gUnk_03002490->unk34 = 0;
    }
    v = gUnk_03002490;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gUnk_020060D0 == 5) {
        k = gUnk_02007D34 * 2;
        w = gUnk_03002490;
        sub_08003014(gUnk_0855D334[k + w->unk2C], gUnk_0855D334[k + w->unk30], (u16)w->unk34, 16, gUnk_03001550);
        sub_0800e2dc(4, gUnk_0200B074 * 2 + gUnk_02007D34);
    } else {
        sub_08003014((u16 *)gUnk_0855D334 + (gUnk_02007D34 + 4) * 16, (u16 *)gUnk_0855D334 + (gUnk_02007D34 + 4) * 16, (u16)gUnk_03002490->unk34, 16, gUnk_03001550);
    }
}

void sub_0800e9a4(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk04 = (u32)sub_0800ea0c;
    gUnk_03002490->unk38 = gUnk_08755650;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk4C = 0x800000;
    gUnk_03002490->unk50 = 0x680000;
    while (gUnk_020060D0 == 6 && gUnk_0200618C != 2)
        TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}
