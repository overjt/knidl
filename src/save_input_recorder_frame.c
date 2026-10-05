#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "save.h"

/* save_input_recorder_frame.c (0x080B75A4-0x080B77D3, issue #94).
 *
 * The input recorder and its playback, one step per frame for each player
 * (src/save_init_slots.c runs one or the other by the mode gInputRecorderMode; both
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
    s32 player;
    u16 key;
    s8 count;
    u16 pos;
    for (player = 0; player < gPlayerCount; player++)
    {
        if (gInputRecorderNextPos[player] >= gInputRecorderEndPos[player])
        {
            gInputRecorderRunning = 0;
            return;
        }
        key = gPlayerHeldKeys[player] & 0x3FF;
        count = gInputRecorderEntryFrames[player];
        pos = gInputRecorderCurPos[player];
        if (gInputRecorderKeys[player] != key)
        {
            count = 1;
            gInputRecorderKeys[player] = key;
            gInputRecorderCurPos[player] = gInputRecorderNextPos[player];
            gInputRecorderNextPos[player] += gPlayerCount;
            pos = gInputRecorderCurPos[player];
        }
        else if (++count == 63)
        {
            gInputRecorderKeys[player] = 0xFFFF;
        }
        gInputRecorderEntryFrames[player] = count;
        gInputRecordingPtr->keyLog[pos] = key | (count << 10);
        WriteInputRecordingEntry((u8 *)&gInputRecordingPtr->keyLog[pos], pos);
    }
}

void InputRecorderPlayFrame(void)
{
    s32 player;
    s32 j;
    u16 key;
    for (player = 0; player < gPlayerCount; player++)
    {
        gPlayerHeldKeys[player] = gPlayerPressedKeys[player] = 0;
        if (gInputRecorderNextPos[player] >= gInputRecorderEndPos[player])
        {
            gInputRecorderRunning = 0;
            return;
        }
        if ((s8)--gInputRecorderEntryFrames[player] <= 0)
        {
            gInputRecorderCurPos[player] = gInputRecorderNextPos[player];
            gInputRecorderNextPos[player] += gPlayerCount;
            key = gInputRecordingPtr->keyLog[gInputRecorderCurPos[player]] & 0x3FF;
            gInputRecorderEntryFrames[player] = gInputRecordingPtr->keyLog[gInputRecorderCurPos[player]] >> 10;
            if (key == 0x3FF)
            {
                for (j = 0; j < gPlayerCount; j++)
                    gInputRecorderNextPos[j] = gInputRecorderEndPos[j];
                return;
            }
            gPlayerPressedKeys[player] = key & ~gInputRecorderKeys[player];
            gInputRecorderKeys[player] = key;
        }
        gPlayerHeldKeys[player] = gInputRecorderKeys[player];
    }
}
