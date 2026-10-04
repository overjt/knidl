/* game_code_and_rodata_080653ec_0806ef5c 0x080BDA2C-0x080BF994
 * (issue #66, module M36 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BDA2C 0x080BF994 src/subgame_bda2c.c --newpb
 *
 * Head of M36, the sub-game task bank the game-mode flow module (M35)
 * starts as task type #95.  The 41-entry anchor table at 0x08756668 that
 * CallTableEntry dispatches on Task.state/unk15 lives here, and every state
 * below is one of its entries.
 *
 * The mode is a four-slot round-robin: Task.unk34 is the slot 0-3 that the
 * turn walks forwards or backwards (Task.unk28 <= 2 selects the direction),
 * Task.unk2C is the speed level 0-6 that indexes the per-level beat length
 * at gBombRallyBeatFrames, and Task.unk1C counts the beats until the level steps
 * up (the per-player-count limits are the three parallel byte tables at
 * 0x0875665C/0x0875665F/0x08756662, indexed by the player count in
 * gSubGameLevel).
 *
 *   BombRallyRound   entry: install the dispatchers and kick BGM 0x82E
 *   BombRallyRoundPass   the beat loop (states 0..6 in Task.unk28)
 *   BombRallyKnockOutTurnPlayer   knock out the player in the turn seat and test for the end of the match
 *   CreateBombRallyBombSmoke   / CreateBombRallyStarBurst / CreateBombRallyBurstStar   sprite placement helpers
 *   BombRallySeatPlayers   seat the players at random (local player in seat 0) and pick the first turn
 *   BombRallyResultsShow   the vblank-flag wait (goto/do-while shape, lesson 6)
 *   BombRallyPlayerJudgePress   the button-timing judgement against the 5-byte records
 *                  at 0x087565F4 (thresholds -> Task.unk20 = 2/1/0)
 *   BombRallyPlayerCpuThrow   the per-turn state advance: RNG over gUnk_087565E0,
 *                  the animation pick from gUnk_087565F4, the SE and the
 *                  eight TaskYield steps that fan the position out.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "room.h"
#include "subgame.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void TaskSetEntry(void *a, u32 i);
extern void PlaySfx(u32 a);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern u32 RandomRange(u32 range);

void BombRallyRound(void)
{
    gCurTask->updateCallback = (u32)BombRallyRoundUpdate;
    BombRallyInitSpeed();
    BombRallySeatPlayers();
    gCurTask->bombRallyThrow = -3;
    PlayBgm(0x82E);
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 2, gBombRallyStates);
    TaskSleepForever();
}

void BombRallyRoundUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 2, gBombRallyStateUpdates);
    SubGameCheckEnd();
}

void BombRallyEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gBombRallyStates);
}

void BombRallyRoundPass(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->updateState = 0;
    TaskYieldTrampoline(16);
    t = gCurTask;
    if (t->bombRallyThrow != -3)
        t->bombRallyThrow = -2;
    while (gCurTask->bombRallyThrow < 0)
        TaskYieldTrampoline(1);
    while (1) {
        gCurTask->bombRallyThrow = gCurTask->bombRallyNextThrow;
        gCurTask->bombRallyNextThrow = -1;
        if (gCurTask->bombRallyThrow == 6 || gCurTask->bombRallyThrow == -1)
            break;
        u = gCurTask;
        if (u->bombRallyThrow <= 2)
            u->bombRallyPassFrames = gBombRallyBeatFrames[u->bombRallySpeedLevel] * (u->bombRallyThrow + 2);
        else
            u->bombRallyPassFrames = gBombRallyBeatFrames[u->bombRallySpeedLevel] * (u->bombRallyThrow - 1);
        gCurTask->bombRallyPassTimer = 0;
        while (gCurTask->bombRallyPassTimer != gCurTask->bombRallyPassFrames) {
            gCurTask->bombRallyPassTimer++;
            TaskYieldTrampoline(1);
        }
        if (gCurTask->bombRallyNextThrow != 6 && gCurTask->bombRallyNextThrow != -1)
            TaskYieldTrampoline(3);
        if (gBombRallySafeBeatsLeft != 0)
            gBombRallySafeBeatsLeft--;
        v = gCurTask;
        v->bombRallyPassCount++;
        if (v->bombRallyPassCount == gBombRallySpeedUpBeats[gSubGameLevel]) {
            v->bombRallyPassCount = 0;
            if (v->bombRallySpeedLevel + 1 != 7)
                v->bombRallySpeedLevel++;
        }
        w = gCurTask;
        if (w->bombRallyThrow <= 2) {
            w->bombRallyTurnSeat++;
            if (w->bombRallyTurnSeat == 4)
                w->bombRallyTurnSeat = 0;
        } else {
            w->bombRallyTurnSeat--;
            if (w->bombRallyTurnSeat < 0)
                w->bombRallyTurnSeat = 3;
        }
    }
    gCurTask->bombRallyThrow = 6;
    if (BombRallyKnockOutTurnPlayer())
        StopBgm();
    TaskYieldTrampoline(120);
    gCurTask->state = 1;
    TaskSleepForever();
}

void BombRallyRoundPassUpdate(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(BombRallyEnterState, gCurTaskIdx);
}

void BombRallyRoundNext(void)
{
    u8 i;

    gCurTask->updateState = 1;
    if (BombRallyIsMatchOver()) {
        while (gBombRallyOutCount != 3) {
            for (i = 0; i < 4; i++) {
                if (gBombRallyFinishOrder[i] == 3) {
                    gBombRallyFinishOrder[i] = gBombRallyOutCount;
                    break;
                }
            }
            gBombRallyOutCount++;
        }
        gCurTask->subGameNextPhase = 2;
    } else {
        BeginFastFadeOutToWhite();
        while (gFadeSteps != 0)
            TaskYieldTrampoline(1);
        BombRallyRestartSpeed();
        BeginFastFadeInFromWhite();
        while (gFadeSteps != 0)
            TaskYieldTrampoline(1);
    }
    gCurTask->state = 0;
    TaskSleepForever();
}

void BombRallyRoundNextUpdate(void)
{
    if (gCurTask->state != 1)
        TaskSetEntry(BombRallyEnterState, gCurTaskIdx);
}

u32 BombRallyKnockOutTurnPlayer(void)
{
    s32 i = gBombRallySeats[gCurTask->bombRallyTurnSeat];

    gBombRallyFinishOrder[i] = gBombRallyOutCount;
    gBombRallyOutCount++;
    gBombRallyOutMask |= 1 << i;
    return BombRallyIsMatchOver();
}

u32 BombRallyIsMatchOver(void)
{
    s32 i;
    s32 mask = 0;

    for (i = 0; i < gPlayerCount; i++)
        mask += 1 << i;
    if (gBombRallyOutCount == 3 || (gBombRallyOutMask & mask) == mask)
        return 1;
    return 0;
}

void BombRallyInitSpeed(void)
{
    struct Task *t;

    gBombRallySafeBeatsLeft = gBombRallySafeBeats[gSubGameLevel];
    t = gCurTask;
    t->bombRallyThrow = -4;
    t->bombRallyNextThrow = 1;
    t->bombRallyPassTimer = 0;
    t->bombRallySpeedLevel = gBombRallyStartSpeeds[gSubGameLevel];
    t->bombRallyPassFrames = gBombRallyBeatFrames[t->bombRallySpeedLevel] * (t->bombRallyNextThrow + 2);
}

void BombRallyRestartSpeed(void)
{
    struct Task *t;
    struct Task *u;

    gBombRallySafeBeatsLeft = gBombRallySafeBeats[gSubGameLevel];
    t = gCurTask;
    t->bombRallyThrow = -4;
    t->bombRallyNextThrow = 1;
    t->bombRallyPassTimer = 0;
    t->bombRallySpeedLevel -= 2;
    if (t->bombRallySpeedLevel < gBombRallyStartSpeeds[gSubGameLevel])
        t->bombRallySpeedLevel = gBombRallyStartSpeeds[gSubGameLevel];
    u = gCurTask;
    u->bombRallyPassFrames = gBombRallyBeatFrames[u->bombRallySpeedLevel] * (u->bombRallyNextThrow + 2);
}

void CreateBombRallyBomb(u32 a)
{
    s32 i = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->parent = gCurTaskIdx;
        t->variant = BOMB_RALLY_OBJECT_VARIANT_BOMB;
        t->bombRallyObjectStartSeat = a;
    }
}

void CreateBombRallyBombSmoke(s32 a, s32 b, u16 c, s32 d)
{
    s32 i = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->parent = gCurTaskIdx;
        t->variant = BOMB_RALLY_OBJECT_VARIANT_BOMB_SMOKE;
        t->unk18 = (s16)c;
        t->unk1C = d;
        t->posX = a + (gCurTask->facing << 19);
        t->posY = b - 0x40000;
        t->pixelX = a >> 16;
        t->pixelY = b >> 16;
    }
}

void CreateBombRallyStarBurst(s32 a, s32 b, u32 c, u32 d)
{
    s32 i = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->parent = gCurTaskIdx;
        t->variant = BOMB_RALLY_OBJECT_VARIANT_STAR_BURST;
        t->unk74 = c;
        t->facing = d;
        t->posX = a;
        t->posY = b;
        t->pixelX = a >> 16;
        t->pixelY = b >> 16;
    }
}

void CreateBombRallyBurstStar(u32 a)
{
    s32 i = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->parent = gCurTaskIdx;
        t->variant = BOMB_RALLY_OBJECT_VARIANT_STAR_BURST;
        t->unk74 = a;
        t->facing = gCurTask->facing;
        t->posX = gCurTask->posX;
        t->posY = gCurTask->posY;
        t->pixelX = gCurTask->pixelX;
        /* The ROM reads unk48 twice here; unk4A is never sourced. */
        t->pixelY = gCurTask->pixelX;
    }
}

void CreateBombRallyStartSign(void)
{
    s32 i = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->parent = gCurTaskIdx;
        t->variant = BOMB_RALLY_OBJECT_VARIANT_START_SIGN;
    }
}

void BombRallySeatPlayers(void)
{
    u8 arr[4];
    s32 mask;
    s32 i;
    s32 j;
    s32 k;
    s32 r;
    s32 slot;

    mask = 0;
    for (i = 0; i <= 3; i++) {
        do {
            r = RandomRange(4);
        } while ((mask >> r) & 1);
        mask |= 1 << r;
        arr[i] = r;
        if (r == gLocalPlayer)
            slot = i;
    }
    r = RandomRange(gPlayerCount);
    for (i = 0; i <= 3; i++) {
        gBombRallySeats[i] = arr[(slot + i) & 3];
        if (gBombRallySeats[i] == r)
            gCurTask->bombRallyTurnSeat = i;
        RequestCopy(2, gUnk_08756528[gBombRallySeats[i]],
                     (u32)(gObjPalette + (i << 5)), 32);
    }
    CreateBombRallyBomb(0);
    for (i = 0; i <= 3; i++) {
        for (j = 0; j < 4; j++)
            if (i == gBombRallySeats[j])
                break;
        k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
        if (k != -1) {
            struct Task *t = &gTasks[k];

            t->parent = gCurTaskIdx;
            t->variant = BOMB_RALLY_OBJECT_VARIANT_PLAYER;
            t->bombRallyObjectPlayerIndex = i;
            t->bombRallyObjectSeat = j;
        }
    }
}

void BombRallyResults(void)
{
    gCurTask->updateCallback = (u32)BombRallyResultsUpdate;
    CreateBombRallyResultsPoses();
    gBg3ScrollX = 0x780000;
    gBg3ScrollY = 0;
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 2, gBombRallyResultsStates);
    TaskSleepForever();
}

void BombRallyResultsUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 2, gBombRallyResultsStateUpdates);
    SubGameCheckEnd();
}

void BombRallyResultsEnterState(void)
{
    CallTableEntry(gCurTask->state, 2, gBombRallyResultsStates);
}

void BombRallyResultsShow(void)
{
    s32 i;
    s32 x;
    vu16 *p;
    s32 m;

    gCurTask->updateState = 0;
    TaskYieldTrampoline(60);
    CreateBombRallyPlaceLabels();
    switch (gBombRallyFinishOrder[gLocalPlayer]) {
    case 3:
        PlayBgm(29);
        break;
    case 1:
    case 2:
        PlayBgm(28);
        break;
    case 0:
        PlayBgm(23);
        break;
    }
    x = 0;
    for (i = 0; i < gPlayerCount; i++) {
        if (gBombRallyFinishOrder[i] == 3) {
            x = 2;
            break;
        }
        if (gBombRallyFinishOrder[i] == 0)
            x = 1;
    }
    if (x == 0)
        TaskYieldTrampoline(75);
    else if (x == 1)
        TaskYieldTrampoline(168);
    else
        TaskYieldTrampoline(174);
    if (gPrevGameState == GAME_STATE_MAIN_MENU) {
        gCurTask->state = 1;
    } else {
        CreateBombRallyLivesIcons();
        TaskYieldTrampoline(60);
        BombRallyAwardLives();
        p = gPlayerPressedKeys;
        m = 9;
        goto wait;
        do {
            TaskYieldTrampoline(1);
        wait:
            ;
        } while ((*p & m) == 0);
        SubGameQuit();
    }
    TaskSleepForever();
}

void BombRallyResultsShowUpdate(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(BombRallyResultsEnterState, gCurTaskIdx);
}

void BombRallyResultsMenu(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 a;
    s32 c;
    u16 b;

    gCurTask->updateState = 1;
    gCurTask->bombRallyMenuPage = 0;
    while (1) {
        gCurTask->bombRallyMenuCursor = 0;
        CreateBombRallyContinueItems(0);
        while (gCurTask->bombRallyMenuPage == 0) {
            TaskYieldTrampoline(1);
            if (gPlayerPressedKeys[0] & 9) {
                BombRallyPlaySfxIfPlayer0(102);
                t = gCurTask;
                if (t->bombRallyMenuCursor == 0) {
                    t->bombRallyMenuPage = 1;
                    TaskYieldTrampoline(16);
                } else {
                    SubGameQuit();
                    TaskSleepForever();
                }
            } else if (gPlayerPressedKeys[0] & 0xF0) {
                BombRallyPlaySfxIfPlayer0(101);
                u = gCurTask;
                u->bombRallyMenuCursor ^= 1;
                if (gPlayerPressedKeys[0] & 0x60)
                    u->bombRallyMenuMoveDir = 1;
                else
                    u->bombRallyMenuMoveDir = 0;
                TaskYieldTrampoline(10);
            }
        }
        gCurTask->bombRallyMenuCursor = c = gSubGameLevel;
        CreateBombRallyLevelItems(c);
        while (gCurTask->bombRallyMenuPage == 1) {
            TaskYieldTrampoline(1);
            a = gPlayerPressedKeys[0] & 9;
            if (a) {
                BombRallyPlaySfxIfPlayer0(102);
                SubGameReplay(gCurTask->bombRallyMenuCursor);
                TaskSleepForever();
            } else {
                b = gPlayerPressedKeys[0] & 2;
                if (b) {
                    gCurTask->bombRallyMenuPage = a;
                    BombRallyPlaySfxIfPlayer0(215);
                    TaskYieldTrampoline(16);
                } else if (gPlayerPressedKeys[0] & 0x90) {
                    BombRallyPlaySfxIfPlayer0(101);
                    v = gCurTask;
                    v->bombRallyMenuCursor++;
                    if (v->bombRallyMenuCursor == 3)
                        v->bombRallyMenuCursor = b;
                    gCurTask->bombRallyMenuMoveDir = b;
                    TaskYieldTrampoline(10);
                } else if (gPlayerPressedKeys[0] & 0x60) {
                    BombRallyPlaySfxIfPlayer0(101);
                    w = gCurTask;
                    w->bombRallyMenuCursor--;
                    if (w->bombRallyMenuCursor < 0)
                        w->bombRallyMenuCursor = 2;
                    gCurTask->bombRallyMenuMoveDir = 1;
                    TaskYieldTrampoline(10);
                }
            }
        }
    }
}

void BombRallyResultsMenuUpdate(void)
{
}

void CreateBombRallyResultsPoses(void)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gPlayerCount == 1) {
        k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
        if (k != -1) {
            t = &gTasks[k];
            t->parent = gCurTaskIdx;
            t->variant = BOMB_RALLY_OBJECT_VARIANT_RESULTS_PLAYER;
            t->unk18 = 0;
            t->unk1C = 0;
            t->unk20 = 0;
        }
    } else {
        for (i = 0; i <= 3; i++) {
            k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->parent = gCurTaskIdx;
                t->variant = BOMB_RALLY_OBJECT_VARIANT_RESULTS_PLAYER;
                t->unk18 = i;
                t->unk1C = 0;
                t->unk20 = gBombRallySeats[i];
            }
        }
    }
}

void CreateBombRallyLivesIcons(void)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gPlayerCount == 1) {
        k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
        if (k != -1) {
            t = &gTasks[k];
            t->parent = gCurTaskIdx;
            t->variant = BOMB_RALLY_OBJECT_VARIANT_RESULTS_PLAYER;
            t->unk18 = 0;
            t->unk1C = 2;
            t->unk20 = 0;
        }
    } else {
        for (i = 0; i <= 3; i++) {
            k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->parent = gCurTaskIdx;
                t->variant = BOMB_RALLY_OBJECT_VARIANT_RESULTS_PLAYER;
                t->unk18 = i;
                t->unk1C = 2;
                t->unk20 = gBombRallySeats[i];
            }
        }
    }
}

void CreateBombRallyPlaceLabels(void)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gPrevGameState != GAME_STATE_HUB) {
        if (gPlayerCount == 1) {
            k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->parent = gCurTaskIdx;
                t->variant = BOMB_RALLY_OBJECT_VARIANT_RESULTS_PLAYER;
                t->unk18 = 0;
                t->unk1C = 1;
                t->unk20 = 0;
            }
        } else {
            for (i = 0; i <= 3; i++) {
                k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
                if (k != -1) {
                    t = &gTasks[k];
                    t->parent = gCurTaskIdx;
                    t->variant = BOMB_RALLY_OBJECT_VARIANT_RESULTS_PLAYER;
                    t->unk18 = i;
                    t->unk1C = 1;
                    t->unk20 = gBombRallySeats[i];
                }
            }
        }
    }
}

void CreateBombRallyContinueItems(u32 a)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gLocalPlayer == 0) {
        for (i = 0; i <= 1; i++) {
            k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->parent = gCurTaskIdx;
                t->variant = BOMB_RALLY_OBJECT_VARIANT_MENU_ITEM;
                t->unk74 = 0;
                t->unk18 = i;
                t->unk1C = a;
            }
        }
    }
}

void CreateBombRallyLevelItems(u32 a)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gLocalPlayer == 0) {
        for (i = 0; i <= 2; i++) {
            k = TaskCreateFrom(TASK_BOMB_RALLY_OBJECT, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->parent = gCurTaskIdx;
                t->variant = BOMB_RALLY_OBJECT_VARIANT_MENU_ITEM;
                t->unk74 = 1;
                t->unk18 = i;
                t->unk1C = a;
            }
        }
    }
}

void BombRallyAwardLives(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        switch (gBombRallyFinishOrder[i]) {
        case 0:
            break;
        case 3:
            AddPlayerLivesNoHud(3, i);
            break;
        case 2:
            AddPlayerLivesNoHud(2, i);
            break;
        case 1:
            AddPlayerLivesNoHud(1, i);
            break;
        }
    }
}

void BombRallyPlaySfxIfPlayer0(u32 a)
{
    if (gLocalPlayer == 0)
        PlaySfx(a);
}

void Task_BombRallyObject(void)
{
    CallTableEntry(gCurTask->variant, 7, gBombRallyObjectVariants);
}

void BombRallyPlayer(void)
{
    struct Task *t;

    t = gCurTask;
    t->drawCallback = (u32)sub_080060c0;
    t->updateCallback = (u32)BombRallyPlayerUpdate;
    if (t->bombRallyObjectSeat <= 1)
        t->layer = 7;
    else
        t->layer = 9;
    gCurTask->state = 0;
    CallTableEntry(gCurTask->state, 13, gBombRallyPlayerStates);
    TaskSleepForever();
}

void BombRallyPlayerUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    t = gCurTask;
    u = &gTasks[t->parent];
    CallTableEntry(t->updateState, 13, gBombRallyPlayerStateUpdates);
    w = gCurTask;
    if (w->state != 9 && u->bombRallyThrow == 6) {
        w->state = 9;
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
    }
}

void BombRallyPlayerEnterState(void)
{
    CallTableEntry(gCurTask->state, 13, gBombRallyPlayerStates);
}

void BombRallyPlayerServe(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;

    v = gCurTask;
    v->moveCallback = (u32)TaskMoveRelativeToBg3;
    v->updateState = 0;
    w = gCurTask;
    w->posX = gUnk_0875672C[gCurTask->bombRallyObjectSeat] << 16;
    w->posY = gUnk_08756734[gCurTask->bombRallyObjectSeat] << 16;
    w->frameTable = gBombRallyPlayerFrames[gCurTask->bombRallyObjectSeat];
    if (gCurTask->bombRallyObjectSeat == 3) {
        w->facing = -1;
        gCurTask->tileWord = 0xA0 << 6;
    } else {
        w->facing = 1;
        gCurTask->tileWord = 0;
    }
    TaskSetFrame(0);
    x = gCurTask;
    x->bombRallyObjectAimSide = 0;
    x->unk24 = 0;
    while (1) {
        TaskYieldTrampoline(1);
        t = gCurTask;
        u = &gTasks[t->parent];
        if (u->bombRallyTurnSeat == t->bombRallyObjectSeat) {
            if (u->bombRallyThrow == -2 && (gPlayerPressedKeys[t->bombRallyObjectPlayerIndex] & 1)) {
                u->bombRallyThrow = -1;
                TaskSetFrame(2);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(5);
                gCurTask->frame++;
                TaskYieldTrampoline(7);
                gCurTask->frame++;
                TaskYieldTrampoline(8);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                PlaySfx(254);
                z = gCurTask;
                CreateBombRallyStarBurst(z->posX + (gUnk_08756560[z->bombRallyObjectSeat] << 16) * z->facing,
                             z->posY + (gUnk_08756564[z->bombRallyObjectSeat] << 16),
                             z->bombRallyObjectSeat, z->facing);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
                u->bombRallyThrow = u->bombRallyNextThrow;
                TaskSetFrame(11);
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(15);
                gCurTask->frame++;
                TaskYieldTrampoline(4);
                TaskSetFrame(1);
                TaskYieldTrampoline(4);
                TaskSetFrame(15);
                TaskYieldTrampoline(6);
                break;
            }
        } else if (u->bombRallyThrow >= 0) {
            break;
        }
    }
    y = gCurTask;
    if (y->bombRallyObjectPlayerIndex < gPlayerCount)
        y->state = 1;
    else
        y->state = 5;
    TaskSleepForever();
}

void BombRallyPlayerServeUpdate(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerReady(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->updateState = 1;
    u = gCurTask;
    if (u->bombRallyObjectSeat == 0 || u->bombRallyObjectSeat == 2) {
        if (u->bombRallyObjectAimSide == 0)
            u->facing = 1;
        else
            u->facing = -1;
        TaskSetFrame(0);
    } else if (u->bombRallyObjectAimSide == 0) {
        TaskSetFrame(0);
    } else {
        TaskSetFrame(16);
    }
    TaskSleepForever();
}

void BombRallyPlayerReadyUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    BombRallyPlayerUpdatePose();
    t = gCurTask;
    if (t->unk24 != t->bombRallyObjectAimSide) {
        t->state = 2;
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
    } else if (gPlayerPressedKeys[t->bombRallyObjectPlayerIndex] & 1) {
        if (BombRallyPlayerJudgePress()) {
            u = gCurTask;
            u->unk24 = 0;
            u->state = 3;
            TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
        } else {
            v = gCurTask;
            v->unk24 = v->bombRallyObjectAimSide ^ 1;
            v->state = 2;
            TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
        }
    }
}

void BombRallyPlayerTurn(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;

    t = gCurTask;
    t->updateState = 2;
    u = gCurTask;
    if (u->bombRallyObjectAimSide == 0) {
        u->bombRallyObjectAimSide = 1;
        if (u->bombRallyObjectSeat == 0 || u->bombRallyObjectSeat == 2)
            u->facing = 1;
        TaskSetFrame(15);
        TaskYieldTrampoline(3);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        v = gCurTask;
        v->frame--;
        TaskYieldTrampoline(2);
        w = gCurTask;
        if (gCurTask->bombRallyObjectSeat == 0 || gCurTask->bombRallyObjectSeat == 2) {
            n = w->facing;
            w->facing = -n;
        }
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
    } else {
        u->bombRallyObjectAimSide = 0;
        if (u->bombRallyObjectSeat == 0 || u->bombRallyObjectSeat == 2)
            u->facing = -1;
        TaskSetFrame(31);
        TaskYieldTrampoline(3);
        TaskSetFrame(17);
        TaskYieldTrampoline(4);
        x = gCurTask;
        x->frame--;
        TaskYieldTrampoline(2);
        y = gCurTask;
        if (y->bombRallyObjectSeat == 0 || y->bombRallyObjectSeat == 2) {
            n = y->facing;
            y->facing = -n;
        }
        TaskSetFrame(1);
        TaskYieldTrampoline(2);
    }
    z = gCurTask;
    z->state = 1;
    TaskSleepForever();
}

void BombRallyPlayerTurnUpdate(void)
{
    if (gPlayerPressedKeys[gCurTask->bombRallyObjectPlayerIndex] & 1)
        gCurTask->unk24 ^= 1;
    if (gCurTask->state != 2)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerThrow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    struct Task *z;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->updateState = 3;
    if (u->bombRallyNextThrow != 6) {
        v = gCurTask;
        n = u->bombRallyPassFrames - u->bombRallyPassTimer;
        v->unk34 = (n - (n >> 1)) - ((n - (n >> 1)) >> 1);
        v->unk30 = (n - (n >> 1)) >> 1;
        v->unk2C = (n >> 1) - (n >> 2);
        v->unk28 = n >> 2;
        if (v->bombRallyObjectSeat == 0 || v->bombRallyObjectSeat == 2) {
            if (v->bombRallyObjectAimSide == 0)
                v->facing = 1;
            else
                v->facing = -1;
        }
        if (gCurTask->bombRallyObjectAimSide == 0)
            TaskSetFrame(2);
        else
            TaskSetFrame(18);

        a = gCurTask;
        if ((a->unk28 >> 1) != 0) {
            a->frame++;
            TaskYieldTrampoline(a->unk28 >> 1);
        } else {
            a->frame++;
        }
        b = gCurTask;
        b->unk28 -= b->unk28 >> 1;
        if (b->unk28 != 0) {
            b->frame++;
            TaskYieldTrampoline(b->unk28);
        } else {
            b->frame++;
        }
        c = gCurTask;
        if ((c->unk2C >> 1) != 0) {
            c->frame++;
            TaskYieldTrampoline(c->unk2C >> 1);
        } else {
            c->frame++;
        }
        d = gCurTask;
        d->unk2C -= d->unk2C >> 1;
        if (d->unk2C != 0) {
            d->frame++;
            TaskYieldTrampoline(d->unk2C);
        } else {
            d->frame++;
        }
        e = gCurTask;
        if ((e->unk30 >> 1) != 0) {
            e->frame++;
            TaskYieldTrampoline(e->unk30 >> 1);
        } else {
            e->frame++;
        }
        f = gCurTask;
        f->unk30 -= f->unk30 >> 1;
        if (f->unk30 != 0) {
            f->frame++;
            TaskYieldTrampoline(f->unk30);
        } else {
            f->frame++;
        }
        g = gCurTask;
        if ((g->unk34 >> 1) != 0) {
            g->frame++;
            TaskYieldTrampoline(g->unk34 >> 1);
        } else {
            g->frame++;
        }
        h = gCurTask;
        h->unk34 -= h->unk34 >> 1;
        if (h->unk34 != 0) {
            h->frame++;
            TaskYieldTrampoline(h->unk34);
        } else {
            h->frame++;
        }
        PlaySfx(254);
        z = gCurTask;
        CreateBombRallyStarBurst(z->posX + (gUnk_08756560[z->bombRallyObjectSeat] << 16) * z->facing,
                     z->posY + (gUnk_08756564[z->bombRallyObjectSeat] << 16),
                     z->bombRallyObjectSeat, z->facing);
        TaskYieldTrampoline(3);
        w = gCurTask;
        w->state = 4;
    }
    TaskSleepForever();
}

void BombRallyPlayerThrowUpdate(void)
{
    if (gPlayerPressedKeys[gCurTask->bombRallyObjectPlayerIndex] & 1)
        gCurTask->unk24 ^= 1;
    if (gCurTask->state != 3)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerFollowThrough(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *z;

    t = gCurTask;
    t->updateState = 4;
    u = gCurTask;
    if (u->bombRallyObjectAimSide == 0) {
        TaskSetFrame(11);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(15);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    } else {
        u->bombRallyObjectAimSide = 0;
        TaskSetFrame(27);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(15);
        v = gCurTask;
        if (v->bombRallyObjectSeat == 0 || v->bombRallyObjectSeat == 2)
            v->facing = 1;
        TaskSetFrame(14);
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    }
    z = gCurTask;
    z->state = 1;
    TaskSleepForever();
}

void BombRallyPlayerFollowThroughUpdate(void)
{
    struct Task *t;
    struct Task *u;

    if (gPlayerPressedKeys[gCurTask->bombRallyObjectPlayerIndex] & 1) {
        if (BombRallyPlayerJudgePress()) {
            t = gCurTask;
            t->state = 3;
            gCurTask->unk24 = 0;
        } else {
            u = gCurTask;
            u->unk24 ^= 1;
        }
    }
    if (gCurTask->state != 4)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

u32 BombRallyPlayerJudgePress(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 q1;
    s32 q2;
    s32 q3;
    u8 *p;
    u8 *r;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 3) & 3) || u->bombRallyThrow > 2)
     && (u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 1) & 3) || u->bombRallyThrow <= 2)) {
        r = gUnk_087565F4[u->bombRallyThrow];
        v = u->bombRallySpeedLevel * 5;
        q1 = r[v + 1];
        q2 = r[v + 2];
        q3 = r[v + 3];
        if (u->bombRallyPassTimer >= r[v] && u->bombRallyPassTimer < q1)
            u->bombRallyNextThrow = 2;
        else if (u->bombRallyPassTimer >= q1 && u->bombRallyPassTimer < q2)
            u->bombRallyNextThrow = 1;
        else if (u->bombRallyPassTimer >= q2 && u->bombRallyPassTimer < q3)
            u->bombRallyNextThrow = 0;
        else
            return 0;
    } else {
        return 0;
    }
    if (gCurTask->bombRallyObjectAimSide == 1)
        u->bombRallyNextThrow += 3;
    return 1;
}

void BombRallyPlayerUpdatePose(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->parent];
    if (!((u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 3) & 3) || u->bombRallyThrow > 2)
       && (u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 1) & 3) || u->bombRallyThrow <= 2))) {
        if (gCurTask->bombRallyObjectAimSide == 0)
            TaskSetFrame(0);
        else
            TaskSetFrame(16);
    } else {
        if (gCurTask->bombRallyObjectAimSide == 0)
            TaskSetFrame(2);
        else
            TaskSetFrame(18);
    }
}

void BombRallyPlayerCpuReady(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gCurTask;
    t->updateState = 5;
    gCurTask->bombRallyObjectAimSide = 0;
    n = RandomRange(4);
    u = gCurTask;
    u->unk28 = (n + 1) * 20;
    if (u->bombRallyObjectSeat == 0 || u->bombRallyObjectSeat == 2)
        u->facing = 1;
    TaskSetFrame(0);
    TaskSleepForever();
}

void BombRallyPlayerCpuReadyUpdate(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 3) & 3) && u->bombRallyThrow <= 2)
     || (u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 1) & 3) && u->bombRallyThrow > 2)) {
        gCurTask->state = 7;
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
    } else {
        w = gCurTask;
        w->unk28--;
        if (w->unk28 == 0) {
            if (RandomRange(8) == 0) {
                gCurTask->state = 6;
                TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
            } else {
                n = RandomRange(4);
                gCurTask->unk28 = (n + 1) * 20;
            }
        }
    }
}

void BombRallyPlayerCpuTurn(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;

    t = gCurTask;
    t->updateState = 6;
    u = gCurTask;
    if (u->bombRallyObjectSeat == 0 || u->bombRallyObjectSeat == 2)
        u->facing = 1;
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(1);
    TaskYieldTrampoline(4);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    v = gCurTask;
    if (v->bombRallyObjectSeat == 0 || v->bombRallyObjectSeat == 2)
        v->facing = -1;
    w = gCurTask;
    w->bombRallyObjectAimSide = 1;
    TaskSetFrame(17);
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskSleepForever();
}

void BombRallyPlayerCpuTurnUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 3) & 3) && u->bombRallyThrow <= 2)
     || (u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 1) & 3) && u->bombRallyThrow > 2))
        gCurTask->state = 7;
    if (gCurTask->state != 6)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerCpuThrow(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    struct Task *z;
    u8 *tbl;
    s32 i;
    u8 q;
    s32 w;
    s32 n;
    u8 k;
    s32 m;
    s32 vv;

    u = &gTasks[gCurTask->parent];
    tbl = gUnk_087565F4[u->bombRallyThrow];
    gCurTask->updateState = 7;
    k = gBombRallyBlastOdds[gSubGameLevel][u->bombRallySpeedLevel * 3 + gBombRallyOutCount];
    if (gBombRallySafeBeatsLeft == 0 && RandomRange(k) == 0) {
        u->bombRallyNextThrow = 6;
    } else {
        q = RandomRange(36);
        for (i = 0; i <= 1; i++) {
            if (q < gUnk_087565E0[i + u->bombRallyThrow * 3])
                break;
        }
        if (gCurTask->bombRallyObjectAimSide == 0)
            u->bombRallyNextThrow = i;
        else
            u->bombRallyNextThrow = i + 3;
    }
    w = u->bombRallyNextThrow;
    if (w > 2)
        w -= 3;
    q = w;
    t = gCurTask;
    vv = u->bombRallySpeedLevel * 5;
    m = q - 2;
    t->unk28 = tbl[vv - m];
    if (t->bombRallyObjectSeat == 1 || t->bombRallyObjectSeat == 3) {
        if (t->bombRallyObjectAimSide == 0)
            TaskSetFrame(2);
        else
            TaskSetFrame(18);
        TaskYieldTrampoline(gCurTask->unk28);
    } else {
        if (t->bombRallyObjectAimSide == 0)
            t->facing = 1;
        else
            t->facing = -1;
        TaskSetFrame(2);
        TaskYieldTrampoline(gCurTask->unk28);
    }
    if (u->bombRallyNextThrow != 6) {
        a = gCurTask;
        n = u->bombRallyPassFrames - a->unk28;
        a->unk34 = (n - (n >> 1)) - ((n - (n >> 1)) >> 1);
        a->unk30 = (n - (n >> 1)) >> 1;
        a->unk2C = (n >> 1) - (n >> 2);
        a->unk28 = n >> 2;
        if ((a->unk28 >> 1) != 0) {
            a->frame++;
            TaskYieldTrampoline(a->unk28 >> 1);
        } else {
            a->frame++;
        }
        b = gCurTask;
        b->unk28 -= b->unk28 >> 1;
        if (b->unk28 != 0) {
            b->frame++;
            TaskYieldTrampoline(b->unk28);
        } else {
            b->frame++;
        }
        c = gCurTask;
        if ((c->unk2C >> 1) != 0) {
            c->frame++;
            TaskYieldTrampoline(c->unk2C >> 1);
        } else {
            c->frame++;
        }
        d = gCurTask;
        d->unk2C -= d->unk2C >> 1;
        if (d->unk2C != 0) {
            d->frame++;
            TaskYieldTrampoline(d->unk2C);
        } else {
            d->frame++;
        }
        e = gCurTask;
        if ((e->unk30 >> 1) != 0) {
            e->frame++;
            TaskYieldTrampoline(e->unk30 >> 1);
        } else {
            e->frame++;
        }
        f = gCurTask;
        f->unk30 -= f->unk30 >> 1;
        if (f->unk30 != 0) {
            f->frame++;
            TaskYieldTrampoline(f->unk30);
        } else {
            f->frame++;
        }
        g = gCurTask;
        if ((g->unk34 >> 1) != 0) {
            g->frame++;
            TaskYieldTrampoline(g->unk34 >> 1);
        } else {
            g->frame++;
        }
        h = gCurTask;
        h->unk34 -= h->unk34 >> 1;
        if (h->unk34 != 0) {
            h->frame++;
            TaskYieldTrampoline(h->unk34);
        } else {
            h->frame++;
        }
        PlaySfx(254);
        z = gCurTask;
        CreateBombRallyStarBurst(z->posX + (gUnk_08756560[z->bombRallyObjectSeat] << 16) * z->facing,
                     z->posY + (gUnk_08756564[z->bombRallyObjectSeat] << 16),
                     z->bombRallyObjectSeat, z->facing);
        TaskYieldTrampoline(3);
        gCurTask->state = 8;
    }
    TaskSleepForever();
}

void BombRallyPlayerCpuThrowUpdate(void)
{
    if (gCurTask->state != 7)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerCpuFollowThrough(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->updateState = 8;
    u = gCurTask;
    if (u->bombRallyObjectAimSide == 0) {
        TaskSetFrame(11);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(15);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    } else {
        u->bombRallyObjectAimSide = 0;
        TaskSetFrame(27);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(15);
        v = gCurTask;
        if (v->bombRallyObjectSeat == 0 || v->bombRallyObjectSeat == 2)
            v->facing = 1;
        TaskSetFrame(14);
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    }
    gCurTask->state = 5;
    TaskSleepForever();
}

void BombRallyPlayerCpuFollowThroughUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->parent];
    if ((u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 3) & 3) && u->bombRallyThrow <= 2)
     || (u->bombRallyTurnSeat == ((t->bombRallyObjectSeat + 1) & 3) && u->bombRallyThrow > 2))
        gCurTask->state = 7;
    if (gCurTask->state != 8)
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
}

void BombRallyPlayerBlownUp(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *z;
    s32 x;
    s32 y;
    s32 c;

    t = gCurTask;
    u = &gTasks[t->parent];
    t->updateState = 9;
    v = gCurTask;
    if (v->bombRallyObjectSeat == u->bombRallyTurnSeat) {
        v->frame = 0xFFFF;
        TaskStop();
        gCurTask->unk6C = 0;
        do {
            x = gUnk_0875672C[gCurTask->bombRallyObjectSeat] - (gBg3ScrollX >> 16)
              + gUnk_08756740[gCurTask->bombRallyObjectSeat] * (s16)gCurTask->unk6C;
            y = gUnk_08756734[gCurTask->bombRallyObjectSeat] - (gBg3ScrollY >> 16)
              + gUnk_08756744[gCurTask->bombRallyObjectSeat] * (s16)gCurTask->unk6C;
            c = gUnk_08756538[gCurTask->bombRallyObjectSeat] + gUnk_08756748[gCurTask->bombRallyObjectSeat] * (s16)gCurTask->unk6C;
            if (c <= 127)
                c = 128;
            if (x >= -63 && x <= 303 && y > -64 && y <= 223)
                QueueSprite(8, DrawAffineSprite(gUnk_08755DC0, c, c,
                                            (((s16)gCurTask->unk6C * 3) << 3) & 0xFF),
                             0, gCurTask->bombRallyObjectSeat << 12, x, (s16)y);
            TaskYieldTrampoline(1);
            z = gCurTask;
            z->unk6C++;
        } while ((s16)z->unk6C <= 63);
    }
    TaskSleepForever();
}

void BombRallyPlayerBlownUpUpdate(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->parent];
    if (u->bombRallyThrow == -4) {
        if (((gBombRallyOutMask >> t->bombRallyObjectPlayerIndex) & 1) == 0)
            t->state = 0;
        else
            t->state = 10;
        TaskSetEntry(BombRallyPlayerEnterState, gCurTaskIdx);
    }
}
