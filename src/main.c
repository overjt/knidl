#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "mode.h"
#include "menu.h"
#include "cutscene.h"
#include "room.h"
#include "effect.h"
#include "save.h"
#include "subgame.h"
#include "ending.h"

/* AgbMain (0x08007300-0x080075B7, issue #33): the game's main loop, entered
 * from crt0 (`Start`) via `bx 0x08007301` after AgbInit (rom-map.md §4).  An
 * infinite dispatch loop over a u16 game-state variable (gGameState) with
 * 23 states; the jump table lives at 0x08007328 inside the function range.
 * Not named `main`: gcc 2.9 inserts a `bl __gccmain` into any function with
 * that literal name, and the ROM has none (lessons-learned.md §3.13).
 *
 * Case bodies appear in source order (this order reproduces the ROM layout);
 * the compiler cross-jumps identical tails (e.g. the "state = 3" stores of
 * cases 1/3 and the "state = 5" tails of cases 10/21). */

void AgbMain(void)
{
    s32 i;

    gGameState = 0;
    InitSaveSlots();
    while (1) {
        switch (gGameState) {
        case 0:
            sub_0800b44c();
            gGameState = 1;
            break;
        case 1:
            if (gWarmBoot == 0)
                BootLogoMain();
            gGameState = 3;
            break;
        case 3:
            TitleMain();
            gGameState = 4;
            gPrevGameState = 3;
            break;
        case 4:
            MainMenuMain();
            ResetScoresAndMaxHealth();
            ResetPlayerRecords();
            break;
        case 7:
            if (gCutscenePending != 0)
                CutsceneMain();
            gCutscenePending = 0;
            gGameState = gPrevGameState;
            break;
        case 5:
            if (gCutscenePending != 0) {
                gPrevGameState = 5;
                gGameState = 7;
            } else {
                sub_0800b5dc();
                while (gGameState == 5) {
                    sub_0800b5dc();
                    sub_08007624();
                }
            }
            break;
        case 6:
            if (gCutscenePending != 0) {
                gPrevGameState = 6;
                gGameState = 7;
            } else {
                sub_0800b628();
                gGameState = 8;
            }
            break;
        case 8:
            while (gGameState == 8)
                sub_0800791c();
            gPrevGameState = 8;
            break;
        case 9:
            while (gGameState == 9)
                sub_0800783c();
            break;
        case 10:
            if (gUnk_03001F30 == 0)
                GoalGameMain();
            else
                sub_0800b628();
            gGameState = 5;
            break;
        case 13:
            sub_08007f9c();
            break;
        case 19:
            while (gGameState == 19)
                sub_0800791c();
            break;
        case 18:
            while (gGameState == 18)
                sub_0800791c();
            break;
        case 17:
            while (gGameState == 17)
                sub_0800791c();
            break;
        case 22:
            GameOverMain();
            break;
        case 14:
        case 15:
        case 16:
            SubGameMain();
            break;
        case 20:
            for (i = 0; i < 4; i++) {
                gPlayerLives[i] = 1;
                gPlayerHealth[i] = 0;
            }
            ResetPlayTime();
            sub_08022f50();
            while (gGameState == 20)
                sub_08008a00();
            gPrevGameState = 20;
            break;
        case 21:
            ResetScoresAndMaxHealth();
            gCutscenePending = 1;
            gGameState = 5;
            break;
        case 11:
            EndingMain();
            gGameState = 12;
            break;
        case 12:
            if (gUnk_03001F30 != 1 && gPrevGameState != 20)
                CreditsMain();
            FinalResultsScreen();
            gGameState = 0;
            break;
        case 2:
            gGameState = 3;
            break;
        }
    }
}
