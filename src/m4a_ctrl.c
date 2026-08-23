#include "gba/gba.h"
#include "gba/m4a_internal.h"

/* m4a/mp2k sound engine, C driver part 3 (issue #55):
 * 0x080CF588-0x080CFA4B — track controls (m4aMPlay{Volume,Pitch,Panpot}
 * Control), ClearModM, m4aMPlay{ModDepthSet,LFOSpeedSet} (dead exports,
 * evidence `curated`, emitted anyway) and the memacc/xcmd handlers
 * (ply_memacc 18-op jump-table switch, ply_xcmd + ply_xxx..ply_xswee).
 * Same compilation unit as parts 1-2, so same recipe: old_agbcc -O2
 * -mthumb-interwork (lesson 3.15; the issue's -O1 guess disagreed and
 * the ROM proved -O2: unlike part 1's wrappers, these ident lock stores
 * ARE in the ROM because gcc cannot prove track[] can't alias
 * mplayInfo->ident across the loop body).
 * Verify with
 * `./tools/fnmatch.sh 0x080CF588 0x080CFA4C src/m4a_ctrl.c --old2`.
 * This unit's ClearModM (0x080CF6CC) is a SECOND copy of the asm core's
 * ClearModM_asm (0x080CE468); the C one belongs here. */

void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume)
{
    s32 i;
    u32 bit;
    struct MusicPlayerTrack *track;

    if (mplayInfo->ident != ID_NUMBER)
        return;

    mplayInfo->ident++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0)
    {
        if (trackBits & bit)
        {
            if (track->flags & MPT_FLG_EXIST)
            {
                track->volX = volume / 4;
                track->flags |= MPT_FLG_VOLCHG;
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->ident = ID_NUMBER;
}

void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch)
{
    s32 i;
    u32 bit;
    struct MusicPlayerTrack *track;

    if (mplayInfo->ident != ID_NUMBER)
        return;

    mplayInfo->ident++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0)
    {
        if (trackBits & bit)
        {
            if (track->flags & MPT_FLG_EXIST)
            {
                track->keyShiftX = pitch >> 8;
                track->pitX = pitch;
                track->flags |= MPT_FLG_PITCHG;
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->ident = ID_NUMBER;
}

void m4aMPlayPanpotControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s8 pan)
{
    s32 i;
    u32 bit;
    struct MusicPlayerTrack *track;

    if (mplayInfo->ident != ID_NUMBER)
        return;

    mplayInfo->ident++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0)
    {
        if (trackBits & bit)
        {
            if (track->flags & MPT_FLG_EXIST)
            {
                track->panX = pan;
                track->flags |= MPT_FLG_VOLCHG;
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->ident = ID_NUMBER;
}

void ClearModM(struct MusicPlayerTrack *track)
{
    track->lfoSpeedC = 0;
    track->modM = 0;

    if (track->modT == 0)
        track->flags |= MPT_FLG_PITCHG;
    else
        track->flags |= MPT_FLG_VOLCHG;
}

/* Dead exports (no in-ROM references, SDK object linked wholesale):
 * kept because the linker kept them, like part 1's wrappers (#31/#53). */
void m4aMPlayModDepthSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 modDepth)
{
    s32 i;
    u32 bit;
    struct MusicPlayerTrack *track;

    if (mplayInfo->ident != ID_NUMBER)
        return;

    mplayInfo->ident++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0)
    {
        if (trackBits & bit)
        {
            if (track->flags & MPT_FLG_EXIST)
            {
                track->mod = modDepth;

                if (!track->mod)
                    ClearModM(track);
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->ident = ID_NUMBER;
}

void m4aMPlayLFOSpeedSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 lfoSpeed)
{
    s32 i;
    u32 bit;
    struct MusicPlayerTrack *track;

    if (mplayInfo->ident != ID_NUMBER)
        return;

    mplayInfo->ident++;

    i = mplayInfo->trackCount;
    track = mplayInfo->tracks;
    bit = 1;

    while (i > 0)
    {
        if (trackBits & bit)
        {
            if (track->flags & MPT_FLG_EXIST)
            {
                track->lfoSpeed = lfoSpeed;

                if (!track->lfoSpeed)
                    ClearModM(track);
            }
        }

        i--;
        track++;
        bit <<= 1;
    }

    mplayInfo->ident = ID_NUMBER;
}

#define MEMACC_COND_JUMP(cond) \
if (cond)                      \
    goto cond_true;            \
else                           \
    goto cond_false;           \

void ply_memacc(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u32 op;
    u8 *addr;
    u8 data;

    op = *track->cmdPtr;
    track->cmdPtr++;

    addr = mplayInfo->memAccArea + *track->cmdPtr;
    track->cmdPtr++;

    data = *track->cmdPtr;
    track->cmdPtr++;

    switch (op)
    {
    case 0:
        *addr = data;
        return;
    case 1:
        *addr += data;
        return;
    case 2:
        *addr -= data;
        return;
    case 3:
        *addr = mplayInfo->memAccArea[data];
        return;
    case 4:
        *addr += mplayInfo->memAccArea[data];
        return;
    case 5:
        *addr -= mplayInfo->memAccArea[data];
        return;
    case 6:
        MEMACC_COND_JUMP(*addr == data)
        return;
    case 7:
        MEMACC_COND_JUMP(*addr != data)
        return;
    case 8:
        MEMACC_COND_JUMP(*addr > data)
        return;
    case 9:
        MEMACC_COND_JUMP(*addr >= data)
        return;
    case 10:
        MEMACC_COND_JUMP(*addr <= data)
        return;
    case 11:
        MEMACC_COND_JUMP(*addr < data)
        return;
    case 12:
        MEMACC_COND_JUMP(*addr == mplayInfo->memAccArea[data])
        return;
    case 13:
        MEMACC_COND_JUMP(*addr != mplayInfo->memAccArea[data])
        return;
    case 14:
        MEMACC_COND_JUMP(*addr > mplayInfo->memAccArea[data])
        return;
    case 15:
        MEMACC_COND_JUMP(*addr >= mplayInfo->memAccArea[data])
        return;
    case 16:
        MEMACC_COND_JUMP(*addr <= mplayInfo->memAccArea[data])
        return;
    case 17:
        MEMACC_COND_JUMP(*addr < mplayInfo->memAccArea[data])
        return;
    default:
        return;
    }

cond_true:
    {
        /* *& defeats CSE of the array base with the element address
         * (pret comment): the ROM pools &gMPlayJumpTable[1] directly. */
        (*&gMPlayJumpTable[1])(mplayInfo, track);
        return;
    }

cond_false:
    track->cmdPtr += 4;
}

void ply_xcmd(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u32 n = *track->cmdPtr;
    track->cmdPtr++;

    gXcmdTable[n](mplayInfo, track);
}

void ply_xxx(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    gMPlayJumpTable[0](mplayInfo, track);
}

#define READ_XCMD_BYTE(var, n)         \
{                                      \
    u32 byte = track->cmdPtr[(n)];     \
    byte <<= n * 8;                    \
    (var) &= ~(0xFF << (n * 8));       \
    (var) |= byte;                     \
}

void ply_xwave(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    u32 wav;

    READ_XCMD_BYTE(wav, 0) /* UB: uninitialized variable (ROM built without UBFIX) */
    READ_XCMD_BYTE(wav, 1)
    READ_XCMD_BYTE(wav, 2)
    READ_XCMD_BYTE(wav, 3)

    track->tone.wav = (struct WaveData *)wav;
    track->cmdPtr += 4;
}

void ply_xtype(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.type = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xatta(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.attack = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xdeca(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.decay = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xsust(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.sustain = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xrele(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.release = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xiecv(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->echoVolume = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xiecl(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->echoLength = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xleng(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.length = *track->cmdPtr;
    track->cmdPtr++;
}

void ply_xswee(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track)
{
    track->tone.pan_sweep = *track->cmdPtr;
    track->cmdPtr++;
}
