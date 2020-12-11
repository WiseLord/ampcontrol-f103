#include "tuner.h"

#include <string.h>

#include "rds.h"
#include "settings.h"
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
#ifdef _LC7213X
#include "lc7213x.h"
#endif

static Tuner tuner;

void tunerTestInit(TunerParam *tPar, TunerStatus *status)
{
    switch (tPar->band) {
    case TUNER_BAND_FM_JAPAN:
        tPar->fMin = 7600;
        tPar->fMax = 9100;
        break;
    case TUNER_BAND_FM_WORLDWIDE:
        tPar->fMin = 7600;
        tPar->fMax = 10800;
        break;
    case TUNER_BAND_FM_EASTEUROPE:
        tPar->fMin = 6500;
        tPar->fMax = 7600;
        break;
    default:
        tPar->fMin = 8700;
        tPar->fMax = 10800;
        break;
    }

    switch (tPar->step) {
    case TUNER_STEP_50K:
        tPar->fStep = 5;
        break;
    case TUNER_STEP_200K:
        tPar->fStep = 20;
        break;
    default:
        tPar->fStep = 10;
        break;
    }

    status->rssi = 10;
    status->flags = TUNER_FLAG_STEREO | TUNER_FLAG_RDS_READY;
}

void tunerTestSetFreq(uint16_t value)
{
    tuner.status.freq = value;
}

static const TunerApi tunerTestApi = {
    .init = tunerTestInit,
    .setFreq = tunerTestSetFreq,
    .seek = tunerStep,
};


void tunerReadSettings(TunerIC defIC)
{
    stationsInit();

    // Read stored parameters
    memset(&tuner, 0, sizeof(tuner));

    tuner.par.ic = settingsRead(PARAM_TUNER_IC, defIC);
    tuner.par.band = settingsRead(PARAM_TUNER_BAND, TUNER_BAND_FM_US_EUROPE);
    tuner.par.step = settingsRead(PARAM_TUNER_STEP, TUNER_STEP_100K);
    tuner.par.deemph = settingsRead(PARAM_TUNER_DEEMPH, TUNER_DEEMPH_50u);
    tuner.par.stationMode = settingsRead(PARAM_TUNER_STA_MODE, false);
    tuner.par.forcedMono = settingsRead(PARAM_TUNER_FMONO, false);
    tuner.par.rds = settingsRead(PARAM_TUNER_RDS, true);
    tuner.par.bassBoost = settingsRead(PARAM_TUNER_BASS, false);
    tuner.par.volume = settingsRead(PARAM_TUNER_VOLUME, TUNER_VOLUME_MAX);
    tuner.status.freq = settingsRead(PARAM_TUNER_FREQ, 9950);

    stationFavInit();

    // API initialization
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
#ifdef _LC7213X
    case TUNER_IC_LC7213X:
        tuner.api = lc7213xGetApi();
        break;
#endif
    case TUNER_IC_TEST:
        tuner.api = &tunerTestApi;
        break;
    default:
        tuner.api = NULL;
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

    if (tuner.api && tuner.api->init) {
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

    if (tuner.api && tuner.api->setPower) {
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

    if (tuner.api && tuner.api->setFreq) {
        tuner.status.freq = value;
        tuner.api->setFreq(value);
    }
}

void tunerSetMute(bool value)
{
    tuner.par.mute = value;

    if (tuner.api && tuner.api->setMute) {
        tuner.api->setMute(value);
    }
}

void tunerSetBassBoost(bool value)
{
    tuner.par.bassBoost = value;

    if (tuner.api && tuner.api->setBassBoost) {
        tuner.api->setBassBoost(value);
    }
}

void tunerSetForcedMono(bool value)
{
    tuner.par.forcedMono = value;

    if (tuner.api && tuner.api->setForcedMono) {
        tuner.api->setForcedMono(value);
    }
}

void tunerSetRds(bool value)
{
    rdsReset();

    tuner.par.rds = value;

    if (tuner.api && tuner.api->setRds) {
        tuner.api->setRds(value);
    }
}

void tunerSetVolume(int8_t value)
{
    tuner.par.volume = value;

    if (tuner.api && tuner.api->setVolume) {
        tuner.api->setVolume(value);
    }
}

void tunerSeek(int8_t direction)
{
    rdsReset();

    if (tuner.api && tuner.api->seek) {
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
    if (tuner.par.stationMode) {
        if (stationGetCount() > 0) {
            stationSeek(direction);
        } else {
            tunerStep(direction);
        }
    } else {
        tunerStep(direction);
    }
}

void tunerUpdateStatus(void)
{
    tuner.status.flags = TUNER_FLAG_INIT;

    if (tuner.api && tuner.api->updateStatus) {
        tuner.api->updateStatus();
    }
}
