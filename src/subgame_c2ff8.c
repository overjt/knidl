#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c2ff8.c (0x080C2FF8-0x080C3647, issue #98).
 *
 * Sub-game 2: task type #96 (class 3) and its variant 0, the racers.
 * 
 *   sub_080c2ff8   the body: sub_08002e98(Task.unk73, 5, gUnk_087572D4), the
 *       three variants sub_080c3018 / sub_080c46ec / sub_080c3f44 (entries
 *       2-4 of gUnk_087572CC; the two words after them are data).
 *   sub_080c3018   variant 0, one per player (Task.unk1C): resets the
 *       player's M37Player record, picks the computer players' speed and
 *       jitter (M37Player.unk28/unk24) from the level M37Game.unk000 when at
 *       most one player is linked (M37Game.unk448), runs until the player
 *       passes the finish line gUnk_0201716C->unk010 (+240), counting frames
 *       in M37Game.unk004[player].
 *   sub_080c3318 / sub_080c33a0   the computer players' input: a target
 *       M37Player.unk30 re-rolled from the LCG around the course record's
 *       unk28, and the resulting "hold A" decision.
 *   sub_080c34ac   variant 0's per-frame callback: reads the player's keys
 *       (gUnk_03000F98/gUnk_03001EB8 for a linked player, sub_080c33a0 for a
 *       computer one) into M37Player.unk02/unk04, counts A presses in unk0E
 *       and publishes the position and the pressed flag in the course
 *       record gUnk_0201716C->unk018[player]. */

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
extern void (*gUnk_087572D4[])(void);
extern u16 *gUnk_08755F54[];
extern u8 gUnk_080CFE2C[][4];
extern vu16 gUnk_03000F98[];
extern vu16 gUnk_03001EB8[];

void TaskYieldTrampoline(s32 frames);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void sub_08006138(void);                                     /* end the running task */
void sub_080c3efc(void);
void sub_080c3f20(void);
u32 sub_080c4cd4(s32 i, u32 range);
void sub_080c4d08(void);
void sub_080c4ea8(void);
void sub_080c3318(s32 player);
void sub_080c34ac(void);

void sub_080c2ff8(void)
{
    sub_08002e98(gUnk_03002490->unk73, 5, gUnk_087572D4);
}

void sub_080c3018(void)
{
    s32 player = gUnk_03002490->unk1C;
    s32 src;

    gUnk_03002490->unk0C = (u32)sub_080c4d08;
    gUnk_03002490->unk00 = (u32)sub_080c4ea8;
    gUnk_03002490->unk38 = (u32 *)gUnk_08755F54;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk40 = gUnk_080CFE2C[gUnk_02017094->unk446][player] << 12;
    gUnk_03002490->unk04 = (u32)sub_080c34ac;
    gUnk_03002490->unk08 = (u32)sub_080c3f20;
    gUnk_03002490->unk14 = 0;
    gUnk_03002490->unk28 = 256;
    gUnk_02017094->unk01C[player].unk14 = 0x80000;
    gUnk_02017094->unk01C[player].unk18 = 0;
    gUnk_02017094->unk01C[player].unk06 = 0;
    gUnk_02017094->unk01C[player].unk08 = 0;
    gUnk_02017094->unk01C[player].unk0C = 0;
    gUnk_02017094->unk01C[player].unk0A = 0;
    gUnk_02017094->unk01C[player].unk00 = 0;
    gUnk_02017094->unk01C[player].unk10 = -9999;
    gUnk_02017094->unk01C[player].unk1C = 0;
    gUnk_02017094->unk01C[player].unk04 = 0;
    gUnk_02017094->unk01C[player].unk02 = 0;
    gUnk_02017094->unk01C[player].unk0E = 0;
    gUnk_02017094->unk01C[player].unk01 = 0;
    gUnk_03002490->unk4C = gUnk_0201716C->unk000 << 16;
    gUnk_03002490->unk54 = 0x28000;
    gUnk_03002490->unk5C = 0;
    gUnk_02017094->unk004[player] = 0;
    gUnk_02017094->unk01C[player].unk20 = 0;
    if (gUnk_02017094->unk448 <= 1) {
        gUnk_02017094->unk01C[player].unk20 = 2;
        if (gUnk_02017094->unk000 == 0) {
            if (player == 1) {
                gUnk_02017094->unk01C[1].unk28 = 2;
                gUnk_02017094->unk01C[1].unk24 = 20;
            } else if (player == 2) {
                gUnk_02017094->unk01C[2].unk28 = 1;
                gUnk_02017094->unk01C[2].unk24 = 15;
            } else if (player == 3) {
                gUnk_02017094->unk01C[3].unk28 = 0;
                gUnk_02017094->unk01C[3].unk24 = 10;
            }
        } else if (gUnk_02017094->unk000 == 1) {
            if (player == 1) {
                gUnk_02017094->unk01C[1].unk28 = 2;
                gUnk_02017094->unk01C[1].unk24 = 5;
            } else if (player == 2) {
                gUnk_02017094->unk01C[2].unk28 = 5;
                gUnk_02017094->unk01C[2].unk24 = 15;
            } else if (player == 3) {
                gUnk_02017094->unk01C[3].unk28 = 10;
                gUnk_02017094->unk01C[3].unk24 = 20;
            }
        } else {
            if (player == 1) {
                gUnk_02017094->unk01C[1].unk28 = 1;
                gUnk_02017094->unk01C[1].unk24 = 2;
            } else if (player == 2) {
                gUnk_02017094->unk01C[2].unk28 = 2;
                gUnk_02017094->unk01C[2].unk24 = 4;
            } else if (player == 3) {
                gUnk_02017094->unk01C[3].unk28 = 3;
                gUnk_02017094->unk01C[3].unk24 = 6;
            }
        }
    } else {
        src = gUnk_080CFE2C[gUnk_02017094->unk446][player];
        if (src >= gUnk_02017094->unk448) {
            gUnk_02017094->unk01C[player].unk20 = src;
            if (src == 2) {
                gUnk_02017094->unk01C[player].unk28 = 5;
                gUnk_02017094->unk01C[player].unk24 = 15;
            } else if (src == 3) {
                gUnk_02017094->unk01C[player].unk28 = 10;
                gUnk_02017094->unk01C[player].unk24 = 5;
            } else {
                while (1)
                    ;
            }
        }
    }
    if (gUnk_02017094->unk01C[player].unk20 != 0)
        sub_080c3318(player);
    gUnk_02017094->unk01C[player].unk21 = 1;
    while (gUnk_0201716C->unk000 < gUnk_0201716C->unk00C)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk14 = 1;
    gUnk_03002490->unk08 = (u32)sub_080c3efc;
    while (gUnk_0201716C->unk018[player].unk00 < gUnk_0201716C->unk010) {
        gUnk_02017094->unk004[player]++;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk14 = 2;
    gUnk_03002490->unk08 = (u32)sub_080c3f20;
    gUnk_03002490->unk5C = 0;
    while (gUnk_0201716C->unk018[player].unk00 < gUnk_0201716C->unk010 + 240)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk54 = 0;
    sub_08006138();
}

void sub_080c3318(s32 player)
{
    s32 speed = gUnk_02017094->unk01C[player].unk28;
    s32 range = gUnk_02017094->unk01C[player].unk24;

    if (gUnk_0201716C->unk018[player].unk14 != 0)
        speed = -speed;
    gUnk_02017094->unk01C[player].unk30 = gUnk_02017094->unk01C[player].unk2C = gUnk_0201716C->unk018[player].unk28;
    gUnk_02017094->unk01C[player].unk30 += speed + sub_080c4cd4(player, range) - range / 2;
}

s32 sub_080c33a0(s32 player, s32 pos)
{
    s32 flag = 0;
    s32 d;

    if (gUnk_0201716C->unk018[player].unk28 > gUnk_02017094->unk01C[player].unk2C)
        sub_080c3318(player);
    d = gUnk_02017094->unk01C[player].unk30 - pos;
    if (gUnk_02017094->unk01C[player].unk20 > 1) {
        if (gUnk_02017094->unk01C[player].unk21) {
            if (d > 0)
                flag = 1;
        } else if (d < 0) {
            flag = 1;
        }
        if (gUnk_02017094->unk01C[player].unk02 != flag)
            gUnk_02017094->unk01C[player].unk21 = !gUnk_02017094->unk01C[player].unk21;
        if (d > 100 && gUnk_0201716C->unk018[player].unk14) {
            flag = 1;
            gUnk_02017094->unk01C[player].unk21 = flag;
        }
    }
    if (gUnk_02017094->unk01C[player].unk20 == 1) {
        if (gUnk_0201716C->unk018[player].unk14) {
            if (d > 0)
                flag = 1;
        } else if (d < 0) {
            flag = 1;
        }
    }
    return flag;
}

void sub_080c34ac(void)
{
    s32 player = gUnk_03002490->unk1C;
    s32 pos = gUnk_03002490->unk4C >> 16;
    u16 prev = gUnk_02017094->unk01C[player].unk02;
    s32 src;

    if (gUnk_02017094->unk01C[player].unk08 != 0)
        gUnk_02017094->unk01C[player].unk08--;
    if (gUnk_02017094->unk01C[player].unk0A != 0)
        gUnk_02017094->unk01C[player].unk0A--;
    if (gUnk_02017094->unk01C[player].unk0C != 0)
        gUnk_02017094->unk01C[player].unk0C--;
    if (pos > gUnk_0201716C->unk010 + 240)
        pos = gUnk_0201716C->unk010 + 240;
    if (gUnk_03002490->unk14 == 1) {
        if (gUnk_02017094->unk448 <= 1) {
            if (player == 0) {
                gUnk_02017094->unk01C[0].unk02 = gUnk_03000F98[0];
                gUnk_02017094->unk01C[0].unk04 = gUnk_03001EB8[0];
            } else {
                u16 keys = sub_080c33a0(player, pos);

                gUnk_02017094->unk01C[player].unk04 = keys & ~gUnk_02017094->unk01C[player].unk02;
                gUnk_02017094->unk01C[player].unk02 = keys;
            }
        } else {
            src = gUnk_080CFE2C[gUnk_02017094->unk446][player];
            if (src < gUnk_02017094->unk448) {
                gUnk_02017094->unk01C[player].unk02 = gUnk_03000F98[src];
                gUnk_02017094->unk01C[player].unk04 = gUnk_03001EB8[src];
            } else {
                u16 keys = sub_080c33a0(player, pos);

                gUnk_02017094->unk01C[player].unk04 = keys & ~gUnk_02017094->unk01C[player].unk02;
                gUnk_02017094->unk01C[player].unk02 = keys;
            }
        }
        if ((prev ^ gUnk_02017094->unk01C[player].unk02) & 1)
            gUnk_02017094->unk01C[player].unk0E++;
    }
    gUnk_0201716C->unk018[player].unk00 = pos;
    if ((gUnk_02017094->unk01C[player].unk02 & 1) && gUnk_02017094->unk01C[player].unk08 == 0)
        gUnk_0201716C->unk018[player].unk04 = 1;
    else
        gUnk_0201716C->unk018[player].unk04 = 0;
}
