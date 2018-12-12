#include "audio.h"

#include <string.h>

#include "../eemul.h"

#ifdef _TDA7439
#include "tda7439.h"
#endif
#ifdef _TDA731X
#include "tda731x.h"
#endif
#ifdef _PT232X
#include "pt232x.h"
#endif

#define FLAG_ON     1
#define FLAG_OFF    0

static AudioProc aProc;

void audioReadSettings(void)
{
    memset(&aProc, 0, sizeof(aProc));

    aProc.par.ic = eeReadU(EE_AUDIO_IC, AUDIO_IC_TDA7439);
    aProc.par.flags = eeReadU(EE_AUDIO_FLAG, AUDIO_FLAG_INIT);
    aProc.par.input = (uint8_t)eeReadU(EE_AUDIO_INPUT, 0);

    for (EE_Param par = EE_AUDIO_PARAM_VOLUME; par < EE_AUDIO_GAIN0; par++) {
        aProc.par.item[par - EE_AUDIO_PARAM_VOLUME].value = (int8_t)eeReadI(par, 0);
    }

    for (EE_Param par = EE_AUDIO_GAIN0; par <= EE_AUDIO_GAIN7; par++) {
        aProc.par.gain[par - EE_AUDIO_GAIN0] = (int8_t)eeReadI(par, 0);
    }

    aProc.par.item[AUDIO_TUNE_GAIN].value = aProc.par.gain[aProc.par.input];
}

void audioSaveSettings(void)
{
    aProc.par.flags &= ~AUDIO_FLAG_MUTE; // Do not save mute

    eeUpdate(EE_AUDIO_IC, (int16_t)aProc.par.ic);
    eeUpdate(EE_AUDIO_FLAG, (int16_t)aProc.par.flags);
    eeUpdate(EE_AUDIO_INPUT, aProc.par.input);

    for (EE_Param par = EE_AUDIO_PARAM_VOLUME; par < EE_AUDIO_GAIN0; par++) {
        eeUpdate(par, aProc.par.item[par - EE_AUDIO_PARAM_VOLUME].value);
    }

    for (EE_Param par = EE_AUDIO_GAIN0; par <= EE_AUDIO_GAIN7; par++) {
        eeUpdate(par, aProc.par.gain[par - EE_AUDIO_GAIN0]);
    }
}

void audioInit(void)
{
    switch (aProc.par.ic) {
#ifdef _TDA7439
    case AUDIO_IC_TDA7439:
        aProc.api.setTune = tda7439SetTune;
        aProc.api.setInput = tda7439SetInput;

        aProc.api.setMute = tda7439SetMute;

        tda7439Init(&aProc.par);
        break;
#endif
#ifdef _TDA731X
    case AUDIO_IC_TDA7313:
        aProc.api.setTune = tda731xSetTune;
        aProc.api.setInput = tda731xSetInput;

        aProc.api.setMute = tda731xSetMute;
        aProc.api.setLoudness = tda731xSetLoudness;

        tda731xInit(&aProc.par);
        break;
#endif
#ifdef _PT232X
    case AUDIO_IC_PT232X:
        aProc.api.setTune = pt232xSetTune;
        aProc.api.setInput = pt232xSetInput;

        aProc.api.setMute = pt232xSetMute;

        pt232xInit(&aProc.par);
        break;
#endif
    default:
        break;
    }
}

AudioProc *audioGet(void)
{
    return &aProc;
}

void audioSetPower(bool value)
{
    audioSetFlag(AUDIO_FLAG_MUTE, !value);

    if (!value) {
        audioSaveSettings();
    } else {
        audioSetInput(aProc.par.input);

        audioSetFlag(AUDIO_FLAG_LOUDNESS, aProc.par.flags & AUDIO_FLAG_LOUDNESS);
        audioSetFlag(AUDIO_FLAG_SURROUND, aProc.par.flags & AUDIO_FLAG_SURROUND);
        audioSetFlag(AUDIO_FLAG_EFFECT3D, aProc.par.flags & AUDIO_FLAG_EFFECT3D);
        audioSetFlag(AUDIO_FLAG_BYPASS, aProc.par.flags & AUDIO_FLAG_BYPASS);

        for (AudioTune tune = AUDIO_TUNE_VOLUME; tune < AUDIO_TUNE_END; tune++) {
            audioSetTune(tune, aProc.par.item[tune].value);
        }
    }

    if (aProc.api.setPower) {
        aProc.api.setPower(value);
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
    }

    if (aProc.api.setTune) {
        aProc.api.setTune(tune, value);
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

    if (aProc.api.setInput) {
        aProc.api.setInput(value);
    }

    audioSetTune(AUDIO_TUNE_GAIN, aProc.par.gain[value]);
}

void audioSetFlag(AudioFlag flag, bool value)
{
    if (value)
        aProc.par.flags |= flag;
    else
        aProc.par.flags &= ~flag;

    switch (flag) {
    case AUDIO_FLAG_MUTE:
        if (aProc.api.setMute) {
            aProc.api.setMute(value);
        }
        break;
    default:
        break;
    }
}
