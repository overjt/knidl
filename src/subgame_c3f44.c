#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c3f44.c (0x080C3F44-0x080C462F, issue #98).
 *
 * Sub-game 2: task type #96 variant 2, the racers' effect sprites.
 * 
 *   sub_080c3f44   variant 2's body.  sub_080c2078(a, racer, kind) spawns it
 *       with Task.unk18 = the racer's task, Task.unk20 = the kind and
 *       Task.unk1C = a, and each kind sets its sprite table, animation and one
 *       of the callbacks below.  The kinds as sub_080c38c8/sub_080c3698 use
 *       them (player 0 only unless noted): 0 and 1 every 4th / 8th frame while
 *       A is held on the course, 2 on a press (M37Game.unk450 blocks a second
 *       copy), 5 on the release (M37Game.unk451), 6 / 7 the two boost ratings,
 *       3 / 4 thrown to either side at the start of a penalty, 8 the penalty
 *       itself (for every racer).  Kinds 0 and 3/4 are scaled by
 *       sub_080c623c and scattered with LCG stream 4.
 *   sub_080c42dc   the shared step: copy the racer's layer, priority and
 *       palette bits, and its position unless Task.unk15 is set.
 *   sub_080c4364 ... sub_080c45fc   the per-kind callbacks (Task.unk04):
 *       movement, the Task.unk6C/unk6E animation counters, and
 *       sub_08005654(gCurTaskIdx) when the effect ends. */

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

extern struct M37Game *gUnk_02017094;
extern struct M37Course *gUnk_0201716C;
extern u32 gUnk_08755FC4[];
extern u32 gUnk_08755FEC[];
extern u32 gUnk_087572E0[];
extern s16 gUnk_080CFF52[];

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
void sub_080055b0(u8 val, s32 idx);
void sub_08005654(s32 id);                                   /* kill task */
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08006138(void);                                     /* end the running task */
u32 sub_080c4ca4(s32 i);
void sub_080c4e10(void);
u32 sub_080c4f60(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */
s32 sub_080c623c(s32 x);
void sub_080c4364(void);
void sub_080c43e8(void);
void sub_080c44bc(void);
void sub_080c44f0(void);
void sub_080c4568(void);
void sub_080c45d4(void);
void sub_080c45fc(void);

void sub_080c3f44(void)
{
    struct Task *u = &gUnk_03002790[gUnk_03002490->unk18];
    s32 scale;
    s32 x;
    s32 y;
    s32 vx;
    s32 vy;

    switch (gUnk_03002490->unk20) {
    case 1:
        gUnk_03002490->unk6E = 3;
        gUnk_03002490->unk0C = (u32)sub_080c4e10;
        gUnk_03002490->unk38 = gUnk_08755FC4;
        gUnk_03002490->unk3C = 5;
        gUnk_03002490->unk04 = (u32)sub_080c44bc;
        break;
    case 2:
        gUnk_03002490->unk6C = 0;
        gUnk_03002490->unk6E = 0;
        gUnk_03002490->unk15 = 0;
        gUnk_03002490->unk0C = (u32)sub_080c4e10;
        gUnk_03002490->unk38 = gUnk_08755FC4;
        gUnk_03002490->unk04 = (u32)sub_080c44f0;
        gUnk_02017094->unk450 = 1;
        break;
    case 5:
        gUnk_03002490->unk6C = 2;
        gUnk_03002490->unk6E = 2;
        gUnk_03002490->unk15 = 1;
        gUnk_03002490->unk0C = (u32)sub_080c4e10;
        gUnk_03002490->unk40 = 0x8210;
        gUnk_03002490->unk38 = gUnk_087572E0;
        gUnk_03002490->unk3C = 0;
        gUnk_03002490->unk04 = (u32)sub_080c4568;
        gUnk_02017094->unk451 = 1;
        break;
    case 0:
        scale = sub_080c623c(gUnk_0201716C->unk018[gUnk_03002490->unk18].unk08);
        gUnk_03002490->unk0C = (u32)sub_080c4e10;
        gUnk_03002490->unk42 = u->unk42;
        gUnk_03002490->unk3E = u->unk3E & 0x6000;
        gUnk_03002490->unk00 = (u32)sub_080059d8;
        gUnk_03002490->unk54 = -u->unk54;
        gUnk_03002490->unk28 = u->unk28;
        x = u->unk48 - scale / 32;
        y = u->unk4A + (s32)((sub_080c4ca4(4) & 7) - 3) * scale / 256;
        gUnk_03002490->unk4C = x << 16;
        gUnk_03002490->unk50 = y << 16;
        gUnk_03002490->unk6C = 2;
        gUnk_03002490->unk6E = 18;
        gUnk_03002490->unk38 = gUnk_08755FC4;
        gUnk_03002490->unk3C = 3;
        gUnk_03002490->unk04 = (u32)sub_080c4364;
        break;
    case 3:
    case 4:
        scale = sub_080c623c(gUnk_0201716C->unk018[gUnk_03002490->unk18].unk08);
        gUnk_03002490->unk0C = (u32)sub_080c4e10;
        gUnk_03002490->unk00 = (u32)sub_080059d8;
        gUnk_03002490->unk42 = u->unk42;
        gUnk_03002490->unk3E = u->unk3E & 0x6000;
        vx = (s32)(sub_080c4ca4(4) & 0xFFF) * (scale << 6) / 256;
        if (gUnk_03002490->unk20 == 3)
            vx = -vx;
        vy = -((s32)((sub_080c4ca4(4) & 0xFFF) * 48 + 0x10000) * scale) / 256;
        gUnk_03002490->unk54 = vx;
        gUnk_03002490->unk58 = vy;
        gUnk_03002490->unk60 = scale << 6;
        gUnk_03002490->unk28 = u->unk28;
        x = u->unk48 + (s32)((sub_080c4ca4(4) & 31) - 15) * scale / 256;
        y = u->unk4A - scale / 32;
        gUnk_03002490->unk4C = x << 16;
        gUnk_03002490->unk50 = y << 16;
        gUnk_03002490->unk6C = 2;
        gUnk_03002490->unk6E = 50;
        gUnk_03002490->unk38 = gUnk_08755FC4;
        gUnk_03002490->unk3C = 3;
        gUnk_03002490->unk04 = (u32)sub_080c43e8;
        break;
    case 6:
    case 7:
        gUnk_03002490->unk0C = (u32)sub_08005ca0;
        gUnk_03002490->unk00 = (u32)sub_080059d8;
        gUnk_03002490->unk42 = 3;
        gUnk_03002490->unk3E = 0;
        gUnk_03002490->unk4C = u->unk48 << 16;
        gUnk_03002490->unk50 = u->unk4A << 16;
        gUnk_03002490->unk58 = u->unk4A < 80 ? 0x10000 : -0x10000;
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk6E = 40;
        gUnk_03002490->unk38 = gUnk_08755FEC;
        gUnk_03002490->unk3C = gUnk_03002490->unk20 == 6 ? 15 : 14;
        gUnk_03002490->unk04 = (u32)sub_080c45d4;
        break;
    case 8:
        gUnk_03002490->unk6C = 12;
        gUnk_03002490->unk6E = 9;
        gUnk_03002490->unk15 = 0;
        gUnk_03002490->unk0C = (u32)sub_080c4e10;
        gUnk_03002490->unk38 = gUnk_08755FC4;
        gUnk_03002490->unk3C = 9;
        gUnk_03002490->unk04 = (u32)sub_080c45fc;
        break;
    }
    sub_080055b0(1, gCurTaskIdx);
    sub_08006138();
}

void sub_080c42dc(s32 layer)
{
    struct Task *u = &gUnk_03002790[gUnk_03002490->unk18];

    gUnk_03002490->unk28 = u->unk28;
    gUnk_03002490->unk42 = u->unk42 + layer;
    gUnk_03002490->unk3E = u->unk3E & 0x6000;
    if (gUnk_03002490->unk15 == 0) {
        gUnk_03002490->unk48 = u->unk48;
        gUnk_03002490->unk4A = u->unk4A;
    } else {
        gUnk_03002490->unk48 = gUnk_0201716C->unk018[gUnk_03002490->unk18].unk0C;
        gUnk_03002490->unk4A = gUnk_0201716C->unk018[gUnk_03002490->unk18].unk10;
    }
}

void sub_080c4364(void)
{
    struct Task *u = &gUnk_03002790[gUnk_03002490->unk18];

    gUnk_03002490->unk42 = u->unk42;
    gUnk_03002490->unk3E = u->unk3E & 0x6000;
    gUnk_03002490->unk28 += 14;
    if (--gUnk_03002490->unk6E < 0) {
        sub_08005654(gCurTaskIdx);
    } else {
        if ((s16)gUnk_03002490->unk6C <= 0) {
            gUnk_03002490->unk6C = 2;
            if (--gUnk_03002490->unk3C < 0)
                gUnk_03002490->unk3C = 3;
        }
        gUnk_03002490->unk6C--;
    }
}

void sub_080c43e8(void)
{
    if (--gUnk_03002490->unk6E < 0) {
        sub_08005654(gCurTaskIdx);
        return;
    }
    if ((s16)gUnk_03002490->unk6C <= 0) {
        gUnk_03002490->unk6C = 2;
        if (--gUnk_03002490->unk3C < 0)
            gUnk_03002490->unk3C = 3;
    }
    gUnk_03002490->unk6C--;
    if ((u16)(gUnk_03002490->unk48 + 63) <= 366
        && gUnk_03002490->unk4A > -64 && gUnk_03002490->unk4A < 224) {
        u32 *tbl = gUnk_03002490->unk38;

        sub_08001a94(gUnk_03002490->unk42,
                     sub_080c4f60((u16 *)tbl[gUnk_03002490->unk3C], gUnk_03002490->unk28),
                     gUnk_03002490->unk3E, gUnk_03002490->unk40,
                     gUnk_03002490->unk48, gUnk_03002490->unk4A);
    } else
        sub_08005654(gCurTaskIdx);
}

void sub_080c44bc(void)
{
    if (--gUnk_03002490->unk6E < 0)
        sub_08005654(gCurTaskIdx);
    else
        sub_080c42dc(0);
}

void sub_080c44f0(void)
{
    struct Task *t = gUnk_03002490;

    /* Task.unk6C is u16 in task.h; this callback counts it as s16 */
    if ((*(s16 *)&t->unk6C)-- <= 0) {
        t->unk6C = 2;
        if (gUnk_080CFF52[++t->unk6E] != -1) {
            t->unk3C = gUnk_080CFF52[t->unk6E];
        } else {
            gUnk_02017094->unk450 = 0;
            sub_08005654(gCurTaskIdx);
            return;
        }
    }
    sub_080c42dc(0);
}

void sub_080c4568(void)
{
    if ((s16)gUnk_03002490->unk6C <= 0) {
        gUnk_03002490->unk6C = 2;
        if (++gUnk_03002490->unk3C > gUnk_03002490->unk6E) {
            gUnk_02017094->unk451 = 0;
            sub_08005654(gCurTaskIdx);
            return;
        }
    }
    gUnk_03002490->unk6C--;
    sub_080c42dc(0);
}

void sub_080c45d4(void)
{
    if (--gUnk_03002490->unk6E < 0)
        sub_08005654(gCurTaskIdx);
}

void sub_080c45fc(void)
{
    /* Task.unk6C is u16 in task.h; this callback counts it as s16 */
    if ((*(s16 *)&gUnk_03002490->unk6C)-- <= 0)
        sub_08005654(gCurTaskIdx);
    else
        sub_080c42dc(-1);
}
