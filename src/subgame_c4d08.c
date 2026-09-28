#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "subgame.h"

/* subgame_c4d08.c (0x080C4D08-0x080C5283, issue #98).
 *
 * Sub-game 2: the racers' motion and drawing, the sprite scaler and the
 * script cursor.
 * 
 *   AirGrindRacerMove   a racer's Task.moveCallback callback: Task.velX (speed) +=
 *       Task.accelX (acceleration), with a catch-up bonus for the computer
 *       racers behind the leader (M37Game.unk1B8), capped by the level's
 *       gUnk_080CFE3C[level][0]; then Task.posX += speed.
 *   AirGrindRacerDraw / AirGrindEffectDrawOrFree   the Task.drawCallback draw callbacks of the
 *       racers (with a blinking extra sprite for three poses) and of the
 *       effect sprites (which end themselves off screen), both drawn scaled
 *       through AirGrindScaleSprite.
 *   AirGrindScaleSprite   copies a sprite's OAM list into M37Game.unk304/unk306[]
 *       with its size and offsets scaled by `scale` (the depth table through
 *       AirGrindGetDepthScale, OBJ sizes from gUnk_080CFF76, double-size affine
 *       objects), fills the affine matrix gOamAffineCount of the OAM shadow
 *       gOamBuffer and returns the address of the first entry written.
 *       Its loop is a goto loop: a do/while hoists the (s16) conversion of
 *       the scale.
 *   AirGrindClearScript / AirGrindStepScript / AirGrindStartScript   the script cursor
 *       gAirGrindScript: clear it, step it (u16 pairs from gUnk_087572EC[id],
 *       0x8000 ends the script, 0x9999 restarts it; the pair lands in unk2/
 *       unk4, which AirGrindScrollCourseTo copies into the course record), and start
 *       script id 1-4 unless a higher-priority one is running. */

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
void TaskFree(s32 id);                                   /* kill task */
s32 sub_080c6258(s32 value);                                 /* PR #133: value / 2 */
u32 AirGrindScaleSprite(u16 *src, s16 scale);                    /* callers pass scale sign-extended (ldrsh / lsls-asrs); the callee narrows it with lsls/lsrs */

void AirGrindRacerDraw(void)
{
    struct Task *t = gCurTask;
    u32 *tbl = t->frameTable;
    u32 *p;
    s32 n;

    if (tbl != NULL && t->frame != -1 && (u16)(t->pixelX + 63) <= 366
        && t->pixelY > -64 && t->pixelY < 224) {
        p = tbl;
        if (gAirGrindFrame & 2) {
            n = 0;
            switch (t->frame) {
            case 0:
                n = 17;
                break;
            case 3:
                n = 18;
                break;
            case 6:
                n = 19;
                break;
            }
            if (n != 0)
                QueueSprite(gCurTask->layer, AirGrindScaleSprite((u16 *)p[n], gCurTask->unk28),
                             gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->pixelX, gCurTask->pixelY);
        }
        QueueSprite(gCurTask->layer, AirGrindScaleSprite((u16 *)p[gCurTask->frame], gCurTask->unk28),
                     gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->pixelX, gCurTask->pixelY);
    }
}

void AirGrindEffectDrawOrFree(void)
{
    struct Task *t = gCurTask;
    u32 *tbl = t->frameTable;

    if (tbl != NULL && t->frame != -1) {
        if ((u16)(t->pixelX + 63) <= 366 && t->pixelY > -64 && t->pixelY < 224)
            QueueSprite(gCurTask->layer, AirGrindScaleSprite((u16 *)tbl[t->frame], t->unk28),
                         gCurTask->spriteFlags, gCurTask->tileWord, gCurTask->pixelX, gCurTask->pixelY);
        else
            TaskFree(gCurTaskIdx);
    }
}

void AirGrindRacerMove(void)
{
    struct M37Game *g = gAirGrindPtr;
    u16 n = g->playerCount;
    struct Task *t;
    s32 v;
    s32 d;

    if (n <= 1 || gUnk_080CFE2C[g->localPlayer][gCurTask->unk1C] >= n) {
        gCurTask->velX += gCurTask->accelX;
    } else {
        t = gCurTask;
        v = t->accelX;
        if (v > 0) {
            d = g->unk1B8 - (t->posX >> 16);
            if (d > 256)
                d = 256;
            t->velX += v + ((v * d) >> 8);
        } else {
            t->velX += v;
        }
    }
    if (gCurTask->velX > 0) {
        if (gCurTask->velX > gUnk_080CFE3C[gAirGrindPtr->level][0])
            gCurTask->velX = gUnk_080CFE3C[gAirGrindPtr->level][0];
    } else if (gCurTask->velX < 0) {
        gCurTask->velX = 0;
    }
    gCurTask->posX += gCurTask->velX;
}

u32 AirGrindScaleSprite(u16 *src, s16 scale)
{
    u16 s = scale;
    u32 ret;
    u16 a, b;
    s32 w, h;
    u16 dbl;
    u16 half;
    s32 k;
    s32 v;

    ret = (u32)&gAirGrindPtr->unk306[gAirGrindPtr->unk304];
loop:
    a = *src++;
    b = *src++;
    w = gUnk_080CFF76[a >> 14][b >> 14][0];
    h = gUnk_080CFF76[a >> 14][b >> 14][1];
    half = sub_080c6258((s16)s);
    if (((s16)half & 0xFF00) || (w == 8 && h == 8))
        dbl = 0;
    else
        dbl = 0x200;
    k = AirGrindGetDepthScale((s16)s);
    v = a & 0xFF;
    if (v & 0x80)
        v |= 0xFFFFFF00;
    if (dbl != 0)
        h -= (k * (h >> 1)) >> 8;
    else
        h = h - ((k * (h >> 1)) >> 8) - (h >> 1);
    v = k * v;
    if (v < 0)
        v += 128;
    else
        v -= 128;
    v >>= 8;
    v -= h;
    v &= 0xFF;
    gAirGrindPtr->unk306[gAirGrindPtr->unk304++] = (s16)((a & 0xFF00) | v | 0x100) | (s16)dbl;
    v = b & 0x1FF;
    if (v & 0x100)
        v |= 0xFFFFFF00;
    if (dbl != 0)
        w -= (k * (w >> 1)) >> 8;
    else
        w = w - ((k * (w >> 1)) >> 8) - (w >> 1);
    v = k * v;
    if (v < 0)
        v += 128;
    else
        v -= 128;
    v >>= 8;
    v -= w;
    v &= 0x1FF;
    gAirGrindPtr->unk306[gAirGrindPtr->unk304++] = (b & 0xC000) | v | (gOamAffineCount << 9);
    gAirGrindPtr->unk306[gAirGrindPtr->unk304++] = 0;
    src++;
    gAirGrindPtr->unk306[gAirGrindPtr->unk304++] = *src++ & 0xF3FF;
    if (!(a & 0x1000))
        goto loop;
    gOamBuffer[(s16)gOamAffineCount * 16 + 3] = half;
    gOamBuffer[(s16)gOamAffineCount * 16 + 7] = 0;
    gOamBuffer[(s16)gOamAffineCount * 16 + 11] = 0;
    gOamBuffer[(s16)gOamAffineCount * 16 + 15] = half;
    gOamAffineCount++;
    return ret;
}

void AirGrindClearScript(void)
{
    gAirGrindScript.scriptId = 0;
    gAirGrindScript.unk2 = 0;
    gAirGrindScript.unk4 = 0;
    gAirGrindScript.step = 0;
}

void AirGrindStepScript(void)
{
    u16 *p;

    if (gAirGrindScript.scriptId != 0) {
        p = gUnk_087572EC[gAirGrindScript.scriptId];
        switch (p[gAirGrindScript.step * 2]) {
        case 0x8000:
            gAirGrindScript.scriptId = 0;
            gAirGrindScript.unk2 = 0;
            gAirGrindScript.unk4 = 0;
            gAirGrindScript.step = 0;
            break;
        case 0x9999:
            gAirGrindScript.step = 0;
        default:
            gAirGrindScript.unk2 = p[gAirGrindScript.step * 2];
            gAirGrindScript.unk4 = p[gAirGrindScript.step * 2 + 1];
            gAirGrindScript.step++;
            break;
        }
    }
}

void AirGrindStartScript(u16 id)
{
    if (id <= 4) {
        if (id == 0) {
            gAirGrindScript.scriptId = id;
            gAirGrindScript.unk2 = id;
            gAirGrindScript.unk4 = id;
            gAirGrindScript.step = 0;
        } else if ((u8)(gAirGrindScript.scriptId - 1) > 3 || gAirGrindScript.scriptId <= id) {
            gAirGrindScript.scriptId = id;
            gAirGrindScript.unk2 = 0;
            gAirGrindScript.unk4 = 0;
            gAirGrindScript.step = 0;
        }
    }
}
