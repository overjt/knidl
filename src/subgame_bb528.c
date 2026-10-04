/* game_code_and_rodata_080653ec_0806ef5c 0x080BB528-0x080BC0CC
 * (issue #95, module M35, file 3 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BB528 0x080BC0CC src/subgame_bb528.c --newpb
 *
 * The duel's results screen (sub-game 0, screen 1).  QuickDrawResults is its
 * task: QuickDrawSetupResults builds the screen, then seven <entry, check> states
 * run through the anchor tables 0x08756378 / 0x08756394 (QuickDrawResultsEnterState /
 * QuickDrawResultsUpdate):
 *
 *   0  wait, play song Task.unk2C | 0x800, then go to state Task.unk30
 *   1  spawn a kind-7 object (CreateQuickDrawBonusSign) when gPrevGameState == 5
 *   2  count the markers in (QuickDrawInitBonusSteps / QuickDrawCreateNextBonus)
 *   3  place the per-player markers (QuickDrawCreateRankLabels -> QuickDrawPlaceRankLabel)
 *   4  two-option cursor (QuickDrawInitContinueMenu, QuickDrawContinueMenuInput): option 0 goes to
 *      state 5, option 1 quits (SubGameQuit)
 *   5  three-option cursor (QuickDrawInitLevelMenu, QuickDrawLevelMenuInput): the choice goes to
 *      SubGameReplay, i.e. into gSubGameLevel, the row QuickDrawWaitForSignal picks
 *      the signal delay from
 *   6  idle until A/Start, then quit (SubGameQuit)
 *
 * CreateQuickDrawBonusSign / CreateQuickDrawBonus / CreateQuickDrawRankLabel spawn task type #94 objects of
 * kinds 7, 8 and 9; CreateQuickDrawDefeatedLabel / CreateQuickDrawBestTimeLabel / CreateQuickDrawResultsPlayer (kinds 1
 * and 0, the second one the reaction-time readout of gQuickDrawBestTime) and the
 * single-player placers sub_080bbbb8 / QuickDrawSetupResultsVsCpu fill the screen.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "room.h"
#include "subgame.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
void CallTableEntry(u32 a, u32 b, u32 *c);
s32 PlaySfx(s32 id);
void TaskSetEntry(void *fn, u32 i);

void QuickDrawSetupResults(void)
{
    struct Task *t;

    if (gPlayerCount != 1)
        QuickDrawSetupResultsLink();
    else
        QuickDrawSetupResultsVsCpu();
    t = gCurTask;
    t->quickDrawMenuDelay = 40;
    t->state = 0;
}

void QuickDrawInitContinueMenu(void)
{
    struct Task *t;

    gCurTask->frameTable = gQuickDrawResultsMenuFrames;
    gCurTask->layer = 4;
    t = gCurTask;
    t->tileWord |= 0x800;
    t->frame = 0;
    t->pixelX = 120;
    t->pixelY = 152;
    t->quickDrawMenuCursor = 0;
}

void QuickDrawPlaySfxIfPlayer0(s32 a0)
{
    if (gLocalPlayer == 0)
        PlaySfx(a0);
}

void QuickDrawContinueMenuInput(void)
{
    struct Task *t;

    if (gPlayerPressedKeys[0] & 9)
    {
        QuickDrawPlaySfxIfPlayer0(102);
        t = gCurTask;
        t->quickDrawMenuActive = 0;
        if (t->quickDrawMenuCursor == 0)
            t->state = 5;
        else
            SubGameQuit();
    }
    else if (gPlayerPressedKeys[0] & 0x30)
    {
        if (gPlayerPressedKeys[0] & 0x20)
        {
            t = gCurTask;
            if (t->quickDrawMenuCursor != 0)
            {
                t->quickDrawMenuCursor = 0;
                t->frame = 0;
                QuickDrawPlaySfxIfPlayer0(101);
            }
        }
        else
        {
            t = gCurTask;
            if (t->quickDrawMenuCursor == 0)
            {
                t->quickDrawMenuCursor = 1;
                t->frame = 1;
                QuickDrawPlaySfxIfPlayer0(101);
            }
        }
    }
}

void QuickDrawInitLevelMenu(void)
{
    struct Task *t = gCurTask;
    s32 v;

    t->pixelX = 120;
    t->pixelY = 152;
    v = gSubGameLevel;
    t->quickDrawMenuCursor = v;
    t->quickDrawMenuDelay = 10;
    t->frame = v + 2;
}

void QuickDrawLevelMenuInput(void)
{
    struct Task *t;

    if (gPlayerPressedKeys[0] & 9)
    {
        QuickDrawPlaySfxIfPlayer0(102);
        SubGameReplay(gCurTask->quickDrawMenuCursor);
        gCurTask->quickDrawMenuActive = 0;
    }
    else if (gPlayerPressedKeys[0] & 2)
    {
        QuickDrawPlaySfxIfPlayer0(215);
        gCurTask->state = 4;
        gCurTask->quickDrawMenuActive = 0;
    }
    else if (gPlayerPressedKeys[0] & 0x30)
    {
        if (gPlayerPressedKeys[0] & 0x20)
        {
            t = gCurTask;
            if (t->quickDrawMenuCursor > 0)
            {
                t->quickDrawMenuCursor--;
                QuickDrawPlaySfxIfPlayer0(101);
            }
        }
        else
        {
            t = gCurTask;
            if (t->quickDrawMenuCursor <= 1)
            {
                t->quickDrawMenuCursor++;
                QuickDrawPlaySfxIfPlayer0(101);
            }
        }
        gCurTask->frame = gCurTask->quickDrawMenuCursor + 2;
    }
}

void CreateQuickDrawBonusSign(u16 a0, u16 a1, u16 a2)
{
    s32 i = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (i != -1)
    {
        struct Task *t = &gTasks[i];

        t->variant = 7;
        t->frame = a0;
        t->pixelX = a1;
        t->pixelY = a2;
    }
}

void QuickDrawInitBonusSteps(void)
{
    if (gPlayerCount != 1)
        gCurTask->quickDrawBonusStepsLeft = 3;
    else
        gCurTask->quickDrawBonusStepsLeft = gQuickDrawDefeatBonusSteps[gUnk_0200B048];
}

void QuickDrawCreateNextBonus(void)
{
    if (gPlayerCount != 1)
        sub_080bba1c();
    else
        sub_080bbbb8();
    gCurTask->quickDrawBonusStepsLeft--;
}

void CreateQuickDrawBonus(u8 a0, s16 a1, s16 a2, s8 a3)
{
    s32 i = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (i != -1)
    {
        struct Task *t = &gTasks[i];

        t->variant = 8;
        t->unk18 = a0;
        t->pixelX = a1;
        t->pixelY = a2;
        t->unk1C = a3;
    }
}

void CreateQuickDrawRankLabel(u8 a0, s16 a1, s16 a2, s8 a3)
{
    s32 i = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (i != -1)
    {
        struct Task *t = &gTasks[i];

        t->pixelX = a1;
        t->pixelY = a2;
        t->variant = 9;
        t->unk18 = a0;
        t->unk1C = a3;
        t->frame = a0;
    }
}

void QuickDrawPlaceRankLabel(u8 a0, s8 a1)
{
    struct Task *t = &gTasks[a1];
    s16 x = t->pixelX - 24;
    s16 y = t->pixelY - 32;

    /* The ROM keeps a dead `ldrsh` of t->unk48: the switch sub_080bb930 has
       on unk4A, with every arm reduced to a no-op.  Merging the arms into
       one loses the load, so the four labels stay (the values are
       sub_080bb930's; the ROM cannot show them). */
    switch (t->pixelX)
    {
    case 120:
        x = x;
        break;
    case 56:
        x = x;
        break;
    case 104:
        x = x;
        break;
    case 72:
        x = x;
        break;
    }
    if (gQuickDrawWins[a1] == 0)
    {
        CreateQuickDrawRankLabel(gUnk_0200B048 + 1, x, y, a1);
    }
    else
    {
        CreateQuickDrawRankLabel(a0, x, y, a1);
        gUnk_0200B048 = a0;
    }
}

void QuickDrawCreateRankLabels(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
        QuickDrawPlaceRankLabel(i, gQuickDrawRanking[i]);
}

void sub_080bb930(u8 a0, s8 a1)
{
    struct Task *base = gTasks;
    struct Task *t = &base[a1];
    s16 x = t->pixelX;
    s16 y = t->pixelY;

    switch (t->pixelY)
    {
    case 120:
        y = 96;
        break;
    case 56:
        y = 40;
        break;
    case 104:
        y = 84;
        break;
    case 72:
        y = 52;
        break;
    }
    if (gQuickDrawWins[a1] == 0)
        CreateQuickDrawBonus(3, x, y, a1);
    else
        CreateQuickDrawBonus(a0, x, y, a1);
}

void sub_080bb9b4(void)
{
    sub_080bb930(2, gQuickDrawRanking[1]);
}

void sub_080bb9cc(void)
{
    sub_080bb930(1, gQuickDrawRanking[1]);
    sub_080bb930(2, gQuickDrawRanking[2]);
}

void sub_080bb9f0(void)
{
    sub_080bb930(1, gQuickDrawRanking[1]);
    sub_080bb930(2, gQuickDrawRanking[2]);
    sub_080bb930(3, gQuickDrawRanking[3]);
}

void sub_080bba1c(void)
{
    struct Task *base = gTasks;
    struct Task *t = &base[gQuickDrawRanking[0]];
    s16 x = t->pixelX + (2 - gCurTask->quickDrawBonusStepsLeft) * 24;
    s16 y = t->pixelY;

    switch (t->pixelY)
    {
    case 120:
        y = 96;
        break;
    case 56:
        y = 40;
        break;
    case 104:
        y = 84;
        break;
    case 72:
        y = 52;
        break;
    }
    CreateQuickDrawBonus(0, x, y, gQuickDrawRanking[0]);
    if (gCurTask->quickDrawBonusStepsLeft == 3)
    {
        switch (gPlayerCount)
        {
        case 4:
            sub_080bb9f0();
            break;
        case 3:
            sub_080bb9cc();
            break;
        case 2:
            sub_080bb9b4();
            break;
        }
    }
}

void QuickDrawPickResultsSong(void)
{
    s32 i = 0;
    s32 found = 0;

    for (; i < gPlayerCount; i++)
    {
        if (gQuickDrawRanking[i] == gLocalPlayer)
        {
            found = i;
            break;
        }
    }
    if ((gPlayerCount == 2 && found != 0) || gQuickDrawWins[gLocalPlayer] == 0)
        found = 3;
    switch (found)
    {
    case 0:
        gCurTask->quickDrawResultsSong = 29;
        break;
    case 1:
    case 2:
        gCurTask->quickDrawResultsSong = 28;
        break;
    case 3:
        gCurTask->quickDrawResultsSong = 23;
        break;
    }
}

void QuickDrawSetupResultsLink(void)
{
    if (gPrevGameState == GAME_STATE_HUB)
        CreateQuickDrawBonusSign(0, 120, 16);
    CreateQuickDrawPlayers(1);
    QuickDrawPickResultsSong();
    if (gPrevGameState == GAME_STATE_HUB)
        gCurTask->quickDrawResultsNextState = 2;
    else
        gCurTask->quickDrawResultsNextState = 3;
}

void sub_080bbbb8(void)
{
    u16 y;

    switch (gUnk_0200B048)
    {
    default:
        y = 152;
        break;
    case 4:
        y = 144;
        break;
    case 5:
        y = (3 - gCurTask->quickDrawBonusStepsLeft) * 18 + 144;
        break;
    }
    CreateQuickDrawBonus(gUnk_0200B048, y, 64, 0);
}

void QuickDrawSetupResultsVsCpu(void)
{
    CreateQuickDrawDefeatedLabel();
    CreateQuickDrawBestTimeLabel();
    if (gPrevGameState != GAME_STATE_HUB)
        CreateQuickDrawResultsPlayer();
    switch (gUnk_0200B048)
    {
    case 5:
        gCurTask->quickDrawResultsSong = 29;
        break;
    case 1 ... 4:
        gCurTask->quickDrawResultsSong = 28;
        break;
    case 0:
        gCurTask->quickDrawResultsSong = 23;
        break;
    }
    gCurTask->quickDrawResultsNextState = 1;
}

void CreateQuickDrawDefeatedLabel(void)
{
    s32 id = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];

        t->variant = 1;
        t->unk18 = gUnk_0200B048;
        t->unk1C = 4;
        t->unk20 = -1;
        t->unk24 = 6;
        t->pixelX = 80;
        t->pixelY = 16;
        t->unk28 = -12;
        t->unk2C = 80;
        t->unk30 = (s32)gUnk_08755A88;
        t->unk34 = 0;
    }
}

void CreateQuickDrawBestTimeLabel(void)
{
    s32 id = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 32);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];
        s32 v;
        s32 n;

        t->variant = 1;
        t->unk1C = 5;
        t->unk20 = -1;
        t->pixelX = 172;
        t->pixelY = 96;
        t->unk28 = -8;
        t->unk2C = 0;
        t->unk30 = (s32)gQuickDrawDigitFrames;
        t->unk34 = 0;
        v = gQuickDrawBestTime;
        n = 0;
        while (v > 9)
        {
            v -= 10;
            n++;
        }
        t->unk24 = n;
        t->unk18 = v;
    }
}

void CreateQuickDrawResultsPlayer(void)
{
    s32 id = TaskCreateFrom(TASK_QUICK_DRAW_OBJECT, 0);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];

        t->parent = gCurTaskIdx;
        t->quickDrawObjectPlayerIndex = id;
        t->variant = 0;
        t->quickDrawObjectStartMode = 2;
        t->unk28 = 0;
    }
}

void QuickDrawResults(void)
{
    struct Task *t;

    QuickDrawSetupResults();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    t = gCurTask;
    t->updateCallback = (u32)QuickDrawResultsUpdate;
    CallTableEntry(t->state, 7, gQuickDrawResultsStates);
    TaskSleepForever();
}

void QuickDrawResultsUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 7, gQuickDrawResultsStateUpdates);
    SubGameCheckEnd();
}

void QuickDrawResultsEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gQuickDrawResultsStates);
}

void QuickDrawResultsPlaySong(void)
{
    gCurTask->updateState = 0;
    TaskYieldTrampoline(30);
    PlayBgm(gCurTask->quickDrawResultsSong | 0x800);
    TaskYieldTrampoline(180);
    gCurTask->state = gCurTask->quickDrawResultsNextState;
    TaskSleepForever();
}

void QuickDrawResultsPlaySongUpdate(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void QuickDrawResultsBonusSign(void)
{
    gCurTask->updateState = 1;
    TaskYieldTrampoline(16);
    if (gPrevGameState == GAME_STATE_HUB)
    {
        if (gUnk_0200B048 != 0)
        {
            CreateQuickDrawBonusSign(0, 96, 64);
            gCurTask->state = 2;
        }
        else
        {
            CreateQuickDrawBonusSign(1, 120, 64);
            gCurTask->state = 6;
        }
    }
    else
        gCurTask->state = 4;
    TaskSleepForever();
}

void QuickDrawResultsBonusSignUpdate(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void QuickDrawResultsAwardBonuses(void)
{
    gCurTask->updateState = 2;
    QuickDrawInitBonusSteps();
    TaskYieldTrampoline(16);
    while (gCurTask->quickDrawBonusStepsLeft != 0)
    {
        QuickDrawCreateNextBonus();
        TaskYieldTrampoline(20);
    }
    gCurTask->state = 6;
    TaskSleepForever();
}

void QuickDrawResultsAwardBonusesUpdate(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void QuickDrawResultsRanking(void)
{
    gCurTask->updateState = 3;
    TaskYieldTrampoline(16);
    QuickDrawCreateRankLabels();
    TaskYieldTrampoline(20);
    gCurTask->state = 4;
    TaskSleepForever();
}

void QuickDrawResultsRankingUpdate(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void QuickDrawResultsContinueMenu(void)
{
    gCurTask->updateState = 4;
    gCurTask->quickDrawMenuActive = 0;
    TaskYieldTrampoline(gCurTask->quickDrawMenuDelay);
    if (gLocalPlayer == 0)
        gCurTask->drawCallback = (u32)TaskDrawScreen;
    QuickDrawInitContinueMenu();
    gCurTask->quickDrawMenuActive = 1;
    TaskSleepForever();
}

void QuickDrawResultsContinueMenuUpdate(void)
{
    if (gCurTask->quickDrawMenuActive != 0)
    {
        QuickDrawContinueMenuInput();
        if (gCurTask->state != 4)
            TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
    }
}

void QuickDrawResultsLevelMenu(void)
{
    gCurTask->updateState = 5;
    gCurTask->quickDrawMenuActive = 0;
    TaskYieldTrampoline(8);
    QuickDrawInitLevelMenu();
    gCurTask->quickDrawMenuActive = 1;
    TaskSleepForever();
}

void QuickDrawResultsLevelMenuUpdate(void)
{
    if (gCurTask->quickDrawMenuActive != 0)
    {
        QuickDrawLevelMenuInput();
        if (gCurTask->state != 5)
            TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
    }
}

void QuickDrawResultsWaitQuit(void)
{
    gCurTask->updateState = 6;
    TaskSleepForever();
}

void QuickDrawResultsWaitQuitUpdate(void)
{
    if (SubGameAnyPressedAOrStart())
        SubGameQuit();
}
