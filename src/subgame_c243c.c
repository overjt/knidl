#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "subgame.h"

/* subgame_c243c.c (0x080C243C-0x080C2FF7, issue #98).
 *
 * Sub-game 2: the results screen (phase 1, gAirGrindPhases entry 1) and the
 * race screen's sky.
 * 
 *   AirGrindResults   the results task body: ranks the four racers by their
 *       frame counts M37Game.raceTimes[] (a bubble sort into gAirGrindResults.unk04,
 *       ties share a place in unk0C), picks each one's score (course record
 *       unk20 or unk24) and installs AirGrindResultsDraw / AirGrindResultsUpdate.
 *   AirGrindResultsDraw   draws the table: time (AirGrindDrawTime), score
 *       (AirGrindDrawRatio), the racer's sprite (AirGrindDrawRacerSprite, the winner pulsing)
 *       and the place or the stars won, then the two palette cycles
 *       (AirGrindResultsDrawCursor).
 *   AirGrindResultsStep   the results state machine (gAirGrindResults.unk00): a place
 *       jingle (songs 0x81D/0x81C/0x817), in the extra mode (gPrevGameState ==
 *       5) the stars counted up and handed to the linked players
 *       (AddPlayerLivesNoHud) before M35's SubGameQuit ends the screen, otherwise a
 *       retry/quit choice and a three-way level choice passed to M35's
 *       SubGameReplay.
 *   AirGrindResultsUpdate / AirGrindResultsDrawCursor / AirGrindResultsSetCursorBlend   the per-frame callback, the
 *       two palette cycles through gUnk_08609F40 with the cursor sprite, and
 *       the cycle's start row for a menu choice.
 *   AirGrindBuildSky   the per-frame hook AirGrindSetupRace installs: builds the
 *       160-line sky gradient M37Game.skyLineColors[] from eleven RGB key colours,
 *       fading from gUnk_080CFE60 to gUnk_080CFEA2 (level 2: gUnk_080CFE81 to
 *       gUnk_080CFEC3) between frames 0x4B0 and 0x8B0 of M37Game.frameCount.  It
 *       reads one key past the end of the eleven-entry local table, as the
 *       ROM does.
 *   AirGrindSkyVBlankCallback   the VBlank hook: sets backdrop colour 0 and re-arms DMA0
 *       to copy that gradient into it on every HBlank (0xA2400001). */

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);  /* sprite draw; callers pass f sign-extended (lsls/asrs #16), the early_1518 definition says u16 */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
void TaskSleepForever(void);                                     /* end the running task */
void AirGrindDrawTime(s32 t, s32 x, s32 y);                   /* draw a frame count as ss:cc */
void AirGrindDrawRatio(s32 a, s32 b, s32 x, s32 y);            /* draw min(a * 1000 / b, 1000) */

void AirGrindResults(void)
{
    s32 x;
    s32 i;
    s32 j;
    u8 t;

    gAirGrindResults.unk00 = 0;
    gAirGrindResults.unk01 = 0;
    gAirGrindResults.unk02 = 0;
    gCurTask->updateCallback = (u32)AirGrindResultsUpdate;
    gCurTask->frame = 0xFFFF;
    gCurTask->drawCallback = (u32)AirGrindResultsDraw;
    x = gAirGrindCoursePtr->unk014 * 2;
    for (i = 0; i < 4; i++) {
        if (gAirGrindPtr->players[i].unk01 == x && gAirGrindPtr->players[i].unk0E == x + 1)
            gAirGrindResults.unk1C[i] = gAirGrindCoursePtr->players[i].unk20;
        else
            gAirGrindResults.unk1C[i] = gAirGrindCoursePtr->players[i].unk24;
    }
    for (i = 0; i < 4; i++)
        gAirGrindResults.unk04[gUnk_080CFE2C[gAirGrindPtr->localPlayer][i]] = i;
    for (j = 3; j > 0; j--) {
        for (i = 0; i < j; i++) {
            if (gAirGrindPtr->raceTimes[gAirGrindResults.unk04[i]] > gAirGrindPtr->raceTimes[gAirGrindResults.unk04[i + 1]]) {
                t = gAirGrindResults.unk04[i];
                gAirGrindResults.unk04[i] = gAirGrindResults.unk04[i + 1];
                gAirGrindResults.unk04[i + 1] = t;
            }
        }
    }
    for (i = 0; i < 4; i++)
        gAirGrindResults.unk08[gAirGrindResults.unk04[i]] = i;
    j = 0;
    for (i = 0; i < 3; i++) {
        if (gAirGrindPtr->raceTimes[gAirGrindResults.unk04[i]] < gAirGrindPtr->raceTimes[gAirGrindResults.unk04[i + 1]]) {
            gAirGrindResults.unk0C[i] = j;
            j = i + 1;
            gAirGrindResults.unk0C[i + 1] = j;
        } else {
            gAirGrindResults.unk0C[i] = gAirGrindResults.unk0C[i + 1] = j;
        }
    }
    gAirGrindResults.unk10 = 0x200;
    gAirGrindResults.unk14 = -4;
    TaskSleepForever();
}

void AirGrindResultsDraw(void)
{
    s32 i;
    s32 y;
    u8 p;
    s32 scale;
    u16 blink;
    s32 pal;
    s32 layer;

    for (i = 0; i < 4; i++) {
        y = i * 32 + 43;
        p = gAirGrindResults.unk04[i];
        scale = 0x200;
        if (p == 0)
            scale = gAirGrindResults.unk10;
        AirGrindSetDigitPalette(8);
        AirGrindDrawTime(gAirGrindPtr->raceTimes[p], 114, y);
        AirGrindDrawRatio(gAirGrindResults.unk1C[p], gAirGrindCoursePtr->players[p].unk20, 180, y);
        pal = gUnk_080CFE2C[gAirGrindPtr->localPlayer][p];
        layer = 4;
        if (p == 0)
            layer = 3;
        AirGrindDrawRacerSprite(0, pal, scale, 76, y, layer);
        if (gAirGrindResults.unk00 != 3) {
            if (gAirGrindResults.unk00 > 3 && gPrevGameState == 5)
                blink = gAirGrindFrame & 64;
            else
                blink = 1;
            if (blink)
                goto place;
        }
        if (p != 0) {
        place:
            AirGrindDrawSymbol(gAirGrindResults.unk0C[i] + 2, 28, y);
        } else if (gAirGrindResults.unk01 != 0 || gAirGrindResults.unk02 != 0) {
            switch (gAirGrindResults.unk01) {
            case 1:
                AirGrindDrawSymbol(0, 28, y);
                break;
            case 2:
                AirGrindDrawSymbol(0, 32, y);
                AirGrindDrawSymbol(0, 24, y);
                break;
            case 3:
                AirGrindDrawSymbol(0, 36, y);
                AirGrindDrawSymbol(0, 28, y);
                AirGrindDrawSymbol(0, 20, y);
                break;
            }
        } else {
            AirGrindDrawSymbol(1, 28, y);
        }
    }
    AirGrindResultsDrawCursor();
}

void AirGrindResultsStep(void)
{
    struct Task *t;
    s32 i;
    u8 pal;

    gAirGrindResults.unk10 += gAirGrindResults.unk14;
    if (gAirGrindResults.unk10 <= 384)
        gAirGrindResults.unk14 = 4;
    if (gAirGrindResults.unk10 > 507)
        gAirGrindResults.unk14 = -4;
    switch (gAirGrindResults.unk00) {
    case 0:
        gAirGrindResults.unk18 = 0;
        gAirGrindResults.unk00++;
        break;
    case 1:
        if (++gAirGrindResults.unk18 > 10) {
            switch (gAirGrindResults.unk0C[gAirGrindResults.unk08[0]]) {
            case 0:
                PlayBgm(0x81D);
                break;
            case 1:
            case 2:
                PlayBgm(0x81C);
                break;
            case 3:
                PlayBgm(0x817);
                break;
            }
            gAirGrindResults.unk18 = 0;
            gAirGrindResults.unk00++;
        }
        break;
    case 2:
        if (++gAirGrindResults.unk18 > 174) {
            if (gPrevGameState != 5) {
                gAirGrindResults.unk00 = 4;
            } else {
                gAirGrindResults.unk18 = 20;
                gAirGrindResults.unk02 = 3 - gAirGrindResults.unk0C[gAirGrindResults.unk08[0]];
                gAirGrindResults.unk03 = 4;
                gAirGrindResults.unk00++;
            }
        }
        break;
    case 3:
        if (gAirGrindResults.unk03 > 0) {
            if (++gAirGrindResults.unk18 > 20) {
                gAirGrindResults.unk18 = 0;
                gAirGrindResults.unk03--;
                if (gAirGrindResults.unk02 > 0) {
                    gAirGrindResults.unk02--;
                    gAirGrindResults.unk01++;
                    PlaySfx(220);
                }
            }
        } else {
            gAirGrindResults.unk00++;
        }
        break;
    case 4:
        if (gPlayerPressedKeys[0] & 9) {
            if (gPrevGameState != 5) {
                t = gCurTask;
                t->unk28 = 0;
                t->unk2C = gAirGrind.level;
                if (gAirGrindPtr->localPlayer == 0) {
                    t->pixelX = 120;
                    t->pixelY = 80;
                    t->frameTable = gUnk_0875602C;
                    t->frame = 1;
                    t->spriteFlags = 0x2000;
                    t->layer = 1;
                    AirGrindResultsSetCursorBlend(-1);
                    PlaySfx(102);
                }
                gAirGrindResults.unk00++;
            } else {
                for (i = 0; i < 4; i++) {
                    pal = gUnk_080CFE2C[gAirGrindPtr->localPlayer][i];
                    if (pal < gAirGrindPtr->playerCount) {
                        switch (gAirGrindResults.unk0C[gAirGrindResults.unk08[i]]) {
                        case 0:
                            AddPlayerLivesNoHud(3, pal);
                            break;
                        case 1:
                            AddPlayerLivesNoHud(2, pal);
                            break;
                        case 2:
                            AddPlayerLivesNoHud(1, pal);
                            break;
                        }
                    }
                }
                gAirGrindResults.unk00 = 99;
                SubGameQuit();
            }
        }
        break;
    case 5:
        if (gPlayerPressedKeys[0] & 64) {
            if (--gCurTask->unk28 < 0)
                gCurTask->unk28 = 1;
            if (gAirGrindPtr->localPlayer == 0) {
                AirGrindResultsSetCursorBlend(gCurTask->unk28);
                PlaySfx(101);
            }
        }
        if (gPlayerPressedKeys[0] & 128) {
            if (++gCurTask->unk28 > 1)
                gCurTask->unk28 = 0;
            if (gAirGrindPtr->localPlayer == 0) {
                AirGrindResultsSetCursorBlend(gCurTask->unk28);
                PlaySfx(101);
            }
        }
        if (gPlayerPressedKeys[0] & 9) {
            if (gCurTask->unk28 == 0) {
                gCurTask->unk2C = gAirGrind.level;
                if (gAirGrindPtr->localPlayer == 0) {
                    gCurTask->frame = 0;
                    AirGrindResultsSetCursorBlend(gCurTask->unk2C);
                }
                gAirGrindResults.unk00++;
            } else {
                SubGameQuit();
                gAirGrindResults.unk00 = 99;
            }
            if (gAirGrindPtr->localPlayer == 0)
                PlaySfx(102);
        }
        break;
    case 6:
        if (gPlayerPressedKeys[0] & 64) {
            if (--gCurTask->unk2C < 0)
                gCurTask->unk2C = 2;
            if (gAirGrindPtr->localPlayer == 0) {
                AirGrindResultsSetCursorBlend(gCurTask->unk2C);
                PlaySfx(101);
            }
        }
        if (gPlayerPressedKeys[0] & 128) {
            if (++gCurTask->unk2C > 2)
                gCurTask->unk2C = 0;
            if (gAirGrindPtr->localPlayer == 0) {
                AirGrindResultsSetCursorBlend(gCurTask->unk2C);
                PlaySfx(101);
            }
        }
        if (gPlayerPressedKeys[0] & 9) {
            SubGameReplay(gCurTask->unk2C);
            gAirGrindResults.unk00 = 99;
            if (gAirGrindPtr->localPlayer == 0)
                PlaySfx(102);
        } else if (gPlayerPressedKeys[0] & 2) {
            if (gAirGrindPtr->localPlayer == 0) {
                gCurTask->frame = 1;
                AirGrindResultsSetCursorBlend(gCurTask->unk28);
                PlaySfx(215);
            }
            gAirGrindResults.unk00--;
        }
        break;
    }
}

void AirGrindResultsUpdate(void)
{
    AirGrindResultsStep();
    gAirGrindFrame++;
    SubGameCheckEnd();
}

void AirGrindResultsDrawCursor(void)
{
    struct Task *t;
    u32 *tbl;
    s32 from;
    s32 to;
    s32 step;
    s32 step2;

    gCurTask->unk6C--;
    if ((s16)gCurTask->unk6C < 0)
        gCurTask->unk6C = 8;
    from = gCurTask->unk6E;
    to = from + 1;
    step = 8 - (s16)gCurTask->unk6C;
    BlendColors(gUnk_08609F40[from], gUnk_08609F40[to], (u16)(step * 32), 16, gObjPaletteBank5);
    gCurTask->unk70--;
    if ((s16)gCurTask->unk70 < 0)
        gCurTask->unk70 = 8;
    step2 = 8 - (s16)gCurTask->unk70;
    BlendColors(gUnk_08609F40[6], gUnk_08609F40[7], (u16)(step2 * 32), 16, &gObjPaletteBank5[16]);
    if (gAirGrindPtr->localPlayer == 0) {
        t = gCurTask;
        tbl = t->frameTable;
        if (tbl != NULL && t->frame != -1) {
            QueueSprite(t->layer, tbl[t->frame + 2], t->spriteFlags, 0x6000, t->pixelX, t->pixelY);
            t = gCurTask;
            QueueSprite(t->layer, tbl[t->frame], t->spriteFlags, 0x5000, t->pixelX, t->pixelY);
        }
    }
}

void AirGrindResultsSetCursorBlend(s32 mode)
{
    switch (mode) {
    case 0:
        gCurTask->unk6E = 0;
        break;
    case 1:
        gCurTask->unk6E = 2;
        break;
    case 2:
        gCurTask->unk6E = 4;
        break;
    default:
        gCurTask->unk6E = 0;
        gCurTask->unk70 = 8;
        break;
    }
    gCurTask->unk6C = 8;
}

void AirGrindBuildSky(void)
{
    s32 col[11][3];
    s32 delta[11][3];
    u32 t;
    s32 i;
    u32 r;
    u32 g;
    u32 b;

    if (gAirGrindPtr->frameCount <= 0x4AF)
        t = 0;
    else if (gAirGrindPtr->frameCount <= 0x8AF)
        t = (gAirGrindPtr->frameCount - 0x4B0) / 4;
    else
        t = 0x100;
    if (gAirGrind.level != 2) {
        for (i = 0; i < 11; i++) {
            col[i][0] = t * (gUnk_080CFEA2[i][0] - gUnk_080CFE60[i][0]) + (gUnk_080CFE60[i][0] << 8);
            col[i][1] = t * (gUnk_080CFEA2[i][1] - gUnk_080CFE60[i][1]) + (gUnk_080CFE60[i][1] << 8);
            col[i][2] = t * (gUnk_080CFEA2[i][2] - gUnk_080CFE60[i][2]) + (gUnk_080CFE60[i][2] << 8);
        }
    } else {
        for (i = 0; i < 11; i++) {
            col[i][0] = t * (gUnk_080CFEC3[i][0] - gUnk_080CFE81[i][0]) + (gUnk_080CFE81[i][0] << 8);
            col[i][1] = t * (gUnk_080CFEC3[i][1] - gUnk_080CFE81[i][1]) + (gUnk_080CFE81[i][1] << 8);
            col[i][2] = t * (gUnk_080CFEC3[i][2] - gUnk_080CFE81[i][2]) + (gUnk_080CFE81[i][2] << 8);
        }
    }
    gAirGrindPtr->backdropColor = 0x7DC1;
    for (i = 0; i <= 10; i++) {
        delta[i][0] = (col[i + 1][0] - col[i][0]) / 16;
        delta[i][1] = (col[i + 1][1] - col[i][1]) / 16;
        delta[i][2] = (col[i + 1][2] - col[i][2]) / 16;
    }
    r = col[0][0];
    g = col[0][1];
    b = col[0][2];
    for (i = 0; i < 160; i++) {
        gAirGrindPtr->skyLineColors[i] = ((b & 0xF800) >> 1) | ((g & 0xF800) >> 6) | ((r & 0xF800) >> 11);
        r += delta[i / 16][0];
        g += delta[i / 16][1];
        b += delta[i / 16][2];
    }
    gAirGrindPtr->frameCount++;
}

void AirGrindSkyVBlankCallback(void)
{
    vu32 *dma = (vu32 *)REG_ADDR_DMA0;

    REG_DMA0CNT_H = 0;
    *(vu16 *)BG_PLTT = gAirGrindPtr->backdropColor;
    dma[0] = (u32)gAirGrindPtr->skyLineColors;
    dma[1] = BG_PLTT;
    dma[2] = 0xA2400001;
}
