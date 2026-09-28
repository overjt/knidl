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
 * entries interleaved: gInputRecorderNextPos[i] is player i's next position and
 * steps by the player count gPlayerCount, gInputRecorderEndPos[i] its end.
 * InputRecorderRecordFrame records gPlayerHeldKeys[i] (the held keys), extending the
 * current entry while the keys stay the same; InputRecorderPlayFrame plays it back into
 * gPlayerHeldKeys[i] and the newly pressed keys gPlayerPressedKeys[i], and an entry
 * of 0x3FF ends the playback for everyone.
 *
 * Matching notes: the plain `gInputRecorderKeys[i] = 0xFFFF;` is the ROM's
 * `ldr =0xFFFF; orrs` (a u16 element store is expanded as a bit-field store,
 * whose AND is dropped for an all-ones value and whose OR survives because cse
 * still holds the element from the `!=` test, lesson 3.469); the count is an
 * s8 local (3.340). */

void InputRecorderRecordFrame(void)
{
    s32 i;
    u16 key;
    s8 count;
    u16 pos;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gInputRecorderNextPos[i] >= gInputRecorderEndPos[i])
        {
            gInputRecorderRunning = 0;
            return;
        }
        key = gPlayerHeldKeys[i] & 0x3FF;
        count = gInputRecorderEntryFrames[i];
        pos = gInputRecorderCurPos[i];
        if (gInputRecorderKeys[i] != key)
        {
            count = 1;
            gInputRecorderKeys[i] = key;
            gInputRecorderCurPos[i] = gInputRecorderNextPos[i];
            gInputRecorderNextPos[i] += gPlayerCount;
            pos = gInputRecorderCurPos[i];
        }
        else if (++count == 63)
        {
            gInputRecorderKeys[i] = 0xFFFF;
        }
        gInputRecorderEntryFrames[i] = count;
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
        if (gInputRecorderNextPos[i] >= gInputRecorderEndPos[i])
        {
            gInputRecorderRunning = 0;
            return;
        }
        if ((s8)--gInputRecorderEntryFrames[i] <= 0)
        {
            gInputRecorderCurPos[i] = gInputRecorderNextPos[i];
            gInputRecorderNextPos[i] += gPlayerCount;
            key = gInputRecordingPtr->unk12C[gInputRecorderCurPos[i]] & 0x3FF;
            gInputRecorderEntryFrames[i] = gInputRecordingPtr->unk12C[gInputRecorderCurPos[i]] >> 10;
            if (key == 0x3FF)
            {
                for (j = 0; j < gPlayerCount; j++)
                    gInputRecorderNextPos[j] = gInputRecorderEndPos[j];
                return;
            }
            gPlayerPressedKeys[i] = key & ~gInputRecorderKeys[i];
            gInputRecorderKeys[i] = key;
        }
        gPlayerHeldKeys[i] = gInputRecorderKeys[i];
    }
}
