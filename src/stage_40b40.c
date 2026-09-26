#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_40b40.c (0x08040B40-0x080413A3, issue #85).
 *
 * The player's motion preset setter, called some 200 times from M09-M14's
 * action bodies: a0 picks one of 14 kinds of motion and a1 an entry of the
 * preset table gUnk_0873AFC4 (two halfword pairs).  Each kind writes the
 * 16.16 motion cells Task.unk54/unk5C (through sub_080061c0, which mirrors
 * them by the facing and leaves a component alone when passed 0x5A5A5A5A)
 * and Task.unk64/unk68 from signed 8.8 halfwords of gUnk_0873AFC4[a1] or of
 * the preset row gUnk_0873AF6C[gUnk_03001F30] (22 halfwords), some of them
 * chosen by PlayerState.unk49, the held keys gUnk_03002458[] or the
 * collision block gUnk_03005550; kind 12 picks one of five rows of
 * gUnk_0873AF58 by the speed.
 *
 * Matching note (issue #85): the key mask is read inline at every test (a
 * cached s32 mask let regmove AND in place and pushed case 13's task pointer
 * into ip, lessons 4.62/4.63 and 3.476). */

extern u8 gUnk_03001F30;
extern u8 gUnk_03005550[];
extern u16 gUnk_03002458[];   /* held keys, latched per player */
extern u16 gUnk_0873AF58[][2];
extern u32 gUnk_0873AF6C[];
extern u32 gUnk_0873AFC4[];

void sub_080061c0(s32 a, s32 b);
void sub_0803e050(s32 a0);

/* Byte-exact.  The key-mask table gUnk_03002458[...] is read inline at every
   test (cse merges the repeats): a halfword read stays an HImode pseudo used
   through a subreg, which regmove does not retarget, so every mask test is
   the ROM's non-destructive `movs r0, #K; ands r0, rM`.  The old `s32 m`
   caches made the value an SImode pseudo that dies at its last test;
   regmove then rewrote `(set T (and m K))` into `(set m (and m K))`
   (`ands r1, r0`), which raised the mask's refs from 3 to 5 and pushed case
   13's task pointer into `ip` (lessons 4.62/4.63; +8 bytes).  Case 13 needs
   no task local at all: cse carries the task pointer loaded by the first
   index into every arm, as in the ROM.  The `u16 b` in its second half is
   the ROM's `lsls #16; lsrs #16` zero-extension of `mask & 16`.  The
   per-block `struct Task *t = gUnk_03002490;` before a sign-extended 8.8 store IS
   load-bearing (the ROM loads the task pointer before the value; without
   it 1833 bytes differ). */

void sub_08040b40(s32 a0, s32 a1)
{
    u16 *q = (u16 *)(gUnk_0873AF6C + gUnk_03001F30 * 11);
    u16 *r = (u16 *)(gUnk_0873AFC4 + a1 * 2);

    switch (a0)
    {
    case 0:
        if (gUnk_03002490->unk88->unk49 == 0)
        {
            s32 v = q[2] << 8;
            if (q[2] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        else
        {
            s32 v = q[4] << 8;
            if (q[4] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        gUnk_03002490->unk64 = 0;
        break;
    case 1:
        if (gUnk_03002490->unk88->unk49 == 0)
        {
            s32 v = q[1] << 8;
            if (q[1] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        else
        {
            s32 v = q[3] << 8;
            if (q[3] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        {
            struct Task *t = gUnk_03002490;

            s32 v = q[0] << 8;
            if (q[0] & 0x8000)
                v |= 0xFF000000;
            t->unk64 = v;
        }
        break;
    case 2:
        if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) != 0)
        {
            if (gUnk_03002490->unk88->unk49 == 0)
            {
                s32 v = q[1] << 8;
                if (q[1] & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(0x5A5A5A5A, v);
            }
            else
            {
                s32 v = q[3] << 8;
                if (q[3] & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(0x5A5A5A5A, v);
            }
            {
                struct Task *t = gUnk_03002490;

                s32 v = q[0] << 8;
                if (q[0] & 0x8000)
                    v |= 0xFF000000;
                t->unk64 = v;
            }
        }
        else
        {
            sub_08040b40(0, 72);
        }
        break;
    case 3:
        if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) != 0)
        {
            if (gUnk_03002490->unk88->unk49 == 0)
            {
                s32 v = q[6] << 8;
                if (q[6] & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(0x5A5A5A5A, v);
            }
            else
            {
                s32 v = q[3] << 8;
                if (q[3] & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(0x5A5A5A5A, v);
            }
            {
                struct Task *t = gUnk_03002490;

                s32 v = q[5] << 8;
                if (q[5] & 0x8000)
                    v |= 0xFF000000;
                t->unk64 = v;
            }
        }
        else
        {
            sub_08040b40(0, 72);
        }
        break;
    case 4:
        if (gUnk_03002490->unk88->unk49 == 0)
        {
            s32 v = q[10] << 8;
            if (q[10] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        else
        {
            s32 v = q[11] << 8;
            if (q[11] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        gUnk_03002490->unk64 = 0;
        break;
    case 5:
        if (gUnk_03002490->unk88->unk49 == 0)
            sub_080061c0(0x20000, 0x1000);
        else
            sub_080061c0(0x20000, 0x800);
        gUnk_03002490->unk64 = 0;
        break;
    case 6:
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
        {
            s32 v = q[17] << 8;
            if (q[17] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
            gUnk_03002490->unk64 = 0x10000;
        }
        else
        {
            s32 v = q[18] << 8;
            if (q[18] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
            gUnk_03002490->unk64 = 0;
        }
        break;
    case 7:
        if ((gUnk_03002458[gUnk_03002490->unk88->unk00] & 48) != 0)
        {
            if ((gUnk_03002490->unk7B & 1) == 0)
            {
                s32 v = q[14] << 8;
                if (q[14] & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(0x5A5A5A5A, v);
                {
                    struct Task *t2 = gUnk_03002490;

                    s32 v = q[16] << 8;
                    if (q[16] & 0x8000)
                        v |= 0xFF000000;
                    t2->unk64 = v;
                }
            }
            else
            {
                sub_080061c0(0x5A5A5A5A, 0x800);
                gUnk_03002490->unk64 = 0x10C00;
            }
            break;
        }
        if ((gUnk_03002490->unk7B & 1) == 0)
        {
            s32 v = q[15] << 8;
            if (q[15] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        else
        {
            sub_080061c0(0x5A5A5A5A, 0x900);
        }
        gUnk_03002490->unk64 = 0;
        break;
    case 8:
        {
            s32 v = q[20] << 8;
            if (q[20] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        gUnk_03002490->unk64 = 0;
        break;
    case 9:
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
        {
            {
                s32 v = q[19] << 8;
                if (q[19] & 0x8000)
                    v |= 0xFF000000;
                sub_080061c0(0x5A5A5A5A, v);
            }
            {
                struct Task *t = gUnk_03002490;

                s32 v = q[21] << 8;
                if (q[21] & 0x8000)
                    v |= 0xFF000000;
                t->unk64 = v;
            }
        }
        else
        {
            s32 v = q[20] << 8;
            if (q[20] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
            gUnk_03002490->unk64 = 0;
        }
        break;
    case 10:
        if (r[0] != 0x9999)
        {
            struct Task *t = gUnk_03002490;

            s32 v = r[0] << 8;
            if (r[0] & 0x8000)
                v |= 0xFF000000;
            t->unk54 = v;
        }
        if (r[1] != 0x9999)
        {
            struct Task *t = gUnk_03002490;

            s32 v = r[1] << 8;
            if (r[1] & 0x8000)
                v |= 0xFF000000;
            t->unk5C = v;
        }
        if (r[2] != 0x9999)
        {
            struct Task *t = gUnk_03002490;

            s32 v = r[2] << 8;
            if (r[2] & 0x8000)
                v |= 0xFF000000;
            t->unk64 = v;
        }
        break;
    case 11:
        if (r[0] != 0x9999)
        {
            s32 v = r[0] << 8;
            if (r[0] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(v, 0x5A5A5A5A);
        }
        if (r[1] != 0x9999)
        {
            s32 v = r[1] << 8;
            if (r[1] & 0x8000)
                v |= 0xFF000000;
            sub_080061c0(0x5A5A5A5A, v);
        }
        if (r[2] != 0x9999)
        {
            struct Task *t = gUnk_03002490;

            s32 v = r[2] << 8;
            if (r[2] & 0x8000)
                v |= 0xFF000000;
            t->unk64 = v;
        }
        break;
    case 12:
        {
            s32 n;

            if (a1 == 1)
            {
                if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
                {
                    switch (gUnk_03002490->unk88->unk4B)
                    {
                    case 0:
                    default:
                        n = 0;
                        break;
                    case 1:
                        n = (gUnk_03002490->unk43 == 1) ? 1 : 2;
                        break;
                    case 2:
                        n = (gUnk_03002490->unk43 == 1) ? 2 : 1;
                        break;
                    case 3:
                        n = (gUnk_03002490->unk43 == 1) ? 3 : 4;
                        break;
                    case 4:
                        n = (gUnk_03002490->unk43 == 1) ? 4 : 3;
                        break;
                    }
                    {
                        struct Task *t = gUnk_03002490;
                        s32 d;

                        s32 v = gUnk_0873AF58[n][1] << 8;
                        if (gUnk_0873AF58[n][1] & 0x8000)
                            v |= 0xFF000000;
                        t->unk64 = v;
                        d = t->unk54;
                        if (d < 0)
                            d = -d;
                        if (d <= v)
                        {
                            s32 v = gUnk_0873AF58[n][0] << 8;
                            if (gUnk_0873AF58[n][0] & 0x8000)
                                v |= 0xFF000000;
                            sub_080061c0(0x5A5A5A5A, v);
                        }
                        else
                        {
                            sub_080061c0(0x5A5A5A5A, 0xE00);
                        }
                    }
                }
                else
                {
                    sub_080061c0(0x5A5A5A5A, 0xE00);
                    gUnk_03002490->unk64 = 0;
                }
            }
            else
            {
                if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
                {
                    sub_080061c0(0x5A5A5A5A, 0xC00);
                    gUnk_03002490->unk64 = 0x20000;
                }
                else
                {
                    sub_080061c0(0x5A5A5A5A, 0x400);
                    gUnk_03002490->unk64 = 0;
                }
            }
        }
        break;
    case 13:
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
        {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 16)
            {
                if ((gUnk_03002490->unk7B & 1) == 0)
                {
                    gUnk_03002490->unk5C = 0x8000;
                    gUnk_03002490->unk64 = 0x20000;
                }
                else
                {
                    gUnk_03002490->unk5C = 0x4000;
                    gUnk_03002490->unk64 = 0x10000;
                }
                if (gUnk_03005550[0] == 2)
                    gUnk_03005550[0] = 0;
            }
            else
            {
                if ((gUnk_03002490->unk7B & 1) == 0)
                {
                    gUnk_03002490->unk5C = 0xFFFF8000;
                    gUnk_03002490->unk64 = 0x20000;
                }
                else
                {
                    gUnk_03002490->unk5C = 0xFFFFC000;
                    gUnk_03002490->unk64 = 0x10000;
                }
                if (gUnk_03005550[0] == 1)
                    gUnk_03005550[0] = 0;
            }
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 192)
            {
                if ((gUnk_03002490->unk7B & 1) == 0)
                    gUnk_03002490->unk64 = 0x18000;
                else
                    gUnk_03002490->unk64 = 0xC000;
            }
        }
        else
        {
            gUnk_03002490->unk64 = 0;
            if (gUnk_03002490->unk54 == 0)
                sub_0803e050(1);
        }
        gUnk_03002490->unk7A = 0;
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 192)
        {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 64)
            {
                if ((gUnk_03002490->unk7B & 1) == 0)
                {
                    gUnk_03002490->unk60 = 0xFFFF8000;
                    gUnk_03002490->unk68 = 0x20000;
                }
                else
                {
                    gUnk_03002490->unk60 = 0xFFFFC000;
                    gUnk_03002490->unk68 = 0x10000;
                }
            }
            else
            {
                if ((gUnk_03002490->unk7B & 1) == 0)
                {
                    gUnk_03002490->unk60 = 0x8000;
                    gUnk_03002490->unk68 = 0x20000;
                }
                else
                {
                    gUnk_03002490->unk60 = 0x4000;
                    gUnk_03002490->unk68 = 0x10000;
                }
            }
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 48)
            {
                u16 b = gUnk_03002458[gUnk_03002490->unk88->unk00] & 16;

                if (b != 0)
                {
                    if (gUnk_03005550[0] == 2)
                        gUnk_03005550[0] = 0;
                }
                else
                {
                    if (gUnk_03005550[0] == 1)
                        gUnk_03005550[0] = 0;
                }
                if ((gUnk_03002490->unk7B & 1) == 0)
                    gUnk_03002490->unk68 = 0x18000;
                else
                    gUnk_03002490->unk68 = 0xC000;
            }
        }
        else
        {
            gUnk_03002490->unk68 = 0;
            if (gUnk_03002490->unk58 == 0)
                sub_0803e050(2);
        }
        break;
    }
}
