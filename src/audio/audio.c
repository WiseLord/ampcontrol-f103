#include "audio.h"

#include <string.h>

#include "settings.h"

#include "tda7439.h"
#include "tda731x.h"
#include "pt232x.h"
#include "tda7418.h"
#include "tda7719.h"

static AudioProc aProc;

static const AudioGrid gridTestVolume       = {NULL, -79,  0, (int8_t)(1.00 * STEP_MULT)}; // -79..0dB with 1dB step
static const AudioGrid gridTestTone         = {NULL,  -7,  7, (int8_t)(2.00 * STEP_MULT)}; // -14..14dB with 2dB step
static const AudioGrid gridTestBalance      = {NULL,  -7,  7, (int8_t)(1.00 * STEP_MULT)}; // -7..7dB with 1dB step
static const AudioGrid gridTestCenterSub    = {NULL, -15,  0, (int8_t)(1.00 * STEP_MULT)}; // -15..0dB with 1dB step
static const AudioGrid gridTestPreamp       = {NULL, -47,  0, (int8_t)(1.00 * STEP_MULT)}; // -47..0dB with 1dB step
static const AudioGrid gridTestGain         = {NULL,   0, 15, (int8_t)(2.00 * STEP_MULT)}; // 0..30dB with 2dB step

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
    aProc.par.flags = settingsRead(PARAM_AUDIO_FLAGS, 0);
    aProc.par.mode = settingsRead(PARAM_AUDIO_MODE, AUDIO_MODE_2_0);

    for (Param par = PARAM_AUDIO_GAIN0; par <= PARAM_AUDIO_GAIN7; par++) {
        aProc.par.gain[par - PARAM_AUDIO_GAIN0] = settingsRead(par, 0);
    }
    for (Param par = PARAM_AUDIO_VOLUME; par <= PARAM_AUDIO_LOUD_PEAK_FREQ; par++) {
        aProc.par.tune[par - PARAM_AUDIO_VOLUME].value = settingsRead(par, 0);
    }

    // API initialization
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
#ifdef _TDA7719
    case AUDIO_IC_TDA7719:
        aProc.api = tda7719GetApi();
        break;
#endif
    case AUDIO_IC_TEST:
        aProc.api = &audioTestApi;
        break;
    default:
        break;
    }
}

void audioSaveSettings(void)
{
    settingsStore(PARAM_AUDIO_IC, (int16_t)aProc.par.ic);
    settingsStore(PARAM_AUDIO_INPUT, aProc.par.input);
    settingsStore(PARAM_AUDIO_FLAGS, aProc.par.flags & ~AUDIO_FLAG_MUTE);

    for (Param par = PARAM_AUDIO_VOLUME; par < PARAM_AUDIO_END; par++) {
        settingsStore(par, aProc.par.tune[par - PARAM_AUDIO_VOLUME].value);
    }

    for (Param par = PARAM_AUDIO_GAIN0; par <= PARAM_AUDIO_GAIN7; par++) {
        settingsStore(par, aProc.par.gain[par - PARAM_AUDIO_GAIN0]);
    }
}

void audioInit(void)
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

        audioSetFlag(AUDIO_FLAG_LOUDNESS, (aProc.par.flags & AUDIO_FLAG_LOUDNESS));
        audioSetFlag(AUDIO_FLAG_SURROUND, (aProc.par.flags & AUDIO_FLAG_SURROUND));
        audioSetFlag(AUDIO_FLAG_EFFECT3D, (aProc.par.flags & AUDIO_FLAG_EFFECT3D));
        audioSetFlag(AUDIO_FLAG_BYPASS, (aProc.par.flags & AUDIO_FLAG_BYPASS));

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
    if (!audioIsTuneValid(tune)) {
        return;
    }

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
            if (!aProc.api->setBypass && (aProc.par.flags & AUDIO_FLAG_BYPASS)) {
                value = 0;
            }
            break;
        }
        aProc.api->setTune(tune, value);
    }
}

void audioChangeTune(AudioTune tune, int8_t diff)
{
    if (!audioIsTuneValid(tune)) {
        return;
    }

    int8_t value = aProc.par.tune[tune].value;
    value += diff;
    audioSetTune(tune, value);
}

void audioSetInput(int8_t value)
{
    if (value >= audioGetInputCount()) {
        value = 0;
    }

    aProc.par.input = value;
    aProc.par.tune[AUDIO_TUNE_GAIN].value = aProc.par.gain[aProc.par.input];

    if (aProc.api && aProc.api->setInput) {
        aProc.api->setInput(value);
    }

    audioSetTune(AUDIO_TUNE_GAIN, aProc.par.gain[value]);
}

int8_t audioGetInputCount(void)
{
    switch (aProc.par.ic) {
    case AUDIO_IC_TDA7439:
    case AUDIO_IC_TDA7440:
        return TDA7439_IN_CNT;
    case AUDIO_IC_TDA7313:
        return TDA7313_IN_CNT;
    case AUDIO_IC_PT232X:
        return PT2323_IN_CNT;
    case AUDIO_IC_TDA7418:
        return TDA7418_IN_CNT;
    case AUDIO_IC_TDA7719:
        return TDA7719_IN_CNT;
    case AUDIO_IC_TEST:
        return MAX_INPUTS;
    default:
        return 1;
    }
}

void audioSetFlag(AudioFlag flag, bool value)
{
    if (value) {
        aProc.par.flags |= flag;
    } else {
        aProc.par.flags &= ~flag;
    }

    if (flag & AUDIO_FLAG_MUTE) {
        if (aProc.api && aProc.api->setMute) {
            aProc.api->setMute(value);
        }
    }
    if (flag & AUDIO_FLAG_LOUDNESS) {
        if (aProc.api && aProc.api->setLoudness) {
            aProc.api->setLoudness(value);
        }
    }
    if (flag & AUDIO_FLAG_SURROUND) {
        if (aProc.api && aProc.api->setSurround) {
            aProc.api->setSurround(value);
        }
    }
    if (flag & AUDIO_FLAG_EFFECT3D) {
        if (aProc.api && aProc.api->setEffect3d) {
            aProc.api->setEffect3d(value);
        }
    }
    if (flag & AUDIO_FLAG_BYPASS) {
        if (aProc.api && aProc.api->setBypass) {
            aProc.api->setBypass(value);
        } else {
            audioSetTune(AUDIO_TUNE_BASS, aProc.par.tune[AUDIO_TUNE_BASS].value);
            audioSetTune(AUDIO_TUNE_MIDDLE, aProc.par.tune[AUDIO_TUNE_MIDDLE].value);
            audioSetTune(AUDIO_TUNE_TREBLE, aProc.par.tune[AUDIO_TUNE_TREBLE].value);
        }
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

bool audioIsTuneValid(AudioTune tune)
{
    if (tune >= AUDIO_TUNE_END) {
        return false;
    }

    return aProc.par.tune[tune].grid != NULL;
}

AudioGroup audioGetGroup(AudioTune tune)
{
    switch (tune) {
    case AUDIO_TUNE_VOLUME:
    case AUDIO_TUNE_PREAMP:
    case AUDIO_TUNE_GAIN:
        return AUDIO_GROUP_VOLUME;
    case AUDIO_TUNE_BASS:
    case AUDIO_TUNE_BASS_FREQ:
    case AUDIO_TUNE_BASS_QUAL:
        return AUDIO_GROUP_BASS;
    case AUDIO_TUNE_MIDDLE:
    case AUDIO_TUNE_MIDDLE_KFREQ:
    case AUDIO_TUNE_MIDDLE_QUAL:
        return AUDIO_GROUP_MIDDLE;
    case AUDIO_TUNE_TREBLE:
    case AUDIO_TUNE_TREBLE_KFREQ:
        return AUDIO_GROUP_TREBLE;
    case AUDIO_TUNE_FRONTREAR:
    case AUDIO_TUNE_BALANCE:
    case AUDIO_TUNE_CENTER:
        return AUDIO_GROUP_BALANCE;
    case AUDIO_TUNE_SUBWOOFER:
    case AUDIO_TUNE_SUB_CUT_FREQ:
        return AUDIO_GROUP_SUBFOOWER;
    case AUDIO_TUNE_LOUDNESS:
    case AUDIO_TUNE_LOUD_PEAK_FREQ:
        return AUDIO_GROUP_LOUDNESS;
    default:
        return AUDIO_GROUP_INVALID;
    }
}

AudioTune audioGetFirstInGroup(AudioGroup group)
{
    switch (group) {
    case AUDIO_GROUP_VOLUME:
        if (audioIsTuneValid(AUDIO_TUNE_VOLUME)) {
            return AUDIO_TUNE_VOLUME;
        } else if (audioIsTuneValid(AUDIO_TUNE_PREAMP)) {
            return AUDIO_TUNE_PREAMP;
        } else if (audioIsTuneValid(AUDIO_TUNE_GAIN)) {
            return AUDIO_TUNE_GAIN;
        }
    case AUDIO_GROUP_BASS:
        if (audioIsTuneValid(AUDIO_TUNE_BASS)) {
            return AUDIO_TUNE_BASS;
        } else if (audioIsTuneValid(AUDIO_TUNE_BASS_FREQ)) {
            return AUDIO_TUNE_BASS_FREQ;
        } else if (audioIsTuneValid(AUDIO_TUNE_BASS_QUAL)) {
            return AUDIO_TUNE_BASS_QUAL;
        }
    case AUDIO_GROUP_MIDDLE:
        if (audioIsTuneValid(AUDIO_TUNE_MIDDLE)) {
            return AUDIO_TUNE_MIDDLE;
        } else if (audioIsTuneValid(AUDIO_TUNE_MIDDLE_KFREQ)) {
            return AUDIO_TUNE_MIDDLE_KFREQ;
        } else if (audioIsTuneValid(AUDIO_TUNE_MIDDLE_QUAL)) {
            return AUDIO_TUNE_MIDDLE_QUAL;
        }
    case AUDIO_GROUP_TREBLE:
        if (audioIsTuneValid(AUDIO_TUNE_TREBLE)) {
            return AUDIO_TUNE_TREBLE;
        } else if (audioIsTuneValid(AUDIO_TUNE_TREBLE_KFREQ)) {
            return AUDIO_TUNE_TREBLE_KFREQ;
        }
    case AUDIO_GROUP_BALANCE:
        if (audioIsTuneValid(AUDIO_TUNE_BALANCE)) {
            return AUDIO_TUNE_BALANCE;
        } else if (audioIsTuneValid(AUDIO_TUNE_FRONTREAR)) {
            return AUDIO_TUNE_FRONTREAR;
        } else if (audioIsTuneValid(AUDIO_TUNE_CENTER)) {
            return AUDIO_TUNE_CENTER;
        }
    case AUDIO_GROUP_SUBFOOWER:
        if (audioIsTuneValid(AUDIO_TUNE_SUBWOOFER)) {
            return AUDIO_TUNE_SUBWOOFER;
        } else if (audioIsTuneValid(AUDIO_TUNE_SUB_CUT_FREQ)) {
            return AUDIO_TUNE_SUB_CUT_FREQ;
        }
    case AUDIO_GROUP_LOUDNESS:
        if (audioIsTuneValid(AUDIO_TUNE_LOUDNESS)) {
            return AUDIO_TUNE_LOUDNESS;
        } else if (audioIsTuneValid(AUDIO_TUNE_LOUD_PEAK_FREQ)) {
            return AUDIO_TUNE_LOUD_PEAK_FREQ;
        }
    }

    return AUDIO_TUNE_INVALID;
}
