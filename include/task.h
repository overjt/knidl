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
 */

struct Actor;
struct ActorDef;
struct AnimCmd;

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
    /*0x1C*/ u32 unk1C;
    /*0x20*/ u32 unk20;
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
    /*0x6C*/ u16 unk6C;
    /*0x6E*/ u16 unk6E;
    /*0x70*/ u16 unk70;
    /*0x72*/ u8 unk72;
    /*0x73*/ u8 unk73;
    /*0x74*/ u8 unk74;
    /*0x75*/ u8 unk75;
    /*0x76*/ u16 unk76;
    /*0x78*/ u16 unk78;
    /*0x7A*/ u8 unk7A;
    /*0x7B*/ u8 unk7B;
    /*0x7C*/ u8 unk7C;
    /*0x7D*/ u8 unk7D;
    /*0x7E*/ s8 unk7E;
    /*0x7F*/ s8 unk7F;
    /*0x80*/ s8 unk80;
    /*0x81*/ u8 unk81;
    /*0x82*/ u16 unk82;
    /*0x84*/ u16 unk84;
    /*0x86*/ u16 unk86;
    /*0x88*/ void *unk88;
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
    /*0x00*/ u32 unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
    /*0x10*/ u32 unk10;
    /*0x14*/ u32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ u32 unk1C;
    /*0x20*/ u32 unk20;
    /*0x24*/ void (*unk24)(u32);
    /*0x28*/ void (*unk28)(void);
};

/* The 12-byte block at Actor+0x64, copied as one unit (0x0806505C). */
struct ActorTail
{
    /*0x00*/ u32 unk00;
    /*0x04*/ u32 unk04;
    /*0x08*/ u32 unk08;
};

/* Per-task actor record (Task.unk8C). */
struct Actor
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
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
    /*0x12*/ u16 unk12;
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
    /*0x60*/ u32 unk60;
    /*0x64*/ struct ActorTail unk64;
};

/* One entry of the actor animation script Actor.unk2C walks (0x080640FC).
   unk00 is the frame id, or -3 (loop) / -2 (stop); unk02 is the delay. */
struct AnimCmd
{
    /*0x00*/ s16 unk00;
    /*0x02*/ s16 unk02;
};

/* 116-byte per-player record at gUnk_03002170 (0x08064EB8). */
struct PlayerState
{
    /*0x00*/ u8 filler00[13];
    /*0x0D*/ s8 unk0D;
    /*0x0E*/ s8 unk0E;
    /*0x0F*/ u8 filler0F[116 - 0x0F];
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

/* Axis-aligned box the actor overlap helpers take (0x08063E2C). */
struct Rect
{
    /*0x00*/ s16 left;
    /*0x02*/ s16 top;
    /*0x04*/ s16 right;
    /*0x06*/ s16 bottom;
};

/* Two 16.16-packed points, laid out as four 16-bit fields (0x08063BD4). */
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
