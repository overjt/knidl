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

#endif /* GUARD_SOUND_H */
