#include "gba/gba.h"
#include "global.h"

/* early_31b8.c (0x080031B8-0x08003483, issue #63).
 *
 * The sound-effect front end of src/early_3110.c: start sound effect `id`
 * (100-578, song gSfxTable[id - 100]) on one of the three SE players,
 * called from almost every module.  gSfxSlotSongs[slot] is the song id of SE
 * slot 1-3, gSfxSlotPlayers[slot] its music player (gMPlayTable) and
 * gSfxPlayerSlots the inverse map, gSfxSlotAges[slot] the slot's age (1 =
 * newest, 0 = unused); gSfxDisabled mutes sound effects.  A slot is taken
 * in this order: a stopped player the song's channel mask allows; a stopped
 * player one of the allowed slots' songs may move to (the two slots swap);
 * the allowed slot playing the lowest priority; on a priority tie the oldest.
 * The ages of the other slots are bumped, the song is started with
 * MPlayStart and the player index is returned (0 when muted or out of range,
 * -1 when no player can take it).
 *
 * Matching notes (issue #63): one scratch variable `t` serves as the move
 * mask, both swap temporaries, the running priority and both ages (the ROM
 * keeps all of them in r4), and the three range guards are separate `if`s;
 * lesson 3.480.  Issue #32 had called the residue a register permutation. */

struct SongEntry
{
    struct SongHeader *header;
    u8 prio;
    u8 chans;
    u8 pad[2];
};

extern const struct SongEntry gSfxTable[];
extern vu16 gSfxDisabled;
extern vu16 gSfxSlotSongs[];
extern vu8 gSfxPlayerSlots[];
extern vu8 gSfxSlotPlayers[];
extern vu8 gSfxSlotAges[];

/* Start sound effect `id` (100-578) on one of the three SE players: a free
 * player the song allows, else a player freed by moving its song to a
 * free one, else the allowed player with the lowest priority (the oldest
 * one on a tie).  Returns the player index, 0 when muted or out of range,
 * -1 when no player can take it. */
s32 PlaySfx(s32 id)
{
    const struct SongEntry *song;
    s32 slot;
    s32 free;
    s32 i;
    s32 j;
    s32 tie;
    s32 t;      /* ONE scratch for the mask, the swaps, the priority, the
                 * best age and the slot's age: the ROM keeps all five in
                 * r4, which only a single pseudo gets. */

    /* Three separate guards: an `||` chain lets reload inherit the first
     * `ldr [sp]` of id, the ROM re-loads it for the second compare. */
    if (gSfxDisabled != 0)
        return 0;
    if (id > 578)
        return 0;
    if (id < 100)
        return 0;
    id -= 100;
    song = &gSfxTable[id];
    if (song->header == NULL)
        return -1;
    slot = -1;
    free = 0;
    for (i = 1; i <= 3; i++)
        if (gMPlayTable[(s8)gSfxSlotPlayers[i]].info->status & 0x80000000)
            free |= 1 << i;
    /* No mask local: loop.c hoists `song->chans & free` and cse2 turns it
     * into the ROM's register copy. */
    if (song->chans & free)
    {
        for (i = 1; i <= 3; i++)
            if (((song->chans & free) >> i) & 1)
                goto found;
    }
    if (free != 0)
    {
        for (i = 1; i <= 3; i++)
        {
            if ((song->chans >> i) & 1)
            {
                t = gSfxTable[(s16)gSfxSlotSongs[i]].chans & free;
                if (t != 0)
                {
                    j = 0;
                    if (!(t & 1))
                    {
                        do
                        {
                            j++;
                            if (j > 3)
                                break;
                        } while (!((t >> j) & 1));
                    }
                    gSfxSlotSongs[j] = gSfxSlotSongs[i];
                    t = (s8)gSfxSlotAges[j];
                    gSfxSlotAges[j] = gSfxSlotAges[i];
                    gSfxSlotAges[i] = t;
                    t = (s8)gSfxSlotPlayers[j];
                    gSfxSlotPlayers[j] = gSfxSlotPlayers[i];
                    gSfxSlotPlayers[i] = t;
                    gSfxPlayerSlots[(s8)gSfxSlotPlayers[i]] = i;
                    gSfxPlayerSlots[(s8)gSfxSlotPlayers[j]] = j;
                    goto found;
                }
            }
        }
    }
    tie = 0;
    t = song->prio;
    for (i = 1; i <= 3; i++)
    {
        if ((song->chans >> i) & 1)
        {
            if (t > gSfxTable[(s16)gSfxSlotSongs[i]].prio)
            {
                t = gSfxTable[(s16)gSfxSlotSongs[i]].prio;
                slot = i;
            }
            else if (song->prio == gSfxTable[(s16)gSfxSlotSongs[i]].prio)
            {
                tie |= 1 << i;
            }
        }
    }
    if (slot == -1)
    {
        t = 0;
        for (i = 1; i <= 3; i++)
        {
            if (((tie >> i) & 1) && (s8)gSfxSlotAges[i] > t)
            {
                t = (s8)gSfxSlotAges[i];
                slot = i;
            }
        }
        if (slot == -1)
            return -1;
    }
    goto play;
found:
    slot = i;
play:
    t = (s8)gSfxSlotAges[slot];
    if (t == 0)
    {
        for (i = 1; i <= 3; i++)
            if (gSfxSlotAges[i] != 0)
                gSfxSlotAges[i]++;
    }
    else
    {
        for (i = 1; i <= 3; i++)
            if (gSfxSlotAges[i] != 0 && (s8)gSfxSlotAges[i] < t)
                gSfxSlotAges[i]++;
    }
    gSfxSlotAges[slot] = 1;
    MPlayStart(gMPlayTable[(s8)gSfxSlotPlayers[slot]].info, song->header);
    gSfxSlotSongs[slot] = id;
    return (s8)gSfxSlotPlayers[slot];
}
