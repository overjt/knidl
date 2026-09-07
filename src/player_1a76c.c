#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct Collider
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 filler01;
    /*0x02*/ u16 unk02;
    /*0x04*/ u16 unk04;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ u8 *unk08;
};

extern struct PlayerState gUnk_03002170[];
extern struct Task *gUnk_03002490;
extern u8 gUnk_03005290;
extern struct Collider gUnk_030052A0[];
extern struct Collider gUnk_030053A0[];
extern u8 gUnk_030054A8;
extern struct Collider gUnk_030054B0[];
extern u8 gUnk_030054F4;
extern u32 gUnk_0874CFEC[];

void sub_080059d8(void);
void sub_0803ddc0(void);

void sub_0801a76c(s32 i)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_0874CFEC;
    gUnk_03002490->unk40 = (i << 13) | (i << 7);
    gUnk_03002490->unk88 = &gUnk_03002170[i];
}

void sub_0801a7b4(void)
{
    struct Collider *p = gUnk_030054B0;
    struct Collider *q = gUnk_030053A0;
    struct Collider *r = gUnk_030052A0;
    u8 *ca = &gUnk_03005290;
    u8 *cb = &gUnk_030054A8;
    u8 *cc = &gUnk_030054F4;
    s32 i;

    for (i = 0; i < 4; i++)
    {
        p->unk00 = 0;
        p->unk02 = 0;
        p->unk04 = 0;
        p->unk08 = 0;
        p++;
    }
    p = q;
    for (i = 0; i < 20; i++)
    {
        p->unk00 = 0;
        p->unk02 = 0;
        p->unk04 = 0;
        p->unk08 = 0;
        p++;
    }
    p = r;
    for (i = 0; i < 20; i++)
    {
        p->unk00 = 0;
        p->unk02 = 0;
        p->unk04 = 0;
        p->unk08 = 0;
        p++;
    }
    *cc = 0;
    *cb = 0;
    *ca = 0;
}

u32 sub_0801a828(u8 idx, u16 x, u16 y, u8 *p)
{
    struct Collider *r;

    switch (p[8] & 0xF0)
    {
    case 0x00:
        if (gUnk_03005290 == 4)
            return 1;
        r = &gUnk_030054B0[gUnk_03005290++];
        break;
    case 0x10:
        if (gUnk_030054A8 == 20)
            return 1;
        r = &gUnk_030053A0[gUnk_030054A8++];
        break;
    case 0x20:
        if (gUnk_030054F4 == 20)
            return 1;
        r = &gUnk_030052A0[gUnk_030054F4++];
        break;
    }
    r->unk00 = idx;
    r->unk02 = x;
    r->unk04 = y;
    r->unk08 = p;
    return 0;
}
