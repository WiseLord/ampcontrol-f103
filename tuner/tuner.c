#include "tuner.h"

#include "../eemul.h"

static Tuner tuner;

static void tunerReadSettings(void)
{
    uint16_t eeData;

    eeData = eeRead(EE_TUNER_IC);
    tuner.ic = (eeData == EE_EMPTY ? TUNER_IC_RDA5807 : eeData);

    eeData = eeRead(EE_TUNER_FREQ);
    tuner.freq = (eeData == EE_EMPTY ? 9950 : eeData);

    eeData = eeRead(EE_TUNER_FLAGS);
    tuner.flags = (eeData == EE_EMPTY ? TUNER_FLAG_INIT : eeData);

    tuner.freqMin = 8700;
    tuner.freqMax = 10800;
    tuner.ic = TUNER_IC_RDA5807;
}

static void tunerSaveSettings(void)
{
    eeUpdate(EE_TUNER_FREQ, tuner.freq);
    eeUpdate(EE_TUNER_FLAGS, tuner.flags);
}

void tunerInit()
{
    tunerReadSettings();

    switch (tuner.ic) {
#ifdef _RDA580X
    case TUNER_IC_RDA5807:
        tuner.api.setFreq = rda580xSetFreq;
        tuner.api.seek = rda580xSeek;

        tuner.api.setMute = rda580xSetMute;
//      tuner.api.setBass = rda580xBass;
//      tuner.api.setStereo = rda580xSetStereo;
//      tuner.api.setRds = rda580xSetRds;

        tuner.api.setPower = rda580xSetPower;

        tuner.api.updateStatus = rda580xUpdateStatus;

        tuner.api.getFreq = rda580xGetFreq;

        rda580xInit();
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
    if (value < tuner.freqMin)
        value = tuner.freqMin;
    else if (value > tuner.freqMax)
        value = tuner.freqMax;

    tuner.freq = value;

    if (tuner.api.setFreq) {
        tuner.api.setFreq(value);
    }
}

void tunerSetFlag(TunerFlag flag, uint8_t value)
{
    if (value)
        tuner.flags |= flag;
    else
        tuner.flags &= ~flag;

    switch (flag) {
    case TUNER_FLAG_MUTE:
        if (tuner.api.setMute) {
            tuner.api.setMute(value);
        }
        break;
    case TUNER_FLAG_BASS:
        if (tuner.api.setBass) {
            tuner.api.setBass(value);
        }
        break;
    case TUNER_FLAG_MONO:
        if (tuner.api.setStereo) {
            tuner.api.setStereo(value);
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
