#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_39c24.c (0x08039C24-0x0803AA63, issue #91).
 *
 * Player action body, part 8: action 21 (sub_08039c24, 3612 bytes), the
 * walk through a door the player entered with action 20.  It plays the
 * ability's door animations (gUnk_0873D632[ability][1..6], or
 * sub_0803f7e0 for the form with Task.unk7B bit 0 set), moves the player
 * by the door side kept in Task.unk2C, drives the door's M08 stage
 * objects through M07's helpers (sub_080264b0 ... sub_08026704) and the
 * cameras through sub_08027850/sub_08027908/sub_08027a30/sub_08027a60;
 * the second mode of gUnk_0200AF00 also uploads an OBJ graphic
 * (LZ77UnCompWram of gUnk_080D07C8 into gUnk_02020000, then 0x06014000).
 * sub_0803aa40, per-frame handler 19, only releases the player once
 * Task.unk28 is set. */

extern u32 gUnk_0873CB1C[];
extern u8 gUnk_020055C4;
extern u16 gUnk_030023AC;               /* number of players */
extern u8 gUnk_0200AF00;
extern s16 gUnk_02005588[];             /* health per player (M02's HUD) */
extern vu16 gUnk_03001E90;
extern u16 gUnk_0873D632[][7];
extern struct PlayerState gUnk_03002170[];
extern u16 gUnk_0873D0F8[][5];
extern u8 gUnk_080D07C8[];
extern u32 gUnk_02020000[];
extern u8 gUnk_080DCC68[];
extern u8 gUnk_020055E8;
extern s16 gUnk_0873D3B8[][2];
extern u8 gUnk_03001F34;

void TaskYieldTrampoline(s32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_080031b8(s32 id);
void sub_080055b0(u8 val, s32 idx);
void sub_08006138(void);
void sub_08006338(s32 a);
void sub_08021c74(s8 *box, s32 id);
void sub_080224b0(void);
void sub_080261d4(u16 a);
s32 sub_080264b0(void);
void sub_0802651c(s32 i);
s32 sub_0802653c(void);
s32 sub_08026584(void);
void sub_08026704(s32 i);
s32 sub_0802672c(void);
void sub_08027850(s32 a);
void sub_08027908(void);
s32 sub_08027a30(void);
void sub_08027a60(void);
void sub_0803cd60(void);
void sub_0803e050(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f6e0(void);
u16 sub_0803f7e0(u16 a0);
s32 sub_0804042c(void);
void sub_08040808(s32 a0);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_08049738(void);                     /* M12, still asm */
void sub_08049a58(void);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08039c24(void)
{
    s32 i;
    s32 r;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 19;
    gUnk_03002490->unk15 = 19;
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk3C = 0xFFFF;
    sub_080224b0();
    sub_08021c74((s8 *)gUnk_0873CB1C, gCurTaskIdx);
    if (gUnk_020055C4 == 0)
    {
        sub_0803f6e0();
        if (gUnk_030023AC > 1 && gUnk_0200AF00 == 1)
        {
            for (i = 0; i < gUnk_030023AC; i++)
            {
                if (gUnk_02005588[i] == 0)
                    sub_080055b0(4, i);
            }
        }
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk28 = 0;
        if (t->unk2C == 1 || t->unk2C == 3)
        {
            t->unk43 = -1;
            gUnk_03002490->unk4C = (gUnk_03002490->unk48 - 10) << 16;
        }
    }
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    if (gUnk_03002490->unk2C == 0)
        r = sub_080264b0();
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        if (!(t->unk7B & 1))
        {
            t->unk46 = gUnk_0873D632[t->unk88->unk0D][1];
            switch (t->unk88->unk0D)
            {
            case 0:
            default:
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                break;
            case 1:
            case 2:
            case 5:
            case 19:
            case 22:
            case 23:
                sub_08006338(gUnk_03002490->unk46);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                break;
            }
        }
        else
        {
            gUnk_03002490->unk46 = sub_0803f7e0(1);
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
    }
    switch (gUnk_0200AF00)
    {
    case 0:
    default:
        switch (gUnk_03002490->unk2C)
        {
        case 0:
        case 2:
        {
            struct Task *t;

            sub_08040b40(10, 9);
            t = gUnk_03002490;
            t->unk54 += t->unk2C << 13;
            break;
        }
        case 1:
        case 3:
        {
            struct Task *t;

            sub_08040b40(10, 10);
            t = gUnk_03002490;
            t->unk54 -= (t->unk2C - 1) << 13;
            break;
        }
        }
        {
            struct Task *t = gUnk_03002490;

            if (!(t->unk7B & 1))
                t->unk46 = gUnk_0873D632[t->unk88->unk0D][2];
            else
                gUnk_03002490->unk46 = sub_0803f7e0(2);
        }
        sub_08006338(gUnk_03002490->unk46);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        if (gUnk_03002490->unk2C == 0)
            sub_0802651c(r);
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
        sub_0803e050(1);
        if (gUnk_03002490->unk2C == 0)
        {
            sub_08026584();
            sub_08040808(gUnk_03002490->unk88->unk00);
        }
        break;
    case 1:
        for (i = 0; i < gUnk_030023AC; i++)
            gUnk_03002170[i].unk42 |= 64;
        if (gUnk_03002490->unk2C != 0)
        {
            switch (gUnk_03002490->unk2C)
            {
            case 0:
            case 2:
            {
                struct Task *t;

                gUnk_03002490->unk43 = 1;
                sub_08040b40(10, 9);
                t = gUnk_03002490;
                t->unk54 += t->unk2C << 13;
                break;
            }
            case 1:
            case 3:
            {
                struct Task *t;

                gUnk_03002490->unk43 = -1;
                sub_08040b40(10, 10);
                t = gUnk_03002490;
                t->unk54 -= (t->unk2C - 1) << 13;
                break;
            }
            }
            {
                struct Task *t = gUnk_03002490;

                if (!(t->unk7B & 1))
                    t->unk46 = gUnk_0873D632[t->unk88->unk0D][2];
                else
                    gUnk_03002490->unk46 = sub_0803f7e0(2);
            }
            sub_08006338(gUnk_03002490->unk46);
            TaskYieldTrampoline(2);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 5; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
            {
                struct Task *t = gUnk_03002490;

                if (!(t->unk7B & 1))
                {
                    sub_08006338((s16)gUnk_0873D0F8[t->unk88->unk0D][0]);
                    TaskYieldTrampoline(1);
                }
                else
                {
                    if (t->unk88->unk0D == 4)
                        t->unk46 = 0x4A0;
                    else if (t->unk88->unk0D == 9)
                        t->unk46 = 0x7C1;
                    else if (t->unk88->unk0D == 10)
                        t->unk46 = 0x8C1;
                    else
                        t->unk46 = 221;
                    sub_08006338(gUnk_03002490->unk46);
                    TaskYieldTrampoline(1);
                }
            }
            sub_0803e050(1);
            gUnk_03002490->unk88->unk42 &= 0xFFEF;
            sub_080055b0(15, gCurTaskIdx);
            break;
        }
        LZ77UnCompWram(gUnk_080D07C8, gUnk_02020000);
        sub_080017e4(3, (u32)gUnk_080DCC68, 0x06014000, 0x400);
        sub_08027850(gCurTaskIdx);
        {
            struct Task *t = gUnk_03002490;

            t->unk3E = 0x2000;
            if (!(t->unk7B & 1))
            {
                t->unk46 = gUnk_0873D632[t->unk88->unk0D][3];
                switch (t->unk88->unk0D)
                {
                case 0:
                default:
                    sub_08040b40(10, 11);
                    gUnk_03002490->unk3C = gUnk_03002490->unk46;
                    TaskYieldTrampoline(4);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    break;
                case 1:
                case 2:
                case 5:
                case 19:
                case 22:
                case 23:
                    sub_08040b40(10, 12);
                    gUnk_03002490->unk3C = gUnk_03002490->unk46;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    gUnk_03002490->unk3C++;
                    TaskYieldTrampoline(2);
                    break;
                }
            }
            else
            {
                sub_08040b40(10, 11);
                gUnk_03002490->unk3C = gUnk_03002490->unk46 = sub_0803f7e0(3);
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            }
        }
        sub_08040b40(10, 13);
        if (!(gUnk_03002490->unk7B & 1))
        {
            struct Task *t;

            sub_080413a4(20);
            t = gUnk_03002490;
            t->unk46 = gUnk_0873D632[t->unk88->unk0D][4];
        }
        else
        {
            sub_080413a4(21);
            gUnk_03002490->unk46 = sub_0803f7e0(4);
        }
        gUnk_03002490->unk3C = gUnk_03002490->unk46;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_0802651c(r);
        gUnk_03002490->unk3C = gUnk_03002490->unk46;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        sub_0803e050(3);
        i = sub_0802653c();
        sub_080261d4(2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 20, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 20, 1);
        sub_0803e650(14);
        sub_080031b8(238);
        {
            struct Task *t = gUnk_03002490;

            if (!(t->unk7B & 1))
            {
                t->unk46 = gUnk_0873D632[t->unk88->unk0D][5];
                switch (t->unk88->unk0D)
                {
                case 0:
                default:
                    gUnk_03002490->unk3C = gUnk_03002490->unk46;
                    TaskYieldTrampoline(24);
                    break;
                case 1:
                case 2:
                case 5:
                case 19:
                case 22:
                case 23:
                    for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 5; gUnk_03002490->unk6C++)
                    {
                        gUnk_03002490->unk3C = gUnk_03002490->unk46;
                        TaskYieldTrampoline(2);
                        gUnk_03002490->unk3C++;
                        TaskYieldTrampoline(2);
                    }
                    break;
                }
            }
            else
            {
                gUnk_03002490->unk3C = sub_0803f7e0(5);
                TaskYieldTrampoline(24);
            }
        }
        gUnk_03002490->unk88->unk42 &= 0xFFEF;
        sub_080055b0(14, gCurTaskIdx);
        sub_0802672c();
        {
            struct Task *t = gUnk_03002490;

            if (!(t->unk7B & 1))
            {
                switch (t->unk88->unk0D)
                {
                case 0:
                default:
                    while (gUnk_020055E8 == 0)
                        TaskYieldTrampoline(1);
                    break;
                case 1:
                case 2:
                case 5:
                case 19:
                case 22:
                case 23:
                    for (;;)
                    {
                        gUnk_03002490->unk3C = gUnk_03002490->unk46;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                        gUnk_03002490->unk3C++;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                        TaskYieldTrampoline(1);
                        if (gUnk_020055E8 != 0)
                            break;
                    }
                    break;
                }
            }
            else
            {
                while (gUnk_020055E8 == 0)
                    TaskYieldTrampoline(1);
            }
        }
        sub_080055b0(0, gCurTaskIdx);
        gUnk_03002490->unk3E = 0x4000;
        sub_08026704(i);
        sub_080017e4(3, (u32)gUnk_02020000, 0x06014000, 0x400);
        sub_08027908();
        sub_08040b40(10, 14);
        if (!(gUnk_03002490->unk7B & 1))
        {
            struct Task *t;

            sub_080413a4(20);
            t = gUnk_03002490;
            t->unk46 = gUnk_0873D3B8[t->unk88->unk0D][1];
            switch (t->unk88->unk0D)
            {
            case 0:
            case 7:
            case 20:
            case 21:
            {
                struct Task *u;

                gUnk_03002490->unk3C = gUnk_03002490->unk46;
                TaskYieldTrampoline(2);
                u = gUnk_03002490;
                u->unk3C = gUnk_0873D632[u->unk88->unk0D][6];
                TaskYieldTrampoline(2);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C--;
                    TaskYieldTrampoline(2);
                }
                break;
            }
            case 10:
                gUnk_03002490->unk46 = 0x841;
                goto anim;
            case 2:
                sub_08049a58();
            default:
            anim:
                gUnk_03002490->unk3C = gUnk_03002490->unk46;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(2);
                break;
            }
            sub_08040b40(10, 15);
            while (1)
            {
                gUnk_03002490->unk3C = gUnk_03002490->unk46;
                TaskYieldTrampoline(1);
                if (gUnk_03002490->unk7A & 1)
                    break;
                TaskYieldTrampoline(1);
                if (gUnk_03002490->unk7A & 1)
                    break;
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
                if (gUnk_03002490->unk7A & 1)
                    break;
                TaskYieldTrampoline(1);
                if (gUnk_03002490->unk7A & 1)
                    break;
            }
        }
        else
        {
            sub_080413a4(21);
            gUnk_03002490->unk3C = sub_0803f7e0(6);
            TaskYieldTrampoline(14);
            sub_08040b40(10, 15);
            while (!(gUnk_03002490->unk7A & 1))
                TaskYieldTrampoline(1);
        }
        sub_0803e050(3);
        gUnk_03002490->unk88->unk3E = 2;
        if (!(gUnk_03002490->unk7B & 1))
            sub_0803cd60();
        sub_08026584();
        while (sub_08027a30() == 0)
            TaskYieldTrampoline(1);
        sub_08027a60();
        for (i = 0; i < gUnk_030023AC; i++)
        {
            gUnk_03002170[i].unk42 &= 0xFFBF;
            if (gUnk_02005588[i] != 0)
                sub_08040808(i);
            if (i != gCurTaskIdx)
                sub_080055b0(0, i);
        }
        sub_08049738();
        break;
    }
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_0803aa40(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        sub_0804042c();
        gUnk_03001F34 = 0;
    }
}
