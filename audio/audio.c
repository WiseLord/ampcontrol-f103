#include "audio.h"

#include <string.h>

#include "../settings.h"
#include "../pins.h"

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

#define FLAG_ON     1
#define FLAG_OFF    0

static AudioProc aProc;

static const AudioGrid gridTestVolume       = {-79,  0, (uint8_t)(1.00 * 8)}; // -79..0dB with 1dB step
static const AudioGrid gridTestTone         = { -7,  7, (uint8_t)(2.00 * 8)}; // -14..14dB with 2dB step
static const AudioGrid gridTestBalance      = { -7,  7, (uint8_t)(1.00 * 8)}; // -7..7dB with 1dB step
static const AudioGrid gridTestCenterSub    = {-15,  0, (uint8_t)(1.00 * 8)}; // -15..0dB with 1dB step
static const AudioGrid gridTestPreamp       = {-47,  0, (uint8_t)(1.00 * 8)}; // -47..0dB with 1dB step
static const AudioGrid gridTestGain         = {  0, 15, (uint8_t)(2.00 * 8)}; // 0..30dB with 2dB step

static void audioTestInit(AudioParam *aPar)
{
    aPar->item[AUDIO_TUNE_VOLUME].grid    = &gridTestVolume;
    aPar->item[AUDIO_TUNE_BASS].grid      = &gridTestTone;
    aPar->item[AUDIO_TUNE_MIDDLE].grid    = &gridTestTone;
    aPar->item[AUDIO_TUNE_TREBLE].grid    = &gridTestTone;
    aPar->item[AUDIO_TUNE_FRONTREAR].grid = &gridTestBalance;
    aPar->item[AUDIO_TUNE_BALANCE].grid   = &gridTestBalance;
    aPar->item[AUDIO_TUNE_CENTER].grid    = &gridTestCenterSub;
    aPar->item[AUDIO_TUNE_SUBWOOFER].grid = &gridTestCenterSub;
    aPar->item[AUDIO_TUNE_PREAMP].grid    = &gridTestPreamp;
    aPar->item[AUDIO_TUNE_GAIN].grid      = &gridTestGain;
}

static const AudioApi audioTestApi = {
    .init = audioTestInit,
};

void audioReadSettings(void)
{
    // Read stored parameters
    memset(&aProc, 0, sizeof(aProc));

    aProc.par.ic = (AudioIC)(settingsRead(PARAM_AUDIO_IC));
    aProc.par.loudness = settingsRead(PARAM_AUDIO_LOUDNESS);
    aProc.par.surround = settingsRead(PARAM_AUDIO_SURROUND);
    aProc.par.effect3d = settingsRead(PARAM_AUDIO_EFFECT3D);
    aProc.par.bypass = settingsRead(PARAM_AUDIO_BYPASS);

    aProc.par.input = (uint8_t)settingsRead(PARAM_AUDIO_INPUT);

    for (Param par = PARAM_AUDIO_VOLUME; par <= PARAM_AUDIO_PREAMP; par++) {
        aProc.par.item[par - PARAM_AUDIO_VOLUME].value = (int8_t)settingsRead(par);
    }

    for (Param par = PARAM_AUDIO_GAIN0; par <= PARAM_AUDIO_GAIN_LAST; par++) {
        aProc.par.gain[par - PARAM_AUDIO_GAIN0] = (int8_t)settingsRead(par);
    }

    for (Param par = PARAM_AUDIO_IN0; par <= PARAM_AUDIO_IN_LAST; par++) {
        aProc.par.inType[par - PARAM_AUDIO_IN0] = (InputType)settingsRead(par);
    }

    aProc.par.item[AUDIO_TUNE_GAIN].value = aProc.par.gain[aProc.par.input];

    aProc.par.inCnt = MAX_INPUTS;

    // API initialization
    aProc.api = &audioTestApi;
    switch (aProc.par.ic) {
#ifdef _TDA7439
    case AUDIO_IC_TDA7439:
    case AUDIO_IC_TDA7440:
        aProc.api = tda7439GetApi();
        break;
#endif
#ifdef _TDA731X
    case AUDIO_IC_TDA7313:
        aProc.api = tda731xGetApi();
        break;
#endif
#ifdef _PT232X
    case AUDIO_IC_PT232X:
        aProc.api = pt232xGetApi();
        break;
#endif
#ifdef _TDA7418
    case AUDIO_IC_TDA7418:
        aProc.api = tda7418GetApi();
        break;
#endif
    default:
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
        settingsStore(par, aProc.par.item[par - PARAM_AUDIO_VOLUME].value);
    }

    for (Param par = PARAM_AUDIO_GAIN0; par <= PARAM_AUDIO_GAIN_LAST; par++) {
        settingsStore(par, aProc.par.gain[par - PARAM_AUDIO_GAIN0]);
    }
}

void audioInit(void)
{
    if (aProc.api->init) {
        aProc.api->init(&aProc.par);
    }
}

AudioProc *audioGet(void)
{
    return &aProc;
}

void audioSetPower(bool value)
{
    audioSetMute(!value);

    if (!value) {
        audioSaveSettings();
    } else {
        audioSetInput(aProc.par.input);

        audioSetLoudness(aProc.par.loudness);
        audioSetSurround(aProc.par.surround);
        audioSetEffect3D(aProc.par.effect3d);
        audioSetBypass(aProc.par.bypass);

        for (AudioTune tune = AUDIO_TUNE_VOLUME; tune < AUDIO_TUNE_END; tune++) {
            audioSetTune(tune, aProc.par.item[tune].value);
        }
    }

    if (aProc.api->setPower) {
        aProc.api->setPower(value);
    }
}

void audioSetTune(AudioTune tune, int8_t value)
{
    if (tune >= AUDIO_TUNE_END)
        return;

    if (aProc.par.item[tune].grid == 0)
        return;

    int8_t min = aProc.par.item[tune].grid->min;
    int8_t max = aProc.par.item[tune].grid->max;

    if (value < min) {
        value = min;
    } else if (value > max) {
        value = max;
    }

    aProc.par.item[tune].value = value;
    if (tune == AUDIO_TUNE_GAIN) {
        aProc.par.gain[aProc.par.input] = value;
    } else if (tune == AUDIO_TUNE_VOLUME) {
        aProc.par.mute = false;
    }

    if (aProc.api->setTune) {
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

    int8_t value = aProc.par.item[tune].value;

    value += diff;

    audioSetTune(tune, value);
}

void audioSetInput(uint8_t value)
{
    if (value >= aProc.par.inCnt)
        value = 0;

    aProc.par.input = value;
    aProc.par.item[AUDIO_TUNE_GAIN].value = aProc.par.gain[aProc.par.input];

    if (aProc.api->setInput) {
        aProc.api->setInput(value);
    }

    audioSetTune(AUDIO_TUNE_GAIN, aProc.par.gain[value]);
}

void audioSetMute(bool value)
{
    aProc.par.mute = value;

    pinsSetMute(value);

    if (aProc.api->setMute) {
        aProc.api->setMute(value);
    }
}

void audioSetLoudness(bool value)
{
    aProc.par.loudness = value;

    if (aProc.api->setLoudness) {
        aProc.api->setLoudness(value);
    }
}

void audioSetSurround(bool value)
{
    aProc.par.surround = value;

    if (aProc.api->setSurround) {
        aProc.api->setSurround(value);
    }
}

void audioSetEffect3D(bool value)
{
    aProc.par.effect3d = value;

    if (aProc.api->setEffect3d) {
        aProc.api->setEffect3d(value);
    }
}

void audioSetBypass(bool value)
{
    aProc.par.bypass = value;

    if (aProc.api->setBypass) {
        aProc.api->setBypass(value);
    } else {
        audioSetTune(AUDIO_TUNE_BASS, aProc.par.item[AUDIO_TUNE_BASS].value);
        audioSetTune(AUDIO_TUNE_MIDDLE, aProc.par.item[AUDIO_TUNE_MIDDLE].value);
        audioSetTune(AUDIO_TUNE_TREBLE, aProc.par.item[AUDIO_TUNE_TREBLE].value);
    }
}
