#ifndef GUARD_SOUND_H
#define GUARD_SOUND_H

#include "gba/types.h"

/* sound.h: the RAM cells and ROM tables of the BGM/SE front end over the m4a
   engine (engine zone) and the m4a C driver.  One declaration per symbol,
   with the type its consumers prove (issue #36 phase 2,
   docs/header-conventions.md). */

struct SongHeader;

struct SongEntry
{
    struct SongHeader *header;
    u8 prio;
    u8 chans;
    u8 pad[2];
};

/* IWRAM */
extern vu8 gSfxPlayerSlots[];
extern vs16 gCurrentBgm;
extern vu16 gSfxSlotSongs[];
extern vu8 gSfxSlotPlayers[];
extern vu8 gSfxSlotAges[];
extern vu16 gSfxDisabled;

/* ROM */
extern const struct SongEntry gSfxTable[];


/* Functions (defined in the files named above each group). */

struct MusicPlayerInfo;
struct MusicPlayerTrack;

/* src/early_3110.c */
s32 PlayBgm(s32 songId);
void ResetBgmPlayer(void);
s32 GetCurrentBgm(void);

/* src/early_3484.c */
void StopAllSound(void);
void PauseAllSound(void);
void ResumeAllSound(void);
void StopBgm(void);
void StopSfxOnPlayer(s32 player, s32 songId);
s32 StopSfx(s32 songId);
s32 StopOtherSfx(s32 songId);
void StopAllSfx(void);
void PlayBgmFadeIn(u16 speed, u16 songId);
void FadeOutBgm(s32 speed);
void SetBgmVolume(u16 volume);
void SetSfxVolume(u16 volume);
void FadeInSfx(u16 speed);
void FadeOutSfx(s32 speed);
void DisableSoundDriver(void);
void EnableSoundDriver(void);

/* src/m4a_ctrl.c */
void ply_xxx(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xwave(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xtype(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xatta(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xdeca(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xsust(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xrele(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xiecv(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xiecl(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xleng(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xswee(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);

#endif /* GUARD_SOUND_H */
