#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b75a4.c (0x080B75A4-0x080B77D3, issue #94).
 *
 * The input recorder and its playback, one step per frame for each player
 * (src/save_b77d4.c runs one or the other by the mode gInputRecorderMode; both
 * clear the running flag gInputRecorderRunning when a player reaches the end).  The
 * recording lives in gInputRecordingPtr->unk12C[] as 16-bit entries, held keys
 * in the low 10 bits and a frame count (1-63) above them, the players'
 * entries interleaved: gUnk_0200EC78[i] is player i's next position and
 * steps by the player count gUnk_030023AC, gUnk_0200EC70[i] its end.
 * InputRecorderRecordFrame records gUnk_03000F98[i] (the held keys), extending the
 * current entry while the keys stay the same; InputRecorderPlayFrame plays it back into
 * gUnk_03000F98[i] and the newly pressed keys gUnk_03001EB8[i], and an entry
 * of 0x3FF ends the playback for everyone.
 *
 * Matching notes: the plain `gUnk_0200EC60[i] = 0xFFFF;` is the ROM's
 * `ldr =0xFFFF; orrs` (a u16 element store is expanded as a bit-field store,
 * whose AND is dropped for an all-ones value and whose OR survives because cse
 * still holds the element from the `!=` test, lesson 3.469); the count is an
 * s8 local (3.340). */

struct LinkSave
{
    /*0x000*/ u32 unk00;
    /*0x004*/ u16 unk04;
    /*0x006*/ u16 unk06;
    /*0x008*/ u8 unk08;
    /*0x009*/ u8 unk09;
    /*0x00A*/ u8 unk0A;
    /*0x00B*/ u8 unk0B;
    /*0x00C*/ u16 unk0C;
    /*0x00E*/ u16 unk0E;
    /*0x010*/ u16 unk10;
    /*0x012*/ u8 unk12;
    /*0x013*/ u8 unk13;
    /*0x014*/ u16 unk14;
    /*0x016*/ u16 unk16[4];
    /*0x01E*/ u16 unk1E[4];
    /*0x026*/ u16 unk26[4];
    /*0x02E*/ u16 unk2E[4];
    /*0x036*/ u16 unk36;
    /*0x038*/ u8 unk38[8];
    /*0x040*/ u16 unk40[8][8];
    /*0x0C0*/ u16 unkC0;
    /*0x0C2*/ u16 unkC2;
    /*0x0C4*/ u8 unkC4[2];
    /*0x0C6*/ u8 unkC6[2];
    /*0x0C8*/ u8 unkC8[2];
    /*0x0CA*/ u8 unkCA[2];
    /*0x0CC*/ u32 unkCC[2];
    /*0x0D4*/ u8 unkD4[8][7];
    /*0x10C*/ u16 unk10C[4];
    /*0x114*/ u16 unk114[4];
    /*0x11C*/ u16 unk11C;
    /*0x11E*/ u8 unk11E;
    /*0x11F*/ u8 pad11F[0xD];
    /*0x12C*/ u16 unk12C[0x3B6A];
};

extern u8 gInputRecorderRunning;
extern u16 gUnk_0200EC60[];
extern u8 gUnk_0200EC68[];
extern struct LinkSave *gInputRecordingPtr;
extern u16 gUnk_0200EC70[];
extern u16 gUnk_0200EC78[];
extern u16 gUnk_02016480[];
extern vu16 gUnk_03000F98[];
extern vu16 gUnk_03001EB8[];
extern u16 gUnk_030023AC;

u32 sub_080b83a0(u8 *src, s32 i);

void InputRecorderRecordFrame(void)
{
    s32 i;
    u16 key;
    s8 count;
    u16 pos;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_0200EC78[i] >= gUnk_0200EC70[i])
        {
            gInputRecorderRunning = 0;
            return;
        }
        key = gUnk_03000F98[i] & 0x3FF;
        count = gUnk_0200EC68[i];
        pos = gUnk_02016480[i];
        if (gUnk_0200EC60[i] != key)
        {
            count = 1;
            gUnk_0200EC60[i] = key;
            gUnk_02016480[i] = gUnk_0200EC78[i];
            gUnk_0200EC78[i] += gUnk_030023AC;
            pos = gUnk_02016480[i];
        }
        else if (++count == 63)
        {
            gUnk_0200EC60[i] = 0xFFFF;
        }
        gUnk_0200EC68[i] = count;
        gInputRecordingPtr->unk12C[pos] = key | (count << 10);
        sub_080b83a0((u8 *)&gInputRecordingPtr->unk12C[pos], pos);
    }
}

void InputRecorderPlayFrame(void)
{
    s32 i;
    s32 j;
    u16 key;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
        if (gUnk_0200EC78[i] >= gUnk_0200EC70[i])
        {
            gInputRecorderRunning = 0;
            return;
        }
        if ((s8)--gUnk_0200EC68[i] <= 0)
        {
            gUnk_02016480[i] = gUnk_0200EC78[i];
            gUnk_0200EC78[i] += gUnk_030023AC;
            key = gInputRecordingPtr->unk12C[gUnk_02016480[i]] & 0x3FF;
            gUnk_0200EC68[i] = gInputRecordingPtr->unk12C[gUnk_02016480[i]] >> 10;
            if (key == 0x3FF)
            {
                for (j = 0; j < gUnk_030023AC; j++)
                    gUnk_0200EC78[j] = gUnk_0200EC70[j];
                return;
            }
            gUnk_03001EB8[i] = key & ~gUnk_0200EC60[i];
            gUnk_0200EC60[i] = key;
        }
        gUnk_03000F98[i] = gUnk_0200EC60[i];
    }
}
