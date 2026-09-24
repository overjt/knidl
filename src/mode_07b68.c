#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_07b68.c (0x08007B68-0x080082CF, issue #96).
 *
 * AgbMain state 13, the title screen of the six extra modes
 * (sub_08007f9c): the mode's picture, two task-#265 decorations and a
 * level select for the first three modes.  In single-pak link play it
 * first stages a multiboot image at 0x02020000 (sub_08007e04, a common
 * blob plus one of three per-mode chunk sets), sends it and runs the
 * 0x5503 SIO handshake (sub_08007b68/sub_08007c5c); sub_08007d4c is the
 * failure prompt that returns to state 4. */

extern s8 gUnk_02000004;
extern u32 gUnk_02004000[];
extern u8 gUnk_02006090;
extern s8 gUnk_02006168;
extern s32 gUnk_02007D00;
extern u8 gUnk_02007FCC;
extern u32 gUnk_0200EBA0;
extern u8 gUnk_0200EBB0;
extern vu8 gUnk_0200EBC0[];
extern vu32 gUnk_0200EC3C;
extern u32 gUnk_0200EC44;
extern u32 gUnk_0200EC48;
extern u32 gUnk_02020000[];
extern vs32 gUnk_03000010;
extern vu16 gUnk_03000038;
extern vu16 gUnk_03000048;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_0300117C;
extern vu16 gUnk_03001E90;
extern vu16 gUnk_03001EB8[];
extern vu16 gUnk_03001ED8;
extern u16 gUnk_03001F18[];
extern u16 gUnk_03001F38;
extern u16 gUnk_03002150;
extern u16 gUnk_03002378[];
extern u16 gUnk_030023AC;
extern u16 gUnk_030023D8;
extern u16 gUnk_0300243C;
extern u16 gUnk_03004D50[4][4];
extern u16 gUnk_03004D88[];
extern u16 gUnk_03004D90[4];
extern s32 gUnk_03005280;
extern u16 gUnk_085B113C[];
extern u16 gUnk_085B119C[];
extern u16 gUnk_085B274C[];
extern u16 gUnk_085B2D0C[];
extern u16 gUnk_085B2D8C[];
extern u16 gUnk_085B450C[];
extern u16 gUnk_085B4ACC[];
extern u16 gUnk_085B4B4C[];
extern u16 gUnk_085B64C8[];
extern u8 gUnk_0876B1FC[];
extern u8 gUnk_0876F690[];
extern u8 gUnk_087954C0[];
extern u8 gUnk_087C0A4C[];
extern u8 gUnk_087E1D58[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022d0(void);
void sub_080022e4(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002d18(void);
void sub_08002d54(s32 count);
void sub_08002d74(s32 count);
void sub_08002d94(s32 count);
void sub_08002db4(void);
void sub_08002de0(void);
void sub_08002e0c(void);
s32 sub_08003110(s32 songId);
void sub_080031b8(s32 id);
void sub_08003864(void);
void sub_08003a00(u8 *start, u8 *end);
void sub_08003a98(void);
void sub_08004000(u16 a);
u32 sub_08004308(void);
void sub_08004390(u32 *src, u32 *dst, u32 size);
u32 sub_08004400(void);
void sub_080044b8(void);
u32 sub_08004714(void);
void sub_08006868(void);
void sub_08006914(u8 *cmd, u16 *send, u16 *recv);
u32 sub_080071dc(void);
u32 sub_080072e0(void);
void sub_080082d0(void);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);
void sub_08008e1c(s32 a0);
void sub_080c6ab4(u16 *a, s32 b, s32 c);

s32 sub_08007b68(u32 *src, u32 *dst, u32 size)
{
    s32 i;
    s32 n;
    s32 m;

    sub_08004390(src, dst, size);
    do {
        sub_080022e4();
        sub_08006914((u8 *)gUnk_03004D88, gUnk_03004D90, gUnk_03004D50[0]);
        if (sub_080072e0() != 0)
            goto fail;
    } while (sub_08004400() == 0);
    sub_08002d54(2);
    sub_080044b8();
    while (sub_08004714() == 0)
        sub_080022e4();
    if (sub_080071dc() != 0)
        goto fail;
    gUnk_03004D90[0] = 0x5503;
    gUnk_03004D90[1] = 0;
    if (gUnk_0200EC44 == gUnk_0200EC3C)
        gUnk_03004D90[1] = 1;
    m = 0;
    n = 0;
    do {
        for (i = 0; i < 4; i++) {
            if (gUnk_03004D50[0][i] == 0x5503) {
                n++;
                if (gUnk_03004D50[1][i] != 0)
                    m++;
            }
        }
        if (n == gUnk_0300243C)
            goto done;
        sub_080022e4();
        sub_08006914((u8 *)gUnk_03004D88, gUnk_03004D90, gUnk_03004D50[0]);
    } while (sub_080072e0() == 0);
fail:
    return 1;
done:
    if (n != m)
        goto fail;
    return 0;
}

s32 sub_08007c5c(void)
{
    u8 *src;
    u32 size;
    s32 ret;

    if (sub_080071dc() != 0)
        return 1;
    if (gUnk_03001F38 != 0) {
        gUnk_0200EBB0 = 0;
        gUnk_03004D90[0] = 0xAA00;
        gUnk_03004D90[1] = gUnk_02006090;
    }
    do {
        sub_080022e4();
        sub_08006914((u8 *)gUnk_03004D88, gUnk_03004D90, gUnk_03004D50[0]);
        if (sub_080072e0() != 0)
            return 1;
    } while (sub_08004308() == 0);
    switch (gUnk_02006090) {
    case 0:
        src = gUnk_0876F690;
        size = gUnk_087954C0 - gUnk_0876F690;
        break;
    case 1:
        src = gUnk_087954C0;
        size = gUnk_087C0A4C - gUnk_087954C0;
        break;
    case 2:
        src = gUnk_087C0A4C;
        size = gUnk_087E1D58 - gUnk_087C0A4C;
        break;
    }
    ret = sub_08007b68((u32 *)src, gUnk_02004000, size);
    if (ret != 0)
        return 1;
    gUnk_03005280 = 2;
    gUnk_03001ED8 &= 0xFEFF;
    gUnk_0300117C = ret;
    sub_080022fc();
    sub_08002d94(64);
    return 0;
}

void sub_08007d4c(void)
{
    sub_080021dc();
    sub_08002de0();
    sub_08006868();
    sub_08008c4c(3);
    sub_08008c64(64);
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x800;
    sub_0800214c();
    sub_08002d74(32);
    gUnk_03000048 = 0;
    while (1) {
        if (gUnk_03000038 & 9) {
            sub_080031b8(102);
            break;
        }
        if (gUnk_03000038 & 2) {
            sub_080031b8(215);
            break;
        }
        sub_080022d0();
    }
    sub_080021dc();
    sub_08002de0();
    gUnk_03002150 = gUnk_02007FCC + 14;
    gUnk_030023D8 = 4;
}

/* Stage the link-play payload at 0x02020000: the common blob
   0x0876B1FC-0x0876F690 plus three chunks picked by gUnk_02006090.  The table
   at 0x020200C8 (inside the copied blob) gets each chunk's address as seen
   once the payload runs from 0x02000000, hence the - 0x20000.  The addresses
   are written as literals: the ROM rebuilds each one as size + constant. */
void sub_08007e04(void)
{
    u32 *tbl = (u32 *)0x020200C8;
    u8 *dst = (u8 *)gUnk_02020000;
    u32 size;

    size = gUnk_0876F690 - gUnk_0876B1FC;
    CpuSet(gUnk_0876B1FC, dst, ((size + 16) / 2) & 0x1FFFFF);
    dst = (u8 *)(size + 0x02020010);
    switch (gUnk_02006090) {
    case 0:
        CpuSet(gUnk_085B4ACC, dst, 0x40);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02020090);
        CpuSet(gUnk_085B4B4C, dst, 0xCBE);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021A0C);
        CpuSet(gUnk_085B64C8, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021FCC);
        break;
    case 1:
        CpuSet(gUnk_085B113C, dst, 0x30);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02020070);
        CpuSet(gUnk_085B119C, dst, 0xAD8);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021620);
        CpuSet(gUnk_085B274C, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021BE0);
        break;
    case 2:
        CpuSet(gUnk_085B2D0C, dst, 0x40);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02020090);
        CpuSet(gUnk_085B2D8C, dst, 0xBC0);
        *tbl++ = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021810);
        CpuSet(gUnk_085B450C, dst, 0x2E0);
        *tbl = (u32)dst - 0x20000;
        dst = (u8 *)(size + 0x02021DD0);
        break;
    }
    sub_08003a00((u8 *)gUnk_02020000, dst);
}

void sub_08007f9c(void)
{
    s32 i;
    s32 n;
    u8 k;

    sub_080022ac();
    sub_08008c4c(4);
    sub_08008e1c(gUnk_02006090);
    sub_08008c64(63);
    gUnk_0300117C = gUnk_03000010 = 0;
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1800;
    if (gUnk_02006090 == 6 || gUnk_02006090 == 7) {
        sub_08008c64(4);
        sub_080017e4(6, 0, 0x06001000, 0x800);
        /* Two complete copies: jump2 cross-jumps the identical tails, which
           is what leaves the ROM's `ldr r0, =F18; b join` arm.  A pointer
           local (if/else or ?:) is folded into "p = b; if (c) p = a". */
        if (gUnk_02006090 == 6) {
            if (gUnk_03001F18[0] != 0 || gUnk_03001F18[1] != 0
                || gUnk_03001F18[2] != 0 || gUnk_03001F18[3] != 0) {
                sub_080c6ab4(gUnk_03001F18, 22, 18);
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1900;
            }
        } else {
            if (gUnk_03002378[0] != 0 || gUnk_03002378[1] != 0
                || gUnk_03002378[2] != 0 || gUnk_03002378[3] != 0) {
                sub_080c6ab4(gUnk_03002378, 22, 18);
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1900;
            }
        }
    }
    sub_080082d0();
    gUnk_02007D00 = 0;
    if (gUnk_0200EC48 == 2) {
        gUnk_0300243C = 999;
        sub_08007e04();
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1900;
        gUnk_03005280 = 0;
        sub_0800214c();
        while (gUnk_03001E90 != 0) {
            sub_080022e4();
            sub_08004000(gUnk_02000004);
        }
        gUnk_03000048 = 0;
        sub_08003a98();
        do {
            sub_080022e4();
            sub_08004000(gUnk_02000004);
        } while (gUnk_0200EBC0[2] != 3 && gUnk_0200EBC0[44] == 0);
        if (gUnk_0200EBC0[3] != 0) {
            sub_08007d4c();
            sub_08003864();
            return;
        }
        sub_08003864();
        gUnk_03005280 = 1;
        sub_08003110(40);
        /* `cancel` sits here in the ROM, between this arm and the else arm */
        if (sub_08007c5c() == 0)
            goto select;
        sub_08007d4c();
        return;
    cancel:
        sub_080031b8(215);
        sub_080021dc();
        sub_08002db4();
        gUnk_030023D8 = 4;
        gUnk_03002150 = gUnk_02007FCC + 14;
        return;
    } else {
        sub_08002358();
        sub_08002378();
        sub_080022fc();
        gUnk_03005280 = 3;
        sub_0800214c();
        sub_08002e0c();
        sub_08002d94(16);
    }
select:
    gUnk_03005280 = 3;
    while (1) {
        sub_08002d18();
        if (gUnk_03002150 == 4) {
            if (gUnk_02006090 <= 2) {
                if ((gUnk_03001EB8[0] & 0x20) && gUnk_02006168 != 0) {
                    sub_080031b8(101);
                    gUnk_02006168--;
                } else if ((gUnk_03001EB8[0] & 0x10) && gUnk_02006168 != 2) {
                    sub_080031b8(101);
                    gUnk_02006168++;
                }
            }
            if (gUnk_030023AC == 1 && (gUnk_03001EB8[0] & 2))
                goto cancel;
            if (gUnk_03001EB8[0] & 9) {
                sub_080031b8(102);
                break;
            }
        } else {
            n = 0;
            for (i = 0; i < gUnk_030023AC; i++) {
                if (gUnk_03001EB8[i] & 9)
                    n++;
            }
            if (n != 0)
                break;
        }
    }
    if (gUnk_0200EC48 == 2)
        gUnk_0200EBA0 = 0;
    gUnk_03005280 = 4;
    sub_08002338();
    /* store address first, then the one read of gUnk_02007FCC, kept in k */
    gUnk_030023D8 = (k = gUnk_02007FCC) + 14;
    if (gUnk_03002150 == 4 && k <= 2)
        sub_08002d94(32);
    sub_080021dc();
    sub_08002e0c();
}
