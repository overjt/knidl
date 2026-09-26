#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* ending_c6c64.c (0x080C6C64-0x080C7E4B, issue #100).
 *
 * The first ending scene, part 1: task type #100 (class 3), which M37's
 * sub_080c62f0 spawns in AgbMain state 11 and waits on (gUnk_02008018).
 *   sub_080c6c64   the body: variant 0 (Task.unk73 == 0) loads the graphics
 *       (sub_080c6ca0) and spawns variants 1, 6, 7, 9 and 10 from the list
 *       gUnk_0875735C (sub_080c6d38); variants 1-10 run gUnk_08757330[unk73].
 *   sub_080c6d84 / sub_080c769c   variant 1, the scene's main sprite, and its
 *       scaling draw callback; sub_080c77cc spawns its four variant-2 helpers.
 *   sub_080c7810   variant 2: while the spawner is drawn (Task.unk34), one of
 *       four effects by Task.unk74 (a shaking sprite, drifting puffs, spark
 *       bursts, four variant-3 sprites).
 *   sub_080c7cc0   variant 3: a sprite that drifts right from its spawner and
 *       plays one of three animations five times. */

extern u16 gUnk_02000028;
extern u16 gUnk_02004C94;
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u16 gUnk_03001570[];         /* palette buffer */
extern u32 gUnk_06010000[];         /* OBJ VRAM */
extern u16 gUnk_080DC628[][16];     /* per-player palettes */
extern void (*gUnk_08757330[])(void);
extern struct GfxHeader gUnk_085995AC;
extern struct GfxHeader gUnk_0859990C;
extern u16 gUnk_0875735C[];
extern u32 gUnk_08755708[];
extern u32 gUnk_080DBEF8[];
extern u32 gUnk_080DBF00[];
extern u32 gUnk_080DBF08[];
extern u32 gUnk_080DBF18[];
extern u32 gUnk_080DBF28[];
extern u32 gUnk_080DBF30[];
extern u8 gUnk_02008018;
extern s16 gUnk_0873FF98[];
extern u32 gUnk_0874CF28[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 sub_08002ee8(u32 range);                                 /* random 0 .. range-1 */
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
s32 sub_080058e4(u32 type, s32 idx);                         /* spawn a task */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005ca0(void);
void sub_08006138(void);                                     /* end the running task */
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
void sub_08008c64(u16 a0);                                   /* load screen graphics */
void sub_080c6ca0(void);
void sub_080c6d38(void);
void sub_080c769c(void);
void sub_080c77cc(void);

/* Task type #100 (class 3): variant 0 loads the graphics and spawns the
   other variants; variants 1-10 run the anchor table gUnk_08757330[]. */
void sub_080c6c64(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    if (gUnk_03002490->unk73 == 0) {
        sub_080c6ca0();
        sub_080c6d38();
    } else {
        sub_08002e98(gUnk_03002490->unk73, 11, gUnk_08757330);
    }
    TaskDispatchTrampoline();
}

/* Task type #100's graphics: two sprite sheets, the first one's palette and,
   with more than one player, this player's palette in slot 1. */
void sub_080c6ca0(void)
{
    struct GfxHeader *h = &gUnk_085995AC;

    sub_08008c64(0);
    LZ77UnCompWram(h->unk0C, gUnk_02020000);
    sub_080017e4(4, (u32)gUnk_02020000, (u32)gUnk_06010000, h->unk02 << 5);
    sub_080017e4(2, (u32)h->unk08, (u32)gUnk_03001570, h->unk00 << 5);
    if (gUnk_02004C94 > 1)
        sub_080017e4(2, (u32)gUnk_080DC628[gUnk_02000028], (u32)&gUnk_03001570[16], 22);
    h = &gUnk_0859990C;
    LZ77UnCompWram(h->unk0C, gUnk_02020000);
    sub_080017e4(3, (u32)gUnk_02020000, (u32)gUnk_06010000, h->unk02 << 5);
    sub_08008c64(71);
}

/* Spawn one task type #100 per variant listed in gUnk_0875735C[] (1, 6, 7,
   9 and 10, ended by 11, the anchor table's size). */
void sub_080c6d38(void)
{
    s32 i;
    s32 id;
    s32 v;
    struct Task *t;

    for (i = 0; v = gUnk_0875735C[i], (s16)gUnk_0875735C[i] <= 10; i++) {
        id = sub_080058e4(100, 32);
        if (id == -1)
            for (;;)
                ;
        t = &gUnk_03002790[id];
        t->unk73 = v;
    }
}

/* Task type #100 variant 1, the scene's main sprite: spawns variant 4 and
   the four variant-2 helpers (sub_080c77cc), draws itself through the
   scaling callback sub_080c769c, runs its timed motion phases and ends the
   scene by clearing gUnk_02008018, which M37's sub_080c62f0 waits for. */
void sub_080c6d84(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_080c769c;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_08755708;
    gUnk_03002490->unk40 = 0x8810;
    gUnk_03002490->unk46 = sub_080058e4(100, 32);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 4;
    gUnk_03002790[gUnk_03002490->unk46].unk44 = gCurTaskIdx;
    sub_080c77cc();
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk4C = 0x600000;
    gUnk_03002490->unk50 = 0x320000;
    sub_080062c4();
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(240);
    TaskYieldTrampoline(12);
    TaskYieldTrampoline(190);
    sub_08001a94(12, (u32)gUnk_080DBEF8, 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    TaskYieldTrampoline(1);
    sub_08001a94(12, (u32)gUnk_080DBF00, 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    TaskYieldTrampoline(1);
    sub_08001a94(12, (u32)gUnk_080DBF08, 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    TaskYieldTrampoline(1);
    sub_08001a94(12, (u32)gUnk_080DBF18, 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    TaskYieldTrampoline(1);
    sub_08001a94(12, (u32)gUnk_080DBF28, 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    TaskYieldTrampoline(1);
    sub_08001a94(12, (u32)gUnk_080DBF30, 0, 0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C = 62;
    sub_080031b8(281);
    gUnk_03002490->unk18 = 0x30000;
    gUnk_03002490->unk28 = 0x1000;
    sub_080062c4();
    TaskYieldTrampoline(4);
    gUnk_03002490->unk54 = -0x2000;
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(80);
    gUnk_03002490->unk54 = -0x1000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk54 = -0x800;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk54 = 0x800;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk54 = 0x1000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk54 = 0x2000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk54 = 0x4000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk54 = 0x8000;
    TaskYieldTrampoline(24);
    gUnk_03002490->unk3C = -1;
    sub_080062c4();
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(28);
    gUnk_03002490->unk54 = -0x6000;
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(32);
    sub_08003110(24);
    gUnk_03002490->unk3C = 62;
    gUnk_03002490->unk54 = 0x8000;
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(48);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    gUnk_03002490->unk58 = 0xC000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C = -1;
    gUnk_03002490->unk54 = -0x10000;
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(14);
    gUnk_03002490->unk54 = 0x6000;
    gUnk_03002490->unk58 = -0x6000;
    TaskYieldTrampoline(32);
    gUnk_03002490->unk34 = 1;
    gUnk_03002490->unk3C = 62;
    gUnk_03002490->unk18 = 0x3F0000;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk54 = -0x20000;
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = -0x10000;
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = -0x8000;
    gUnk_03002490->unk58 = -0xC000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = -0x4000;
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0x4000;
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk6C = 0;
    do {
        sub_080062c4();
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        sub_080062c4();
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        sub_080062c4();
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk6C = 0;
    do {
        sub_080062c4();
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        sub_080062c4();
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        sub_080062c4();
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x800;
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(32);
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x800;
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(32);
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6C = 0;
    do {
        sub_080062c4();
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        sub_080062c4();
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        sub_080062c4();
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_080062c4();
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x800;
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(28);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = 0x4000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x800;
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(28);
    gUnk_03002490->unk54 = 0x400;
    gUnk_03002490->unk58 = -0x4000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk34 = 1;
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(32);
        gUnk_03002490->unk58 = 0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(24);
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x400;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 10);
    gUnk_03002490->unk54 = -0x8000;
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x10000;
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x20000;
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(12);
    gUnk_03002490->unk54 = -0x10000;
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = -0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x8000;
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x20000;
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(40);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x8000;
    gUnk_03002490->unk58 = -0x2000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x2000;
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(60);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(14);
    TaskYieldTrampoline(68);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    sub_080062c4();
    TaskYieldTrampoline(46);
    gUnk_02008018 = 0;
    sub_08006138();
}

/* Task type #100 variant 1's draw callback: grow the scale Task.unk18 by
   Task.unk28 (clamped to 0 .. 127 in 16.16), alternate two animation frames
   every three calls and draw the sprite, scaled through gUnk_0873FF98[] when
   the scale is not 1.0 (0x100). */
void sub_080c769c(void)
{
    s32 n;
    struct Task *t;
    u32 *tbl;
    s16 scale;

    gUnk_03002490->unk18 += gUnk_03002490->unk28;
    if (gUnk_03002490->unk18 < 0)
        gUnk_03002490->unk18 = 0;
    if ((gUnk_03002490->unk18 >> 16) > 127)
        gUnk_03002490->unk18 = 0x7F0000;
    if (gUnk_03002490->unk38 != NULL && gUnk_03002490->unk3C != -1) {
        n = 0;
        if ((s16)gUnk_03002490->unk70 > 1)
            n = 1;
        if ((s16)++gUnk_03002490->unk70 > 5)
            gUnk_03002490->unk70 = 0;
        t = gUnk_03002490;
        if ((u16)(t->unk48 + 63) <= 366 && t->unk4A > -64 && t->unk4A < 224) {
            tbl = t->unk38;
            scale = gUnk_0873FF98[t->unk18 >> 16];
            if ((u16)scale != 0x100)
                sub_08001a94(gUnk_03002490->unk42, sub_08001cc8(tbl[t->unk3C + n], scale, scale, 0),
                             gUnk_03002490->unk3E, gUnk_03002490->unk40, gUnk_03002490->unk48,
                             gUnk_03002490->unk4A);
            else
                sub_08001a94(t->unk42, tbl[t->unk3C + n], t->unk3E, t->unk40, t->unk48, t->unk4A);
        }
    }
}

/* Spawn the four task type #100 variant-2 helpers of variant 1 (Task.unk74
   = 0..3, Task.unk44 = the calling task). */
void sub_080c77cc(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i <= 3; i++) {
        id = sub_080058e4(100, 32);
        t = &gUnk_03002790[id];
        t->unk73 = 2;
        t->unk74 = i;
        t->unk44 = gCurTaskIdx;
    }
}

/* Task type #100 variant 2 (four of them, spawned by variant 1's
   sub_080c77cc with Task.unk74 = 0..3 and Task.unk44 = variant 1's task):
   while the spawner has graphics (Task.unk34), each one plays its own
   effect relative to it - 0 a shaking sprite, 1 two drifting puffs, 2
   three bursts of sparks, 3 spawns four variant 3 sprites. */
void sub_080c7810(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk38 = gUnk_0874CF28;
    gUnk_03002490->unk40 = 0;
    for (;;) {
        if (gUnk_03002790[gUnk_03002490->unk44].unk34 == 0) {
            TaskYieldTrampoline(1);
            continue;
        }
        switch (gUnk_03002490->unk74) {
        case 0:
            sub_080062c4();
            gUnk_03002490->unk42 = 12;
            gUnk_03002490->unk4C = 8 << 16;
            gUnk_03002490->unk50 = 0;
            gUnk_03002490->unk54 = 0x10000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = -0x10000;
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = 0x10000;
            gUnk_03002490->unk3C = 1;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = -0x10000;
            gUnk_03002490->unk3C = 1;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = 0x10000;
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = -0x10000;
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = 0x10000;
            gUnk_03002490->unk3C = 3;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk54 = -0x10000;
            gUnk_03002490->unk3C = 3;
            TaskYieldTrampoline(1);
            break;
        case 1:
            sub_080062c4();
            gUnk_03002490->unk42 = 13;
            gUnk_03002490->unk4C = (sub_08002ee8(12) + 8) << 16;
            gUnk_03002490->unk50 = (sub_08002ee8(8) - 4) << 16;
            sub_0800625c(0x10000, 0x4000, 0x5A5A5A5A, 0x8000, -0x800, 0x5A5A5A5A);
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 5;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 7;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 8;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk4C = (sub_08002ee8(12) + 10) << 16;
            gUnk_03002490->unk50 = (sub_08002ee8(8) - 4) << 16;
            sub_0800625c(0x10000, 0x4000, 0x5A5A5A5A, -0x8000, 0x800, 0x5A5A5A5A);
            gUnk_03002490->unk3C = 4;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 5;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 6;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 7;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C = 8;
            TaskYieldTrampoline(2);
            break;
        case 2:
            sub_080062c4();
            gUnk_03002490->unk42 = 10;
            gUnk_03002490->unk4C = (sub_08002ee8(32) + 8) << 16;
            gUnk_03002490->unk50 = (sub_08002ee8(32) - 16) << 16;
            gUnk_03002490->unk3C = 9;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 10;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 11;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 12;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 13;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 14;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk4C = (sub_08002ee8(32) + 8) << 16;
            gUnk_03002490->unk50 = (sub_08002ee8(32) - 16) << 16;
            gUnk_03002490->unk3C = 9;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 10;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 11;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 12;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 13;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 14;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 0xFFFF;
            TaskYieldTrampoline(15);
            gUnk_03002490->unk4C = (sub_08002ee8(32) + 8) << 16;
            gUnk_03002490->unk50 = (sub_08002ee8(32) - 16) << 16;
            gUnk_03002490->unk3C = 9;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 10;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 11;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 12;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 13;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = 14;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C = -1;
            TaskYieldTrampoline(30);
            break;
        case 3:
            gUnk_03002490->unk46 = sub_080058e4(100, 32);
            gUnk_03002790[gUnk_03002490->unk46].unk73 = 3;
            gUnk_03002790[gUnk_03002490->unk46].unk74 = 2;
            gUnk_03002790[gUnk_03002490->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(16);
            gUnk_03002490->unk46 = sub_080058e4(100, 32);
            gUnk_03002790[gUnk_03002490->unk46].unk73 = 3;
            gUnk_03002790[gUnk_03002490->unk46].unk74 = 1;
            gUnk_03002790[gUnk_03002490->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(16);
            gUnk_03002490->unk46 = sub_080058e4(100, 32);
            gUnk_03002790[gUnk_03002490->unk46].unk73 = 3;
            gUnk_03002790[gUnk_03002490->unk46].unk74 = 0;
            gUnk_03002790[gUnk_03002490->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(8);
            gUnk_03002490->unk46 = sub_080058e4(100, 32);
            gUnk_03002790[gUnk_03002490->unk46].unk73 = 3;
            gUnk_03002790[gUnk_03002490->unk46].unk74 = 0;
            gUnk_03002790[gUnk_03002490->unk46].unk44 = gCurTaskIdx;
            TaskYieldTrampoline(8);
            break;
        }
    }
}

/* Task type #100 variant 3: a sprite that starts at the spawning task's
   (Task.unk44) position, 0-15 pixels lower, moves right and plays one of
   three four-frame animations (Task.unk74) five times. */
void sub_080c7cc0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_0874CF28;
    gUnk_03002490->unk40 = 0;
    gUnk_03002490->unk4C = gUnk_03002790[gUnk_03002490->unk44].unk48 << 16;
    gUnk_03002490->unk50 = (gUnk_03002790[gUnk_03002490->unk44].unk4A + sub_08002ee8(16) - 8) << 16;
    gUnk_03002490->unk54 = 0x10000;
    gUnk_03002490->unk5C = 0x2000;
    switch (gUnk_03002490->unk74) {
    case 0:
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C = 15;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 16;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 17;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 18;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 1:
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C = 19;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 20;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 21;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 22;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    case 2:
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C = 23;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 24;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 25;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C = 26;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        break;
    }
    TaskDispatchTrampoline();
}
