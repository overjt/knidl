#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* boot_091ac.c (0x080091AC-0x080099FB, issue #96).
 *
 * AgbMain states 1 and 3 and the class-0 task types #0-#2. */

extern vs32 gUnk_03000010;
extern vu16 gUnk_03000038;
extern vu8 gUnk_03000040;
extern vu16 gUnk_03000044;
extern vu16 gUnk_03000048;
extern vu16 gUnk_030004A4;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03000B18;
extern vs32 gUnk_03000B78;
extern vu8 gUnk_03000F7C;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern vs16 gUnk_03000FB8;
extern vs32 gUnk_03000FC0;
extern vu16 gUnk_03000FD4;
extern vu16 gUnk_03001174;
extern vs32 gUnk_0300117C;
extern vu8 gUnk_0300118C;
extern u16 gUnk_03001430[];
extern vu16 gUnk_03001E90;
extern vs32 gUnk_03001E94;
extern vu16 gUnk_03001E98;
extern vu8 gUnk_03001EAC;
extern u16 *gUnk_03001ED4;
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;
extern s32 gUnk_03001F2C;
extern u16 gUnk_03002150;
extern u16 gUnk_0300243C;
extern u16 gUnk_08541D98[][16];
extern u16 gUnk_08541F58[];
extern u16 gUnk_08731C88[];
extern u16 gUnk_08731CC8[];
extern u8 gUnk_08731CDC[];
extern u16 gUnk_08731CE6[];
extern u32 gUnk_087555B4[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 sub_080008e8(u16 steps, u16 delta, u16 *mask);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_080020b8(void);
void sub_0800214c(void);
void sub_08002198(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022e4(void);
void sub_08002d54(s32 count);
void sub_08002db4(void);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);
void sub_08003688(void);
void sub_08003770(u16 volume);
s32 sub_080058e4(u32 type, s32 idx);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08006138(void);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);
void sub_080caa3c(void);
void sub_080caab8(void);
s32 sub_08009200(void);
s32 sub_08009398(s32 n);
void sub_080095e4(void);
void sub_08009640(void);
s32 sub_0800973c(void);
void sub_080098a8(void);
s32 sub_080099c8(s32 n);

void sub_080091ac(void)
{
    s32 i, vol;

    if (sub_08009200() == 1) {
        sub_080008e8(16, 2, gUnk_08731C88);
        vol = 256;
        for (i = 0; i < 16; i++) {
            sub_08003770(vol);
            sub_080022e4();
            vol -= 16;
        }
        sub_08003688();
        sub_08003770(256);
        sub_080022ac();
    } else {
        gUnk_03002150 = 1;
    }
}

s32 sub_08009200(void)
{
    s32 i;

    sub_080022ac();
    sub_080caa3c();
    gUnk_0300243C = 0x9999;
    sub_08008c4c(0);
    sub_08008c64(6);
    sub_08008c64(7);
    gUnk_03000010 = gUnk_0300117C = gUnk_03000FC0 = gUnk_03001EE0 = gUnk_03001E94 = gUnk_03000F8C = gUnk_03000FA8 = gUnk_03000B78 = 0;
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1100;
    sub_0800214c();
    sub_08002db4();
    sub_08002d54(60);
    sub_080058e4(0, 0);
    sub_08002d54(60);
    gUnk_0300243C = 1;
    if (sub_08009398(70) != 0)
        return 1;
    sub_080031b8(0x10D);
    if (sub_08009398(35) != 0)
        return 1;
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1F00;
    if (sub_08009398(21) != 0)
        return 1;
    gUnk_03000B18 = 49;
    gUnk_03000F7C = 62;
    gUnk_03000FD4 = 255;
    gUnk_03000044 = 160;
    gUnk_03001ED8 |= 0x2000;
    for (i = 85; i >= 0; i--) {
        gUnk_03000FD4 = i * 3;
        sub_080022e4();
    }
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1E00;
    gUnk_03001ED8 &= 0xDFFF;
    return 0;
}

s32 sub_08009398(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        if (gUnk_03000038 & 11)
            return 1;
        sub_080022e4();
    }
    return 0;
}

s32 sub_080093cc(void)
{
    while (gUnk_03001E90 != 0) {
        if (gUnk_03000038 & 11)
            return 1;
        sub_080022e4();
    }
    return 0;
}

void sub_080093fc(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    for (;;) {
        sub_080caab8();
        TaskYieldTrampoline(1);
    }
}

void sub_08009418(void)
{
    struct Task *t = gUnk_03002490;
    s32 *p;
    u16 *q;

    t->unk00 = 0;
    t->unk0C = 0;
    for (t->unk6C = 0; (s16)gUnk_03002490->unk6C <= 10; gUnk_03002490->unk6C++) {
        sub_080017e4(2, (u32)gUnk_08541D98[(s16)gUnk_03002490->unk6C], (u32)gUnk_03001430, 32);
        TaskYieldTrampoline(1);
    }
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 11; gUnk_03002490->unk6C++) {
        sub_08003014(gUnk_08541D98[13], gUnk_08541D98[11], (u16)(((s16)gUnk_03002490->unk6C + 1) * 21), 16, gUnk_03001430);
        sub_08003014(gUnk_08541F58, gUnk_08541F58 + 8, (u16)(((s16)gUnk_03002490->unk6C + 1) * 21), 8, gUnk_03001430 + 17);
        TaskYieldTrampoline(1);
    }
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 7; gUnk_03002490->unk6C++) {
        sub_08003014(gUnk_08541D98[11], gUnk_08541D98[12], (u16)(((s16)gUnk_03002490->unk6C + 1) * 32), 16, gUnk_03001430);
        sub_08003014(gUnk_08541F58 + 8, gUnk_08541F58, (u16)(((s16)gUnk_03002490->unk6C + 1) * 32), 8, gUnk_03001430 + 17);
        TaskYieldTrampoline(1);
    }
    /* Loop 4: the ROM hoists the store's &gUnk_03001F2C (after the task
       address) but reloads it from the pool for the u16 re-read.  The store
       goes through p, set inside the outer loop so loop.c hoists it in that
       order; the read goes through q, which is also assigned here, before
       the loop.  That dead assignment makes q live outside the loop, so
       loop.c does not merge q's in-loop load with p (combine_movables skips
       loop-global regs).  q's load then stays in the loop, as in the ROM.
       With a plain `(u16)gUnk_03001F2C` read, GCSE (store address in the
       same iteration) or combine_movables (p in the loop) gives the read a
       hoisted register instead. */
    q = (u16 *)&gUnk_03001F2C;
    for (;;) {
        gUnk_03002490->unk6C = 0;
        p = &gUnk_03001F2C;
        for (; (s16)gUnk_03002490->unk6C <= 15; gUnk_03002490->unk6C++) {
            *p = ((s16)gUnk_03002490->unk6C > 7 ? 16 - (s16)gUnk_03002490->unk6C : (s16)gUnk_03002490->unk6C) << 5;
            q = (u16 *)&gUnk_03001F2C;
            sub_08003014(gUnk_08541D98[12], gUnk_08541D98[13], *q, 16, gUnk_03001430);
            TaskYieldTrampoline(1);
        }
    }
}

void sub_080095c0(void)
{
    if (gUnk_03002490->unk18 == -1)
        sub_080095e4();
    else
        sub_08009640();
}

void sub_080095e4(void)
{
    struct Task *t;
    s32 idx;

    TaskYieldTrampoline(gUnk_03002490->unk1C);
    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < 10; gUnk_03002490->unk6C++) {
        idx = sub_080058e4(2, 0);
        t = gUnk_03002490;
        t->unk28 = idx;
        gUnk_03002790[idx].unk18 = (s16)t->unk6C;
        TaskYieldTrampoline(3);
    }
    TaskDispatchTrampoline();
}

void sub_08009640(void)
{
    struct Task *t, *u, *v;
    s32 m;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005ca0;
    t->unk38 = gUnk_087555B4;
    t->unk42 = 8;
    u = gUnk_03002490;
    if (u->unk18 <= 4) {
        u->unk4C = u->unk18 * 0x140000 + 0x180000;
    } else {
        m = (u->unk18 - 5) * 0x140000;
        /* Zero-byte clobber: keeps the product out of r0 so the else arm
         * allocates like the ROM (and is not cross-jumped into the then arm). */
        asm("" ::: "r0");
        u->unk4C = m + 0x880000;
    }
    u->unk50 = 0x800000;
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    v = gUnk_03002490;
    v->unk3C = gUnk_08731CC8[v->unk18];
    sub_08006138();
}

void sub_080096e0(void)
{
    while (sub_0800973c() == 0)
        sub_080098a8();
    gUnk_03001E90 = 10;
    gUnk_03000FB8 = 2;
    gUnk_030004A4 = 3;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
    sub_08002db4();
}

/* The key-wait loop is `while (1)` with both exits as gotos (a `break` would
   make expand_end_loop rotate it), and the result goes through one `ret`
   local: with a plain `return 0` the done block ends in a jump and jump.c's
   "if (foo) bar; else break;" swap moves it in front of the loop body. */
s32 sub_0800973c(void)
{
    s32 idx, i, ret;

    if (gUnk_03002150 != 1) {
        sub_080022ac();
        sub_08008c4c(0);
        sub_08008c64(7);
        gUnk_03000010 = gUnk_0300117C = gUnk_03000FC0 = gUnk_03001EE0 = gUnk_03001E94 = gUnk_03000F8C = gUnk_03000FA8 = gUnk_03000B78 = 0;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1E00;
    }
    if (gUnk_03002150 != 1) {
        sub_0800214c();
        sub_08002db4();
        idx = sub_080058e4(2, 0);
        gUnk_03002790[idx].unk18 = -1;
        gUnk_03002790[idx].unk1C = 0;
    } else {
        idx = sub_080058e4(2, 0);
        gUnk_03002790[idx].unk18 = -1;
        gUnk_03002790[idx].unk1C = 90;
    }
    sub_080058e4(1, 0);
    sub_08003110(26);
    if (gUnk_03002150 != 1)
        sub_08002d54(60);
    else
        sub_08002d54(210);
    i = 600;
    while (1) {
        if (--i == 0)
            goto timeout;
        sub_080022e4();
        if (gUnk_03000038 & 9)
            goto pressed;
    }
pressed:
    ret = 1;
    goto out;
timeout:
    gUnk_03002150 = 3;
    sub_080021dc();
    sub_08002db4();
    ret = 0;
out:
    return ret;
}

void sub_080098a8(void)
{
    s32 i;
    s32 j;
    s32 t;

    sub_080022ac();
    sub_08008c4c(1);
    sub_08008c64(8);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1C00;
    gUnk_0300118C = 84;
    gUnk_03000040 = 8;
    gUnk_03000B08 = 0;
    gUnk_03001EAC = 16;
    sub_080020b8();
    sub_08002db4();
    for (i = 0; i < 9; i++) {
        sub_08008c64(gUnk_08731CDC[i]);
        t = sub_080058e4(237, 0);
        gUnk_03002790[t].unk18 = i;
        for (j = 0; j <= 16; j++) {
            gUnk_03000B08 = j;
            gUnk_03001EAC = 16 - j;
            if (sub_080099c8(2))
                goto end;
        }
        if (sub_080099c8(gUnk_08731CE6[i]))
            break;
        if (i == 8)
            break;
        for (j = 0; j <= 16; j++) {
            gUnk_03000B08 = 16 - j;
            gUnk_03001EAC = j;
            if (sub_080099c8(2))
                goto end;
        }
    }
end:
    sub_08002198();
    sub_08002db4();
    gUnk_0300118C = gUnk_03000040 = gUnk_03000B08 = gUnk_03001EAC = 0;
}

s32 sub_080099c8(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        if (gUnk_03000038 & 11)
            return 1;
        sub_080022e4();
    }
    return 0;
}
