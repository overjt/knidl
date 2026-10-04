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

    gGameState = GAME_STATE_RESET;
    InitSaveSlots();
    while (1) {
        switch (gGameState) {
        case GAME_STATE_RESET:
            sub_0800b44c();
            gGameState = GAME_STATE_BOOT_LOGO;
            break;
        case GAME_STATE_BOOT_LOGO:
            if (gWarmBoot == 0)
                BootLogoMain();
            gGameState = GAME_STATE_TITLE;
            break;
        case GAME_STATE_TITLE:
            TitleMain();
            gGameState = GAME_STATE_MAIN_MENU;
            gPrevGameState = GAME_STATE_TITLE;
            break;
        case GAME_STATE_MAIN_MENU:
            MainMenuMain();
            ResetScoresAndMaxHealth();
            ResetPlayerRecords();
            break;
        case GAME_STATE_CUTSCENE:
            if (gCutscenePending != 0)
                CutsceneMain();
            gCutscenePending = 0;
            gGameState = gPrevGameState;
            break;
        case GAME_STATE_HUB:
            if (gCutscenePending != 0) {
                gPrevGameState = GAME_STATE_HUB;
                gGameState = GAME_STATE_CUTSCENE;
            } else {
                sub_0800b5dc();
                while (gGameState == GAME_STATE_HUB) {
                    sub_0800b5dc();
                    HubMain();
                }
            }
            break;
        case GAME_STATE_STAGE_START:
            if (gCutscenePending != 0) {
                gPrevGameState = GAME_STATE_STAGE_START;
                gGameState = GAME_STATE_CUTSCENE;
            } else {
                sub_0800b628();
                gGameState = GAME_STATE_STAGE;
            }
            break;
        case GAME_STATE_STAGE:
            while (gGameState == GAME_STATE_STAGE)
                StageMain();
            gPrevGameState = GAME_STATE_STAGE;
            break;
        case GAME_STATE_BIG_SWITCH_VIEW:
            while (gGameState == GAME_STATE_BIG_SWITCH_VIEW)
                BigSwitchViewMain();
            break;
        case GAME_STATE_GOAL_GAME:
            if (gMetaKnightmareMode == 0)
                GoalGameMain();
            else
                sub_0800b628();
            gGameState = GAME_STATE_HUB;
            break;
        case GAME_STATE_EXTRA_MODE_TITLE:
            ExtraModeTitleMain();
            break;
        case GAME_STATE_ARENA:
            while (gGameState == GAME_STATE_ARENA)
                StageMain();
            break;
        case GAME_STATE_MUSEUM:
            while (gGameState == GAME_STATE_MUSEUM)
                StageMain();
            break;
        case GAME_STATE_WARP_STAR_STATION:
            while (gGameState == GAME_STATE_WARP_STAR_STATION)
                StageMain();
            break;
        case GAME_STATE_GAME_OVER:
            GameOverMain();
            break;
        case GAME_STATE_QUICK_DRAW:
        case GAME_STATE_BOMB_RALLY:
        case GAME_STATE_AIR_GRIND:
            SubGameMain();
            break;
        case GAME_STATE_BOSS_ENDURANCE:
            for (i = 0; i < 4; i++) {
                gPlayerLives[i] = 1;
                gPlayerHealth[i] = 0;
            }
            ResetPlayTime();
            sub_08022f50();
            while (gGameState == GAME_STATE_BOSS_ENDURANCE)
                BossEnduranceMain();
            gPrevGameState = GAME_STATE_BOSS_ENDURANCE;
            break;
        case GAME_STATE_META_KNIGHTMARE:
            ResetScoresAndMaxHealth();
            gCutscenePending = 1;
            gGameState = GAME_STATE_HUB;
            break;
        case GAME_STATE_ENDING:
            EndingMain();
            gGameState = GAME_STATE_CREDITS;
            break;
        case GAME_STATE_CREDITS:
            if (gMetaKnightmareMode != 1 && gPrevGameState != GAME_STATE_BOSS_ENDURANCE)
                CreditsMain();
            FinalResultsScreen();
            gGameState = GAME_STATE_RESET;
            break;
        case 2:
            gGameState = GAME_STATE_TITLE;
            break;
        }
    }
}
