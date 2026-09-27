#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"

/* External functions */
extern void TaskExitTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void ResetFadeAndBlend(void);
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern u32 RandomRange(u32 range);
extern void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 PlayBgm(s32 songId);
extern void sub_08003184(void);
extern s32 PlaySfx(s32 id);
extern void StopBgm(void);
extern void StopSfxOnPlayer(s32 player, s32 songId);
extern void FadeOutBgm(s32 speed);
extern void SetBgmVolume(u16 volume);
extern void TaskSetSkipMask(u8 val, s32 idx);
extern void TaskFree(s32 id);
extern void TaskIntegrateMotion(void);
extern void TaskMove(void);
extern void TaskMoveRelativeToParent(void);
extern u32 TaskIsOnScreen(void);
extern void TaskDrawWorld(void);
extern void TaskSleepForever(void);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskSetMotionX(s32 a, s32 b, s32 c);
extern void TaskSetMotionXFacing(s32 a, s32 b);
extern void TaskSetMotionY(s32 a, s32 b, s32 c);
extern void TaskStopY(void);
extern void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void TaskStop(void);
extern void TaskUpdateFlip(void);
extern void TaskSetFrame(s32 a);
extern void TaskSetFrameNoFlip(s32 a);
extern void TaskSetFrameFlip(s32 a);
extern s32 AddPlayerHealth(s32 a, s32 b);
extern void SetPlayerAbilityNoHud();
extern void HudShowHpBar(void);
extern void HudStartHpBar();
extern void HudSetTaskHpBar();
extern void HudRemoveHpBar(void);
extern void sub_0800a698(void);
extern s32 GetCollisionTileAtOffset(s16 x, s16 y, s32 c, s32 d);
extern void TaskInitWaterFlags(void);
extern u8 ClampTaskToRoom(struct Task *t);
extern u32 sub_0802294c();
extern void ExitClearedStage();
extern void sub_08025a30();
extern void sub_08025acc();
extern void sub_08025b5c();
extern void RequestScreenShake(u32 a);
extern void sub_080275cc();
extern void StartScrollLock();
extern s32 CreateMapEvent();
extern void sub_0802ffe8();
extern void TaskBreakBlocksNoPlayer();
extern void sub_08030db8();
extern void sub_0803e68c();
extern void sub_08040858();
extern void sub_08040894();
extern void sub_0805ddb0();
extern void sub_0805deac();
extern void sub_0805e110();
extern s32 sub_08063698(u32 type, s32 start);
extern void ActorLoadDef(struct ActorDef *d);
extern void ActorLoadDefSlot(u32 i, struct ActorDef *d);
extern void ActorSetState();
extern void ActorSetStateSlot(u32 i, u16 v);
extern void ActorSetHitReactions(u32 v);
extern void ActorSetAttackBox(u32 v);
extern void ActorSetAttackBoxSlot(u32 i, u32 v);
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
extern void sub_08063a14(u32 i, u32 v);
extern s32 TaskFindNearestPlayer(void);
extern s32 TaskGetDxTo(u32 i);
extern s32 TaskGetNearestPlayerDx(void);
extern s32 TaskGetDyTo(u32 i);
extern s32 TaskGetNearestPlayerDy(void);
extern void TaskGetPosSlot(u32 i);
extern void TaskGetNearestPlayerPos(void);
extern s32 TaskGetFacingToward(u32 i);
extern void TaskFaceNearestPlayer(void);
extern s32 TaskIsInRectSlot(struct Rect *r, u32 i);
extern void ActorDestroySlot(s32 i);
extern void ActorDestroy(void);
extern void TaskTurnAroundAndReverseX(void);
extern void TaskToggleFacingAndReverseX(void);
extern s32 ActorStartAnimNoFlip(struct AnimCmd *p);
extern void ActorStopAnim(void);
extern s32 ActorStartAnim(struct AnimCmd *p);
extern s32 ActorTickAnimFacingNearestPlayer(s32 n);
extern s32 ActorTickAnim(s32 n);
extern void AngleToVector(s16 t, s16 mag);
extern u16 TaskGetAngleTo(u32 i, s32 prec);
extern u16 TaskGetAngleToNearestPlayer(s32 prec);
extern u8 TaskGetYDirBitTo(u32 i);
extern u8 TaskGetYDirBitToNearestPlayer(void);
extern u8 TaskGetXDirBitTo(u32 i);
extern u8 TaskGetXDirBitToNearestPlayer(void);
extern void TaskAccelerateTowardNearestPlayer(s32 step, s32 limit);
extern void TaskAccelerateInDir(s32 step, s32 limit, u16 dir);
extern s32 TaskFindNearestPlayerInScreenXBand(u16 lo, u16 hi);
extern s32 TaskFindNearestPlayerInScreenYBand(u16 lo, u16 hi);
extern void TaskGetScreenPosSlot(u32 i);
extern s32 TaskGetNearestPlayerScreenPos(void);
extern void TaskGetScreenPos(void);
extern void TaskFaceLikeParent(void);
extern s32 CreateActorFromDescHere(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDescAtOffsetFacing(struct ActorSpawn *p, u8 keepPrio);
extern s32 CreateActorFromDesc(struct ActorSpawn *p, u8 keepPrio);
extern void sub_08064bcc(void);
extern s32 CreateChildTask(u32 type, int xArg, int yArg, int prioArg);
extern s32 CreateChildTaskAtOffsetFacing(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 CreateChildTaskHere(u32 type, u8 keepPrio);
extern s32 CreateChildTaskAt(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 CreateActorByKind(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 sub_0806505c(u8 p3, u8 p4, u32 x, u32 y, u16 prio);
extern u8 ActorIsInView(void);
extern void ActorDrawWorldInView(void);
extern void ActorDrawWorldInViewOrDestroy(void);
extern void sub_080653ec(void);
extern void sub_08065438(void);
extern void sub_08065640(void);
extern void ActorMove(void);
extern void TaskMoveRelativeToView(void);
extern void ResetBgPaletteBlend(void);
extern void EndBgPaletteBlend(u32 v);
extern void StartBgPaletteBlend(u32 a, u32 b);
extern void sub_08065dbc(u32 slot, u32 sub);
extern void sub_08065dd0(u32 slot, u32 i);
extern void sub_08065dfc(u32 slot);
extern u8 TaskHasSameSerial(u32 i);
extern s16 ActorComputeHealth(void);
extern u16 sub_08066088(u32 mode);
extern void sub_08066144(void);
extern void sub_0806619c(u32 p0, u32 p1, u32 p2, u16 p3, u8 p4);
extern void sub_0806621c(void);
extern s32 sub_08066394(void);
extern void sub_080664e0(struct AnimCmd *p);
extern void sub_08066544(void);
extern void sub_08066564(void);
extern void sub_08066580(void);
extern u16 sub_0806660c(u16 a);
extern u16 sub_08066630(u16 a);
extern void sub_080666f8(struct AnimCmd *p);
extern u32 sub_08066718(void);
extern void ActorLoadPalette(void *src, u32 size, u8 force);
extern u8 sub_08066a6c(void);
extern void sub_08066f50(s32 x, s32 y);
extern void sub_08066fc0(u8 p3, s16 x, s16 y);
extern void sub_0806704c(void);
extern void sub_080670ac(u16 a);
extern void sub_080670d4(void);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern s32 sub_08067120(s16 x, s16 y, s16 dir, u8 p8);
extern void sub_080685ec(s32 i, s32 j, u8 c);
extern void sub_08068920(s32 i, u8 c);
extern void sub_08068950(s16 x, s16 y, s16 d);
extern u32 ActorCheckHitsWithBox(s32 a);
extern u32 ActorCheckHits(void);
extern u32 sub_08068f68(void);
extern u32 ActorCollideTerrain(void);
extern u32 sub_0806951c(void);
extern u32 sub_0806956c(void);
extern u32 sub_080695bc(void);
extern u32 sub_080696a0(void);
extern u32 sub_08069888(void);
extern u32 ActorReactToHit(void);
extern u32 sub_08069b84(void);
extern u32 sub_08069bbc(void);
extern u32 ActorReactToDefeat(void);
extern void ActorDie(void);
extern void sub_0806b05c(void);
extern void sub_0806b098(void);
extern s16 CreateStarFlash(u8 kind, s32 dx, s32 dy);
extern s16 CreateDustTrail(u8 flag, u16 vx, s32 c, s32 d);
extern void CreateBurstEffect(u32 a, s32 b);
extern void PlayExplosionAnim(void);
extern s32 sub_0806e6f8(s16 x, s16 y);
extern s32 sub_0806e808(s16 x, s16 y);
extern s32 sub_0806e9b4(u8 a, s16 x, s16 y);
extern void sub_080a1570(void);
extern void sub_080b7cb4();

/* Module functions */
void sub_080a1590();
void sub_080a15f0();
s32 sub_080a1618();
void sub_080a1624();
void sub_080a1668();
void sub_080a1704();
void sub_080a1740();
void sub_080a1790();
void sub_080a180c();
void sub_080a1864();
void sub_080a18d4();
void sub_080a1980();
void Task_MrShineAndMrBright();
void sub_080a19ec();
void CreateMrShineAndMrBright();
void sub_080a1b94();
void sub_080a1bd8(u16 a, u16 b);
void sub_080a1c1c();
void sub_080a1c90();
void sub_080a1d2c();
void sub_080a1d84();
s32 sub_080a1dbc(s32 a);
void sub_080a1dd4();
s32 sub_080a1df8();
s32 sub_080a1e4c();
s32 sub_080a1ec4();
void sub_080a1f90();
s32 sub_080a1fc8();
void sub_080a2020();
void sub_080a2030();
void sub_080a2090();
void sub_080a2164();
void sub_080a21a0();
s32 sub_080a2224();
void sub_080a2274();
s32 sub_080a22b0(void);
void sub_080a22d4();
s32 sub_080a2390();
void sub_080a23c0();
void sub_080a2400();
void sub_080a2448();
void sub_080a2488();
s32 sub_080a24f8(void);
void sub_080a2590();
void sub_080a25c4();
void sub_080a2608();
void sub_080a263c();
void sub_080a268c();
void sub_080a2754();
void sub_080a27b8();
void sub_080a2814();
void sub_080a28d0();
void sub_080a291c();
void sub_080a2954();
void sub_080a2994();
void sub_080a29cc();
void sub_080a2a00();
void sub_080a2a24();
void sub_080a2a44();
void sub_080a2a94();
void sub_080a2af4();
void sub_080a2b14();
void sub_080a2b2c();
void sub_080a2bc4();
void sub_080a2c90();
void sub_080a2ca8();
void sub_080a2ce8();
void sub_080a2d38();
s32 sub_080a2de0();
void sub_080a2e88(u8 a);
void sub_080a2edc();
void sub_080a2f38();
void sub_080a3000();
void sub_080a301c();
void sub_080a306c();
void sub_080a30d0();
void MrShineAndMrBrightInit();
void MrShineAndMrBrightUpdate();
void MrShineAndMrBrightEnterState();
void sub_080a3184();
void sub_080a31a4();
void sub_080a31d0();
void sub_080a31f0();
void sub_080a31f4();
void sub_080a3238();
void sub_080a3250();
void sub_080a3268();
void sub_080a3280();
void MrShineInit();
void MrShineUpdate();
void MrShineEnterState();
void MrShineFall();
void sub_080a33a4();
void sub_080a33dc();
void sub_080a3424();
void sub_080a346c();
void sub_080a348c();
void sub_080a349c();
void sub_080a34b8();
void sub_080a34c8();
void sub_080a34f0();
void sub_080a352c();
void sub_080a3548();
void sub_080a3588();
void sub_080a35a8();
void sub_080a35b8();
void sub_080a35d8();
void sub_080a35e8();
void sub_080a361c();
void sub_080a3628();
void sub_080a368c();
void sub_080a36c8();
void sub_080a3768();
void sub_080a37ac();
void sub_080a3840();
void sub_080a387c();
void sub_080a38ec();
void sub_080a3954();
void sub_080a3a10();
void sub_080a3a40();
void sub_080a3b10();
void sub_080a3b4c();
void sub_080a3b78();
void sub_080a3b7c();
void sub_080a3ba0();
void sub_080a3bc0();
void sub_080a3c08();
void MrBrightInit();
void MrBrightUpdate();
void MrBrightEnterState();
void MrBrightFall();
void sub_080a3d3c();
void sub_080a3d84();
void sub_080a3dd0();
void sub_080a3e10();
void sub_080a3e3c();
void sub_080a3e60();
void sub_080a3e7c();
void sub_080a3ea0();
void sub_080a3edc();
void sub_080a3f24();
void sub_080a3f54();
void sub_080a3fb8();
void sub_080a3fd4();
void sub_080a3ff8();
void sub_080a4018();
void sub_080a403c();
void sub_080a406c();
void sub_080a408c();
void sub_080a412c();
void sub_080a4180();
void sub_080a4200();
void sub_080a4260();
void sub_080a42f8();
void sub_080a4350();
void sub_080a43d8();
void sub_080a4430();
void sub_080a44c4();
void sub_080a4508();
void sub_080a45bc();
void sub_080a4604();
void sub_080a4630();
void sub_080a4634();
void sub_080a4658();
void sub_080a4678();
void sub_080a46c0();
void sub_080a46e0();
void sub_080a4708();
void sub_080a472c();
void sub_080a4808();
void sub_080a4814();
void sub_080a488c();
void Task_KingDededeStar();
void sub_080a49cc();
void sub_080a4a1c();
void sub_080a4a60();
void sub_080a4aa8();
void sub_080a4ac4();
void sub_080a4b1c();
void sub_080a4b68();
void sub_080a4ba8();
void sub_080a4bdc();
void sub_080a4c20();
void sub_080a4c3c();
void sub_080a4c80();
void sub_080a4c84();
void sub_080a4cc4();
void sub_080a4d00();
void sub_080a4d6c();
void sub_080a4d88();
void sub_080a4dd8();
void sub_080a4df4();
void sub_080a4e10();
void sub_080a4e14();
void sub_080a4e9c();
void sub_080a4ee0();
void sub_080a4f24();
void sub_080a4f40();
void sub_080a5008();
void sub_080a5020();
void sub_080a503c();
void sub_080a5040();
void sub_080a5084();
void sub_080a50f0();
void sub_080a510c();
void sub_080a5164();
void sub_080a5168();
void sub_080a5184();
void sub_080a5188();
void sub_080a51dc();
void sub_080a5220();
void sub_080a523c();
void sub_080a528c();
void sub_080a52c8();
void sub_080a5304();
void sub_080a5320();
void sub_080a5324();
void sub_080a5388();
void sub_080a53a8();
void sub_080a5484();
void sub_080a54a4();
void sub_080a54e4();
void sub_080a5524();
void sub_080a556c();
void sub_080a55ac();
void Task_MetaKnight();
void MetaKnightInit();
void MetaKnightEnterState();
void MetaKnightUpdate();
void sub_080a57d4();
void sub_080a5a78();
void sub_080a5aa0();
void sub_080a5af4();
void sub_080a5dd0();
void sub_080a5e30();
void sub_080a5e60();
void sub_080a5e9c();
void sub_080a5ecc();
void sub_080a5ef0();
void sub_080a5f20();
void sub_080a5f7c();
void sub_080a5fac();
void sub_080a6020();
void MetaKnightRun();
void sub_080a6130();
void sub_080a61b4();
void sub_080a6264();
void sub_080a6280();
void sub_080a6330();
void sub_080a634c();
void sub_080a638c();
void sub_080a63a4();
void sub_080a63d8();
void sub_080a63f0();
void sub_080a6420();
void sub_080a6438();
void sub_080a650c();
void sub_080a6544();
void sub_080a6574();
void sub_080a658c();
void sub_080a6628();
void sub_080a6650();
void sub_080a6680();
void sub_080a6698();
void sub_080a66c8();
void sub_080a6710();
void sub_080a6734();
void sub_080a6754();
void sub_080a680c();
void sub_080a6868();
void sub_080a6988();
void sub_080a69e4();
void sub_080a6aac();
void sub_080a6b3c();
void sub_080a6c3c();
void sub_080a6d08();
void sub_080a6d60();
s32 sub_080a6e1c(void);
void sub_080a6e58();
void sub_080a6e98();
s32 sub_080a6f38(s16 x, s16 y);
s32 sub_080a6f74();
void sub_080a6fb4();
void sub_080a7080();
void sub_080a7168();
void sub_080a7190();
s32 sub_080a720c();
s32 sub_080a7260();
void sub_080a72b0();
s32 sub_080a72fc();
s32 sub_080a7334();
void sub_080a73b4();
void sub_080a73fc();
void sub_080a741c();
void sub_080a7438();
void sub_080a75a0();
void sub_080a75c8();
void sub_080a787c();
void sub_080a7880();
void sub_080a78a0();
void sub_080a7998();
void sub_080a7ae4();
void sub_080a7b88();
void sub_080a7c9c();
s32 sub_080a7d20();
void Task_Kracko();
void sub_080a7d98();
void sub_080a7df4();
void sub_080a7e10();
void sub_080a7e44();
void sub_080a8038();
void sub_080a85e4();
void sub_080a860c();
void sub_080a87c8();
void sub_080a8834();
void sub_080a8850();
void sub_080a8878();
void sub_080a8948();
void sub_080a8970();
void sub_080a8b44();
void sub_080a8b6c();
void sub_080a8bcc();
void sub_080a8bf4();
void sub_080a8c84();
void sub_080a8d1c();
void sub_080a8f18();
void sub_080a8f40();
void sub_080a8fb4();
void sub_080a8fdc();
void sub_080a9304();
void sub_080a932c();
s32 sub_080a93ec();
void sub_080a9434();
void sub_080a94d4();
s32 sub_080a95dc();
void sub_080a96a4();
void sub_080a96dc();
void sub_080a9738();
s32 sub_080a9760();
void sub_080a9794();
s32 sub_080a97d8();
void sub_080a9814();
void sub_080a983c();
void sub_080a9910();
void sub_080a99a0();
void sub_080a9ba0();
void sub_080a9c28();
void sub_080a9cf0();
void sub_080a9da4();
void sub_080a9e14();
s32 sub_080a9e88(s32 a);
void sub_080a9ea4();
void sub_080a9ed8();
void sub_080a9ef4();
void sub_080aa16c();
void sub_080aa188();
void Task_NightmareWizard();
void NightmareWizardInit();
void NightmareWizardEnterState();
void NightmareWizardUpdate();
void sub_080aa47c();
void sub_080aa52c();
void sub_080aa560();
void sub_080aa62c();
void sub_080aa67c();
void sub_080aa6a8();
void sub_080aa6d0();
void sub_080aa71c();
void sub_080aa744();
void sub_080aa970();
void sub_080aa998();
void sub_080aab4c();
void sub_080aab80();
void sub_080aad64();
void sub_080aad98();
void sub_080aaf38();
void sub_080aaf6c();
void sub_080ab158();
void sub_080ab1a8();
void sub_080ab394();
void sub_080ab3c8();
void sub_080ab418();
void sub_080ab440();
void sub_080ab46c();
void sub_080ab4a0();
void sub_080ab4d0(s32 a);
void sub_080ab570();
void sub_080ab5c0();
void sub_080ab670(s32 a);
void sub_080ab728(s32 a);
void sub_080ab7cc();
void sub_080ab810();
s32 sub_080ab854();
s32 sub_080ab8a8();
void sub_080ab93c();
void sub_080abcb4();
void sub_080abcd0(s32 x, s32 y, s32 d);
void sub_080abd04();
void sub_080abe38();
void sub_080abe7c();
void sub_080abf10();
void sub_080abf94();
void sub_080ac020();
void sub_080ac08c();
void sub_080ac124();
void sub_080ac1f0();
void sub_080ac27c();
void sub_080ac30c();
void sub_080ac3a4();
void sub_080ac410();
void sub_080ac47c();
void sub_080ac510();
void sub_080ac530();
s32 sub_080ac678();
void sub_080ac684();
void sub_080ac72c();
void sub_080ac82c();
void sub_080ac84c();
void sub_080ac868();
void sub_080ac94c();
void sub_080ac950();
void sub_080aca38();
void sub_080aca3c();
void sub_080aca60();
void sub_080aca90();
void sub_080acaf0();
void sub_080acb20();
void sub_080acc18();
void sub_080acc8c();
void sub_080acc9c();
void sub_080accf0();
void sub_080acd38();
void sub_080ace60();
void Task_PaintRoller();
void sub_080acf3c();
void sub_080acf48();
void sub_080acf68();
void sub_080acffc();
void sub_080ad08c();
void sub_080ad0f8();
void sub_080ad128();
s32 sub_080ad13c();
void sub_080ad160();
void sub_080ad170();
void sub_080ad278();
void sub_080ad32c();
s32 sub_080ad37c(void);
void sub_080ad3a0();
void sub_080ad3e8();
void sub_080ad458();
void sub_080ad47c();
void Task_HeavyMole();
void sub_080ad630();
void sub_080ad650();
void sub_080ad710();
void sub_080ad788();
void sub_080ad7f0();
void sub_080ad9dc();
void sub_080ada20();
s32 sub_080adaf8();
void sub_080adb58();
void sub_080adb90();
s32 sub_080adbf0();
void sub_080adc44();
void sub_080adca4();
void sub_080add48();
void sub_080addf8();
void sub_080ade98();
void sub_080adebc();
void sub_080adefc();
void sub_080adf50();
void sub_080adfd4();
void sub_080adff8();
void sub_080ae038();
void sub_080ae0b4();
void sub_080ae174();
void sub_080ae198();
void sub_080ae1f0();
void sub_080ae37c();
s32 sub_080ae380(s32 i);
void Task_NightmarePowerOrb();
void sub_080ae470();
void sub_080ae4c4();
void sub_080ae548();
void sub_080ae628();
void sub_080ae79c();
void sub_080ae7ec();
void sub_080ae870();
void sub_080ae8e0();
void sub_080aec00();
void sub_080aed5c();
s32 sub_080aeeb0();
void sub_080aeef8();
void sub_080aef30();
void sub_080aef50();
void sub_080aef5c();
void sub_080aefd4();
void sub_080aeff8();
void sub_080af020();
s32 sub_080af100(void);
void sub_080af114();
void sub_080af144();
void sub_080af178();
void sub_080af188();
void sub_080af1b8();
void sub_080af1c8();
void sub_080af1d4();
void sub_080af20c(u8 a);
void sub_080af26c();
void sub_080af278();
void sub_080af294();
void sub_080af308();
void sub_080af30c();
void sub_080af358();
void sub_080af38c();
void sub_080af4a4();
void sub_080af5bc();
void sub_080af6c8();
void sub_080af7d4();
void sub_080af844();
void sub_080af938();
void sub_080af9e4();
void sub_080afaa4();
void sub_080afb50();
void sub_080afc10();
void sub_080afcd4();
void sub_080afd9c();
void sub_080afdf0();
void sub_080aff40();
void sub_080b0144();
void sub_080b0338();
void sub_080b0570();
void sub_080b05e8();
void sub_080b07d8();
void Task_NightmarePowerOrbIntroScroll();
void sub_080b08a0();
void sub_080b08e4();
void sub_080b09ac();
void sub_080b0b04();
void sub_080b0b50();
void sub_080b0cc8();
void sub_080b0de4();
void sub_080b0e80();
void sub_080b0f04();
void sub_080b0f98();
void sub_080b102c();
void sub_080b10c8();
void sub_080b123c();
void sub_080b1264();
void sub_080b1398();
void sub_080b13bc();
void sub_080b14ac();
void sub_080b14fc();
void sub_080b1564();
void sub_080b1578();
s32 sub_080b1588();
s32 sub_080b1594();
s32 sub_080b15b4();
void sub_080b15c0();
void sub_080b1710();
void sub_080b1770();
void sub_080b17d0();
void sub_080b1830();
void sub_080b1890();
void sub_080b1910();
void sub_080b199c();
void sub_080b1a00();
void sub_080b1a1c();
void sub_080b1b2c();
void sub_080b1c04();
void sub_080b1d2c();
void sub_080b1d98();
void sub_080b1e20();
void sub_080b1ef8();
void sub_080b1f80();
void sub_080b2058(u8 a);
void sub_080b20d4();
void sub_080b214c();
void sub_080b21a0();
void sub_080b2228();
void sub_080b2294();
void sub_080b22b8();
void sub_080b22f8();
void sub_080b2418();
void sub_080b2538();
void sub_080b2550();
void sub_080b2574();
void sub_080b25a4();
void sub_080b25e8();
void sub_080b2664();
void sub_080b2768();
void sub_080b27b0();
s32 sub_080b2804();
void sub_080b2884();
void sub_080b2890();
void Task_WhispyWoods();
void WhispyWoodsInit();
void WhispyWoodsUpdate();
void WhispyWoodsEnterState();
void sub_080b2ae4();
void sub_080b2b28();
void sub_080b2b40();
void sub_080b2bf0();
void sub_080b2c18();
void sub_080b2cc8();
void sub_080b2cf0();
void sub_080b2d80();
void sub_080b2dd4();
void sub_080b2e20();
void sub_080b2e3c();
void sub_080b2e58();
void sub_080b2f34();
s32 sub_080b2f38();
void sub_080b2f78();
void sub_080b2fb0();
void sub_080b2fe8();
void sub_080b3010(u8 a);
void Task_WhispyWoodsApple();
void sub_080b3090();
void sub_080b30c8();
void sub_080b3110();
void sub_080b312c();
void sub_080b319c();
void sub_080b31a0();
void sub_080b31e0();
void sub_080b3214();
void sub_080b3258();
void sub_080b328c();
void sub_080b32d0();
void Task_WhispyWoodsAirPuff();
void sub_080b3368();
void sub_080b3398();
void sub_080b33bc();
void sub_080b33d8();
void sub_080b3758();
void sub_080b37ec();
void sub_080b38f0();
void sub_080b3a00();
void sub_080b3a64();
void sub_080b3c68();
void sub_080b3e30();
void Task_OneUp();
void Task_MaximTomato();
void Task_InvincibleCandy();
void Task_EnergyDrink();
void sub_080b3f54();
void sub_080b3fcc();
void sub_080b3ffc();
s32 sub_080b404c();
s32 sub_080b406c();
s32 sub_080b408c();
void PickupInit();
void PickupUpdate();
void PickupEnterState();
void sub_080b4174();
void sub_080b4190();
void sub_080b4194();
void sub_080b41c8();
void sub_080b41cc();
void sub_080b4200();
s32 sub_080b4204();
void PickupHeal();
void MaximTomatoHeal();
void EnergyDrinkHeal();
s32 sub_080b4390();
s32 sub_080b43d4();
s32 sub_080b43f4();
s32 sub_080b442c();
void sub_080b447c();
void sub_080b44f0();
s32 sub_080b4524();
s32 sub_080b45c0();
void sub_080b460c();
void sub_080b4648();
void Task_AbilityStar();
void sub_080b4714();
void sub_080b4754();
void sub_080b4770();
void sub_080b4788();
void sub_080b4794();
void sub_080b47c0();
void sub_080b47cc();
void sub_080b480c();
void sub_080b4878();
void sub_080b48e0();
void sub_080b48f8();
void sub_080b4968();
void sub_080b4a34();
void sub_080b4a5c();
void sub_080b4a8c();
void sub_080b4afc();
void sub_080b4b18();
void sub_080b4b94();
void sub_080b4bb0();
void sub_080b4bd8();
void sub_080b4be4();
void sub_080b4c14();
void sub_080b4c84();
void sub_080b4ca0();
void sub_080b4d1c();
void sub_080b4d50();
void sub_080b4db4();
void sub_080b4dd0();
void sub_080b4df8();
void sub_080b4e04();
void sub_080b4e40();
void sub_080b4ea8();
void sub_080b5024();
s32 sub_080b5338();
void sub_080b54a4();
void sub_080b54d0();
void sub_080b5540();
void sub_080b5558();
s32 sub_080b55d8();
s32 sub_080b5628();
s32 sub_080b5654();
s32 sub_080b5670();
s32 sub_080b5840();
s32 sub_080b590c();
void sub_080b59d8();
s32 sub_080b5a94();
s32 sub_080b5bdc();
s32 sub_080b5d84();
void HBlankScrollVBlankCallback();
void UpdateHBlankScroll();
extern void CpuSet(const void *src, void *dst, u32 control);

void MetaKnightInit(void)
{
    struct Task *t;

    t = gCurTask;
    t->updateCallback = (u32)MetaKnightUpdate;
    t->unk28 = 0;
    t->unk2C = -1;
    t->unk30 = -1;
    t->unk34 = 0;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[3] = (s16)ActorComputeHealth();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 24, gMetaKnightStates);
}

void MetaKnightEnterState(void)
{
    CallTableEntry(gCurTask->state, 24, gMetaKnightStates);
}

void MetaKnightUpdate(void)
{
    struct Task *t;
    u32 rr;
    u32 *q;

    if ((u8)ActorCollideTerrain() == 0)
        CallTableEntry(gCurTask->updateState, 24, gMetaKnightStateUpdates);
    if (gCurTask->unk30 != -1)
    {
        ActorSetAttackBox(gUnk_08748D38[gCurTask->unk30]);
        gUnk_02007D00[1] = gCurTask->health;
        sub_08068f68();
        switch (gCurTask->hitKind)
        {
        case 1:
        case 2:
            if ((u8)sub_080a720c() == 1)
            {
                gCurTask->hitKind = 0;
                t = gCurTask;
                t->health = gUnk_02007D00[1];
                t->unk30 = 2;
                ActorSetState(22);
                sub_080a7168();
            }
            break;
        case 6:
            gUnk_02007D00[0] = gCurTask->hitterPlayer;
            switch (gCurTask->unk82)
            {
            case 4:
                gUnk_03001F2C = sub_080a6e1c();
                q = &gUnk_03002448;
                rr = RandomRange(16);
                *q = rr;
                if ((s32)rr < gUnk_08748D6C[gUnk_03001F2C])
                {
                    gCurTask->unk30 = 2;
                    ActorSetState(22);
                    sub_080a7168();
                }
                else if ((s32)rr < gUnk_08748D70[gUnk_03001F2C])
                {
                    gCurTask->unk30 = 2;
                    ActorSetState(16);
                    sub_080a7168();
                }
                break;
            case 2:
            case 3:
                gCurTask->unk30 = 2;
                ActorSetState(23);
                sub_080a7168();
                break;
            }
            break;
        }
        ActorReactToHit();
    }
}

void sub_080a57d4(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    s32 i;

    gCurTask->updateState = 0;
    gCurTask->posX = (gViewRect[0] + 216) << 16;
    gCurTask->posY = (gViewRect[2] + 44) << 16;
    if (gUnk_03001F30 == 0)
    {
        for (i = 0; i < gActivePlayerCount; i++)
        {
            sp.subtype = 18;
            sp.taskType = 120;
            sp.variant = 0;
            sp.spawnArg = i;
            sp.x = gUnk_08748D28[i + (gActivePlayerCount - 1) * 4] + gViewRect[0];
            sp.y = gViewRect[2];
            sp.tileWord = gCurTask->unk8C->savedTileWord;
            sp.checkTerrain = 0;
            CreateActorFromDesc(&sp, 1);
        }
    }
    else
        gUnk_02007D00[2] = gPlayerCount;
    gCurTask->facing = 1;
    TaskSetFrame(4);
    while (gUnk_02007D00[2] != gActivePlayerCount)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(1);
    gCurTask->unk30 = 2;
    sub_08066544();
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(185, (s16)(t->pixelX + 8), t->pixelY, t->unk8C->savedTileWord);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(18);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    t = gCurTask;
    CreateChildTask(188, (s16)(t->pixelX - 32), t->pixelY, t->unk8C->savedTileWord | (240 << 8));
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    gCurTask->velY = 128 << 10;
    gCurTask->accelY = -0x10000;
    TaskYieldTrampoline(3);
    TaskStopY();
    TaskYieldTrampoline(20);
    gCurTask->facing = 255;
    TaskSetFrame(60);
    TaskYieldTrampoline(10);
    CreateChildTaskHere(184, 0);
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(9);
    TaskSleepForever();
}

void sub_080a5a78(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5aa0(void)
{
    struct Task *t;
    s32 v;

    gCurTask->updateState = 1;
    t = gCurTask;
    t->unk28 &= ~128;
    t->unk30 = 0;
    v = TaskFindNearestPlayer();
    gUnk_02007D00[5] = v;
    if (gUnk_0300244C != 0)
    {
        if (gCurTask->unk2C != v)
        {
            gCurTask->unk20 = 0;
            gCurTask->unk24 = 0;
            gCurTask->unk2C = gUnk_02007D00[5];
        }
    }
    sub_080a6e98();
}

void sub_080a5af4(void)
{
    struct Task *pt;
    s16 *px;
    vu16 *pv;
    vu16 *f9;
    s32 d;

    gCurTask->unk1C--;
    if (gCurTask->unk1C == 0)
    {
        sub_080a7190();
        return;
    }
    pt = &gTasks[gUnk_02007D00[5]];
    px = &pt->pixelX;
    d = (*px + gCurTask->unk18) - gCurTask->pixelX;
    switch (gCurTask->unk24)
    {
    case 0:
        if (gCurTask->unk34 & 2)
        {
            if (d < 0)
                gCurTask->facing = 255;
            else
                gCurTask->facing = 1;
        }
        else
            TaskFaceNearestPlayer();
        if (abs(d) <= 1)
        {
            gCurTask->unk24 = 1;
            gCurTask->velX = 0;
            gUnk_02007D00[7] = 0;
        }
        else if (abs(d) <= 27)
        {
            gCurTask->unk20 &= ~2;
            if (d > 0)
                gCurTask->velX = 128 << 9;
            else
                gCurTask->velX = -0x10000;
        }
        else
        {
            gCurTask->unk20 |= 2;
            if (d > 0)
                gCurTask->velX = 128 << 10;
            else
                gCurTask->velX = -0x20000;
        }
        gUnk_02007D00[6] = pt->pixelX;
        break;
    case 1:
        gCurTask->unk34 &= ~6;
        if ((u8)sub_080a6f38(gCurTask->pixelX, gCurTask->pixelY) == 1)
        {
            gCurTask->unk24 = 2;
            gCurTask->velX = 0;
        }
        else
        {
            gCurTask->pixelX = *px + gCurTask->unk18;
            gCurTask->posX = gCurTask->pixelX << 16;
            TaskFaceNearestPlayer();
            f9 = gPlayerHeldKeys;
            pv = &f9[gUnk_02007D00[5]];
            if ((*pv & 48) && *px != gUnk_02007D00[6])
            {
                if ((u16)(*pv & 16) != 0)
                {
                    if (gUnk_02007D00[7] < 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]++;
                }
                else
                {
                    if (gUnk_02007D00[7] > 0)
                        gUnk_02007D00[7] = 0;
                    gUnk_02007D00[7]--;
                }
                if (abs(gUnk_02007D00[7]) == 15)
                    gCurTask->unk24 = 0;
            }
            else if (pt->velX == 0)
            {
                gCurTask->unk24 = 2;
                gCurTask->velX = 0;
            }
        }
        gUnk_02007D00[6] = pt->pixelX;
        break;
    case 2:
        TaskFaceNearestPlayer();
        if (*px != gUnk_02007D00[6])
        {
            if ((u8)sub_080a6f38((s16)(*px + gCurTask->unk18), gCurTask->pixelY) == 0)
            {
                gCurTask->unk24 = 0;
                gCurTask->unk20 &= ~1;
                break;
            }
        }
        if (gCurTask->frame == 65)
            gCurTask->unk20 |= 1;
        break;
    }
    if ((u8)sub_080a6f74() == 1)
    {
        gCurTask->velX = 0;
        gCurTask->unk24 = 2;
        if (!(gCurTask->unk34 & 1))
        {
            gCurTask->unk34 |= 1;
            gCurTask->unk1C = RandomRange(30) + 30;
        }
    }
    TaskUpdateFlip();
    if (!(gCurTask->unk34 & 6))
    {
        if (abs(TaskGetNearestPlayerDx()) <= 31)
        {
            ActorSetState(gUnk_08748DA8[pt->onGround][RandomRange(8)]);
            sub_080a7168();
        }
    }
}

void sub_080a5dd0(void)
{
    gCurTask->updateState = 2;
    if (!(gCurTask->unk34 & 2))
    {
        if (gCurTask->facing == 1)
            gCurTask->unk18 = 64;
        else
            gCurTask->unk18 = -64;
    }
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk34 &= 2;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5e30(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 2)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5e60(void)
{
    gCurTask->updateState = 3;
    gCurTask->unk18 = -gCurTask->unk18;
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    gCurTask->unk34 = 2;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5e9c(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 3)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5ecc(void)
{
    gCurTask->updateState = 4;
    gCurTask->unk1C = 48;
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5ef0(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 4)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5f20(void)
{
    struct Task *t;

    gCurTask->updateState = 5;
    gCurTask->unk34 = 4;
    TaskFaceNearestPlayer();
    t = gCurTask;
    if (t->facing == 1)
        t->unk18 = -64;
    else
        t->unk18 = 64;
    sub_080a6e58();
    gCurTask->unk20 = 0;
    gCurTask->unk24 = 0;
    TaskSetFrame(61);
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a5f7c(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 5)
        TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a5fac(void)
{
    gCurTask->updateState = 6;
    if (gCurTask->unk18 < 0)
        gUnk_02007D00[6] = 1;
    else
        gUnk_02007D00[6] = 0;
    {
        struct Task *t2 = &gTasks[TaskFindNearestPlayer()];

        gUnk_02007D00[7] = t2->pixelX;
    }
    gCurTask->unk20 = 0;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    TaskSetFrame(61);
    sub_080a6e98();
}

void sub_080a6020(void)
{
    s32 d;
    s32 x;

    ClampTaskToRoom(gCurTask);
    x = gCurTask->pixelX;
    d = x - gUnk_02007D00[7];
    if (d >= 0 ? d <= 5 : gUnk_02007D00[7] - x <= 5)
    {
        if (abs(TaskGetNearestPlayerDy()) <= 47)
            ActorSetState(gUnk_08748E48[RandomRange(16)]);
        else
            ActorSetState(gUnk_08748E68[RandomRange(16)]);
        if (gCurTask->state == 3)
            gUnk_02007D00[6]++;
        gCurTask->unk18 = gUnk_08748D60[gUnk_02007D00[6]];
        gCurTask->unk34 = 2;
        sub_080a7168();
    }
}

void MetaKnightRun(void)
{
    gCurTask->updateState = 7;
    TaskFaceNearestPlayer();
    TaskSetMotionXFacing(224 << 9, 0x5A5A5A5A);
    for (;;)
    {
        TaskSetFrame(82);
        TaskYieldTrampoline(2);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 4);
    }
}

void sub_080a6130(void)
{
    ClampTaskToRoom(gCurTask);
    if (abs(TaskGetNearestPlayerDx()) <= 43)
    {
        gCurTask->unk34 = 0;
        if (gCurTask->health >= gUnk_02007D00[3] >> 1)
            ActorSetState(gUnk_08748E88[RandomRange(8)]);
        else
            ActorSetState(gUnk_08748E98[RandomRange(8)]);
        sub_080a7168();
    }
}

void sub_080a61b4(void)
{
    gCurTask->updateState = 9;
    gCurTask->unk30 = 0;
    gUnk_02007D00[7] = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x26000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    if (gCurTask->onGround == 0)
    {
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(10);
    TaskSleepForever();
}

void sub_080a6264(void)
{
    sub_080a6fb4();
    if (gCurTask->state != 9)
        sub_080a7168();
}

void sub_080a6280(void)
{
    gCurTask->updateState = 8;
    gUnk_02007D00[7] = 0;
    gCurTask->onGround = 0;
    TaskSetMotionY(-0x33000, 168 << 5, 192 << 10);
    TaskSetMotionXFacing(gUnk_02007D00[5], 0x5A5A5A5A);
    TaskSetFrame(72);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    if (gCurTask->onGround == 0)
    {
        while (gCurTask->onGround == 0)
            TaskYieldTrampoline(1);
    }
    TaskStop();
    ActorSetState(10);
    TaskSleepForever();
}

void sub_080a6330(void)
{
    sub_080a6fb4();
    if (gCurTask->state != 8)
        sub_080a7168();
}

void sub_080a634c(void)
{
    gCurTask->updateState = 10;
    TaskStop();
    gCurTask->unk30 = 0;
    TaskSetFrame(81);
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a638c(void)
{
    if (gCurTask->state != 10)
        sub_080a7168();
}

void sub_080a63a4(void)
{
    gCurTask->updateState = 11;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 0;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a63d8(void)
{
    if (gCurTask->state != 11)
        sub_080a7168();
}

void sub_080a63f0(void)
{
    gCurTask->updateState = 12;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 0;
    gUnk_02007D00[6] = 0;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6420(void)
{
    if (gCurTask->state != 12)
        sub_080a7168();
}

void sub_080a6438(void)
{
    struct Task *t;
    s32 i;

    gCurTask->updateState = 13;
    t = gCurTask;
    t->accelY = 144 << 7;
    t->speedLimitY = 128 << 11;
    TaskGetScreenPosSlot(gCurTaskIdx);
    if (gUnk_030023D4 <= 43)
        gUnk_02007D00[7] = 4;
    else
        gUnk_02007D00[7] = 2;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gUnk_02007D00[7])
    {
        PlaySfx(0x225);
        TaskSetFrame(114);
        TaskYieldTrampoline(1);
        gCurTask->unk6E = 0;
        do
        {
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->unk6E++;
        } while ((s16)gCurTask->unk6E <= 6);
        gCurTask->unk6C++;
    }
    TaskSetFrame(122);
    TaskYieldTrampoline(2);
    TaskSetFrame(80);
    TaskSleepForever();
}

void sub_080a650c(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->onGround != 0)
    {
        TaskStop();
        ActorSetState(10);
        sub_080a7168();
    }
}

void sub_080a6544(void)
{
    gCurTask->updateState = 16;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 15;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6574(void)
{
    if (gCurTask->state != 16)
        sub_080a7168();
}

void sub_080a658c(void)
{
    gCurTask->updateState = 15;
    TaskStop();
    TaskSetFrame(111);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskSetMotionY(128 << 10, 144 << 7, 128 << 11);
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    RequestScreenShake(2);
    PlaySfx(504);
    sub_0806e9b4(0, 0, 3);
    gCurTask->unk30 = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(12);
    TaskSetFrame(60);
    TaskYieldTrampoline(56);
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a6628(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->state != 15)
        sub_080a7168();
}

void sub_080a6650(void)
{
    gCurTask->updateState = 14;
    TaskFaceNearestPlayer();
    gUnk_02007D00[5] = 192 << 9;
    gUnk_02007D00[6] = 17;
    ActorSetState(8);
    TaskSleepForever();
}

void sub_080a6680(void)
{
    if (gCurTask->state != 14)
        sub_080a7168();
}

void sub_080a6698(void)
{
    gCurTask->updateState = 17;
    sub_080a7080();
    TaskSetMotionY(0, 168 << 5, 192 << 10);
    TaskSetFrame(80);
    TaskSleepForever();
}

void sub_080a66c8(void)
{
    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gCurTask->onGround != 0)
    {
        StopSfxOnPlayer(gUnk_02007D00[4], 137 << 2);
        TaskStop();
        ActorSetState(10);
        sub_080a7168();
    }
}

void sub_080a6710(void)
{
    gCurTask->updateState = 18;
    TaskStop();
    sub_080a7080();
    ActorSetState(2);
    TaskSleepForever();
}

void sub_080a6734(void)
{
    ClampTaskToRoom(gCurTask);
    if (gCurTask->state != 18)
        sub_080a7168();
}

void sub_080a6754(void)
{
    struct Task *t;

    gCurTask->updateState = 19;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(12);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->unk30 = 0;
    gCurTask->frame++;
    TaskYieldTrampoline(24);
    TaskSetFrame(60);
    TaskYieldTrampoline(28);
    ActorSetState(5);
    TaskSleepForever();
}

void sub_080a680c(void)
{
    s32 v;

    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->velX = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->state != 19)
        sub_080a7168();
}

void sub_080a6868(void)
{
    struct Task *t;

    gCurTask->updateState = 20;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(88);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    TaskSetFrame(97);
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame--;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    TaskSetFrame(88);
    TaskYieldTrampoline(10);
    ActorSetState(21);
    TaskSleepForever();
}

void sub_080a6988(void)
{
    s32 v;

    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 15)
    {
        v = gUnk_02007D00[7] + 1;
        gUnk_02007D00[7] = v;
        if (v == 16)
            gCurTask->velX = 0;
        else
            TaskSetMotionXFacing(gUnk_08748D44[v >> 3], 0x5A5A5A5A);
    }
    if (gCurTask->state != 20)
        sub_080a7168();
}

void sub_080a69e4(void)
{
    struct Task *t;

    gCurTask->updateState = 21;
    TaskStop();
    gUnk_02007D00[7] = -1;
    TaskSetFrame(105);
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    PlaySfx(137 << 2);
    t = gCurTask;
    CreateChildTask(145, (s16)(t->pixelX - t->facing * 8), (s16)(t->pixelY + 8), 0);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(20);
    gCurTask->unk30 = 0;
    TaskSetFrame(60);
    TaskYieldTrampoline(48);
    ActorSetState(5);
    TaskSleepForever();
}

void sub_080a6aac(void)
{
    ClampTaskToRoom(gCurTask);
    if (gUnk_02007D00[7] <= 25)
    {
        gUnk_02007D00[7]++;
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2
             && gUnk_02007D00[7] >= gUnk_08748D4C[(s16)gCurTask->unk6C]; gCurTask->unk6C++)
            ;
        TaskSetMotionXFacing(gUnk_08748D50[(s16)gCurTask->unk6C], 0x5A5A5A5A);
    }
    if (gCurTask->state != 21)
        sub_080a7168();
}

void sub_080a6b3c(void)
{
    struct Task *t;

    gCurTask->updateState = 22;
    gUnk_02007D00[6] = gCurTask->onGround;
    gUnk_02007D00[7] = 20;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    t = gCurTask;
    if (t->onGround == 0)
    {
        CreateChildTask(188, (s16)(t->pixelX + t->facing * 4), (s16)(t->pixelY - 4),
                     t->unk8C->savedTileWord | (240 << 8));
        gCurTask->accelY = 168 << 5;
        gCurTask->speedLimitY = 192 << 10;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(70);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
    }
    else
    {
        CreateChildTask(188, (s16)(t->pixelX + t->facing * 16), (s16)(t->pixelY + 4),
                     t->unk8C->savedTileWord | (240 << 8));
        TaskStop();
        TaskSetFrame(69);
    }
    TaskSleepForever();
}

void sub_080a6c3c(void)
{
    s32 *pD;
    u8 *b3v;
    s32 *pE;
    s32 w;
    s32 v;

    ClampTaskToRoom(gCurTask);
    pD = gUnk_02007D00;
    v = pD[7];
    if (v == 0)
    {
        gCurTask->unk30 = v;
        if (gCurTask->onGround != 0)
        {
            if (pD[6] == 0)
            {
                TaskStop();
                ActorSetState(10);
                TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
            }
            else
            {
                TaskStop();
                gCurTask->unk24 = v;
                sub_080a7190();
            }
        }
    }
    else
    {
        w = v - 1;
        pD[7] = w;
        pD[5] = 0;
        b3v = (u8 *)gUnk_08748D78;
        pE = pD;
        while (w >= *(u8 *)(pD[5] + (u32)b3v))
            pD[5]++;
        TaskSetMotionXFacing(gUnk_08748D80[pE[5]], 0x5A5A5A5A);
        if (gCurTask->onGround != 0)
        {
            TaskStopY();
            pE[6] = gCurTask->onGround;
            TaskSetFrame(69);
        }
    }
}

void sub_080a6d08(void)
{
    struct Task *t;

    gCurTask->updateState = 23;
    TaskStop();
    gUnk_02007D00[6] = gCurTask->onGround;
    t = gCurTask;
    if (t->onGround == 0)
    {
        t->accelY = 168 << 5;
        t->speedLimitY = 192 << 10;
    }
    gUnk_02007D00[7] = 60;
    TaskFaceNearestPlayer();
    PlaySfx(134 << 2);
    TaskSetFrame(69);
    TaskSleepForever();
}

void sub_080a6d60(void)
{
    u16 v;

    ClampTaskToRoom(gCurTask);
    if (gPlayerStates[gUnk_02007D00[0]].unk40 & 4) {
        if (gCurTask->onGround != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                gUnk_02007D00[6] = gCurTask->onGround;
            }
            if (--gUnk_02007D00[7] == 0) {
                gCurTask->unk30 = 1;
                v = gUnk_08748EA8[RandomRange(2)];
                ActorSetState(v);
                TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
            }
        }
    } else {
        if (gCurTask->onGround != 0) {
            if (gUnk_02007D00[6] == 0) {
                TaskStopY();
                v = 10;
            } else {
                v = 2;
            }
            ActorSetState(v);
            TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
        } else {
            TaskSetFrame(70);
        }
    }
}

s32 sub_080a6e1c(void)
{
    s32 x = gCurTask->health;
    s32 q = gUnk_02007D00[3] >> 2;

    if (x < q)
        return 0;
    if (x < gUnk_02007D00[3] >> 1)
        return 1;
    if (x < q * 3)
        return 2;
    return 3;
}

void sub_080a6e58(void)
{
    if (gCurTask->health >= gUnk_02007D00[3] >> 1)
        gCurTask->unk1C = RandomRange(54) + 48;
    else
        gCurTask->unk1C = RandomRange(84) + 96;
}

void sub_080a6e98(void)
{
    struct Task **c = &gCurTask;
    struct Task **d;
    u8 *a = gUnk_08748D98;
    u8 *b = a + 8;
    struct Task *t;
    struct Task *u;
    s32 ix;
    s32 n;

    for (;;)
    {
        t = *c;
        if (t->unk20 & 2)
        {
            ix = t->frame - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)b));
        }
        else
        {
            ix = t->frame - 61;
            TaskYieldTrampoline(*(u8 *)(ix + (u32)a));
        }
        t = *c;
        n = t->unk20 & 1;
        d = &gCurTask;
        if (n == 0)
        {
            if ((t->facing == 1 && t->velX >= 0) || (t->facing == -1 && t->velX <= 0))
            {
                u = *c;
                u->frame++;
                if ((s16)u->frame > 68)
                    u->frame = 61;
            }
            else
            {
                u = *d;
                u->frame--;
                if ((s16)u->frame <= 60)
                    u->frame = 68;
            }
        }
    }
}

s32 sub_080a6f38(s16 x, s16 y)
{
    if ((u16)GetCollisionTileAtOffset(x, y, 1, 0) == 0 && (u16)GetCollisionTileAtOffset(x, y, -1, 0) == 0)
        return 0;
    return 1;
}

s32 sub_080a6f74(void)
{
    u8 v = ClampTaskToRoom(gCurTask);

    if (((v & 1) && gCurTask->velX < 0) || ((v & 2) && gCurTask->velX > 0))
        return 1;
    return 0;
}

void sub_080a6fb4(void)
{
    s32 k;
    s32 g;

    if ((u8)sub_080a6f74() == 1)
        TaskTurnAroundAndReverseX();
    if (gUnk_02007D00[7] == 0 && gCurTask->onGround == 0 && gCurTask->velY > 0)
    {
        if (gUnk_02007D00[6] == 0)
        {
            gCurTask->unk34 = 0;
            gUnk_02007D00[7]++;
            gUnk_02007D00[4] = TaskGetNearestPlayerDx();
            if (TaskGetNearestPlayerDy() < 0)
                k = 0;
            else
            {
                g = gUnk_02007D00[4];
                if (abs(g) <= 15)
                    k = 1;
                else if ((g > 0 && gCurTask->facing == 1) ||
                         (g < 0 && gCurTask->facing == -1))
                    k = 2;
                else
                    k = 3;
            }
            gUnk_02007D00[6] = gUnk_08748E28[k][RandomRange(4)];
            if (gUnk_02007D00[6] == 0)
                return;
        }
        ActorSetState((u16)gUnk_02007D00[6]);
    }
}

void sub_080a7080(void)
{
    gUnk_02007D00[4] = -1;
    TaskSetFrame(98);
    TaskYieldTrampoline(6);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gUnk_02007D00[4] = PlaySfx(137 << 2);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x40000;
    gCurTask->accelY = 128 << 8;
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskStopY();
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->velY = 128 << 11;
    gCurTask->accelY = -0x8000;
    TaskSetFrame(101);
    TaskYieldTrampoline(2);
    TaskStopY();
    gCurTask->frame--;
    TaskYieldTrampoline(2);
}

void sub_080a7168(void)
{
    gCurTask->unk28 |= 128;
    TaskSetEntry(MetaKnightEnterState, gCurTaskIdx);
}

void sub_080a7190(void)
{
    struct Task *pt = &gTasks[TaskFindNearestPlayer()];

    if (gCurTask->unk34 & 1)
        ActorSetState(gUnk_08748DC8[RandomRange(16)]);
    else
        ActorSetState(gUnk_08748DE8[pt->onGround][RandomRange(16)]);
    gCurTask->unk34 = 0;
    sub_080a7168();
}

s32 sub_080a720c(void)
{
    u8 k;

    if (gCurTask->unk28 & 128)
        gCurTask->unk28++;
    if (gCurTask->unk28 & 3)
    {
        gCurTask->unk28--;
        k = sub_080a6e1c();
        if (RandomRange(16) < gUnk_08748D74[k])
            return 1;
    }
    return 0;
}

s32 sub_080a7260(void)
{
    gBg2Cnt |= 64;
    gBg3Cnt |= 64;
    gCurTask->unk28 |= 2;
    sub_0806619c(13, (u32)sub_080a72b0, (u32)gUnk_082F427C, 16, 1);
    return 0;
}

void sub_080a72b0(void)
{
    gBgMosaic = gUnk_08748EAC[gUnk_02006190[3] >> 1];
    if (gUnk_02006190[3] == 0)
    {
        gBg2Cnt &= 0xFFBF;
        gBg3Cnt &= 0xFFBF;
        sub_0806621c();
    }
}

s32 sub_080a72fc(void)
{
    TaskStop();
    ActorSetHitReactions((u32)gUnk_08749B30);
    gCurTask->unk30 = -1;
    TaskSetEntry(ActorDie, gCurTaskIdx);
    return 1;
}

s32 sub_080a7334(void)
{
    struct Task *t;

    switch (gCurTask->state)
    {
    case 1:
        t = gCurTask;
        t->unk24 = 1;
        t->velX = 0;
        break;
    case 8:
    case 13:
    case 15:
    case 17:
        TaskTurnAroundAndReverseX();
        break;
    }
    return 0;
}

void sub_080a73b4(void)
{
    struct Task *t = gCurTask;

    t->drawCallback = (u32)sub_080653ec;
    t->updateCallback = (u32)sub_080a73fc;
    t->frameTable = gMetaKnightFrames;
    t->layer = 4;
    TaskFaceNearestPlayer();
    ActorSetState(0);
    CallTableEntry(gCurTask->state, 2, gUnk_08748F7C);
}

void sub_080a73fc(void)
{
    ActorCollideTerrain();
    CallTableEntry(gCurTask->updateState, 2, gUnk_08748F84);
}

void sub_080a741c(void)
{
    CallTableEntry(gCurTask->state, 2, gUnk_08748F7C);
}

void sub_080a7438(void)
{
    struct ActorSpawn sp;

    gCurTask->updateState = 0;
    TaskStop();
    gCurTask->onGround = 0;
    gCurTask->accelY = 168 << 5;
    gCurTask->speedLimitY = 192 << 10;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    sp.subtype = 18;
    sp.taskType = 120;
    sp.variant = 1;
    sp.spawnArg = 0;
    sp.x = 0;
    sp.y = 0;
    sp.tileWord = gCurTask->unk8C->savedTileWord + (128 << 5);
    sp.checkTerrain = 0;
    CreateActorFromDescHere(&sp, 1);
    TaskGetScreenPosSlot(gCurTaskIdx);
    gCurTask->unk30 = gUnk_030023B4;
    TaskGetNearestPlayerScreenPos();
    if ((u32)(gUnk_030023B4 - 88) > 64)
        gCurTask->unk28 = 128;
    else if (gCurTask->unk30 <= 127)
        gCurTask->unk28 = 64;
    else
        gCurTask->unk28 = 176;
    if (gCurTask->unk30 < gCurTask->unk28)
        gCurTask->facing = 1;
    else
        gCurTask->facing = -1;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    TaskSetFrame(21);
    gCurTask->unk2C = abs(gCurTask->unk30 - gCurTask->unk28) >> 1;
    TaskSetMotionY(-((gCurTask->unk2C >> 1) << 13), 128 << 6, 192 << 10);
    if (gCurTask->velY == 0)
        gCurTask->velY = -0x10000;
    gCurTask->onGround = 0;
    while (gCurTask->onGround == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    ActorSetState(1);
    TaskSleepForever();
}

void sub_080a75a0(void)
{
    if (gCurTask->state != 0)
        TaskSetEntry(sub_080a741c, gCurTaskIdx);
}

void sub_080a75c8(void)
{
    struct Task *t;

    gCurTask->updateState = 1;
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(23);
        TaskYieldTrampoline(2);
        TaskSetFrame(21);
        TaskYieldTrampoline(14);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    t = gCurTask;
    CreateChildTask(186, (s16)(t->pixelX - t->facing * 2), (s16)(t->pixelY - 1), t->unk8C->savedTileWord);
    TaskSetFrame(24);
    TaskYieldTrampoline(8);
    gCurTask->frame++;
    TaskYieldTrampoline(4);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x30000;
    gCurTask->accelY = 128 << 9;
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x20000;
    gCurTask->accelY = 128 << 9;
    TaskYieldTrampoline(3);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(5);
    TaskYieldTrampoline(5);
    gCurTask->onGround = 0;
    gCurTask->velY = -0x44000;
    gCurTask->accelY = 128 << 7;
    gCurTask->frame++;
    TaskYieldTrampoline(10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 6);
    gCurTask->frame++;
    TaskYieldTrampoline(9);
    TaskStop();
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(3);
    gCurTask->frame++;
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    TaskSetFrame(45);
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 7);
    gCurTask->frame--;
    TaskYieldTrampoline(2);
    TaskSetFrame(57);
    TaskYieldTrampoline(2);
    gCurTask->frame++;
    TaskYieldTrampoline(1);
    gCurTask->frame++;
    gCurTask->onGround = 0;
    gCurTask->velY = -0x70000;
    while (gCurTask->pixelY > 10)
        TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->frame = 0xFFFF;
    TaskYieldTrampoline(30);
    FadeOutBgm(8);
    TaskYieldTrampoline(32);
    sub_08066fc0(0, 128, 104);
    ActorDestroy();
}

void sub_080a787c(void)
{
}

void sub_080a7880(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_080a78a0;
    TaskSleepForever();
}
