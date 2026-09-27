#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "player.h"
#include "actor.h"

/* player_4b5b4.c (0x0804B5B4-0x0804B857, issue #88).
 *
 * Player action bodies, part 20: action 52 and per-frame handler 49.
 * PlayerActionLight (action 52, mode 13) is a linear yield script with the
 * stage frozen (gUnk_03001F34 = 1): M14's CreatePlayerObject(player, 9, 0),
 * the animations 0xDFD-0xE05 in Task.unk6C counter loops and velocity
 * presets 54/55; it then restores the default script gPlayerDefaultTerrainBox in
 * PlayerState.terrainBox, resets the HUD ability panel (SetPlayerAbility(0, -1,
 * player)) and plays 0xE06 on the ground.  Its handler PlayerActionLightUpdate
 * requests action 1 (on the ground) or 7 once Task.unk28 is set; the
 * ROM keeps a dead `ldr [t, #84]` of a test whose two arms were
 * merged. */

s32 SetPlayerAbility(s32 a, s32 b, u32 c);       /* landed (hud_099fc.c); M12 calls it as SetPlayerAbility(0, -1, p->unk00) */
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionLight(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 13;
    gCurTask->updateState = 49;
    gUnk_03001F34 = 1;
    gCurTask->unk80 = 0;
    PlayerStopAxes(3);
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        t->unk2C = 0;
        t->player->unk42 |= 0x700;
        t->player->terrainBox = 0;
    }
    FreezeOtherTasks(15);
    CreatePlayerObject(gCurTask->player->playerIndex, 9, 0);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xDFD);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 3);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xDFF);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 1);
    TaskSetFrame(0xE01);
    TaskYieldTrampoline(1);
    PlayerSetMotionYPreset(54);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xE03);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 3);
    PlayerStopAxes(2);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xE03);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        TaskSetFrame(0xE05);
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 4);
    PlayerSetMotionYPreset(55);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0xE03);
        TaskYieldTrampoline(1);
        gCurTask->frame++;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 15);
    {
        struct Task *t = gCurTask;
        t->player->terrainBox = (u32)gPlayerDefaultTerrainBox;
        SetPlayerAbility(0, -1, t->player->playerIndex);
    }
    FreezeOtherTasks(0);
    if (gCurTask->onGround & 1) {
        TaskSetFrame(0xE06);
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(4);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
    }
    {
        struct Task *t = gCurTask;
        t->unk28++;
        gUnk_03001F34 = 0;
        t->player->unk42 &= 0xF8FF;
    }
    TaskSleepForever();
}

void PlayerActionLightUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0) {
        if (t->onGround & 1) {
            /* Both arms store 1: jump2 cross-jumps them after reload and
               deletes the branch, but the `ldr [t, #84]` that fed the test
               stays in the ROM (0x0804B834) as a dead load. */
            if (t->velX == 0)
                t->player->requestedAction = 1;
            else
                t->player->requestedAction = 1;
        } else {
            t->player->requestedAction = 7;
        }
    }
    PlayerStopAtCeilingAndWall();
}
