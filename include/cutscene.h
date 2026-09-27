#ifndef GUARD_CUTSCENE_H
#define GUARD_CUTSCENE_H

#include "gba/types.h"
#include "task.h"

/* cutscene.h: the RAM cells and ROM tables of the scripted sequences: AgbMain
   state 7, the sequence director (M04) and the cutscene bank (M19).  One
   declaration per symbol, with the type its consumers prove (issue #36 phase
   2, docs/header-conventions.md). */

struct GfxHeader;

/* The 24-entry animation rows at 0x08740320 / 0x087404A0 the credits
   particles walk: `unk00` indexes the gfx pointer table, `unk02` is the
   step's delay. */
struct M19Frame
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The eight 4-byte records at 0x03000FE0 that M19's credits tasks animate:
   a frame table index (unk00), the frame within it (unk01), the timer
   (unk02) and the countdown sub_080782b4 draws on (unk03). */
struct M19Particle
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The 0x087401E4 script records M19's ending-sequence tasks walk: a pointer
   table indexed by Task.unk20, each entry a header plus two `s16` step lists
   (the "forward" list at +6 and the "reverse" one at +18) that
   sub_0807777c picks between on Task.unk18/unk24. */
struct M19Script
{
    /*0x00*/ u8 unk00[4];
    /*0x04*/ u16 unk04;
    /*0x06*/ u16 unk06[6];
    /*0x12*/ u16 unk12[1];
};

/* EWRAM */
extern u32 gUnk_02004B4C;
extern u32 gUnk_02005584;

/* IWRAM */
extern struct M19Particle gUnk_03000FE0[];

/* ROM */
extern u32 gUnk_080D21C8[];
extern u32 gUnk_0824A9CC[];
extern u32 gUnk_0825D2C8[];
extern u32 gUnk_085E6FA4[];
extern u32 gUnk_085E6FE4[];
extern u32 gUnk_085E72D4[];
extern struct GfxHeader *const gUnk_08731F78[];
extern u32 gCutsceneDurations[];
extern u32 gUnk_08731FA8[];
extern u32 gUnk_08731FC8[];
extern s16 gUnk_087320C4[][16];
extern u32 gUnk_08732104[];
extern u32 gUnk_08732134[];
extern u32 gUnk_08732138[];
extern u32 gUnk_0873F554[];
extern u32 gUnk_0873F5CC[];
extern u32 gUnk_0873F5E4[];
extern u32 gUnk_0873FB7C[];
extern s32 gUnk_0873FB94[];
extern u32 gWarpStarStates[];
extern u32 gWarpStarStateUpdates[];
extern u32 gUnk_0873FBC4[];
extern u32 gUnk_0873FC2C[];
extern u32 gUnk_0873FC94[];
extern s16 gUnk_0873FCF8[];
extern s16 gUnk_0873FD20[];
extern s16 gUnk_0873FD48[];
extern s16 gUnk_0873FD70[];
extern u32 gUnk_0873FD98[];
extern u32 gUnk_0873FE98[];
extern u32 gUnk_08740098[];
extern u16 gUnk_0874009C[];
extern u16 gUnk_087400A6[];
extern u32 gUnk_087400B0[];
extern u32 gUnk_087400C8[];
extern u32 gCannonVariants[];
extern u16 gUnk_087400E4[];
extern u32 gCannonStates[];
extern u32 gCannonStateUpdates[];
extern u32 gCannonFuseVariants[];
extern u16 gUnk_08740124[];
extern s8 gUnk_087401CC[];
extern struct M19Script *gUnk_087401E4[];
extern u32 gCannonFuseStates[];
extern u32 gCannonFuseStateUpdates[];
extern u32 gBigSwitchVariants[];
extern u32 gBigSwitchStates[];
extern u32 gBigSwitchStateUpdates[];
extern u32 gStakeVariants[];
extern u32 gStakeStates[];
extern u32 gStakeStateUpdates[];
extern u32 gUnk_087402FC[];
extern struct M19Frame gUnk_08740320[][24];
extern struct M19Frame gUnk_087404A0[][24];
extern u8 gUnk_08740620[];
extern u32 gWaddleDeeVariants[];
extern struct AnimCmd gUnk_087406A0[];
extern u32 gUnk_08740BD4[];
extern u32 gWaddleDeeFrames[];
extern u32 gCannonFrames[];
extern u32 gCannonFuseFrames[];
extern u32 gBigSwitchFrames[];
extern u32 gStakeFrames[];
extern u32 gUnk_08752D8C[];
extern u32 gUnk_08752DB8[];
extern u32 gUnk_08752E00[];
extern u32 gUnk_08754A14[];
extern u32 gUnk_08754ABC[];
extern u32 gUnk_08754B80[];
extern u32 gUnk_08754C90[];
extern u32 gUnk_08754D60[];
extern u32 gUnk_08754E7C[];
extern u32 gUnk_08754F68[];
extern u32 gUnk_0875549C[];

#endif /* GUARD_CUTSCENE_H */
