#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* early_31b8.c (0x080031B8-0x08003483, issue #63).
 *
 * The sound-effect front end of src/early_3110.c: start sound effect `id`
 * (100-578, song gUnk_0872EB38[id - 100]) on one of the three SE players,
 * called from almost every module.  gUnk_03000F80[slot] is the song id of SE
 * slot 1-3, gUnk_03001180[slot] its music player (gMPlayTable) and
 * gUnk_0300001C the inverse map, gUnk_03001674[slot] the slot's age (1 =
 * newest, 0 = unused); gUnk_03001EDC mutes sound effects.  A slot is taken
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

extern const struct SongEntry gUnk_0872EB38[];
extern vu16 gUnk_03001EDC;
extern vu16 gUnk_03000F80[];
extern vu8 gUnk_0300001C[];
extern vu8 gUnk_03001180[];
extern vu8 gUnk_03001674[];

/* Start sound effect `id` (100-578) on one of the three SE players: a free
 * player the song allows, else a player freed by moving its song to a
 * free one, else the allowed player with the lowest priority (the oldest
 * one on a tie).  Returns the player index, 0 when muted or out of range,
 * -1 when no player can take it. */
s32 sub_080031b8(s32 id)
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
    if (gUnk_03001EDC != 0)
        return 0;
    if (id > 578)
        return 0;
    if (id < 100)
        return 0;
    id -= 100;
    song = &gUnk_0872EB38[id];
    if (song->header == NULL)
        return -1;
    slot = -1;
    free = 0;
    for (i = 1; i <= 3; i++)
        if (gMPlayTable[(s8)gUnk_03001180[i]].info->status & 0x80000000)
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
                t = gUnk_0872EB38[(s16)gUnk_03000F80[i]].chans & free;
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
                    gUnk_03000F80[j] = gUnk_03000F80[i];
                    t = (s8)gUnk_03001674[j];
                    gUnk_03001674[j] = gUnk_03001674[i];
                    gUnk_03001674[i] = t;
                    t = (s8)gUnk_03001180[j];
                    gUnk_03001180[j] = gUnk_03001180[i];
                    gUnk_03001180[i] = t;
                    gUnk_0300001C[(s8)gUnk_03001180[i]] = i;
                    gUnk_0300001C[(s8)gUnk_03001180[j]] = j;
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
            if (t > gUnk_0872EB38[(s16)gUnk_03000F80[i]].prio)
            {
                t = gUnk_0872EB38[(s16)gUnk_03000F80[i]].prio;
                slot = i;
            }
            else if (song->prio == gUnk_0872EB38[(s16)gUnk_03000F80[i]].prio)
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
            if (((tie >> i) & 1) && (s8)gUnk_03001674[i] > t)
            {
                t = (s8)gUnk_03001674[i];
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
    t = (s8)gUnk_03001674[slot];
    if (t == 0)
    {
        for (i = 1; i <= 3; i++)
            if (gUnk_03001674[i] != 0)
                gUnk_03001674[i]++;
    }
    else
    {
        for (i = 1; i <= 3; i++)
            if (gUnk_03001674[i] != 0 && (s8)gUnk_03001674[i] < t)
                gUnk_03001674[i]++;
    }
    gUnk_03001674[slot] = 1;
    MPlayStart(gMPlayTable[(s8)gUnk_03001180[slot]].info, song->header);
    gUnk_03000F80[slot] = id;
    return (s8)gUnk_03001180[slot];
}
