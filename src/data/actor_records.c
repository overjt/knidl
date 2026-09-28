#include "global.h"
#include "task.h"
#include "actor.h"

/* The actor records of seg 18 (0x0873F2B8-0x0874C3CF, issue #36 phase 2
 * run 3): 136 struct ActorDef (0x2C bytes) and 43 struct ActorAux (8 bytes)
 * records in 34 runs, each run in address order.  sub_080637e4 and
 * ActorLoadDefSlot (src/actor_63698.c) read an ActorDef when a task binds
 * it (Actor.def); ActorDef.unk10 becomes Actor.unk60, the ActorAux whose
 * altAttackBox becomes gAttackBox (src/actor_673ec.c).  The six pointer
 * tables are src/data/actor_defs.c; the rest of actor_rodata (behaviour
 * tables, hit boxes, scripts) stays structure-only data between the runs.
 *
 * Each run is a named section .actor_rec_<address>: linker.ld lists them
 * between the data pieces of actor_rodata inside ONE output section
 * (docs/data.md 5.2).  The records are not const (the qualifier would
 * reach ActorLoadDef's parameter and Actor.def under -Werror); the section
 * attribute is what places them in ROM.  A word the data file did not
 * symbolize would stay a number here, so the shift test sees the same
 * relocations; since ActorDef.unk10 is a pointer everywhere (#36 run 3)
 * there is none. */

#define ACTOR_REC(addr) __attribute__((section(".actor_rec_" #addr)))

/* Functions whose prototype lives in a header this file does not include
 * (enemy.h and cutscene.h declare 31 of these records as u32 [], a view
 * that conflicts with the definitions below, lesson 3.517). */
void sub_08079e70(void);
void sub_0807c484(void);
void sub_0807efec(void);
void sub_0808e054(void);
void sub_0809f808(void);
void sub_080acc8c(void);

/* Record targets no header declares: labels of the data files. */
extern const u8 gUnk_0873F4C8[];
extern const u8 gUnk_0873F4E4[];
extern const u8 gUnk_0873F51C[];
extern const u8 gUnk_0873F538[];
extern const u8 gUnk_0873F570[];
extern const u8 gUnk_0873F58C[];
extern const u8 gUnk_0873F5A8[];
extern const u8 gUnk_0873F5C4[];
extern const u8 gUnk_0873F5DC[];
extern const u8 gUnk_0873F5EC[];
extern const u8 gUnk_0873F5F4[];
extern const u8 gUnk_0873F5FC[];
extern const u8 gUnk_0873F618[];
extern const u8 gUnk_0873F634[];
extern const u8 gUnk_0873F640[];
extern const u8 gUnk_0873F64C[];
extern const u8 gUnk_0873F658[];
extern const u8 gUnk_0873F720[];
extern const u8 gUnk_0873F73C[];
extern const u8 gUnk_0873F774[];
extern const u8 gUnk_0873F7AC[];
extern const u8 gUnk_0873F7C8[];
extern const u8 gUnk_0873F800[];
extern const u8 gUnk_0873F89C[];
extern const u8 gUnk_0873F8A4[];
extern const u8 gUnk_0873F8E4[];
extern const u8 gUnk_0873F944[];
extern const u8 gUnk_08740E00[];
extern const u8 gUnk_08740E54[];
extern const u8 gUnk_08740E70[];
extern const u8 gUnk_08740E8C[];
extern const u8 gUnk_08740EA8[];
extern const u8 gUnk_08740EC4[];
extern const u8 gUnk_08740EE0[];
extern const u8 gUnk_08740EFC[];
extern const u8 gUnk_08740F08[];
extern const u8 gUnk_08740F14[];
extern const u8 gUnk_08740F20[];
extern const u8 gUnk_08740F38[];
extern const u8 gUnk_08740F44[];
extern const u8 gUnk_08740F68[];
extern const u8 gUnk_08740F74[];
extern const u8 gUnk_08740F80[];
extern const u8 gUnk_08740F8C[];
extern const u8 gUnk_08740F98[];
extern const u8 gUnk_0874113C[];
extern const u8 gUnk_08741190[];
extern const u8 gUnk_087411AC[];
extern const u8 gUnk_087411B4[];
extern const u8 gUnk_08741AA4[];
extern const u8 gUnk_08741AC0[];
extern const u8 gUnk_08741AF8[];
extern const u8 gUnk_08741B14[];
extern const u8 gUnk_08741B1C[];
extern const u8 gUnk_08741B24[];
extern const u8 gUnk_08741B2C[];
extern const u8 gUnk_08741B34[];
extern const u8 gUnk_08741B3C[];
extern const u8 gUnk_08741B4C[];
extern const u8 gUnk_08741B54[];
extern const u8 gUnk_08741B5C[];
extern const u8 gUnk_08741B64[];
extern const u8 gUnk_08741B80[];
extern const u8 gUnk_08741B9C[];
extern const u8 gUnk_08741BB8[];
extern const u8 gUnk_08741BD4[];
extern const u8 gUnk_08741BF0[];
extern const u8 gUnk_08741C0C[];
extern const u8 gUnk_08741C28[];
extern const u8 gUnk_08741C44[];
extern const u8 gUnk_08741C60[];
extern const u8 gUnk_08741C7C[];
extern const u8 gUnk_08741C98[];
extern const u8 gUnk_08741CA4[];
extern const u8 gUnk_08741CB0[];
extern const u8 gUnk_08741CBC[];
extern const u8 gUnk_08741CC8[];
extern const u8 gUnk_08741CD4[];
extern const u8 gUnk_08741CEC[];
extern const u8 gUnk_08741CF8[];
extern const u8 gUnk_08741D04[];
extern const u8 gUnk_08741D10[];
extern const u8 gUnk_08741D1C[];
extern const u8 gUnk_08741D28[];
extern const u8 gUnk_08741D34[];
extern const u8 gUnk_08741D40[];
extern const u8 gUnk_08741D4C[];
extern const u8 gUnk_08741D58[];
extern const u8 gUnk_08741F0C[];
extern const u8 gUnk_08741F28[];
extern const u8 gUnk_08741F44[];
extern const u8 gUnk_08741F4C[];
extern const u8 gUnk_08741F58[];
extern const u8 gUnk_08742BF8[];
extern const u8 gUnk_08742C14[];
extern const u8 gUnk_08742CD8[];
extern const u8 gUnk_08742CE0[];
extern const u8 gUnk_08742CE8[];
extern const u8 gUnk_08742CF0[];
extern const u8 gUnk_08742D0C[];
extern const u8 gUnk_08742D28[];
extern const u8 gUnk_08742D44[];
extern const u8 gUnk_08742D60[];
extern const u8 gUnk_08742D7C[];
extern const u8 gUnk_08742D98[];
extern const u8 gUnk_08742DB4[];
extern const u8 gUnk_08742DD0[];
extern const u8 gUnk_08742DEC[];
extern const u8 gUnk_08742E08[];
extern const u8 gUnk_08742E14[];
extern const u8 gUnk_08742E20[];
extern const u8 gUnk_08742E2C[];
extern const u8 gUnk_08742E38[];
extern const u8 gUnk_08742E44[];
extern const u8 gUnk_08742E50[];
extern const u8 gUnk_08742E68[];
extern const u8 gUnk_08742E74[];
extern const u8 gUnk_08742E80[];
extern const u8 gUnk_08742E8C[];
extern const u8 gUnk_08742E98[];
extern const u8 gUnk_0874306C[];
extern const u8 gUnk_08743088[];
extern const u8 gUnk_087430A4[];
extern const u8 gUnk_087430C0[];
extern const u8 gUnk_087430DC[];
extern const u8 gUnk_087430E4[];
extern const u8 gUnk_087430EC[];
extern const u8 gUnk_08743108[];
extern const u8 gUnk_08743124[];
extern const u8 gUnk_08743130[];
extern const u8 gUnk_08743414[];
extern const u8 gUnk_08743470[];
extern const u8 gUnk_0874348C[];
extern const u8 gUnk_087434C4[];
extern const u8 gUnk_087434E0[];
extern const u8 gUnk_087434FC[];
extern const u8 gUnk_08743518[];
extern const u8 gUnk_08743534[];
extern const u8 gUnk_08743540[];
extern const u8 gUnk_0874354C[];
extern const u8 gUnk_08743564[];
extern const u8 gUnk_08743570[];
extern const u8 gUnk_0874357C[];
extern const u8 gUnk_0874369C[];
extern const u8 gUnk_087436B8[];
extern const u8 gUnk_087436D4[];
extern const u8 gUnk_087436DC[];
extern const u8 gUnk_087436E4[];
extern const u8 gUnk_08743700[];
extern const u8 gUnk_0874371C[];
extern const u8 gUnk_08743728[];
extern const u8 gUnk_08743BD0[];
extern const u8 gUnk_08743BEC[];
extern const u8 gUnk_08743C40[];
extern const u8 gUnk_08743C94[];
extern const u8 gUnk_08743CE8[];
extern const u8 gUnk_08743D3C[];
extern const u8 gUnk_08743DE4[];
extern const u8 gUnk_08743E00[];
extern const u8 gUnk_08743E54[];
extern const u8 gUnk_08743E8C[];
extern const u8 gUnk_08743EFC[];
extern const u8 gUnk_08743F18[];
extern const u8 gUnk_08743F6C[];
extern const u8 gUnk_08744014[];
extern const u8 gUnk_0874401C[];
extern const u8 gUnk_08744024[];
extern const u8 gUnk_0874407C[];
extern const u8 gUnk_08744098[];
extern const u8 gUnk_087440B4[];
extern const u8 gUnk_087440D0[];
extern const u8 gUnk_087440E8[];
extern const u8 gUnk_08744100[];
extern const u8 gUnk_0874425C[];
extern const u8 gUnk_08744278[];
extern const u8 gUnk_08744294[];
extern const u8 gUnk_087442B0[];
extern const u8 gUnk_087442B8[];
extern const u8 gUnk_087442C0[];
extern const u8 gUnk_087442C8[];
extern const u8 gUnk_087442E4[];
extern const u8 gUnk_08744300[];
extern const u8 gUnk_08744318[];
extern const u8 gUnk_08744330[];
extern const u8 gUnk_087449E8[];
extern const u8 gUnk_08744A20[];
extern const u8 gUnk_08744A74[];
extern const u8 gUnk_08744AC8[];
extern const u8 gUnk_08744B54[];
extern const u8 gUnk_08744BA8[];
extern const u8 gUnk_08744C34[];
extern const u8 gUnk_08744C88[];
extern const u8 gUnk_08744CDC[];
extern const u8 gUnk_08744D14[];
extern const u8 gUnk_08744D68[];
extern const u8 gUnk_08744DBC[];
extern const u8 gUnk_08744E10[];
extern const u8 gUnk_08744E64[];
extern const u8 gUnk_08744EB8[];
extern const u8 gUnk_08744F0C[];
extern const u8 gUnk_08744F60[];
extern const u8 gUnk_08744FB4[];
extern const u8 gUnk_08745008[];
extern const u8 gUnk_0874505C[];
extern const u8 gUnk_087450B0[];
extern const u8 gUnk_0874521C[];
extern const u8 gUnk_08745254[];
extern const u8 gUnk_087452FC[];
extern const u8 gUnk_08745304[];
extern const u8 gUnk_08745314[];
extern const u8 gUnk_087453BC[];
extern const u8 gUnk_087453D8[];
extern const u8 gUnk_087453F4[];
extern const u8 gUnk_08745410[];
extern const u8 gUnk_08745428[];
extern const u8 gUnk_08745440[];
extern const u8 gUnk_08745588[];
extern const u8 gUnk_087455A4[];
extern const u8 gUnk_087455C0[];
extern const u8 gUnk_087455C8[];
extern const u8 gUnk_087455E4[];
extern const u8 gUnk_08745600[];
extern const u8 gUnk_0874560C[];
extern const u8 gUnk_08745868[];
extern const u8 gUnk_087458D8[];
extern const u8 gUnk_08745964[];
extern const u8 gUnk_08745A0C[];
extern const u8 gUnk_08745A1C[];
extern const u8 gUnk_08745A3C[];
extern const u8 gUnk_08745A58[];
extern const u8 gUnk_08745A74[];
extern const u8 gUnk_08745A8C[];
extern const u8 gUnk_08745BB4[];
extern const u8 gUnk_08745BEC[];
extern const u8 gUnk_08745C5C[];
extern const u8 gUnk_08745C78[];
extern const u8 gUnk_08745C88[];
extern const u8 gUnk_08745C90[];
extern const u8 gUnk_08745CAC[];
extern const u8 gUnk_08745CC8[];
extern const u8 gUnk_08745CE0[];
extern const u8 gUnk_08747EBC[];
extern const u8 gUnk_08747ED8[];
extern const u8 gUnk_08747F10[];
extern const u8 gUnk_08747F2C[];
extern const u8 gUnk_08747F48[];
extern const u8 gUnk_08747F64[];
extern const u8 gUnk_08748178[];
extern const u8 gUnk_08748180[];
extern const u8 gUnk_08748188[];
extern const u8 gUnk_08748190[];
extern const u8 gUnk_087481A0[];
extern const u8 gUnk_087481BC[];
extern const u8 gUnk_087481D8[];
extern const u8 gUnk_087481F4[];
extern const u8 gUnk_08748210[];
extern const u8 gUnk_0874821C[];
extern const u8 gUnk_08748228[];
extern const u8 gUnk_08748234[];
extern const u8 gUnk_08748240[];
extern const u8 gUnk_0874824C[];
extern const u8 gUnk_08748258[];
extern const u8 gUnk_087487E8[];
extern const u8 gUnk_08748804[];
extern const u8 gUnk_0874883C[];
extern const u8 gUnk_08748858[];
extern const u8 gUnk_087488AC[];
extern const u8 gUnk_087488C8[];
extern const u8 gUnk_08748900[];
extern const u8 gUnk_08748908[];
extern const u8 gUnk_08748910[];
extern const u8 gUnk_08748930[];
extern const u8 gUnk_0874894C[];
extern const u8 gUnk_08748968[];
extern const u8 gUnk_08748980[];
extern const u8 gUnk_08748998[];
extern const u8 gUnk_08748BB8[];
extern const u8 gUnk_08748BF0[];
extern const u8 gUnk_08748C60[];
extern const u8 gUnk_08748C7C[];
extern const u8 gUnk_08748C98[];
extern const u8 gUnk_08748CB4[];
extern const u8 gUnk_08748CD0[];
extern const u8 gUnk_08748CD8[];
extern const u8 gUnk_08748CE0[];
extern const u8 gUnk_08748CE8[];
extern const u8 gUnk_08748CF0[];
extern const u8 gUnk_08748CF8[];
extern const u8 gUnk_08748D10[];
extern const u8 gUnk_08748D1C[];
extern const u8 gUnk_08749598[];
extern const u8 gUnk_08749608[];
extern const u8 gUnk_087496E8[];
extern const u8 gUnk_0874973C[];
extern const u8 gUnk_08749790[];
extern const u8 gUnk_08749870[];
extern const u8 gUnk_0874988C[];
extern const u8 gUnk_08749AD8[];
extern const u8 gUnk_08749AE0[];
extern const u8 gUnk_08749B08[];
extern const u8 gUnk_08749B24[];
extern const u8 gUnk_08749B3C[];
extern const u8 gUnk_08749B54[];
extern const u8 gUnk_08749C70[];
extern const u8 gUnk_08749C8C[];
extern const u8 gUnk_08749CA8[];
extern const u8 gUnk_08749CC4[];
extern const u8 gUnk_08749CCC[];
extern const u8 gUnk_08749CD4[];
extern const u8 gUnk_08749CE0[];
extern const u8 gUnk_0874B38C[];
extern const u8 gUnk_0874B3C4[];
extern const u8 gUnk_0874B3E0[];
extern const u8 gUnk_0874B418[];
extern const u8 gUnk_0874B434[];
extern const u8 gUnk_0874B4C0[];
extern const u8 gUnk_0874B4E0[];
extern const u8 gUnk_0874B4F8[];
extern const u8 gUnk_0874B510[];
extern const u8 gUnk_0874BB7C[];
extern const u8 gUnk_0874BBB4[];
extern const u8 gUnk_0874BF34[];
extern const u8 gUnk_0874BF50[];
extern const u8 gUnk_0874BF58[];
extern const u8 gUnk_0874BF74[];
extern const u8 gUnk_0874BF80[];
extern const u8 gUnk_0874BF8C[];
extern const u8 gUnk_0874BF98[];
extern const u8 gUnk_0874BFA4[];
extern const u8 gUnk_0874BFB0[];
extern const u8 gUnk_0874BFBC[];
extern const u8 gUnk_0874BFC8[];
extern const u8 gUnk_0874BFE0[];
extern const u8 gUnk_0874BFEC[];
extern const u8 gUnk_0874C184[];
extern const u8 gUnk_0874C1BC[];
extern const u8 gUnk_0874C1F4[];
extern const u8 gUnk_0874C204[];
extern const u8 gUnk_0874C3D0[];
extern const u8 gUnk_0874C3EC[];
extern const u8 gUnk_0874C408[];
extern const u8 gUnk_0874C410[];
extern const u8 gUnk_0874C418[];
extern const u8 gUnk_0874C434[];
extern const u8 gUnk_0874C440[];

/* ActorAux records defined further down. */
extern struct ActorAux gUnk_0873F8EC;
extern struct ActorAux gUnk_0874402C;
extern struct ActorAux gUnk_08744054;
extern struct ActorAux gUnk_0874406C;
extern struct ActorAux gUnk_0874531C;
extern struct ActorAux gUnk_08745354;
extern struct ActorAux gUnk_087453B4;
extern struct ActorAux gUnk_08745A2C;
extern struct ActorAux gUnk_08748198;
extern struct ActorAux gUnk_08748918;
extern struct ActorAux gUnk_08748920;
extern struct ActorAux gUnk_08748928;
extern struct ActorAux gUnk_08749AE8;
extern struct ActorAux gUnk_08749AF0;
extern struct ActorAux gUnk_08749AF8;
extern struct ActorAux gUnk_0874B4C8;
extern struct ActorAux gUnk_0874B4D0;
extern struct ActorAux gUnk_0874C1FC;

/* ---- 0x0873F2B8-0x0873F4C8: 12 record(s), section .actor_rec_0873f2b8 ---- */
struct ActorDef gUnk_0873F2B8 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4C8,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F5FC,
    .hitReactions = (u32)gUnk_0873F634,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F2E4 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)gUnk_0873F618,
    .hitReactions = (u32)gUnk_0873F640,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F310 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)gUnk_0873F618,
    .hitReactions = (u32)gUnk_0873F640,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F33C ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)gUnk_0873F618,
    .hitReactions = (u32)gUnk_0873F640,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F368 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x1,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F4E4,
    .terrainBox = (s32)gUnk_0873F5C4,
    .terrainHandlers = (u32)gUnk_0873F618,
    .hitReactions = (u32)gUnk_0873F640,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F394 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F51C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0873F64C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F3C0 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F538,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F3EC ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F570,
    .terrainBox = (s32)gUnk_0873F5DC,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F418 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F58C,
    .terrainBox = (s32)gUnk_0873F5EC,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F444 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F5A8,
    .terrainBox = (s32)gUnk_0873F5F4,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_0873F658,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F470 ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F49C ACTOR_REC(0873f2b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0873F664-0x0873F6E8: 3 record(s), section .actor_rec_0873f664 ---- */
struct ActorDef gUnk_0873F664 ACTOR_REC(0873f664) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F7C8,
    .terrainBox = (s32)gUnk_0873F8E4,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0873F944,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F690 ACTOR_REC(0873f664) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 1,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0873F7C8,
    .terrainBox = (s32)gUnk_0873F8E4,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0873F944,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0873F6BC ACTOR_REC(0873f664) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F800,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0873F8EC-0x0873F8F4: 1 record(s), section .actor_rec_0873f8ec ---- */
struct ActorAux gUnk_0873F8EC ACTOR_REC(0873f8ec) = {
    .hitDuration = 30,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0873F7AC,
};

/* ---- 0x08740BD4-0x08740DE4: 12 record(s), section .actor_rec_08740bd4 ---- */
struct ActorDef gUnk_08740BD4 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)gUnk_08740E54,
    .hitReactions = (u32)gUnk_08740EFC,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740C00 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 10,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F73C,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)gUnk_08740E54,
    .hitReactions = (u32)gUnk_08740F08,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740C2C ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 13,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_08740E70,
    .hitReactions = (u32)gUnk_08740F14,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740C58 ACTOR_REC(08740bd4) = {
    .health1Player = 1,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 100,
    .ability = 20,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_08740E8C,
    .hitReactions = (u32)gUnk_08740F20,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740C84 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 2,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F774,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08740EA8,
    .hitReactions = (u32)gUnk_08740F38,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_08079e70,
};
struct ActorDef gUnk_08740CB0 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 2,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08740E00,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08740F44,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740CDC ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = 4,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08740EC4,
    .hitReactions = (u32)gUnk_08740F68,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740D08 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = 4,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08740EC4,
    .hitReactions = (u32)gUnk_08740F68,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740D34 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F7AC,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08740F74,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740D60 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 700,
    .ability = 12,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08740EE0,
    .hitReactions = (u32)gUnk_08740F80,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08740D8C ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 700,
    .ability = 24,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08740F8C,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0807c484,
};
struct ActorDef gUnk_08740DB8 ACTOR_REC(08740bd4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 10,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08740F98,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x087410E4-0x0874113C: 2 record(s), section .actor_rec_087410e4 ---- */
struct ActorDef gUnk_087410E4 ACTOR_REC(087410e4) = {
    .health1Player = 1,
    .health2Players = 1,
    .health3Players = 1,
    .health4Players = 1,
    .score = 0,
    .ability = 13,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0874113C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741110 ACTOR_REC(087410e4) = {
    .health1Player = 1,
    .health2Players = 1,
    .health3Players = 1,
    .health4Players = 1,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741190,
    .terrainBox = (s32)gUnk_087411AC,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_087411B4,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x087417B8-0x08741AA4: 17 record(s), section .actor_rec_087417b8 ---- */
struct ActorDef gUnk_087417B8 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = 17,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B14,
    .terrainHandlers = (u32)gUnk_08741B64,
    .hitReactions = (u32)gUnk_08741C98,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087417E4 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 3,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B1C,
    .terrainHandlers = (u32)gUnk_08741B80,
    .hitReactions = (u32)gUnk_08741CA4,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741810 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B24,
    .terrainHandlers = (u32)gUnk_08741B9C,
    .hitReactions = (u32)gUnk_08741CB0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874183C ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B24,
    .terrainHandlers = (u32)gUnk_08741B9C,
    .hitReactions = (u32)gUnk_08741CBC,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741868 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741AA4,
    .terrainBox = (s32)gUnk_08741B2C,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741894 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = 21,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741AC0,
    .terrainBox = (s32)gUnk_08741B34,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08741CC8,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0807efec,
};
struct ActorDef gUnk_087418C0 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741BB8,
    .hitReactions = (u32)gUnk_08741CD4,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087418EC ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = 19,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741BD4,
    .hitReactions = (u32)gUnk_08741CEC,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741918 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 15,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B3C,
    .terrainHandlers = (u32)gUnk_08741BF0,
    .hitReactions = (u32)gUnk_08741CF8,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741944 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 1,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B4C,
    .terrainHandlers = (u32)gUnk_08741C0C,
    .hitReactions = (u32)gUnk_08741D04,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741970 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741C28,
    .hitReactions = (u32)gUnk_08741D10,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874199C ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741AF8,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741C44,
    .hitReactions = (u32)gUnk_08741D1C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087419C8 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741AF8,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741C44,
    .hitReactions = (u32)gUnk_08741D28,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087419F4 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741C44,
    .hitReactions = (u32)gUnk_08741D34,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741A20 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08741C44,
    .hitReactions = (u32)gUnk_08741D40,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741A4C ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 800,
    .ability = 8,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B54,
    .terrainHandlers = (u32)gUnk_08741C60,
    .hitReactions = (u32)gUnk_08741D4C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741A78 ACTOR_REC(087417b8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = 5,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08741B5C,
    .terrainHandlers = (u32)gUnk_08741C7C,
    .hitReactions = (u32)gUnk_08741D58,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08741EB4-0x08741F0C: 2 record(s), section .actor_rec_08741eb4 ---- */
struct ActorDef gUnk_08741EB4 ACTOR_REC(08741eb4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741F0C,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08741F4C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08741EE0 ACTOR_REC(08741eb4) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 1,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08741F28,
    .terrainBox = (s32)gUnk_08741F44,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08741F58,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x087429E8-0x08742BF8: 12 record(s), section .actor_rec_087429e8 ---- */
struct ActorDef gUnk_087429E8 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 11,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_08742CF0,
    .hitReactions = (u32)gUnk_08742E08,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742A14 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 500,
    .ability = 14,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08742BF8,
    .terrainBox = (s32)gUnk_08742CD8,
    .terrainHandlers = (u32)gUnk_08742D0C,
    .hitReactions = (u32)gUnk_08742E14,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742A40 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 16,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)gUnk_08742D28,
    .hitReactions = (u32)gUnk_08742E20,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742A6C ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 10,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F73C,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)gUnk_08742D28,
    .hitReactions = (u32)gUnk_08742E2C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742A98 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08742E38,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742AC4 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08742D44,
    .hitReactions = (u32)gUnk_08742E44,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742AF0 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F8A4,
    .terrainHandlers = (u32)gUnk_08742D60,
    .hitReactions = (u32)gUnk_08742E50,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742B1C ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 18,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08742C14,
    .terrainBox = (s32)gUnk_08742CE0,
    .terrainHandlers = (u32)gUnk_08742D7C,
    .hitReactions = (u32)gUnk_08742E68,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742B48 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08742D98,
    .hitReactions = (u32)gUnk_08742E74,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742B74 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_08742DB4,
    .hitReactions = (u32)gUnk_08742E80,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742BA0 ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 300,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_08742DD0,
    .hitReactions = (u32)gUnk_08742E8C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742BCC ACTOR_REC(087429e8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_08742CE8,
    .terrainHandlers = (u32)gUnk_08742DEC,
    .hitReactions = (u32)gUnk_08742E98,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08742FBC-0x0874306C: 4 record(s), section .actor_rec_08742fbc ---- */
struct ActorDef gUnk_08742FBC ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0874306C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08742FE8 ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08743088,
    .terrainBox = (s32)gUnk_087430DC,
    .terrainHandlers = (u32)gUnk_087430EC,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08743014 ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_087430A4,
    .terrainBox = (s32)gUnk_087430E4,
    .terrainHandlers = (u32)gUnk_08743108,
    .hitReactions = (u32)gUnk_08743124,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08743040 ACTOR_REC(08742fbc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_087430C0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08743130,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874330C-0x08743414: 6 record(s), section .actor_rec_0874330c ---- */
struct ActorDef gUnk_0874330C ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 200,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_087434C4,
    .hitReactions = (u32)gUnk_08743534,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08743338 ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 6,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08743540,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0808e054,
};
struct ActorDef gUnk_08743364 ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_087434E0,
    .hitReactions = (u32)gUnk_0874354C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08743390 ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_08743470,
    .terrainBox = (s32)gUnk_08743414,
    .terrainHandlers = (u32)gUnk_087434FC,
    .hitReactions = (u32)gUnk_08743564,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087433BC ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0873F720,
    .terrainBox = (s32)gUnk_0873F89C,
    .terrainHandlers = (u32)gUnk_08743518,
    .hitReactions = (u32)gUnk_08743570,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087433E8 ACTOR_REC(0874330c) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 400,
    .ability = 10,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0874348C,
    .terrainBox = (s32)gUnk_08743414,
    .terrainHandlers = (u32)gUnk_087434FC,
    .hitReactions = (u32)gUnk_0874357C,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08743644-0x0874369C: 2 record(s), section .actor_rec_08743644 ---- */
struct ActorDef gUnk_08743644 ACTOR_REC(08743644) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_0874369C,
    .terrainBox = (s32)gUnk_087436D4,
    .terrainHandlers = (u32)gUnk_087436E4,
    .hitReactions = (u32)gUnk_0874371C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08743670 ACTOR_REC(08743644) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0873F8EC,
    .attackBox = (u32)gUnk_087436B8,
    .terrainBox = (s32)gUnk_087436DC,
    .terrainHandlers = (u32)gUnk_08743700,
    .hitReactions = (u32)gUnk_08743728,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08743B4C-0x08743BD0: 3 record(s), section .actor_rec_08743b4c ---- */
struct ActorDef gBonkersDef ACTOR_REC(08743b4c) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 1200,
    .ability = 9,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874402C,
    .attackBox = (u32)gUnk_08743BD0,
    .terrainBox = (s32)gUnk_08744014,
    .terrainHandlers = (u32)gUnk_0874407C,
    .hitReactions = (u32)gUnk_087440D0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gPoppyBrosSrDef ACTOR_REC(08743b4c) = {
    .health1Player = 24,
    .health2Players = 32,
    .health3Players = 38,
    .health4Players = 43,
    .score = 1000,
    .ability = 20,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08744054,
    .attackBox = (u32)gUnk_08743DE4,
    .terrainBox = (s32)gUnk_0874401C,
    .terrainHandlers = (u32)gUnk_08744098,
    .hitReactions = (u32)gUnk_087440E8,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gBugzzyDef ACTOR_REC(08743b4c) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 2000,
    .ability = 22,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874406C,
    .attackBox = (u32)gUnk_08743EFC,
    .terrainBox = (s32)gUnk_08744024,
    .terrainHandlers = (u32)gUnk_087440B4,
    .hitReactions = (u32)gUnk_08744100,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874402C-0x0874407C: 10 record(s), section .actor_rec_0874402c ---- */
struct ActorAux gUnk_0874402C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743BEC,
};
struct ActorAux gUnk_08744034 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743C40,
};
struct ActorAux gUnk_0874403C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743C94,
};
struct ActorAux gUnk_08744044 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743CE8,
};
struct ActorAux gUnk_0874404C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743D3C,
};
struct ActorAux gUnk_08744054 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743E00,
};
struct ActorAux gUnk_0874405C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743E54,
};
struct ActorAux gUnk_08744064 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743E8C,
};
struct ActorAux gUnk_0874406C ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743F18,
};
struct ActorAux gUnk_08744074 ACTOR_REC(0874402c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08743F6C,
};

/* ---- 0x087441D8-0x0874425C: 3 record(s), section .actor_rec_087441d8 ---- */
struct ActorDef gUnk_087441D8 ACTOR_REC(087441d8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874425C,
    .terrainBox = (s32)gUnk_087442B0,
    .terrainHandlers = (u32)gUnk_087442C8,
    .hitReactions = (u32)gUnk_08744300,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08744204 ACTOR_REC(087441d8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08744278,
    .terrainBox = (s32)gUnk_087442B8,
    .terrainHandlers = (u32)gUnk_087442E4,
    .hitReactions = (u32)gUnk_08744318,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08744230 ACTOR_REC(087441d8) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08744294,
    .terrainBox = (s32)gUnk_087442C0,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08744330,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08744964-0x087449E8: 3 record(s), section .actor_rec_08744964 ---- */
struct ActorDef gGrandWheelieDef ACTOR_REC(08744964) = {
    .health1Player = 25,
    .health2Players = 33,
    .health3Players = 40,
    .health4Players = 45,
    .score = 1000,
    .ability = 8,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874531C,
    .attackBox = (u32)gUnk_087449E8,
    .terrainBox = (s32)gUnk_087452FC,
    .terrainHandlers = (u32)gUnk_087453BC,
    .hitReactions = (u32)gUnk_08745410,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gFireLionDef ACTOR_REC(08744964) = {
    .health1Player = 27,
    .health2Players = 36,
    .health3Players = 43,
    .health4Players = 48,
    .score = 1500,
    .ability = 5,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08745354,
    .attackBox = (u32)gUnk_08744CDC,
    .terrainBox = (s32)gUnk_08745304,
    .terrainHandlers = (u32)gUnk_087453D8,
    .hitReactions = (u32)gUnk_08745428,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gPhanPhanDef ACTOR_REC(08744964) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 1500,
    .ability = 23,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_087453B4,
    .attackBox = (u32)gUnk_0874521C,
    .terrainBox = (s32)gUnk_08745314,
    .terrainHandlers = (u32)gUnk_087453F4,
    .hitReactions = (u32)gUnk_08745440,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874531C-0x087453BC: 20 record(s), section .actor_rec_0874531c ---- */
struct ActorAux gUnk_0874531C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744A20,
};
struct ActorAux gUnk_08745324 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744A74,
};
struct ActorAux gUnk_0874532C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744AC8,
};
struct ActorAux gUnk_08745334 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744B54,
};
struct ActorAux gUnk_0874533C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744BA8,
};
struct ActorAux gUnk_08745344 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744C34,
};
struct ActorAux gUnk_0874534C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744C88,
};
struct ActorAux gUnk_08745354 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744D14,
};
struct ActorAux gUnk_0874535C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744D68,
};
struct ActorAux gUnk_08745364 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744DBC,
};
struct ActorAux gUnk_0874536C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744E10,
};
struct ActorAux gUnk_08745374 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744E64,
};
struct ActorAux gUnk_0874537C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744EB8,
};
struct ActorAux gUnk_08745384 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744F0C,
};
struct ActorAux gUnk_0874538C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744F60,
};
struct ActorAux gUnk_08745394 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08744FB4,
};
struct ActorAux gUnk_0874539C ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08745008,
};
struct ActorAux gUnk_087453A4 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874505C,
};
struct ActorAux gUnk_087453AC ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_087450B0,
};
struct ActorAux gUnk_087453B4 ACTOR_REC(0874531c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08745254,
};

/* ---- 0x08745530-0x08745588: 2 record(s), section .actor_rec_08745530 ---- */
struct ActorDef gUnk_08745530 ACTOR_REC(08745530) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08745588,
    .terrainBox = (s32)gUnk_087455C0,
    .terrainHandlers = (u32)gUnk_087455C8,
    .hitReactions = (u32)gUnk_08745600,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874555C ACTOR_REC(08745530) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_087455A4,
    .terrainBox = (s32)gUnk_0873F894,
    .terrainHandlers = (u32)gUnk_087455E4,
    .hitReactions = (u32)gUnk_0874560C,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08745810-0x08745868: 2 record(s), section .actor_rec_08745810 ---- */
struct ActorDef gMrFrostyDef ACTOR_REC(08745810) = {
    .health1Player = 22,
    .health2Players = 29,
    .health3Players = 35,
    .health4Players = 39,
    .score = 1500,
    .ability = 14,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08745A2C,
    .attackBox = (u32)gUnk_08745868,
    .terrainBox = (s32)gUnk_08745A0C,
    .terrainHandlers = (u32)gUnk_08745A3C,
    .hitReactions = (u32)gUnk_08745A74,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gMrTickTockDef ACTOR_REC(08745810) = {
    .health1Player = 26,
    .health2Players = 35,
    .health3Players = 41,
    .health4Players = 46,
    .score = 1500,
    .ability = 7,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08745A2C,
    .attackBox = (u32)gUnk_08745964,
    .terrainBox = (s32)gUnk_08745A1C,
    .terrainHandlers = (u32)gUnk_08745A58,
    .hitReactions = (u32)gUnk_08745A8C,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08745A2C-0x08745A34: 1 record(s), section .actor_rec_08745a2c ---- */
struct ActorAux gUnk_08745A2C ACTOR_REC(08745a2c) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_087458D8,
};

/* ---- 0x08745B30-0x08745BB4: 3 record(s), section .actor_rec_08745b30 ---- */
struct ActorDef gUnk_08745B30 ACTOR_REC(08745b30) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08745BB4,
    .terrainBox = (s32)gUnk_08745C78,
    .terrainHandlers = (u32)gUnk_08745C90,
    .hitReactions = (u32)gUnk_08745CC8,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08745B5C ACTOR_REC(08745b30) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08745BEC,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08745B88 ACTOR_REC(08745b30) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08745C5C,
    .terrainBox = (s32)gUnk_08745C88,
    .terrainHandlers = (u32)gUnk_08745CAC,
    .hitReactions = (u32)gUnk_08745CE0,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08747C80-0x08747EBC: 13 record(s), section .actor_rec_08747c80 ---- */
struct ActorDef gUnk_08747C80 ACTOR_REC(08747c80) = {
    .health1Player = 55,
    .health2Players = 72,
    .health3Players = 48,
    .health4Players = 60,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747CAC ACTOR_REC(08747c80) = {
    .health1Player = 64,
    .health2Players = 60,
    .health3Players = 64,
    .health4Players = 66,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747CD8 ACTOR_REC(08747c80) = {
    .health1Player = 54,
    .health2Players = 52,
    .health3Players = 72,
    .health4Players = 48,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747D04 ACTOR_REC(08747c80) = {
    .health1Player = 55,
    .health2Players = 64,
    .health3Players = 66,
    .health4Players = 56,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747D30 ACTOR_REC(08747c80) = {
    .health1Player = 60,
    .health2Players = 54,
    .health3Players = 48,
    .health4Players = 60,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747D5C ACTOR_REC(08747c80) = {
    .health1Player = 6,
    .health2Players = 6,
    .health3Players = 6,
    .health4Players = 6,
    .score = 500,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gUnk_08748178,
    .terrainHandlers = (u32)gUnk_087481A0,
    .hitReactions = (u32)gUnk_08748210,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0809f808,
};
struct ActorDef gUnk_08747D88 ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08747F10,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874821C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747DB4 ACTOR_REC(08747c80) = {
    .health1Player = 8,
    .health2Players = 8,
    .health3Players = 8,
    .health4Players = 8,
    .score = 500,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gUnk_08748180,
    .terrainHandlers = (u32)gUnk_087481BC,
    .hitReactions = (u32)gUnk_08748228,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0809f808,
};
struct ActorDef gUnk_08747DE0 ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08747F2C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747E0C ACTOR_REC(08747c80) = {
    .health1Player = 6,
    .health2Players = 6,
    .health3Players = 6,
    .health4Players = 6,
    .score = 500,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gUnk_08748188,
    .terrainHandlers = (u32)gUnk_087481D8,
    .hitReactions = (u32)gUnk_08748234,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0809f808,
};
struct ActorDef gUnk_08747E38 ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08747F48,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748240,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08747E64 ACTOR_REC(08747c80) = {
    .health1Player = 4,
    .health2Players = 4,
    .health3Players = 4,
    .health4Players = 4,
    .score = 500,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748198,
    .attackBox = (u32)gUnk_08747EBC,
    .terrainBox = (s32)gUnk_08748190,
    .terrainHandlers = (u32)gUnk_087481F4,
    .hitReactions = (u32)gUnk_0874824C,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_0809f808,
};
struct ActorDef gUnk_08747E90 ACTOR_REC(08747c80) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08747F64,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748258,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08748198-0x087481A0: 1 record(s), section .actor_rec_08748198 ---- */
struct ActorAux gUnk_08748198 ACTOR_REC(08748198) = {
    .hitDuration = 18,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08747ED8,
};

/* ---- 0x08748764-0x087487E8: 3 record(s), section .actor_rec_08748764 ---- */
struct ActorDef gKingDededeDef ACTOR_REC(08748764) = {
    .health1Player = 100,
    .health2Players = 135,
    .health3Players = 160,
    .health4Players = 180,
    .score = 80000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748918,
    .attackBox = (u32)gUnk_087487E8,
    .terrainBox = (s32)gUnk_08748900,
    .terrainHandlers = (u32)gUnk_08748930,
    .hitReactions = (u32)gUnk_08748968,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gMrShineAndMrBrightDef ACTOR_REC(08748764) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 15000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748920,
    .attackBox = (u32)gUnk_0874883C,
    .terrainBox = (s32)gUnk_08748908,
    .terrainHandlers = (u32)gUnk_0874894C,
    .hitReactions = (u32)gUnk_08748980,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_087487BC ACTOR_REC(08748764) = {
    .health1Player = 30,
    .health2Players = 40,
    .health3Players = 48,
    .health4Players = 54,
    .score = 15000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08748928,
    .attackBox = (u32)gUnk_087488AC,
    .terrainBox = (s32)gUnk_08748910,
    .terrainHandlers = (u32)gUnk_0874894C,
    .hitReactions = (u32)gUnk_08748998,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08748918-0x08748930: 3 record(s), section .actor_rec_08748918 ---- */
struct ActorAux gUnk_08748918 ACTOR_REC(08748918) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08748804,
};
struct ActorAux gUnk_08748920 ACTOR_REC(08748918) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08748858,
};
struct ActorAux gUnk_08748928 ACTOR_REC(08748918) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_087488C8,
};

/* ---- 0x08748AB0-0x08748BB8: 6 record(s), section .actor_rec_08748ab0 ---- */
struct ActorDef gUnk_08748AB0 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748BB8,
    .terrainBox = (s32)gUnk_08748CD0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748CF8,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748ADC ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748BF0,
    .terrainBox = (s32)gUnk_08748CD8,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B08 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748C60,
    .terrainBox = (s32)gUnk_08748CE0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748D10,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B34 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 3,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748C7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748D10,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B60 ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748C98,
    .terrainBox = (s32)gUnk_08748CE8,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748D1C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08748B8C ACTOR_REC(08748ab0) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08748CB4,
    .terrainBox = (s32)gUnk_08748CF0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_08748D1C,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08749514-0x08749598: 3 record(s), section .actor_rec_08749514 ---- */
struct ActorDef gMetaKnightDef ACTOR_REC(08749514) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 60000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08749AE8,
    .attackBox = (u32)gUnk_08749598,
    .terrainBox = (s32)gUnk_08749AD8,
    .terrainHandlers = (u32)gUnk_08749B08,
    .hitReactions = (u32)gUnk_08749B24,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gKrackoDef ACTOR_REC(08749514) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 40000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08749AF0,
    .attackBox = (u32)gUnk_087496E8,
    .terrainBox = (s32)gUnk_08749AE0,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08749B3C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gNightmareWizardDef ACTOR_REC(08749514) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 200000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_08749AF8,
    .attackBox = (u32)gUnk_08749790,
    .terrainBox = 0,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08749B54,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x08749AE8-0x08749B08: 4 record(s), section .actor_rec_08749ae8 ---- */
struct ActorAux gUnk_08749AE8 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08749608,
};
struct ActorAux gUnk_08749AF0 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874973C,
};
struct ActorAux gUnk_08749AF8 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_08749870,
};
struct ActorAux gUnk_08749B00 ACTOR_REC(08749ae8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874988C,
};

/* ---- 0x08749BEC-0x08749C70: 3 record(s), section .actor_rec_08749bec ---- */
struct ActorDef gUnk_08749BEC ACTOR_REC(08749bec) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08749C70,
    .terrainBox = (s32)gUnk_08749CC4,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_08749C18 ACTOR_REC(08749bec) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 10,
    .ability = 15,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08749C8C,
    .terrainBox = (s32)gUnk_08741B3C,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08749CD4,
    .unk24 = NULL,
    .teardown = (void (*)(void))sub_080acc8c,
};
struct ActorDef gUnk_08749C44 ACTOR_REC(08749bec) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_08749CA8,
    .terrainBox = (s32)gUnk_08749CCC,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_08749CE0,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874B2DC-0x0874B38C: 4 record(s), section .actor_rec_0874b2dc ---- */
struct ActorDef gPaintRollerDef ACTOR_REC(0874b2dc) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 30000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874B4C8,
    .attackBox = (u32)gUnk_0874B38C,
    .terrainBox = (s32)gUnk_0874B4C0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874B4E0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gHeavyMoleDef ACTOR_REC(0874b2dc) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 50000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874B4D0,
    .attackBox = (u32)gUnk_0874B3E0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874B4F8,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gNightmarePowerOrbDef ACTOR_REC(0874b2dc) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 50000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874B434,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874B510,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B360 ACTOR_REC(0874b2dc) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874B4C8-0x0874B4D8: 2 record(s), section .actor_rec_0874b4c8 ---- */
struct ActorAux gUnk_0874B4C8 ACTOR_REC(0874b4c8) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874B3C4,
};
struct ActorAux gUnk_0874B4D0 ACTOR_REC(0874b4c8) = {
    .hitDuration = 54,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874B418,
};

/* ---- 0x0874B940-0x0874BB7C: 13 record(s), section .actor_rec_0874b940 ---- */
struct ActorDef gUnk_0874B940 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B96C ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 8,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)gUnk_0874BF58,
    .hitReactions = (u32)gUnk_0874BF74,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B998 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)gUnk_0874BF58,
    .hitReactions = (u32)gUnk_0874BF80,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B9C4 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)gUnk_0874BF58,
    .hitReactions = (u32)gUnk_0874BF8C,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874B9F0 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 7,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)gUnk_0874BF58,
    .hitReactions = (u32)gUnk_0874BF98,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BA1C ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 18,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874BFA4,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BA48 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 20,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)gUnk_0874BF58,
    .hitReactions = (u32)gUnk_0874BFB0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BA74 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 2,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874BFBC,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BAA0 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 10,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BB7C,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874BFC8,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BACC ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BBB4,
    .terrainBox = (s32)gUnk_0874BF50,
    .terrainHandlers = (u32)gUnk_0874BF58,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BAF8 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = 0,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = 0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BB24 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 9,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BF34,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874BFE0,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874BB50 ACTOR_REC(0874b940) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 11,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874BF34,
    .terrainBox = 0,
    .terrainHandlers = 0,
    .hitReactions = (u32)gUnk_0874BFEC,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874C158-0x0874C184: 1 record(s), section .actor_rec_0874c158 ---- */
struct ActorDef gWhispyWoodsDef ACTOR_REC(0874c158) = {
    .health1Player = 60,
    .health2Players = 81,
    .health3Players = 96,
    .health4Players = 108,
    .score = 10000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = &gUnk_0874C1FC,
    .attackBox = (u32)gUnk_0874C184,
    .terrainBox = (s32)gUnk_0874C1F4,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_0874C204,
    .unk24 = NULL,
    .teardown = NULL,
};

/* ---- 0x0874C1FC-0x0874C204: 1 record(s), section .actor_rec_0874c1fc ---- */
struct ActorAux gUnk_0874C1FC ACTOR_REC(0874c1fc) = {
    .hitDuration = 32,
    .filler01 = { 0x0, 0x0, 0x0 },
    .altAttackBox = (u32)gUnk_0874C1BC,
};

/* ---- 0x0874C378-0x0874C3D0: 2 record(s), section .actor_rec_0874c378 ---- */
struct ActorDef gUnk_0874C378 ACTOR_REC(0874c378) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 1000,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874C3D0,
    .terrainBox = (s32)gUnk_0874C408,
    .terrainHandlers = (u32)gUnk_0874C418,
    .hitReactions = (u32)gUnk_0874C434,
    .unk24 = NULL,
    .teardown = NULL,
};
struct ActorDef gUnk_0874C3A4 ACTOR_REC(0874c378) = {
    .health1Player = 2,
    .health2Players = 2,
    .health3Players = 2,
    .health4Players = 2,
    .score = 0,
    .ability = 0,
    .unk0D = 0x0,
    .unk0E = 0x0,
    .unk10 = NULL,
    .attackBox = (u32)gUnk_0874C3EC,
    .terrainBox = (s32)gUnk_0874C410,
    .terrainHandlers = (u32)gUnk_0873F8F4,
    .hitReactions = (u32)gUnk_0874C440,
    .unk24 = NULL,
    .teardown = NULL,
};
