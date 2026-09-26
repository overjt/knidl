#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* ending_c9004.c (0x080C9004-0x080CAA3B, issue #100).
 *
 * The second ending scene: task type #101 (class 3), which M37's
 * sub_080c6388 spawns in AgbMain state 11 and waits on (gUnk_02008018).
 *   sub_080c9004   the body: variant 0 loads the graphics (sub_080c9040) and
 *       spawns variants 1, 3, 6, 7, 8 and 11 from the list gUnk_08757424
 *       (sub_080c90c8); variants 1-11 run gUnk_087573F4[Task.unk73].
 *   sub_080c9114 / sub_080c9418   variant 1, a sprite that falls in, then
 *       rides the BG3 layer and flashes its palette.
 *   sub_080c9d10   variant 2, a 22-frame animation played six times.
 *   sub_080c94cc / sub_080c972c / sub_080c97a0   variant 3, which spawns its
 *       variant-5 companion, crosses the screen twice leaving variant-4
 *       tasks behind, fades the music out and ends the scene (clearing
 *       gUnk_02008018).
 *   sub_080c9884, sub_080c98d8 / sub_080c9974   variants 4 and 5.
 *   sub_080ca71c, sub_080ca830, sub_080ca8f0   variants 6-8, scripted
 *       sprites drawn by M05's sub_0801a3e4.
 *   sub_080c9e8c   variant 9, eleven sprites bursting out of one point.
 *   sub_080ca344 / sub_080ca570 / sub_080ca640   variant 10, sixteen falling
 *       sprites with two sway scripts.
 *   sub_080c9a28 / sub_080c9cf0   variant 11, the finale: palette flashes,
 *       variants 2, 9 and 10, a fade to an OBJ-only display. */

extern vu16 gUnk_03001ED8;          /* DISPCNT shadow */
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03000B78;          /* ... BG3 */
extern u16 gUnk_02000028;
extern u16 gUnk_02004C94;
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u16 gUnk_03001570[];         /* palette buffer */
extern u32 gUnk_06010000[];         /* OBJ VRAM */
extern u16 gUnk_080DC628[][16];     /* per-player palettes */
extern void (*gUnk_087573F4[])(void);
extern struct GfxHeader gUnk_0859A09C;
extern u16 gUnk_085E0070[];
extern u32 gUnk_085E0090[];
extern u16 gUnk_08757424[];
extern u32 gUnk_0875585C[];
extern u16 gUnk_0859A0B0[];
extern u16 gUnk_0859A0D0[];
extern u16 gUnk_03001470[];
extern u8 gUnk_02008018;
extern u16 gUnk_08757432[];
extern u32 gUnk_0874C500[];
extern u16 gUnk_085E2920[];
extern u16 gUnk_085E2A20[];
extern u16 gUnk_085E2B20[];
extern u16 gUnk_0875743E[];
extern u16 gUnk_03001370[];
extern vu16 gUnk_03001270[];
extern vu16 gUnk_03001E90;
extern vs16 gUnk_03000FB8;
extern vu16 gUnk_030004A4;
extern vu16 gUnk_03001174;
extern vu16 gUnk_03001E98;
extern vu16 gUnk_03000048;
extern u16 *gUnk_03001ED4;
extern u32 gUnk_0874CF94[];
extern u32 gUnk_0874C44C[];
extern u32 gUnk_0874CF28[];
extern u32 gUnk_08755440[];
extern u32 gUnk_0875546C[];
extern u32 gUnk_08755484[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 sub_08002ee8(u32 range);                                 /* random 0 .. range-1 */
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
void sub_08003770(u16 volume);
s32 sub_080058e4(u32 type, s32 idx);                         /* spawn a task */
void sub_080059d8(void);
u32 sub_08005acc(void);
u32 sub_08005c4c(void);
void sub_08005ca0(void);
void sub_08005d9c(void);
void sub_08006138(void);                                     /* end the running task */
void sub_080062c4(void);
void sub_0801a3e4(void);
void sub_08026278(s32 x, s32 y);
void sub_080c9040(void);
void sub_080c90c8(void);
void sub_080c9418(void);
void sub_080c972c(void);
void sub_080c97a0(void);
void sub_080c9974(void);
void sub_080c9cf0(void);
void sub_080ca570(void);
void sub_080ca640(void);

/* Task type #101 (class 3): variant 0 loads the graphics and spawns the
   other variants; variants 1-11 run the anchor table gUnk_087573F4[]. */
void sub_080c9004(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    if (gUnk_03002490->unk73 == 0) {
        sub_080c9040();
        sub_080c90c8();
    } else {
        sub_08002e98(gUnk_03002490->unk73, 12, gUnk_087573F4);
    }
    TaskDispatchTrampoline();
}

/* Task type #101's graphics. */
void sub_080c9040(void)
{
    struct GfxHeader *h = &gUnk_0859A09C;

    LZ77UnCompWram(h->unk0C, gUnk_02020000);
    sub_080017e4(4, (u32)gUnk_02020000, (u32)gUnk_06010000, h->unk02 << 5);
    sub_080017e4(2, (u32)h->unk08, (u32)gUnk_03001570, h->unk00 << 5);
    if (gUnk_02004C94 > 1)
        sub_080017e4(2, (u32)gUnk_080DC628[gUnk_02000028], (u32)gUnk_03001570, 22);
    sub_080017e4(2, (u32)gUnk_085E0070, (u32)&gUnk_03001570[80], 32);
    LZ77UnCompWram(gUnk_085E0090, gUnk_02020000);
}

/* Spawn one task type #101 per variant listed in gUnk_08757424[] (1, 3, 6,
   7, 8 and 11, ended by 12, the anchor table's size). */
void sub_080c90c8(void)
{
    s32 i;
    s32 id;
    s32 v;
    struct Task *t;

    for (i = 0; v = gUnk_08757424[i], (s16)gUnk_08757424[i] <= 11; i++) {
        id = sub_080058e4(101, 32);
        if (id == -1)
            for (;;)
                ;
        t = &gUnk_03002790[id];
        t->unk73 = v;
    }
}

/* Task type #101 variant 1: a sprite at the camera's (120, -24) that falls
   in, plays a 16-frame animation twice, then pins itself to the BG3 layer
   (sub_080c9418) and flashes its palette ever faster. */
void sub_080c9114(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875585C;
    gUnk_03002490->unk40 = 0x8810;
    gUnk_03002490->unk4C = (gUnk_03002348 + 120) << 16;
    gUnk_03002490->unk50 = (gUnk_030023E4 - 24) << 16;
    TaskYieldTrampoline(32);
    sub_080062c4();
    gUnk_03002490->unk58 = 0x10000;
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 18;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 19;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 22;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk3C = 23;
    TaskYieldTrampoline(3);
    sub_080062c4();
    sub_080031b8(238);
    gUnk_03002490->unk0C = (u32)sub_080c9418;
    gUnk_03002490->unk4C = (gUnk_03002490->unk48 - gUnk_03002348 + (gUnk_03000B78 >> 16)) << 16;
    gUnk_03002490->unk50 = (gUnk_03002490->unk4A - gUnk_030023E4 + (gUnk_03000FA8 >> 16)) << 16;
    gUnk_03002490->unk6C = 0;
    do {
        sub_08003014(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(5);
        sub_08003014(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(5);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 11);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08003014(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(4);
        sub_08003014(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 11);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08003014(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(3);
        sub_08003014(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gUnk_03001470 + ((gUnk_03002490->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 10);
    TaskYieldTrampoline(96);
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08006138();
}

void sub_080c9418(void)
{
    struct Task *t;
    u32 *g;

    if (gUnk_03002490->unk38 == NULL)
        return;
    if (gUnk_03002490->unk3C == -1)
        return;
    if (sub_08005c4c() == 0)
        return;
    t = gUnk_03002490;
    if (t->unk48 - (gUnk_03000B78 >> 16) > -64 && t->unk48 - (gUnk_03000B78 >> 16) <= 303
        && t->unk4A - (gUnk_03000FA8 >> 16) > -64 && t->unk4A - (gUnk_03000FA8 >> 16) <= 223) {
        g = t->unk38;
        sub_08001a94(t->unk42, g[t->unk3C], t->unk3E, t->unk40,
                     t->unk48 - (gUnk_03000B78 >> 16), t->unk4A - (gUnk_03000FA8 >> 16));
    }
}

/* Task type #101 variant 3: spawns its variant-5 companion (which follows
   it), flies across the screen twice leaving a trail of variant-4 tasks
   (sub_080c972c while Task.unk28 is set), then fades the music out
   (sub_080c97a0) and clears gUnk_02008018. */
void sub_080c94cc(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_080c97a0;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_0875585C;
    gUnk_03002490->unk40 = 0x8810;
    gUnk_03002490->unk04 = (u32)sub_080c972c;
    gUnk_03002490->unk34 = 255;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk46 = sub_080058e4(101, 32);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 5;
    gUnk_03002790[gUnk_03002490->unk46].unk44 = gCurTaskIdx;
    sub_080062c4();
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(99);
    TaskYieldTrampoline(96);
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk4C = (gUnk_03002348 + 255) << 16;
    gUnk_03002490->unk50 = (gUnk_030023E4 + 80) << 16;
    sub_080031b8(290);
    gUnk_03002490->unk3C = 5;
    gUnk_03002490->unk54 = -0x80000;
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x2000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x8000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0xC000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x14000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(13);
    sub_080062c4();
    gUnk_03002490->unk28 = 0;
    TaskYieldTrampoline(70);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(97);
    TaskYieldTrampoline(210);
    sub_080031b8(291);
    gUnk_03002490->unk4C = (gUnk_03002348 - 16) << 16;
    gUnk_03002490->unk50 = (gUnk_030023E4 + 176) << 16;
    gUnk_03002490->unk54 = 0x80000;
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x40000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x14000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk54 = 0x10000;
    TaskYieldTrampoline(24);
    TaskYieldTrampoline(20);
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = (gUnk_03002490->unk4A << 16) - 1;
    gUnk_03002490->unk54 = 0xE000;
    gUnk_03002490->unk58 = -0xE000;
    TaskYieldTrampoline(20);
    gUnk_03002490->unk54 = 0xC000;
    gUnk_03002490->unk58 = -0xC000;
    TaskYieldTrampoline(80);
    TaskYieldTrampoline(82);
    gUnk_03002490->unk24 = 2;
    TaskYieldTrampoline(128);
    gUnk_02008018 = 0;
    sub_08006138();
}

/* Task type #101 variant 3's per-frame callback: while Task.unk28 is set,
   spawn a variant-4 task at this task's position every third frame. */
void sub_080c972c(void)
{
    s32 id;
    struct Task *t;

    if (gUnk_03002490->unk28 != 0) {
        if (--gUnk_03002490->unk18 <= 0) {
            id = sub_080058e4(101, 32);
            t = &gUnk_03002790[id];
            t->unk73 = 4;
            t->unk48 = gUnk_03002490->unk48;
            t->unk4A = gUnk_03002490->unk4A;
            t->unk4C = t->unk48 << 16;
            t->unk50 = t->unk4A << 16;
        }
        if (gUnk_03002490->unk18 <= 0)
            gUnk_03002490->unk18 = 3;
    }
}

/* Task type #101 variant 3's draw callback: cycle the frame offset through
   gUnk_08757432[6], draw, and fade the music out by Task.unk24 per frame
   (volume in Task.unk34). */
void sub_080c97a0(void)
{
    s16 off;

    if ((s16)gUnk_03002490->unk70 < 0)
        gUnk_03002490->unk70 = 0;
    if ((s16)gUnk_03002490->unk70 > 5)
        gUnk_03002490->unk70 = 0;
    off = gUnk_08757432[(s16)gUnk_03002490->unk70];
    gUnk_03002490->unk70++;
    if (gUnk_03002490->unk38 != NULL && gUnk_03002490->unk3C != -1 && sub_08005acc() != 0) {
        struct Task *u = gUnk_03002490;
        u32 *g = u->unk38;

        sub_08001a94(u->unk42, g[u->unk3C + off], u->unk3E, u->unk40,
                     u->unk48 - gUnk_03002348, u->unk4A - gUnk_030023E4);
    }
    if (gUnk_03002490->unk24 != 0) {
        gUnk_03002490->unk34 -= gUnk_03002490->unk24;
        if (gUnk_03002490->unk34 < 0)
            gUnk_03002490->unk34 = 0;
        sub_08003770(gUnk_03002490->unk34);
    }
}

/* Task type #101 variant 4. */
void sub_080c9884(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_0874C500;
    gUnk_03002490->unk40 = 0;
    gUnk_03002490->unk3C = sub_08002ee8(4) + 4;
    gUnk_03002490->unk58 = 0x40000;
    TaskYieldTrampoline(24);
    TaskDispatchTrampoline();
}

/* Task type #101 variant 5: a sprite drawn at another task's position
   (sub_080c9974), flipped after 863 frames, then looping a 4-frame
   animation. */
void sub_080c98d8(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080c9974;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_0875585C;
    gUnk_03002490->unk40 = 0x8810;
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(207);
    gUnk_03002490->unk3E |= 0x8000;
    TaskYieldTrampoline(56);
    for (;;) {
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(2);
    }
}

void sub_080c9974(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    u32 *g;

    t->unk48 = gUnk_03002790[t->unk44].unk48;
    t->unk4A = gUnk_03002790[t->unk44].unk4A;
    if (t->unk38 != NULL && t->unk3C != -1 && sub_08005acc() != 0) {
        u = gUnk_03002490;
        g = u->unk38;
        sub_08001a94(u->unk42, g[u->unk3C], u->unk3E, u->unk40,
                     u->unk48 - gUnk_03002348, u->unk4A - gUnk_030023E4);
    }
}

/* Task type #101 variant 11: flash the OBJ palettes, spawn variant 2 and
   eleven variant-9 tasks, fade the screen out, switch the display to OBJ
   only and spawn sixteen variant-10 tasks. */
void sub_080c9a28(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk04 = (u32)sub_080c9cf0;
    gUnk_03002490->unk4C = (gUnk_03002348 + 120) << 16;
    gUnk_03002490->unk50 = (gUnk_030023E4 + 80) << 16;
    sub_080062c4();
    TaskYieldTrampoline(99);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08003014(gUnk_085E2920, gUnk_085E2B20, (u16)((s16)gUnk_03002490->unk6C * 32), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08003014(gUnk_085E2B20, gUnk_085E2920, (u16)((s16)gUnk_03002490->unk6C * 32), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08003014(gUnk_085E2920, gUnk_085E2A20, (u16)((s16)gUnk_03002490->unk6C * 16), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 15);
    TaskYieldTrampoline(68);
    TaskYieldTrampoline(80);
    gUnk_03002490->unk46 = sub_080058e4(101, 32);
    gUnk_03002790[gUnk_03002490->unk46].unk73 = 2;
    TaskYieldTrampoline(60);
    TaskYieldTrampoline(60);
    TaskYieldTrampoline(30);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk46 = sub_080058e4(101, 32);
        gUnk_03002790[gUnk_03002490->unk46].unk73 = 9;
        gUnk_03002790[gUnk_03002490->unk46].unk74 = gUnk_03002490->unk6C;
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 10);
    sub_080031b8(288);
    gUnk_03002490->unk58 = -0x30000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = -0x20000;
    gUnk_03001E90 = 32;
    gUnk_03000FB8 = 0;
    gUnk_030004A4 = 1;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 0;
    gUnk_03001ED4 = 0;
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    sub_080017e4(2, (u32)gUnk_0875743E, (u32)gUnk_03001270, 2);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1000;
    gUnk_03000FB8 = 0;
    sub_080062c4();
    TaskYieldTrampoline(120);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk46 = sub_080058e4(101, 32);
        gUnk_03002790[gUnk_03002490->unk46].unk73 = 10;
        gUnk_03002790[gUnk_03002490->unk46].unk74 = gUnk_03002490->unk6C;
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 15);
    sub_08006138();
}

/* Task type #101 variant 11's per-frame hook: move the camera target to
   the task's position (M07's sub_08026278). */
void sub_080c9cf0(void)
{
    struct Task *t = gUnk_03002490;

    sub_08026278(t->unk48, t->unk4A);
}

/* Task type #101 variant 2: a sprite at the camera's (120, 40) that plays
   a 22-frame animation six times. */
void sub_080c9d10(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_0874CF94;
    gUnk_03002490->unk40 = 0;
    gUnk_03002490->unk4C = (gUnk_03002348 + 120) << 16;
    gUnk_03002490->unk50 = (gUnk_030023E4 + 40) << 16;
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 18;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 19;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 11;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 7;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 12;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 9;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 10;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    TaskDispatchTrampoline();
}

/* Task type #101 variant 9: one of eleven sprites that burst out of the
   camera's (120, 48) point; Task.unk74 picks its sprite, frame, direction
   and speed, each slowing down over four steps. */
void sub_080c9e8c(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 48 << 16;
    gUnk_03002490->unk40 = 0;
    switch (gUnk_03002490->unk74) {
    case 0:
        gUnk_03002490->unk38 = gUnk_0874C500;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk54 = 0x30000;
        gUnk_03002490->unk58 = -0x30000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = 0x14000;
        gUnk_03002490->unk58 = -0x14000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = 0xC000;
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = 0x4000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 1:
        gUnk_03002490->unk38 = gUnk_0874C500;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk54 = -0x30000;
        gUnk_03002490->unk58 = -0x30000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = -0x14000;
        gUnk_03002490->unk58 = -0x14000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = -0xC000;
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = -0x4000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 2:
        gUnk_03002490->unk38 = gUnk_0874C500;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 3:
        gUnk_03002490->unk38 = gUnk_0874CF28;
        gUnk_03002490->unk3C = 24;
        gUnk_03002490->unk54 = 0x14000;
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = 0xC000;
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = 0x2000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 4:
        gUnk_03002490->unk38 = gUnk_0874C44C;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk54 = -0x14000;
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = -0xC000;
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = -0x2000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 5:
        gUnk_03002490->unk38 = gUnk_0874C44C;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk54 = 0x40000;
        gUnk_03002490->unk58 = -0x14000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = 0x20000;
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = 0xC000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = 0x4000;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(12);
        break;
    case 6:
        gUnk_03002490->unk38 = gUnk_0874CF28;
        gUnk_03002490->unk3C = 26;
        gUnk_03002490->unk54 = -0x40000;
        gUnk_03002490->unk58 = -0x14000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = -0x20000;
        gUnk_03002490->unk58 = -0xC000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = -0xC000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = -0x4000;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(12);
        break;
    case 7:
        TaskYieldTrampoline(2);
        gUnk_03002490->unk38 = gUnk_0874CF28;
        gUnk_03002490->unk3C = 26;
        gUnk_03002490->unk54 = 0xA000;
        gUnk_03002490->unk58 = -0x1E000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = 0x4000;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = 0x2000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(10);
        break;
    case 8:
        TaskYieldTrampoline(2);
        gUnk_03002490->unk38 = gUnk_0874C500;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk54 = -0xA000;
        gUnk_03002490->unk58 = -0x1E000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = -0x4000;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = -0x2000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(10);
        break;
    case 9:
        TaskYieldTrampoline(4);
        gUnk_03002490->unk38 = gUnk_0874C500;
        gUnk_03002490->unk3C = 4;
        gUnk_03002490->unk54 = 0x1E000;
        gUnk_03002490->unk58 = -0xA000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = 0x10000;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = 0x8000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = 0x4000;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        break;
    case 10:
        TaskYieldTrampoline(4);
        gUnk_03002490->unk38 = gUnk_0874CF28;
        gUnk_03002490->unk3C = 24;
        gUnk_03002490->unk54 = -0x1E000;
        gUnk_03002490->unk58 = -0xA000;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk54 = -0x10000;
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk54 = -0x8000;
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk54 = -0x4000;
        gUnk_03002490->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        break;
    }
    TaskDispatchTrampoline();
}

/* Task type #101 variant 10: one of sixteen falling sprites; Task.unk74
   picks its start delay, its column and (0-7 / 8-15) which of the two
   sway scripts it runs, over and over. */
void sub_080ca344(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk40 = 0;
    for (;;) {
        gUnk_03002490->unk3C = 0xFFFF;
        sub_080062c4();
        switch (gUnk_03002490->unk74) {
        case 0:
            TaskYieldTrampoline(30);
            gUnk_03002490->unk4C = 64 << 16;
            sub_080ca570();
            break;
        case 1:
            TaskYieldTrampoline(34);
            gUnk_03002490->unk4C = 96 << 16;
            sub_080ca570();
            break;
        case 2:
            TaskYieldTrampoline(100);
            gUnk_03002490->unk4C = 0;
            sub_080ca570();
            break;
        case 3:
            TaskYieldTrampoline(160);
            TaskYieldTrampoline(210);
            gUnk_03002490->unk4C = 128 << 16;
            sub_080ca570();
            break;
        case 4:
            TaskYieldTrampoline(14);
            TaskYieldTrampoline(210);
            gUnk_03002490->unk4C = 160 << 16;
            sub_080ca570();
            break;
        case 5:
            TaskYieldTrampoline(210);
            gUnk_03002490->unk4C = 32 << 16;
            sub_080ca570();
            break;
        case 6:
            TaskYieldTrampoline(200);
            TaskYieldTrampoline(210);
            gUnk_03002490->unk4C = 80 << 16;
            sub_080ca570();
            break;
        case 7:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(34);
            gUnk_03002490->unk4C = 192 << 16;
            sub_080ca570();
            break;
        case 8:
            TaskYieldTrampoline(30);
            gUnk_03002490->unk4C = 64 << 16;
            sub_080ca640();
            break;
        case 9:
            TaskYieldTrampoline(90);
            gUnk_03002490->unk4C = 144 << 16;
            sub_080ca640();
            break;
        case 10:
            TaskYieldTrampoline(130);
            gUnk_03002490->unk4C = 96 << 16;
            sub_080ca640();
            break;
        case 11:
            TaskYieldTrampoline(180);
            gUnk_03002490->unk4C = 32 << 16;
            sub_080ca640();
            break;
        case 12:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(98);
            gUnk_03002490->unk4C = 128 << 16;
            sub_080ca640();
            break;
        case 13:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(160);
            gUnk_03002490->unk4C = 240 << 16;
            sub_080ca640();
            break;
        case 14:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(130);
            gUnk_03002490->unk4C = 160 << 16;
            sub_080ca640();
            break;
        case 15:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(98);
            gUnk_03002490->unk4C = 192 << 16;
            sub_080ca640();
            break;
        }
    }
}

/* Task type #101 variant 10, one of its two falling kinds: sway left and
   right five times while drifting down. */
void sub_080ca570(void)
{
    gUnk_03002490->unk38 = gUnk_0874C44C;
    gUnk_03002490->unk3C = sub_08002ee8(2) * 2 + 4;
    gUnk_03002490->unk50 = -0x80000;
    gUnk_03002490->unk58 = 0x8000;
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk54 = 0x2000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x10000;
        TaskYieldTrampoline(20);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x3000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x800;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = -0x8000;
        TaskYieldTrampoline(20);
        gUnk_03002490->unk54 = -0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = -0x800;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 4);
}

/* Task type #101 variant 10, the other falling kind: sway right and left
   ten times while drifting down. */
void sub_080ca640(void)
{
    gUnk_03002490->unk38 = gUnk_0874C500;
    gUnk_03002490->unk3C = sub_08002ee8(4) + 4;
    gUnk_03002490->unk50 = -0x80000;
    gUnk_03002490->unk58 = 0x4000;
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk54 = -0x2000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = -0x10000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk54 = -0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = -0x2000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x800;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk54 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x8000;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk54 = 0x2000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk54 = 0x800;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
}

/* Task type #101 variant 6. */
void sub_080ca71c(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0801a3e4;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08755440;
    gUnk_03002490->unk40 = 0xD350;
    gUnk_03002490->unk4C = 128 << 16;
    gUnk_03002490->unk50 = 192 << 16;
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(131);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 7;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 10);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08006138();
}

/* Task type #101 variant 7. */
void sub_080ca830(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0801a3e4;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875546C;
    gUnk_03002490->unk40 = 0xD350;
    gUnk_03002490->unk4C = 128 << 16;
    gUnk_03002490->unk50 = 192 << 16;
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(115);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(65);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(200);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(72);
    gUnk_03002490->unk3C = -1;
    sub_08006138();
}

/* Task type #101 variant 8. */
void sub_080ca8f0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_0801a3e4;
    gUnk_03002490->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_08755484;
    gUnk_03002490->unk40 = 0xD350;
    gUnk_03002490->unk4C = 128 << 16;
    gUnk_03002490->unk50 = 192 << 16;
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(140);
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 1;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk3C = 5;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08006138();
}
