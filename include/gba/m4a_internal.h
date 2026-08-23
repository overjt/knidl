#ifndef GUARD_GBA_M4A_INTERNAL_H
#define GUARD_GBA_M4A_INTERNAL_H

#include "gba/types.h"
#include "gba/syscall.h"

/*
 * m4a/mp2k sound engine internals (issue #31/#53).  Struct layouts and
 * names follow the SDK sources as recovered by the pret projects (this
 * build's shapes match katam's m4a variant: PCM_DMA_BUF_SIZE 1584,
 * pcmSamplesPerVBlank as s32 at offset 16, ExtVolPit at offset 60).
 * Byte-verified against this ROM's driver at 0x080CD89C-0x080CFA4B.
 */

/* ASCII 'Smsh' (reversed) — presumably short for SMASH, the developer of
 * MKS4AGB.  soundInfo->ident holds this except during locked operations. */
#define ID_NUMBER 0x68736D53

#define C_V 0x40 /* center value for PAN, BEND and TUNE */

#define SOUND_MODE_REVERB_VAL   0x0000007F
#define SOUND_MODE_REVERB_SET   0x00000080
#define SOUND_MODE_MAXCHN       0x00000F00
#define SOUND_MODE_MAXCHN_SHIFT 8
#define SOUND_MODE_MASVOL       0x0000F000
#define SOUND_MODE_MASVOL_SHIFT 12
#define SOUND_MODE_FREQ_05734   0x00010000
#define SOUND_MODE_FREQ_07884   0x00020000
#define SOUND_MODE_FREQ_10512   0x00030000
#define SOUND_MODE_FREQ_13379   0x00040000
#define SOUND_MODE_FREQ_15768   0x00050000
#define SOUND_MODE_FREQ_18157   0x00060000
#define SOUND_MODE_FREQ_21024   0x00070000
#define SOUND_MODE_FREQ_26758   0x00080000
#define SOUND_MODE_FREQ_31536   0x00090000
#define SOUND_MODE_FREQ_36314   0x000A0000
#define SOUND_MODE_FREQ_40137   0x000B0000
#define SOUND_MODE_FREQ_42048   0x000C0000
#define SOUND_MODE_FREQ         0x000F0000
#define SOUND_MODE_FREQ_SHIFT   16
#define SOUND_MODE_DA_BIT_9     0x00800000
#define SOUND_MODE_DA_BIT_8     0x00900000
#define SOUND_MODE_DA_BIT_7     0x00A00000
#define SOUND_MODE_DA_BIT_6     0x00B00000
#define SOUND_MODE_DA_BIT       0x00B00000
#define SOUND_MODE_DA_BIT_SHIFT 20

struct WaveData
{
    u16 type;
    u16 status;
    u32 freq;
    u32 loopStart;
    u32 size; /* number of samples */
    s8 data[1]; /* samples */
};

#define TONEDATA_TYPE_CGB 0x07
#define TONEDATA_TYPE_FIX 0x08
#define TONEDATA_TYPE_SPL 0x40 /* key split */
#define TONEDATA_TYPE_RHY 0x80 /* rhythm */

#define TONEDATA_P_S_PAN 0xc0
#define TONEDATA_P_S_PAM TONEDATA_P_S_PAN

struct ToneData
{
    u8 type;
    u8 key;
    u8 length; /* sound length (compatible sound) */
    u8 pan_sweep; /* pan or sweep (compatible sound ch. 1) */
    struct WaveData *wav;
    u8 attack;
    u8 decay;
    u8 sustain;
    u8 release;
};

/* CgbChannel sf (status flags) bits, pret names. */
#define SOUND_CHANNEL_SF_START       0x80
#define SOUND_CHANNEL_SF_STOP        0x40
#define SOUND_CHANNEL_SF_LOOP        0x10
#define SOUND_CHANNEL_SF_IEC         0x04
#define SOUND_CHANNEL_SF_ENV         0x03
#define SOUND_CHANNEL_SF_ENV_ATTACK  0x03
#define SOUND_CHANNEL_SF_ENV_DECAY   0x02
#define SOUND_CHANNEL_SF_ENV_SUSTAIN 0x01
#define SOUND_CHANNEL_SF_ENV_RELEASE 0x00
#define SOUND_CHANNEL_SF_ON (SOUND_CHANNEL_SF_START | SOUND_CHANNEL_SF_STOP \
                           | SOUND_CHANNEL_SF_IEC | SOUND_CHANNEL_SF_ENV)

/* CgbChannel mo (modify) bits. */
#define CGB_CHANNEL_MO_PIT 0x02
#define CGB_CHANNEL_MO_VOL 0x01

/* NRx2 envelope direction bit. */
#define CGB_NRx2_ENV_DIR_INC 0x08
#define CGB_NRx2_ENV_DIR_DEC 0x00

struct CgbChannel
{
    u8 sf;
    u8 ty;
    u8 rightVolume;
    u8 leftVolume;
    u8 at;
    u8 de;
    u8 su;
    u8 re;
    u8 ky;
    u8 ev;
    u8 eg;
    u8 ec;
    u8 echoVolume;
    u8 echoLength;
    u8 d1;
    u8 d2;
    u8 gt;
    u8 mk;
    u8 ve;
    u8 pr;
    u8 rp;
    u8 d3[3];
    u8 d5;
    u8 sg;
    u8 n4;
    u8 pan;
    u8 panMask;
    u8 mo;
    u8 le;
    u8 sw;
    u32 fr;
    u32 *wp;
    u32 cp;
    u32 tp;
    u32 pp;
    u32 np;
    u8 d4[8];
};

struct MusicPlayerTrack;

struct SoundChannel
{
    u8 status;
    u8 type;
    u8 rightVolume;
    u8 leftVolume;
    u8 attack;
    u8 decay;
    u8 sustain;
    u8 release;
    u8 ky;
    u8 ev;
    u8 er;
    u8 el;
    u8 echoVolume;
    u8 echoLength;
    u8 d1;
    u8 d2;
    u8 gt;
    u8 mk;
    u8 ve;
    u8 pr;
    u8 rp;
    u8 d3[3];
    u32 ct;
    u32 fw;
    u32 freq;
    struct WaveData *wav;
    u32 cp;
    struct MusicPlayerTrack *track;
    u32 pp;
    u32 np;
    u32 d4;
    u16 xpi;
    u16 xpc;
};

#define MAX_DIRECTSOUND_CHANNELS 12

#define PCM_DMA_BUF_SIZE 1584 /* size of Direct Sound buffer */

struct SoundInfo
{
    /* Normally equal to ID_NUMBER; set to other values during sensitive
     * operations for locking purposes. */
    u32 ident;
    vu8 pcmDmaCounter;

    /* Direct Sound */
    u8 reverb;
    u8 maxChans;
    u8 masterVolume;
    u8 freq;

    u8 mode;
    u8 c15;
    u8 pcmDmaPeriod; /* number of V-blanks per PCM DMA */
    u8 maxLines;
    u8 gap[3];
    s32 pcmSamplesPerVBlank;
    s32 pcmFreq;
    s32 divFreq;
    struct CgbChannel *cgbChans;
    u32 func;
    u32 intp;
    void (*CgbSound)(void);
    void (*CgbOscOff)(u8);
    u32 (*MidiKeyToCgbFreq)(u8, u8, u8);
    u32 MPlayJumpTable;
    void *plynote;
    void (*ExtVolPit)(void);
    u8 gap2[16];
    struct SoundChannel chans[MAX_DIRECTSOUND_CHANNELS];
    s8 pcmBuffer[PCM_DMA_BUF_SIZE * 2];
};

struct SongHeader
{
    u8 trackCount;
    u8 blockCount;
    u8 priority;
    u8 reverb;
    struct ToneData *tone;
    u8 *part[1];
};

#define MPT_FLG_VOLSET 0x01
#define MPT_FLG_VOLCHG 0x03
#define MPT_FLG_PITSET 0x04
#define MPT_FLG_PITCHG 0x0C
#define MPT_FLG_START  0x40
#define MPT_FLG_EXIST  0x80

struct MusicPlayerTrack
{
    u8 flags;
    u8 wait;
    u8 patternLevel;
    u8 repN;
    u8 gateTime;
    u8 key;
    u8 velocity;
    u8 runningStatus;
    u8 keyM;
    u8 pitM;
    s8 keyShift;
    s8 keyShiftX;
    s8 tune;
    u8 pitX;
    s8 bend;
    u8 bendRange;
    u8 volMR;
    u8 volML;
    u8 vol;
    u8 volX;
    s8 pan;
    s8 panX;
    s8 modM;
    u8 mod;
    u8 modT;
    u8 lfoSpeed;
    u8 lfoSpeedC;
    u8 lfoDelay;
    u8 lfoDelayC;
    u8 priority;
    u8 echoVolume;
    u8 echoLength;
    struct SoundChannel *chan;
    struct ToneData tone;
    u8 gap[10];
    u16 unk_3A;
    u32 unk_3C;
    u8 *cmdPtr;
    u8 *patternStack[3];
};

#define MUSICPLAYER_STATUS_TRACK 0x0000ffff
#define MUSICPLAYER_STATUS_PAUSE 0x80000000

#define MAX_MUSICPLAYER_TRACKS 16

#define TEMPORARY_FADE 0x0001
#define FADE_IN        0x0002
#define FADE_VOL_MAX   64
#define FADE_VOL_SHIFT 2

struct MusicPlayerInfo
{
    struct SongHeader *songHeader;
    u32 status;
    u8 trackCount;
    u8 priority;
    u8 cmd;
    u8 unk_B;
    u32 clock;
    u8 gap[8];
    u8 *memAccArea;
    u16 tempoD;
    u16 tempoU;
    u16 tempoI;
    u16 tempoC;
    u16 fadeOI;
    u16 fadeOC;
    u16 fadeOV;
    struct MusicPlayerTrack *tracks;
    struct ToneData *tone;
    u32 ident;
    u32 func;
    u32 intp;
};

struct MusicPlayer
{
    struct MusicPlayerInfo *info;
    struct MusicPlayerTrack *track;
    u8 unk_8;
    u16 unk_A;
};

struct Song
{
    struct SongHeader *header;
    u16 ms;
    u16 me;
};

/* CpuSet wrappers (pret gba/macro.h precedent).  The control words match
 * the ROM's pools: 0x04000100 (copy 0x400 bytes), 0x050003EC (fill
 * sizeof(struct SoundInfo)), 0x05000040, 0x05000318. */
#define CPU_SET_SRC_FIXED 0x01000000
#define CPU_SET_32BIT     0x04000000

#define CpuCopy32(src, dest, size) \
    CpuSet(src, dest, CPU_SET_32BIT | ((size) >> 2))

#define CpuFill32(value, dest, size)                                   \
{                                                                      \
    vu32 tmp = (vu32)(value);                                          \
    CpuSet((const void *)&tmp, dest,                                   \
           CPU_SET_SRC_FIXED | CPU_SET_32BIT | ((size) >> 2));         \
}

/* RAM cells (tools/split_config.json data_symbols, rom-map §8.4). */
extern struct SoundInfo gSoundInfo;                     /* 0x030056D0 */
typedef void (*MPlayFunc)();
extern MPlayFunc gMPlayJumpTable[36];                   /* 0x03006680 */

/* Engine rodata xcmd dispatch table (asm/m4a_engine_rodata.s, §8.3). */
typedef void (*XcmdFunc)(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
extern const XcmdFunc gXcmdTable[];                     /* 0x0860A3E8 */
extern struct CgbChannel gCgbChans[4];                  /* 0x03006710 */
extern u8 gMPlayMemAccArea[0x10];                       /* 0x030068D0 */
extern char gSoundMainRAM_Buffer[0x400];                /* 0x03007150 */
extern struct SoundInfo *SOUND_INFO_PTR;                /* 0x03007FF0 */

/* Engine rodata (asm/m4a_engine_rodata.s + asm/m4a_song_table.s, §8.3). */
extern const struct MusicPlayer gMPlayTable[];          /* 0x0860B430 */
extern const struct Song gSongTable[];                  /* 0x0860B460 */
extern const u8 gScaleTable[];
extern const u32 gFreqTable[];
extern const u16 gPcmSamplesPerVBlankTable[];
extern const u8 gCgbScaleTable[];
extern const s16 gCgbFreqTable[];
extern const u8 gNoiseTable[];
extern const u8 gCgb3Vol[];

/* Link-time constants (absolute symbols in asm/rom_syms.s): the SDK reads
 * the music-player count and max-lines limit as the ADDRESS of these
 * symbols — the values live in the linker map, not in memory. */
extern char gNumMusicPlayers[];
extern char gMaxLines[];

#define NUM_MUSIC_PLAYERS ((u16)(u32)gNumMusicPlayers)
#define MAX_LINES ((u32)gMaxLines)

/* asm core (asm/m4a_1.s, 0x080CD89C-0x080CE51F) */
u32 umul3232H32(u32 multiplier, u32 multiplicand);
void SoundMain(void);
void SoundMainBTM(void);
void RealClearChain(void *x);
void MPlayJumpTableCopy(MPlayFunc *mplayJumpTable);
void TrackStop(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void MPlayMain(void);
extern char SoundMainRAM[];

/* SDK stubs outside the driver */
void DummyFunc(void);                                   /* 0x080CFA4C */

/* C driver (src/m4a.c, 0x080CE520-) */
void MPlayContinue(struct MusicPlayerInfo *mplayInfo);
void MPlayFadeOut(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aSoundInit(void);
void m4aSoundMain(void);
void m4aSongNumStart(u16 n);
void m4aSongNumStartOrChange(u16 n);
void m4aSongNumStartOrContinue(u16 n);
void m4aSongNumStop(u16 n);
void m4aSongNumContinue(u16 n);
void m4aMPlayAllStop(void);
void m4aMPlayContinue(struct MusicPlayerInfo *mplayInfo);
void m4aMPlayAllContinue(void);
void m4aMPlayFadeOut(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aMPlayFadeIn(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aMPlayImmInit(struct MusicPlayerInfo *mplayInfo);
void MPlayExtender(struct CgbChannel *cgbChans);
void MusicPlayerJumpTableCopy(void);
void ClearChain(void *x);
void Clear64byte(void *x);
void SoundInit(struct SoundInfo *soundInfo);
void SampleFreqSet(u32 freq);
void m4aSoundMode(u32 mode);
void SoundClear(void);
void m4aSoundVSyncOff(void);
void m4aSoundVSyncOn(void);
void MPlayOpen(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *tracks, u8 trackCount);
void MPlayStart(struct MusicPlayerInfo *mplayInfo, struct SongHeader *songHeader);
void m4aMPlayStop(struct MusicPlayerInfo *mplayInfo);
void FadeOutBody(struct MusicPlayerInfo *mplayInfo);
void TrkVolPitSet(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);

/* C driver part 2, CGB/PSG side (src/m4a_cgb.c, 0x080CEFB4-0x080CF587) */
u32 MidiKeyToCgbFreq(u8 chanNum, u8 key, u8 fineAdjust);
void CgbOscOff(u8 chanNum);
void CgbModVol(struct CgbChannel *chan);
void CgbSound(void);
void m4aMPlayTempoControl(struct MusicPlayerInfo *mplayInfo, u16 tempo);

/* C driver part 3 (src/m4a_ctrl.c, 0x080CF588-0x080CFA4B) */
void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume);
void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch);
void m4aMPlayPanpotControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s8 pan);
void ClearModM(struct MusicPlayerTrack *track);
void m4aMPlayModDepthSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 modDepth);
void m4aMPlayLFOSpeedSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 lfoSpeed);

/* sound command handlers (asm core + parts 3, ply_* names per pret) */
void ply_fine(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_goto(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_patt(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_pend(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_rept(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_memacc(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_prio(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_tempo(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_keysh(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_voice(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_volu(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_pan(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_bend(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_bendr(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_lfos(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_lfodl(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_mod(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_modt(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_tune(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_port(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xcmd(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_endtie(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_note(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);

#endif /* GUARD_GBA_M4A_INTERNAL_H */
