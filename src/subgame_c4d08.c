#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c4d08.c (0x080C4D08-0x080C5283, issue #98).
 *
 * Sub-game 2: the racers' motion and drawing, the sprite scaler and the
 * script cursor.
 * 
 *   sub_080c4ea8   a racer's Task.unk00 callback: Task.unk54 (speed) +=
 *       Task.unk5C (acceleration), with a catch-up bonus for the computer
 *       racers behind the leader (M37Game.unk1B8), capped by the level's
 *       gUnk_080CFE3C[level][0]; then Task.unk4C += speed.
 *   sub_080c4d08 / sub_080c4e10   the Task.unk0C draw callbacks of the
 *       racers (with a blinking extra sprite for three poses) and of the
 *       effect sprites (which end themselves off screen), both drawn scaled
 *       through sub_080c4f60.
 *   sub_080c4f60   copies a sprite's OAM list into M37Game.unk304/unk306[]
 *       with its size and offsets scaled by `scale` (the depth table through
 *       sub_080c623c, OBJ sizes from gUnk_080CFF76, double-size affine
 *       objects), fills the affine matrix gUnk_03000B1C of the OAM shadow
 *       gUnk_03000050 and returns the address of the first entry written.
 *       Its loop is a goto loop: a do/while hoists the (s16) conversion of
 *       the scale.
 *   sub_080c51c0 / sub_080c51d4 / sub_080c523c   the script cursor
 *       gUnk_03006928: clear it, step it (u16 pairs from gUnk_087572EC[id],
 *       0x8000 ends the script, 0x9999 restarts it; the pair lands in unk2/
 *       unk4, which sub_080c3670 copies into the course record), and start
 *       script id 1-4 unless a higher-priority one is running. */

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

/* gUnk_03006928: a cursor into one of the u16-pair scripts gUnk_087572EC[]
   (sub_080c51c0 clears it, sub_080c51d4 steps it, 0x8000 = end, 0x9999 =
   loop) */
struct M37Script
{
    /*0x00*/ u8 unk0;           /* script id, 0 = none */
    /*0x01*/ u8 unk1;           /* step */
    /*0x02*/ s16 unk2;
    /*0x04*/ s16 unk4;
};

extern struct M37Game *gUnk_02017094;
extern u16 gUnk_02017170;
extern struct M37Script gUnk_03006928;
extern u8 gUnk_080CFE2C[][4];
extern s32 gUnk_080CFE3C[][3];
extern u8 gUnk_080CFF76[4][4][2];    /* OBJ shape/size -> {width, height} */
extern vu16 gUnk_03000B1C;             /* affine matrix index */
extern vu16 gUnk_03000050[];           /* OAM shadow (attrs + affine params) */
extern u16 *gUnk_087572EC[];

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
void sub_08005654(s32 id);                                   /* kill task */
s32 sub_080c623c(s32 x);
s32 sub_080c6258(s32 value);                                 /* PR #133: value / 2 */
u32 sub_080c4f60(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */

void sub_080c4d08(void)
{
    struct Task *t = gUnk_03002490;
    u32 *tbl = t->unk38;
    u32 *p;
    s32 n;

    if (tbl != NULL && t->unk3C != -1 && (u16)(t->unk48 + 63) <= 366
        && t->unk4A > -64 && t->unk4A < 224) {
        p = tbl;
        if (gUnk_02017170 & 2) {
            n = 0;
            switch (t->unk3C) {
            case 0:
                n = 17;
                break;
            case 3:
                n = 18;
                break;
            case 6:
                n = 19;
                break;
            }
            if (n != 0)
                sub_08001a94(gUnk_03002490->unk42, sub_080c4f60((u16 *)p[n], gUnk_03002490->unk28),
                             gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk48, gUnk_03002490->unk4A);
        }
        sub_08001a94(gUnk_03002490->unk42, sub_080c4f60((u16 *)p[gUnk_03002490->unk3C], gUnk_03002490->unk28),
                     gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    }
}

void sub_080c4e10(void)
{
    struct Task *t = gUnk_03002490;
    u32 *tbl = t->unk38;

    if (tbl != NULL && t->unk3C != -1) {
        if ((u16)(t->unk48 + 63) <= 366 && t->unk4A > -64 && t->unk4A < 224)
            sub_08001a94(gUnk_03002490->unk42, sub_080c4f60((u16 *)tbl[t->unk3C], t->unk28),
                         gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk48, gUnk_03002490->unk4A);
        else
            sub_08005654(gCurTaskIdx);
    }
}

void sub_080c4ea8(void)
{
    struct M37Game *g = gUnk_02017094;
    u16 n = g->unk448;
    struct Task *t;
    s32 v;
    s32 d;

    if (n <= 1 || gUnk_080CFE2C[g->unk446][gUnk_03002490->unk1C] >= n) {
        gUnk_03002490->unk54 += gUnk_03002490->unk5C;
    } else {
        t = gUnk_03002490;
        v = t->unk5C;
        if (v > 0) {
            d = g->unk1B8 - (t->unk4C >> 16);
            if (d > 256)
                d = 256;
            t->unk54 += v + ((v * d) >> 8);
        } else {
            t->unk54 += v;
        }
    }
    if (gUnk_03002490->unk54 > 0) {
        if (gUnk_03002490->unk54 > gUnk_080CFE3C[gUnk_02017094->unk000][0])
            gUnk_03002490->unk54 = gUnk_080CFE3C[gUnk_02017094->unk000][0];
    } else if (gUnk_03002490->unk54 < 0) {
        gUnk_03002490->unk54 = 0;
    }
    gUnk_03002490->unk4C += gUnk_03002490->unk54;
}

u32 sub_080c4f60(u16 *src, s16 scale)
{
    u16 s = scale;
    u32 ret;
    u16 a, b;
    s32 w, h;
    u16 dbl;
    u16 half;
    s32 k;
    s32 v;

    ret = (u32)&gUnk_02017094->unk306[gUnk_02017094->unk304];
loop:
    a = *src++;
    b = *src++;
    w = gUnk_080CFF76[a >> 14][b >> 14][0];
    h = gUnk_080CFF76[a >> 14][b >> 14][1];
    half = sub_080c6258((s16)s);
    if (((s16)half & 0xFF00) || (w == 8 && h == 8))
        dbl = 0;
    else
        dbl = 0x200;
    k = sub_080c623c((s16)s);
    v = a & 0xFF;
    if (v & 0x80)
        v |= 0xFFFFFF00;
    if (dbl != 0)
        h -= (k * (h >> 1)) >> 8;
    else
        h = h - ((k * (h >> 1)) >> 8) - (h >> 1);
    v = k * v;
    if (v < 0)
        v += 128;
    else
        v -= 128;
    v >>= 8;
    v -= h;
    v &= 0xFF;
    gUnk_02017094->unk306[gUnk_02017094->unk304++] = (s16)((a & 0xFF00) | v | 0x100) | (s16)dbl;
    v = b & 0x1FF;
    if (v & 0x100)
        v |= 0xFFFFFF00;
    if (dbl != 0)
        w -= (k * (w >> 1)) >> 8;
    else
        w = w - ((k * (w >> 1)) >> 8) - (w >> 1);
    v = k * v;
    if (v < 0)
        v += 128;
    else
        v -= 128;
    v >>= 8;
    v -= w;
    v &= 0x1FF;
    gUnk_02017094->unk306[gUnk_02017094->unk304++] = (b & 0xC000) | v | (gUnk_03000B1C << 9);
    gUnk_02017094->unk306[gUnk_02017094->unk304++] = 0;
    src++;
    gUnk_02017094->unk306[gUnk_02017094->unk304++] = *src++ & 0xF3FF;
    if (!(a & 0x1000))
        goto loop;
    gUnk_03000050[(s16)gUnk_03000B1C * 16 + 3] = half;
    gUnk_03000050[(s16)gUnk_03000B1C * 16 + 7] = 0;
    gUnk_03000050[(s16)gUnk_03000B1C * 16 + 11] = 0;
    gUnk_03000050[(s16)gUnk_03000B1C * 16 + 15] = half;
    gUnk_03000B1C++;
    return ret;
}

void sub_080c51c0(void)
{
    gUnk_03006928.unk0 = 0;
    gUnk_03006928.unk2 = 0;
    gUnk_03006928.unk4 = 0;
    gUnk_03006928.unk1 = 0;
}

void sub_080c51d4(void)
{
    u16 *p;

    if (gUnk_03006928.unk0 != 0) {
        p = gUnk_087572EC[gUnk_03006928.unk0];
        switch (p[gUnk_03006928.unk1 * 2]) {
        case 0x8000:
            gUnk_03006928.unk0 = 0;
            gUnk_03006928.unk2 = 0;
            gUnk_03006928.unk4 = 0;
            gUnk_03006928.unk1 = 0;
            break;
        case 0x9999:
            gUnk_03006928.unk1 = 0;
        default:
            gUnk_03006928.unk2 = p[gUnk_03006928.unk1 * 2];
            gUnk_03006928.unk4 = p[gUnk_03006928.unk1 * 2 + 1];
            gUnk_03006928.unk1++;
            break;
        }
    }
}

void sub_080c523c(u16 id)
{
    if (id <= 4) {
        if (id == 0) {
            gUnk_03006928.unk0 = id;
            gUnk_03006928.unk2 = id;
            gUnk_03006928.unk4 = id;
            gUnk_03006928.unk1 = 0;
        } else if ((u8)(gUnk_03006928.unk0 - 1) > 3 || gUnk_03006928.unk0 <= id) {
            gUnk_03006928.unk0 = id;
            gUnk_03006928.unk2 = 0;
            gUnk_03006928.unk4 = 0;
            gUnk_03006928.unk1 = 0;
        }
    }
}
