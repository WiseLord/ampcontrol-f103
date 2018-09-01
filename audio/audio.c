#include "audio.h"

#ifdef _TDA7439
#include "tda7439.h"
#endif

#define FLAG_ON     1
#define FLAG_OFF    0

static const AudioGrid grid_0_0_0 = {  0,  0, 0.00 * STEP_MULT};    // Not implemented
static void setNothing(void) {}

static AudioProc aProc;

void audioInit()
{
    // Reset grid and function pointers
    aProc.setFlag = setNothing;
    for (AudioParam par = 0; par < AUDIO_PARAM_END; par++) {
        aProc.item[par].grid = &grid_0_0_0;
        aProc.item[par].set = setNothing;
    }

    // TODO: Read values from settings
    aProc.ic = AUDIO_IC_TDA7439;
    aProc.flag = 0x0000;
    aProc.input = 0;
    for (AudioParam par = 0; par < AUDIO_PARAM_END; par++) {
        aProc.item[par].value = 0;
    }

    // TODO: move to audio driver init function
    switch (aProc.ic) {
#ifdef _TDA7439
    case AUDIO_IC_TDA7439:
        tda7439Init(&aProc);
        break;
#endif
    default:
        break;
    }
}

AudioProc *audioProcGet(void)
{
    return &aProc;
}

void audioPowerOn(void)
{
    audioSetFlag(AUDIO_FLAG_MUTE, FLAG_ON);
    audioSetInput(aProc.input);

    for (AudioParam param = AUDIO_PARAM_GAIN0 - 1; param >= AUDIO_PARAM_VOLUME; param--) {
        audioSetParam(param, aProc.item[param].value);
    }

    audioSetFlag(AUDIO_FLAG_MUTE, FLAG_OFF);
}

void audioPowerOff(void)
{
    // TODO: Save all audioparams to FLASH/BKP
}

void audioSetInput(uint8_t value)
{
    if (value >= aProc.inCnt)
        value = 0;

    aProc.input = value;

    if (aProc.setInput) {
        aProc.setInput();
    }
}

void audioSetParam(AudioParam param, int8_t value)
{
    if (param >= AUDIO_PARAM_END)
        return;

    int8_t min = aProc.item[param].grid->min;
    int8_t max = aProc.item[param].grid->max;

    if (value < min)
        value = min;
    else if (value > max)
        value = max;

    aProc.item[param].value = value;

    if (aProc.item[param].set) {
        aProc.item[param].set();
    }
}

void audioChangeParam(AudioParam param, int8_t diff)
{
    if (param >= AUDIO_PARAM_END)
        return;

    int8_t value = aProc.item[param].value;

    value += diff;

    audioSetParam(param, value);
}

void audioSetFlag(AudioFlag flag, uint8_t value)
{
    if (value)
        aProc.flag |= flag;
    else
        aProc.flag &= ~flag;

    if (aProc.setFlag) {
        aProc.setFlag();
    }
}
