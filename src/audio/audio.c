#include "audio.h"

#include <string.h>

#include "settings.h"

#ifdef _TDA7439
#include "tda7439.h"
#endif
#ifdef _TDA731X
#include "tda731x.h"
#endif
#ifdef _PT232X
#include "pt232x.h"
#endif
#ifdef _TDA7418
#include "tda7418.h"
#endif
#ifdef _TDA7719
#include "tda7719.h"
#endif


static AudioProc aProc;

static const AudioGrid gridTestVolume       = {-79,  0, (int8_t)(1.00 * STEP_MULT)}; // -79..0dB with 1dB step
static const AudioGrid gridTestTone         = { -7,  7, (int8_t)(2.00 * STEP_MULT)}; // -14..14dB with 2dB step
static const AudioGrid gridTestBalance      = { -7,  7, (int8_t)(1.00 * STEP_MULT)}; // -7..7dB with 1dB step
static const AudioGrid gridTestCenterSub    = {-15,  0, (int8_t)(1.00 * STEP_MULT)}; // -15..0dB with 1dB step
static const AudioGrid gridTestPreamp       = {-47,  0, (int8_t)(1.00 * STEP_MULT)}; // -47..0dB with 1dB step
static const AudioGrid gridTestGain         = {  0, 15, (int8_t)(2.00 * STEP_MULT)}; // 0..30dB with 2dB step

static void audioTestInit(AudioParam *aPar)
{
    aPar->tune[AUDIO_TUNE_VOLUME].grid    = &gridTestVolume;
    aPar->tune[AUDIO_TUNE_BASS].grid      = &gridTestTone;
    aPar->tune[AUDIO_TUNE_MIDDLE].grid    = &gridTestTone;
    aPar->tune[AUDIO_TUNE_TREBLE].grid    = &gridTestTone;
    aPar->tune[AUDIO_TUNE_FRONTREAR].grid = &gridTestBalance;
    aPar->tune[AUDIO_TUNE_BALANCE].grid   = &gridTestBalance;
    aPar->tune[AUDIO_TUNE_CENTER].grid    = &gridTestCenterSub;
    aPar->tune[AUDIO_TUNE_SUBWOOFER].grid = &gridTestCenterSub;
    aPar->tune[AUDIO_TUNE_PREAMP].grid    = &gridTestPreamp;
    aPar->tune[AUDIO_TUNE_GAIN].grid      = &gridTestGain;
}

static const AudioApi audioTestApi = {
    .init = audioTestInit,
};

void audioReadSettings(AudioIC ic)
{
    // Read stored parameters
    memset(&aProc, 0, sizeof(aProc));

    aProc.par.ic = settingsRead(PARAM_AUDIO_IC, ic);
    aProc.par.input = settingsRead(PARAM_AUDIO_INPUT, 0);
    aProc.par.loudness = settingsRead(PARAM_AUDIO_LOUDNESS, false);
    aProc.par.surround = settingsRead(PARAM_AUDIO_SURROUND, false);
    aProc.par.effect3d = settingsRead(PARAM_AUDIO_EFFECT3D, false);
    aProc.par.bypass = settingsRead(PARAM_AUDIO_BYPASS, false);
    aProc.par.mode = settingsRead(PARAM_AUDIO_MODE, false);

    for (Param par = PARAM_AUDIO_GAIN0; par <= PARAM_AUDIO_GAIN7; par++) {
         aProc.par.gain[par - PARAM_AUDIO_GAIN0] = settingsRead(par, 0);
    }
    for (Param par = PARAM_AUDIO_VOLUME; par <= PARAM_AUDIO_PREAMP; par++) {
         aProc.par.tune[par - PARAM_AUDIO_VOLUME].value = settingsRead(par, 0);
    }

    // API initialization
    switch (aProc.par.ic) {
#ifdef _TDA7439
    case AUDIO_IC_TDA7439:
    case AUDIO_IC_TDA7440:
        aProc.api = tda7439GetApi();
        aProc.par.inCnt = TDA7439_IN_CNT;
        break;
#endif
#ifdef _TDA731X
    case AUDIO_IC_TDA7313:
        aProc.api = tda731xGetApi();
        aProc.par.inCnt = TDA7313_IN_CNT;
        break;
#endif
#ifdef _PT232X
    case AUDIO_IC_PT232X:
        aProc.api = pt232xGetApi();
        aProc.par.inCnt = PT2323_IN_CNT;
        break;
#endif
#ifdef _TDA7418
    case AUDIO_IC_TDA7418:
        aProc.api = tda7418GetApi();
        aProc.par.inCnt = TDA7418_IN_CNT;
        break;
#endif
#ifdef _TDA7719
    case AUDIO_IC_TDA7719:
        aProc.api = tda7719GetApi();
        aProc.par.inCnt = TDA7719_IN_CNT;
        break;
#endif
    case AUDIO_IC_TEST:
        aProc.api = &audioTestApi;
        aProc.par.inCnt = MAX_INPUTS;
        break;
    default:
        aProc.par.inCnt = 1;
        break;
    }
}

void audioSaveSettings(void)
{
    settingsStore(PARAM_AUDIO_IC, (int16_t)aProc.par.ic);
    settingsStore(PARAM_AUDIO_INPUT, aProc.par.input);
    settingsStore(PARAM_AUDIO_LOUDNESS, aProc.par.loudness);
    settingsStore(PARAM_AUDIO_SURROUND, aProc.par.surround);
    settingsStore(PARAM_AUDIO_EFFECT3D, aProc.par.effect3d);
    settingsStore(PARAM_AUDIO_BYPASS, aProc.par.bypass);

    for (Param par = PARAM_AUDIO_VOLUME; par <= PARAM_AUDIO_PREAMP; par++) {
        settingsStore(par, aProc.par.tune[par - PARAM_AUDIO_VOLUME].value);
    }

    for (Param par = PARAM_AUDIO_GAIN0; par <= PARAM_AUDIO_GAIN7; par++) {
        settingsStore(par, aProc.par.gain[par - PARAM_AUDIO_GAIN0]);
    }
}

void audioReset(void)
{
    if (aProc.api && aProc.api->init) {
        aProc.api->init(&aProc.par);
    }
}

AudioProc *audioGet(void)
{
    return &aProc;
}

void audioSetRawBalance(AudioRaw *raw, int8_t volume, bool rear2bass)
{
    AudioParam *aPar = &aProc.par;

    raw->frontLeft = volume;
    raw->frontRight = volume;
    raw->rearLeft = volume;
    raw->rearRight = volume;
    raw->center = volume;
    raw->subwoofer = volume;

    if (aPar->tune[AUDIO_TUNE_BALANCE].value > 0) {
        raw->frontLeft -= aPar->tune[AUDIO_TUNE_BALANCE].value;
        if (!rear2bass) {
            raw->rearLeft -= aPar->tune[AUDIO_TUNE_BALANCE].value;
        }
    } else {
        raw->frontRight += aPar->tune[AUDIO_TUNE_BALANCE].value;
        if (!rear2bass) {
            raw->rearRight += aPar->tune[AUDIO_TUNE_BALANCE].value;
        }
    }
    if (rear2bass) {
        raw->rearLeft += aPar->tune[AUDIO_TUNE_SUBWOOFER].value;
        raw->rearRight += aPar->tune[AUDIO_TUNE_SUBWOOFER].value;
    } else {
        if (aPar->tune[AUDIO_TUNE_FRONTREAR].value > 0) {
            raw->rearLeft -= aPar->tune[AUDIO_TUNE_FRONTREAR].value;
            raw->rearRight -= aPar->tune[AUDIO_TUNE_FRONTREAR].value;
        } else {
            raw->frontLeft += aPar->tune[AUDIO_TUNE_FRONTREAR].value;
            raw->frontRight += aPar->tune[AUDIO_TUNE_FRONTREAR].value;
        }
    }

    raw->center += aPar->tune[AUDIO_TUNE_CENTER].value;
    raw->subwoofer += aPar->tune[AUDIO_TUNE_SUBWOOFER].value;
}

void audioSetPower(bool value)
{
    if (!value) {
        audioSaveSettings();
    } else {
        audioSetInput(aProc.par.input);

        audioSetLoudness(aProc.par.loudness);
        audioSetSurround(aProc.par.surround);
        audioSetEffect3D(aProc.par.effect3d);
        audioSetBypass(aProc.par.bypass);

        for (AudioTune tune = AUDIO_TUNE_VOLUME; tune < AUDIO_TUNE_END; tune++) {
            audioSetTune(tune, aProc.par.tune[tune].value);
        }
    }

    if (aProc.api && aProc.api->setPower) {
        aProc.api->setPower(value);
    }
}

void audioSetTune(AudioTune tune, int8_t value)
{
    if (tune >= AUDIO_TUNE_END)
        return;

    if (aProc.par.tune[tune].grid == 0)
        return;

    int8_t min = aProc.par.tune[tune].grid->min;
    int8_t max = aProc.par.tune[tune].grid->max;

    if (value < min) {
        value = min;
    } else if (value > max) {
        value = max;
    }

    aProc.par.tune[tune].value = value;
    if (tune == AUDIO_TUNE_GAIN) {
        aProc.par.gain[aProc.par.input] = value;
    }

    if (aProc.api && aProc.api->setTune) {
        switch (tune) {
        case AUDIO_TUNE_BASS:
        case AUDIO_TUNE_MIDDLE:
        case AUDIO_TUNE_TREBLE:
            if (!aProc.api->setBypass && aProc.par.bypass) {
                value = 0;
            }
            break;
        }
        aProc.api->setTune(tune, value);
    }
}

void audioChangeTune(AudioTune tune, int8_t diff)
{
    if (tune >= AUDIO_TUNE_END)
        return;

    int8_t value = aProc.par.tune[tune].value;

    value += diff;

    audioSetTune(tune, value);
}

void audioSetInput(int8_t value)
{
    if (value >= aProc.par.inCnt) {
        value = 0;
    }

    aProc.par.input = value;
    aProc.par.tune[AUDIO_TUNE_GAIN].value = aProc.par.gain[aProc.par.input];

    if (aProc.api && aProc.api->setInput) {
        aProc.api->setInput(value);
    }

    audioSetTune(AUDIO_TUNE_GAIN, aProc.par.gain[value]);
}

void audioSetMute(bool value)
{
    aProc.par.mute = value;

    if (aProc.api && aProc.api->setMute) {
        aProc.api->setMute(value);
    }
}

void audioSetLoudness(bool value)
{
    aProc.par.loudness = value;

    if (aProc.api && aProc.api->setLoudness) {
        aProc.api->setLoudness(value);
    }
}

void audioSetSurround(bool value)
{
    aProc.par.surround = value;

    if (aProc.api && aProc.api->setSurround) {
        aProc.api->setSurround(value);
    }
}

void audioSetEffect3D(bool value)
{
    aProc.par.effect3d = value;

    if (aProc.api && aProc.api->setEffect3d) {
        aProc.api->setEffect3d(value);
    }
}

void audioSetBypass(bool value)
{
    aProc.par.bypass = value;

    if (aProc.api && aProc.api->setBypass) {
        aProc.api->setBypass(value);
    } else {
        audioSetTune(AUDIO_TUNE_BASS, aProc.par.tune[AUDIO_TUNE_BASS].value);
        audioSetTune(AUDIO_TUNE_MIDDLE, aProc.par.tune[AUDIO_TUNE_MIDDLE].value);
        audioSetTune(AUDIO_TUNE_TREBLE, aProc.par.tune[AUDIO_TUNE_TREBLE].value);
    }
}

bool audioIsModeSupported(AudioMode mode)
{
    bool ret = false;

    AudioIC ic = aProc.par.ic;

    switch (mode) {
    case AUDIO_MODE_2_0:
        ret = true;
        break;
    case AUDIO_MODE_2_1:
    case AUDIO_MODE_4_0:
        switch (ic) {
        case AUDIO_IC_TDA7313:
        case AUDIO_IC_PT232X:
        case AUDIO_IC_TDA7418:
        case AUDIO_IC_TDA7719:
            ret = true;
            break;
        }
        break;
    case AUDIO_MODE_4_1:
        switch (ic) {
        case AUDIO_IC_PT232X:
        case AUDIO_IC_TDA7418:
        case AUDIO_IC_TDA7719:
            ret = true;
            break;
        }
        break;
    case AUDIO_MODE_5_1:
        switch (ic) {
        case AUDIO_IC_PT232X:
            ret = true;
            break;
        }
        break;
    }

    return ret;
}
