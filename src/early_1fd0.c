#include "gba/gba.h"
#include "global.h"

/* Early game-code block 0x08001CC8-0x08002377 (issue #32, batch B2).
 *
 * Contents, in ROM order:
 *   sub_08001cc8  affine/rotscale OBJ emitter: walks a sprite template list,
 *                 converts each entry into the affine OAM staging buffer at
 *                 0x03001190 and fills one 0x03000050 affine matrix.
 *   sub_08001fd0  clears the eight BG scroll shadow cells.
 *   sub_08002028  clears the brightness/fade block and the blend/window
 *                 shadow bytes.
 *   sub_080020b8 .. sub_08002220  the fade-request family: each one seeds the
 *                 brightness state block (target/current level, step, flags).
 *   sub_08002268  compares and refreshes the 9 header bytes at 0x03000000
 *                 against the ROM copy at 0x0872EB2C; result cached in
 *                 gUnk_03000B00 and returned.
 *   sub_080022a0 .. sub_08002358  small helpers (reset the cached flag,
 *                 re-init wrappers, OAM-shadow bookkeeping, 0x03005274 codes).
 *
 * Recipe: this translation unit is old_agbcc -O2 -mthumb-interwork
 * (fnmatch.sh --old2), NOT the default agbcc -O2 that the rest of the early
 * game code uses -- see the report/lessons note: the leaf functions here have
 * no `push {lr}` at all, which agbcc always emits.
 *
 * Three functions in this range are dead exports with no in-ROM references
 * (lesson 2.13): sub_08002104, sub_08002220 and sub_08002358.  They sit
 * between evidenced functions and were recovered by disassembling the gaps.
 */

extern vs16 gUnk_03001190[];       /* affine OBJ staging buffer */
extern vu16 gUnk_03001A80;         /* staging buffer write index */
extern vu16 gUnk_03000050[];       /* OAM shadow (attrs + affine params) */
extern vu16 gUnk_03000B1C;         /* affine matrix index */
extern u8 gUnk_0872EB14[];   /* shape/size -> {w,h} half-dims */
extern s16 gUnk_0872FB30[];  /* trig table (mid pointer) */


/* BG scroll shadow cells (cleared as one volatile chain each). */
extern vu32 gUnk_0300117C;
extern vu32 gUnk_03001EE0;
extern vu32 gUnk_03000F8C;
extern vu32 gUnk_03000B78;
extern vu32 gUnk_03000010;
extern vu32 gUnk_03000FC0;
extern vu32 gUnk_03001E94;
extern vu32 gUnk_03000FA8;

/* Brightness/fade state block. */
extern vs16 gUnk_03001E90;
extern vs16 gUnk_03000FB8;
extern vs16 gUnk_030004A4;
extern vs16 gUnk_03001174;
extern vs16 gUnk_03001E98;
extern vs16 gUnk_03000048;
extern u32 gUnk_03001ED4;

/* Blend/window shadow bytes. */
extern vu8 gUnk_0300118C;
extern vu8 gUnk_03000040;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03001EAC;
extern u16 gUnk_03001EEC;

extern vu32 gUnk_03000B00;
extern u16 gUnk_03000498[4];
extern vu16 gUnk_03001EB8[4];
extern vu16 gUnk_03000F98[4];
extern vu16 gUnk_03005274;
extern vu16 gUnk_03001F38;
extern const u8 gUnk_0872EB2C[];

extern void sub_08004fec(void);
extern void sub_08001a0c(void);
extern void sub_08001a84(void);
extern void sub_08000de4(void);
extern void sub_08001a34(void);
extern void sub_08005228(void);

void sub_08001fd0(void)
{
    gUnk_0300117C = gUnk_03001EE0 = gUnk_03000F8C = gUnk_03000B78 = 0;
    gUnk_03000010 = gUnk_03000FC0 = gUnk_03001E94 = gUnk_03000FA8 = 0;
}

void sub_08002028(void)
{
    gUnk_03000FB8 = gUnk_030004A4 = gUnk_03001174 = gUnk_03001E98 = gUnk_03001E90 = gUnk_03000048 = gUnk_03001ED4 = 0;
    gUnk_0300118C = gUnk_03000040 = gUnk_03000B08 = gUnk_03001EAC = gUnk_03001EEC = 0;
}

void sub_080020b8(void)
{
    gUnk_03001E90 = gUnk_03000FB8 = 31;
    gUnk_030004A4 = -1;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
}

void sub_08002104(void)
{
    gUnk_03001E90 = 31;
    gUnk_03000FB8 = -31;
    gUnk_030004A4 = 1;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
}

void sub_0800214c(void)
{
    gUnk_03001E90 = 16;
    gUnk_03000FB8 = 32;
    gUnk_030004A4 = -2;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
}

void sub_08002198(void)
{
    gUnk_03001E90 = 31;
    gUnk_03000FB8 = 0;
    gUnk_030004A4 = 1;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
}

void sub_080021dc(void)
{
    gUnk_03001E90 = 16;
    gUnk_03000FB8 = 0;
    gUnk_030004A4 = 2;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
}

void sub_08002220(void)
{
    gUnk_03001E90 = 31;
    gUnk_03000FB8 = 0;
    gUnk_030004A4 = -1;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 0;
    gUnk_03001ED4 = 0;
}

u32 sub_08002268(void)
{
    vu8 *p = (vu8 *)0x03000000;
    u32 ok = 1;
    u32 i;
    u32 c;

    for (i = 0; i < 9; i++)
    {
        c = gUnk_0872EB2C[i];
        if (*p != c)
            ok = 0;
        *p++ = c;
    }
    gUnk_03000B00 = ok;
    return ok;
}

u32 sub_080022a0(u32 arg)
{
    gUnk_03000B00 = 0;
    return arg;
}

void sub_080022ac(void)
{
    sub_08004fec();
    sub_08001a0c();
}

void sub_080022bc(void)
{
    gUnk_03000498[3] = 0;
    gUnk_03000498[2] = 0;
    gUnk_03000498[1] = 0;
    gUnk_03000498[0] = 0;
}

void sub_080022d0(void)
{
    sub_08001a84();
    sub_08000de4();
    sub_08001a34();
}

void sub_080022e4(void)
{
    sub_08005228();
    sub_08001a84();
    sub_08000de4();
    sub_08001a34();
}

void sub_080022fc(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
    gUnk_03005274 = 0x8800;
}

void sub_08002338(void)
{
    gUnk_03005274 = 0x9900;
}

void sub_08002348(void)
{
    gUnk_03005274 = 0x6600;
}

void sub_08002358(void)
{
    if (gUnk_03001F38 != 0)
        gUnk_03005274 = 0x7755;
}
