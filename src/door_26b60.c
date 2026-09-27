#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "collision.h"
#include "room.h"

/* door_26b60.c (0x08026B60-0x080270CF, issue #93).
 *
 * The door objects: one gDoorStates record per RoomDef door that is not
 * one of the special ids 0x1A0A, 0x1E61 or 0x15B3.  sub_08026b60 and
 * UpdateDoors (the per-frame body, flag 16 of gRoomUpdateFlags) decide
 * whether a door is usable - in multi-player every present player must be
 * within 128 pixels - and step its animation (frame in the low nibble of
 * byte 4, timer in the high one, the star doors from gUnk_0873264C);
 * DrawDoors draws the visible ones with QueueSprite. */

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 IsWorldPosOnScreen(s16 x, s16 y);

void sub_08026b60(void)
{
    struct Door *d = gCurRoomDef->doors;
    s16 i;

    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s32 lim;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gDoorStates[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        p->unk1 = 0;
        if (gPlayerCount > 1 && gActivePlayerCount > 1)
        {
            gUnk_03002344 = 128;
            gUnk_03001F2C = x - gRoomEntryX;
            gUnk_03002448 = y - gRoomEntryY;
            lim = 0x4000;
            if (lim < gUnk_03001F2C * gUnk_03001F2C + gUnk_03002448 * gUnk_03002448)
                p->unk1 = 0;
            else
                p->unk1 = 1;
        }
        else
        {
            p->unk1 = 1;
        }
        if (--p->unk4_4 == 0)
        {
            if (++p->unk4_0 > 3)
                p->unk4_0 = 0;
            p->unk4_4 = 3;
        }
    }
}

void UpdateDoors(void)
{
    struct Door *d = gCurRoomDef->doors;
    s16 i;

    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s16 j;
        s16 k;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gDoorStates[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        p->unk1 = 0;
        k = GetCollisionTileAtPixel(x, y);
        if (gCollisionTileDoor[k] == 0)
            continue;
        if (gPlayerCount > 1 && gActivePlayerCount > 1)
        {
            gUnk_03002160 = 0;
            gUnk_03002344 = 128;
            for (j = 0; j < gPlayerCount; j++)
            {
                if ((gActivePlayerMask >> j) & 1)
                {
                    struct Task *t = &gTasks[j];

                    gUnk_03001F2C = x - t->pixelX;
                    gUnk_03002448 = y - t->pixelY;
                    if (gUnk_03002344 * gUnk_03002344 >= gUnk_03001F2C * gUnk_03001F2C + gUnk_03002448 * gUnk_03002448)
                        gUnk_03002160++;
                }
                else
                {
                    gUnk_03002160++;
                }
            }
            if (gUnk_03002160 == gPlayerCount)
                p->unk1 = 1;
            else
                p->unk1 = 0;
        }
        else
        {
            p->unk1 = 1;
        }
        if (p->unk0 != 2)
        {
            if (--p->unk4_4 == 0)
            {
                if (++p->unk4_0 > 3)
                    p->unk4_0 = 0;
                p->unk4_4 = 3;
            }
        }
        else
        {
            if (--p->unk4_4 == 0)
            {
                if (++p->unk4_0 > 5)
                    p->unk4_0 = 0;
                p->unk4_4 = gUnk_0873264C[p->unk4_0][1];
            }
        }
    }
}

void DrawDoors(void)
{
    struct Door *d;
    s16 i;

    if (gUnk_03002444 != 0)
        return;
    d = gCurRoomDef->doors;
    for (i = 0; i < gCurRoomDef->doorCount; d++, i++)
    {
        struct Unk02004B90 *p;
        s16 x;
        s16 y;
        s16 k;

        if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61 || d->unk0 == 0x15B3)
            continue;
        p = &gDoorStates[i];
        x = d->unk2 << 4;
        y = d->unk4 << 4;
        if (IsWorldPosOnScreen(x, y) == 0)
            continue;
        k = GetCollisionTileAtPixel(x, y);
        if (k != 16 && k != 144)
            continue;
        gUnk_03001F10 = -1;
        switch (p->unk0)
        {
        case 1:
            gUnk_03001F2C = 16;
            if ((gUnk_02007CF0 != 2 || gActivePlayerCount <= 1) && p->unk1 != 0)
                gUnk_03001F10 = 8;
            else
                gUnk_03001F10 = 12;
            gUnk_03001F10 += p->unk4_0;
            break;
        default:
        case 0:
            gUnk_03001F2C = 8;
            if ((gUnk_02007CF0 != 2 || gActivePlayerCount <= 1) && p->unk1 != 0)
                gUnk_03001F10 = 0;
            else
                gUnk_03001F10 = 4;
            gUnk_03001F10 += p->unk4_0;
            break;
        case 2:
            gUnk_03001F2C = 8;
            if (p->unk1 != 0)
                gUnk_03001F10 = 0;
            else
                gUnk_03001F10 = 6;
            gUnk_03001F10 += gUnk_0873264C[p->unk4_0][0];
            break;
        }
        QueueSprite(15, gUnk_0874CDF8[gUnk_03001F10], 0, 0, x + gUnk_03001F2C - gSpriteCameraX,
                     -gSpriteCameraY + y);
    }
}
