#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "subgame.h"

/* subgame_c623c.c (0x080C623C-0x080C6257, issue #98).
 *
 * Sub-game 2: the depth scale.
 * 
 *   sub_080c623c   gUnk_080D059A[x / 4 - 32], the sprite scale for a course
 *       depth (sub_080c3e18, AirGrindEffect, AirGrindScaleSprite).  It follows the
 *       still-asm AirGrindDrawCourse and precedes PR #133's src/sub_080c6258.c. */

s32 sub_080c623c(s32 x)
{
    return gUnk_080D059A[x / 4 - 32];
}
