#include "gba/gba.h"
#include "gba/m4a_internal.h"

/* m4a/mp2k sound engine, C driver part 2 (issue #54):
 * 0x080CEFB4-0x080CF587 — CGB/PSG side: MidiKeyToCgbFreq, CgbOscOff,
 * CgbModVol, CgbSound, plus the unreferenced m4aMPlayTempoControl the
 * linker kept at 0x080CF554.
 * SDK library code: old_agbcc -O2 -mthumb-interwork (same recipe as
 * part 1; verify with
 * `./tools/fnmatch.sh 0x080CEFB4 0x080CF588 src/m4a_cgb.c --old2`).
 * Source shapes follow pokeruby's src/libs/m4a.c generation; this SDK
 * revision's CgbModVol has no soundInfo->mode bit-0 check yet. */

u32 MidiKeyToCgbFreq(u8 chanNum, u8 key, u8 fineAdjust)
{
    if (chanNum == 4)
    {
        if (key <= 20)
        {
            key = 0;
        }
        else
        {
            key -= 21;
            if (key > 59)
                key = 59;
        }

        return gNoiseTable[key];
    }
    else
    {
        s32 val1;
        s32 val2;

        if (key <= 35)
        {
            fineAdjust = 0;
            key = 0;
        }
        else
        {
            key -= 36;
            if (key > 130)
            {
                key = 130;
                fineAdjust = 255;
            }
        }

        val1 = gCgbScaleTable[key];
        val1 = gCgbFreqTable[val1 & 0xF] >> (val1 >> 4);

        val2 = gCgbScaleTable[key + 1];
        val2 = gCgbFreqTable[val2 & 0xF] >> (val2 >> 4);

        return val1 + ((fineAdjust * (val2 - val1)) >> 8) + 2048;
    }
}

void CgbOscOff(u8 chanNum)
{
    switch (chanNum)
    {
    case 1:
        REG_NR12 = 8;
        REG_NR14 = 0x80;
        break;
    case 2:
        REG_NR22 = 8;
        REG_NR24 = 0x80;
        break;
    case 3:
        REG_NR30 = 0;
        break;
    default:
        REG_NR42 = 8;
        REG_NR44 = 0x80;
    }
}

static inline int CgbPan(struct CgbChannel *chan)
{
    u32 rightVolume = chan->rightVolume;
    u32 leftVolume = chan->leftVolume;

    if ((rightVolume = (u8)rightVolume) >= (leftVolume = (u8)leftVolume))
    {
        if (rightVolume / 2 >= leftVolume)
        {
            chan->pan = 0x0F;
            return 1;
        }
    }
    else
    {
        if (leftVolume / 2 >= rightVolume)
        {
            chan->pan = 0xF0;
            return 1;
        }
    }

    return 0;
}

void CgbModVol(struct CgbChannel *chan)
{
    if (!CgbPan(chan))
    {
        chan->pan = 0xFF;
        chan->eg = (u32)(chan->rightVolume + chan->leftVolume) >> 4;
    }
    else
    {
        chan->eg = (u32)(chan->rightVolume + chan->leftVolume) >> 4;
        if (chan->eg > 15)
            chan->eg = 15;
    }

    chan->sg = (chan->eg * chan->su + 15) >> 4;
    chan->pan &= chan->panMask;
}

void CgbSound(void)
{
    s32 ch;
    struct CgbChannel *channels;
    s32 prevC15;
    struct SoundInfo *soundInfo = SOUND_INFO_PTR;
    vu8 *nrx0ptr;
    vu8 *nrx1ptr;
    vu8 *nrx2ptr;
    vu8 *nrx3ptr;
    vu8 *nrx4ptr;
    s32 envelopeStepTimeAndDir;

    /* Most comparison operations that cast to s8 perform 'and' by 0xFF. */
    int mask = 0xff;

    /* Use the periodic counter for the master volume. */
    if (soundInfo->c15)
        soundInfo->c15--;
    else
        soundInfo->c15 = 14;

    for (ch = 1, channels = soundInfo->cgbChans; ch <= 4; ch++, channels++)
    {
        if (!(channels->sf & SOUND_CHANNEL_SF_ON))
            continue;

        /* 1. determine hardware channel registers */
        switch (ch)
        {
        case 1:
            nrx0ptr = (vu8 *)REG_ADDR_NR10;
            nrx1ptr = (vu8 *)REG_ADDR_NR11;
            nrx2ptr = (vu8 *)REG_ADDR_NR12;
            nrx3ptr = (vu8 *)REG_ADDR_NR13;
            nrx4ptr = (vu8 *)REG_ADDR_NR14;
            break;
        case 2:
            nrx0ptr = (vu8 *)(REG_ADDR_NR10 + 1);
            nrx1ptr = (vu8 *)REG_ADDR_NR21;
            nrx2ptr = (vu8 *)REG_ADDR_NR22;
            nrx3ptr = (vu8 *)REG_ADDR_NR23;
            nrx4ptr = (vu8 *)REG_ADDR_NR24;
            break;
        case 3:
            nrx0ptr = (vu8 *)REG_ADDR_NR30;
            nrx1ptr = (vu8 *)REG_ADDR_NR31;
            nrx2ptr = (vu8 *)REG_ADDR_NR32;
            nrx3ptr = (vu8 *)REG_ADDR_NR33;
            nrx4ptr = (vu8 *)REG_ADDR_NR34;
            break;
        default:
            nrx0ptr = (vu8 *)(REG_ADDR_NR30 + 1);
            nrx1ptr = (vu8 *)REG_ADDR_NR41;
            nrx2ptr = (vu8 *)REG_ADDR_NR42;
            nrx3ptr = (vu8 *)REG_ADDR_NR43;
            nrx4ptr = (vu8 *)REG_ADDR_NR44;
            break;
        }

        prevC15 = soundInfo->c15;
        envelopeStepTimeAndDir = *nrx2ptr;

        /* 2. calculate envelope volume */
        if (channels->sf & SOUND_CHANNEL_SF_START)
        {
            if (!(channels->sf & SOUND_CHANNEL_SF_STOP))
            {
                channels->sf = SOUND_CHANNEL_SF_ENV_ATTACK;
                channels->mo = CGB_CHANNEL_MO_PIT | CGB_CHANNEL_MO_VOL;
                CgbModVol(channels);
                switch (ch)
                {
                case 1:
                    *nrx0ptr = channels->sw;
                    /* fallthrough */
                case 2:
                    *nrx1ptr = ((u32)channels->wp << 6) + channels->le;
                    goto init_env_step_time_dir;
                case 3:
                    if (channels->wp != (u32 *)channels->cp)
                    {
                        *nrx0ptr = 0x40;
                        REG_WAVE_RAM0 = channels->wp[0];
                        REG_WAVE_RAM1 = channels->wp[1];
                        REG_WAVE_RAM2 = channels->wp[2];
                        REG_WAVE_RAM3 = channels->wp[3];
                        channels->cp = (u32)channels->wp;
                    }
                    *nrx0ptr = 0;
                    *nrx1ptr = channels->le;
                    if (channels->le)
                        channels->n4 = 0xC0;
                    else
                        channels->n4 = 0x80;
                    break;
                default:
                    *nrx1ptr = channels->le;
                    *nrx3ptr = (u32)channels->wp << 3;
                init_env_step_time_dir:
                    envelopeStepTimeAndDir = channels->at + CGB_NRx2_ENV_DIR_INC;
                    if (channels->le)
                        channels->n4 = 0x40;
                    else
                        channels->n4 = 0x00;
                    break;
                }
                channels->ec = channels->at;
                if ((s8)(channels->at & mask))
                {
                    channels->ev = 0;
                    goto envelope_step_complete;
                }
                else
                {
                    /* skip attack phase if attack is instantaneous (=0) */
                    goto envelope_decay_start;
                }
            }
            else
            {
                goto oscillator_off;
            }
        }
        else if (channels->sf & SOUND_CHANNEL_SF_IEC)
        {
            channels->echoLength--;
            if ((s8)(channels->echoLength & mask) <= 0)
            {
            oscillator_off:
                CgbOscOff(ch);
                channels->sf = 0;
                goto channel_complete;
            }
            goto envelope_complete;
        }
        else if ((channels->sf & SOUND_CHANNEL_SF_STOP) && (channels->sf & SOUND_CHANNEL_SF_ENV))
        {
            channels->sf &= ~SOUND_CHANNEL_SF_ENV;
            channels->ec = channels->re;
            if ((s8)(channels->re & mask))
            {
                channels->mo |= CGB_CHANNEL_MO_VOL;
                if (ch != 3)
                    envelopeStepTimeAndDir = channels->re | CGB_NRx2_ENV_DIR_DEC;
                goto envelope_step_complete;
            }
            else
            {
                goto envelope_pseudoecho_start;
            }
        }
        else
        {
        envelope_step_repeat:
            if (channels->ec == 0)
            {
                if (ch == 3)
                    channels->mo |= CGB_CHANNEL_MO_VOL;

                CgbModVol(channels);
                if ((channels->sf & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_RELEASE)
                {
                    channels->ev--;
                    if ((s8)(channels->ev & mask) <= 0)
                    {
                    envelope_pseudoecho_start:
                        channels->ev = ((channels->eg * channels->echoVolume) + 0xFF) >> 8;
                        if (channels->ev)
                        {
                            channels->sf |= SOUND_CHANNEL_SF_IEC;
                            channels->mo |= CGB_CHANNEL_MO_VOL;
                            if (ch != 3)
                                envelopeStepTimeAndDir = 0 | CGB_NRx2_ENV_DIR_INC;
                            goto envelope_complete;
                        }
                        else
                        {
                            goto oscillator_off;
                        }
                    }
                    else
                    {
                        channels->ec = channels->re;
                    }
                }
                else if ((channels->sf & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_SUSTAIN)
                {
                envelope_sustain:
                    channels->ev = channels->sg;
                    channels->ec = 7;
                }
                else if ((channels->sf & SOUND_CHANNEL_SF_ENV) == SOUND_CHANNEL_SF_ENV_DECAY)
                {
                    int envelopeVolume, sustainGoal;

                    channels->ev--;
                    envelopeVolume = (s8)(channels->ev & mask);
                    sustainGoal = (s8)(channels->sg);
                    if (envelopeVolume <= sustainGoal)
                    {
                    envelope_sustain_start:
                        if (channels->su == 0)
                        {
                            channels->sf &= ~SOUND_CHANNEL_SF_ENV;
                            goto envelope_pseudoecho_start;
                        }
                        else
                        {
                            channels->sf--;
                            channels->mo |= CGB_CHANNEL_MO_VOL;
                            if (ch != 3)
                                envelopeStepTimeAndDir = 0 | CGB_NRx2_ENV_DIR_INC;
                            goto envelope_sustain;
                        }
                    }
                    else
                    {
                        channels->ec = channels->de;
                    }
                }
                else
                {
                    channels->ev++;
                    if ((u8)(channels->ev & mask) >= channels->eg)
                    {
                    envelope_decay_start:
                        channels->sf--;
                        channels->ec = channels->de;
                        if ((u8)(channels->ec & mask))
                        {
                            channels->mo |= CGB_CHANNEL_MO_VOL;
                            channels->ev = channels->eg;
                            if (ch != 3)
                                envelopeStepTimeAndDir = channels->de | CGB_NRx2_ENV_DIR_DEC;
                        }
                        else
                        {
                            goto envelope_sustain_start;
                        }
                    }
                    else
                    {
                        channels->ec = channels->at;
                    }
                }
            }
        }

    envelope_step_complete:
        /* every 15 frames, envelope calculation has to be done twice
         * to keep up with the hardware envelope rate (1/64 s) */
        channels->ec--;
        if (prevC15 == 0)
        {
            prevC15--;
            goto envelope_step_repeat;
        }

    envelope_complete:
        /* 3. apply pitch to HW registers */
        if (channels->mo & CGB_CHANNEL_MO_PIT)
        {
            if (ch < 4 && (channels->ty & TONEDATA_TYPE_FIX))
            {
                int dac_pwm_rate = REG_SOUNDBIAS_H;

                if (dac_pwm_rate < 0x40)        /* if PWM rate = 32768 Hz */
                    channels->fr = (channels->fr + 2) & 0x7fc;
                else if (dac_pwm_rate < 0x80)   /* if PWM rate = 65536 Hz */
                    channels->fr = (channels->fr + 1) & 0x7fe;
            }

            if (ch != 4)
                *nrx3ptr = channels->fr;
            else
                *nrx3ptr = (*nrx3ptr & 0x08) | channels->fr;
            channels->n4 = (channels->n4 & 0xC0) + (*((u8 *)&channels->fr + 1));
            *nrx4ptr = (s8)(channels->n4 & mask);
        }

        /* 4. apply envelope & volume to HW registers */
        if (channels->mo & CGB_CHANNEL_MO_VOL)
        {
            REG_NR51 = (REG_NR51 & ~channels->panMask) | channels->pan;
            if (ch == 3)
            {
                *nrx2ptr = gCgb3Vol[channels->ev];
                if (channels->n4 & 0x80)
                {
                    *nrx0ptr = 0x80;
                    *nrx4ptr = channels->n4;
                    channels->n4 &= 0x7f;
                }
            }
            else
            {
                envelopeStepTimeAndDir &= 0xf;
                *nrx2ptr = (channels->ev << 4) + envelopeStepTimeAndDir;
                *nrx4ptr = channels->n4 | 0x80;
                if (ch == 1 && !(*nrx0ptr & 0x08))
                    *nrx4ptr = channels->n4 | 0x80;
            }
        }

    channel_complete:
        channels->mo = 0;
    }
}

/* Unreferenced in this game, but linked between CgbSound and
 * m4aMPlayVolumeControl.  The ident lock write pair is in the source as
 * in pokeruby's m4a.c; old_agbcc -O2 optimizes the two stores away. */
void m4aMPlayTempoControl(struct MusicPlayerInfo *mplayInfo, u16 tempo)
{
    if (mplayInfo->ident == ID_NUMBER)
    {
        mplayInfo->ident++;
        mplayInfo->tempoU = tempo;
        mplayInfo->tempoI = (mplayInfo->tempoD * mplayInfo->tempoU) >> 8;
        mplayInfo->ident = ID_NUMBER;
    }
}
