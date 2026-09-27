#include "gba/gba.h"
#include "global.h"
#include "main.h"

/* early_6e9c.c (0x08006E9C-0x08007003, issue #63).
 *
 * The receive step of the SIO multi-play link driver (pokeruby's DoRecv, see
 * src/early_6ac8.c), called from the serial interrupt SerialCB in the
 * connected state: it snapshots the four SIOMULTI words into gUnk_03004D38.
 * On the checksum round (send index unk18 == 0) it compares every player's
 * word with the running checksum chk (a mismatch sets unk13), clears chk and
 * records REG_VCOUNT in gLinkRecvVCount; otherwise it adds the words to chk,
 * ORs them into gRecvNonzeroCheck and stores them into the receive ring at the
 * current command index unk19 (a full ring sets unk14 = 2), and after the
 * fourth command queues the frame if any word was non-zero.  While
 * gLinkCommand is in the 0x88xx range a word above 4 in command 3 sets
 * gLinkPauseFrames = 6 and clears gLinkDriverMode.
 *
 * Matching note (issue #63): this revision walks the staging buffer with a
 * pointer, `p = gUnk_03004D38;` after the copy and `*p++` in each loop; the
 * ROM steps the register that holds the buffer's address (lesson 3.483). */

/* The link work area gLink (0x4D2 bytes; layout as in
 * src/early_6d18.c).  It is the SIO multi-play library pokeruby ships as
 * src/link.c (struct Link there), in an earlier revision: four command
 * words per frame and 30-entry queues. */
struct Link {
    /*0x000*/ u8 unk00, unk01, unk02, count;
    /*0x004*/ u16 recv[4];
    /*0x00C*/ u8 unk0C, unk0D, unk0E, unk0F;
    /*0x010*/ u8 unk10, unk11, unk12, unk13, unk14, unk15;
    /*0x016*/ u16 chk;
    /*0x018*/ u8 unk18, unk19, unk1A, unk1B;
    /*0x01C*/ u16 ring[4][30];
    /*0x10C*/ u8 unk10C, unk10D, unk10E, unk10F;
    /*0x110*/ u16 buf[4][4][30];
    /*0x4D0*/ u8 unk4D0, unk4D1;
};

struct Pair { u32 a, b; };

/* Not from link.h: this file's view of gLink differs (lesson 3.517). */
extern struct Link gLink;
extern vu16 gUnk_03004D38[];    /* receive staging, 4 halfwords */
extern vu16 gUnk_04000120;      /* REG_SIOMULTI0 */
extern u32 gChecksumAvailable;
extern u32 gLinkRecvVCount;
extern u16 gRecvNonzeroCheck;

/* Receive step of the serial interrupt (pokeruby's DoRecv): snapshot the
 * four SIOMULTI words, then either check the round's checksum or queue the
 * words into the receive ring.  This older revision walks the staging
 * buffer with a pointer (`*p++`): the ROM's loops step the register that
 * holds &gUnk_03004D38 itself, where `gUnk_03004D38[i]` makes a strength-
 * reduced copy of it.  REG_VCOUNT is the io_reg.h macro (a symbol would be
 * hoisted into a callee-saved register by gcse, lesson 3.482). */
void DoRecv(void)
{
    u32 i;
    u32 index;
    vu16 *p;

    *(struct Pair *)gUnk_03004D38 = *(struct Pair *)&gUnk_04000120;
    p = gUnk_03004D38;

    if (gLink.unk18 == 0)
    {
        for (i = 0; i < gLink.count; i++)
            if (gLink.chk != *p++ && gChecksumAvailable)
                gLink.unk13 = 1;
        gLink.chk = 0;
        gChecksumAvailable = 1;
        gLinkRecvVCount = REG_VCOUNT;
    }
    else
    {
        index = gLink.unk4D0 + gLink.unk4D1;
        if (index >= 30)
            index -= 30;
        if (gLink.unk4D1 < 30)
        {
            for (i = 0; i < gLink.count; i++)
            {
                gLink.chk += *p;
                if ((gLinkCommand & 0xFF00) == 0x8800 && gLink.unk19 == 3
                 && *p > 4)
                {
                    gLinkPauseFrames = 6;
                    gLinkDriverMode = 0;
                }
                gRecvNonzeroCheck |= *p;
                gLink.buf[i][gLink.unk19][index] = *p++;
            }
        }
        else
        {
            gLink.unk14 = 2;
        }
        gLink.unk19++;
        if (gLink.unk19 == 4 && gRecvNonzeroCheck)
        {
            gLink.unk4D1++;
            gRecvNonzeroCheck = 0;
        }
    }
}
