#include "gba/gba.h"
#include "global.h"

/* Main per-frame driver / VBlank waiter (0x08000DE4-0x080010CB, issue #32).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Runs the palette fade step (sub_08000934), advances the BGM/SE volume ramp
 * state machine (gUnk_03000B0C mode, gUnk_03000FBC volume 0..256,
 * gUnk_03000FCC delta), calls the per-frame hook gUnk_0300003C, spins on the
 * VBlank flag gUnk_03001EC4 (cleared by the handler in src/early_10cc.c),
 * handles the A+B+Start+Select soft-reset combo, ticks the play-time clock
 * gUnk_03000498[] (frames/seconds/minutes/hours, 59 rollovers, hour cap 998)
 * and finally calls the post-frame hook gUnk_03000014.
 *
 * Matching note: the clock's two `& mask` truncations are ONE HImode value.
 * A plain `u16 mask` local is promoted to SImode and loads as a single
 * `ldr rN,=0xFFFF`; the ROM instead has `ldr r5,=0xFFFF; adds r2,r5,#0` --
 * the agbcc HImode-move-of-a-large-constant shape (scratch register + copy
 * into the destination), which only appears when the variable really lives
 * in HImode.  `register u16 mask asm("r2")` reproduces it exactly (same
 * idiom as FadeOutBody in src/m4a_c1.c) and is also what forces r5 into the
 * prologue push, which in turn moves the case-3 `= 256` constant into r5.
 */

extern vs16 gUnk_03000B0C;
extern vs16 gUnk_03000FBC;
extern vu16 gUnk_03000FCC;
extern vu16 gUnk_03000AF8;
extern void (*gUnk_0300003C)(void);
extern vu16 gUnk_03001014;
extern u32 gUnk_03000B74;
extern vu16 gUnk_03001EC4;
extern vu16 gUnk_03000FD0;
extern vu16 gUnk_03001EF4;
extern vu16 gUnk_03001E90;
extern vu16 gUnk_03000038;
extern vu16 gUnk_0300243C;
extern vu16 gUnk_03001EF8;
extern vu32 gUnk_0200EBA0;
extern vu16 gUnk_03001EA4;
extern u16 gUnk_03000498[4];
extern void (*gUnk_03000014)(void);

struct MusicPlayerInfo;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;
extern void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume);
extern void m4aSoundVSync(void);
extern void SoundDriverVSyncOff(void);
extern void SoftReset(u32 resetFlags);
extern void sub_08000934(void);
extern void sub_08003484(void);
extern void sub_08004734(void);

void sub_08000de4(void)
{
    s32 i;
    u16 keys;
    register u16 mask asm("r2");
    sub_08000934();

    switch (gUnk_03000B0C)
    {
    case 0:
        break;
    case 1:
        gUnk_03000FBC += gUnk_03000FCC;
        if (gUnk_03000FBC > 255)
        {
            gUnk_03000FBC = 256;
            gUnk_03000FCC = 0;
            gUnk_03000B0C = 0;
        }
        if (gUnk_03000AF8 == 0)
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFF, gUnk_03000FBC);
        break;
    case 2:
        gUnk_03000FBC += gUnk_03000FCC;
        if (gUnk_03000FBC <= 0)
        {
            gUnk_03000FBC = 0;
            gUnk_03000FCC = 0;
            gUnk_03000B0C = 0;
            if (gUnk_03000AF8 == 0)
                sub_08003484();
        }
        if (gUnk_03000AF8 == 0)
            m4aMPlayVolumeControl(&gMPlayInfo_BGM, 0xFF, gUnk_03000FBC);
        break;
    case 3:
        gUnk_03000FBC += gUnk_03000FCC;
        if (gUnk_03000FBC > 255)
        {
            gUnk_03000FBC = 256;
            gUnk_03000FCC = 0;
            gUnk_03000B0C = 0;
        }
        if (gUnk_03000AF8 == 0)
        {
            m4aMPlayVolumeControl(&gMPlayInfo_SE1, 0xFF, gUnk_03000FBC);
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFF, gUnk_03000FBC);
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, 0xFF, gUnk_03000FBC);
        }
        break;
    case 4:
        gUnk_03000FBC += gUnk_03000FCC;
        if (gUnk_03000FBC <= 0)
        {
            gUnk_03000FBC = 0;
            gUnk_03000FCC = 0;
            gUnk_03000B0C = 0;
        }
        if (gUnk_03000AF8 == 0)
        {
            m4aMPlayVolumeControl(&gMPlayInfo_SE1, 0xFF, gUnk_03000FBC);
            m4aMPlayVolumeControl(&gMPlayInfo_SE2, 0xFF, gUnk_03000FBC);
            m4aMPlayVolumeControl(&gMPlayInfo_SE3, 0xFF, gUnk_03000FBC);
        }
        break;
    }

    if (gUnk_0300003C != 0)
    {
        gUnk_03001014 = 0;
        gUnk_0300003C();
        gUnk_03000B74 |= 1;
    }

    gUnk_03001EC4 = 1;
    gUnk_03000FD0 = 0;

    if (REG_IME & 1)
    {
        while (gUnk_03001EC4 != 0)
            ;
    }

    keys = gUnk_03001EF4 & 15;
    if (keys == 15 && gUnk_03001E90 == 0 && (keys & gUnk_03000038) != 0 && gUnk_0300243C == 1)
    {
        gUnk_03001EF8 = REG_IME = REG_IME & 0xFFFE;
        SoundDriverVSyncOff();
        m4aSoundVSync();
        for (i = 0x4000; i != 0; i--)
            ;
        SoftReset(0x1C);
    }

    gUnk_03000FD0 = 1;

    if (gUnk_0200EBA0 == 2)
        sub_08004734();

    gUnk_03001EA4++;

    if (gUnk_03000498[3] <= 998)
    {
        gUnk_03000498[0]++;
        mask = 0xFFFF;
        if (gUnk_03000498[0] > 59)
        {
            gUnk_03000498[0] = 0;
            gUnk_03000498[1]++;
            if ((gUnk_03000498[1] & mask) > 59)
            {
                gUnk_03000498[1] = 0;
                gUnk_03000498[2]++;
                if ((gUnk_03000498[2] & mask) > 59)
                {
                    gUnk_03000498[2] = 0;
                    gUnk_03000498[3]++;
                }
            }
        }
    }

    if (gUnk_03000014 != 0)
        gUnk_03000014();
}
