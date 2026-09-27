#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_5239c.c (0x0805239C-0x08052F6B, issue #90).
 *
 * Task type #6, variants 7-9, each variant body followed by the callbacks
 * only it installs.  Variant 7 (sub_0805239c, animation tables
 * gUnk_08751B40/gUnk_08751E5C) switches on the sub-state Task.unk18 & 15
 * and installs M11's sub_0803dfc8 and its own collision callback
 * sub_0805268c, which registers the collider row gUnk_0873BE24 and runs the
 * hit test sub_08030804(gUnk_0873CC1C) at the spawner's position (Task.unk8C).
 * Variant 8 (sub_080527a4, gUnk_08751BB0) traces six-step paths from the
 * 8.8 velocity rows gUnk_0873B7C0[k] and the animation rows
 * gUnk_0873B808[k] (sound 129) with the callback sub_08052b08 (collider row
 * gUnk_0873BE38, hit test gUnk_0873CC2C), which variant 10
 * (src/plobj_52f6c.c) installs too; both end in a `pop {r1}` epilogue
 * without setting r0, so they are declared s32 with no return.  Variant 9
 * (sub_08052b88, gUnk_0875204C) is a copy of the spawner's sprite:
 * sub-state 0 queues the tiles gUnk_08204B98 (four 320-byte rows) and the
 * palette gUnk_08204B78 into the spawner's OBJ slots through the VRAM
 * transfer queue RequestCopy, blinks, flies to the top centre of the
 * screen over 30 frames (Div) leaving sub-state-1 sparkles of itself,
 * plays sounds 175 and 176 and calls M07's race-record hook sub_08027588;
 * sub-state 1 is the three-frame sparkle. */

extern u32 gUnk_08751B40[];
extern u32 gUnk_08751E5C[];
extern u32 gUnk_0873BE24[];
extern u32 gUnk_0873CC1C[];
extern u32 gUnk_08751BB0[];
extern u16 gUnk_0873B7C0[][2][6];
extern u8 gUnk_0873B808[][6][5];
extern u32 gUnk_0873BE38[];
extern u32 gUnk_0873CC2C[];
extern u8 gObjPalette[];              /* OBJ palette buffer (M11 spelling) */
extern s16 gSpriteCameraX;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gSpriteCameraY;               /* scalar, read with ldrsh (32 landed files) */
extern u32 gUnk_0875204C[];
extern u8 gUnk_08204B98[];
extern u8 gUnk_08204B78[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
u32 RandomRange(u32 range);
s32 PlaySfx(s32 id);
void TaskFree(s32 id);
void TaskMove(void);
void TaskMoveRelativeToParent(void);
void TaskDrawScreen(void);
void TaskDrawWorld(void);
void TaskSetFrameByFacing(s16 a);
void TaskSetMotionXFacing(s32 a, s32 b);
void TaskStop(void);
u16 RandomSpread(u16 base, u8 scale, u8 amount);
s16 RandomSpreadFacing(u16 base, u8 scale, u8 amount);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
s32 sub_08027588(void);
u16 sub_08030804(struct HitBoxSet *p, s32 x, s32 y, s32 e);
s32 sub_08030848(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/sub_08050c48.c); landed M09/M12/M13 files spell it u16 */
void sub_0803dfc8(void);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);

s32 CreatePlayerObject(s8 player, u8 variant, s32 arg);
void sub_0805268c(void);
s32 sub_08052b08(void);   /* returns a value: pop {r1} epilogue; plobj_52f6c.c spells it void */

void sub_0805239c(void)
{
    {
        struct Task *t = gCurTask;
        t->unk00 = (u32)TaskMove;
        t->unk04 = (u32)sub_0805268c;
        t->unk28 = 0;
        t->unk80 = 13;
    }
    {
        struct Task *t = gCurTask;
        switch (t->unk18 & 15)
        {
        case 0:
            t->unk0C = (u32)sub_0803dfc8;
            t->unk38 = gUnk_08751B40;
            t->unk42 = 7;
            {
                struct Task *u = gCurTask;
                u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 12;
                u->unk3C = 0xFFFF;
            }
            TaskYieldTrampoline(8);
            while (gCurTask->unk28 == 0)
            {
                {
                    s16 d = RandomSpreadFacing(24, 1, 8);
                    struct Task *u = gCurTask;
                    u->unk4C = (d + ((struct Task *)u->unk8C)->unk48) << 16;
                }
                gCurTask->unk50 = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
                TaskSetMotionXFacing(0x10000, 0x4000);
                gCurTask->unk58 = 0;
                gCurTask->unk60 = (RandomRange(36) - 24) << 8;
                TaskSetFrameByFacing(0);
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C += 2;
                    TaskYieldTrampoline(2);
                }
                gCurTask->unk3C += 2;
                TaskYieldTrampoline(1);
                if (gCurTask->unk28 != 0)
                    break;
                {
                    s16 d = RandomSpreadFacing(24, 1, 8);
                    struct Task *u = gCurTask;
                    u->unk4C = (d + ((struct Task *)u->unk8C)->unk48) << 16;
                }
                gCurTask->unk50 = (RandomSpread(0, 1, 8) + ((struct Task *)gCurTask->unk8C)->unk4A) << 16;
                TaskSetMotionXFacing(0x10000, 0x4000);
                gCurTask->unk58 = 0;
                gCurTask->unk60 = (RandomRange(36) - 24) << 8;
                TaskSetFrameByFacing(14);
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C += 2;
                    TaskYieldTrampoline(2);
                }
                gCurTask->unk3C += 2;
                TaskYieldTrampoline(1);
            }
            break;
        case 1:
            t->unk0C = (u32)TaskDrawWorld;
            t->unk38 = gUnk_08751E5C;
            t->unk42 = 7;
            gCurTask->unk43 = ((struct Task *)gCurTask->unk8C)->unk43;
            {
                struct Task *u = gCurTask;
                u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 8;
            }
            do
            {
                struct Task *u = gCurTask;
                if (u->unk43 == 1)
                {
                    u->unk4C = (((struct Task *)u->unk8C)->unk48 + 24) << 16;
                    u->unk50 = ((struct Task *)u->unk8C)->unk4A << 16;
                }
                else
                {
                    u->unk4C = (((struct Task *)u->unk8C)->unk48 - 24) << 16;
                    u->unk50 = ((struct Task *)u->unk8C)->unk4A << 16;
                }
                TaskSetMotionXFacing(0x10000, (RandomRange(32) + 16) << 8);
                gCurTask->unk58 = 0;
                {
                    s32 r = RandomRange(32);
                    struct Task *v = gCurTask;
                    v->unk60 = -(r << 8);
                    v->unk3C = 0;
                }
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while (gCurTask->unk28 == 0);
            break;
        }
    }
    TaskExitTrampoline();
}

void sub_0805268c(void)
{
    struct Task *t = gCurTask;

    if (!(t->unk88->unk40 & 0x100) && (((struct Task *)t->unk8C)->unk7B & 1))
    {
        TaskFree(gCurTaskIdx);
        return;
    }
    if (gCurTask->unk88->unk0D != 13)
    {
        TaskFree(gCurTaskIdx);
        return;
    }
    {
        struct Task *u = gCurTask;
        if ((u->unk18 & 15) == 1)
        {
            u->unk78 = 127;
            sub_0801a828(gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873BE24);
            sub_08030848((struct HitBoxSet *)gUnk_0873CC1C, gCurTask->unk44);
            {
                struct Task *v = gCurTask;
                s32 off;
                if (v->unk43 == 1)
                    off = 12;
                else
                    off = -12;
                sub_08030804((struct HitBoxSet *)gUnk_0873CC1C, ((struct Task *)v->unk8C)->unk48 + off,
                             ((struct Task *)v->unk8C)->unk4A + 2, v->unk44);
            }
        }
    }
    {
        struct Task *u = gCurTask;
        if (u->unk28 == 0)
        {
            if (u->unk88->unk04 != 13 || u->unk43 != ((struct Task *)u->unk8C)->unk43)
                u->unk28 = 1;
        }
    }
}

s32 sub_080527a4(void)
{
    u16 *xs;
    u16 *ys;
    u8 *e;

    {
        struct Task *t = gCurTask;
        t->unk00 = (u32)TaskMoveRelativeToParent;
        t->unk0C = (u32)TaskDrawWorld;
        t->unk04 = (u32)sub_08052b08;
        t->unk42 = 5;
    }
    {
        struct Task *t = gCurTask;
        t->unk38 = gUnk_08751BB0;
        t->unk80 = 16;
    }
    {
        struct Task *t = gCurTask;
        t->unk40 = ((struct Task *)t->unk8C)->unk40 | 0xF008;
        if (t->unk43 == 1)
            t->unk28 = 14;
        else
            t->unk28 = -14;
    }
    {
        struct Task *t = gCurTask;
        t->unk2C = 2;
        switch (t->unk18 & 15)
        {
        case 0:
            if (!(t->unk88->unk42 & 0x80))
                PlaySfxIfLocalPlayer(129, t->unk44);
            xs = gUnk_0873B7C0[0][0];
            ys = gUnk_0873B7C0[0][1];
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
            {
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->unk4C = u->unk28 << 16;
                    u->unk50 = u->unk2C << 16;
                    v = xs[(s16)u->unk6C] << 8;
                    if (xs[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->unk6C] << 8;
                    if (ys[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                    u->unk58 = v;
                    e = gUnk_0873B808[0][(s16)u->unk6C];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->unk3C = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->unk54 = 0;
                    u->unk58 = 0;
                }
            }
            break;
        case 1:
            t->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            xs = gUnk_0873B7C0[1][0];
            ys = gUnk_0873B7C0[1][1];
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
            {
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->unk4C = u->unk28 << 16;
                    u->unk50 = u->unk2C << 16;
                    v = xs[(s16)u->unk6C] << 8;
                    if (xs[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->unk6C] << 8;
                    if (ys[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                    u->unk58 = v;
                    e = gUnk_0873B808[1][(s16)u->unk6C];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->unk3C = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->unk54 = 0;
                    u->unk58 = 0;
                }
            }
            break;
        case 2:
            t->unk3C = 0xFFFF;
            TaskYieldTrampoline(2);
            xs = gUnk_0873B7C0[2][0];
            ys = gUnk_0873B7C0[2][1];
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
            {
                s32 v;
                {
                    struct Task *u = gCurTask;
                    u->unk4C = u->unk28 << 16;
                    u->unk50 = u->unk2C << 16;
                    v = xs[(s16)u->unk6C] << 8;
                    if (xs[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                }
                TaskSetMotionXFacing(v, 0x5A5A5A5A);
                {
                    struct Task *u = gCurTask;
                    v = ys[(s16)u->unk6C] << 8;
                    if (ys[(s16)u->unk6C] & 0x8000)
                        v |= 0xFF000000;
                    u->unk58 = v;
                    e = gUnk_0873B808[2][(s16)u->unk6C];
                    u->unk6E = 0;
                }
                do
                {
                    gCurTask->unk3C = e[gCurTask->unk6E];
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 4);
                {
                    struct Task *u = gCurTask;
                    u->unk54 = 0;
                    u->unk58 = 0;
                }
            }
            break;
        }
    }
    TaskExitTrampoline();
}

s32 sub_08052b08(void)
{
    struct Task *t = gCurTask;

    t->unk78 = 127;
    if (t->unk88->unk04 != 13)
    {
        TaskFree(gCurTaskIdx);
    }
    else if ((t->unk18 & 15) == 0)
    {
        if (t->unk3C != -1)
            sub_0801a828(gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873BE38);
        sub_08030848((struct HitBoxSet *)gUnk_0873CC2C, gCurTask->unk44);
    }
}

void sub_08052b88(void)
{
    struct Task *t = gCurTask;

    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)TaskDrawScreen;
    t->unk38 = gUnk_0875204C;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk42 = 5;
        {
            u32 off = (((struct Task *)gCurTask->unk8C)->unk40 & 0xFFF) << 5;

            RequestCopy(1, gUnk_08204B98, (void *)(off + 0x06010080), 320);
            RequestCopy(1, gUnk_08204B98 + 320, (void *)(off + 0x06010480), 320);
            RequestCopy(1, gUnk_08204B98 + 640, (void *)(off + 0x06010880), 320);
            RequestCopy(1, gUnk_08204B98 + 960, (void *)(off + 0x06010C80), 320);
        }
        RequestCopy(2, gUnk_08204B78,
                     gObjPalette + (((((struct Task *)gCurTask->unk8C)->unk40 >> 12) + 1) << 5), 32);
        {
            struct Task *u = gCurTask;

            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 4;
            if (u->unk43 == 1)
                u->unk4C = (u->unk48 - gSpriteCameraX + 3) << 16;
            else
                u->unk4C = (u->unk48 - gSpriteCameraX - 3) << 16;
            u->unk50 = (u->unk4A - gSpriteCameraY) << 16;
        }
        PlaySfx(175);
        gCurTask->unk58 = 0x6000;
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
        {
            gCurTask->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
        {
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        }
        gCurTask->unk54 = Div((120 - gCurTask->unk48) << 16, 30);
        gCurTask->unk58 = Div(-gCurTask->unk4A << 16, 30);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 14; gCurTask->unk6C++)
        {
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->unk88->unk00, 9, 1);
        }
        PlaySfx(176);
        sub_08027588();
        {
            struct Task *u = gCurTask;

            u->unk0C = (u32)TaskDrawWorld;
            u->unk4C = (gSpriteCameraX + 120) << 16;
            u->unk50 = gSpriteCameraY << 16;
        }
        TaskStop();
        gCurTask->unk60 = 0x400;
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C--;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 12;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
        {
            ((volatile struct Task *)gCurTask)->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 6;
            TaskYieldTrampoline(1);
            ((volatile struct Task *)gCurTask)->unk3C = 0xFFFF;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        t->unk42 = 8;
        {
            struct Task *u = gCurTask;

            u->unk40 = gTasks[u->unk44].unk40;
            u->unk3C = 14;
        }
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        break;
    }
    TaskExitTrampoline();
}
