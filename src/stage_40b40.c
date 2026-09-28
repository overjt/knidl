#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "room.h"
#include "player.h"

/* stage_40b40.c (0x08040B40-0x080413A3, issue #85).
 *
 * The player's motion preset setter, called some 200 times from M09-M14's
 * action bodies: a0 picks one of 14 kinds of motion and a1 an entry of the
 * preset table gPlayerMotionXPresets (two halfword pairs).  Each kind writes the
 * 16.16 motion cells Task.velX/unk5C (through TaskSetMotionXFacing, which mirrors
 * them by the facing and leaves a component alone when passed 0x5A5A5A5A)
 * and Task.speedLimitX/unk68 from signed 8.8 halfwords of gPlayerMotionXPresets[a1] or of
 * the preset row gUnk_0873AF6C[gMetaKnightmareMode] (22 halfwords), some of them
 * chosen by PlayerState.onSlipperyFloor, the held keys gLatchedHeldKeys[] or the
 * collision block gTerrainResult; kind 12 picks one of five rows of
 * gUnk_0873AF58 by the speed.
 *
 * Matching note (issue #85): the key mask is read inline at every test (a
 * cached s32 mask let regmove AND in place and pushed case 13's task pointer
 * into ip, lessons 4.62/4.63 and 3.476). */

/* Not from collision.h: this file's view of gTerrainResult differs (lesson
   3.517). */
extern u8 gTerrainResult[];

/* Byte-exact.  The key-mask table gLatchedHeldKeys[...] is read inline at every
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
   per-block `struct Task *t = gCurTask;` before a sign-extended 8.8 store IS
   load-bearing (the ROM loads the task pointer before the value; without
   it 1833 bytes differ). */

void PlayerSetMotionXPreset(s32 a0, s32 a1)
{
    u16 *q = (u16 *)(gUnk_0873AF6C + gMetaKnightmareMode * 11);
    u16 *r = (u16 *)(gPlayerMotionXPresets + a1 * 2);

    switch (a0)
    {
    case 0:
        if (gCurTask->player->onSlipperyFloor == 0)
        {
            s32 v = q[2] << 8;
            if (q[2] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        else
        {
            s32 v = q[4] << 8;
            if (q[4] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        gCurTask->speedLimitX = 0;
        break;
    case 1:
        if (gCurTask->player->onSlipperyFloor == 0)
        {
            s32 v = q[1] << 8;
            if (q[1] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        else
        {
            s32 v = q[3] << 8;
            if (q[3] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        {
            struct Task *t = gCurTask;

            s32 v = q[0] << 8;
            if (q[0] & 0x8000)
                v |= 0xFF000000;
            t->speedLimitX = v;
        }
        break;
    case 2:
        if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) != 0)
        {
            if (gCurTask->player->onSlipperyFloor == 0)
            {
                s32 v = q[1] << 8;
                if (q[1] & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(0x5A5A5A5A, v);
            }
            else
            {
                s32 v = q[3] << 8;
                if (q[3] & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(0x5A5A5A5A, v);
            }
            {
                struct Task *t = gCurTask;

                s32 v = q[0] << 8;
                if (q[0] & 0x8000)
                    v |= 0xFF000000;
                t->speedLimitX = v;
            }
        }
        else
        {
            PlayerSetMotionXPreset(0, 72);
        }
        break;
    case 3:
        if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) != 0)
        {
            if (gCurTask->player->onSlipperyFloor == 0)
            {
                s32 v = q[6] << 8;
                if (q[6] & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(0x5A5A5A5A, v);
            }
            else
            {
                s32 v = q[3] << 8;
                if (q[3] & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(0x5A5A5A5A, v);
            }
            {
                struct Task *t = gCurTask;

                s32 v = q[5] << 8;
                if (q[5] & 0x8000)
                    v |= 0xFF000000;
                t->speedLimitX = v;
            }
        }
        else
        {
            PlayerSetMotionXPreset(0, 72);
        }
        break;
    case 4:
        if (gCurTask->player->onSlipperyFloor == 0)
        {
            s32 v = q[10] << 8;
            if (q[10] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        else
        {
            s32 v = q[11] << 8;
            if (q[11] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        gCurTask->speedLimitX = 0;
        break;
    case 5:
        if (gCurTask->player->onSlipperyFloor == 0)
            TaskSetMotionXFacing(0x20000, 0x1000);
        else
            TaskSetMotionXFacing(0x20000, 0x800);
        gCurTask->speedLimitX = 0;
        break;
    case 6:
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            s32 v = q[17] << 8;
            if (q[17] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
            gCurTask->speedLimitX = 0x10000;
        }
        else
        {
            s32 v = q[18] << 8;
            if (q[18] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
            gCurTask->speedLimitX = 0;
        }
        break;
    case 7:
        if ((gLatchedHeldKeys[gCurTask->player->playerIndex] & 48) != 0)
        {
            if ((gCurTask->waterFlags & 1) == 0)
            {
                s32 v = q[14] << 8;
                if (q[14] & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(0x5A5A5A5A, v);
                {
                    struct Task *t2 = gCurTask;

                    s32 v = q[16] << 8;
                    if (q[16] & 0x8000)
                        v |= 0xFF000000;
                    t2->speedLimitX = v;
                }
            }
            else
            {
                TaskSetMotionXFacing(0x5A5A5A5A, 0x800);
                gCurTask->speedLimitX = 0x10C00;
            }
            break;
        }
        if ((gCurTask->waterFlags & 1) == 0)
        {
            s32 v = q[15] << 8;
            if (q[15] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        else
        {
            TaskSetMotionXFacing(0x5A5A5A5A, 0x900);
        }
        gCurTask->speedLimitX = 0;
        break;
    case 8:
        {
            s32 v = q[20] << 8;
            if (q[20] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        gCurTask->speedLimitX = 0;
        break;
    case 9:
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            {
                s32 v = q[19] << 8;
                if (q[19] & 0x8000)
                    v |= 0xFF000000;
                TaskSetMotionXFacing(0x5A5A5A5A, v);
            }
            {
                struct Task *t = gCurTask;

                s32 v = q[21] << 8;
                if (q[21] & 0x8000)
                    v |= 0xFF000000;
                t->speedLimitX = v;
            }
        }
        else
        {
            s32 v = q[20] << 8;
            if (q[20] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
            gCurTask->speedLimitX = 0;
        }
        break;
    case 10:
        if (r[0] != 0x9999)
        {
            struct Task *t = gCurTask;

            s32 v = r[0] << 8;
            if (r[0] & 0x8000)
                v |= 0xFF000000;
            t->velX = v;
        }
        if (r[1] != 0x9999)
        {
            struct Task *t = gCurTask;

            s32 v = r[1] << 8;
            if (r[1] & 0x8000)
                v |= 0xFF000000;
            t->accelX = v;
        }
        if (r[2] != 0x9999)
        {
            struct Task *t = gCurTask;

            s32 v = r[2] << 8;
            if (r[2] & 0x8000)
                v |= 0xFF000000;
            t->speedLimitX = v;
        }
        break;
    case 11:
        if (r[0] != 0x9999)
        {
            s32 v = r[0] << 8;
            if (r[0] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(v, 0x5A5A5A5A);
        }
        if (r[1] != 0x9999)
        {
            s32 v = r[1] << 8;
            if (r[1] & 0x8000)
                v |= 0xFF000000;
            TaskSetMotionXFacing(0x5A5A5A5A, v);
        }
        if (r[2] != 0x9999)
        {
            struct Task *t = gCurTask;

            s32 v = r[2] << 8;
            if (r[2] & 0x8000)
                v |= 0xFF000000;
            t->speedLimitX = v;
        }
        break;
    case 12:
        {
            s32 n;

            if (a1 == 1)
            {
                if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
                {
                    switch (gCurTask->player->slope)
                    {
                    case 0:
                    default:
                        n = 0;
                        break;
                    case 1:
                        n = (gCurTask->facing == 1) ? 1 : 2;
                        break;
                    case 2:
                        n = (gCurTask->facing == 1) ? 2 : 1;
                        break;
                    case 3:
                        n = (gCurTask->facing == 1) ? 3 : 4;
                        break;
                    case 4:
                        n = (gCurTask->facing == 1) ? 4 : 3;
                        break;
                    }
                    {
                        struct Task *t = gCurTask;
                        s32 d;

                        s32 v = gUnk_0873AF58[n][1] << 8;
                        if (gUnk_0873AF58[n][1] & 0x8000)
                            v |= 0xFF000000;
                        t->speedLimitX = v;
                        d = t->velX;
                        if (d < 0)
                            d = -d;
                        if (d <= v)
                        {
                            s32 v = gUnk_0873AF58[n][0] << 8;
                            if (gUnk_0873AF58[n][0] & 0x8000)
                                v |= 0xFF000000;
                            TaskSetMotionXFacing(0x5A5A5A5A, v);
                        }
                        else
                        {
                            TaskSetMotionXFacing(0x5A5A5A5A, 0xE00);
                        }
                    }
                }
                else
                {
                    TaskSetMotionXFacing(0x5A5A5A5A, 0xE00);
                    gCurTask->speedLimitX = 0;
                }
            }
            else
            {
                if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
                {
                    TaskSetMotionXFacing(0x5A5A5A5A, 0xC00);
                    gCurTask->speedLimitX = 0x20000;
                }
                else
                {
                    TaskSetMotionXFacing(0x5A5A5A5A, 0x400);
                    gCurTask->speedLimitX = 0;
                }
            }
        }
        break;
    case 13:
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 16)
            {
                if ((gCurTask->waterFlags & 1) == 0)
                {
                    gCurTask->accelX = 0x8000;
                    gCurTask->speedLimitX = 0x20000;
                }
                else
                {
                    gCurTask->accelX = 0x4000;
                    gCurTask->speedLimitX = 0x10000;
                }
                if (gTerrainResult[0] == 2)
                    gTerrainResult[0] = 0;
            }
            else
            {
                if ((gCurTask->waterFlags & 1) == 0)
                {
                    gCurTask->accelX = 0xFFFF8000;
                    gCurTask->speedLimitX = 0x20000;
                }
                else
                {
                    gCurTask->accelX = 0xFFFFC000;
                    gCurTask->speedLimitX = 0x10000;
                }
                if (gTerrainResult[0] == 1)
                    gTerrainResult[0] = 0;
            }
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 192)
            {
                if ((gCurTask->waterFlags & 1) == 0)
                    gCurTask->speedLimitX = 0x18000;
                else
                    gCurTask->speedLimitX = 0xC000;
            }
        }
        else
        {
            gCurTask->speedLimitX = 0;
            if (gCurTask->velX == 0)
                PlayerStopAxes(1);
        }
        gCurTask->onGround = 0;
        if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 192)
        {
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 64)
            {
                if ((gCurTask->waterFlags & 1) == 0)
                {
                    gCurTask->accelY = 0xFFFF8000;
                    gCurTask->speedLimitY = 0x20000;
                }
                else
                {
                    gCurTask->accelY = 0xFFFFC000;
                    gCurTask->speedLimitY = 0x10000;
                }
            }
            else
            {
                if ((gCurTask->waterFlags & 1) == 0)
                {
                    gCurTask->accelY = 0x8000;
                    gCurTask->speedLimitY = 0x20000;
                }
                else
                {
                    gCurTask->accelY = 0x4000;
                    gCurTask->speedLimitY = 0x10000;
                }
            }
            if (gLatchedHeldKeys[gCurTask->player->playerIndex] & 48)
            {
                u16 b = gLatchedHeldKeys[gCurTask->player->playerIndex] & 16;

                if (b != 0)
                {
                    if (gTerrainResult[0] == 2)
                        gTerrainResult[0] = 0;
                }
                else
                {
                    if (gTerrainResult[0] == 1)
                        gTerrainResult[0] = 0;
                }
                if ((gCurTask->waterFlags & 1) == 0)
                    gCurTask->speedLimitY = 0x18000;
                else
                    gCurTask->speedLimitY = 0xC000;
            }
        }
        else
        {
            gCurTask->speedLimitY = 0;
            if (gCurTask->velY == 0)
                PlayerStopAxes(2);
        }
        break;
    }
}
