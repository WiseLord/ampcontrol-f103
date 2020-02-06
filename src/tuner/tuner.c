#include "tuner.h"

#include <string.h>

#include "../settings.h"
#include "rds.h"
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

static const TunerApi tunerTestApi = {
    .seek = tunerStep,
    .updateStatus = tunerTestUpdateStatus,
};

void tunerReadSettings(void)
{
    stationsInit();

    // Read stored parameters
    memset(&tuner, 0, sizeof(tuner));

    tuner.par.ic = (TunerIC)(settingsRead(PARAM_TUNER_IC));

    tuner.par.band = (TunerBand)(settingsRead(PARAM_TUNER_BAND));
    tuner.par.step = (TunerStep)(settingsRead(PARAM_TUNER_STEP));
    tuner.par.deemph = (TunerDeemph)(settingsRead(PARAM_TUNER_DEEMPH));
    tuner.par.mode = (TunerMode)(settingsRead(PARAM_TUNER_MODE));

    tuner.par.forcedMono = settingsRead(PARAM_TUNER_FMONO);
    tuner.par.rds = settingsRead(PARAM_TUNER_RDS);
    tuner.par.bassBoost = settingsRead(PARAM_TUNER_BASS);

    tuner.par.volume = settingsRead(PARAM_TUNER_VOLUME) & 0x0F;

    tuner.par.freq = (uint16_t)settingsRead(PARAM_TUNER_FREQ);
    tuner.status.freq = tuner.par.freq;

    tuner.par.fMin = 8700;
    tuner.par.fMax = 10800;
    tuner.par.fStep = 10;

    // API initialization
    tuner.api = &tunerTestApi;
    switch (tuner.par.ic) {
#ifdef _RDA580X
    case TUNER_IC_RDA5807:
        tuner.api = rda580xGetApi();
        break;
#endif
#ifdef _SI470X
    case TUNER_IC_SI4703:
        tuner.api = si470xGetApi();
        break;
#endif
#ifdef _TEA5767
    case TUNER_IC_TEA5767:
        tuner.api = tea5767GetApi();
        break;
#endif
    default:
        break;
    }
}

void tunerSaveSettings(void)
{
    settingsStore(PARAM_TUNER_FMONO, tuner.par.forcedMono);
    settingsStore(PARAM_TUNER_RDS, tuner.par.rds);
    settingsStore(PARAM_TUNER_BASS, tuner.par.bassBoost);

    settingsStore(PARAM_TUNER_FREQ, (int16_t)tuner.status.freq);
}

void tunerInit(void)
{
    rdsReset();

    if (tuner.api->init) {
        tuner.api->init(&tuner.par, &tuner.status);
    }
}

Tuner *tunerGet(void)
{
    return &tuner;
}

void tunerSetPower(bool value)
{
    rdsReset();

    if (!value) {
        tunerSaveSettings();
    }

    if (tuner.api->setPower) {
        tuner.api->setPower(value);
    }
}

void tunerSetFreq(uint16_t value)
{
    rdsReset();

    const uint16_t freqMin = tuner.par.fMin;
    const uint16_t freqMax = tuner.par.fMax;

    if (value < freqMin) {
        value = freqMin;
    } else if (value > freqMax) {
        value = freqMax;
    }

    tuner.par.freq = value;
    tuner.status.freq = tuner.par.freq;

    if (tuner.api->setFreq) {
        tuner.api->setFreq(value);
    }
}

void tunerSetMute(bool value)
{
    tuner.par.mute = value;

    if (tuner.api->setMute) {
        tuner.api->setMute(value);
    }
}

void tunerSetBassBoost(bool value)
{
    tuner.par.bassBoost = value;

    if (tuner.api->setBassBoost) {
        tuner.api->setBassBoost(value);
    }
}

void tunerSetForcedMono(bool value)
{
    tuner.par.forcedMono = value;

    if (tuner.api->setForcedMono) {
        tuner.api->setForcedMono(value);
    }
}

void tunerSetRds(bool value)
{
    rdsReset();

    tuner.par.rds = value;

    if (tuner.api->setRds) {
        tuner.api->setRds(value);
    }
}

void tunerSetVolume(int8_t value)
{
    tuner.par.volume = value;

    if (tuner.api->setVolume) {
        tuner.api->setVolume(value);
    }
}

void tunerSeek(int8_t direction)
{
    rdsReset();

    if (tuner.api->seek) {
        tuner.api->seek(direction);
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
        if (stationGetCount() > 0) {
            stationSeek(direction);
        } else {
            tunerStep(direction);
        }
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
    tuner.status.flags = TUNER_FLAG_INIT;

    if (tuner.api->updateStatus) {
        tuner.api->updateStatus();
    }
}
