#include "gba/gba.h"
#include "global.h"

/* Early game-code block 0x08003110-0x08003887 (issue #32, batch D1): the
 * BGM/SE front end that sits on top of the m4a driver.
 *
 * RECIPE: this translation unit is old_agbcc -O2 -mthumb-interwork
 * (tools/fnmatch.sh --old2), NOT the default agbcc -O2.  Fingerprint per
 * lesson 3.18: sub_08003194, sub_0800374c, sub_080037f8 and sub_0800381c are
 * leaves that end in a bare `bx lr` with no `push {lr}` at all.
 *
 * Contents, in ROM order (four functions have no symbols.csv entry of their
 * own - lesson 2.13 dead exports hidden inside a neighbour's size):
 *   sub_08003110  start/continue a BGM by song id; bit 0x800 of the argument
 *                 means "forget the current song first".
 *   sub_08003184  m4aMPlayImmInit on the BGM player.
 *   sub_08003194  current BGM song id, or -1 while the BGM player is paused.
 *   sub_080031b8  the SE allocator: picks one of the three SE players for a
 *                 song, stealing/ageing slots by channel mask and priority.
 *   sub_08003484  stop everything and forget the current BGM.
 *   sub_080034a0  (hidden) stop everything unless sound is muted.
 *   sub_080034b8  (hidden) continue everything unless sound is muted.
 *   sub_080034d0  stop the BGM player (gMPlayTable[0]).
 *   sub_080034f0  stop one SE player if it is playing this song.
 *   sub_08003564  stop every SE player playing this song; returns a bitmask.
 *   sub_080035f4  stop every SE player NOT playing this song; returns the
 *                 bitmask of the ones that were.
 *   sub_08003688  stop all three SE players.
 *   sub_080036b8  (hidden) BGM change with a fade request seeded first.
 *   sub_0800374c  fade-out request (mode 2).
 *   sub_08003770  BGM volume.
 *   sub_080037a4  (hidden) SE volume (all three SE players).
 *   sub_080037f8  fade request (mode 3).
 *   sub_0800381c  fade request (mode 4).
 *   sub_08003840  m4aSoundVSyncOff + forget the BGM.
 *   sub_08003864  m4aSoundVSyncOn + forget the BGM.
 *
 * Matching notes:
 *  - gUnk_03000490 must be `vs16`, not `vu16`: only the signed type keeps the
 *    -999 "no song" constant as the ROM's full-word 0xFFFFFC19 pool entry
 *    (a u16 destination truncates it to 0x0000FC19 at compile time), and the
 *    volatile read is what gives the ROM's `ldrh` + `lsls #16` + `asrs #16`
 *    instead of a narrowed `ldrsh`.
 *  - `if (a > 56) return; if (a < 0) return;` must be two separate `if`s:
 *    written as `a > 56 || a < 0`, fold collapses the pair into a single
 *    unsigned `(u32)a > 56` compare and the ROM's two compares disappear.
 *  - sub_08003564/sub_080035f4 need the 478 bound in a `limit` local.  Spelled
 *    as a literal, gcc materialises the constant AFTER computing `songId-100`;
 *    the ROM materialises it first, and only a variable reproduces that order.
 *  - sub_080036b8 needs `s16 id`, not `s32 id` + `(s16)` casts: only a real
 *    HImode local lets combine rewrite `(u16)id` into the ROM's single
 *    `lsrs r0, r4, #16` reusing the `songId << 16` value (lesson 3.27).
 *  - the arrays are volatile so that the indexed stores emit the ROM's dead
 *    pre-read `ldrb`/`ldrh` before the `strb`/`strh` (lesson 3.9).
 *
 * NOT YET MATCHING: sub_080031b8 is 36 bytes off (the size is exact, and every
 * instruction up to 0x0800331e is byte-identical).  The residue is a pure
 * register permutation over its last third: the ROM puts the priority mask in
 * `sl` and the song-table base in `r9` (plus prio=r4 / walk=r2), this source
 * gets the mirror image (mask=r9, table=sl, prio=r2, walk=r1).  Nothing in the
 * C changes it - 60+ shape variants (block scoping, declaration order,
 * variable sharing, loop forms, explicit walking pointers, `register ... asm`
 * pinning) all reproduce the same allocation.
 * without it gcc folds `t = e->chans; t &= busy;` into one expression, swaps
 * the AND operands and emits an extra `adds r4, r2, #0` (candidate 720 bytes
 * instead of 716).  The asm gives `t` a second reference so combine leaves the
 * load alone, which is what makes the ROM's two-instruction
 * `ldrb r4,[r0,#5]` / `ands r4,r2` come out.  Whatever the original source did
 * there (it is a single AND in the ROM) probably also explains the register
 * permutation above.
 */

extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;

struct SongEntry
{
    struct SongHeader *header;
    u8 prio;
    u8 chans;
    u8 pad[2];
};

extern const struct SongEntry gUnk_0872EB38[];

extern vs16 gUnk_03000490;
extern vu16 gUnk_03000AF8;
extern vu16 gUnk_03000B0C;
extern vu16 gUnk_03000FBC;
extern vu16 gUnk_03000FCC;
extern vu16 gUnk_03001EDC;
extern vu16 gUnk_03001EE4;

extern vu16 gUnk_03000F80[];
extern vu8 gUnk_0300001C[];
extern vu8 gUnk_03001180[];
extern vu8 gUnk_03001674[];





void sub_08003484(void)
{
    gUnk_03000490 = -999;
    m4aMPlayAllStop();
}

void sub_080034a0(void)
{
    if (gUnk_03000AF8 == 0)
        m4aMPlayAllStop();
}

void sub_080034b8(void)
{
    if (gUnk_03000AF8 == 0)
        m4aMPlayAllContinue();
}

void sub_080034d0(void)
{
    if (gUnk_03000AF8 == 0)
        m4aMPlayStop(gMPlayTable[0].info);
}

void sub_080034f0(s32 player, s32 songId)
{
    struct MusicPlayerInfo *info;
    s32 id;

    if (gUnk_03001EDC != 0)
        return;
    id = songId - 100;
    if ((u32)id > 478)
        return;
    if ((u32)player > 3)
        return;
    info = gMPlayTable[player].info;
    if ((s32)info->status < 0)
        return;
    if ((s16)gUnk_03000F80[(s8)gUnk_0300001C[player]] != id)
        return;
    m4aMPlayStop(info);
    gUnk_03000F80[(s8)gUnk_0300001C[player]] = 0xFFFF;
}

s32 sub_08003564(s32 songId)
{
    struct MusicPlayerInfo *info;
    s32 mask;
    s32 id;
    s32 i;
    s32 limit;

    mask = 0;
    if (gUnk_03001EDC != 0)
        return 0;
    limit = 478;
    if ((u32)(id = songId - 100) > limit)
        return 0;
    for (i = 1; i <= 3; i++)
    {
        info = gMPlayTable[i].info;
        if ((s32)info->status < 0)
            continue;
        if ((s16)gUnk_03000F80[(s8)gUnk_0300001C[i]] != id)
            continue;
        m4aMPlayStop(info);
        gUnk_03000F80[(s8)gUnk_0300001C[i]] = 0xFFFF;
        mask |= 1 << i;
    }
    return mask;
}

s32 sub_080035f4(s32 songId)
{
    struct MusicPlayerInfo *info;
    s32 mask;
    s32 id;
    s32 i;
    s32 limit;

    mask = 0;
    if (gUnk_03001EDC != 0)
        return 0;
    limit = 478;
    if ((u32)(id = songId - 100) > limit)
        return 0;
    for (i = 1; i <= 3; i++)
    {
        info = gMPlayTable[i].info;
        if ((s32)info->status < 0)
            continue;
        if ((s16)gUnk_03000F80[(s8)gUnk_0300001C[i]] != id)
        {
            m4aMPlayStop(info);
            gUnk_03000F80[(s8)gUnk_0300001C[i]] = 0xFFFF;
        }
        else
        {
            mask |= 1 << i;
        }
    }
    return mask;
}

void sub_08003688(void)
{
    s32 i;

    if (gUnk_03001EDC != 0)
        return;
    for (i = 1; i <= 3; i++)
        m4aMPlayStop(gMPlayTable[i].info);
}

void sub_080036b8(u16 speed, u16 songId)
{
    s16 id;
    s32 flag;

    id = (s16)songId;
    if (id > 56)
        return;
    if (id < 0)
        return;
    flag = (s16)gUnk_03000AF8;
    if (flag != 0)
    {
        gUnk_03000490 = songId;
        return;
    }
    gUnk_03000B0C = 1;
    gUnk_03000FBC = flag;
    gUnk_03000FCC = speed;
    if (gUnk_03000490 == id)
    {
        if (gUnk_03000AF8 == 0)
            m4aSongNumStartOrContinue(id);
        return;
    }
    if (id == -1)
        return;
    if (gUnk_03000AF8 != 0)
        return;
    m4aSongNumStart(id);
    m4aMPlayImmInit(&gMPlayInfo_BGM);
    gUnk_03000490 = songId;
}

void sub_0800374c(s32 speed)
{
    gUnk_03000B0C = 2;
    gUnk_03000FBC = 256;
    gUnk_03000FCC = -speed;
}

void sub_08003770(u16 volume)
{
    if (volume > 256)
        volume = 256;
    gUnk_03000FBC = volume;
    if (gUnk_03000AF8 == 0)
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFF, volume);
}

void sub_080037a4(u16 volume)
{
    if (volume > 256)
        volume = 256;
    gUnk_03000FBC = volume;
    if (gUnk_03000AF8 == 0)
    {
        m4aMPlayVolumeControl(&gMPlayInfo_SE1, 0xFF, volume);
        m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFF, volume);
        m4aMPlayVolumeControl(&gMPlayInfo_SE3, 0xFF, volume);
    }
}

void sub_080037f8(u16 speed)
{
    gUnk_03000B0C = 3;
    gUnk_03000FBC = 0;
    gUnk_03000FCC = speed;
}

void sub_0800381c(s32 speed)
{
    gUnk_03000B0C = 4;
    gUnk_03000FBC = 256;
    gUnk_03000FCC = -speed;
}

void sub_08003840(void)
{
    m4aSoundVSyncOff();
    gUnk_03000490 = -999;
    gUnk_03001EE4 = 0;
}

void sub_08003864(void)
{
    m4aSoundVSyncOn();
    gUnk_03000490 = -999;
    gUnk_03001EE4 = 1;
}
