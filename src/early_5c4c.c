#include "gba/gba.h"
#include "global.h"

/*
 * Cooperative task system, user side (issue #32, batch F2:
 * 0x08005654-0x08005D9C).  Recipe: old_agbcc -O2 (`--old2`), established with
 * the leaf `push {lr}` fingerprint of lessons-learned 3.18 (sub_08005954 and
 * sub_08005acc are leaves that end in a bare `bx lr`).
 *
 * The zone manages a 64-entry task table:
 *   gUnk_03002494  s32   allocation cursor (rotates 0..63), volatile
 *   gUnk_03004CA0  s16[64] per-slot "type" (-1 = free)
 *   gUnk_03002790  struct Task[64] (0x90 bytes each) task control blocks
 *   gUnk_03002490  struct Task *  currently running task
 *   gUnk_030026F0  s32   live task count
 *   gTaskFlagsTable (0x030025F0) u32[64] per-slot flag word from the ROM table
 *   gUnk_03004B90  u32[64] per-slot 256-byte scratch pointer into 0x0203BFE0
 *   gUnk_03002700  u8[]  per-class round-robin write index
 *   gUnk_030024A0  u8[]  per-class slot list (class*64 + n)
 *   gUnk_03002710  u16[64] packed (class << 8) | n back-reference
 *   gUnk_0872FF30  ROM table, 8 bytes per task type: u8 class, u32 flags
 */

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
    /*0x18*/ u32 unk18;
    /*0x1C*/ u32 unk1C;
    /*0x20*/ u32 unk20;
    /*0x24*/ u32 unk24;
    /*0x28*/ u32 unk28;
    /*0x2C*/ u32 unk2C;
    /*0x30*/ u32 unk30;
    /*0x34*/ u32 unk34;
    /*0x38*/ u32 *unk38;
    /*0x3C*/ s16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u8 unk42;
    /*0x43*/ u8 unk43;
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
    /*0x88*/ u32 unk88;
    /*0x8C*/ u32 unk8C;
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

extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern struct TaskType gUnk_0872FF30[];
extern vs32 gUnk_03002494;
extern vs16 gUnk_03004CA0[];
extern s32 gUnk_030026F0;
extern s32 gUnk_030026F4;
extern u32 gUnk_03004B90[];
extern u8 gUnk_0203BFE0[];
extern u32 gTaskFlagsTable[];
extern s32 gCurTaskIdx;
extern vu8 gUnk_030024A0[];
extern vu8 gUnk_03002700[];
extern vu16 gUnk_03002710[];
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000FA8;
extern u16 gUnk_03002348;
extern u16 gUnk_030023E4;
extern s16 gUnk_03002158[];
extern u8 gUnk_03001470[];

extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
/* NOTE: src/early_1518.c declares the last parameter `u16 f`; the two call
 * sites in this file pass a sign-extended s16, so the real prototype must be
 * signed (see the report). */
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);

void sub_08005654(s32 id);
s32 sub_0800579c(u32 type);
void sub_080059a0(void);
void sub_08005954(void);

/* Free the task in slot `id`. */

/* Allocate a task of the given type; returns its slot index or -1. */

/* Allocate, optionally forcing the cursor to `idx` first. */

/* Allocate, scanning [start, end] for a free slot first. */

/* Clamp the running task's velocity to its per-axis maximum. */

/* Integrate acceleration into velocity and velocity into position. */

/* Task body: integrate, then publish the 16.16 position as screen coords. */

/* Task body: integrate if moving, then publish position relative to the
 * parent task's position (Task.unk44 indexes the task array). */

/* Hidden (unreferenced) export inside sub_080059fc's symbols.csv size. */

/* Task body: integrate, publish position relative to the camera. */

/* Is the running task on screen (with a 63/64-pixel margin) relative to
 * gUnk_03002348/gUnk_030023E4? */

/* Hidden (unreferenced) export inside sub_08005acc's symbols.csv size:
 * upload the running task's tile stream plus its palette. */

/* Upload the running task's tile stream. */

/* Is the running task inside the rectangle at gUnk_03002158 (+/- 64)? */
u32 sub_08005c4c(void)
{
    if (gUnk_03002158[0] - 64 >= gUnk_03002490->unk48)
        return 0;
    if (gUnk_03002490->unk48 >= gUnk_03002158[1] + 64)
        return 0;
    if (gUnk_03002158[2] - 64 >= gUnk_03002490->unk4A)
        return 0;
    if (gUnk_03002490->unk4A >= gUnk_03002158[3] + 64)
        return 0;
    return 1;
}

/* Task body: enqueue the running task's sprite if it is on screen. */
void sub_08005ca0(void)
{
    struct Task *t;
    s16 y;

    t = gUnk_03002490;
    if (t->unk38 == 0)
        return;
    if (t->unk3C == -1)
        return;
    if ((u16)(t->unk48 + 63) > 366)
        return;
    y = t->unk4A;
    if (y <= -64)
        return;
    if (y > 223)
        return;
    sub_08001a94(t->unk42, t->unk38[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
}

/* Same, but free the task when it leaves the screen. */
void sub_08005d18(void)
{
    struct Task *t;
    s16 y;

    t = gUnk_03002490;
    if (t->unk38 == 0)
        return;
    if (t->unk3C == -1)
        return;
    if ((u16)(t->unk48 + 63) > 366)
        goto kill;
    y = t->unk4A;
    if (y <= -64)
        goto kill;
    if (y > 223)
        goto kill;
    sub_08001a94(t->unk42, t->unk38[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
    return;
kill:
    sub_08005654(gCurTaskIdx);
}
