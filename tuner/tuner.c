#include "tuner.h"

#include "../eemul.h"

static Tuner tuner;

static void tunerReadSettings(void)
{
    tuner.ic = eeReadU(EE_TUNER_IC, TUNER_IC_RDA5807);

    tuner.par.flags = eeReadU(EE_TUNER_FLAGS, TUNER_FLAG_INIT);
    tuner.par.band = eeReadU(EE_TUNER_BAND, TUNER_BAND_FM_US_EUROPE);
    tuner.par.step = eeReadU(EE_TUNER_STEP, TUNER_STEP_100K);
    tuner.par.deemph = eeReadU(EE_TUNER_DEEMPH, TUNER_DEEMPH_50u);
    tuner.par.volume = eeReadI(EE_TUNER_VOLUME, TUNER_VOLUME_MAX);

    tuner.freq = eeReadU(EE_TUNER_FREQ, 9950);
}

static void tunerSaveSettings(void)
{
    eeUpdate(EE_TUNER_FLAGS, tuner.par.flags & (~TUNER_FLAG_MUTE));
    eeUpdate(EE_TUNER_FREQ, tuner.freq);
}

void tunerInit()
{
    tunerReadSettings();

    switch (tuner.ic) {
#ifdef _RDA580X
    case TUNER_IC_RDA5807:
        tuner.api.setFreq = rda580xSetFreq;
        tuner.api.seek = rda580xSeek;

        tuner.api.setVolume = rda580xSetVolume;

        tuner.api.setMute = rda580xSetMute;
        tuner.api.setBassBoost = rda580xSetBassBoost;
        tuner.api.setForcedMono = rda580xSetForcedMono;
        tuner.api.setRds = rda580xSetRds;

        tuner.api.setPower = rda580xSetPower;

        tuner.api.updateStatus = rda580xUpdateStatus;

        tuner.api.getFreq = rda580xGetFreq;

        rda580xInit(&tuner.par);

        break;
#endif
#ifdef _SI470X
    case TUNER_IC_SI4703:
        si470xInit();
        break;
#endif
    default:
        break;
    }
}

Tuner *tunerGet(void)
{
    return &tuner;
}

TunerParam *tunerGetPar(void)
{
    return &tuner.par;
}

void tunerSetPower(uint16_t value)
{
    if (value) {

    } else {
        tunerSaveSettings();
    }

    if (tuner.api.setPower) {
        tuner.api.setPower(value);
    }
}

void tunerSetFreq(uint16_t value)
{
    uint16_t freqMin = tuner.par.fMin;
    uint16_t freqMax = tuner.par.fMax;

    if (value < freqMin)
        value = freqMin;
    else if (value > freqMax)
        value = freqMax;

    tuner.freq = value;

    if (tuner.api.setFreq) {
        tuner.api.setFreq(value);
    }
}

void tunerSetFlag(TunerFlag flag, uint8_t value)
{
    if (value)
        tuner.par.flags |= flag;
    else
        tuner.par.flags &= ~flag;

    switch (flag) {
    case TUNER_FLAG_MUTE:
        if (tuner.api.setMute) {
            tuner.api.setMute(value);
        }
        break;
    case TUNER_FLAG_BASS:
        if (tuner.api.setBassBoost) {
            tuner.api.setBassBoost(value);
        }
        break;
    case TUNER_FLAG_MONO:
        if (tuner.api.setForcedMono) {
            tuner.api.setForcedMono(value);
        }
        break;
    case TUNER_FLAG_RDS:
        if (tuner.api.setRds) {
            tuner.api.setRds(value);
        }
        break;
    default:
        break;
    }
}

void tunerNextStation(int8_t direction)
{
    // Temporarly
    if (tuner.api.seek) {
        tuner.api.seek(direction);
    }
}

void tunerUpdateStatus(void)
{
    if (tuner.api.updateStatus) {
        tuner.api.updateStatus();

        if (tuner.api.getFreq) {
            tuner.freq = tuner.api.getFreq();
        }
    }
}
