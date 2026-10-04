#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "save.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void HudDrawTiles(u8 *s, s32 a, s32 b, s32 c);
extern void TaskSetEntry(void *fn, s32 i);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);

void PlayerLifeRequestClear(void)
{
    struct Unk02005E00 *s;
    struct Task *t;
    u8 *f;
    u8 *e;
    s32 z;

    s = &gLifeRequests;
    t = gCurTask;
    f = (u8 *)s + 4;
    e = &f[t->playerLifeGiver];
    z = 0;
    *e = z;
    s->timeout = z;
}
void PlayerClearOwnLifeRequests(void)
{
    struct Unk02005E00 *s;
    u8 *f;
    u8 *e;
    s32 i;
    s32 v;
    s32 z;

    i = 0;
    s = &gLifeRequests;
    f = (u8 *)s + 4;
    z = 0;
    do
    {
        e = (u8 *)(i + (u32)f);
        v = *e << 24;
        if (v != 0 && (u32)v >> 28 == gCurTaskIdx)
        {
            *e = z;
            s->timeout = z;
        }
        i++;
    } while (i <= 3);
}
void PlayerLifeRequestPickStartState(void)
{
    struct Task *t;
    s32 i;
    s32 n;
    t = gCurTask;
    t->moveCallback = 0;
    t->drawCallback = 0;
    if (gLifeRequests.gameOver[gCurTaskIdx] != 0)
    {
        t->state = 6;
    }
    else
    {
        for (i = 0, n = 0; i < 4; i++)
        {
            if (gLifeRequests.requests[i] != 0 && gLifeRequests.requests[i] >> 4 == gCurTaskIdx)
            {
                gCurTask->playerLifeGiver = i;
                n++;
            }
        }
        if (n != 0)
        {
            PlayerLifeRequestLoadGfx(1);
            if (gPlayerLives[gCurTask->playerLifeGiver] > 0)
                gCurTask->state = 2;
            else
                gCurTask->state = 4;
        }
        else
        {
            gCurTask->state = n;
        }
    }
}
void PlayerLifeRequestLoadGfx(s32 a)
{
    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        RequestCopy(2, (u32)gUnk_085ADD1C, (u32)gBgPalette, 64);
        if (a <= 2)
            RequestCopy(1, gUnk_0875625C[a], 192 << 19, gUnk_08756268[a] << 5);
    }
}
void PlayerLifeRequestOpenMenu(void)
{
    struct Task *t;

    t = gCurTask;
    t->playerLifeRequestCursor = 0;
    if (gLocalPlayer == t->player->playerIndex)
    {
        PlayerLifeRequestLoadGfx(0);
        HudClearTilemap();
        PlayerLifeRequestDrawMenu(gCurTask->playerLifeRequestCursor);
    }
}
void PlayerLifeRequestOpenGiverList(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gLifeRequestShownLives[i] |= 0xFFFF;
    gCurTask->playerLifeRequestCursor = 0;
    gCurTask->playerLifeGiverMask = 0;
    PlayerLifeRequestDrawGiverList();
}
void PlayerLifeRequestStartAsking(void)
{
    struct Unk02005E00 *s;

    s = &gLifeRequests;
    if (s->timeout == 0)
        s->timeout = 1200;
    PlayerLifeRequestLoadGfx(1);
    PlayerLifeRequestDrawAsking(gCurTask->playerLifeGiver);
}
void PlayerLifeRequestTakeLife(void)
{
    AddPlayerLives(-1, gCurTask->playerLifeGiver);
    AddPlayerLives(1, gCurTask->player->playerIndex);
    PlayerLifeRequestClear();
    PlayerLifeRequestDrawBorrowed(gCurTask->playerLifeGiver);
}
void PlayerLifeRequestFinish(void)
{
    if (gLocalPlayer == gCurTask->player->playerIndex)
        HudRedraw(gLocalPlayer);
    TaskExitTrampoline();
}
void PlayerLifeRequestStartFail(void)
{
    PlayerLifeRequestClear();
    PlayerLifeRequestDrawCannotBorrow();
}
void PlayerLifeRequestStartNoGiver(void)
{
    PlayerLifeRequestLoadGfx(2);
    PlayerLifeRequestDrawGotNothing();
}
void PlayerLifeRequestShowGameOver(void)
{
    struct Unk02005E00 *s;
    vs32 *ip;
    u8 *g;

    PlayerLifeRequestLoadGfx(2);
    PlayerLifeRequestDrawGameOver();
    s = &gLifeRequests;
    ip = &gCurTaskIdx;
    g = (u8 *)s + 8;
    g[*ip] = 1;
}
void PlayerLifeRequestMoveMenuCursor(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    e = &k[t->player->playerIndex];
    if ((*e & 0xC0) != 0)
    {
        if ((*e & 0x40) != 0)
        {
            if (t->playerLifeRequestCursor == 1)
            {
                t->playerLifeRequestCursor = 0;
                PlayerLifeRequestDrawMenu(0);
            }
        }
        else if (t->playerLifeRequestCursor == 0)
        {
            t->playerLifeRequestCursor = 1;
            PlayerLifeRequestDrawMenu(1);
        }
    }
}
void PlayerLifeRequestSelectChoice(void)
{
    vu16 *k;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    if ((k[t->player->playerIndex] & 1) != 0)
    {
        if (t->playerLifeRequestCursor == 0)
        {
            if (PlayerLifeRequestCountGivers() != 0)
                gCurTask->state = 1;
            else
                gCurTask->state = 5;
        }
        else
        {
            t->state = 6;
        }
        TaskSetEntry(PlayerLifeRequestEnterState, gCurTaskIdx);
    }
}
void PlayerLifeRequestMoveListCursor(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;

    if (gCurTask->playerLifeRequestCursor > gCurTask->playerLifeGiverLastRow)
    {
        do
        {
            gCurTask->playerLifeRequestCursor--;
            PlayerLifeRequestDrawListCursor(gCurTask->playerLifeRequestCursor);
        } while (gCurTask->playerLifeRequestCursor > gCurTask->playerLifeGiverLastRow);
    }
    k = gPlayerPressedKeys;
    t = gCurTask;
    e = &k[t->player->playerIndex];
    if ((*e & 0xC0) != 0)
    {
        if ((*e & 0x40) != 0)
        {
            if (t->playerLifeRequestCursor != 0)
            {
                t->playerLifeRequestCursor--;
                PlayerLifeRequestDrawListCursor(t->playerLifeRequestCursor);
            }
        }
        else if (t->playerLifeRequestCursor < t->playerLifeGiverLastRow)
        {
            t->playerLifeRequestCursor++;
            PlayerLifeRequestDrawListCursor(t->playerLifeRequestCursor);
        }
    }
}
void PlayerLifeRequestSelectGiver(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;
    s32 i;
    s32 j;

    k = gPlayerPressedKeys;
    t = gCurTask;
    e = &k[t->player->playerIndex];
    if ((*e & 11) != 0)
    {
        if ((*e & 1) != 0)
        {
            for (i = 0, j = 0; i < gPlayerCount; i++)
            {
                if (i == gCurTaskIdx)
                    continue;
                if ((1 & (gCurTask->playerLifeGiverMask >> i)) == 0)
                    continue;
                if (j == gCurTask->playerLifeRequestCursor)
                {
                    gCurTask->playerLifeGiver = i;
                    break;
                }
                j++;
            }
            gLifeRequests.requests[gCurTask->playerLifeGiver] = (gCurTaskIdx << 4) | 1;
            gCurTask->state = 2;
        }
        else
        {
            t->state = 0;
        }
    }
}
void PlayerLifeRequestReceiveCheckPress(void)
{
    vu16 *k;
    s32 p;

    k = gPlayerPressedKeys;
    p = gCurTask->player->playerIndex;
    if ((k[p] & 1) != 0)
    {
        if (gLocalPlayer == p)
            HudRedraw(p);
        TaskFree(gCurTaskIdx);
    }
}
void PlayerLifeRequestFailCheckPress(void)
{
    vu16 *k;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    if ((k[t->player->playerIndex] & 1) != 0)
        t->state = 0;
    if (gCurTask->state != 4)
        TaskSetEntry(PlayerLifeRequestEnterState, gCurTaskIdx);
}
void PlayerLifeRequestNoGiverCheckPress(void)
{
    vu16 *k;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    if ((k[t->player->playerIndex] & 1) != 0)
        t->state = 0;
    if (gCurTask->state != 5)
        TaskSetEntry(PlayerLifeRequestEnterState, gCurTaskIdx);
}
s32 PlayerLifeRequestCountGivers(void)
{
    s32 i;
    s32 n;
    u8 *f;

    gCurTask->playerLifeGiverMask = 0;
    for (i = 0, n = 0; i < gPlayerCount; i++)
    {
        if ((1 & (gActivePlayerMask >> i)) != 0 && gPlayerLives[i] > 0 && gLifeRequests.requests[i] == 0)
        {
            n++;
            gCurTask->playerLifeGiverMask |= 1 << i;
        }
    }
    gCurTask->playerLifeGiverLastRow = n - 1;
    return n;
}
void PlayerLifeRequestRefreshGiverList(s32 a, s32 b)
{
    struct Task *t;
    s32 i;
    s32 j;
    s32 v;
    u16 u;
    u8 *f;
    s16 *e;
    s16 *q;

    t = gCurTask;
    if (a != t->playerLifeGiverLastRow || b != t->playerLifeGiverMask)
    {
        PlayerLifeRequestDrawGiverList();
    }
    else
    {
        for (i = 0, j = 0; i < gPlayerCount; i++)
        {
            if ((1 & (gActivePlayerMask >> i)) != 0 && gLifeRequests.requests[i] == 0)
            {
                e = &gPlayerLives[i];
                u = *e;
                v = *e;
                if (v > 0)
                {
                    if (((gCurTask->playerLifeGiverMask >> i) & 1) != 0)
                    {
                        q = (s16 *)&gLifeRequestShownLives[i];
                        if (*q != v)
                            *q = u;
                    }
                    PlayerLifeRequestDrawLives(i, j, gPlayerLives[i]);
                    j++;
                }
            }
        }
    }
}
void PlayerLifeRequestCheckGiven(void)
{
    struct Task *t;
    u8 v;
    u8 *p;

    p = (u8 *)&gLifeRequests;
    t = gCurTask;
    p += 4;
    v = p[t->playerLifeGiver];
    if ((v & 2) != 0)
    {
        if ((v >> 4) == gCurTaskIdx)
            t->state = 3;
    }
}
void PlayerLifeRequestCheckTimeout(void)
{
    if (gLifeRequests.timeout <= 0)
        gCurTask->state = 4;
    gLifeRequests.timeout--;
    if (gCurTask->state != 2)
        TaskSetEntry(PlayerLifeRequestEnterState, gCurTaskIdx);
}
void PlayerLifeRequestCheckGiverLives(void)
{
    struct Task *t;
    s16 *p;

    p = gPlayerLives;
    t = gCurTask;
    if (p[t->playerLifeGiver] <= 0)
        t->state = 4;
}
void PlayerLifeRequestInit(void)
{
    gCurTask->updateCallback = (u32)PlayerLifeRequestUpdate;
    PlayerLifeRequestPickStartState();
    CallTableEntry(gCurTask->state, 7, gPlayerLifeRequestStates);
}
void PlayerLifeRequestUpdate(void)
{
    CallTableEntry(gCurTask->updateState, 7, gPlayerLifeRequestStateUpdates);
}
void PlayerLifeRequestEnterState(void)
{
    CallTableEntry(gCurTask->state, 7, gPlayerLifeRequestStates);
}
void PlayerLifeRequestChoose(void)
{
    gCurTask->updateState = 0;
    PlayerLifeRequestOpenMenu();
    TaskSleepForever();
}
void PlayerLifeRequestChooseUpdate(void)
{
    PlayerLifeRequestMoveMenuCursor();
    PlayerLifeRequestSelectChoice();
}
void PlayerLifeRequestPickGiver(void)
{
    gCurTask->updateState = 1;
    PlayerLifeRequestOpenGiverList();
    TaskSleepForever();
}
void PlayerLifeRequestPickGiverUpdate(void)
{
    s32 a;
    s32 b;

    a = gCurTask->playerLifeGiverLastRow;
    b = gCurTask->playerLifeGiverMask;
    if (PlayerLifeRequestCountGivers() != 0)
    {
        PlayerLifeRequestRefreshGiverList(a, b);
        PlayerLifeRequestMoveListCursor();
        PlayerLifeRequestSelectGiver();
    }
    else
    {
        gCurTask->state = 5;
    }
    if (gCurTask->state != 1)
        TaskSetEntry(PlayerLifeRequestEnterState, gCurTaskIdx);
}
void PlayerLifeRequestWait(void)
{
    gCurTask->updateState = 2;
    PlayerLifeRequestStartAsking();
    while (1)
    {
        TaskYieldTrampoline(4);
        PlayerLifeRequestCheckGiverLives();
    }
}
void PlayerLifeRequestWaitUpdate(void)
{
    PlayerLifeRequestCheckGiven();
    PlayerLifeRequestCheckTimeout();
}
void PlayerLifeRequestReceive(void)
{
    gCurTask->updateState = 3;
    PlayerLifeRequestTakeLife();
    TaskYieldTrampoline(180);
    PlayerLifeRequestFinish();
    TaskSleepForever();
}
void PlayerLifeRequestReceiveUpdate(void)
{
    PlayerLifeRequestReceiveCheckPress();
}
void PlayerLifeRequestFail(void)
{
    gCurTask->updateState = 4;
    PlayerLifeRequestStartFail();
    TaskYieldTrampoline(180);
    gCurTask->state = 0;
    TaskSleepForever();
}
void PlayerLifeRequestFailUpdate(void)
{
    PlayerLifeRequestFailCheckPress();
}
void PlayerLifeRequestNoGiver(void)
{
    gCurTask->updateState = 5;
    PlayerLifeRequestStartNoGiver();
    TaskYieldTrampoline(300);
    gCurTask->state = 0;
    TaskSleepForever();
}
void PlayerLifeRequestNoGiverUpdate(void)
{
    PlayerLifeRequestNoGiverCheckPress();
}
void PlayerLifeRequestGameOver(void)
{
    gCurTask->updateState = 6;
    PlayerLifeRequestShowGameOver();
    TaskSleepForever();
}
void PlayerLifeRequestGameOverUpdate(void)
{
}
void PlayerLifeRequestDrawMenu(s32 a)
{
    u8 *p;
    u8 *q;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        if (a == 0)
        {
            p = gUnk_085B09BC;
            q = gUnk_085B09DC;
        }
        else
        {
            p = gUnk_085B0A10;
            q = gUnk_085B0A30;
        }
        HudDrawTiles(p, 1, 2, 8);
        HudDrawTiles(p + 16, 1, 3, 8);
        HudDrawTiles(q, 1, 4, 13);
        HudDrawTiles(q + 26, 1, 5, 13);
    }
}
void PlayerLifeRequestDrawListTitle(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        p = gUnk_085B0A64;
        HudDrawTiles(p, 1, 2, 19);
        p += 38;
        HudDrawTiles(p, 1, 3, 19);
    }
}
void PlayerLifeRequestDrawGiverList(void)
{
    s32 i;
    s32 j;
    s16 *e;
    s16 *pe;
    u8 *f;
    u16 v;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        HudClearTilemap();
        for (i = 0, j = 0; i < gPlayerCount; i++)
        {
            f = gLifeRequests.requests;
            if (((gActivePlayerMask >> i) & 1) != 0)
            {
                pe = gPlayerLives;
                e = &pe[i];
                v = *e;
                if (*e > 0 && f[i] == 0)
                {
                    gLifeRequestShownLives[i] = v;
                    PlayerLifeRequestDrawGiverIcon(i, j);
                    PlayerLifeRequestDrawLives(i, j, *e);
                    j++;
                }
            }
        }
        PlayerLifeRequestDrawListTitle();
        PlayerLifeRequestDrawListCursor(gCurTask->playerLifeRequestCursor);
    }
}
void PlayerLifeRequestDrawGiverIcon(s32 a, s32 b)
{
    u8 *p;
    u8 *q;

    s32 off;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        off = a * 4;
        p = gUnk_085B0AB0;
        HudDrawTiles(p + off, 3, b * 2 + 4, 2);
        p += 16;
        HudDrawTiles(p + off, 3, b * 2 + 5, 2);
        q = gUnk_085B0AD0;
        HudDrawTiles(q, 5, b * 2 + 4, 1);
        q += 2;
        HudDrawTiles(q, 5, b * 2 + 5, 1);
    }
}
void PlayerLifeRequestDrawLives(s32 a, s32 b, s32 c)
{
    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        IntToDigits(c);
        HudDrawTiles(&gUnk_085B0AD4[gDigits[1] * 2], 6, b * 2 + 4, 1);
        HudDrawTiles(&gUnk_085B0AD4[20 + gDigits[1] * 2], 6, b * 2 + 5, 1);
        HudDrawTiles(&gUnk_085B0AD4[gDigits[0] * 2], 7, b * 2 + 4, 1);
        HudDrawTiles(&gUnk_085B0AD4[20 + gDigits[0] * 2], 7, b * 2 + 5, 1);
    }
}
void PlayerLifeRequestDrawListCursor(s32 a)
{
    s32 i;
    u8 *p;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        for (i = 0; i < gPlayerCount; i++)
        {
            HudClearTiles(2, i * 2 + 4, 1);
            HudClearTiles(2, i * 2 + 5, 1);
        }
        p = gUnk_085B0AFC;
        HudDrawTiles(p, 2, a * 2 + 4, 1);
        p += 2;
        HudDrawTiles(p, 2, a * 2 + 5, 1);
    }
}
void PlayerLifeRequestDrawAsking(s32 a)
{
    u8 *p;
    u8 *q;
    u8 *r;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        HudClearTilemap();
        p = gUnk_085B0B00;
        HudDrawTiles(p, 13, 6, 2);
        p += 4;
        HudDrawTiles(p, 13, 7, 2);
        a *= 4;
        q = gUnk_085B0BB4;
        HudDrawTiles(q + a, 16, 8, 2);
        q += 16;
        HudDrawTiles(q + a, 16, 9, 2);
        r = gUnk_085B0B10;
        HudDrawTiles(r, 10, 8, 6);
        r += 12;
        HudDrawTiles(r, 10, 9, 6);
    }
}
void PlayerLifeRequestDrawBorrowed(s32 a)
{
    u8 *p;
    u8 *q;
    u8 *r;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        HudClearTilemap();
        p = gUnk_085B0B08;
        HudDrawTiles(p, 13, 6, 2);
        p += 4;
        HudDrawTiles(p, 13, 7, 2);
        a *= 4;
        q = gUnk_085B0BB4;
        HudDrawTiles(q + a, 19, 8, 2);
        q += 16;
        HudDrawTiles(q + a, 19, 9, 2);
        r = gUnk_085B0B28;
        HudDrawTiles(r, 6, 8, 13);
        r += 26;
        HudDrawTiles(r, 6, 9, 13);
    }
}
void PlayerLifeRequestDrawCannotBorrow(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        HudClearTilemap();
        p = gUnk_085B0B5C;
        HudDrawTiles(p, 4, 8, 22);
        p += 44;
        HudDrawTiles(p, 4, 9, 22);
    }
}
void PlayerLifeRequestDrawGotNothing(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        HudClearTilemap();
        p = gUnk_085B0BD4;
        HudDrawTiles(p, 4, 8, 22);
        p += 44;
        HudDrawTiles(p, 4, 9, 22);
    }
}
void PlayerLifeRequestDrawGameOver(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->player->playerIndex)
    {
        HudClearTilemap();
        p = gUnk_085B0C2C;
        HudDrawTiles(p, 8, 8, 14);
        p += 28;
        HudDrawTiles(p, 8, 9, 14);
    }
}
