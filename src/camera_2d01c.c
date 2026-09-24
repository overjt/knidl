#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camera_2d01c.c (0x0802D01C-0x0802D38B, issue #86).
 *
 * Camera mode switches, the room's BG animation scripts and the camera
 * task spawners.  gUnk_030055C0 is the camera mode: sub_0802d01c leaves
 * mode 3 for 4 or 0 from the flags in gUnk_03005680.unk1, sub_0802d074
 * snaps the 16.16 camera target gUnk_03005614/gUnk_03005634 to the player
 * and picks mode 4 or 5, and sub_0802d0c4 is a dead export (both arms of
 * its test store 0).  sub_0802d0f4 resets the ten BG animation slots
 * gUnk_02007D70[] and loads the room's scripts from
 * gUnk_087E1F20[gUnk_030055EC->unk40]; sub_0802d188 runs them every frame,
 * eight-byte commands: 0 copies tiles to 0x06004000 (sub_0802d25c), 1
 * starts a palette fade (sub_0802d278, stepped by sub_0802d294 into the
 * palette buffer gUnk_030012B0), 2 waits, 3 loops, 5 sets a metatile's
 * solid flag (sub_0802d2f0), 6 plays a sound effect, anything else stops
 * the slot (sub_0802d32c).  sub_0802d344 spawns task type #4 through M07's
 * sub_0802621c; sub_0802d370, the type's body, dispatches on Task.unk14
 * into the seven camera tasks of the anchor table gUnk_087328A0. */

struct Unk03005680
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 unkC;
};

/* One of the ten BG animation slots at 0x02007D70 (28 bytes): a script of
   8-byte commands (unk4, pc unk0, wait timer unk2) plus a palette fade
   (unk8 step, read signed, 0xFFFF = idle; unkC/unk10 the two palettes, unk14 the colour
   index into the BG palette buffer, unk16 the count, unk18 the rate). */
struct Unk02007D70Cmd
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ void *unk4;
};

struct Unk02007D70
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk02007D70Cmd *unk4;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u16 *unkC;
    /*0x10*/ u16 *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u32 unk18;
};

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

/* The room header gUnk_030055EC points at (one entry of the gUnk_087E1D58
   room table): unk18/unk28 are length-prefixed palettes, unk30 the BG map
   streamed into 0x06003000, unk40 the room's BG animation script set. */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x18];
    /*0x18*/ u16 *unk18;
    /*0x1C*/ u8 filler1C[0xC];
    /*0x28*/ u16 *unk28;
    /*0x2C*/ u8 filler2C[4];
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u8 filler34[0xC];
    /*0x40*/ u16 unk40;
};

struct Unk0802D25C
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4[0];
};

struct Unk0802D278
{
    /*0x00*/ u16 *unk0;
    /*0x04*/ u16 *unk4;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ u32 unkC;
};

struct MapTile
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

extern u16 gUnk_030055C0;
extern struct Unk03005680 gUnk_03005680;
extern s32 gUnk_03005610;
extern s32 gUnk_03005664;
extern u8 gUnk_03002340;
extern s32 gUnk_03005614;
extern s16 gUnk_03002398;
extern s32 gUnk_03005634;
extern s16 gUnk_03001F00;
extern s8 gUnk_03002444;
extern struct RoomDef *gUnk_030055EC;
extern struct Unk02007D70 gUnk_02007D70[];
extern struct Unk02007D70Cmd **gUnk_087E1F20[];
extern u16 gUnk_030012B0[];
extern s16 gUnk_0300561C;
extern s16 gUnk_03005620;
extern struct MapTile *gUnk_03005660;
extern void (*gUnk_087328A0[])(void);

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080031b8(u32 a);
s32 sub_0802621c(s32 type);
void sub_08028948(void);
void sub_08028b1c(void);
void sub_0802d25c(struct Unk0802D25C *a);
void sub_0802d278(struct Unk02007D70 *p, struct Unk0802D278 *q);
void sub_0802d294(struct Unk02007D70 *p);
void sub_0802d2f0(u32 x, u32 y, u32 v);
void sub_0802d32c(struct Unk02007D70 *p);

void sub_0802d01c(void)
{
    if (gUnk_030055C0 == 3)
    {
        if (gUnk_03005680.unk1 & 1)
            gUnk_03005610 = 6;
        if (gUnk_03005680.unk1 & 2)
            gUnk_03005664 = 3;
        if (gUnk_03005680.unk1 != 0)
            gUnk_030055C0 = 4;
        else
            gUnk_030055C0 = 0;
        gUnk_03005680.unk0 = gUnk_03002340;
    }
}

void sub_0802d074(void)
{
    gUnk_03005614 = gUnk_03002398 << 16;
    gUnk_03005634 = gUnk_03001F00 << 16;
    if (gUnk_03002444 != 0)
        gUnk_030055C0 = 4;
    else
        gUnk_030055C0 = 5;
}

void sub_0802d0c4(void)
{
    sub_08028948();
    sub_08028b1c();
    /* both arms store 0 in the ROM too (sub_0802d074 stores 4 / 5) */
    if (gUnk_03002444 != 0)
        gUnk_030055C0 = 0;
    else
        gUnk_030055C0 = 0;
}

void sub_0802d0f4(void)
{
    s32 i;
    s32 k;

    i = 0;
    for (k = 0; k < 10; k++)
    {
        gUnk_02007D70[k].unk0 = 0x7FFF;
        gUnk_02007D70[k].unk8 |= 0xFFFF;
    }
    if (gUnk_030055EC->unk40 != 0)
    {
        while (gUnk_087E1F20[gUnk_030055EC->unk40][i] != 0)
        {
            struct Unk02007D70 *p = &gUnk_02007D70[i];
            p->unk4 = gUnk_087E1F20[gUnk_030055EC->unk40][i];
            p->unk0 = 0;
            p->unk2 = 0;
            i++;
        }
    }
}

/* The do { } while (0) around the loop body is load-bearing: its loop
   notes weight every reference inside by one more loop level, which lifts
   the slot pointer above the command pointer in global-alloc priority
   (p = r4, cmd = r5 as in the ROM).  The command loop itself is a goto
   loop, as a real one gets rotated. */
void sub_0802d188(void)
{
    s32 i;
    struct Unk02007D70 *p;
    struct Unk02007D70Cmd *cmd;

    for (i = 0; i < 10; i++)
    {
        do
        {
            p = &gUnk_02007D70[i];
            if (p->unk0 == 0x7FFF)
                continue;
        loop:
            if ((s16)p->unk8 != -1)
                sub_0802d294(p);
            if (--p->unk2 > 0)
                continue;
            cmd = &p->unk4[p->unk0];
            switch (cmd->unk0)
            {
            case 0:
                sub_0802d25c(cmd->unk4);
                p->unk2 = cmd->unk2 + 1;
                p->unk0++;
                goto loop;
            case 1:
                sub_0802d278(p, cmd->unk4);
                p->unk2 = cmd->unk2 + 1;
                p->unk0++;
                goto loop;
            case 2:
                p->unk2 = cmd->unk2 + 1;
                p->unk0++;
                goto loop;
            case 3:
                p->unk0 = 0;
                goto loop;
            case 5:
                sub_0802d2f0(cmd->unk2 >> 8, cmd->unk2 & 0xFF, (u16)(u32)cmd->unk4);
                p->unk0++;
                goto loop;
            case 6:
                sub_080031b8(cmd->unk2);
                p->unk0++;
                goto loop;
            default:
                sub_0802d32c(p);
                break;
            }
        } while (0);
    }
}

void sub_0802d25c(struct Unk0802D25C *a)
{
    sub_080017e4(1, (u32)a->unk4, 0x06004000 + a->unk0 * 32, a->unk2);
}

void sub_0802d278(struct Unk02007D70 *p, struct Unk0802D278 *q)
{
    p->unkC = q->unk0;
    p->unk10 = q->unk4;
    p->unk14 = q->unk8;
    p->unk16 = q->unkA;
    p->unk18 = q->unkC;
    p->unk8 = 0;
}

void sub_0802d294(struct Unk02007D70 *p)
{
    s32 t;

    p->unk8++;
    t = (p->unk18 * (s16)p->unk8) >> 8;
    if (t > 0x100)
        t = 0x100;
    sub_08003014(p->unkC, p->unk10, (u16)t, p->unk16, &gUnk_030012B0[p->unk14]);
    if (t == 0x100)
        p->unk8 = -1;
}

void sub_0802d2f0(u32 x, u32 y, u32 v)
{
    if (x < gUnk_03005620 && y < gUnk_0300561C)
        gUnk_03005660[y * gUnk_03005620 + x].unk3 = v;
}

void sub_0802d32c(struct Unk02007D70 *p)
{
    p->unk4 = 0;
    p->unk0 = 0x7FFF;
    p->unk2 = 0;
    p->unk8 = -1;
}

s32 sub_0802d344(s32 a)
{
    s32 id;
    struct Task *t;

    id = sub_0802621c(4);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk14 = a;
    }
    return id;
}

void sub_0802d370(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_087328A0);
}
