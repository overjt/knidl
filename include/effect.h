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
extern u32 gUnk_085B9B2C[];
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
extern s16 gGoalGameLayerHeights[];
extern u32 gPlayerGoalGameStates[];
extern u32 gPlayerGoalGameStateUpdates[];
extern u32 gUnk_0873DC3C[];
extern u8 gUnk_0873DC4C[];
extern u8 gUnk_0873DC66[];
extern u8 gUnk_0873DC80[];
extern u16 gGoalGameLayerScores[];
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


/* Functions (defined in the files named above each group). */

/* src/effect_53af4.c */
void Task_PlayerEffect(void);
void sub_08053b40(void);
void sub_08053be0(void);
void sub_08053c1c(void);
void sub_08053c48(void);
void sub_08053d08(void);
void sub_08053db8(void);
void sub_08053e34(void);
void sub_08053e38(void);
void sub_08053f70(void);
void sub_080540d0(void);
void sub_08054298(void);

/* src/effect_54330.c */
void sub_08054330(void);
void sub_08054504(void);
void sub_08054538(void);
void sub_08054838(void);
void PlayerEffectSplash(void);
void sub_080548f0(void);
void PlayerEffectBubble(void);
void sub_08054a44(void);

/* src/effect_54a80.c */
void sub_08054a80(void);
void sub_08054b98(void);
void sub_08054d94(void);
void sub_08054de8(void);
void sub_08054fe4(void);
void sub_080552fc(void);
void sub_080553d4(void);

/* src/effect_55460.c */
void sub_08055460(void);
void sub_08055520(void);
void sub_0805569c(void);
void sub_0805574c(void);
void sub_080557d4(void);
void sub_0805587c(void);
void sub_08055a40(void);
void sub_08055abc(void);

/* src/effect_55b24.c */
void sub_08055b24(void);
void sub_08055d24(void);
void sub_08055d74(void);
void sub_080560fc(void);
void sub_0805614c(void);
void sub_08056300(void);
void sub_08056320(void);
void sub_08056428(void);

/* src/effect_56448.c */
void sub_08056448(void);
void sub_080564ac(void);
void sub_08056770(void);
void sub_08056da8(void);

/* src/effect_56dd4.c */
void sub_08056dd4(void);
void sub_0805707c(void);
void sub_0805710c(void);
void sub_080573a4(void);
void sub_08057430(void);

/* src/effect_57494.c */
void sub_08057494(void);
void sub_08057a10(void);
void sub_08057a48(void);
void sub_08057ad4(void);
void sub_08057c98(void);

/* src/effect_57ce0.c */
void sub_08057ce0(void);
void sub_08057e90(void);
void sub_08057f90(void);
void sub_08058410(void);
void sub_08058460(void);
void sub_080586fc(void);
void sub_08058720(void);

/* src/effect_58810.c */
void sub_08058810(void);
void sub_08058e80(void);
void sub_08058f10(void);
void sub_080594e0(void);

/* src/effect_59570.c */
void sub_08059570(void);
void sub_08059aac(void);
void sub_08059b18(void);
void sub_08059c28(void);
void sub_08059d7c(void);
void sub_0805a320(void);

/* src/effect_5a358.c */
void sub_0805a358(void);
void sub_0805a508(void);
void sub_0805a52c(void);
void sub_0805ab04(void);
void sub_0805ac50(void);
void sub_0805acec(void);
void sub_0805ae00(void);
void sub_0805ae94(void);
void sub_0805af44(void);
void sub_0805af80(void);

/* src/effect_5afac.c */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 CreatePlayerEffectHighSlot(s32 a0, s32 a1, s32 a2);
void GoalGameMain(void);
void GoalGameInit(void);
void PlayerGoalGameInit(void);
void PlayerGoalGameEnterState(void);
void sub_0805b370(void);
s32 PlayerGoalGameUpdate(void);
void sub_0805b4d8(void);
void sub_0805b508(void);
void sub_0805b514(void);
void sub_0805b534(void);
s32 sub_0805b5b0(void);
void sub_0805b61c(void);
void sub_0805b644(void);
void sub_0805b660(void);
void sub_0805b670(void);
void sub_0805b688(void);
void sub_0805b6c0(void);
void sub_0805b788(void);
void sub_0805b83c(void);
void sub_0805b8b8(void);
void sub_0805b8f8(void);
void sub_0805b998(void);
void sub_0805b9a4(void);
void sub_0805b9c0(void);
void sub_0805ba08(void);
void sub_0805bb84(void);
void sub_0805bb90(void);
void sub_0805bc1c(void);
void sub_0805bc50(void);
void sub_0805bc5c(void);
void sub_0805bca4(void);
void sub_0805bce0(void);
void sub_0805bd28(void);
void sub_0805bd34(void);
void sub_0805be3c(void);
void sub_0805be48(void);
void Task_GoalGameLaunchStars(void);
void GoalGameLaunchStarsFall(void);
void GoalGameLaunchStarsUpdate(void);
void GoalGameLaunchStarsDraw(void);
void Task_GoalGameBigTrailStar(void);
void Task_GoalGameSmallTrailStar(void);
void sub_0805c584(void);
void Task_GoalGameCamera(void);
void GoalGameCameraFollowPlayer(void);
void GoalGameCameraUpdate(void);
void Task_GoalGameSpring(void);
void Task_GoalGamePlayerMarker(void);
s32 GoalGamePlayerMarkerFollowParent(void);
void Task_GoalGameSign(void);
void sub_0805ceec(void);
void Task_GoalGameHelperKirby(void);
void GoalGameHelperKirbyUpdate(void);
void sub_0805d564(void);
void sub_0805d5fc(void);
void Task_GoalGameOneUp(void);
void TaskStartFrameScript(s32 a0);
void TaskStartFrameScriptId(s32 a0);
void TaskUpdateFrameScript(void);
void TaskAdvanceFrameScript(void);
void sub_0805d994(s32 a0, s32 a1);
void sub_0805da2c(void);
void sub_0805dba0(void);
void sub_0805dbfc(void);
void sub_0805dc18(void);
void sub_0805dd4c(void);
void sub_0805dd88(void);
void sub_0805ddb0(s32 a0);
void sub_0805deac(void);
void sub_0805df9c(void);
void sub_0805dfe8(void);
void sub_0805e038(s32 a0);
void sub_0805e110(s32 a0);
void sub_0805e15c(void);
void sub_0805e1bc(void);
void sub_0805e24c(void);
void sub_0805e2d4(void);
void sub_0805e7b4(void);
void sub_0805eb2c(s32 a0, s32 a1, s32 a2);
void sub_0805ee90(void);
void sub_0805f1bc(void);
void sub_0805f778(void);
void sub_0805fb88(void);
void sub_08060308(void);
void sub_08060c2c(void);
void sub_080613e4(void);
void sub_08061cac(void);

#endif /* GUARD_EFFECT_H */
