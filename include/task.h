#ifndef GUARD_TASK_H
#define GUARD_TASK_H

#include "gba/types.h"

/*
 * Cooperative task system data model.
 *
 * `struct Task` is the 0x90-byte task control block first mapped in issue #32
 * (`src/early_58e4.c`, `src/early_5d9c.c`); the 64-entry table lives at
 * gUnk_03002790 and gUnk_03002490 points at the task that is currently
 * running.  `struct Actor` is the larger per-task actor record hanging off
 * Task.unk8C that module M17 (issue #65) is the field API for, and
 * `struct ActorDef` is the ROM descriptor an actor is bound to
 * (Actor.unk44).
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
    /*0x00*/ u32 unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ u32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ s8 unk12;
    /*0x13*/ u8 unk13;
    /*0x14*/ u8 unk14;
    /*0x15*/ u8 unk15;
    /*0x16*/ u16 unk16;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 unk34;
    /*0x38*/ u32 *unk38;
    /*0x3C*/ s16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u8 unk42;
    /*0x43*/ s8 unk43;
    /*0x44*/ s16 unk44;
    /*0x46*/ s16 unk46;
    /*0x48*/ s16 unk48;
    /*0x4A*/ s16 unk4A;
    /*0x4C*/ s32 unk4C;
    /*0x50*/ s32 unk50;
    /*0x54*/ s32 unk54;
    /*0x58*/ s32 unk58;
    /*0x5C*/ s32 unk5C;
    /*0x60*/ s32 unk60;
    /*0x64*/ s32 unk64;
    /*0x68*/ s32 unk68;
    /* 0x6C and 0x70 are read with `ldrsh`/`(s16)` casts throughout M18
       (issue #64) but unsigned in M17's src/actor_673ec.c, so they stay u16
       and the signed sites cast. 0x6E is signed everywhere. */
    /*0x6C*/ u16 unk6C;
    /*0x6E*/ s16 unk6E;
    /*0x70*/ u16 unk70;
    /*0x72*/ u8 unk72;
    /*0x73*/ u8 unk73;
    /*0x74*/ u8 unk74;
    /*0x75*/ s8 unk75;
    /*0x76*/ u16 unk76;
    /*0x78*/ s16 unk78;
    /*0x7A*/ s8 unk7A;
    /*0x7B*/ s8 unk7B;
    /*0x7C*/ s8 unk7C;
    /*0x7D*/ u8 unk7D;
    /*0x7E*/ s8 unk7E;
    /*0x7F*/ s8 unk7F;
    /*0x80*/ s8 unk80;
    /*0x81*/ u8 unk81;
    /*0x82*/ u16 unk82;
    /*0x84*/ u16 unk84;
    /*0x86*/ u16 unk86;
    /* Context pointer: the player record for actor tasks (0x08064EB8), but
       task-class specific - sub_08063A9C stores another task here. */
    /*0x88*/ struct PlayerState *unk88;
    /*0x8C*/ struct Actor *unk8C;
};

/* 8 bytes per task type in ROM at 0x0872FF30. */
struct TaskType
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 pad01[3];
    /*0x04*/ u32 unk04;
};

/* Per-task graphics descriptor reached through Task.unk38[Task.unk3C]. */
struct TaskGfx
{
    /*0x00*/ u32 unk00;
    /*0x04*/ u16 *unk04;
    /*0x08*/ u16 *unk08;
};

/* ROM descriptor an actor is bound to (Actor.unk44). */
struct ActorDef
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ u16 unk06;
    /*0x08*/ u32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ struct ActorAux *unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ u32 unk1C;
    /*0x20*/ u32 unk20;
    /*0x24*/ void (*unk24)(u32);
    /*0x28*/ void (*unk28)(void);
};

/* Block ActorDef.unk10 / Actor.unk60 point at (0x08068E04). */
struct ActorAux
{
    /*0x00*/ s8 unk00;
    /*0x01*/ u8 filler01[3];
    /*0x04*/ u32 unk04;
};

/* Graphics header the actor's tail block points at (0x08066088). */
struct GfxHeader
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02;
    /*0x04*/ u32 unk04;
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
};

/* The 12-byte block at Actor+0x64, copied as one unit (0x0806505C). */
struct ActorTail
{
    /*0x00*/ struct GfxHeader *unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
};

/* Per-task actor record (Task.unk8C). */
struct Actor
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ s8 unk02;
    /*0x03*/ s8 unk03;
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 unk07;
    /*0x08*/ u8 unk08;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ s16 unk0E;
    /*0x10*/ s16 unk10;
    /*0x12*/ s16 unk12;
    /*0x14*/ u16 unk14;
    /*0x16*/ s16 unk16;
    /*0x18*/ u16 unk18;
    /*0x1A*/ s16 unk1A;
    /*0x1C*/ u16 unk1C;
    /*0x1E*/ u16 unk1E;
    /*0x20*/ u16 unk20;
    /*0x22*/ u16 unk22;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u32 unk28;
    /*0x2C*/ struct AnimCmd *unk2C;
    /*0x30*/ u32 unk30;
    /*0x34*/ s32 unk34;
    /*0x38*/ s32 unk38;
    /*0x3C*/ u32 unk3C;
    /*0x40*/ void (*unk40)(void);
    /*0x44*/ struct ActorDef *unk44;
    /*0x48*/ u32 unk48;
    /*0x4C*/ u32 unk4C;
    /*0x50*/ u32 unk50;
    /*0x54*/ u32 unk54;
    /*0x58*/ u32 unk58;
    /*0x5C*/ u32 unk5C;
    /*0x60*/ struct ActorAux *unk60;
    /*0x64*/ struct ActorTail unk64;
};

/* One entry of the actor animation script Actor.unk2C walks (0x080640FC).
   unk00 is the frame id, or -3 (loop) / -2 (stop); unk02 is the delay. */
struct AnimCmd
{
    /*0x00*/ s16 unk00;
    /*0x02*/ s16 unk02;
};

/* 116-byte per-player record at gUnk_03002170 (0x08064EB8); Task.unk88 points
   at the record of the player the task belongs to (0x08064EB8). */
struct PlayerState
{
    /*0x00*/ s8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 unk06;
    /*0x07*/ u8 filler07[6];
    /*0x0D*/ s8 unk0D;
    /*0x0E*/ s8 unk0E;
    /*0x0F*/ u8 filler0F[5];
    /*0x14*/ u16 unk14;
    /*0x16*/ u8 unk16;
    /*0x17*/ u8 filler17[11];
    /*0x22*/ u8 unk22;
    /*0x23*/ u8 filler23;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u8 unk28;
    /*0x29*/ u8 unk29;
    /*0x2A*/ u8 filler2A;
    /*0x2B*/ u8 unk2B;
    /*0x2C*/ s16 unk2C;
    /*0x2E*/ s16 unk2E;
    /*0x30*/ u8 filler30[7];
    /*0x37*/ u8 unk37;
    /*0x38*/ u8 filler38[8];
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ u8 filler44;
    /*0x45*/ u8 unk45;
    /*0x46*/ u8 filler46[0x5E - 0x46];
    /*0x5E*/ u16 unk5E;
    /*0x60*/ u16 unk60;
    /*0x62*/ u8 filler62[116 - 0x62];
};

/* Spawn descriptor sub_08064A78 turns into a class-4 task. */
struct ActorSpawn
{
    /*0x00*/ u32 unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u8 unk08;
    /*0x09*/ u8 unk09;
    /*0x0A*/ u8 unk0A;
    /*0x0B*/ u8 unk0B;
    /*0x0C*/ s16 unk0C;
    /*0x0E*/ s16 unk0E;
    /*0x10*/ u16 unk10;
};

/* Axis-aligned box the actor overlap helpers take (0x08063E2C).
 *
 * INCOMPLETE MODEL - read this before declaring a caller of sub_08063E2C or
 * sub_08063F00.  Four separate `s16` fields are what the CALLEE reads (that is
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
 * Also the shape every M22 caller of the sub_08063E2C / sub_08063F00 overlap
 * helpers passes them - see the note on struct Rect above. */
struct PointPair
{
    u32 x0:16;
    u32 y0:16;
    u32 x1:16;
    u32 y1:16;
};

extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern vs32 gCurTaskIdx;

#endif // GUARD_TASK_H
