#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern void (*gFrameCallback)(void);
extern u8 gHBlankScrollStarted;
extern u8 gHBlankScrollState;
extern s32 gHBlankScrollTimer;
extern u16 gHBlankScrollEffect;

void UpdateHBlankScroll(void);
void UpdateRoomHBlankScroll(void);

void ResetHBlankScroll(void)
{
    gFrameCallback = NULL;
    gHBlankScrollTimer = 0;
    REG_DMA0CNT_H = 0;
}
void StopHBlankScroll(void)
{
    gHBlankScrollState = 2;
}
void StartHBlankScroll(s32 a)
{
    gHBlankScrollTimer = 0;
    gHBlankScrollState = 1;
    gHBlankScrollEffect = a;
    gFrameCallback = UpdateHBlankScroll;
    gHBlankScrollStarted = 1;
}
void StartRoomHBlankScroll(s32 a)
{
    gHBlankScrollTimer = 0;
    gHBlankScrollState = 1;
    gHBlankScrollEffect = a;
    gFrameCallback = UpdateRoomHBlankScroll;
    gHBlankScrollStarted = 1;
}
void HoldHBlankScroll(void)
{
    if (gHBlankScrollStarted != 0)
        gHBlankScrollState = 3;
}
void SuspendHBlankScroll(void)
{
    if (gHBlankScrollStarted != 0)
        gFrameCallback = NULL;
}
void RestoreRoomHBlankScroll(void)
{
    if (gHBlankScrollStarted != 0)
        gFrameCallback = UpdateRoomHBlankScroll;
}
void ResumeHBlankScroll(void)
{
    if (gHBlankScrollStarted != 0)
        gHBlankScrollState = 1;
}
