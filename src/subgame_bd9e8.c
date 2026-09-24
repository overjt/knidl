/* game_code_and_rodata_080653ec_0806ef5c 0x080BD9E8-0x080BDA2C
 * (issue #95, module M35, file 5 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BD9E8 0x080BDA2C src/subgame_bd9e8.c --newpb
 *
 * The bomb-pass game's two framework hooks (sub-game 1 of the tables in
 * src/subgame_b9d0c.c): the init hook that clears the elimination state
 * M36 keeps in gUnk_0200AFF0 / gUnk_0200AF10 / gUnk_0200B044[], and the task
 * body that dispatches the game's phase through 0x08756568.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u8 gUnk_02007D2C;
extern u8 gUnk_0200AF10;
extern u8 gUnk_0200AFF0;
extern u8 gUnk_0200B044[];
extern u32 gUnk_08756568[];

void sub_08002e98(u32 a, u32 b, u32 *c);
void sub_08006138(void);

void sub_080bd9e8(void)
{
    gUnk_0200AFF0 = 0;
    gUnk_0200AF10 = 0;
    gUnk_0200B044[3] = 3;
    gUnk_0200B044[2] = 3;
    gUnk_0200B044[1] = 3;
    gUnk_0200B044[0] = 3;
}

void sub_080bda0c(void)
{
    sub_08002e98(gUnk_02007D2C, 2, gUnk_08756568);
    sub_08006138();
}
