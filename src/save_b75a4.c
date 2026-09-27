#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "save.h"

/* save_b75a4.c (0x080B75A4-0x080B77D3, issue #94).
 *
 * The input recorder and its playback, one step per frame for each player
 * (src/save_b77d4.c runs one or the other by the mode gInputRecorderMode; both
 * clear the running flag gInputRecorderRunning when a player reaches the end).  The
 * recording lives in gInputRecordingPtr->unk12C[] as 16-bit entries, held keys
 * in the low 10 bits and a frame count (1-63) above them, the players'
 * entries interleaved: gUnk_0200EC78[i] is player i's next position and
 * steps by the player count gPlayerCount, gUnk_0200EC70[i] its end.
 * InputRecorderRecordFrame records gPlayerHeldKeys[i] (the held keys), extending the
 * current entry while the keys stay the same; InputRecorderPlayFrame plays it back into
 * gPlayerHeldKeys[i] and the newly pressed keys gPlayerPressedKeys[i], and an entry
 * of 0x3FF ends the playback for everyone.
 *
 * Matching notes: the plain `gUnk_0200EC60[i] = 0xFFFF;` is the ROM's
 * `ldr =0xFFFF; orrs` (a u16 element store is expanded as a bit-field store,
 * whose AND is dropped for an all-ones value and whose OR survives because cse
 * still holds the element from the `!=` test, lesson 3.469); the count is an
 * s8 local (3.340). */

u32 WriteInputRecordingEntry(u8 *src, s32 i);

void InputRecorderRecordFrame(void)
{
    s32 i;
    u16 key;
    s8 count;
    u16 pos;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gUnk_0200EC78[i] >= gUnk_0200EC70[i])
        {
            gInputRecorderRunning = 0;
            return;
        }
        key = gPlayerHeldKeys[i] & 0x3FF;
        count = gUnk_0200EC68[i];
        pos = gUnk_02016480[i];
        if (gUnk_0200EC60[i] != key)
        {
            count = 1;
            gUnk_0200EC60[i] = key;
            gUnk_02016480[i] = gUnk_0200EC78[i];
            gUnk_0200EC78[i] += gPlayerCount;
            pos = gUnk_02016480[i];
        }
        else if (++count == 63)
        {
            gUnk_0200EC60[i] = 0xFFFF;
        }
        gUnk_0200EC68[i] = count;
        gInputRecordingPtr->unk12C[pos] = key | (count << 10);
        WriteInputRecordingEntry((u8 *)&gInputRecordingPtr->unk12C[pos], pos);
    }
}

void InputRecorderPlayFrame(void)
{
    s32 i;
    s32 j;
    u16 key;
    for (i = 0; i < gPlayerCount; i++)
    {
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = 0;
        if (gUnk_0200EC78[i] >= gUnk_0200EC70[i])
        {
            gInputRecorderRunning = 0;
            return;
        }
        if ((s8)--gUnk_0200EC68[i] <= 0)
        {
            gUnk_02016480[i] = gUnk_0200EC78[i];
            gUnk_0200EC78[i] += gPlayerCount;
            key = gInputRecordingPtr->unk12C[gUnk_02016480[i]] & 0x3FF;
            gUnk_0200EC68[i] = gInputRecordingPtr->unk12C[gUnk_02016480[i]] >> 10;
            if (key == 0x3FF)
            {
                for (j = 0; j < gPlayerCount; j++)
                    gUnk_0200EC78[j] = gUnk_0200EC70[j];
                return;
            }
            gPlayerPressedKeys[i] = key & ~gUnk_0200EC60[i];
            gUnk_0200EC60[i] = key;
        }
        gPlayerHeldKeys[i] = gUnk_0200EC60[i];
    }
}
