#include "gba/gba.h"
#include "global.h"

/* early_4984.c (0x08004984-0x08004D6B, issue #63).
 *
 * MultiBootMain of the AGB SDK multiboot library (src/early_4734.c and
 * src/early_4d6c.c hold the rest of it: MultiBootInit, MultiBootSend,
 * MultiBootStartProbe, MultiBootStartMaster, MultiBootCheckComplete, the
 * handshake and the wait helpers).  The link-play code in src/early_3964.c
 * calls it with the MultiBootParam gMultiBootParam while a boot image goes to
 * the clients; each call is one step: it probes the clients through the
 * SIOMULTI registers, exchanges the header and palette bytes, starts the BIOS
 * MultiBoot transfer and runs the final handshake; it returns 0 or one of the
 * SDK error codes.
 *
 * This is an older revision of the source pokeemerald ships as
 * src/multiboot.c: the `probe_count >= 0xE0` branch has no server_type test, a
 * client-info mismatch in case 0 also clears response_bit, and case 0's first
 * loop tests SIOMULTI3 once in front of the loop.  masterp is a plain `u8 *`
 * (a volatile pointee changes the header bytes' registers), and SIOMULTI is
 * read through the io_reg.h constant REG_SIOMULTI(i) (issue #32 had used the
 * symbol gUnk_04000120, with which gcse keeps the base in a callee-saved
 * register); lesson 3.481. */

/* AGB SDK MultiBootParam (0x4C bytes); the live instance is gMultiBootParam. */
struct MultiBootParam
{
    /*0x00*/ u32 system_work[5];
    /*0x14*/ u8 handshake_data;
    /*0x15*/ u8 padding;
    /*0x16*/ u16 handshake_timeout;
    /*0x18*/ u8 probe_count;
    /*0x19*/ u8 client_data[3];
    /*0x1C*/ u8 palette_data;
    /*0x1D*/ u8 response_bit;
    /*0x1E*/ u8 client_bit;
    /*0x1F*/ u8 reserved1;
    /*0x20*/ u8 *boot_srcp;
    /*0x24*/ u8 *boot_endp;
    /*0x28*/ u8 *masterp;
    /*0x2C*/ u8 *reserved2[3];
    /*0x38*/ u32 system_work2[4];
    /*0x48*/ u8 sendflag;
    /*0x49*/ u8 probe_target_bit;
    /*0x4A*/ u8 check_wait;
    /*0x4B*/ u8 server_type;
};

extern u16 gUnk_03006920[];

void MultiBootInit(struct MultiBootParam *mp);
int MultiBootSend(struct MultiBootParam *mp, u16 data);
void MultiBootStartProbe(struct MultiBootParam *mp);
int MultiBootCheckComplete(struct MultiBootParam *mp);
int MultiBootHandShake(struct MultiBootParam *mp);
void MultiBootWaitSendDone(void);

/* MultiBootMain (AGB SDK): one step of the master's multiboot state machine,
 * run once per frame while a boot image is sent to the clients. */
u32 MultiBootMain(struct MultiBootParam *mp)
{
    int i;
    int j;
    int k;

    if (MultiBootCheckComplete(mp))
        return 0;

    if (mp->check_wait > 15)
    {
        mp->check_wait--;
        return 0;
    }

output_burst:
    if (mp->sendflag)
    {
        mp->sendflag = 0;

        i = REG_SIOCNT & 0xFC;
        if (i != 8)
        {
            MultiBootInit(mp);
            return i ^ 8;
        }
    }

    if (mp->probe_count >= 0xE0)
    {
        i = MultiBootHandShake(mp);
        if (i)
            return i;

        if (mp->probe_count > 0xE1
         && MultiBootCheckComplete(mp) == 0)
        {
            MultiBootWaitSendDone();
            goto output_burst;
        }

        if (MultiBootCheckComplete(mp) == 0)
        {
            if (mp->handshake_timeout == 0)
            {
                MultiBootInit(mp);
                return 0x71;
            }
            mp->handshake_timeout--;
        }

        return 0;
    }

    switch (mp->probe_count)
    {
    case 0:
        k = 0x0E;
        /* This revision's loop tests SIOMULTI3 once in front of the loop:
         * the plain `for (i = 3; i != 0; i--)` gives the same code except
         * that the loop's pointer starts from the test's base register
         * instead of its own pool word 0x04000126. */
        i = 3;
        if (REG_SIOMULTI(i) == 0xFFFF)
        {
            do
            {
                k >>= 1;
                if (--i == 0)
                    break;
            } while (REG_SIOMULTI(i) == 0xFFFF);
        }

        k &= 0x0E;
        mp->response_bit = k;

        for (i = 3; i != 0; i--)
        {
            j = REG_SIOMULTI(i);
            if (mp->client_bit & (1 << i))
            {
                if (j != ((0x72 << 8) | (1 << i)))
                {
                    /* not in pokeemerald's revision */
                    mp->response_bit = k = 0;
                    break;
                }
            }
        }

        mp->client_bit &= k;

        if (k == 0)
            mp->check_wait = 15;

        if (mp->check_wait)
        {
            mp->check_wait--;
        }
        else
        {
            if (mp->response_bit != mp->client_bit)
            {
                MultiBootStartProbe(mp);
                goto case_1;
            }
        }

    output_master_info:
        return MultiBootSend(mp, (0x62 << 8) | mp->client_bit);

    case_1:
    case 1:
        mp->probe_target_bit = 0;
        for (i = 3; i != 0; i--)
        {
            j = REG_SIOMULTI(i);
            if ((j >> 8) == 0x72)
            {
                gUnk_03006920[i - 1] = j;
                j &= 0xFF;
                if (j == (1 << i))
                    mp->probe_target_bit |= j;
            }
        }

        if (mp->response_bit != mp->probe_target_bit)
            goto output_master_info;

        mp->probe_count = 2;
        return MultiBootSend(mp, (0x61 << 8) | mp->probe_target_bit);

    case 2:
        for (i = 3; i != 0; i--)
        {
            if (mp->probe_target_bit & (1 << i))
            {
                j = REG_SIOMULTI(i);
                if (j != gUnk_03006920[i - 1])
                    mp->probe_target_bit ^= 1 << i;
            }
        }
        goto output_header;

    case 0xD0:
        k = 1;
        for (i = 3; i != 0; i--)
        {
            j = REG_SIOMULTI(i);
            mp->client_data[i - 1] = j;
            if (mp->probe_target_bit & (1 << i))
            {
                if ((j >> 8) != 0x72
                 && (j >> 8) != 0x73)
                {
                    MultiBootInit(mp);
                    return 0x60;
                }
                if (j == gUnk_03006920[i - 1])
                    k = 0;
            }
        }

        if (k == 0)
            return MultiBootSend(mp, (0x63 << 8) | mp->palette_data);

        mp->probe_count = 0xD1;

        k = 0x11;
        for (i = 3; i != 0; i--)
            k += mp->client_data[i - 1];
        mp->handshake_data = k;
        return MultiBootSend(mp, (0x64 << 8) | (k & 0xFF));

    case 0xD1:
        for (i = 3; i != 0; i--)
        {
            j = REG_SIOMULTI(i);
            if (mp->probe_target_bit & (1 << i))
            {
                if ((j >> 8) != 0x73)
                {
                    MultiBootInit(mp);
                    return 0x60;
                }
            }
        }

        i = MultiBoot(mp);

        if (i == 0)
        {
            mp->probe_count = 0xE0;
            mp->handshake_timeout = 400;
            return 0;
        }
        MultiBootInit(mp);
        mp->check_wait = 30;
        return 0x70;

    default:
        for (i = 3; i != 0; i--)
        {
            if (mp->probe_target_bit & (1 << i))
            {
                j = REG_SIOMULTI(i);
                if ((j >> 8) != (0x61 + 1 - (mp->probe_count >> 1))
                 || ((j & 0xFF) != (1 << i)))
                    mp->probe_target_bit ^= 1 << i;
            }
        }

        if (mp->probe_count == 0xC4)
        {
            mp->client_bit = mp->probe_target_bit & 0x0E;
            mp->probe_count = 0;
            goto output_master_info;
        }

    output_header:
        if (mp->probe_target_bit == 0)
        {
            MultiBootInit(mp);
            return 0x50;
        }

        mp->probe_count += 2;
        if (mp->probe_count == 0xC4)
            goto output_master_info;
        i = MultiBootSend(mp,
            (mp->masterp[mp->probe_count - 4 + 1] << 8)
            | mp->masterp[mp->probe_count - 4]);

        if (i)
            return i;
        if (mp->server_type == 1)
        {
            MultiBootWaitSendDone();
            goto output_burst;
        }
        return 0;
    }
}
