#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* ending_c7e4c.c (0x080C7E4C-0x080C9003, issue #100).
 *
 * The first ending scene, part 2: task type #100 variants 4-10.
 *   sub_080c7e4c / sub_080c8298   variant 4, the long scripted sprite, and
 *       its draw callback (follows the task Task.unk44 while Task.unk28 is
 *       set, cycles a palette blend, draws scaled like it); near the end it
 *       spawns variant 5 (sub_080c8468).
 *   sub_080c8498   variant 5, an effect drifting away from its spawner.
 *   sub_080c85d8 / sub_080c8778   variant 6: hidden for 2278 frames, then a
 *       sprite that drifts in, shrinks while it fades in and falls away.
 *   sub_080c88f0   variant 7: after 240 frames spawns the nine variant-8
 *       sprites (sub_080c8924) and calls M07's sub_080269e8.
 *   sub_080c8958   variant 8, nine sprites: the centre one flickers, the
 *       other eight fly outwards along gUnk_08757374[]..gUnk_087573D4[].
 *   sub_080c8cd4 / sub_080c8e88   variant 9, an invisible task the camera
 *       follows (M07's sub_08026278) that shakes vertically.
 *   sub_080c8ea8   variant 10: after 900 frames, eleven times, copy the next
 *       4 KiB of BG tiles from the decompression buffer, blend them in, hold
 *       and blend them out; then turn BG0 off. */

extern vu16 gDispCnt;          /* DISPCNT shadow */
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u32 gUnk_08755708[];
extern u16 gObjPalette[];
extern u16 gFrameCount;
extern u16 gUnk_0859A0B0[];
extern u16 gUnk_0859A0D0[];
extern u8 gUnk_08757368[];
extern s16 gUnk_0873FF98[];
extern u32 gUnk_0875581C[];
extern u16 gUnk_0859A0F0[];
extern u16 gUnk_0859A110[];
extern u32 gUnk_0874CEE8[];
extern s32 gUnk_08757374[];
extern s32 gUnk_08757394[];
extern s32 gUnk_087573B4[];
extern s32 gUnk_087573D4[];
extern vs16 gBrightness;
extern vu8 gBldCntTarget1;
extern vu8 gBldCntTarget2;
extern vu8 gBldAlphaEva;
extern vu8 gBldAlphaEvb;

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskMove(void);
void TaskDrawScreen(void);
void TaskDrawScreenOrFree(void);
void TaskSleepForever(void);                                     /* end the running task */
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void sub_08026278(s32 x, s32 y);
void sub_080269e8(void);
void sub_080c8298(void);
void sub_080c8468(void);
void sub_080c8778(void);
void sub_080c8924(void);
void sub_080c8e88(void);

/* Task type #100 variant 4: the long scripted sprite of the first ending
   scene; its draw callback sub_080c8298 cycles the palette blend.  Near
   the end it spawns variant 5 (sub_080c8468) with sound 0x121. */
void sub_080c7e4c(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_080c8298;
    gCurTask->unk42 = 7;
    gCurTask->unk38 = gUnk_08755708;
    gCurTask->unk40 = 0x8810;
    gCurTask->unk28 = 1;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(64);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(252);
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(60);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(96);
    gCurTask->unk3C = -1;
    TaskYieldTrampoline(46);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(208);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(128);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(170);
    TaskYieldTrampoline(16);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(68);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(10);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(6);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(6);
        gCurTask->unk3C = 12;
        TaskYieldTrampoline(10);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(6);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(6);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk3C = 22;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 20;
    TaskYieldTrampoline(10);
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(8);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 60;
    TaskYieldTrampoline(134);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(12);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(152);
    TaskYieldTrampoline(2);
    gCurTask->unk28 = 0;
    gCurTask->unk3C = 24;
    gCurTask->unk54 = 0x90000;
    gCurTask->unk58 = -0xFB000;
    gCurTask->unk60 = 0x2000;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = -0x3E000;
    gCurTask->unk60 = 0x2000;
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(28);
    gCurTask->unk3C = 26;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 28;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 30;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk3C = 28;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3C = 32;
    gCurTask->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3C = 34;
    TaskYieldTrampoline(8);
    gCurTask->unk5C = 0x2000;
    TaskYieldTrampoline(6);
    TaskStop();
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 36;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 38;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    gCurTask->unk5C = -0x2000;
    gCurTask->unk58 = -0x1C000;
    gCurTask->unk60 = 0x8000;
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 44;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk58 = 0x28000;
    gCurTask->unk60 = -0x10000;
    sub_080c8468();
    PlaySfx(0x121);
    gCurTask->unk3C = 46;
    TaskYieldTrampoline(4);
    TaskStop();
    gCurTask->unk3C = 46;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x2B000;
    gCurTask->unk60 = 0x2000;
    gCurTask->unk3C = 47;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 48;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 49;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 50;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 51;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 52;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 53;
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 48;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 54;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->unk3C = 55;
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

/* The draw callback task type #100 variant 4 installs: follows the task
   Task.unk44 while Task.unk28 is set, cycles a palette blend, and draws the
   sprite scaled like the followed task. */
void sub_080c8298(void)
{
    struct Task *p;
    struct Task *t;
    u32 *tbl;
    s32 k;
    s32 n;
    s16 s;

    if (gCurTask->unk28 != 0) {
        gCurTask->unk48 = gTasks[gCurTask->unk44].unk48;
        gCurTask->unk4A = gTasks[gCurTask->unk44].unk4A;
        gCurTask->unk4C = gCurTask->unk48 << 16;
        gCurTask->unk50 = gCurTask->unk4A << 16;
    }
    p = &gTasks[gCurTask->unk44];
    if (gCurTask->unk38 == NULL)
        return;
    if (gCurTask->unk3C == -1)
        return;
    if (p->unk38 == NULL)
        return;
    if (p->unk3C == -1)
        return;
    k = 0;
    if ((u16)gCurTask->unk3C <= 45 || gCurTask->unk3C == 60) {
        if ((gFrameCount & 3) == 0)
            k = 1;
        n = gCurTask->unk34 + 1;
        gCurTask->unk34 = n;
        if (n < 0)
            gCurTask->unk34 = 0;
        if (gCurTask->unk34 > 10)
            gCurTask->unk34 = 0;
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, gUnk_08757368[gCurTask->unk34], 16,
                     &gObjPalette[((gCurTask->unk40 >> 12) + 2) * 16]);
    }
    t = gCurTask;
    if ((u16)(t->unk48 + 63) <= 366 && t->unk4A > -64 && t->unk4A <= 223) {
        tbl = t->unk38;
        s = gUnk_0873FF98[p->unk18 >> 16];
        if ((u16)s != 0x100)
            QueueSprite(gCurTask->unk42, DrawAffineSprite(tbl[t->unk3C + k], s, s, 0),
                         gCurTask->unk3E, gCurTask->unk40, gCurTask->unk48, gCurTask->unk4A);
        else
            QueueSprite(t->unk42, tbl[t->unk3C + k], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

/* Spawn task type #100 variant 5 next to the calling task (Task.unk44). */
void sub_080c8468(void)
{
    s32 id;
    struct Task *t;

    id = TaskCreateFrom(100, 32);
    t = &gTasks[id];
    t->unk73 = 5;
    t->unk44 = gCurTaskIdx;
}

/* Task type #100 variant 5: an effect that starts next to the task that
   spawned it (Task.unk44), drifts up and left, and loops a 16-frame
   animation. */
void sub_080c8498(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreenOrFree;
    gCurTask->unk42 = 8;
    gCurTask->unk38 = gUnk_0875581C;
    gCurTask->unk40 = 0xA000;
    gCurTask->unk4C = (gTasks[gCurTask->unk44].unk48 - 8) << 16;
    gCurTask->unk50 = (gTasks[gCurTask->unk44].unk4A - 4) << 16;
    gCurTask->unk54 = -0x40000;
    gCurTask->unk58 = -0x30000;
    for (;;) {
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 7;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 12;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(2);
    }
}

/* Task type #100 variant 6: hidden for 2278 frames, then a sprite that
   drifts in from (224, 208), shrinks from 63.0 while it fades in, and
   falls away. */
void sub_080c85d8(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_080c8778;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_08755708;
    gCurTask->unk40 = 0x8810;
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(78);
    gCurTask->unk4C = 224 << 16;
    gCurTask->unk50 = 208 << 16;
    gCurTask->unk18 = 63 << 16;
    gCurTask->unk28 = 0;
    gCurTask->unk3C = 64;
    gCurTask->unk54 = -0x1200;
    gCurTask->unk58 = -0x4400;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(16);
    gCurTask->unk54 = 0x800;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(64);
    gCurTask->unk54 = 0x400;
    gCurTask->unk58 = -0x5000;
    TaskYieldTrampoline(72);
    TaskSetMotion(0, 0, 0x5A5A5A5A, -0x200, 0x1000, 0x5A5A5A5A);
    TaskYieldTrampoline(24);
    gCurTask->unk2C = 5;
    gCurTask->unk28 = -0x71C7;
    TaskSetMotion(-0x4000, 0, 0x5A5A5A5A, 0x18000, -0x600, 0x5A5A5A5A);
    TaskYieldTrampoline(72);
    gCurTask->unk18 = 63 << 16;
    gCurTask->unk28 = 0;
    gCurTask->unk2C = 0;
    gCurTask->unk42 = 13;
    gCurTask->unk5C = 0x800;
    gCurTask->unk3C = 66;
    TaskYieldTrampoline(12);
    gCurTask->unk3C = 67;
    TaskYieldTrampoline(12);
    gCurTask->unk60 = -0x400;
    gCurTask->unk3C = 68;
    TaskYieldTrampoline(60);
    TaskStop();
    TaskExitTrampoline();
}

/* The draw callback task type #100 variant 6 installs: grows the scale
   Task.unk18 by Task.unk28 (up to 127.0), fades the palette in by Task.unk2C
   per frame, and draws the sprite scaled, frame 64 alternating with 65. */
void sub_080c8778(void)
{
    struct Task *t;
    u32 *tbl;
    s32 k;
    s16 s;

    gCurTask->unk18 += gCurTask->unk28;
    if (gCurTask->unk18 < 0)
        gCurTask->unk18 = 0;
    if (gCurTask->unk18 >> 16 > 127)
        gCurTask->unk18 = 127 << 16;
    if (gCurTask->unk2C > 0) {
        gCurTask->unk1C += gCurTask->unk2C;
        if (gCurTask->unk1C > 255)
            gCurTask->unk1C = 255;
        BlendColors(gUnk_0859A0F0, gUnk_0859A110, (u16)gCurTask->unk1C, 16,
                     &gObjPalette[(gCurTask->unk40 >> 12) * 16]);
    }
    if (gCurTask->unk38 == NULL)
        return;
    if (gCurTask->unk3C == -1)
        return;
    k = 0;
    if (gCurTask->unk3C == 64) {
        if ((s16)gCurTask->unk70 > 3)
            k = 1;
        gCurTask->unk70++;
        if ((s16)gCurTask->unk70 > 7)
            gCurTask->unk70 = 0;
    }
    t = gCurTask;
    if ((u16)(t->unk48 + 63) <= 366 && t->unk4A > -64 && t->unk4A <= 223) {
        tbl = t->unk38;
        s = gUnk_0873FF98[t->unk18 >> 16];
        if ((u16)s != 0x100)
            QueueSprite(gCurTask->unk42, DrawAffineSprite(tbl[t->unk3C + k], s, s, 0),
                         gCurTask->unk3E, gCurTask->unk40, gCurTask->unk48, gCurTask->unk4A);
        else
            QueueSprite(t->unk42, tbl[t->unk3C + k], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

/* Task type #100 variant 7. */
void sub_080c88f0(void)
{
    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    TaskYieldTrampoline(240);
    sub_080c8924();
    PlaySfx(282);
    TaskYieldTrampoline(12);
    sub_080269e8();
    TaskExitTrampoline();
}

/* Spawn the nine task type #100 variant-8 sprites (Task.unk74 = 0..8). */
void sub_080c8924(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    for (i = 0; i <= 8; i++) {
        id = TaskCreateFrom(100, 32);
        t = &gTasks[id];
        t->unk73 = 8;
        t->unk74 = i;
    }
}

/* Task type #100 variant 8 (nine of them, Task.unk74 = 0 .. 8, spawned by
   sub_080c8924): at (103, 52) the centre one (0) flickers through its
   frames, the other eight fly outwards along gUnk_08757374[] ..
   gUnk_087573D4[] while their frames blink. */
void sub_080c8958(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 12;
    gCurTask->unk38 = gUnk_0874CEE8;
    gCurTask->unk40 = 0;
    gCurTask->unk4C = 103 << 16;
    gCurTask->unk50 = 52 << 16;
    switch (gCurTask->unk74) {
    case 0:
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 12;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(3);
            gCurTask->unk3C = 12;
            TaskYieldTrampoline(1);
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(3);
            gCurTask->unk3C = 12;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 9);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 14;
            TaskYieldTrampoline(1);
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 12;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 11);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(5);
            gCurTask->unk3C = 14;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
        break;
    case 1 ... 8:
        gCurTask->unk54 = gUnk_08757374[gCurTask->unk74 - 1];
        gCurTask->unk58 = gUnk_08757394[gCurTask->unk74 - 1];
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(3);
        gCurTask->unk54 = gUnk_087573B4[gCurTask->unk74 - 1];
        gCurTask->unk58 = gUnk_087573D4[gCurTask->unk74 - 1];
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(1);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 8;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = -1;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 7;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = -1;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 9;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = -1;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 11);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 8;
            TaskYieldTrampoline(2);
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 9;
            TaskYieldTrampoline(2);
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 10;
            TaskYieldTrampoline(1);
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 11;
            TaskYieldTrampoline(1);
            gCurTask->unk3C |= -1;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        break;
    }
    TaskExitTrampoline();
}

/* Task type #100 variant 9: an invisible task at the camera's (120, 80)
   that shakes vertically with a growing and then shrinking amplitude
   (Task.unk04 = sub_080c8e88 follows it), with a 4-frame flash between. */
void sub_080c8cd4(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = 0;
    gCurTask->unk04 = (u32)sub_080c8e88;
    gCurTask->unk4C = (gSpriteCameraX + 120) << 16;
    gCurTask->unk50 = (gSpriteCameraY + 80) << 16;
    TaskStop();
    TaskYieldTrampoline(60);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 14);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk58 = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 14);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk58 = 0x30000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = -0x30000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 13);
    TaskStop();
    gBrightness = 31;
    TaskYieldTrampoline(4);
    gBrightness = 0;
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk58 = 0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
    TaskStop();
    TaskYieldTrampoline(120);
    TaskYieldTrampoline(64);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(54);
    gCurTask->unk58 = 0x3000;
    TaskSleepForever();
}

/* Task type #100 variant 9's per-frame hook: move the camera target to
   the task's position (M07's sub_08026278). */
void sub_080c8e88(void)
{
    struct Task *t = gCurTask;

    sub_08026278(t->unk48, t->unk4A);
}

/* Task type #100 variant 10: after 900 frames, eleven times: copy the next
   4 KiB of BG tiles from the decompression buffer, blend them in over 32
   frames, hold 224 frames and blend them out again; then clear the
   blend registers and turn BG0 off. */
void sub_080c8ea8(void)
{
    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    gBldCntTarget1 = 65;
    gBldCntTarget2 = 28;
    gBldAlphaEva = 0;
    gBldAlphaEvb = 16;
    gCurTask->unk6C = 0;
    do {
        TaskYieldTrampoline(60);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 14);
    gCurTask->unk6C = 0;
    do {
        RequestCopy(1, (u32)gUnk_02020000 + ((s16)gCurTask->unk6C << 12), 0x06000000, 0x1000);
        gCurTask->unk6E = 0;
        do {
            gBldAlphaEva = (gCurTask->unk6E + 1) >> 1;
            gBldAlphaEvb = 16 - gBldAlphaEva;
            TaskYieldTrampoline(1);
            gCurTask->unk6E++;
        } while (gCurTask->unk6E <= 31);
        TaskYieldTrampoline(224);
        gCurTask->unk6E = 0;
        do {
            gBldAlphaEvb = (gCurTask->unk6E + 1) >> 1;
            gBldAlphaEva = 16 - gBldAlphaEvb;
            TaskYieldTrampoline(1);
            gCurTask->unk6E++;
        } while (gCurTask->unk6E <= 31);
        TaskYieldTrampoline(12);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = 0;
    gDispCnt &= 0xFEFF;
    TaskExitTrampoline();
}
