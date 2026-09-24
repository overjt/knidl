#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menutask_0daf8.c (0x0800DAF8-0x0800E313, issue #99).
 *
 * Menu sprite tasks, first part: the file-select screen.  Task types #238
 * and #239 (sub_0800daf8, sub_0800db64 with body sub_0800dbdc; three of
 * each, spawned by sub_0800da9c) slide the three save-slot sprites in with
 * sub_0800dc98 and recolour a slot when the cursor moves; #240
 * (sub_0800dcd8, body sub_0800dda0) is the cursor; #241 (sub_0800de6c,
 * body sub_0800dfdc) a sprite group that slides with the screen; #242
 * (sub_0800e0c0, body sub_0800e148) the file-menu highlight, which
 * marks the selected entry (sub_0800e28c) and loads its picture
 * (sub_0800e2dc: LZ77 into 0x02020000, one 2 KiB part to 0x06004200). */

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

extern s8 gUnk_020055E4;
extern s8 gUnk_020060D0;
extern s8 gUnk_0200B074;
extern struct SaveSlot gUnk_0200E600[];
extern u32 gUnk_02020000[];
extern u16 gUnk_03001270[];
extern u16 gUnk_0300153C[];
extern u16 gUnk_03001668[];
extern s32 gUnk_030023E8;
extern u8 gUnk_08550B9C[];
extern u16 gUnk_08554B60[][4];
extern u16 gUnk_08554D7A[];
extern u16 gUnk_08554D80[];
extern u16 gUnk_08559B68[][10];
extern u16 gUnk_08559B90[];
extern void *const gUnk_08731E34[];
extern u32 gUnk_08755620[];
extern u32 gUnk_08755650[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08008c64(u16 a0);
s32 sub_0800bf10(s32 slot, u32 pal);
u8 sub_0800ffe8(void);
void sub_0800dbdc(void);
void sub_0800dc98(void);
void sub_0800dda0(void);
void sub_0800dfdc(void);
void sub_0800e148(void);
void sub_0800e28c(void);
s32 sub_0800e2dc(s32 id, s32 part);

void sub_0800daf8(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_08755620;
    gUnk_03002490->unk3C = gUnk_03002490->unk1C + 3;
    sub_0800dc98();
    while (gUnk_020060D0 != 1)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0x1AE000;
    TaskYieldTrampoline(10);
    TaskDispatchTrampoline();
}

void sub_0800db64(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = (u32)sub_0800dbdc;
    gUnk_03002490->unk42 = 9;
    gUnk_03002490->unk38 = gUnk_08755620;
    gUnk_03002490->unk3C = gUnk_03002490->unk1C;
    gUnk_03002490->unk28 = -1;
    sub_0800dc98();
    while (gUnk_020060D0 != 1)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0x1AE000;
    TaskYieldTrampoline(10);
    TaskDispatchTrampoline();
}

void sub_0800dbdc(void)
{
    struct Task *t;
    struct SaveSlot *s;
    s32 slot;
    s32 i;

    if (gUnk_0200B074 != gUnk_03002490->unk28) {
        slot = gUnk_03002490->unk1C;
        s = gUnk_0200E600;
        i = slot * 256;
        if (gUnk_0200E600[slot].unk12[1] != 0 && gUnk_0200E600[slot].unk04 != 0x99999999)
            i++;
        sub_0800bf10(slot, (s8)s->unk16[i]);
        gUnk_03002490->unk28 = gUnk_0200B074;
    }

    if (gUnk_0200E600[gUnk_03002490->unk1C].unk12[1] != 0 && gUnk_0200E600[gUnk_03002490->unk1C].unk04 != 0x99999999) {
        t = gUnk_03002490;
        sub_08001a94(t->unk42 - 1, gUnk_08755620[t->unk1C + 6], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

void sub_0800dc98(void)
{
    struct Task *t = gUnk_03002490;
    s32 n;

    t->unk4C = 0x1500000;
    n = t->unk1C;
    t->unk50 = ((n + 1) * 5) << 19;
    TaskYieldTrampoline(n * 5);
    gUnk_03002490->unk54 = 0xFFE52000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0;
}

void sub_0800dcd8(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = (u32)sub_0800dda0;
    gUnk_03002490->unk42 = 6;
    gUnk_03002490->unk38 = gUnk_08755620;
    gUnk_03002490->unk3C = 9;
    gUnk_03002490->unk28 = -1;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk4C = 0x160000;
    gUnk_03002490->unk50 = ((gUnk_0200B074 * 5) << 19) + 0x240000;
    gUnk_03002490->unk54 = 0x30000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0;
    while (gUnk_020060D0 != 1) {
        gUnk_03002490->unk50 = ((gUnk_0200B074 * 5) << 19) + 0x240000;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk54 = 0xFFFA0000;
    TaskYieldTrampoline(8);
    TaskDispatchTrampoline();
}

void sub_0800dda0(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u, *v, *w;
    s32 cur;

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
    w = gUnk_03002490;
    sub_08003014(gUnk_08554B60[w->unk2C], gUnk_08554B60[w->unk30], (u16)w->unk34, 4, gUnk_03001668);
    cur = gUnk_0200B074;
    if (cur != gUnk_03002490->unk28) {
        u8 *p;
        sub_080017e4(3, (u32)&gUnk_08550B9C[cur * 192], 0x06013580, 96);
        p = gUnk_08550B9C;
        sub_080017e4(3, (u32)&p[gUnk_0200B074 * 192 + 96], 0x06013980, 96);
        gUnk_03002490->unk28 = gUnk_0200B074;
    }
}

void sub_0800de6c(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u, *v, *w;
    s8 s;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0800dfdc;
    t->unk3E = 0x2000;
    t->unk28 = 0;
    t->unk18 = 0;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk6C = 0;
    for (; (s16)gUnk_03002490->unk6C <= 7; gUnk_03002490->unk6C++) {
        u = gUnk_03002490;
        u->unk18 += 0xFFF90000;
        u->unk20 += 0xFFFA0000;
        TaskYieldTrampoline(1);
        s = gUnk_020060D0;
        if (s == 0 || s == 4 || s == 7 || s == 8) {
            gUnk_03002490->unk28 = 1;
            break;
        }
    }
    for (;;) {
        TaskYieldTrampoline(1);
        s = gUnk_020060D0;
        if (s == 1) {
            v = gUnk_03002490;
            if (v->unk18 < (s32)0xFFC80000) {
                v->unk3E = 0;
                v->unk18 += 0x33000;
                v->unk1C += 0xFFFD2000;
                if (v->unk18 > (s32)0xFFC80000) {
                    v->unk18 = 0xFFC80000;
                    v->unk1C = 0;
                }
            } else {
                v->unk3E = 0x2000;
            }
        } else if (s == 6) {
            v = gUnk_03002490;
            v->unk3E = 0;
            if (v->unk18 > (s32)0xFFA80000) {
                v->unk18 += 0xFFFCD000;
                v->unk1C += 0x2E000;
                if (v->unk18 < (s32)0xFFA80000) {
                    v->unk18 = 0xFFA80000;
                    v->unk1C = 0x1D0000;
                }
            }
        } else if (s == 0 || s == 4 || s == 7 || s == 8) {
            gUnk_03002490->unk28 = 1;
            break;
        }
    }
    gUnk_03002490->unk6C = 0;
    do {
        w = gUnk_03002490;
        w->unk18 += 0x70000;
        w->unk20 += 0x33000;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 7);
    TaskDispatchTrampoline();
}

void sub_0800dfdc(void)
{
    struct Task *t;
    u32 *tbl;

    if (sub_0800ffe8()) {
        tbl = gUnk_08755650;
        sub_08001a94(12, tbl[3], gUnk_03002490->unk3E, 0, (gUnk_03002490->unk18 >> 16) + 304, (gUnk_03002490->unk1C >> 16) + 40);
        sub_08001a94(12, tbl[2], gUnk_03002490->unk3E, 0, (gUnk_03002490->unk18 >> 16) + 304, (gUnk_03002490->unk1C >> 16) + 40);
        sub_08001a94(11, tbl[5], gUnk_03002490->unk3E, 0, (gUnk_03002490->unk18 >> 16) + 304, (gUnk_03002490->unk1C >> 16) + 40);
        sub_08001a94(13, tbl[4], gUnk_03002490->unk3E, 0, (gUnk_03002490->unk20 >> 16) + 296, 40);
        t = gUnk_03002490;
        if (t->unk28 == 0) {
            tbl = gUnk_08755620;
            sub_08001a94(11, tbl[gUnk_030023E8 + 3], t->unk3E, 0, (t->unk18 >> 16) + 234, (t->unk1C >> 16) + 13);
        }
    }
}

void sub_0800e0c0(void)
{
    s8 s;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk04 = (u32)sub_0800e148;
    gUnk_03002490->unk38 = gUnk_08755650;
    gUnk_03002490->unk28 = -1;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk4C = 0x640000;
    for (;;) {
        s = gUnk_020060D0;
        if (s == 0 || s == 4 || s == 7 || s == 8)
            break;
        gUnk_03002490->unk50 = ((gUnk_020055E4 * 3) << 19) + 0x300000;
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_0800e148(void)
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
    if (gUnk_020060D0 == 1) {
        w = gUnk_03002490;
        sub_08003014(gUnk_08559B68[w->unk2C], gUnk_08559B68[w->unk30], (u16)w->unk34, 10, gUnk_0300153C);
        sub_08008c64(23);
        if (gUnk_020055E4 != gUnk_03002490->unk28) {
            sub_0800e28c();
            sub_0800e2dc(0, gUnk_020055E4);
            gUnk_03002490->unk28 = gUnk_020055E4;
        }
    } else {
        sub_08003014(gUnk_08559B90, gUnk_08559B90, (u16)gUnk_03002490->unk34, 10, gUnk_0300153C);
        sub_08008c64(24);
        gUnk_03002490->unk28 = -1;
    }
    if (sub_0800ffe8()) {
        tbl = gUnk_08755650;
        sub_08001a94(6, tbl[1], 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
        sub_08001a94(10, tbl[0], 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    }
}

void sub_0800e28c(void)
{
    s32 i;
    u16 *p;

    for (i = 0; i <= 3; i++) {
        p = gUnk_03001270;
        if (i == gUnk_020055E4)
            sub_080017e4(2, (u32)gUnk_08554D7A, (u32)&p[i * 3 + 1], 6);
        else
            sub_080017e4(2, (u32)gUnk_08554D80, (u32)&p[i * 3 + 1], 6);
    }
}

s32 sub_0800e2dc(s32 id, s32 part)
{
    LZ77UnCompWram(gUnk_08731E34[id], gUnk_02020000);
    sub_080017e4(1, (u32)gUnk_02020000 + (part << 11), 0x06004200, 0x800);
}
