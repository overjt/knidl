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

s32 sub_08003110(s32 songId)
{
    if (songId & 0x800)
    {
        gUnk_03000490 = -999;
        songId &= ~0x800;
    }
    if (songId > 56)
        return 0;
    if (songId < 0)
        return 0;
    if (gUnk_03000490 == songId)
    {
        if (gUnk_03000AF8 == 0)
            m4aSongNumStartOrContinue(songId);
        gUnk_03000490 = songId;
        return 0;
    }
    if (gUnk_03000AF8 == 0)
        m4aSongNumStart(songId);
    gUnk_03000490 = songId;
    return 1;
}

void sub_08003184(void)
{
    m4aMPlayImmInit(&gMPlayInfo_BGM);
}

s32 sub_08003194(void)
{
    if ((s32)gMPlayInfo_BGM.status < 0)
        return -1;
    return gUnk_03000490;
}

















