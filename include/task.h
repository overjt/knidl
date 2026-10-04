#ifndef GUARD_TASK_H
#define GUARD_TASK_H

#include "gba/types.h"

/*
 * Cooperative task system data model.
 *
 * `struct Task` is the 0x90-byte task control block first mapped in issue #32
 * (`src/early_58e4.c`, `src/early_5d9c.c`); the 64-entry table lives at
 * gTasks and gCurTask points at the task that is currently
 * running.  `struct Actor` is the larger per-task actor record hanging off
 * Task.u8C.actor that module M17 (issue #65) is the field API for, and
 * `struct ActorDef` is the ROM descriptor an actor is bound to
 * (Actor.def).
 *
 * Field signedness is evidence-based: `ldrsh`/`ldrsb`, or `ldrh`/`ldrb`
 * followed by a `lsls #16; asrs #16` (or `#24`) pair, means the field is
 * signed.  Where M17 and M18 disagree about one field, the header keeps the
 * unsigned type and the signed call sites cast (see Task.unk6C/unk70).
 */

struct Actor;
struct ActorDef;
struct PlayerState;
struct AnimCmd;
struct GfxHeader;
struct ActorAux;

struct Task
{
    /*0x00*/ u32 moveCallback;
    /*0x04*/ u32 updateCallback;
    /*0x08*/ u32 lateUpdateCallback;
    /*0x0C*/ u32 drawCallback;
    /*0x10*/ u16 sleepFrames;
    /*0x12*/ s8 taskClass;
    /*0x13*/ u8 skipMask;
    /*0x14*/ u8 state;
    /*0x15*/ u8 updateState;
    /*0x16*/ u16 serial;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 unk34;
    /*0x38*/ u32 *frameTable;
    /*0x3C*/ s16 frame;
    /*0x3E*/ u16 spriteFlags;
    /*0x40*/ u16 tileWord;
    /*0x42*/ u8 layer;
    /*0x43*/ s8 facing;
    /*0x44*/ s16 parent;
    /*0x46*/ s16 unk46;
    /*0x48*/ s16 pixelX;
    /*0x4A*/ s16 pixelY;
    /*0x4C*/ s32 posX;
    /*0x50*/ s32 posY;
    /*0x54*/ s32 velX;
    /*0x58*/ s32 velY;
    /*0x5C*/ s32 accelX;
    /*0x60*/ s32 accelY;
    /*0x64*/ s32 speedLimitX;
    /*0x68*/ s32 speedLimitY;
    /* 0x6C and 0x70 are read with `ldrsh`/`(s16)` casts throughout M18
       (issue #64) but unsigned in M17's src/actor_673ec.c, so they stay u16
       and the signed sites cast. 0x6E is signed everywhere. */
    /*0x6C*/ u16 unk6C;
    /*0x6E*/ s16 unk6E;
    /*0x70*/ u16 unk70;
    /*0x72*/ u8 actorKind;
    /*0x73*/ u8 variant;
    /*0x74*/ u8 unk74;
    /*0x75*/ s8 hitTimer;
    /*0x76*/ u16 unk76;
    /*0x78*/ s16 health;
    /*0x7A*/ s8 onGround;
    /*0x7B*/ s8 waterFlags;
    /*0x7C*/ s8 hitKind;
    /*0x7D*/ u8 hitDirection;
    /*0x7E*/ s8 hitterSlot;
    /*0x7F*/ s8 hitterPlayer;
    /* An actor's nearest player (TaskFindNearestPlayerSlot, TaskFindNearestPlayer); for
       the player and its objects and effects (#5-#7) the ability of the
       running attack, which ActorPlayHitSfx reads off the hitter. */
    /* packed: agbcc pads every union to 4 bytes (lesson 3.522). */
    /*0x80*/ union {
        s8 nearestPlayer;
        s8 attackAbility;
    } __attribute__((packed)) u80;
    /*0x81*/ u8 unk81;
    /*0x82*/ u16 hitEffect;
    /*0x84*/ u16 unk84;
    /*0x86*/ u16 unk86;
    /* The record of the player the task belongs to: Task_Player binds
       &gPlayerStates[slot] and the player's objects and effects copy it.
       For actors, ActorInitSlot's TaskFindNearestPlayerSlot and TaskFindNearestPlayer
       store the nearest player's struct Task * here instead, which no actor
       reads back; ActorAttachToHitter rebinds it to the hitter's record. */
    /*0x88*/ struct PlayerState *player;
    /* The task's actor record &gActors[slot] (CreateActor, sub_08064a78,
       sub_08064d9c, SetPaletteAnimSource); task types #6/#7 keep their parent
       task &gTasks[Task.parent] here instead (Task_PlayerObject,
       Task_PlayerEffect, sub_08056770). */
    /*0x8C*/ union {
        struct Actor *actor;
        struct Task *parentTask;
    } u8C;
};

/* 8 bytes per task type in ROM at 0x0872FF30. */
struct TaskType
{
    /*0x00*/ u8 taskClass;
    /*0x01*/ u8 pad01[3];
    /*0x04*/ u32 entry;
};

/* Per-task graphics descriptor reached through Task.frameTable[Task.frame]. */
struct TaskGfx
{
    /*0x00*/ u32 oamTemplate;
    /*0x04*/ u16 *palette;
    /*0x08*/ u16 *tiles;
};

/* ROM descriptor an actor is bound to (Actor.def). */
struct ActorDef
{
    /*0x00*/ u16 health1Player;
    /*0x02*/ u16 health2Players;
    /*0x04*/ u16 health3Players;
    /*0x06*/ u16 health4Players;
    /*0x08*/ u32 score;
    /*0x0C*/ u8 ability;
    /*0x0D*/ u8 isItem;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ struct ActorAux *unk10;
    /*0x14*/ u32 attackBox;
    /*0x18*/ s32 terrainBox;
    /*0x1C*/ u32 terrainHandlers;
    /*0x20*/ u32 hitReactions;
    /*0x24*/ void (*initCallback)(u32);
    /*0x28*/ void (*teardown)(void);
};

/* Block ActorDef.unk10 / Actor.unk60 point at (0x08068E04). */
struct ActorAux
{
    /*0x00*/ s8 hitDuration;
    /*0x01*/ u8 filler01[3];
    /*0x04*/ u32 altAttackBox;
};

/* Graphics header the actor's tail block points at (0x08066088). */
struct GfxHeader
{
    /*0x00*/ u16 paletteBankCount;
    /*0x02*/ u16 tileCount;
    /*0x04*/ u32 unk04;
    /*0x08*/ void *palette;
    /*0x0C*/ void *tiles;
};

/* The 12-byte block at Actor+0x64, copied as one unit (0x0806505C). */
struct ActorTail
{
    /*0x00*/ struct GfxHeader *header;
    /*0x04*/ u32 tileBits;
    /*0x08*/ u32 paletteBank;
};

/* Per-task actor record (Task.u8C.actor). */
struct Actor
{
    /*0x00*/ u8 ability;
    /*0x01*/ u8 hitStunTimer;
    /*0x02*/ s8 healthBonus;
    /*0x03*/ s8 extraLayerOffset;
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 hitState;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 animNoFlip;
    /*0x09*/ u8 animScriptPos;
    /*0x0A*/ u8 paletteOverridden;
    /*0x0B*/ u8 paletteLocked;
    /*0x0C*/ u8 paletteVariant;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ s16 unk0E;
    /*0x10*/ s16 attachedTask;
    /*0x12*/ s16 attachedTaskLifetime;
    /*0x14*/ u16 savedFrame;
    /*0x16*/ s16 unk16;
    /*0x18*/ u16 extraOffsetY;
    /*0x1A*/ s16 extraFrame;
    /*0x1C*/ u16 prevState;
    /*0x1E*/ u16 extraTileWord;
    /*0x20*/ u16 savedTileWord;
    /*0x22*/ u16 savedPaletteBits;
    /*0x24*/ u16 paletteColorCount;
    /*0x26*/ u16 unk26;
    /*0x28*/ u32 palette;
    /*0x2C*/ struct AnimCmd *animScript;
    /*0x30*/ u32 score;
    /*0x34*/ s32 unk34;
    /*0x38*/ s32 sfxOverride;
    /*0x3C*/ u32 defeatSweepCallback;
    /*0x40*/ void (*teardown)(void);
    /*0x44*/ struct ActorDef *def;
    /*0x48*/ u32 attackBox;
    /*0x4C*/ u32 unk4C;
    /*0x50*/ u32 terrainBox;
    /*0x54*/ u32 terrainHandlers;
    /*0x58*/ u32 prevTerrainHandlers;
    /*0x5C*/ u32 hitReactions;
    /*0x60*/ struct ActorAux *unk60;
    /*0x64*/ struct ActorTail gfx;
};

/* One entry of the actor animation script Actor.animScript walks (0x080640FC).
   frame is the frame id, or -3 (loop) / -2 (stop); delay is the number of
   frames until the next entry. */
struct AnimCmd
{
    /*0x00*/ s16 frame;
    /*0x02*/ s16 delay;
};

/* 116-byte per-player record at gPlayerStates (0x08064EB8); Task.player points
   at the record of the player the task belongs to. */
struct PlayerState
{
    /*0x00*/ s8 playerIndex;
    /*0x01*/ u8 requestedAction;
    /*0x02*/ u8 action;
    /*0x03*/ u8 prevAction;
    /*0x04*/ u8 mode;
    /*0x05*/ u8 prevMode;
    /*0x06*/ u8 mouthState;
    /*0x07*/ u8 attachedCount;
    /*0x08*/ u8 heldCount;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 pendingAbility;
    /*0x0C*/ u8 pendingAbilityUses;
    /*0x0D*/ s8 ability;
    /*0x0E*/ s8 abilityUses;
    /*0x0F*/ u8 unk0F;
    /*0x10*/ s8 unk10;
    /*0x11*/ u8 filler11;
    /*0x12*/ u16 invulnerabilityTimer;
    /*0x14*/ u16 unk14;
    /*0x16*/ u8 unk16;
    /*0x17*/ u8 invincible;
    /*0x18*/ u16 invincibleTimer;
    /*0x1A*/ u16 unk1A;
    /*0x1C*/ u16 unk1C;
    /*0x1E*/ u16 unk1E;
    /*0x20*/ u16 unk20;
    /*0x22*/ u8 unk22;
    /*0x23*/ u8 filler23;
    /*0x24*/ u16 pixelOffsetX;
    /*0x26*/ u16 pixelOffsetY;
    /*0x28*/ u8 offsetScriptStep;
    /*0x29*/ u8 offsetScriptDelay;
    /*0x2A*/ u8 offsetScript;
    /*0x2B*/ u8 unk2B;
    /*0x2C*/ s16 sfxPlayer;
    /*0x2E*/ s16 sfxId;
    /*0x30*/ u8 unk30;
    /*0x31*/ u8 unk31;
    /*0x32*/ s8 unk32;
    /*0x33*/ s8 unk33;
    /*0x34*/ s8 unk34;
    /*0x35*/ s8 unk35;
    /*0x36*/ u8 unk36;
    /*0x37*/ u8 unk37;
    /*0x38*/ u16 shareTimer;
    /*0x3A*/ u8 shareItem;
    /*0x3B*/ u8 sharedMask;
    /*0x3C*/ u8 unk3C;
    /* M11's MetaKnightActionDashSlashUpdate writes 0/1 here (issue #85). */
    /*0x3D*/ u8 running;
    /*0x3E*/ u8 bumpKind;
    /*0x3F*/ u8 invulnerability;
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ u8 blocksBroken;
    /*0x45*/ u8 hitsThisFrame;
    /*0x46*/ u8 unk46;
    /*0x47*/ u8 unk47;
    /*0x48*/ u8 boundsClamp;
    /*0x49*/ u8 onSlipperyFloor;
    /* gTerrainResult's byte 0 after the probes: 1 right wall, 2 left wall,
       3 both (stuck inside a block); sub_0801c8dc adds 1 when the top of
       the box is inside a solid tile on the ground. */
    /*0x4A*/ u8 wallSide;
    /*0x4B*/ u8 slope;
    /*0x4C*/ u8 atDoor;
    /*0x4D*/ u8 filler4D;
    /*0x4E*/ s16 unk4E;
    /*0x50*/ u8 unk50;
    /*0x51*/ u8 filler51[3];
    /*0x54*/ s32 driftVelX;
    /*0x58*/ s32 driftVelY;
    /* M09's player task copies Task.waterFlags here and tests bit 0 (issue #92). */
    /*0x5C*/ u8 prevWaterFlags;
    /*0x5D*/ u8 filler5D;
    /*0x5E*/ u16 prevPixelX;
    /*0x60*/ u16 prevPixelY;
    /*0x62*/ u8 filler62[2];
    /* M16's PlayerDance zeroes bodyBox/terrainBox/hitBoxSet per player
       when a run starts (issue #83). */
    /*0x64*/ u32 bodyBox;
    /*0x68*/ u32 terrainBox;
    /* M11 keeps &gPlayerHitBoxSets[playerIndex] here and clears it to 0
       (issue #85). */
    /*0x6C*/ void *hitBoxSet;
    /*0x70*/ u32 *prevTerrainBox;
};

/* Spawn descriptor sub_08064A78 turns into a task of actor kind 4
   (Task.actorKind). */
struct ActorSpawn
{
    /*0x00*/ u32 subtype;
    /*0x04*/ u32 taskType;
    /*0x08*/ u8 variant;
    /*0x09*/ u8 spawnArg;
    /*0x0A*/ u8 checkTerrain;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ s16 x;
    /*0x0E*/ s16 y;
    /*0x10*/ u16 tileWord;
};

/* Axis-aligned box the actor overlap helpers take (0x08063E2C).
 *
 * INCOMPLETE MODEL - read this before declaring a caller of TaskIsInRect or
 * TaskIsNearestPlayerInRect.  Four separate `s16` fields are what the CALLEE reads (that is
 * how src/actor_63698.c matches), but a caller that fills the box in the
 * caller's own stack frame does NOT necessarily see this type: M22 (issue #69)
 * has three of them (sub_08083020, sub_08083488, sub_08083fbc) where the ROM
 * builds the argument with 32-bit read-modify-write over PAIRS of halfwords
 * (`ldr; ands 0xFFFF0000; orrs; str`), which four `s16` fields can only ever
 * compile to `strh`.  Those callers declare the helper as taking a
 * `struct PointPair *` instead, and that is what byte-matches.
 *
 * So the original almost certainly had one packed "two corners" type (or a
 * union of the two views) and this header currently models only the callee's
 * half.  If you hit the same fork, try `struct PointPair *` before rewriting
 * the assignments - and if you work out the real type, fix it here rather than
 * adding a fourth per-file spelling.
 */
struct Rect
{
    /*0x00*/ s16 left;
    /*0x02*/ s16 top;
    /*0x04*/ s16 right;
    /*0x06*/ s16 bottom;
};

/* Two 16.16-packed points, laid out as four 16-bit fields (0x08063BD4).
 * Also the shape every M22 caller of the TaskIsInRect / TaskIsNearestPlayerInRect overlap
 * helpers passes them - see the note on struct Rect above. */
struct PointPair
{
    u32 x0:16;
    u32 y0:16;
    u32 x1:16;
    u32 y1:16;
};



/* EWRAM */
extern u8  gTaskSkipMaskStack[2][64];
extern u8  gTaskSkipMaskDepth;
extern u8 gUnk_0203BFE0[];

/* IWRAM */
extern vu32 gTaskSavedSp;
extern vu8  gTaskClassListPos[];
extern vu8  gTaskClassPassEnd[];
extern vu32 gCurTaskListPos;
extern vs32 gCurTaskIdx;
extern struct Task *gCurTask;
extern vs32 gTaskCursor;
extern vu8  gTaskClassLists[5][64];
extern s32  gTaskSavedR0;
extern u32  gTaskResumeAddrs[];
extern vu32 gTaskCount;
extern vs32 gTaskRunPhase;
extern vu32 gTaskSavedLr;
extern vu8  gTaskClassListLen[];
extern vu8  gTaskClassPassStart[];
extern vu16 gTaskListRefs[];
extern struct Task gTasks[];
extern u32 gTaskStackPtrs[];
extern vs32 gCurTaskClass;
extern vu32 gTaskBaseSp;
extern vs16 gTaskSlotTypes[];

/* ROM */
extern const struct TaskType gTaskTypes[];

/* Functions (defined in the files named above each group). */

/* src/early_4fec.c */
void InitTasks(void);

/* src/early_5228.c */
void RunTasks(void);

/* src/early_55b0.c */
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSetOthersSkipMask(u16 val, s32 idx);
void TaskSetAllSkipMask(u16 val);

/* src/early_5654.c */
void TaskFree(s32 id);
s32 TaskCreate(u32 type);

/* src/early_58e4.c */
s32 TaskCreateFrom(u32 type, s32 idx);
s32 TaskCreateInRange(u32 type, s32 start, s32 end);
void TaskClampVelocity(void);
void TaskIntegrateMotion(void);
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskUpdatePixelPos(void);
void TaskMoveRelativeToBg3(void);

/* src/early_5acc.c */
u32 TaskLoadFrameTilesAndPalette(u32 alt);
u32 TaskLoadFrameTiles(u32 alt);

/* src/early_5c4c.c */
void TaskDrawScreen(void);
void TaskDrawScreenOrFree(void);

/* asm/sdk_libc.s: the task trampolines into the ARM task switcher (rom-map section 6) */
void TaskExitTrampoline(void);
void TaskSwitchTrampoline(s32 id, u32 fn, u32 stack);
void TaskYieldTrampoline(u32 frames);

#endif // GUARD_TASK_H
