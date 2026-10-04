#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "subgame.h"

/* subgame_c623c.c (0x080C623C-0x080C6257, issue #98).
 *
 * Sub-game 2: the depth scale.
 * 
 *   AirGrindGetDepthScale   gAirGrindDepthScales[x / 4 - 32], the sprite scale for a course
 *       depth (AirGrindRacerUpdateScreenPos, AirGrindEffect, AirGrindScaleSprite).  It follows the
 *       still-asm AirGrindDrawCourse and precedes PR #133's src/sub_080c6258.c. */

s32 AirGrindGetDepthScale(s32 x)
{
    return gAirGrindDepthScales[x / 4 - 32];
}
