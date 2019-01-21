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
    // Read stored parameters
    memset(&aProc, 0, sizeof(aProc));

    aProc.par.ic = eeReadU(EE_AUDIO_IC, AUDIO_IC_TDA7439);
    aProc.par.loudness = eeReadB(EE_AUDIO_LOUDNESS, false);
    aProc.par.surround = eeReadB(EE_AUDIO_SURROUND, false);
    aProc.par.effect3d = eeReadB(EE_AUDIO_EFFECT3D, false);
    aProc.par.bypass = eeReadB(EE_AUDIO_BYPASS, false);

    aProc.par.input = (uint8_t)eeReadU(EE_AUDIO_INPUT, 0);

    for (EE_Param par = EE_AUDIO_PARAM_VOLUME; par <= EE_AUDIO_PARAM_PREAMP; par++) {
        aProc.par.item[par - EE_AUDIO_PARAM_VOLUME].value = (int8_t)eeReadI(par, 0);
    }

    for (EE_Param par = EE_AUDIO_GAIN0; par <= EE_AUDIO_GAIN7; par++) {
        aProc.par.gain[par - EE_AUDIO_GAIN0] = (int8_t)eeReadI(par, 0);
    }

    for (EE_Param par = EE_AUDIO_IN0; par <= EE_AUDIO_IN7; par++) {
        aProc.par.inType[par - EE_AUDIO_IN0] = (InputType)eeReadU(par, 0);
    }

    aProc.par.item[AUDIO_TUNE_GAIN].value = aProc.par.gain[aProc.par.input];

    // API initialization
    switch (aProc.par.ic) {
#ifdef _TDA7439
    case AUDIO_IC_TDA7439:
        aProc.api.init = tda7439Init;

        aProc.api.setTune = tda7439SetTune;
        aProc.api.setInput = tda7439SetInput;

        aProc.api.setMute = tda7439SetMute;
        break;
#endif
#ifdef _TDA731X
    case AUDIO_IC_TDA7313:
        aProc.api.init = tda731xInit;

        aProc.api.setTune = tda731xSetTune;
        aProc.api.setInput = tda731xSetInput;

        aProc.api.setMute = tda731xSetMute;
        aProc.api.setLoudness = tda731xSetLoudness;
        break;
#endif
#ifdef _PT232X
    case AUDIO_IC_PT232X:
        aProc.api.init = pt232xInit;

        aProc.api.setTune = pt232xSetTune;
        aProc.api.setInput = pt232xSetInput;

        aProc.api.setMute = pt232xSetMute;
        aProc.api.setSurround = pt232xSetSurround;
        aProc.api.setEffect3d = pt232xSetEffect3D;
        aProc.api.setBypass = pt232xSetBypass;
        break;
#endif
    default:
        break;
    }
}

void audioSaveSettings(void)
{
    eeUpdate(EE_AUDIO_IC, (int16_t)aProc.par.ic);
    eeUpdate(EE_AUDIO_INPUT, aProc.par.input);
    eeUpdate(EE_AUDIO_LOUDNESS, aProc.par.loudness);
    eeUpdate(EE_AUDIO_SURROUND, aProc.par.surround);
    eeUpdate(EE_AUDIO_EFFECT3D, aProc.par.effect3d);
    eeUpdate(EE_AUDIO_BYPASS, aProc.par.bypass);

    for (EE_Param par = EE_AUDIO_PARAM_VOLUME; par <= EE_AUDIO_PARAM_PREAMP; par++) {
        eeUpdate(par, aProc.par.item[par - EE_AUDIO_PARAM_VOLUME].value);
    }

    for (EE_Param par = EE_AUDIO_GAIN0; par <= EE_AUDIO_GAIN7; par++) {
        eeUpdate(par, aProc.par.gain[par - EE_AUDIO_GAIN0]);
    }
}

void audioInit(void)
{
    if (aProc.api.init) {
        aProc.api.init(&aProc.par);
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

void audioSetMute(bool value)
{
    aProc.par.mute = value;

    if (aProc.api.setMute) {
        aProc.api.setMute(value);
    }
}

void audioSetLoudness(bool value)
{
    aProc.par.loudness = value;

    if (aProc.api.setLoudness) {
        aProc.api.setLoudness(value);
    }
}

void audioSetSurround(bool value)
{
    aProc.par.surround = value;

    if (aProc.api.setSurround) {
        aProc.api.setSurround(value);
    }
}

void audioSetEffect3D(bool value)
{
    aProc.par.effect3d = value;

    if (aProc.api.setEffect3d) {
        aProc.api.setEffect3d(value);
    }
}

void audioSetBypass(bool value)
{
    aProc.par.bypass = value;

    if (aProc.api.setBypass) {
        aProc.api.setBypass(value);
    }
}
