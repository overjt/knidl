#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* save_b8694.c (0x080B8694-0x080B8887, issue #94).
 *
 * Link play: copy what each player sent this frame into its 96-byte record
 * gUnk_0200EA00[] (src/early_2b04.c calls it when a mailbox row reads
 * 0x66xx).  The low byte of row 0 of the SIO mailbox gUnk_03004D50 says what
 * rows 1-3 carry: 1 = the byte pairs unk06/unk08/unk0A/unk0C and the
 * halfword unk02 of slot gUnk_03002464, 2 = the word unk10 of that slot and
 * unk00, 3 and up = one third of a row of the 8x7 grid unk18 (row (k - 3) / 3,
 * columns 0-2, 3-5 or 6 by (k - 3) % 3). */

struct LinkRec
{
    /*0x00*/ u16 unk00;
    /*0x02*/ u16 unk02[2];
    /*0x06*/ u8 unk06[2];
    /*0x08*/ u8 unk08[2];
    /*0x0A*/ u8 unk0A[2];
    /*0x0C*/ u8 unk0C[2];
    /*0x0E*/ u16 pad0E;
    /*0x10*/ u32 unk10[2];
    /*0x18*/ u8 unk18[8][7];
    /*0x50*/ u8 filler50[0x10];
};

extern struct LinkRec gUnk_0200EA00[];
extern u8 gUnk_03002464;
extern u16 gUnk_030023AC;
extern u16 gUnk_03004D50[4][4];

void sub_080b8694(void)
{
    s32 i;
    s32 q;
    s32 r;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        switch (gUnk_03004D50[0][i] & 0xFF)
        {
        case 0:
            break;
        case 1:
            gUnk_0200EA00[i].unk06[gUnk_03002464] = gUnk_03004D50[1][i] >> 8;
            gUnk_0200EA00[i].unk08[gUnk_03002464] = gUnk_03004D50[1][i];
            gUnk_0200EA00[i].unk0A[gUnk_03002464] = gUnk_03004D50[2][i] >> 8;
            gUnk_0200EA00[i].unk0C[gUnk_03002464] = gUnk_03004D50[2][i];
            gUnk_0200EA00[i].unk02[gUnk_03002464] = gUnk_03004D50[3][i];
            break;
        case 2:
            gUnk_0200EA00[i].unk10[gUnk_03002464] = (gUnk_03004D50[1][i] << 16) | gUnk_03004D50[2][i];
            gUnk_0200EA00[i].unk00 = gUnk_03004D50[3][i];
            break;
        default:
            q = Div((gUnk_03004D50[0][i] & 0xFF) - 3, 3);
            r = Mod((gUnk_03004D50[0][i] & 0xFF) - 3, 3);
            if (q <= 7)
            {
                switch (r)
                {
                case 0:
                    gUnk_0200EA00[i].unk18[q][0] = gUnk_03004D50[1][i];
                    gUnk_0200EA00[i].unk18[q][1] = gUnk_03004D50[2][i];
                    gUnk_0200EA00[i].unk18[q][2] = gUnk_03004D50[3][i];
                    break;
                case 1:
                    gUnk_0200EA00[i].unk18[q][3] = gUnk_03004D50[1][i];
                    gUnk_0200EA00[i].unk18[q][4] = gUnk_03004D50[2][i];
                    gUnk_0200EA00[i].unk18[q][5] = gUnk_03004D50[3][i];
                    break;
                case 2:
                    gUnk_0200EA00[i].unk18[q][6] = gUnk_03004D50[1][i];
                    break;
                }
            }
            break;
        }
    }
}
