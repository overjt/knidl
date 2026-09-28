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
    (struct ActorDef *)gUnk_08740BD4,
    &gUnk_087417B8,
    &gUnk_087429E8,
    &gUnk_0874330C,
    &gUnk_08740C2C,
    &gUnk_08743338,
    &gUnk_08742A14,
    &gUnk_087417E4,
    &gUnk_08741810,
    (struct ActorDef *)gUnk_08742A40,
    &gUnk_08741868,
    &gUnk_08741894,
    &gUnk_08742A98,
    &gUnk_087418C0,
    &gUnk_08740C58,
    &gUnk_08743364,
    &gUnk_08742AC4,
    (struct ActorDef *)gUnk_08743390,
    &gUnk_08740C84,
    &gUnk_087418EC,
    &gUnk_08742AF0,
    &gUnk_08740CB0,
    &gUnk_08742B1C,
    &gUnk_08741918,
    &gUnk_08741944,
    &gUnk_08742B48,
    &gUnk_08742B74,
    &gUnk_08742BA0,
    &gUnk_08740CDC,
    &gUnk_08740D08,
    &gUnk_08741970,
    &gUnk_0874199C,
    &gUnk_087419C8,
    &gUnk_087433BC,
    &gUnk_08741A4C,
    &gUnk_08741A78,
    &gUnk_08740D60,
    &gUnk_08740D8C,
    &gUnk_08740DB8,
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
    &gUnk_0873F3C0,
    &gUnk_0873F3EC,
    &gUnk_0873F418,
    &gUnk_0873F444,
    &gUnk_0873F470,
    &gUnk_0873F49C,
};

/* every other kind */
struct ActorDef *const gUnk_0873EE88[] = {
    &gUnk_0873F2B8,
    &gUnk_0873F2E4,
    &gUnk_0873F310,
    &gUnk_0873F33C,
    &gUnk_0873F368,
    &gUnk_0873F394,
};
