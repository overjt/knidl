#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c1ffc.c (0x080C1FFC-0x080C243B, issue #98).
 *
 * Sub-game 2 (gUnk_02007FCC == 2), the race screen: phase 0 of M35's
 * framework (M36's sub_080c1fdc dispatches gUnk_02007D2C through
 * gUnk_087572CC; entry 0 is sub_080c21b0).
 * 
 *   sub_080c1ffc / sub_080c2038 / sub_080c2078   spawn task type #96:
 *       variant 0 once per player (Task.unk1C = the player), variant 1 (its
 *       task index kept in M37Game.unk44C) and variant 2 (Task.unk18/unk1C/
 *       unk20 from the caller).
 *   sub_080c20b4   the state set-up: gUnk_02017094 = &gUnk_02016C40,
 *       gUnk_0201716C = &gUnk_0201B0E0, the linked-player count and mode
 *       cells, the four players' course records, the per-frame hook
 *       sub_080c2d38 (gUnk_0300003C, called by the frame driver sub_08000de4)
 *       and the VBlank hook sub_080c2fb8 (gUnk_03000FA4, called by the VBlank
 *       handler), and two 16-colour rows of gUnk_08609E40.
 *   sub_080c21b0   the screen's task body: waits for the scroll position
 *       gUnk_0201716C->unk000 to reach the two course lines unk00C and unk010
 *       (a sign sprite and songs 0x82B/0x82A/0x82C at each), then for all
 *       four players to pass unk010, and ends the screen (Task.unk18 = 2).
 *   sub_080c241c   its per-frame callback: the frame counter gUnk_02017170,
 *       the script cursor, the palette fades and M35's sub_080b9e30. */

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

extern struct M37Game gUnk_02016C40;
extern struct M37Game *gUnk_02017094;
extern struct M37Course gUnk_0201B0E0;
extern struct M37Course *gUnk_0201716C;
extern u16 gUnk_02017170;
extern u16 gUnk_03002360;
extern u16 gUnk_0300243C;
extern u32 gUnk_0300003C;
extern u32 gUnk_03000FA4;
extern u16 gUnk_08609E40[];
extern u32 gUnk_08755FEC[];
extern u16 gUnk_0860A042[];
extern u32 gUnk_03000AF4;

void TaskYieldTrampoline(s32 frames);
void sub_08001488(void);
s32 sub_08003110(s32 songId);
void sub_080034d0(void);
void sub_08003688(void);
s32 sub_080058e4(u32 type, s32 idx);                         /* spawn a task */
void sub_08005ca0(void);
void sub_08006138(void);                                     /* end the running task */
void sub_080b9e30(void);
void sub_080c2d38(void);
void sub_080c2fb8(void);
void sub_080c4860(s32 y);
void sub_080c4890(void);                                  /* step the four palette fades gUnk_020170A0[] */
void sub_080c495c(void);
s32 sub_080c4974(u16 *src, s32 pal, s32 period, s32 steps, s32 count, s32 repeat);
void sub_080c4a20(s32 i);
void sub_080c4c78(void);
void sub_080c51c0(void);
void sub_080c51d4(void);                                  /* step the gUnk_03006928 script */
void sub_080c241c(void);

void sub_080c1ffc(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i < 4; i++) {
        id = sub_080058e4(96, 0);
        if (id != -1) {
            t = &gUnk_03002790[id];
            t->unk18 = id;
            t->unk1C = i;
            t->unk73 = 0;
        }
    }
}

void sub_080c2038(s32 unused)
{
    s32 id;
    struct Task *t;

    id = sub_080058e4(96, 32);
    if (id != -1) {
        t = &gUnk_03002790[id];
        t->unk73 = 1;
        gUnk_02017094->unk44C = id;
    }
}

void sub_080c2078(s32 a, s32 b, s32 c)
{
    s32 id;
    struct Task *t;

    id = sub_080058e4(96, 32);
    if (id != -1) {
        t = &gUnk_03002790[id];
        t->unk18 = b;
        t->unk1C = a;
        t->unk73 = 2;
        t->unk20 = c;
    }
}

void sub_080c20b4(void)
{
    s32 i;

    gUnk_02017094 = &gUnk_02016C40;
    gUnk_0201716C = &gUnk_0201B0E0;
    gUnk_02017094->unk446 = gUnk_03002360;
    gUnk_02017094->unk448 = gUnk_0300243C;
    gUnk_02017170 = 0;
    sub_080034d0();
    sub_08003688();
    sub_080c4c78();
    sub_080c51c0();
    for (i = 0; i < 4; i++) {
        gUnk_0201716C->unk018[i].unk00 = gUnk_0201716C->unk000;
        gUnk_0201716C->unk018[i].unk04 = 0;
    }
    gUnk_02017094->unk014 = -1;
    gUnk_02017094->unk300 = 0;
    gUnk_0300003C = (u32)sub_080c2d38;
    gUnk_03000FA4 = (u32)sub_080c2fb8;
    sub_080c495c();
    for (i = 0; i < 16; i++) {
        gUnk_02017094->unk0EC.unk76[i] = gUnk_08609E40[32 + i];
        gUnk_02017094->unk0EC.unk96[i] = gUnk_08609E40[64 + i];
    }
}

void sub_080c21b0(void)
{
    s32 n;
    s32 i;

    sub_080c20b4();
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk38 = gUnk_08755FEC;
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08003110(0x82B);
    sub_080c1ffc();
    sub_080c2038(0);
    gUnk_03002490->unk04 = (u32)sub_080c241c;
    while (gUnk_0201716C->unk000 < gUnk_0201716C->unk00C - 240)
        TaskYieldTrampoline(1);
    sub_080c4860(gUnk_0201716C->unk00C);
    while (1) {
        if (gUnk_0201716C->unk000 >= gUnk_0201716C->unk00C)
            break;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk48 = 144;
    gUnk_03002490->unk4A = 80;
    gUnk_03002490->unk34 = sub_080c4974(gUnk_0860A042, 241, 10, 8, 15, 0);
    gUnk_03002490->unk3C = 0;
    sub_08003110(0x82A);
    while (gUnk_0201716C->unk000 < gUnk_0201716C->unk00C + 240)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk08 = 0;
    gUnk_03002490->unk3C = 0xFFFF;
    sub_080c4a20(gUnk_03002490->unk34);
    if (gUnk_0300003C != 0 && gUnk_02016C40.unk000 != 2) {
        while (gUnk_02017094->unk300 <= 0x4AF)
            TaskYieldTrampoline(1);
        sub_080c4974(&gUnk_02017094->unk0EC.unk76[1], 161, 256, 2, 6, 1);
    }
    while (gUnk_0201716C->unk000 < gUnk_0201716C->unk010 - 240)
        TaskYieldTrampoline(1);
    sub_080c4860(gUnk_0201716C->unk010);
    while (1) {
        if (gUnk_0201716C->unk000 >= gUnk_0201716C->unk010)
            break;
        TaskYieldTrampoline(1);
    }
    while (gUnk_0201716C->unk018[0].unk00 < gUnk_0201716C->unk010)
        TaskYieldTrampoline(1);
    gUnk_03002490->unk48 = 112;
    gUnk_03002490->unk4A = 80;
    gUnk_03002490->unk34 = sub_080c4974(gUnk_0860A042, 241, 10, 8, 15, 0);
    gUnk_03002490->unk3C = 1;
    sub_08003110(0x82C);
    while (1) {
        n = 0;
        for (i = 0; i < 4; i++)
            if (gUnk_0201716C->unk018[i].unk00 > gUnk_0201716C->unk010)
                n++;
        if (n > 3)
            break;
        TaskYieldTrampoline(1);
    }
    TaskYieldTrampoline(180);
    sub_080034d0();
    sub_08003688();
    sub_08001488();
    gUnk_03000AF4 = 0;
    gUnk_03002490->unk18 = 2;
    sub_08006138();
}

void sub_080c241c(void)
{
    gUnk_02017170++;
    sub_080c51d4();
    sub_080c4890();
    sub_080b9e30();
}
