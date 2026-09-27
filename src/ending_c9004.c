#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* ending_c9004.c (0x080C9004-0x080CAA3B, issue #100).
 *
 * The second ending scene: task type #101 (class 3), which M37's
 * sub_080c6388 spawns in AgbMain state 11 and waits on (gEndingSceneActive).
 *   sub_080c9004   the body: variant 0 loads the graphics (sub_080c9040) and
 *       spawns variants 1, 3, 6, 7, 8 and 11 from the list gUnk_08757424
 *       (sub_080c90c8); variants 1-11 run gUnk_087573F4[Task.unk73].
 *   sub_080c9114 / sub_080c9418   variant 1, a sprite that falls in, then
 *       rides the BG3 layer and flashes its palette.
 *   sub_080c9d10   variant 2, a 22-frame animation played six times.
 *   sub_080c94cc / sub_080c972c / sub_080c97a0   variant 3, which spawns its
 *       variant-5 companion, crosses the screen twice leaving variant-4
 *       tasks behind, fades the music out and ends the scene (clearing
 *       gEndingSceneActive).
 *   sub_080c9884, sub_080c98d8 / sub_080c9974   variants 4 and 5.
 *   sub_080ca71c, sub_080ca830, sub_080ca8f0   variants 6-8, scripted
 *       sprites drawn by M05's sub_0801a3e4.
 *   sub_080c9e8c   variant 9, eleven sprites bursting out of one point.
 *   sub_080ca344 / sub_080ca570 / sub_080ca640   variant 10, sixteen falling
 *       sprites with two sway scripts.
 *   sub_080c9a28 / sub_080c9cf0   variant 11, the finale: palette flashes,
 *       variants 2, 9 and 10, a fade to an OBJ-only display. */

extern vu16 gDispCnt;          /* DISPCNT shadow */
extern vs32 gBg3ScrollY;
extern vs32 gBg3ScrollX;          /* ... BG3 */
extern u16 gUnk_02000028;
extern u16 gUnk_02004C94;
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u16 gUnk_03001570[];         /* palette buffer */
extern u32 gObjVram[];         /* OBJ VRAM */
extern u16 gUnk_080DC628[][16];     /* per-player palettes */
extern void (*gUnk_087573F4[])(void);
extern struct GfxHeader gUnk_0859A09C;
extern u16 gUnk_085E0070[];
extern u32 gUnk_085E0090[];
extern u16 gUnk_08757424[];
extern u32 gUnk_0875585C[];
extern u16 gUnk_0859A0B0[];
extern u16 gUnk_0859A0D0[];
extern u16 gObjPalette[];
extern u8 gEndingSceneActive;
extern u16 gUnk_08757432[];
extern u32 gUnk_0874C500[];
extern u16 gUnk_085E2920[];
extern u16 gUnk_085E2A20[];
extern u16 gUnk_085E2B20[];
extern u16 gUnk_0875743E[];
extern u16 gUnk_03001370[];
extern vu16 gBgPalette[];
extern vu16 gFadeSteps;
extern vs16 gBrightness;
extern vu16 gFadeStep;
extern vu16 gFadeTimer;
extern vu16 gFadeInterval;
extern vu16 gUnk_03000048;
extern u16 *gFadeKeepMask;
extern u32 gUnk_0874CF94[];
extern u32 gUnk_0874C44C[];
extern u32 gUnk_0874CF28[];
extern u32 gUnk_08755440[];
extern u32 gUnk_0875546C[];
extern u32 gUnk_08755484[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 (lesson 3.428) */
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
u32 RandomRange(u32 range);                                 /* random 0 .. range-1 */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);                                    /* play a sound effect */
void SetBgmVolume(u16 volume);
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void TaskMove(void);
u32 TaskIsOnScreen(void);
u32 TaskIsInView(void);
void TaskDrawScreen(void);
void TaskDrawWorld(void);
void TaskSleepForever(void);                                     /* end the running task */
void TaskStop(void);
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
    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    if (gCurTask->unk73 == 0) {
        sub_080c9040();
        sub_080c90c8();
    } else {
        CallTableEntry(gCurTask->unk73, 12, gUnk_087573F4);
    }
    TaskExitTrampoline();
}

/* Task type #101's graphics. */
void sub_080c9040(void)
{
    struct GfxHeader *h = &gUnk_0859A09C;

    LZ77UnCompWram(h->unk0C, gUnk_02020000);
    RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, h->unk02 << 5);
    RequestCopy(2, (u32)h->unk08, (u32)gUnk_03001570, h->unk00 << 5);
    if (gUnk_02004C94 > 1)
        RequestCopy(2, (u32)gUnk_080DC628[gUnk_02000028], (u32)gUnk_03001570, 22);
    RequestCopy(2, (u32)gUnk_085E0070, (u32)&gUnk_03001570[80], 32);
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
        id = TaskCreateFrom(101, 32);
        if (id == -1)
            for (;;)
                ;
        t = &gTasks[id];
        t->unk73 = v;
    }
}

/* Task type #101 variant 1: a sprite at the camera's (120, -24) that falls
   in, plays a 16-frame animation twice, then pins itself to the BG3 layer
   (sub_080c9418) and flashes its palette ever faster. */
void sub_080c9114(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_0875585C;
    gCurTask->unk40 = 0x8810;
    gCurTask->unk4C = (gSpriteCameraX + 120) << 16;
    gCurTask->unk50 = (gSpriteCameraY - 24) << 16;
    TaskYieldTrampoline(32);
    TaskStop();
    gCurTask->unk58 = 0x10000;
    gCurTask->unk6C = 0;
    do {
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
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 16;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 17;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 18;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk3C = 23;
    TaskYieldTrampoline(3);
    TaskStop();
    PlaySfx(238);
    gCurTask->unk0C = (u32)sub_080c9418;
    gCurTask->unk4C = (gCurTask->unk48 - gSpriteCameraX + (gBg3ScrollX >> 16)) << 16;
    gCurTask->unk50 = (gCurTask->unk4A - gSpriteCameraY + (gBg3ScrollY >> 16)) << 16;
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gObjPalette + ((gCurTask->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(5);
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gObjPalette + ((gCurTask->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 11);
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gObjPalette + ((gCurTask->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(4);
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gObjPalette + ((gCurTask->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 11);
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 0, 16,
                     gObjPalette + ((gCurTask->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(3);
        BlendColors(gUnk_0859A0B0, gUnk_0859A0D0, 255, 16,
                     gObjPalette + ((gCurTask->unk40 >> 12) + 1) * 16);
        TaskYieldTrampoline(3);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    TaskYieldTrampoline(96);
    gCurTask->unk3C = 0xFFFF;
    TaskSleepForever();
}

void sub_080c9418(void)
{
    struct Task *t;
    u32 *g;

    if (gCurTask->unk38 == NULL)
        return;
    if (gCurTask->unk3C == -1)
        return;
    if (TaskIsInView() == 0)
        return;
    t = gCurTask;
    if (t->unk48 - (gBg3ScrollX >> 16) > -64 && t->unk48 - (gBg3ScrollX >> 16) <= 303
        && t->unk4A - (gBg3ScrollY >> 16) > -64 && t->unk4A - (gBg3ScrollY >> 16) <= 223) {
        g = t->unk38;
        QueueSprite(t->unk42, g[t->unk3C], t->unk3E, t->unk40,
                     t->unk48 - (gBg3ScrollX >> 16), t->unk4A - (gBg3ScrollY >> 16));
    }
}

/* Task type #101 variant 3: spawns its variant-5 companion (which follows
   it), flies across the screen twice leaving a trail of variant-4 tasks
   (sub_080c972c while Task.unk28 is set), then fades the music out
   (sub_080c97a0) and clears gEndingSceneActive. */
void sub_080c94cc(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_080c97a0;
    gCurTask->unk42 = 8;
    gCurTask->unk38 = gUnk_0875585C;
    gCurTask->unk40 = 0x8810;
    gCurTask->unk04 = (u32)sub_080c972c;
    gCurTask->unk34 = 255;
    gCurTask->unk24 = 0;
    gCurTask->unk46 = TaskCreateFrom(101, 32);
    gTasks[gCurTask->unk46].unk73 = 5;
    gTasks[gCurTask->unk46].unk44 = gCurTaskIdx;
    TaskStop();
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(99);
    TaskYieldTrampoline(96);
    gCurTask->unk28 = 1;
    gCurTask->unk4C = (gSpriteCameraX + 255) << 16;
    gCurTask->unk50 = (gSpriteCameraY + 80) << 16;
    PlaySfx(290);
    gCurTask->unk3C = 5;
    gCurTask->unk54 = -0x80000;
    gCurTask->unk58 = 0x10000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x8000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x2000;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = -0x8000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0xC000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x14000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = -0x20000;
    TaskYieldTrampoline(13);
    TaskStop();
    gCurTask->unk28 = 0;
    TaskYieldTrampoline(70);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(97);
    TaskYieldTrampoline(210);
    PlaySfx(291);
    gCurTask->unk4C = (gSpriteCameraX - 16) << 16;
    gCurTask->unk50 = (gSpriteCameraY + 176) << 16;
    gCurTask->unk54 = 0x80000;
    gCurTask->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x40000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x14000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x10000;
    TaskYieldTrampoline(24);
    TaskYieldTrampoline(20);
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = (gCurTask->unk4A << 16) - 1;
    gCurTask->unk54 = 0xE000;
    gCurTask->unk58 = -0xE000;
    TaskYieldTrampoline(20);
    gCurTask->unk54 = 0xC000;
    gCurTask->unk58 = -0xC000;
    TaskYieldTrampoline(80);
    TaskYieldTrampoline(82);
    gCurTask->unk24 = 2;
    TaskYieldTrampoline(128);
    gEndingSceneActive = 0;
    TaskSleepForever();
}

/* Task type #101 variant 3's per-frame callback: while Task.unk28 is set,
   spawn a variant-4 task at this task's position every third frame. */
void sub_080c972c(void)
{
    s32 id;
    struct Task *t;

    if (gCurTask->unk28 != 0) {
        if (--gCurTask->unk18 <= 0) {
            id = TaskCreateFrom(101, 32);
            t = &gTasks[id];
            t->unk73 = 4;
            t->unk48 = gCurTask->unk48;
            t->unk4A = gCurTask->unk4A;
            t->unk4C = t->unk48 << 16;
            t->unk50 = t->unk4A << 16;
        }
        if (gCurTask->unk18 <= 0)
            gCurTask->unk18 = 3;
    }
}

/* Task type #101 variant 3's draw callback: cycle the frame offset through
   gUnk_08757432[6], draw, and fade the music out by Task.unk24 per frame
   (volume in Task.unk34). */
void sub_080c97a0(void)
{
    s16 off;

    if ((s16)gCurTask->unk70 < 0)
        gCurTask->unk70 = 0;
    if ((s16)gCurTask->unk70 > 5)
        gCurTask->unk70 = 0;
    off = gUnk_08757432[(s16)gCurTask->unk70];
    gCurTask->unk70++;
    if (gCurTask->unk38 != NULL && gCurTask->unk3C != -1 && TaskIsOnScreen() != 0) {
        struct Task *u = gCurTask;
        u32 *g = u->unk38;

        QueueSprite(u->unk42, g[u->unk3C + off], u->unk3E, u->unk40,
                     u->unk48 - gSpriteCameraX, u->unk4A - gSpriteCameraY);
    }
    if (gCurTask->unk24 != 0) {
        gCurTask->unk34 -= gCurTask->unk24;
        if (gCurTask->unk34 < 0)
            gCurTask->unk34 = 0;
        SetBgmVolume(gCurTask->unk34);
    }
}

/* Task type #101 variant 4. */
void sub_080c9884(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 12;
    gCurTask->unk38 = gUnk_0874C500;
    gCurTask->unk40 = 0;
    gCurTask->unk3C = RandomRange(4) + 4;
    gCurTask->unk58 = 0x40000;
    TaskYieldTrampoline(24);
    TaskExitTrampoline();
}

/* Task type #101 variant 5: a sprite drawn at another task's position
   (sub_080c9974), flipped after 863 frames, then looping a 4-frame
   animation. */
void sub_080c98d8(void)
{
    gCurTask->unk00 = 0;
    gCurTask->unk0C = (u32)sub_080c9974;
    gCurTask->unk42 = 7;
    gCurTask->unk38 = gUnk_0875585C;
    gCurTask->unk40 = 0x8810;
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(207);
    gCurTask->unk3E |= 0x8000;
    TaskYieldTrampoline(56);
    for (;;) {
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(2);
    }
}

void sub_080c9974(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    u32 *g;

    t->unk48 = gTasks[t->unk44].unk48;
    t->unk4A = gTasks[t->unk44].unk4A;
    if (t->unk38 != NULL && t->unk3C != -1 && TaskIsOnScreen() != 0) {
        u = gCurTask;
        g = u->unk38;
        QueueSprite(u->unk42, g[u->unk3C], u->unk3E, u->unk40,
                     u->unk48 - gSpriteCameraX, u->unk4A - gSpriteCameraY);
    }
}

/* Task type #101 variant 11: flash the OBJ palettes, spawn variant 2 and
   eleven variant-9 tasks, fade the screen out, switch the display to OBJ
   only and spawn sixteen variant-10 tasks. */
void sub_080c9a28(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = 0;
    gCurTask->unk04 = (u32)sub_080c9cf0;
    gCurTask->unk4C = (gSpriteCameraX + 120) << 16;
    gCurTask->unk50 = (gSpriteCameraY + 80) << 16;
    TaskStop();
    TaskYieldTrampoline(99);
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_085E2920, gUnk_085E2B20, (u16)((s16)gCurTask->unk6C * 32), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_085E2B20, gUnk_085E2920, (u16)((s16)gCurTask->unk6C * 32), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk6C = 0;
    do {
        BlendColors(gUnk_085E2920, gUnk_085E2A20, (u16)((s16)gCurTask->unk6C * 16), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 15);
    TaskYieldTrampoline(68);
    TaskYieldTrampoline(80);
    gCurTask->unk46 = TaskCreateFrom(101, 32);
    gTasks[gCurTask->unk46].unk73 = 2;
    TaskYieldTrampoline(60);
    TaskYieldTrampoline(60);
    TaskYieldTrampoline(30);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk46 = TaskCreateFrom(101, 32);
        gTasks[gCurTask->unk46].unk73 = 9;
        gTasks[gCurTask->unk46].unk74 = gCurTask->unk6C;
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    PlaySfx(288);
    gCurTask->unk58 = -0x30000;
    TaskYieldTrampoline(16);
    gCurTask->unk58 = -0x20000;
    gFadeSteps = 32;
    gBrightness = 0;
    gFadeStep = 1;
    gFadeTimer = 0;
    gFadeInterval = 1;
    gUnk_03000048 = 0;
    gFadeKeepMask = 0;
    while (gFadeSteps != 0)
        TaskYieldTrampoline(1);
    RequestCopy(2, (u32)gUnk_0875743E, (u32)gBgPalette, 2);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1000;
    gBrightness = 0;
    TaskStop();
    TaskYieldTrampoline(120);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk46 = TaskCreateFrom(101, 32);
        gTasks[gCurTask->unk46].unk73 = 10;
        gTasks[gCurTask->unk46].unk74 = gCurTask->unk6C;
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 15);
    TaskSleepForever();
}

/* Task type #101 variant 11's per-frame hook: move the camera target to
   the task's position (M07's sub_08026278). */
void sub_080c9cf0(void)
{
    struct Task *t = gCurTask;

    sub_08026278(t->unk48, t->unk4A);
}

/* Task type #101 variant 2: a sprite at the camera's (120, 40) that plays
   a 22-frame animation six times. */
void sub_080c9d10(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 12;
    gCurTask->unk38 = gUnk_0874CF94;
    gCurTask->unk40 = 0;
    gCurTask->unk4C = (gSpriteCameraX + 120) << 16;
    gCurTask->unk50 = (gSpriteCameraY + 40) << 16;
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 16;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 17;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 18;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 7;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 12;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 5);
    TaskExitTrampoline();
}

/* Task type #101 variant 9: one of eleven sprites that burst out of the
   camera's (120, 48) point; Task.unk74 picks its sprite, frame, direction
   and speed, each slowing down over four steps. */
void sub_080c9e8c(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 12;
    gCurTask->unk4C = 120 << 16;
    gCurTask->unk50 = 48 << 16;
    gCurTask->unk40 = 0;
    switch (gCurTask->unk74) {
    case 0:
        gCurTask->unk38 = gUnk_0874C500;
        gCurTask->unk3C = 4;
        gCurTask->unk54 = 0x30000;
        gCurTask->unk58 = -0x30000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = 0x14000;
        gCurTask->unk58 = -0x14000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0xC000;
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = 0x4000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 1:
        gCurTask->unk38 = gUnk_0874C500;
        gCurTask->unk3C = 4;
        gCurTask->unk54 = -0x30000;
        gCurTask->unk58 = -0x30000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = -0x14000;
        gCurTask->unk58 = -0x14000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = -0xC000;
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = -0x4000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 2:
        gCurTask->unk38 = gUnk_0874C500;
        gCurTask->unk3C = 4;
        gCurTask->unk58 = -0x40000;
        TaskYieldTrampoline(6);
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(6);
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(18);
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 3:
        gCurTask->unk38 = gUnk_0874CF28;
        gCurTask->unk3C = 24;
        gCurTask->unk54 = 0x14000;
        gCurTask->unk58 = -0x40000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = 0xC000;
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0x8000;
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = 0x2000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 4:
        gCurTask->unk38 = gUnk_0874C44C;
        gCurTask->unk3C = 4;
        gCurTask->unk54 = -0x14000;
        gCurTask->unk58 = -0x40000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = -0xC000;
        gCurTask->unk58 = -0x20000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = -0x8000;
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = -0x2000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(12);
        break;
    case 5:
        gCurTask->unk38 = gUnk_0874C44C;
        gCurTask->unk3C = 4;
        gCurTask->unk54 = 0x40000;
        gCurTask->unk58 = -0x14000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = 0x20000;
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0xC000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = 0x4000;
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(12);
        break;
    case 6:
        gCurTask->unk38 = gUnk_0874CF28;
        gCurTask->unk3C = 26;
        gCurTask->unk54 = -0x40000;
        gCurTask->unk58 = -0x14000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = -0x20000;
        gCurTask->unk58 = -0xC000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = -0xC000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = -0x4000;
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(12);
        break;
    case 7:
        TaskYieldTrampoline(2);
        gCurTask->unk38 = gUnk_0874CF28;
        gCurTask->unk3C = 26;
        gCurTask->unk54 = 0xA000;
        gCurTask->unk58 = -0x1E000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = 0x8000;
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0x4000;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = 0x2000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(10);
        break;
    case 8:
        TaskYieldTrampoline(2);
        gCurTask->unk38 = gUnk_0874C500;
        gCurTask->unk3C = 4;
        gCurTask->unk54 = -0xA000;
        gCurTask->unk58 = -0x1E000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = -0x8000;
        gCurTask->unk58 = -0x10000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = -0x4000;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = -0x2000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(10);
        break;
    case 9:
        TaskYieldTrampoline(4);
        gCurTask->unk38 = gUnk_0874C500;
        gCurTask->unk3C = 4;
        gCurTask->unk54 = 0x1E000;
        gCurTask->unk58 = -0xA000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = 0x10000;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0x8000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = 0x4000;
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        break;
    case 10:
        TaskYieldTrampoline(4);
        gCurTask->unk38 = gUnk_0874CF28;
        gCurTask->unk3C = 24;
        gCurTask->unk54 = -0x1E000;
        gCurTask->unk58 = -0xA000;
        TaskYieldTrampoline(6);
        gCurTask->unk54 = -0x10000;
        gCurTask->unk58 = -0x8000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = -0x8000;
        gCurTask->unk58 = -0x4000;
        TaskYieldTrampoline(18);
        gCurTask->unk54 = -0x4000;
        gCurTask->unk58 = -0x2000;
        TaskYieldTrampoline(8);
        break;
    }
    TaskExitTrampoline();
}

/* Task type #101 variant 10: one of sixteen falling sprites; Task.unk74
   picks its start delay, its column and (0-7 / 8-15) which of the two
   sway scripts it runs, over and over. */
void sub_080ca344(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 12;
    gCurTask->unk40 = 0;
    for (;;) {
        gCurTask->unk3C = 0xFFFF;
        TaskStop();
        switch (gCurTask->unk74) {
        case 0:
            TaskYieldTrampoline(30);
            gCurTask->unk4C = 64 << 16;
            sub_080ca570();
            break;
        case 1:
            TaskYieldTrampoline(34);
            gCurTask->unk4C = 96 << 16;
            sub_080ca570();
            break;
        case 2:
            TaskYieldTrampoline(100);
            gCurTask->unk4C = 0;
            sub_080ca570();
            break;
        case 3:
            TaskYieldTrampoline(160);
            TaskYieldTrampoline(210);
            gCurTask->unk4C = 128 << 16;
            sub_080ca570();
            break;
        case 4:
            TaskYieldTrampoline(14);
            TaskYieldTrampoline(210);
            gCurTask->unk4C = 160 << 16;
            sub_080ca570();
            break;
        case 5:
            TaskYieldTrampoline(210);
            gCurTask->unk4C = 32 << 16;
            sub_080ca570();
            break;
        case 6:
            TaskYieldTrampoline(200);
            TaskYieldTrampoline(210);
            gCurTask->unk4C = 80 << 16;
            sub_080ca570();
            break;
        case 7:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(34);
            gCurTask->unk4C = 192 << 16;
            sub_080ca570();
            break;
        case 8:
            TaskYieldTrampoline(30);
            gCurTask->unk4C = 64 << 16;
            sub_080ca640();
            break;
        case 9:
            TaskYieldTrampoline(90);
            gCurTask->unk4C = 144 << 16;
            sub_080ca640();
            break;
        case 10:
            TaskYieldTrampoline(130);
            gCurTask->unk4C = 96 << 16;
            sub_080ca640();
            break;
        case 11:
            TaskYieldTrampoline(180);
            gCurTask->unk4C = 32 << 16;
            sub_080ca640();
            break;
        case 12:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(98);
            gCurTask->unk4C = 128 << 16;
            sub_080ca640();
            break;
        case 13:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(160);
            gCurTask->unk4C = 240 << 16;
            sub_080ca640();
            break;
        case 14:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(130);
            gCurTask->unk4C = 160 << 16;
            sub_080ca640();
            break;
        case 15:
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(240);
            TaskYieldTrampoline(98);
            gCurTask->unk4C = 192 << 16;
            sub_080ca640();
            break;
        }
    }
}

/* Task type #101 variant 10, one of its two falling kinds: sway left and
   right five times while drifting down. */
void sub_080ca570(void)
{
    gCurTask->unk38 = gUnk_0874C44C;
    gCurTask->unk3C = RandomRange(2) * 2 + 4;
    gCurTask->unk50 = -0x80000;
    gCurTask->unk58 = 0x8000;
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk54 = 0x2000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x10000;
        TaskYieldTrampoline(20);
        gCurTask->unk54 = 0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x3000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = -0x800;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = -0x8000;
        TaskYieldTrampoline(20);
        gCurTask->unk54 = -0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = -0x800;
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 4);
}

/* Task type #101 variant 10, the other falling kind: sway right and left
   ten times while drifting down. */
void sub_080ca640(void)
{
    gCurTask->unk38 = gUnk_0874C500;
    gCurTask->unk3C = RandomRange(4) + 4;
    gCurTask->unk50 = -0x80000;
    gCurTask->unk58 = 0x4000;
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk54 = -0x2000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = -0x10000;
        TaskYieldTrampoline(16);
        gCurTask->unk54 = -0x8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = -0x2000;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x800;
        TaskYieldTrampoline(4);
        gCurTask->unk54 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x8000;
        TaskYieldTrampoline(16);
        gCurTask->unk54 = 0x2000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0x800;
        TaskYieldTrampoline(4);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
}

/* Task type #101 variant 6. */
void sub_080ca71c(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_0801a3e4;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_08755440;
    gCurTask->unk40 = 0xD350;
    gCurTask->unk4C = 128 << 16;
    gCurTask->unk50 = 192 << 16;
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(131);
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 9;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 8;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 7;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 6;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(5);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 10);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0xFFFF;
    TaskSleepForever();
}

/* Task type #101 variant 7. */
void sub_080ca830(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_0801a3e4;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_0875546C;
    gCurTask->unk40 = 0xD350;
    gCurTask->unk4C = 128 << 16;
    gCurTask->unk50 = 192 << 16;
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(115);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 1;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(65);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(200);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(72);
    gCurTask->unk3C = -1;
    TaskSleepForever();
}

/* Task type #101 variant 8. */
void sub_080ca8f0(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_0801a3e4;
    gCurTask->unk42 = 10;
    gCurTask->unk38 = gUnk_08755484;
    gCurTask->unk40 = 0xD350;
    gCurTask->unk4C = 128 << 16;
    gCurTask->unk50 = 192 << 16;
    gCurTask->unk3C = 0xFFFF;
    TaskYieldTrampoline(140);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = -1;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = -1;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(5);
    gCurTask->unk6C = 0;
    do {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(5);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 1;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 0xFFFF;
    TaskSleepForever();
}
