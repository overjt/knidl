#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* roomtask_23618.c (0x08023618-0x08023947, issue #93).
 *
 * Task type #3 (class 4), the room's per-frame driver.  sub_08023618, the
 * type's body, dispatches Task.unk14 through the anchor table
 * gUnk_08732614: sub_08023634 (index 0), sub_08023e34 (1), sub_08023e78
 * (2), sub_08024540 (3), sub_080242d0 (4), sub_08024904 (5) and
 * sub_08024da4 (6); every room loader spawns the task with its own index
 * through sub_080235ec.  Each variant installs the task callbacks: unk0C
 * = sub_080236d4 (door objects and HUD), unk04 = the camera update of the
 * camera mode gUnk_030055C0 (sub_080236e4 here, M08's five camera modes)
 * and unk08 = one per-frame body chosen by the BG layout gUnk_0200B050
 * (sub_08023748 ... sub_080238ec here).  The bodies are gated by the flag
 * cell gUnk_03005624: 1 = camera, BG animation and BG streaming, 2 =
 * screen shake, 8 = HUD, 16 = door objects (sub_08026ca4). */

extern void (*gUnk_08732614[])(void);
extern u8 gUnk_0200B050;
extern u16 gUnk_03005624;
extern u16 gUnk_030055C0;

void sub_08002e98(u32 idx, u32 count, void (**fns)(void));
void sub_08006138(void);
void sub_0800a6a4(void);
void sub_0800a778(void);
void sub_0800a854(void);
void sub_08026ca4(void);
void sub_08026eec(void);
void sub_080293d8(void);
void sub_08029444(void);
void sub_08029474(void);
void sub_080294d0(void);
void sub_080295b8(void);
void sub_080296a0(void);
void sub_080297dc(void);
void sub_080298c4(void);
void sub_08029930(void);
void sub_080299e8(void);
void sub_08029bb8(void);
void sub_0802b4bc(void);
void sub_0802b62c(void);
void sub_0802be80(void);
void sub_0802bff4(void);
void sub_0802c42c(void);
void sub_0802cd38(void);
void sub_0802d188(void);
void sub_080236d4(void);
void sub_080236e4(void);
void sub_08023748(void);
void sub_080237a4(void);
void sub_080237fc(void);
void sub_0802385c(void);
void sub_080238a4(void);
void sub_080238ec(void);

void sub_08023618(void)
{
    sub_08002e98(gUnk_03002490->unk14, 7, gUnk_08732614);
}

void sub_08023634(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = (u32)sub_080236d4;
    t->unk04 = (u32)sub_080236e4;
    switch (gUnk_0200B050)
    {
    default:
    case 0:
        gUnk_03002490->unk08 = (u32)sub_08023748;
        break;
    case 1:
        gUnk_03002490->unk08 = (u32)sub_080237fc;
        break;
    case 2:
        gUnk_03002490->unk08 = (u32)sub_080238ec;
        break;
    case 3:
        gUnk_03002490->unk08 = (u32)sub_080237a4;
        break;
    case 4:
        gUnk_03002490->unk08 = (u32)sub_0802385c;
        break;
    case 5:
        gUnk_03002490->unk08 = (u32)sub_080238a4;
        break;
    }
    sub_08006138();
}

void sub_080236d4(void)
{
    sub_08026eec();
    sub_0800a6a4();
}

void sub_080236e4(void)
{
    if (gUnk_03005624 & 1)
    {
        switch (gUnk_030055C0)
        {
        default:
        case 0:
        case 1:
            sub_0802b4bc();
            break;
        case 2:
            sub_0802b62c();
            break;
        case 3:
            sub_0802be80();
            break;
        case 4:
            sub_0802bff4();
            break;
        case 5:
            sub_0802c42c();
            break;
        }
    }
}

void sub_08023748(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_080293d8();
        sub_080294d0();
        sub_08029bb8();
    }
    sub_08029930();
    if (gUnk_03005624 & 16)
        sub_08026ca4();
    if (gUnk_03005624 & 8)
        sub_0800a854();
    sub_0800a778();
}

void sub_080237a4(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_080293d8();
        sub_080296a0();
    }
    sub_08029930();
    if (gUnk_03005624 & 16)
        sub_08026ca4();
    if (gUnk_03005624 & 8)
        sub_0800a854();
    sub_0800a778();
}

void sub_080237fc(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_080293d8();
        sub_080294d0();
        sub_080295b8();
        sub_08029bb8();
    }
    sub_08029930();
    if (gUnk_03005624 & 16)
        sub_08026ca4();
    if (gUnk_03005624 & 8)
        sub_0800a854();
    sub_0800a778();
}

void sub_0802385c(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_08029444();
        sub_080297dc();
    }
    sub_080299e8();
    if (gUnk_03005624 & 8)
        sub_0800a854();
    sub_0800a778();
}

void sub_080238a4(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_08029444();
        sub_080298c4();
    }
    sub_080299e8();
    if (gUnk_03005624 & 8)
        sub_0800a854();
    sub_0800a778();
}

void sub_080238ec(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_08029474();
        sub_080294d0();
        sub_08029bb8();
    }
    sub_08029930();
    if (gUnk_03005624 & 16)
        sub_08026ca4();
    if (gUnk_03005624 & 8)
        sub_0800a854();
    sub_0800a778();
}
