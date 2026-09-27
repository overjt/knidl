/* game_code_and_rodata_080653ec_0806ef5c 0x080BD9E8-0x080BDA2C
 * (issue #95, module M35, file 5 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BD9E8 0x080BDA2C src/subgame_bd9e8.c --newpb
 *
 * The bomb-pass game's two framework hooks (sub-game 1 of the tables in
 * src/subgame_b9d0c.c): the init hook that clears the elimination state
 * M36 keeps in gBombRallyOutCount / gBombRallyOutMask / gBombRallyFinishOrder[], and the task
 * body that dispatches the game's phase through 0x08756568.  That table sits
 * among M36's own rodata (0x08756528-0x08756564, 0x08756570 on), so these two
 * probably belong to M36's translation unit; they are a file of their own
 * only because M36's carve starts at 0x080BDA2C.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "subgame.h"

void CallTableEntry(u32 a, u32 b, u32 *c);
void TaskSleepForever(void);

void BombRallyInit(void)
{
    gBombRallyOutCount = 0;
    gBombRallyOutMask = 0;
    gBombRallyFinishOrder[3] = 3;
    gBombRallyFinishOrder[2] = 3;
    gBombRallyFinishOrder[1] = 3;
    gBombRallyFinishOrder[0] = 3;
}

void BombRallyMain(void)
{
    CallTableEntry(gSubGamePhase, 2, gBombRallyPhases);
    TaskSleepForever();
}
