#ifndef GUARD_EFFECT_H
#define GUARD_EFFECT_H

#include "gba/types.h"

/* effect.h: the RAM cells and ROM tables of the player effect objects (task
   type #7, M15) and the effect spawner (M16).  One declaration per symbol,
   with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

/* EWRAM */
extern u32 gUnk_020060CC[];
extern u8 gUnk_02006A14[];
extern s8 gUnk_02008010;
extern u16 gUnk_0200AF20[];
extern u16 gUnk_0200B000[]; /* 20 task indices, 0xFFFF = empty; non-volatile, signed reads cast (s16) (variant 37 in effect_57ce0.c) */

/* IWRAM */
extern u16 gUnk_03001570[]; /* palette buffer */

/* ROM */
extern u8 gUnk_081FD870[];
extern u8 gUnk_082030D8[];
extern u32 gUnk_085B9B6C[];
extern void (*gPlayerEffectVariants[])(void); /* task type #7's 49 variants, indexed by Task.unk18 >> 24 */
extern s16 gUnk_0873B9EC[]; /* [6][8]: s16 x, y offsets, 8.8 velocities */
extern u16 gUnk_0873BA4C[][4]; /* [8 + 4][4]: 8.8 velocity x, y, acceleration x, y */
extern s16 gUnk_0873BA8C[][2][3]; /* {base, scale, amount} rows for RandomSpreadFacing */
extern u16 gUnk_0873BAB0[][3]; /* per sub-state: 8.8 x velocity, 8.8 y acceleration, frame */
extern u16 gUnk_0873BAE6[];
extern u16 gUnk_0873BAEE[];
extern u8 gUnk_0873BAFA[];
extern u16 gUnk_0873BAFC[];
extern u16 gUnk_0873BB0E[][3]; /* per sub-state: 8.8 x velocity, 8.8 y velocity, frame */
extern s16 gUnk_0873BB26[];
extern u8 gUnk_0873BB3E[];
extern u16 gUnk_0873BB7E[];
extern u16 gUnk_0873BC3E[];
extern u32 gUnk_0873C038[];
extern u8 gUnk_0873C04C[];
extern u32 gUnk_0873C23C[];
extern u32 gUnk_0873C250[];
extern u8 gUnk_0873C2B4[];
extern s8 gUnk_0873CB74[]; /* collision box passed to sub_0801c3a4 */
extern u32 gUnk_0873CC94[];
extern u32 gUnk_0873CF8C[]; /* hit-box set, passed as (struct HitBoxSet *) */
extern s16 gUnk_0873DBAC[];
extern s16 gUnk_0873DBD4[];
extern u32 gUnk_0873DBE4[];
extern u32 gUnk_0873DC10[];
extern u32 gUnk_0873DC3C[];
extern u8 gUnk_0873DC4C[];
extern u8 gUnk_0873DC66[];
extern u8 gUnk_0873DC80[];
extern u16 gUnk_0873DC9A[];
extern u32 gUnk_0873DCA8[];
extern u32 gUnk_0873DCC0[];
extern u32 gUnk_0873DCC8[];
extern u32 gUnk_0873DCCC[];
extern u32 gUnk_0873DD16[];
extern u32 gUnk_0873DD30[];
extern u32 gUnk_0873DD4C[];
extern u32 gUnk_0873DD5C[];
extern u32 gUnk_0873DD64[];
extern u32 gUnk_0873DD80[];
extern u32 gUnk_0873DDA2[];
extern u32 gUnk_0873DDB4[];
extern u32 gUnk_0873DDBE[];
extern u32 gUnk_0873DDE8[];
extern u32 gUnk_0873DEA0[];
extern u16 gUnk_0873DEA8[];
extern u32 gUnk_0873DEDC[];
extern u32 gUnk_0874C600[];
extern u32 gUnk_0874C648[];
extern u32 gUnk_0874C67C[];
extern u32 gUnk_0874C6F4[];
extern u32 gUnk_0874C718[];
extern u32 gUnk_0874C780[];
extern u32 gUnk_0874C784[];
extern u32 gUnk_0874C7A4[];
extern u32 gUnk_0874C7B4[];
extern u32 gUnk_0874C804[];
extern u32 gUnk_0874C828[];
extern u32 gUnk_0874C890[];
extern u32 gUnk_0874C930[];
extern u32 gUnk_0874C960[];
extern u32 gUnk_0874C980[];
extern u32 gUnk_08751C44[];
extern u32 gUnk_08751C74[];
extern u32 gUnk_08751CBC[];
extern u32 gUnk_08751CEC[];
extern u32 gUnk_08751CF0[];
extern u32 gUnk_08751D50[];
extern u32 gUnk_08751D80[];
extern u32 gUnk_08751D88[];
extern u32 gUnk_08751DB0[];
extern u32 gUnk_08751DBC[];
extern u32 gUnk_08751DD0[];
extern u32 gUnk_08751E00[];
extern u32 gUnk_08751E7C[];
extern u32 gUnk_08751ECC[];
extern u32 gUnk_08751F0C[];
extern u32 gUnk_08751F84[];
extern u32 gUnk_08751FCC[];
extern u32 gUnk_08752020[];
extern u32 gUnk_08752090[];
extern u32 gUnk_08754850[];
extern u32 gUnk_0875488C[];
extern u32 gUnk_087548A0[];

#endif /* GUARD_EFFECT_H */
