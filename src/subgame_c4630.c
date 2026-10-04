#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "subgame.h"

/* subgame_c4630.c (0x080C4630-0x080C4D07, issue #98).
 *
 * Sub-game 2: task type #96 variant 1 and the module's shared helpers.
 * 
 *   AirGrindScenery / AirGrindSceneryUpdate / AirGrindRollSceneryObject / AirGrindDrawSceneryObject   variant 1
 *       (one task, index in AirGrindState.sceneryTaskSlot): seven background objects
 *       (AirGrindState.scenery) that scroll with the camera at their own rate,
 *       re-rolled from LCG stream 4 when they leave the screen (a sprite id
 *       1-4 and one of three height bands gUnk_080CFF60, never the same band
 *       twice in a row).
 *   AirGrindShowCourseSign / AirGrindCourseSignUpdate   put the variant-1 task's own sprite (the
 *       course line sign) at a course position and move it with the scroll
 *       until it leaves the screen.
 *   AirGrindStartPaletteFade / AirGrindStepPaletteFades / AirGrindStopPaletteFade / AirGrindStopAllPaletteFades   four palette
 *       fades (gAirGrindPaletteFades[]): start one (source rows, destination in
 *       gObjPalette, period, steps, colour count, repeats), step them every
 *       frame with BlendColors, free one, free all.
 *   AirGrindSetDigitPalette ... AirGrindDrawRacerSprite   the HUD sprites: the digit palette, a
 *       digit, a symbol, a frame count as ss:cc, a number with leading blanks
 *       (a goto loop over the divisors gUnk_080CFF70), a ratio capped at 1000,
 *       and a racer's sprite scaled by AirGrindScaleSprite.
 *   AirGrindSeedRandom / AirGrindRandom / AirGrindRandomRange   five LCG streams
 *       AirGrindState.randomStates[] (x = (x * 61 + 0x579) & 0xFFF): seed all, step one,
 *       step one and scale it to a range. */

/* Not from main.h: this file's view of gObjPalette differs (lesson 3.517). */
extern u16 gObjPalette[];

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
u32 Random(void);                                      /* LCG step */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSleepForever(void);                                     /* end the running task */
u32 AirGrindScaleSprite(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */

void AirGrindDrawSceneryObject(s32 idx, s32 x, s32 y, u16 attr)
{
    u32 *tbl = gUnk_08755FA8;
    u32 c = 0x6000;

    QueueSprite(9, tbl[idx], c, attr, x, y);
}

void AirGrindRollSceneryObject(s32 i)
{
    struct AirGrindState *g = gAirGrindPtr;
    struct AirGrindScenerySet *set = &g->scenery;
    struct AirGrindSceneryObject *o = &set->unk04[i];
    u32 r;
    s32 k;
    s16 id;
    s16 *tbl;

    r = AirGrindRandomRange(4, 53);
    k = r & 7;
    tbl = gUnk_080CFF60;
    id = tbl[k];
    while (g->scenery.unk74 == id) {
        k = (k + 1) & 7;
        id = tbl[k];
    }
    set->unk74 = id;
    o->unk4 = (id + r) << 16;
    o->unk0 = AirGrindRandomRange(4, 4) + 1;
    o->unk2 = 0xA000;
}

void AirGrindScenery(void)
{
    struct AirGrindState *g = gAirGrindPtr;
    s32 *last = &g->scenery.unk00;
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
    g->scenery.unk74 = 0;
    for (i = 0, p = &g->scenery.unk04[0].unk8, x = 0; i <= 6; i++) {
        AirGrindRollSceneryObject(i);
        *p = x;
        p += 4;
        x += 0x340000;
    }
    *last = gAirGrindCoursePtr->scrollPos;
    gCurTask->updateCallback = (u32)AirGrindSceneryUpdate;
    TaskSetSkipMask(TASK_SKIP_COROUTINE, gCurTaskIdx);
    TaskSleepForever();
}

void AirGrindSceneryUpdate(void)
{
    struct AirGrindState *g = gAirGrindPtr;
    s32 *last = &g->scenery.unk00;
    s32 i;
    struct AirGrindSceneryObject *o;

    for (i = 0, o = g->scenery.unk04; i <= 6; o++, i++) {
        struct AirGrindCourse *c = gAirGrindCoursePtr;
        s32 x;

        o->unkC = o->unk4 - ((c->players[0].screenY - 160) << 16) / 4;
        x = o->unk8;
        x += 0xFFFF0000;
        x += (*last - c->scrollPos) << 16;
        o->unk8 = x;
        if (x >> 16 < -64) {
            AirGrindRollSceneryObject(i);
            o->unk8 += 0x1700000;
        }
        AirGrindDrawSceneryObject(o->unk0, o->unk8 >> 16, o->unkC >> 16, o->unk2);
    }
    *last = gAirGrindCoursePtr->scrollPos;
}

void AirGrindCourseSignUpdate(void)
{
    struct Task *t = gCurTask;

    t->pixelX = (t->posX >> 16) - gAirGrindCoursePtr->scrollPos + 120;
    t->frame = gAirGrindFrame & 1;
    if (t->pixelX < -120) {
        t->frame = 0xFFFF;
        t->lateUpdateCallback = 0;
    }
}

void AirGrindShowCourseSign(s32 y)
{
    struct Task *t = &gTasks[gAirGrindPtr->sceneryTaskSlot];

    t->posX = y << 16;
    t->lateUpdateCallback = (u32)AirGrindCourseSignUpdate;
}

void AirGrindStepPaletteFades(void)
{
    s32 i;

    for (i = 0; i <= 3; i++) {
        if (gAirGrindPaletteFades[i].active != 0) {
            if (--gAirGrindPaletteFades[i].timer < 0) {
                gAirGrindPaletteFades[i].timer = gAirGrindPaletteFades[i].period;
                if (++gAirGrindPaletteFades[i].step >= gAirGrindPaletteFades[i].lastStep) {
                    if (gAirGrindPaletteFades[i].repeatCount == 0 || --gAirGrindPaletteFades[i].repeatCount > 0)
                        gAirGrindPaletteFades[i].step = 0;
                    else {
                        AirGrindStopPaletteFade(i);
                        continue;
                    }
                }
            }
        {
            s32 k = gAirGrindPaletteFades[i].step;
            s32 k1 = k + 1;
            s32 r = (gAirGrindPaletteFades[i].period - gAirGrindPaletteFades[i].timer) * gAirGrindPaletteFades[i].ratioStep;
            u16 *pal = (u16 *)gAirGrindPaletteFades[i].src;
            BlendColors(pal + k * 16, pal + k1 * 16, (u16)r, (u16)gAirGrindPaletteFades[i].colorCount, (u16 *)gAirGrindPaletteFades[i].dst);
        }
        }
    }
}

void AirGrindStopAllPaletteFades(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gAirGrindPaletteFades[i].active = 0;
}

s32 AirGrindStartPaletteFade(u16 *src, s32 pal, s32 period, s32 steps, s32 count, s32 repeat)
{
    s32 i;

    for (i = 0; i < 4; i++)
        if (gAirGrindPaletteFades[i].active == 0)
            break;
    if (i > 3)
        while (1)
            ;
    gAirGrindPaletteFades[i].active = 1;
    gAirGrindPaletteFades[i].timer = period;
    gAirGrindPaletteFades[i].period = period;
    gAirGrindPaletteFades[i].step = 0;
    gAirGrindPaletteFades[i].lastStep = steps - 1;
    gAirGrindPaletteFades[i].ratioStep = Div(256, period);
    gAirGrindPaletteFades[i].colorCount = count;
    gAirGrindPaletteFades[i].src = (s32)src;
    gAirGrindPaletteFades[i].dst = (s32)&gObjPalette[pal];
    gAirGrindPaletteFades[i].repeatCount = repeat;
    return i;
}

void AirGrindStopPaletteFade(s32 i)
{
    if (i > 3 || gAirGrindPaletteFades[i].active == 0)
        while (1)
            ;
    gAirGrindPaletteFades[i].active = 0;
}

void AirGrindSetDigitPalette(s32 pal)
{
    gAirGrindPtr->digitTileWord = (pal << 12) & 0xF000;
}

void AirGrindDrawDigit(s32 digit, s32 x, s32 y)
{
    QueueSprite(2, gUnk_08755FEC[digit], 0x2000, gAirGrindPtr->digitTileWord, x, y);
}

void AirGrindDrawSymbol(s32 idx, s32 x, s32 y)
{
    QueueSprite(2, gUnk_0875603C[idx], 0x2000, 0, x, y);
}

void AirGrindDrawTime(s32 t, s32 x, s32 y)
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
    AirGrindDrawDigit(Mod(frac, 10) + 2, x + 36, y);
    AirGrindDrawDigit(Div(frac, 10) + 2, x + 27, y);
    AirGrindDrawDigit(12, x + 18, y);
    AirGrindDrawDigit(Mod(sec, 10) + 2, x + 9, y);
    AirGrindDrawDigit(Div(sec, 10) + 2, x, y);
}

void AirGrindDrawNumber(s32 n, s32 x, s32 y)
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
        AirGrindDrawDigit(*dp + 2, i * 9 + x, y);
        shown = 1;
    }
    n = Mod(n, *div);
    i++;
    if (div == tbl)
        AirGrindDrawDigit(n + 2, i * 9 + x, y);
    dp--;
    div--;
    if (i <= 2)
        goto loop;
}

void AirGrindDrawRatio(s32 a, s32 b, s32 x, s32 y)
{
    s16 n;

    n = b != 0 ? Div(a * 1000, b) : 0;
    if (n > 1000)
        n = 1000;
    AirGrindDrawNumber(n, x, y);
}

void AirGrindDrawRacerSprite(s32 idx, s32 pal, s32 scale, s32 x, s32 y, u32 layer)
{
    QueueSprite(layer, AirGrindScaleSprite(gUnk_08755F54[idx], scale), 0x2000, (pal << 12) & 0xF000, x, y);
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
