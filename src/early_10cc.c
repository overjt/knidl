#include "gba/gba.h"
#include "global.h"

/* VBlank interrupt handler (0x080010CC-0x080011AB, issue #32 batch A1).
 * agbcc -O2 -mthumb-interwork (game-code recipe).
 *
 * Pointer-called entry: runs the sound driver's vsync/main, flushes the OAM
 * and palette shadows via sub_080011ac, pumps the copy queue, and clears the
 * frame flag gUnk_03001EC4 that sub_08000de4 spins on. */

extern vu16 gUnk_03001174;
extern vu16 gUnk_03001E98;
extern vu16 gUnk_03001E90;
extern vs16 gUnk_03000FB8;
extern vu16 gUnk_030004A4;
extern u16 *gUnk_03001ED4;
extern vu32 gUnk_03000FB0;
extern vu16 gUnk_03000048;
extern vu16 gUnk_03001ED8;
extern u16 gUnk_03001270[];
extern u16 gUnk_03001A90[];

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
extern vu16 gUnk_03000038;
extern vu16 gUnk_0300243C;
extern vu16 gUnk_03001EF8;
extern vu32 gUnk_0200EBA0;
extern vu16 gUnk_03001EA4;
extern u16 gUnk_03000498[4];
extern void (*gUnk_03000014)(void);

extern vu16 gUnk_03001EE4;
extern u32 gUnk_03004D30;
extern void (*gUnk_03000FA4)(void);
extern vu16 gUnk_03000FAC;
extern vu16 gUnk_03001008;
extern void (*gUnk_03000F90)(void);
extern void (*gUnk_030004A0)(void);
extern void (*gUnk_03000AF4)(void);

struct MusicPlayerInfo;
extern struct MusicPlayerInfo gMPlayInfo_BGM;
extern struct MusicPlayerInfo gMPlayInfo_SE1;
extern struct MusicPlayerInfo gMPlayInfo_SE2;
extern struct MusicPlayerInfo gMPlayInfo_SE3;
extern void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume);
extern void m4aSoundVSync(void);
extern void m4aSoundMain(void);
extern void SoundDriverVSyncOff(void);
extern void SoftReset(u32 resetFlags);
extern void sub_08003484(void);
extern void sub_08004734(void);
extern void sub_08006cd4(void);
extern void sub_080011dc(void);
extern void sub_08001280(void);
extern void sub_080011ac(void);
extern void sub_080013f8(void);

void sub_08000934(void);

void sub_080010cc(void)
{
    if (gUnk_03001EE4 != 0)
        m4aSoundVSync();

    if (gUnk_0300243C != 1)
    {
        if (gUnk_03004D30 != 0)
        {
            gUnk_03004D30--;
            if (gUnk_03004D30 == 0)
                gUnk_0200EBA0 = 1;
        }
    }

    if (gUnk_0200EBA0 == 1)
        sub_08006cd4();

    if (gUnk_03000FA4 != 0)
        gUnk_03000FA4();

    gUnk_03000FAC++;

    if (gUnk_03000FD0 != 0)
    {
        if (gUnk_03001008 != 0)
            sub_080011dc();
    }
    else
    {
        sub_080011dc();
        if (gUnk_03000F90 != 0)
            gUnk_03000F90();
        sub_08001280();
        sub_080011ac();
        sub_080013f8();
        if (gUnk_030004A0 != 0)
            gUnk_030004A0();
    }

    if (gUnk_03001EE4 != 0)
        m4aSoundMain();

    if (gUnk_03000AF4 != 0)
        gUnk_03000AF4();

    gUnk_03001EC4 = 0;
}
