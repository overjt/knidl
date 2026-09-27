#ifndef GUARD_ENDING_H
#define GUARD_ENDING_H

#include "gba/types.h"

/* ending.h: the RAM cells and ROM tables of the ending: AgbMain state 11, the
   final results, the staff credits, the boot logo objects and the game-over
   screen (M37-M38).  One declaration per symbol, with the type its consumers
   prove (issue #36 phase 2, docs/header-conventions.md). */

struct GfxHeader;

/* EWRAM */
extern u16 gUnk_02004C94;
extern s16 gGameOverTimer; /* game-over screen: frames left */
extern s8 gGameOverCursor; /* game-over screen: cursor (continue = 0?) */
extern u16 gUnk_0200616C;
extern u8 gGameOverDone; /* game-over screen: done flag */
extern s16 gGameOverPlayerTask; /* game-over screen: the #264 variant-0 task's index */
extern u16 gUnk_02007D3C;
extern u8 gEndingSceneActive;
extern s16 *gUnk_0201BFD0[];
extern u8 gUnk_0201C19C;
extern s32 gUnk_0201C1A0; /* credits: BG0 vertical scroll, 16.16 */
extern s32 gUnk_0201C1A4; /* credits: the score saved over the demos */
extern u8 gUnk_0201C1A8;
extern s32 gUnk_0201C1AC; /* credits: BG0 horizontal scroll, 16.16 */
extern u8 gUnk_0201C1B0; /* credits: current demo scene */
extern s32 gUnk_0201C1B4; /* credits: scroll since the last page copy, 1/16 pixel */

/* IWRAM */
extern u16 gUnk_030014F0[];

/* ROM */
extern u32 gUnk_080DBEF8[];
extern u32 gUnk_080DBF00[];
extern u32 gUnk_080DBF08[];
extern u32 gUnk_080DBF18[];
extern u32 gUnk_080DBF28[];
extern u32 gUnk_080DBF30[];
extern u16 gUnk_08584BB0[][4];
extern struct GfxHeader gUnk_085995AC;
extern struct GfxHeader gUnk_0859990C;
extern struct GfxHeader gUnk_0859A09C;
extern u16 gUnk_0859A0B0[];
extern u16 gUnk_0859A0D0[];
extern u16 gUnk_0859A0F0[];
extern u16 gUnk_0859A110[];
extern u16 gUnk_085E0070[];
extern u32 gUnk_085E0090[];
extern u32 gUnk_085E2C20[];
extern u32 gUnk_085E2CE0[];
extern u32 gUnk_085E4064[];
extern u32 gUnk_085E5BC4[];
extern u32 gUnk_0874CEE8[];
extern u32 gUnk_0874CF28[];
extern u32 gUnk_0874CF94[];
extern u32 gUnk_087548A8[];
extern u32 gUnk_087548B8[];
extern u32 gUnk_08754908[];
extern u32 gUnk_08754914[];
extern u32 gUnk_08754984[];
extern u32 gUnk_087549B0[];
extern u32 gUnk_087549FC[];
extern u32 gUnk_087554B8[];
extern u32 gUnk_087556E0[];
extern u32 gUnk_08755708[];
extern u32 gUnk_0875581C[];
extern u32 gUnk_0875585C[];
extern void (*gUnk_08757330[])(void);
extern u16 gUnk_0875735C[];
extern s32 gUnk_08757374[];
extern s32 gUnk_08757394[];
extern s32 gUnk_087573B4[];
extern s32 gUnk_087573D4[];
extern void (*gUnk_087573F4[])(void);
extern u16 gUnk_08757424[];
extern u16 gUnk_08757432[];
extern u16 gUnk_0875743E[];
extern s16 gUnk_08757440[];
extern s16 *gUnk_087577D8[];
extern u16 gUnk_08758274[];
extern u16 gUnk_08758284[];
extern void (*gGameOverObjectVariants[])(void);
extern void (*gUnk_087582AC[])(void);
extern void (*gUnk_087582B8[])(void);
extern void (*gUnk_087582C4[])(void);
extern void (*gUnk_087582DC[])(void);
extern s32 gUnk_087582F4[];
extern void (*gUnk_08758324[])(void);
extern void (*gUnk_0875832C[])(void);
extern u16 gUnk_08758334[];
extern u16 gUnk_08758374[];
extern u32 *gUnk_087583B4[]; /* credits: the 14 compressed text pages */
extern u32 gUnk_087583CC[][8]; /* credits: per variant, the scenes' recorded demos, 0-terminated */
extern u16 gUnk_0875841E[][7]; /* credits: per variant, the scenes' lengths in frames */

#endif /* GUARD_ENDING_H */
