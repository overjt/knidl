#include "global.h"

extern void CpuSet(const void *src, void *dst, u32 control);
extern void sub_08004068(void);

extern u16 gUnk_04000208;
extern u16 gUnk_03001EF8;
extern u16 gUnk_0200EBC0[];
extern u32 gUnk_0200EBA8;
extern u32 gUnk_0200EC48;

struct Unk_030023A8
{
    s8 unk0;
    s8 unk1;
    s8 unk2;
    u8 unk3;
};

extern vs16 gUnk_0300244C;
extern struct Unk_030023A8 gUnk_030023A8;
extern vu16 gUnk_04000128;
extern vu16 gUnk_0400012A;
extern void *gUnk_030004B0[];
extern u16 gUnk_03000018;

void sub_08003888(void)
{
    register volatile u16 *ptr_03001EF8 asm("r6") = &gUnk_03001EF8;
    register volatile u16 *ptr_04000208 asm("r5") = &gUnk_04000208;
    register u16 *zeroPtr asm("r1");
    register volatile u16 *r4 asm("r4");
    register u32 clear asm("r3");
    u16 zero;

    *ptr_04000208 &= 0xFFFE;
    *ptr_03001EF8 = *ptr_04000208;

    zeroPtr = &zero;
    *zeroPtr = 0;

    r4 = gUnk_0200EBC0;
    CpuSet(&zero, (void *)r4, 0x01000018);

    {
        u16 value = r4[4];

        clear = 0;
        value = 0x80;
        value <<= 1;
        r4[4] = value;

        value = r4[4];
        r4[3];
        r4[3] = value;
    }

    gUnk_0200EBA8 = clear;
    gUnk_0200EC48 = clear;

    gUnk_0300244C = (gUnk_030023A8.unk3 = 255,
                     gUnk_030023A8.unk2 = -1,
                     gUnk_030023A8.unk1 = -1,
                     gUnk_030023A8.unk0 = -1);

    gUnk_04000128 = 0x2000;
    gUnk_04000128 |= 0x4003;
    gUnk_0400012A = clear;

    gUnk_030004B0[0] = sub_08004068;
    gUnk_030004B0[1] = sub_08004068;

    {
        // Usamos la dirección de memoria directa en lugar de la variable inventada
        register volatile u16 *r3 asm("r3") = (volatile u16 *)0x04000200;
        register volatile u16 *r2 asm("r2") = &gUnk_03000018;

        *r2 |= 0x80;
        *r3 = *r2;
    }

    *ptr_04000208 |= 1;
    *ptr_03001EF8 = *ptr_04000208;
}