#include "global.h"
#include "task.h"
#include "actor.h"
#include "cutscene.h"
#include "enemy.h"

/* The actor definition tables (0x0873ECEC-0x0873EE9F, issue #36 phase 2):
 * one array of struct ActorDef pointers per actor kind.  sub_08063704
 * (src/actor_63698.c) binds a task to its definition with
 * table[Task.unk76] picked by Task.actorKind; the lengths are the spans
 * between the consumer-referenced labels (docs/data.md 5.1).  The records
 * stay structure-only data in seg 18 (actor_rodata).  The four
 * records that other files read as u32 arrays are cast.  Carved by
 * tools/carve_data.py. */

/* kind 0: the enemies, by subtype */
struct ActorDef *const gUnk_0873ECEC[] = {
    (struct ActorDef *)gWaddleDeeDef,
    &gRockyDef,
    &gNoddyDef,
    &gBroomHatterDef,
    &gPengyDef,
    &gLaserBallDef,
    &gChillyDef,
    &gSirKibbleDef,
    &gCappyDef,
    (struct ActorDef *)gWaddleDooDef,
    &gGordoDef,
    &gCoolSpookDef,
    &gBrontoBurtDef,
    &gKabuDef,
    &gBomberDef,
    &gUnk_08743364,
    &gTwizzyDef,
    (struct ActorDef *)gShotzoDef,
    &gSparkyDef,
    &gTwisterDef,
    &gSquishyDef,
    &gScarfyDef,
    &gUnk_08742B1C,
    &gStarmanDef,
    &gHotHeadDef,
    &gGlunkDef,
    &gUnk_08742B74,
    &gBlipperDef,
    &gUnk_08740CDC,
    &gUnk_08740D08,
    &gPoppyBrosJrDef,
    &gUnk_0874199C,
    &gUnk_087419C8,
    &gUnk_087433BC,
    &gWheelieDef,
    &gFlamerDef,
    &gNeedlousDef,
    &gUFODef,
    &gParasolDef,
    &gUnk_08742BCC,
    &gUnk_08740D34,
};

/* kinds 1 and 3: the mid-bosses */
struct ActorDef *const gMidBossDefs[] = {
    &gBonkersDef,
    &gPoppyBrosSrDef,
    &gGrandWheelieDef,
    &gMrFrostyDef,
    &gMrTickTockDef,
    &gBugzzyDef,
    &gFireLionDef,
    &gPhanPhanDef,
    NULL,
    NULL,
};

/* kind 2: the bosses (the subtype is also kept in gUnk_02007F50) */
struct ActorDef *const gBossDefs[] = {
    &gKingDededeDef,
    &gPaintRollerDef,
    &gMetaKnightDef,
    &gHeavyMoleDef,
    &gMrShineAndMrBrightDef,
    &gWhispyWoodsDef,
    &gKrackoDef,
    &gNightmarePowerOrbDef,
    &gNightmareWizardDef,
};

/* kind 4 */
struct ActorDef *const gUnk_0873EDDC[] = {
    &gUnk_087410E4,
    &gUnk_08743644,
    &gUnk_08742FBC,
    &gUnk_08741EB4,
    &gUnk_08742FE8,
    &gUnk_08743014,
    &gUnk_08741EE0,
    &gUnk_08743670,
    &gUnk_087441D8,
    &gUnk_08744204,
    &gUnk_08748AB0,
    &gUnk_08748ADC,
    &gUnk_08745530,
    &gUnk_08745B30,
    &gUnk_0874B940,
    (struct ActorDef *)gUnk_0874BACC,
    &gUnk_08745B5C,
    &gUnk_08745B88,
    &gUnk_08749BEC,
    &gUnk_0874BAF8,
    &gUnk_0874BB24,
    &gUnk_0874BB50,
    &gUnk_0874C378,
    &gUnk_0874C3A4,
    &gUnk_08748B08,
    &gUnk_08749C18,
    &gUnk_08747D88,
    &gUnk_08747DE0,
    &gUnk_08747E38,
    &gUnk_08747E90,
    &gUnk_08744230,
    &gUnk_08741110,
    &gUnk_0874B360,
    &gUnk_08749C44,
    &gUnk_08743040,
    &gUnk_0874555C,
    &gUnk_0873F664,
};

/* kind 5 */
struct ActorDef *const gUnk_0873EE70[] = {
    &gWarpStarDef,
    &gCannonDef,
    &gCannonFuseDef,
    &gBigSwitchDef,
    &gStakeDef,
    &gUnk_0873F49C,
};

/* every other kind */
struct ActorDef *const gUnk_0873EE88[] = {
    &gAbilityStarDef,
    &gOneUpDef,
    &gMaximTomatoDef,
    &gInvincibleCandyDef,
    &gEnergyDrinkDef,
    &gUnk_0873F394,
};
