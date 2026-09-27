#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_b5670.c (0x080B5670-0x080B583F, issue #97).
 *
 * Graphics loader for one object of the room's object list gUnk_020055D8
 * (called by src/hud_b4ea8.c's sub_080b4ea8 for every kind-1 entry, which
 * counts the return values): e is the entry, idx its index in the list and n
 * the next free graphics slot of gUnk_020060A0.  If an earlier kind-1 entry
 * already uses the same graphics descriptor gUnk_0873EEA0[e->unk1], the
 * entry shares that slot (gUnk_02006130[idx]) and the function returns 0.
 * Otherwise it claims slot n: it allocates OBJ tiles (sub_080b5628) and
 * copies the descriptor's tiles (through gUnk_02020000 when compressed), then
 * shares the palette of an earlier slot in the same palette group
 * gUnk_0873EF48[kind] or allocates one (sub_080b5654) and copies it into the
 * palette buffer gObjPalette; a non-zero high nibble of e->unk2 is passed
 * to sub_08065dbc with the slot's palette.  It returns 1.
 *
 * Matching notes (issue #97): sub_08065dbc takes three arguments; the VRAM
 * base is a pointer local that global allocation drops (lesson 3.258), and the
 * byte the ROM keeps at [sp, #4] is the compiler's own copy of
 * gUnk_02006130[i] (lesson 3.474). */

struct Unk020055D8Entry
{
    /*0x00*/ s8 unk0;
    /*0x01*/ s8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 filler3;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
};

struct Unk020055D8
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk020055D8Entry *unk4;
};

struct Unk0873EEA0
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u32 unk8;
    /*0x0C*/ u32 unkC;
};

struct Unk020060A0
{
    /*0x00*/ s8 unk0;
    /*0x01*/ s8 unk1;
    /*0x02*/ s16 unk2;
};

extern struct Unk020055D8 gUnk_020055D8;
extern struct Unk020060A0 gUnk_020060A0[];
extern s8 gUnk_02006130[];
extern u8 gUnk_02020000[];
extern u8 gObjPalette[];
extern u8 gObjVram[];
extern struct Unk0873EEA0 *gUnk_0873EEA0[];
extern s8 gUnk_0873EF48[];

void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void sub_08065dbc(u32 slot, u32 sub, u32 level);
s32 sub_080b5628(u32 a);
s32 sub_080b5654(u32 a);

s32 sub_080b5670(struct Unk020055D8Entry *e, s32 idx, s32 n)
{
    struct Unk0873EEA0 *d;
    s32 i;
    s32 cnt;
    u32 vram;

    /* a plain pointer local that global alloc drops: reload rematerialises
       `ldr r0, =gObjVram` at both uses, which the ROM's reload order
       needs (lesson 3.258) */
    vram = (u32)gObjVram;
    d = gUnk_0873EEA0[e->unk1];
    if (d == NULL)
        return 0;
    for (i = 0; i < idx; i++)
    {
        if (gUnk_020055D8.unk4[i].unk0 == 1)
        {
            if (gUnk_02006130[i] != -1
             && gUnk_0873EEA0[gUnk_020060A0[gUnk_02006130[i]].unk0] == d)
            {
                /* the compiler's QImode copy of this byte is what spills to
                   the ROM's `mov r5, sp; strb r0, [r5, #4]` slot */
                gUnk_02006130[idx] = gUnk_02006130[i];
                return 0;
            }
        }
    }
    gUnk_02006130[idx] = n;
    gUnk_020060A0[n].unk0 = e->unk1;
    if (d->unk2 != 0)
    {
        gUnk_020060A0[n].unk2 = sub_080b5628(d->unk2);
        if (d->unk6 != 0)
        {
            LZ77UnCompVram((void *)d->unkC, gUnk_02020000);
            RequestCopy(4, (u32)gUnk_02020000, (gUnk_020060A0[n].unk2 << 6) + vram, d->unk2 << 5);
        }
        else
        {
            RequestCopy(4, d->unkC, (gUnk_020060A0[n].unk2 << 6) + vram, d->unk2 << 5);
        }
    }
    if (d->unk0 != 0)
    {
        cnt = 0;
        if (gUnk_0873EF48[e->unk1] != -1)
        {
            for (i = 0; i < n; i++)
            {
                if (gUnk_020055D8.unk4[i].unk0 == 1
                 && gUnk_0873EF48[gUnk_020060A0[i].unk0] == gUnk_0873EF48[e->unk1])
                {
                    gUnk_020060A0[n].unk1 = gUnk_020060A0[i].unk1;
                    cnt++;
                    break;
                }
            }
        }
        if (cnt == 0)
        {
            gUnk_020060A0[n].unk1 = sub_080b5654(d->unk0);
            RequestCopy(2, d->unk8, (u32)gObjPalette + (gUnk_020060A0[n].unk1 << 5), d->unk0 << 5);
        }
        if (e->unk2 >> 4)
            sub_08065dbc(gUnk_020060A0[n].unk1, e->unk1, e->unk2 >> 4);
    }
    return 1;
}
