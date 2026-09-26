#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c4630.c (0x080C4630-0x080C4D07, issue #98).
 *
 * Sub-game 2: task type #96 variant 1 and the module's shared helpers.
 * 
 *   sub_080c46ec / sub_080c4790 / sub_080c4664 / sub_080c4630   variant 1
 *       (one task, index in M37Game.unk44C): seven background objects
 *       (M37Game.unk0EC) that scroll with the camera at their own rate,
 *       re-rolled from LCG stream 4 when they leave the screen (a sprite id
 *       1-4 and one of three height bands gUnk_080CFF60, never the same band
 *       twice in a row).
 *   sub_080c4860 / sub_080c4818   put the variant-1 task's own sprite (the
 *       course line sign) at a course position and move it with the scroll
 *       until it leaves the screen.
 *   sub_080c4974 / sub_080c4890 / sub_080c4a20 / sub_080c495c   four palette
 *       fades (gUnk_020170A0[]): start one (source rows, destination in
 *       gUnk_03001470, period, steps, colour count, repeats), step them every
 *       frame with sub_08003014, free one, free all.
 *   sub_080c4a48 ... sub_080c4c30   the HUD sprites: the digit palette, a
 *       digit, a symbol, a frame count as ss:cc, a number with leading blanks
 *       (a goto loop over the divisors gUnk_080CFF70), a ratio capped at 1000,
 *       and a racer's sprite scaled by sub_080c4f60.
 *   sub_080c4c78 / sub_080c4ca4 / sub_080c4cd4   five LCG streams
 *       M37Game.unk1A4[] (x = (x * 61 + 0x579) & 0xFFF): seed all, step one,
 *       step one and scale it to a range. */

/* 16-byte object records, M37ObjSet.unk04[7] (sub_080c4664, sub_080c4790) */
struct M37Obj
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ s32 unk4;
    /*0x08*/ s32 unk8;          /* 16.16; the ROM also reads its high half */
    /*0x0C*/ s32 unkC;          /* 16.16; the ROM also reads its high half */
};

/* M37Game.unk0EC (0xB8 bytes with the compiler's 2-byte tail pad): task
   type #96 variant 1's seven scrolling objects.  sub_080c4664 addresses an
   object as &set->unk04[i] off the set's own base (`lsls #4; adds #4`), so
   the records are a sub-struct, not flat fields; the two 16-colour rows
   sub_080c20b4 fills end it exactly at M37Game.unk1A4. */
struct M37ObjSet
{
    /*0x00*/ s32 unk00;         /* the scroll position last frame */
    /*0x04*/ struct M37Obj unk04[7];
    /*0x74*/ s16 unk74;         /* the last object's sprite id */
    /*0x76*/ u16 unk76[16];     /* M37Game + 0x162 */
    /*0x96*/ u16 unk96[16];     /* M37Game + 0x182 */
};

/* per-player records, M37Game.unk01C[4] (0x34 bytes) */
struct M37Player
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ s16 unk06;
    /*0x08*/ s16 unk08;
    /*0x0A*/ s16 unk0A;
    /*0x0C*/ s16 unk0C;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ u8 unk20;
    /*0x21*/ u8 unk21;
    /*0x22*/ u8 pad22[2];
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
};

/* gUnk_02016C40, the game's state; always used through gUnk_02017094 */
struct M37Game
{
    /*0x000*/ s32 unk000;       /* the level (M36's sub_080c1f9c copies gUnk_02006168) */
    /*0x004*/ s32 unk004[4];
    /*0x014*/ s32 unk014;
    /*0x018*/ s32 unk018;
    /*0x01C*/ struct M37Player unk01C[4];
    /*0x0EC*/ struct M37ObjSet unk0EC;
    /*0x1A4*/ s32 unk1A4[5];    /* five LCG streams (sub_080c4ca4, sub_080c4cd4) */
    /*0x1B8*/ s32 unk1B8;
    /*0x1BC*/ u16 unk1BC[160]; /* per-scanline colour, HBlank DMA source */
    /*0x2FC*/ u16 unk2FC;
    /*0x2FE*/ u8 pad2FE[2];
    /*0x300*/ u32 unk300;       /* frame counter */
    /*0x304*/ s16 unk304;       /* sub_080c4f60's OAM list: entry count */
    /*0x306*/ s16 unk306[160];  /* ... and entries */
    /*0x446*/ u16 unk446;       /* gUnk_03002360 */
    /*0x448*/ u16 unk448;       /* gUnk_0300243C */
    /*0x44A*/ u8 pad44A[2];
    /*0x44C*/ s32 unk44C;       /* a task index into gUnk_03002790 */
    /*0x450*/ u8 unk450;
    /*0x451*/ u8 unk451;
    /*0x452*/ u8 pad452[2];
};

/* four 40-byte records at gUnk_020170A0 (sub_080c4974 fills one) */
struct M37Timer
{
    /*0x00*/ s32 unk00;         /* in use */
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
};

/* per-player records of gUnk_0201B0E0, M37Course.unk018[4] (0x3C bytes) */
struct M37CoursePlayer
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 unk34;
    /*0x38*/ s32 unk38;
};

/* gUnk_0201B0E0, reached through gUnk_0201716C (and directly by the
   0x080C5284-0x080C623C builder) */
struct M37Course
{
    /*0x000*/ s32 unk000;
    /*0x004*/ s32 unk004;
    /*0x008*/ s32 unk008;
    /*0x00C*/ s32 unk00C;
    /*0x010*/ s32 unk010;
    /*0x014*/ s32 unk014;
    /*0x018*/ struct M37CoursePlayer unk018[4];
    /*0x108*/ s32 unk108;
    /*0x10C*/ s32 unk10C;
    /*0x110*/ s32 unk110;
};

extern struct M37Game *gUnk_02017094;
extern struct M37Timer gUnk_020170A0[4];
extern struct M37Course *gUnk_0201716C;
extern u16 gUnk_02017170;
extern u32 gUnk_08755FA8[];
extern s16 gUnk_080CFF60[];
extern u32 gUnk_08755FBC[];
extern u16 gUnk_03001470[];
extern u32 gUnk_08755FEC[];
extern u32 gUnk_0875603C[];
extern s16 gUnk_080CFF70[];
extern u16 *gUnk_08755F54[];

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
u32 sub_08002ec0(void);                                      /* LCG step */
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080055b0(u8 val, s32 idx);
void sub_08005ca0(void);
void sub_08006138(void);                                     /* end the running task */
u32 sub_080c4f60(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */
void sub_080c4790(void);
void sub_080c4a20(s32 i);
u32 sub_080c4cd4(s32 i, u32 range);

void sub_080c4630(s32 idx, s32 x, s32 y, u16 attr)
{
    u32 *tbl = gUnk_08755FA8;
    u32 c = 0x6000;

    sub_08001a94(9, tbl[idx], c, attr, x, y);
}

void sub_080c4664(s32 i)
{
    struct M37Game *g = gUnk_02017094;
    struct M37ObjSet *set = &g->unk0EC;
    struct M37Obj *o = &set->unk04[i];
    u32 r;
    s32 k;
    s16 id;
    s16 *tbl;

    r = sub_080c4cd4(4, 53);
    k = r & 7;
    tbl = gUnk_080CFF60;
    id = tbl[k];
    while (g->unk0EC.unk74 == id) {
        k = (k + 1) & 7;
        id = tbl[k];
    }
    set->unk74 = id;
    o->unk4 = (id + r) << 16;
    o->unk0 = sub_080c4cd4(4, 4) + 1;
    o->unk2 = 0xA000;
}

void sub_080c46ec(void)
{
    struct M37Game *g = gUnk_02017094;
    s32 *last = &g->unk0EC.unk00;
    s32 i;
    s32 x;
    s32 *p;

    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk42 = 3;
    gUnk_03002490->unk3E = 0;
    gUnk_03002490->unk4A = 40;
    gUnk_03002490->unk38 = gUnk_08755FBC;
    gUnk_03002490->unk3C = 0xFFFF;
    g->unk0EC.unk74 = 0;
    for (i = 0, p = &g->unk0EC.unk04[0].unk8, x = 0; i <= 6; i++) {
        sub_080c4664(i);
        *p = x;
        p += 4;
        x += 0x340000;
    }
    *last = gUnk_0201716C->unk000;
    gUnk_03002490->unk04 = (u32)sub_080c4790;
    sub_080055b0(1, gCurTaskIdx);
    sub_08006138();
}

void sub_080c4790(void)
{
    struct M37Game *g = gUnk_02017094;
    s32 *last = &g->unk0EC.unk00;
    s32 i;
    struct M37Obj *o;

    for (i = 0, o = g->unk0EC.unk04; i <= 6; o++, i++) {
        struct M37Course *c = gUnk_0201716C;
        s32 x;

        o->unkC = o->unk4 - ((c->unk018[0].unk10 - 160) << 16) / 4;
        x = o->unk8;
        x += 0xFFFF0000;
        x += (*last - c->unk000) << 16;
        o->unk8 = x;
        if (x >> 16 < -64) {
            sub_080c4664(i);
            o->unk8 += 0x1700000;
        }
        sub_080c4630(o->unk0, o->unk8 >> 16, o->unkC >> 16, o->unk2);
    }
    *last = gUnk_0201716C->unk000;
}

void sub_080c4818(void)
{
    struct Task *t = gUnk_03002490;

    t->unk48 = (t->unk4C >> 16) - gUnk_0201716C->unk000 + 120;
    t->unk3C = gUnk_02017170 & 1;
    if (t->unk48 < -120) {
        t->unk3C = 0xFFFF;
        t->unk08 = 0;
    }
}

void sub_080c4860(s32 y)
{
    struct Task *t = &gUnk_03002790[gUnk_02017094->unk44C];

    t->unk4C = y << 16;
    t->unk08 = (u32)sub_080c4818;
}

void sub_080c4890(void)
{
    s32 i;

    for (i = 0; i <= 3; i++) {
        if (gUnk_020170A0[i].unk00 != 0) {
            if (--gUnk_020170A0[i].unk04 < 0) {
                gUnk_020170A0[i].unk04 = gUnk_020170A0[i].unk08;
                if (++gUnk_020170A0[i].unk0C >= gUnk_020170A0[i].unk10) {
                    if (gUnk_020170A0[i].unk24 == 0 || --gUnk_020170A0[i].unk24 > 0)
                        gUnk_020170A0[i].unk0C = 0;
                    else {
                        sub_080c4a20(i);
                        continue;
                    }
                }
            }
        {
            s32 k = gUnk_020170A0[i].unk0C;
            s32 k1 = k + 1;
            s32 r = (gUnk_020170A0[i].unk08 - gUnk_020170A0[i].unk04) * gUnk_020170A0[i].unk14;
            u16 *pal = (u16 *)gUnk_020170A0[i].unk1C;
            sub_08003014(pal + k * 16, pal + k1 * 16, (u16)r, (u16)gUnk_020170A0[i].unk18, (u16 *)gUnk_020170A0[i].unk20);
        }
        }
    }
}

void sub_080c495c(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gUnk_020170A0[i].unk00 = 0;
}

s32 sub_080c4974(u16 *src, s32 pal, s32 period, s32 steps, s32 count, s32 repeat)
{
    s32 i;

    for (i = 0; i < 4; i++)
        if (gUnk_020170A0[i].unk00 == 0)
            break;
    if (i > 3)
        while (1)
            ;
    gUnk_020170A0[i].unk00 = 1;
    gUnk_020170A0[i].unk04 = period;
    gUnk_020170A0[i].unk08 = period;
    gUnk_020170A0[i].unk0C = 0;
    gUnk_020170A0[i].unk10 = steps - 1;
    gUnk_020170A0[i].unk14 = Div(256, period);
    gUnk_020170A0[i].unk18 = count;
    gUnk_020170A0[i].unk1C = (s32)src;
    gUnk_020170A0[i].unk20 = (s32)&gUnk_03001470[pal];
    gUnk_020170A0[i].unk24 = repeat;
    return i;
}

void sub_080c4a20(s32 i)
{
    if (i > 3 || gUnk_020170A0[i].unk00 == 0)
        while (1)
            ;
    gUnk_020170A0[i].unk00 = 0;
}

void sub_080c4a48(s32 pal)
{
    gUnk_02017094->unk018 = (pal << 12) & 0xF000;
}

void sub_080c4a5c(s32 digit, s32 x, s32 y)
{
    sub_08001a94(2, gUnk_08755FEC[digit], 0x2000, gUnk_02017094->unk018, x, y);
}

void sub_080c4a94(s32 idx, s32 x, s32 y)
{
    sub_08001a94(2, gUnk_0875603C[idx], 0x2000, 0, x, y);
}

void sub_080c4ac4(s32 t, s32 x, s32 y)
{
    s32 frac;
    s32 sec;

    frac = Mod(t, 60);
    sec = Div(t, 60);
    if (sec > 99) {
        sec = 99;
        frac = 99;
    } else {
        frac *= 0x411A;
        frac = Div(frac, 10000);
        frac += sec & 1;
    }
    sub_080c4a5c(Mod(frac, 10) + 2, x + 36, y);
    sub_080c4a5c(Div(frac, 10) + 2, x + 27, y);
    sub_080c4a5c(12, x + 18, y);
    sub_080c4a5c(Mod(sec, 10) + 2, x + 9, y);
    sub_080c4a5c(Div(sec, 10) + 2, x, y);
}

void sub_080c4b64(s32 n, s32 x, s32 y)
{
    s16 d[3];
    s32 i;
    s32 shown;
    s16 *dp;
    s16 *div;
    s16 *tbl;

    i = 0;
    shown = 0;
    dp = &d[2];
    tbl = gUnk_080CFF70;
    div = tbl + 2;
loop:
    *dp = Div(n, *div);
    if (shown != 0 || *dp != 0) {
        sub_080c4a5c(*dp + 2, i * 9 + x, y);
        shown = 1;
    }
    n = Mod(n, *div);
    i++;
    if (div == tbl)
        sub_080c4a5c(n + 2, i * 9 + x, y);
    dp--;
    div--;
    if (i <= 2)
        goto loop;
}

void sub_080c4bec(s32 a, s32 b, s32 x, s32 y)
{
    s16 n;

    n = b != 0 ? Div(a * 1000, b) : 0;
    if (n > 1000)
        n = 1000;
    sub_080c4b64(n, x, y);
}

void sub_080c4c30(s32 idx, s32 pal, s32 scale, s32 x, s32 y, u32 layer)
{
    sub_08001a94(layer, sub_080c4f60(gUnk_08755F54[idx], scale), 0x2000, (pal << 12) & 0xF000, x, y);
}

void sub_080c4c78(void)
{
    u32 seed;
    s32 i;

    seed = sub_08002ec0();
    for (i = 0; i <= 4; i++)
        gUnk_02017094->unk1A4[i] = seed;
}

u32 sub_080c4ca4(s32 i)
{
    return gUnk_02017094->unk1A4[i] = (gUnk_02017094->unk1A4[i] * 61 + 0x579) & 0xFFF;
}

u32 sub_080c4cd4(s32 i, u32 range)
{
    u32 x;

    gUnk_02017094->unk1A4[i] = x = (gUnk_02017094->unk1A4[i] * 61 + 0x579) & 0xFFF;
    return (x * range) >> 12;
}
