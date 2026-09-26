#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c243c.c (0x080C243C-0x080C2FF7, issue #98).
 *
 * Sub-game 2: the results screen (phase 1, gUnk_087572CC entry 1) and the
 * race screen's sky.
 * 
 *   sub_080c243c   the results task body: ranks the four racers by their
 *       frame counts M37Game.unk004[] (a bubble sort into gUnk_02017140.unk04,
 *       ties share a place in unk0C), picks each one's score (course record
 *       unk20 or unk24) and installs sub_080c25c4 / sub_080c2b8c.
 *   sub_080c25c4   draws the table: time (sub_080c4ac4), score
 *       (sub_080c4bec), the racer's sprite (sub_080c4c30, the winner pulsing)
 *       and the place or the stars won, then the two palette cycles
 *       (sub_080c2ba8).
 *   sub_080c2740   the results state machine (gUnk_02017140.unk00): a place
 *       jingle (songs 0x81D/0x81C/0x817), in the extra mode (gUnk_03002150 ==
 *       5) the stars counted up and handed to the linked players
 *       (sub_08009eb8) before M35's sub_080b9d24 ends the screen, otherwise a
 *       retry/quit choice and a three-way level choice passed to M35's
 *       sub_080b9d0c.
 *   sub_080c2b8c / sub_080c2ba8 / sub_080c2ccc   the per-frame callback, the
 *       two palette cycles through gUnk_08609F40 with the cursor sprite, and
 *       the cycle's start row for a menu choice.
 *   sub_080c2d38   the per-frame hook sub_080c20b4 installs: builds the
 *       160-line sky gradient M37Game.unk1BC[] from eleven RGB key colours,
 *       fading from gUnk_080CFE60 to gUnk_080CFEA2 (level 2: gUnk_080CFE81 to
 *       gUnk_080CFEC3) between frames 0x4B0 and 0x8B0 of M37Game.unk300.  It
 *       reads one key past the end of the eleven-entry local table, as the
 *       ROM does.
 *   sub_080c2fb8   the VBlank hook: sets backdrop colour 0 and re-arms DMA0
 *       to copy that gradient into it on every HBlank (0xA2400001). */

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

/* the results screen's state (sub_080c243c, sub_080c25c4, sub_080c2740) */
struct M37Results
{
    /*0x00*/ s8 unk00;          /* state */
    /*0x01*/ s8 unk01;
    /*0x02*/ s8 unk02;
    /*0x03*/ s8 unk03;
    /*0x04*/ u8 unk04[4];       /* the players, sorted by score */
    /*0x08*/ u8 unk08[4];       /* each player's index into unk04 */
    /*0x0C*/ u8 unk0C[4];       /* the places, ties shared */
    /*0x10*/ s32 unk10;         /* the winner's pulsing scale */
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;         /* frame timer */
    /*0x1C*/ s32 unk1C[4];
};

extern struct M37Game gUnk_02016C40;
extern struct M37Game *gUnk_02017094;
extern struct M37Course *gUnk_0201716C;
extern u16 gUnk_02017170;
extern struct M37Results gUnk_02017140;
extern u8 gUnk_080CFE2C[][4];
extern u16 gUnk_03002150;
extern vu16 gUnk_03001EB8[];
extern u32 gUnk_0875602C[];
extern u16 gUnk_08609F40[][16];
extern u16 gUnk_03001510[];
extern u8 gUnk_080CFE60[11][3];
extern u8 gUnk_080CFE81[11][3];
extern u8 gUnk_080CFEA2[11][3];
extern u8 gUnk_080CFEC3[11][3];

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);
void sub_08006138(void);                                     /* end the running task */
s32 sub_08009eb8(s32 a, u32 b);
void sub_080b9d0c(s32 a0);
void sub_080b9d24(void);
void sub_080b9e30(void);
void sub_080c4a48(s32 pal);
void sub_080c4a94(s32 idx, s32 x, s32 y);
void sub_080c4ac4(s32 t, s32 x, s32 y);                   /* draw a frame count as ss:cc */
void sub_080c4bec(s32 a, s32 b, s32 x, s32 y);            /* draw min(a * 1000 / b, 1000) */
void sub_080c4c30(s32 idx, s32 pal, s32 scale, s32 x, s32 y, u32 layer);
void sub_080c25c4(void);
void sub_080c2b8c(void);
void sub_080c2ba8(void);
void sub_080c2ccc(s32 mode);

void sub_080c243c(void)
{
    s32 x;
    s32 i;
    s32 j;
    u8 t;

    gUnk_02017140.unk00 = 0;
    gUnk_02017140.unk01 = 0;
    gUnk_02017140.unk02 = 0;
    gUnk_03002490->unk04 = (u32)sub_080c2b8c;
    gUnk_03002490->unk3C = 0xFFFF;
    gUnk_03002490->unk0C = (u32)sub_080c25c4;
    x = gUnk_0201716C->unk014 * 2;
    for (i = 0; i < 4; i++) {
        if (gUnk_02017094->unk01C[i].unk01 == x && gUnk_02017094->unk01C[i].unk0E == x + 1)
            gUnk_02017140.unk1C[i] = gUnk_0201716C->unk018[i].unk20;
        else
            gUnk_02017140.unk1C[i] = gUnk_0201716C->unk018[i].unk24;
    }
    for (i = 0; i < 4; i++)
        gUnk_02017140.unk04[gUnk_080CFE2C[gUnk_02017094->unk446][i]] = i;
    for (j = 3; j > 0; j--) {
        for (i = 0; i < j; i++) {
            if (gUnk_02017094->unk004[gUnk_02017140.unk04[i]] > gUnk_02017094->unk004[gUnk_02017140.unk04[i + 1]]) {
                t = gUnk_02017140.unk04[i];
                gUnk_02017140.unk04[i] = gUnk_02017140.unk04[i + 1];
                gUnk_02017140.unk04[i + 1] = t;
            }
        }
    }
    for (i = 0; i < 4; i++)
        gUnk_02017140.unk08[gUnk_02017140.unk04[i]] = i;
    j = 0;
    for (i = 0; i < 3; i++) {
        if (gUnk_02017094->unk004[gUnk_02017140.unk04[i]] < gUnk_02017094->unk004[gUnk_02017140.unk04[i + 1]]) {
            gUnk_02017140.unk0C[i] = j;
            j = i + 1;
            gUnk_02017140.unk0C[i + 1] = j;
        } else {
            gUnk_02017140.unk0C[i] = gUnk_02017140.unk0C[i + 1] = j;
        }
    }
    gUnk_02017140.unk10 = 0x200;
    gUnk_02017140.unk14 = -4;
    sub_08006138();
}

void sub_080c25c4(void)
{
    s32 i;
    s32 y;
    u8 p;
    s32 scale;
    u16 blink;
    s32 pal;
    s32 layer;

    for (i = 0; i < 4; i++) {
        y = i * 32 + 43;
        p = gUnk_02017140.unk04[i];
        scale = 0x200;
        if (p == 0)
            scale = gUnk_02017140.unk10;
        sub_080c4a48(8);
        sub_080c4ac4(gUnk_02017094->unk004[p], 114, y);
        sub_080c4bec(gUnk_02017140.unk1C[p], gUnk_0201716C->unk018[p].unk20, 180, y);
        pal = gUnk_080CFE2C[gUnk_02017094->unk446][p];
        layer = 4;
        if (p == 0)
            layer = 3;
        sub_080c4c30(0, pal, scale, 76, y, layer);
        if (gUnk_02017140.unk00 != 3) {
            if (gUnk_02017140.unk00 > 3 && gUnk_03002150 == 5)
                blink = gUnk_02017170 & 64;
            else
                blink = 1;
            if (blink)
                goto place;
        }
        if (p != 0) {
        place:
            sub_080c4a94(gUnk_02017140.unk0C[i] + 2, 28, y);
        } else if (gUnk_02017140.unk01 != 0 || gUnk_02017140.unk02 != 0) {
            switch (gUnk_02017140.unk01) {
            case 1:
                sub_080c4a94(0, 28, y);
                break;
            case 2:
                sub_080c4a94(0, 32, y);
                sub_080c4a94(0, 24, y);
                break;
            case 3:
                sub_080c4a94(0, 36, y);
                sub_080c4a94(0, 28, y);
                sub_080c4a94(0, 20, y);
                break;
            }
        } else {
            sub_080c4a94(1, 28, y);
        }
    }
    sub_080c2ba8();
}

void sub_080c2740(void)
{
    struct Task *t;
    s32 i;
    u8 pal;

    gUnk_02017140.unk10 += gUnk_02017140.unk14;
    if (gUnk_02017140.unk10 <= 384)
        gUnk_02017140.unk14 = 4;
    if (gUnk_02017140.unk10 > 507)
        gUnk_02017140.unk14 = -4;
    switch (gUnk_02017140.unk00) {
    case 0:
        gUnk_02017140.unk18 = 0;
        gUnk_02017140.unk00++;
        break;
    case 1:
        if (++gUnk_02017140.unk18 > 10) {
            switch (gUnk_02017140.unk0C[gUnk_02017140.unk08[0]]) {
            case 0:
                sub_08003110(0x81D);
                break;
            case 1:
            case 2:
                sub_08003110(0x81C);
                break;
            case 3:
                sub_08003110(0x817);
                break;
            }
            gUnk_02017140.unk18 = 0;
            gUnk_02017140.unk00++;
        }
        break;
    case 2:
        if (++gUnk_02017140.unk18 > 174) {
            if (gUnk_03002150 != 5) {
                gUnk_02017140.unk00 = 4;
            } else {
                gUnk_02017140.unk18 = 20;
                gUnk_02017140.unk02 = 3 - gUnk_02017140.unk0C[gUnk_02017140.unk08[0]];
                gUnk_02017140.unk03 = 4;
                gUnk_02017140.unk00++;
            }
        }
        break;
    case 3:
        if (gUnk_02017140.unk03 > 0) {
            if (++gUnk_02017140.unk18 > 20) {
                gUnk_02017140.unk18 = 0;
                gUnk_02017140.unk03--;
                if (gUnk_02017140.unk02 > 0) {
                    gUnk_02017140.unk02--;
                    gUnk_02017140.unk01++;
                    sub_080031b8(220);
                }
            }
        } else {
            gUnk_02017140.unk00++;
        }
        break;
    case 4:
        if (gUnk_03001EB8[0] & 9) {
            if (gUnk_03002150 != 5) {
                t = gUnk_03002490;
                t->unk28 = 0;
                t->unk2C = gUnk_02016C40.unk000;
                if (gUnk_02017094->unk446 == 0) {
                    t->unk48 = 120;
                    t->unk4A = 80;
                    t->unk38 = gUnk_0875602C;
                    t->unk3C = 1;
                    t->unk3E = 0x2000;
                    t->unk42 = 1;
                    sub_080c2ccc(-1);
                    sub_080031b8(102);
                }
                gUnk_02017140.unk00++;
            } else {
                for (i = 0; i < 4; i++) {
                    pal = gUnk_080CFE2C[gUnk_02017094->unk446][i];
                    if (pal < gUnk_02017094->unk448) {
                        switch (gUnk_02017140.unk0C[gUnk_02017140.unk08[i]]) {
                        case 0:
                            sub_08009eb8(3, pal);
                            break;
                        case 1:
                            sub_08009eb8(2, pal);
                            break;
                        case 2:
                            sub_08009eb8(1, pal);
                            break;
                        }
                    }
                }
                gUnk_02017140.unk00 = 99;
                sub_080b9d24();
            }
        }
        break;
    case 5:
        if (gUnk_03001EB8[0] & 64) {
            if (--gUnk_03002490->unk28 < 0)
                gUnk_03002490->unk28 = 1;
            if (gUnk_02017094->unk446 == 0) {
                sub_080c2ccc(gUnk_03002490->unk28);
                sub_080031b8(101);
            }
        }
        if (gUnk_03001EB8[0] & 128) {
            if (++gUnk_03002490->unk28 > 1)
                gUnk_03002490->unk28 = 0;
            if (gUnk_02017094->unk446 == 0) {
                sub_080c2ccc(gUnk_03002490->unk28);
                sub_080031b8(101);
            }
        }
        if (gUnk_03001EB8[0] & 9) {
            if (gUnk_03002490->unk28 == 0) {
                gUnk_03002490->unk2C = gUnk_02016C40.unk000;
                if (gUnk_02017094->unk446 == 0) {
                    gUnk_03002490->unk3C = 0;
                    sub_080c2ccc(gUnk_03002490->unk2C);
                }
                gUnk_02017140.unk00++;
            } else {
                sub_080b9d24();
                gUnk_02017140.unk00 = 99;
            }
            if (gUnk_02017094->unk446 == 0)
                sub_080031b8(102);
        }
        break;
    case 6:
        if (gUnk_03001EB8[0] & 64) {
            if (--gUnk_03002490->unk2C < 0)
                gUnk_03002490->unk2C = 2;
            if (gUnk_02017094->unk446 == 0) {
                sub_080c2ccc(gUnk_03002490->unk2C);
                sub_080031b8(101);
            }
        }
        if (gUnk_03001EB8[0] & 128) {
            if (++gUnk_03002490->unk2C > 2)
                gUnk_03002490->unk2C = 0;
            if (gUnk_02017094->unk446 == 0) {
                sub_080c2ccc(gUnk_03002490->unk2C);
                sub_080031b8(101);
            }
        }
        if (gUnk_03001EB8[0] & 9) {
            sub_080b9d0c(gUnk_03002490->unk2C);
            gUnk_02017140.unk00 = 99;
            if (gUnk_02017094->unk446 == 0)
                sub_080031b8(102);
        } else if (gUnk_03001EB8[0] & 2) {
            if (gUnk_02017094->unk446 == 0) {
                gUnk_03002490->unk3C = 1;
                sub_080c2ccc(gUnk_03002490->unk28);
                sub_080031b8(215);
            }
            gUnk_02017140.unk00--;
        }
        break;
    }
}

void sub_080c2b8c(void)
{
    sub_080c2740();
    gUnk_02017170++;
    sub_080b9e30();
}

void sub_080c2ba8(void)
{
    struct Task *t;
    u32 *tbl;
    s32 from;
    s32 to;
    s32 step;
    s32 step2;

    gUnk_03002490->unk6C--;
    if ((s16)gUnk_03002490->unk6C < 0)
        gUnk_03002490->unk6C = 8;
    from = gUnk_03002490->unk6E;
    to = from + 1;
    step = 8 - (s16)gUnk_03002490->unk6C;
    sub_08003014(gUnk_08609F40[from], gUnk_08609F40[to], (u16)(step * 32), 16, gUnk_03001510);
    gUnk_03002490->unk70--;
    if ((s16)gUnk_03002490->unk70 < 0)
        gUnk_03002490->unk70 = 8;
    step2 = 8 - (s16)gUnk_03002490->unk70;
    sub_08003014(gUnk_08609F40[6], gUnk_08609F40[7], (u16)(step2 * 32), 16, &gUnk_03001510[16]);
    if (gUnk_02017094->unk446 == 0) {
        t = gUnk_03002490;
        tbl = t->unk38;
        if (tbl != NULL && t->unk3C != -1) {
            sub_08001a94(t->unk42, tbl[t->unk3C + 2], t->unk3E, 0x6000, t->unk48, t->unk4A);
            t = gUnk_03002490;
            sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, 0x5000, t->unk48, t->unk4A);
        }
    }
}

void sub_080c2ccc(s32 mode)
{
    switch (mode) {
    case 0:
        gUnk_03002490->unk6E = 0;
        break;
    case 1:
        gUnk_03002490->unk6E = 2;
        break;
    case 2:
        gUnk_03002490->unk6E = 4;
        break;
    default:
        gUnk_03002490->unk6E = 0;
        gUnk_03002490->unk70 = 8;
        break;
    }
    gUnk_03002490->unk6C = 8;
}

void sub_080c2d38(void)
{
    s32 col[11][3];
    s32 delta[11][3];
    u32 t;
    s32 i;
    u32 r;
    u32 g;
    u32 b;

    if (gUnk_02017094->unk300 <= 0x4AF)
        t = 0;
    else if (gUnk_02017094->unk300 <= 0x8AF)
        t = (gUnk_02017094->unk300 - 0x4B0) / 4;
    else
        t = 0x100;
    if (gUnk_02016C40.unk000 != 2) {
        for (i = 0; i < 11; i++) {
            col[i][0] = t * (gUnk_080CFEA2[i][0] - gUnk_080CFE60[i][0]) + (gUnk_080CFE60[i][0] << 8);
            col[i][1] = t * (gUnk_080CFEA2[i][1] - gUnk_080CFE60[i][1]) + (gUnk_080CFE60[i][1] << 8);
            col[i][2] = t * (gUnk_080CFEA2[i][2] - gUnk_080CFE60[i][2]) + (gUnk_080CFE60[i][2] << 8);
        }
    } else {
        for (i = 0; i < 11; i++) {
            col[i][0] = t * (gUnk_080CFEC3[i][0] - gUnk_080CFE81[i][0]) + (gUnk_080CFE81[i][0] << 8);
            col[i][1] = t * (gUnk_080CFEC3[i][1] - gUnk_080CFE81[i][1]) + (gUnk_080CFE81[i][1] << 8);
            col[i][2] = t * (gUnk_080CFEC3[i][2] - gUnk_080CFE81[i][2]) + (gUnk_080CFE81[i][2] << 8);
        }
    }
    gUnk_02017094->unk2FC = 0x7DC1;
    for (i = 0; i <= 10; i++) {
        delta[i][0] = (col[i + 1][0] - col[i][0]) / 16;
        delta[i][1] = (col[i + 1][1] - col[i][1]) / 16;
        delta[i][2] = (col[i + 1][2] - col[i][2]) / 16;
    }
    r = col[0][0];
    g = col[0][1];
    b = col[0][2];
    for (i = 0; i < 160; i++) {
        gUnk_02017094->unk1BC[i] = ((b & 0xF800) >> 1) | ((g & 0xF800) >> 6) | ((r & 0xF800) >> 11);
        r += delta[i / 16][0];
        g += delta[i / 16][1];
        b += delta[i / 16][2];
    }
    gUnk_02017094->unk300++;
}

void sub_080c2fb8(void)
{
    vu32 *dma = (vu32 *)0x040000B0;

    REG_DMA0CNT_H = 0;
    *(vu16 *)0x05000000 = gUnk_02017094->unk2FC;
    dma[0] = (u32)gUnk_02017094->unk1BC;
    dma[1] = 0x05000000;
    dma[2] = 0xA2400001;
}
