/* game_code_and_rodata 0x0806EF5C-0x0806FF24 (issue #64, module M18 batch 7).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x0806EF5C 0x0806FF24 src/actor_6ef5c.c --newpb
 *
 * Class-1 task bodies for the scripted "player enters / leaves the stage"
 * sequences, laid out as repeating pairs:
 *
 *   <sequence body>   sets Task.updateState to the state id, installs
 *                     sub_0803ddc0 in Task.drawCallback, picks a mask from
 *                     PlayerState.playerIndex and hands it to TaskSetMotionXFacing, then
 *                     walks a fixed run of `Task.velY = <16.16 offset>;
 *                     TaskYieldTrampoline(8);` steps with
 *                     `while (gCurTask->unk24 != K) TaskYieldTrampoline(1);`
 *                     barriers between them.  PlayerWarpStarRideState1, PlayerWarpStarRideState3,
 *                     PlayerWarpStarRideState4, PlayerWarpStarRideState5 (the 0x474 leader),
 *                     PlayerWarpStarRideState6 and PlayerWarpStarRideState7.
 *   <per-frame hook>  PlayerWarpStarRideState1Update, PlayerWarpStarRideState3Update, PlayerWarpStarRideState4Update,
 *                     PlayerWarpStarRideState5Update and PlayerWarpStarRideState6Update: re-aim the camera at
 *                     the owning player, run TerrainCollideBox over a ROM
 *                     descriptor, and advance Task.unk24 on the
 *                     `Task.onGround & 1` edge.
 *
 * PlayerWarpStarRideState2 is the idle animation loop the sequences fall back to, and
 * PlayerWarpStarRideState2Update is a real no-op callback slot (a ROM-pointer entry, not
 * padding).
 *
 * Two symbol-DB false positives live in this range and are curated away in
 * tools/symdb.py: 0x0806F0E2 (inside PlayerWarpStarRideState1, whose true extent is
 * 0x0806EFEC-0x0806F174) and 0x0806FC3E (inside PlayerWarpStarRideState6,
 * 0x0806FB0C-0x0806FC98).  Both come from the word 0xFFFFF000 in a
 * neighbouring literal pool decoding as a bl pair.
 */

#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "room.h"
#include "player.h"
#include "actor.h"

extern void TerrainCollideBox(u8 *);
extern void sub_080706a8(void);

void PlayerWarpStarRideState2(void)
{
    s32 d, n, i;
    u8 k;

    gCurTask->updateState = 2;
    sub_08070208();
    PlayerLoadSparkTiles();
    k = gCurTask->player->ability;
    if (k == 1 || k == 2 || (s8)k == 5 || (s8)k == 19 || (s8)k == 25)
    {
        n = 3;
        d = 2;
        if (gCurTask->player->ability == ABILITY_STAR_ROD)
        {
            n = 1;
            d = 1;
        }
        while (1)
        {
            gCurTask->frame = gCurTask->unk28;
            TaskYieldTrampoline(d);
            for (i = 0; i < n; i++)
            {
                gCurTask->frame++;
                TaskYieldTrampoline(d);
            }
        }
    }
    TaskSleepForever();
}

void PlayerWarpStarRideState2Update(void)
{
}

void PlayerWarpStarRideState1(void)
{
    struct PlayerState *p;

    gCurTask->updateState = 1;
    gCurTask->unk24 = 1;
    gCurTask->unk34 = 0;
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    LoadAbilityTiles();
    gCurTask->drawCallback = (u32)sub_0803ddc0;
    gCurTask->layer = 7;
    gCurTask->spriteFlags &= 0x7FFF;
    sub_0807029c();
    switch (gCurTask->player->playerIndex)
    {
    case 0:
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0x6000, 0x5A5A5A5A);
        break;
    case 2:
        TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
        break;
    case 3:
        TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
        break;
    }
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    while (gCurTask->unk24 != 3)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void PlayerWarpStarRideState1Update(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->player->playerIndex == gLocalPlayer)
        SetCameraFocus(t->pixelX, t->pixelY);
    TerrainCollideBox(gUnk_0873F5D4);
    if (gCurTask->onGround & 1)
    {
        gCurTask->onGround = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 2)
            sub_08070264();
    }
    sub_0807042c();
}

void PlayerWarpStarRideState3(void)
{
    struct PlayerState *p;

    gCurTask->updateState = 3;
    gCurTask->unk24 = 1;
    gCurTask->unk34 = 0;
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    LoadAbilityTiles();
    gCurTask->drawCallback = (u32)sub_0803ddc0;
    gCurTask->layer = 7;
    gCurTask->spriteFlags &= 0x7FFF;
    sub_0807029c();
    switch (gCurTask->player->playerIndex)
    {
    case 0:
        TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0xA000, 0x5A5A5A5A);
        break;
    case 2:
        TaskSetMotionXFacing(0x6000, 0x5A5A5A5A);
        break;
    case 3:
        TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
        break;
    }
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    while (gCurTask->unk24 != 3)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void PlayerWarpStarRideState3Update(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->player->playerIndex == gLocalPlayer)
        SetCameraFocus(t->pixelX, t->pixelY);
    TerrainCollideBox(gUnk_0873F5D4);
    if (gCurTask->onGround & 1)
    {
        gCurTask->onGround = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 2)
            sub_08070264();
    }
    sub_0807042c();
}

void PlayerWarpStarRideState4(void)
{
    struct PlayerState *p;

    gCurTask->updateState = 4;
    gCurTask->unk24 = 0;
    gCurTask->unk34 = 0;
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    LoadAbilityTiles();
    gCurTask->drawCallback = (u32)sub_0803ddc0;
    gCurTask->layer = 7;
    gCurTask->facing = -1;
    gCurTask->spriteFlags &= 0x7FFF;
    sub_0807029c();
    switch (gCurTask->player->playerIndex)
    {
    case 0:
        TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0xA000, 0x5A5A5A5A);
        break;
    case 2:
        TaskSetMotionXFacing(0x6000, 0x5A5A5A5A);
        break;
    case 3:
        TaskSetMotionXFacing(0xC000, 0x5A5A5A5A);
        break;
    }
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFEC000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF4000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xC000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x14000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    while (gCurTask->unk24 != 3)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void PlayerWarpStarRideState4Update(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->player->playerIndex == gLocalPlayer)
        SetCameraFocus(t->pixelX, t->pixelY);
    TerrainCollideBox(gUnk_0873F5D4);
    if (gCurTask->onGround & 1)
    {
        gCurTask->onGround = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 2)
        {
            u->facing = 1;
            sub_08070264();
        }
    }
    sub_0807042c();
}

void PlayerWarpStarRideState5(void)
{
    struct PlayerState *p;
    s32 k;

    gCurTask->updateState = 5;
    gCurTask->unk24 = 0;
    gCurTask->unk34 = 0;
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    LoadAbilityTiles();
    gCurTask->drawCallback = (u32)sub_0803ddc0;
    gCurTask->layer = 7;
    gCurTask->facing = 1;
    switch (gCurTask->player->playerIndex)
    {
    case 0:
        TaskSetMotionXFacing(0xFFFEF000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0xFFFF2000, 0x5A5A5A5A);
        break;
    case 2:
        TaskSetMotionXFacing(0xFFFED000, 0x5A5A5A5A);
        break;
    case 3:
        TaskSetMotionXFacing(0xFFFF4000, 0x5A5A5A5A);
        break;
    }
    k = gCurTask->player->ability;
    if (k == 10)
        TaskSetFrame(0x841);
    else
        TaskSetFrame(gUnk_0873D3B8[k][1]);
    gCurTask->velY = 0xFFFD0000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    switch (gCurTask->player->ability)
    {
    case ABILITY_SWORD:
    case ABILITY_HAMMER:
        TaskSetFrame(gUnk_0873D3B8[gCurTask->player->ability][1]);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x10000;
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        break;
    case ABILITY_PARASOL:
        TaskSetFrame(0x841);
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->velY = 0x10000;
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        break;
    default:
        TaskSetFrame((s16)(gUnk_0873D384[gCurTask->player->ability] + 6));
        gCurTask->velY = 0x8000;
        TaskYieldTrampoline(2);
        TaskSetFrame((s16)(gUnk_0873D384[gCurTask->player->ability] + 5));
        TaskYieldTrampoline(2);
        TaskSetFrame((s16)(gUnk_0873D384[gCurTask->player->ability] + 4));
        TaskYieldTrampoline(2);
        TaskSetFrame((s16)(gUnk_0873D384[gCurTask->player->ability] + 3));
        TaskYieldTrampoline(2);
        TaskSetFrame((s16)(gUnk_0873D384[gCurTask->player->ability] + 2));
        gCurTask->velY = 0x10000;
        TaskYieldTrampoline(2);
        TaskSetFrame((s16)(gUnk_0873D384[gCurTask->player->ability] + 1));
        TaskYieldTrampoline(2);
        break;
    }
    k = gCurTask->player->ability;
    if (k == 10)
        TaskSetFrame(0x841);
    else
        TaskSetFrame(gUnk_0873D3B8[k][1]);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velY = 0x30000;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    gCurTask->velY = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void PlayerWarpStarRideState5Update(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->player->playerIndex == gLocalPlayer)
        SetCameraFocus(t->pixelX, t->pixelY);
    TerrainCollideBox(gUnk_0873F5D4);
    if (gCurTask->onGround & 1)
    {
        gCurTask->onGround = 0;
        gCurTask->unk24++;
    }
}

void PlayerWarpStarRideState6(void)
{
    struct PlayerState *p;

    gCurTask->updateState = 6;
    gCurTask->unk24 = 1;
    gCurTask->unk34 = 0;
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    LoadAbilityTiles();
    gCurTask->drawCallback = (u32)sub_0803ddc0;
    gCurTask->layer = 7;
    gCurTask->spriteFlags &= 0x7FFF;
    sub_0807029c();
    switch (gCurTask->player->playerIndex)
    {
    case 0:
        TaskSetMotionXFacing(0x4000, 0x5A5A5A5A);
        break;
    case 1:
        TaskSetMotionXFacing(0x6000, 0x5A5A5A5A);
        break;
    case 2:
        TaskSetMotionXFacing(0x2000, 0x5A5A5A5A);
        break;
    case 3:
        TaskSetMotionXFacing(0x8000, 0x5A5A5A5A);
        break;
    }
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x40000;
    while (gCurTask->unk24 != 2)
        TaskYieldTrampoline(1);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    while (gCurTask->unk24 != 3)
        TaskYieldTrampoline(1);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}

void PlayerWarpStarRideState6Update(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    if (t->player->playerIndex == gLocalPlayer)
        SetCameraFocus(t->pixelX, t->pixelY);
    TerrainCollideBox(gUnk_0873F5D4);
    if (gCurTask->onGround & 1)
    {
        gCurTask->onGround = 0;
        u = gCurTask;
        u->unk24++;
        if (u->unk24 == 2)
            sub_08070264();
    }
    sub_0807042c();
}

void PlayerWarpStarRideState7(void)
{
    struct PlayerState *p;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gCurTask->updateState = 7;
    p = gCurTask->player;
    if (p->ability == ABILITY_STAR_ROD)
        p->unk37 = 3;
    LoadAbilityTiles();
    gCurTask->drawCallback = (u32)sub_0803ddc0;
    gCurTask->layer = 7;
    gCurTask->spriteFlags &= 0x7FFF;
    gCurTask->unk24 = 1;
    gCurTask->unk34 = 0;
    sub_0807029c();
    t = gCurTask;
    switch (t->player->playerIndex)
    {
    case 0:
        t->velX = 0x10000;
        break;
    case 1:
        t->velX = 0x12000;
        break;
    case 2:
        t->velX = 0xE000;
        break;
    case 3:
        t->velX = 0x14000;
        break;
    }
    gCurTask->velY = 0xFFFC0000;
    TaskYieldTrampoline(12);
    gCurTask->velY = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(4);
    u = gCurTask;
    switch (u->player->playerIndex)
    {
    case 0:
        u->velX = 0x8000;
        break;
    case 1:
        u->velX = 0xA000;
        break;
    case 2:
        u->velX = 0x6000;
        break;
    case 3:
        u->velX = 0xC000;
        break;
    }
    gCurTask->velY = 0xFFFF8000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFFE000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0xFFFFF800;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x800;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x2000;
    TaskYieldTrampoline(4);
    gCurTask->velY = 0x8000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(16);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(50);
    TaskStop();
    v = gCurTask;
    v->unk24++;
    v->velY = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->velY = 0xFFFF0000;
    TaskYieldTrampoline(12);
    gCurTask->velY = 0x10000;
    TaskYieldTrampoline(12);
    gCurTask->velY = 0x20000;
    TaskYieldTrampoline(8);
    TaskStop();
    gCurTask->unk24++;
    sub_08070264();
    TaskStop();
    gCurTask->playerLoopCount = 0;
    do
    {
        gCurTask->velX = 0x10000;
        TaskYieldTrampoline(2);
        gCurTask->velX = 0xFFFE0000;
        TaskYieldTrampoline(2);
    } while (++*(s16 *)&gCurTask->playerLoopCount <= 25);
    TaskStop();
    sub_08070614(gCurTaskIdx);
    TaskSleepForever();
}
