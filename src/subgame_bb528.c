/* game_code_and_rodata_080653ec_0806ef5c 0x080BB528-0x080BC0CC
 * (issue #95, module M35, file 3 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BB528 0x080BC0CC src/subgame_bb528.c --newpb
 *
 * The duel's results screen (sub-game 0, screen 1).  QuickDrawResults is its
 * task: sub_080bb528 builds the screen, then seven <entry, check> states
 * run through the anchor tables 0x08756378 / 0x08756394 (QuickDrawResultsEnterState /
 * QuickDrawResultsUpdate):
 *
 *   0  wait, play song Task.unk2C | 0x800, then go to state Task.unk30
 *   1  spawn a kind-7 object (sub_080bb718) when gPrevGameState == 5
 *   2  count the markers in (sub_080bb760 / sub_080bb7a0)
 *   3  place the per-player markers (sub_080bb8f8 -> sub_080bb874)
 *   4  two-option cursor (sub_080bb554, sub_080bb5b8): option 0 goes to
 *      state 5, option 1 quits (SubGameQuit)
 *   5  three-option cursor (sub_080bb63c, sub_080bb66c): the choice goes to
 *      SubGameReplay, i.e. into gSubGameLevel, the row QuickDrawWaitForSignal picks
 *      the signal delay from
 *   6  idle until A/Start, then quit (SubGameQuit)
 *
 * sub_080bb718 / sub_080bb7cc / sub_080bb820 spawn task type #94 objects of
 * kinds 7, 8 and 9; sub_080bbc70 / sub_080bbcdc / sub_080bbd4c (kinds 1
 * and 0, the second one the reaction-time readout of gQuickDrawBestTime) and the
 * single-player placers sub_080bbbb8 / sub_080bbc04 fill the screen.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s32 gCurTaskIdx;
extern s8 gSubGameLevel;
extern u8 gQuickDrawBestTime;
extern u8 gQuickDrawWins[];
extern u8 gUnk_0200B048;
extern u8 gQuickDrawRanking[];
extern vs16 gBrightness;
extern vu16 gPlayerPressedKeys[];
extern u16 gPrevGameState;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern u32 gUnk_08755A34[];
extern u32 gUnk_08755A88[];
extern u32 gUnk_08755BAC[];
extern s16 gUnk_0875636C[];
extern u32 gUnk_08756378[];
extern u32 gUnk_08756394[];

void TaskYieldTrampoline(u32 frames);
void CallTableEntry(u32 a, u32 b, u32 *c);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskDrawScreen(void);
void TaskSleepForever(void);
void TaskSetEntry(void *fn, u32 i);
void SubGameReplay(s32 a0);
void SubGameQuit(void);
u8 sub_080b9d68(void);
void SubGameCheckEnd(void);
void CreateQuickDrawPlayers(s32 a0);

void sub_080bbb70(void);
void sub_080bbc04(void);
void sub_080bba1c(void);
void sub_080bbbb8(void);
void sub_080bbc70(void);
void sub_080bbcdc(void);
void sub_080bbd4c(void);
void QuickDrawResultsUpdate(void);

void sub_080bb528(void)
{
    struct Task *t;

    if (gPlayerCount != 1)
        sub_080bbb70();
    else
        sub_080bbc04();
    t = gCurTask;
    t->unk24 = 40;
    t->state = 0;
}

void sub_080bb554(void)
{
    struct Task *t;

    gCurTask->frameTable = gUnk_08755BAC;
    gCurTask->layer = 4;
    t = gCurTask;
    t->tileWord |= 0x800;
    t->frame = 0;
    t->pixelX = 120;
    t->pixelY = 152;
    t->unk1C = 0;
}

void sub_080bb59c(s32 a0)
{
    if (gLocalPlayer == 0)
        PlaySfx(a0);
}

void sub_080bb5b8(void)
{
    struct Task *t;

    if (gPlayerPressedKeys[0] & 9)
    {
        sub_080bb59c(102);
        t = gCurTask;
        t->unk28 = 0;
        if (t->unk1C == 0)
            t->state = 5;
        else
            SubGameQuit();
    }
    else if (gPlayerPressedKeys[0] & 0x30)
    {
        if (gPlayerPressedKeys[0] & 0x20)
        {
            t = gCurTask;
            if (t->unk1C != 0)
            {
                t->unk1C = 0;
                t->frame = 0;
                sub_080bb59c(101);
            }
        }
        else
        {
            t = gCurTask;
            if (t->unk1C == 0)
            {
                t->unk1C = 1;
                t->frame = 1;
                sub_080bb59c(101);
            }
        }
    }
}

void sub_080bb63c(void)
{
    struct Task *t = gCurTask;
    s32 v;

    t->pixelX = 120;
    t->pixelY = 152;
    v = gSubGameLevel;
    t->unk1C = v;
    t->unk24 = 10;
    t->frame = v + 2;
}

void sub_080bb66c(void)
{
    struct Task *t;

    if (gPlayerPressedKeys[0] & 9)
    {
        sub_080bb59c(102);
        SubGameReplay(gCurTask->unk1C);
        gCurTask->unk28 = 0;
    }
    else if (gPlayerPressedKeys[0] & 2)
    {
        sub_080bb59c(215);
        gCurTask->state = 4;
        gCurTask->unk28 = 0;
    }
    else if (gPlayerPressedKeys[0] & 0x30)
    {
        if (gPlayerPressedKeys[0] & 0x20)
        {
            t = gCurTask;
            if (t->unk1C > 0)
            {
                t->unk1C--;
                sub_080bb59c(101);
            }
        }
        else
        {
            t = gCurTask;
            if (t->unk1C <= 1)
            {
                t->unk1C++;
                sub_080bb59c(101);
            }
        }
        gCurTask->frame = gCurTask->unk1C + 2;
    }
}

void sub_080bb718(u16 a0, u16 a1, u16 a2)
{
    s32 i = TaskCreateFrom(94, 32);

    if (i != -1)
    {
        struct Task *t = &gTasks[i];

        t->variant = 7;
        t->frame = a0;
        t->pixelX = a1;
        t->pixelY = a2;
    }
}

void sub_080bb760(void)
{
    if (gPlayerCount != 1)
        gCurTask->unk20 = 3;
    else
        gCurTask->unk20 = gUnk_0875636C[gUnk_0200B048];
}

void sub_080bb7a0(void)
{
    if (gPlayerCount != 1)
        sub_080bba1c();
    else
        sub_080bbbb8();
    gCurTask->unk20--;
}

void sub_080bb7cc(u8 a0, s16 a1, s16 a2, s8 a3)
{
    s32 i = TaskCreateFrom(94, 32);

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

void sub_080bb820(u8 a0, s16 a1, s16 a2, s8 a3)
{
    s32 i = TaskCreateFrom(94, 32);

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

void sub_080bb874(u8 a0, s8 a1)
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
        sub_080bb820(gUnk_0200B048 + 1, x, y, a1);
    }
    else
    {
        sub_080bb820(a0, x, y, a1);
        gUnk_0200B048 = a0;
    }
}

void sub_080bb8f8(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
        sub_080bb874(i, gQuickDrawRanking[i]);
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
        sub_080bb7cc(3, x, y, a1);
    else
        sub_080bb7cc(a0, x, y, a1);
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
    s16 x = t->pixelX + (2 - gCurTask->unk20) * 24;
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
    sub_080bb7cc(0, x, y, gQuickDrawRanking[0]);
    if (gCurTask->unk20 == 3)
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

void sub_080bbad4(void)
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
        gCurTask->unk2C = 29;
        break;
    case 1:
    case 2:
        gCurTask->unk2C = 28;
        break;
    case 3:
        gCurTask->unk2C = 23;
        break;
    }
}

void sub_080bbb70(void)
{
    if (gPrevGameState == 5)
        sub_080bb718(0, 120, 16);
    CreateQuickDrawPlayers(1);
    sub_080bbad4();
    if (gPrevGameState == 5)
        gCurTask->unk30 = 2;
    else
        gCurTask->unk30 = 3;
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
        y = (3 - gCurTask->unk20) * 18 + 144;
        break;
    }
    sub_080bb7cc(gUnk_0200B048, y, 64, 0);
}

void sub_080bbc04(void)
{
    sub_080bbc70();
    sub_080bbcdc();
    if (gPrevGameState != 5)
        sub_080bbd4c();
    switch (gUnk_0200B048)
    {
    case 5:
        gCurTask->unk2C = 29;
        break;
    case 1 ... 4:
        gCurTask->unk2C = 28;
        break;
    case 0:
        gCurTask->unk2C = 23;
        break;
    }
    gCurTask->unk30 = 1;
}

void sub_080bbc70(void)
{
    s32 id = TaskCreateFrom(94, 32);

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

void sub_080bbcdc(void)
{
    s32 id = TaskCreateFrom(94, 32);

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
        t->unk30 = (s32)gUnk_08755A34;
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

void sub_080bbd4c(void)
{
    s32 id = TaskCreateFrom(94, 0);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];

        t->parent = gCurTaskIdx;
        t->unk18 = id;
        t->variant = 0;
        t->unk74 = 2;
        t->unk28 = 0;
    }
}

void QuickDrawResults(void)
{
    struct Task *t;

    sub_080bb528();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    t = gCurTask;
    t->updateCallback = (u32)QuickDrawResultsUpdate;
    CallTableEntry(t->state, 7, gUnk_08756378);
    TaskSleepForever();
}

void QuickDrawResultsUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 7, gUnk_08756394);
    SubGameCheckEnd();
}

void QuickDrawResultsEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gUnk_08756378);
}

void sub_080bbe20(void)
{
    gCurTask->updateState = 0;
    TaskYieldTrampoline(30);
    PlayBgm(gCurTask->unk2C | 0x800);
    TaskYieldTrampoline(180);
    gCurTask->state = gCurTask->unk30;
    TaskSleepForever();
}

void sub_080bbe58(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void sub_080bbe80(void)
{
    gCurTask->updateState = 1;
    TaskYieldTrampoline(16);
    if (gPrevGameState == 5)
    {
        if (gUnk_0200B048 != 0)
        {
            sub_080bb718(0, 96, 64);
            gCurTask->state = 2;
        }
        else
        {
            sub_080bb718(1, 120, 64);
            gCurTask->state = 6;
        }
    }
    else
        gCurTask->state = 4;
    TaskSleepForever();
}

void sub_080bbedc(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void sub_080bbf04(void)
{
    gCurTask->updateState = 2;
    sub_080bb760();
    TaskYieldTrampoline(16);
    while (gCurTask->unk20 != 0)
    {
        sub_080bb7a0();
        TaskYieldTrampoline(20);
    }
    gCurTask->state = 6;
    TaskSleepForever();
}

void sub_080bbf44(void)
{
    if (gCurTask->state != 2)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void sub_080bbf6c(void)
{
    gCurTask->updateState = 3;
    TaskYieldTrampoline(16);
    sub_080bb8f8();
    TaskYieldTrampoline(20);
    gCurTask->state = 4;
    TaskSleepForever();
}

void sub_080bbf9c(void)
{
    if (gCurTask->state != 3)
        TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
}

void sub_080bbfc4(void)
{
    gCurTask->updateState = 4;
    gCurTask->unk28 = 0;
    TaskYieldTrampoline(gCurTask->unk24);
    if (gLocalPlayer == 0)
        gCurTask->drawCallback = (u32)TaskDrawScreen;
    sub_080bb554();
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_080bc008(void)
{
    if (gCurTask->unk28 != 0)
    {
        sub_080bb5b8();
        if (gCurTask->state != 4)
            TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
    }
}

void sub_080bc03c(void)
{
    gCurTask->updateState = 5;
    gCurTask->unk28 = 0;
    TaskYieldTrampoline(8);
    sub_080bb63c();
    gCurTask->unk28 = 1;
    TaskSleepForever();
}

void sub_080bc06c(void)
{
    if (gCurTask->unk28 != 0)
    {
        sub_080bb66c();
        if (gCurTask->state != 5)
            TaskSetEntry(QuickDrawResultsEnterState, gCurTaskIdx);
    }
}

void sub_080bc0a0(void)
{
    gCurTask->updateState = 6;
    TaskSleepForever();
}

void sub_080bc0b8(void)
{
    if (sub_080b9d68())
        SubGameQuit();
}
