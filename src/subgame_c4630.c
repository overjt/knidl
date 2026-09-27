#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "subgame.h"

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
 *   AirGrindStartPaletteFade / AirGrindStepPaletteFades / AirGrindStopPaletteFade / sub_080c495c   four palette
 *       fades (gAirGrindPaletteFades[]): start one (source rows, destination in
 *       gObjPalette, period, steps, colour count, repeats), step them every
 *       frame with BlendColors, free one, free all.
 *   sub_080c4a48 ... sub_080c4c30   the HUD sprites: the digit palette, a
 *       digit, a symbol, a frame count as ss:cc, a number with leading blanks
 *       (a goto loop over the divisors gUnk_080CFF70), a ratio capped at 1000,
 *       and a racer's sprite scaled by sub_080c4f60.
 *   AirGrindSeedRandom / AirGrindRandom / AirGrindRandomRange   five LCG streams
 *       M37Game.randomStates[] (x = (x * 61 + 0x579) & 0xFFF): seed all, step one,
 *       step one and scale it to a range. */

/* Not from main.h: this file's view of gObjPalette differs (lesson 3.517). */
extern u16 gObjPalette[];

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
u32 Random(void);                                      /* LCG step */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSleepForever(void);                                     /* end the running task */
u32 sub_080c4f60(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */

void sub_080c4630(s32 idx, s32 x, s32 y, u16 attr)
{
    u32 *tbl = gUnk_08755FA8;
    u32 c = 0x6000;

    QueueSprite(9, tbl[idx], c, attr, x, y);
}

void sub_080c4664(s32 i)
{
    struct M37Game *g = gAirGrindPtr;
    struct M37ObjSet *set = &g->unk0EC;
    struct M37Obj *o = &set->unk04[i];
    u32 r;
    s32 k;
    s16 id;
    s16 *tbl;

    r = AirGrindRandomRange(4, 53);
    k = r & 7;
    tbl = gUnk_080CFF60;
    id = tbl[k];
    while (g->unk0EC.unk74 == id) {
        k = (k + 1) & 7;
        id = tbl[k];
    }
    set->unk74 = id;
    o->unk4 = (id + r) << 16;
    o->unk0 = AirGrindRandomRange(4, 4) + 1;
    o->unk2 = 0xA000;
}

void sub_080c46ec(void)
{
    struct M37Game *g = gAirGrindPtr;
    s32 *last = &g->unk0EC.unk00;
    s32 i;
    s32 x;
    s32 *p;

    gCurTask->drawCallback = (u32)TaskDrawScreen;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->layer = 3;
    gCurTask->spriteFlags = 0;
    gCurTask->pixelY = 40;
    gCurTask->frameTable = gUnk_08755FBC;
    gCurTask->frame = 0xFFFF;
    g->unk0EC.unk74 = 0;
    for (i = 0, p = &g->unk0EC.unk04[0].unk8, x = 0; i <= 6; i++) {
        sub_080c4664(i);
        *p = x;
        p += 4;
        x += 0x340000;
    }
    *last = gAirGrindCoursePtr->scrollPos;
    gCurTask->updateCallback = (u32)sub_080c4790;
    TaskSetSkipMask(1, gCurTaskIdx);
    TaskSleepForever();
}

void sub_080c4790(void)
{
    struct M37Game *g = gAirGrindPtr;
    s32 *last = &g->unk0EC.unk00;
    s32 i;
    struct M37Obj *o;

    for (i = 0, o = g->unk0EC.unk04; i <= 6; o++, i++) {
        struct M37Course *c = gAirGrindCoursePtr;
        s32 x;

        o->unkC = o->unk4 - ((c->players[0].unk10 - 160) << 16) / 4;
        x = o->unk8;
        x += 0xFFFF0000;
        x += (*last - c->scrollPos) << 16;
        o->unk8 = x;
        if (x >> 16 < -64) {
            sub_080c4664(i);
            o->unk8 += 0x1700000;
        }
        sub_080c4630(o->unk0, o->unk8 >> 16, o->unkC >> 16, o->unk2);
    }
    *last = gAirGrindCoursePtr->scrollPos;
}

void sub_080c4818(void)
{
    struct Task *t = gCurTask;

    t->pixelX = (t->posX >> 16) - gAirGrindCoursePtr->scrollPos + 120;
    t->frame = gAirGrindFrame & 1;
    if (t->pixelX < -120) {
        t->frame = 0xFFFF;
        t->lateUpdateCallback = 0;
    }
}

void sub_080c4860(s32 y)
{
    struct Task *t = &gTasks[gAirGrindPtr->unk44C];

    t->posX = y << 16;
    t->lateUpdateCallback = (u32)sub_080c4818;
}

void AirGrindStepPaletteFades(void)
{
    s32 i;

    for (i = 0; i <= 3; i++) {
        if (gAirGrindPaletteFades[i].unk00 != 0) {
            if (--gAirGrindPaletteFades[i].unk04 < 0) {
                gAirGrindPaletteFades[i].unk04 = gAirGrindPaletteFades[i].unk08;
                if (++gAirGrindPaletteFades[i].unk0C >= gAirGrindPaletteFades[i].unk10) {
                    if (gAirGrindPaletteFades[i].unk24 == 0 || --gAirGrindPaletteFades[i].unk24 > 0)
                        gAirGrindPaletteFades[i].unk0C = 0;
                    else {
                        AirGrindStopPaletteFade(i);
                        continue;
                    }
                }
            }
        {
            s32 k = gAirGrindPaletteFades[i].unk0C;
            s32 k1 = k + 1;
            s32 r = (gAirGrindPaletteFades[i].unk08 - gAirGrindPaletteFades[i].unk04) * gAirGrindPaletteFades[i].unk14;
            u16 *pal = (u16 *)gAirGrindPaletteFades[i].unk1C;
            BlendColors(pal + k * 16, pal + k1 * 16, (u16)r, (u16)gAirGrindPaletteFades[i].unk18, (u16 *)gAirGrindPaletteFades[i].unk20);
        }
        }
    }
}

void sub_080c495c(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gAirGrindPaletteFades[i].unk00 = 0;
}

s32 AirGrindStartPaletteFade(u16 *src, s32 pal, s32 period, s32 steps, s32 count, s32 repeat)
{
    s32 i;

    for (i = 0; i < 4; i++)
        if (gAirGrindPaletteFades[i].unk00 == 0)
            break;
    if (i > 3)
        while (1)
            ;
    gAirGrindPaletteFades[i].unk00 = 1;
    gAirGrindPaletteFades[i].unk04 = period;
    gAirGrindPaletteFades[i].unk08 = period;
    gAirGrindPaletteFades[i].unk0C = 0;
    gAirGrindPaletteFades[i].unk10 = steps - 1;
    gAirGrindPaletteFades[i].unk14 = Div(256, period);
    gAirGrindPaletteFades[i].unk18 = count;
    gAirGrindPaletteFades[i].unk1C = (s32)src;
    gAirGrindPaletteFades[i].unk20 = (s32)&gObjPalette[pal];
    gAirGrindPaletteFades[i].unk24 = repeat;
    return i;
}

void AirGrindStopPaletteFade(s32 i)
{
    if (i > 3 || gAirGrindPaletteFades[i].unk00 == 0)
        while (1)
            ;
    gAirGrindPaletteFades[i].unk00 = 0;
}

void sub_080c4a48(s32 pal)
{
    gAirGrindPtr->unk018 = (pal << 12) & 0xF000;
}

void sub_080c4a5c(s32 digit, s32 x, s32 y)
{
    QueueSprite(2, gUnk_08755FEC[digit], 0x2000, gAirGrindPtr->unk018, x, y);
}

void sub_080c4a94(s32 idx, s32 x, s32 y)
{
    QueueSprite(2, gUnk_0875603C[idx], 0x2000, 0, x, y);
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
    QueueSprite(layer, sub_080c4f60(gUnk_08755F54[idx], scale), 0x2000, (pal << 12) & 0xF000, x, y);
}

void AirGrindSeedRandom(void)
{
    u32 seed;
    s32 i;

    seed = Random();
    for (i = 0; i <= 4; i++)
        gAirGrindPtr->randomStates[i] = seed;
}

u32 AirGrindRandom(s32 i)
{
    return gAirGrindPtr->randomStates[i] = (gAirGrindPtr->randomStates[i] * 61 + 0x579) & 0xFFF;
}

u32 AirGrindRandomRange(s32 i, u32 range)
{
    u32 x;

    gAirGrindPtr->randomStates[i] = x = (gAirGrindPtr->randomStates[i] * 61 + 0x579) & 0xFFF;
    return (x * range) >> 12;
}
