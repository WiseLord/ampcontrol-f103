#include "tuner.h"

#include <string.h>

#include "../eemap.h"
#include "stations.h"

#ifdef _RDA580X
#include "rda580x.h"
#endif
#ifdef _SI470X
#include "si470x.h"
#endif
#ifdef _TEA5767
#include "tea5767.h"
#endif

static Tuner tuner;

static void tunerTestUpdateStatus(void)
{
    tuner.status.freq = tuner.par.freq;
    tuner.status.rssi = 10;

    switch (tuner.par.step) {
    case TUNER_STEP_50K:
        tuner.par.fStep = 5;
        break;
    case TUNER_STEP_200K:
        tuner.par.fStep = 20;
        break;
    default:
        tuner.par.fStep = 10;
        break;
    }
}

void tunerReadSettings(void)
{
    // Read stored parameters
    memset(&tuner, 0, sizeof(tuner));

    tuner.par.ic = (TunerIC)(eeRead(EE_TUNER_IC));

    tuner.par.band = (TunerBand)(eeRead(EE_TUNER_BAND));
    tuner.par.step = (TunerStep)(eeRead(EE_TUNER_STEP));
    tuner.par.deemph = (TunerDeemph)(eeRead(EE_TUNER_DEEMPH));
    tuner.par.mode = (TunerMode)(eeRead(EE_TUNER_MODE));

    tuner.par.forcedMono = eeRead(EE_TUNER_FMONO);
    tuner.par.rds = eeRead(EE_TUNER_RDS);
    tuner.par.bassBoost = eeRead(EE_TUNER_BASS);

    tuner.par.volume = eeRead(EE_TUNER_VOLUME) & 0x0F;

    tuner.par.freq = (uint16_t)eeRead(EE_TUNER_FREQ);
    tuner.status.freq = tuner.par.freq;

    tuner.par.fMin = 8700;
    tuner.par.fMax = 10800;
    tuner.par.fStep = 10;

    // API initialization
    switch (tuner.par.ic) {
#ifdef _RDA580X
    case TUNER_IC_RDA5807:
        tuner.api.init = rda580xInit;

        tuner.api.setFreq = rda580xSetFreq;
        tuner.api.seek = rda580xSeek;

        tuner.api.setVolume = rda580xSetVolume;

        tuner.api.setMute = rda580xSetMute;
        tuner.api.setBassBoost = rda580xSetBassBoost;
        tuner.api.setForcedMono = rda580xSetForcedMono;
        tuner.api.setRds = rda580xSetRds;

        tuner.api.setPower = rda580xSetPower;

        tuner.api.updateStatus = rda580xUpdateStatus;
        break;
#endif
#ifdef _SI470X
    case TUNER_IC_SI4703:
        tuner.api.init = si470xInit;

        tuner.api.setFreq = si470xSetFreq;
        tuner.api.seek = si470xSeek;

        tuner.api.setVolume = si470xSetVolume;

        tuner.api.setMute = si470xSetMute;
        tuner.api.setForcedMono = si470xSetForcedMono;
        tuner.api.setRds = si470xSetRds;

        tuner.api.setPower = si470xSetPower;

        tuner.api.updateStatus = si470xUpdateStatus;
        break;
#endif
#ifdef _TEA5767
    case TUNER_IC_TEA5767:
        tuner.api.init = tea5767Init;

        tuner.api.setFreq = tea5767SetFreq;
        tuner.api.seek = tea5767Seek;

        tuner.api.setMute = tea5767SetMute;

        tuner.api.setPower = tea5767SetPower;

        tuner.api.updateStatus = tea5767UpdateStatus;
        break;
#endif
    case TUNER_IC_TEST:
        tuner.api.seek = tunerStep;

        tuner.api.updateStatus = tunerTestUpdateStatus;
        break;
    default:
        break;
    }
}

void tunerSaveSettings(void)
{
    eeUpdate(EE_TUNER_FMONO, tuner.par.forcedMono);
    eeUpdate(EE_TUNER_RDS, tuner.par.rds);
    eeUpdate(EE_TUNER_BASS, tuner.par.bassBoost);

    eeUpdate(EE_TUNER_FREQ, (int16_t)tuner.status.freq);
}

void tunerInit(void)
{
    if (tuner.api.init) {
        tuner.api.init(&tuner.par, &tuner.status);
    }
}

Tuner *tunerGet(void)
{
    return &tuner;
}

void tunerSetPower(bool value)
{
    if (!value) {
        tunerSaveSettings();
    }

    if (tuner.api.setPower) {
        tuner.api.setPower(value);
    }
}

void tunerSetFreq(uint16_t value)
{
    const uint16_t freqMin = tuner.par.fMin;
    const uint16_t freqMax = tuner.par.fMax;

    if (value < freqMin) {
        value = freqMin;
    } else if (value > freqMax) {
        value = freqMax;
    }

    tuner.par.freq = value;
    tuner.status.freq = tuner.par.freq;

    if (tuner.api.setFreq) {
        tuner.api.setFreq(value);
    }
}

void tunerSetMute(bool value)
{
    tuner.par.mute = value;

    if (tuner.api.setMute) {
        tuner.api.setMute(value);
    }
}

void tunerSetBassBoost(bool value)
{
    tuner.par.bassBoost = value;

    if (tuner.api.setBassBoost) {
        tuner.api.setBassBoost(value);
    }
}

void tunerSetForcedMono(bool value)
{
    tuner.par.forcedMono = value;

    if (tuner.api.setForcedMono) {
        tuner.api.setForcedMono(value);
    }
}

void tunerSetRds(bool value)
{
    tuner.par.rds = value;

    if (tuner.api.setRds) {
        tuner.api.setRds(value);
    }
}

void tunerSetVolume(int8_t value)
{
    tuner.par.volume = value;

    if (tuner.api.setVolume) {
        tuner.api.setVolume(value);
    }
}

void tunerSeek(int8_t direction)
{
    if (tuner.api.seek) {
        tuner.api.seek(direction);
    }
}

void tunerStep(int8_t direction)
{
    uint16_t freq = tuner.status.freq;
    const uint16_t fMin = tuner.par.fMin;
    const uint16_t fMax = tuner.par.fMax;

    if (direction > 0) {
        if (freq == fMax) {
            freq = fMin;
        } else {
            freq += tuner.par.fStep;
        }
    } else {
        if (freq == fMin) {
            freq = fMax;
        } else {
            freq -= tuner.par.fStep;
        }
    }

    tunerSetFreq(freq);
}

void tunerMove(int8_t direction)
{
    switch (tuner.par.mode) {
    case TUNER_MODE_GRID:
        tunerStep(direction);
        break;
    case TUNER_MODE_STATIONS:
        stationSeek(direction);
        break;
    case TUNER_MODE_SCAN:
        tunerSeek(direction);
        break;
    default:
        break;
    }
}

void tunerUpdateStatus(void)
{
    if (tuner.api.updateStatus) {
        tuner.api.updateStatus();
    }
}
