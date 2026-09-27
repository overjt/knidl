#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "mode.h"
#include "ending.h"

/* boot_caab8.c (0x080CAAB8-0x080CACEF, issue #100).
 *
 * BootLogoUpdateObjects, the per-frame interpreter of the boot logo's 115 sprite
 * objects that src/boot_caa3c.c seeds (called by M02's task type #0,
 * Task_BootLogo).  Each active object (unk04 != -1) waits out unk0A frames,
 * then runs its s16 command stream until it waits or is switched off: a
 * command is a bit mask and one argument, and bits 0-5 take one more word
 * each (x/y velocity, x/y acceleration, sprite id, wait; the word is skipped
 * when the bit is clear); bit 6 starts sound effect `arg` (PlaySfx),
 * bit 7 calls script `arg` (the cursor is saved in gUnk_0201BFD0[i]),
 * bit 8 switches to script `arg`, bits 9/10 set and run a loop of `arg`
 * passes, bit 11 restarts the script, bit 12 returns to the saved cursor
 * and bit 13 switches the object off.  The object then moves in 24.8 fixed
 * point and is drawn (QueueSprite, sprite gUnk_087554B8[unk06], layer
 * unk08), or switched off once it leaves the screen.
 *
 * Matching notes (#100's final campaign, lesson 3.494): the locals are
 * initialized at their declarations although each is assigned before it is
 * read; flow deletes those stores, but they raise the insn count gcse sees,
 * which is what orders the two PRE spill slots [sp, #12]/[sp, #16] as in the
 * ROM (lesson 4.105).  The off-screen switch-off keeps TWO zero-byte levers
 * (see the site): combine folds the plain `obj->unk04 = 0xFFFF;` into a
 * constant store because it knows the PRE-loaded old halfword fits in 16
 * bits, where the ROM keeps `ldr r1, =0xFFFF; adds r0, r1, #0; orrs r0, r7`.
 * No natural spelling that keeps the ROM's OR was found; the natural best
 * (the plain store, 40 differing bytes) is recorded on #100. */

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
s32 PlaySfx(s32 id);

/* Run the 115 boot-logo objects one frame: interpret each active object's
   script until it waits or ends, move it and draw it, switching it off
   once it leaves the screen. */
void BootLogoUpdateObjects(void)
{
    struct M38LogoObj *obj = gUnk_02030000;
    s32 i;
    s16 *p = 0;
    s16 mask = 0;
    s16 arg = 0;
    s32 j = 0;
    s32 x = 0, y = 0;

    for (i = 0; i < 115; obj++, i++) {
        if (obj->scriptId == -1)
            continue;
        if (obj->sleepFrames != 0) {
            obj->sleepFrames--;
        } else {
            while (obj->scriptId != -1 && obj->sleepFrames == 0) {
                p = obj->scriptPos;
                mask = *p++;
                arg = *p;
                for (j = 0; j < 14; j++) {
                    if ((mask >> j) & 1) {
                        switch (1 << j) {
                        case 1:
                            obj->velX = *p++;
                            break;
                        case 2:
                            obj->velY = *p++;
                            break;
                        case 4:
                            obj->accelX = *p++;
                            break;
                        case 8:
                            obj->accelY = *p++;
                            break;
                        case 16:
                            obj->spriteId = *p++;
                            break;
                        case 32:
                            obj->sleepFrames = *p++;
                            break;
                        case 64:
                            PlaySfx(arg);
                            break;
                        case 128:
                            gUnk_0201BFD0[i] = p;
                            p = gUnk_087577D8[arg];
                            break;
                        case 256:
                            obj->scriptId = arg;
                            p = gUnk_087577D8[obj->scriptId];
                            break;
                        case 512:
                            gUnk_0201BFD0[i] = p;
                            if (arg != 0)
                                obj->loopCount = arg;
                            break;
                        case 1024:
                            if (obj->loopCount != 0 && --obj->loopCount == 0)
                                break;
                            p = gUnk_0201BFD0[i];
                            break;
                        case 4096:
                            p = gUnk_0201BFD0[i];
                            break;
                        case 2048:
                            p = gUnk_087577D8[obj->scriptId];
                            break;
                        case 8192:
                            obj->scriptId = 0xFFFF;
                            break;
                        }
                    } else if (j <= 5) {
                        p++;
                    }
                }
                obj->scriptPos = p;
            }
        }
        obj->velX += obj->accelX;
        obj->velY += obj->accelY;
        obj->posX += obj->velX;
        obj->posY += obj->velY;
        x = obj->posX >> 8;
        y = obj->posY >> 8;
        if (obj->spriteId != -1) {
            if ((u32)(x + 15) <= 286 && y > -32 && y <= 191)
                QueueSprite(obj->layer, gUnk_087554B8[obj->spriteId], 0, 0, x, y);
            else {
                s32 m;

                /* LEVER 1 (zero bytes): an opaque 0xFFFF.  Combine would
                   fold (ior old 0xFFFF) into a bare constant store because
                   it knows the PRE-loaded old halfword has no bits above
                   0xFFFF (lessons 3.457, 3.494). */
                asm("" : "=r"(m) : "0"(0xFFFF));
                obj->scriptId |= m;
                /* LEVER 2 (zero bytes): keeps the mask live past the OR, so
                   the OR's result takes its own register (the ROM's
                   `adds r0, r1, #0` copy before the `orrs`). */
                asm("" : : "r"(m));
            }
        }
    }
}
