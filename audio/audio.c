#include "audio.h"

static AudioProc audioProc;

void audioInit()
{
    // TODO: Read from settings
    audioProc.ic = AUDIO_IC_TDA7439;
    audioProc.inCnt = 4;
}

void audioSetInput(uint8_t value)
{
    if (value >= audioProc.inCnt)
        value = 0;

    audioProc.input = value;
}

void audioSetParam(AudioParam param, int8_t value)
{
    if (param >= AUDIO_PARAM_END)
        return;

    int8_t min = audioProc.item[param].min;
    int8_t max = audioProc.item[param].max;

    if (value < min)
        value = min;
    else if (value > max)
        value = max;

    audioProc.item[param].value = value;

    if (audioProc.item[param].set) {
        audioProc.item[param].set(value);
    }
}

void audioChangeParam(AudioParam param, int8_t diff)
{
    if (param >= AUDIO_PARAM_END)
        return;

    int8_t value = audioProc.item[param].value;

    value += diff;

    audioSetParam(param, value);
}

void audioSetFlag(AudioFlag flag, uint8_t value)
{
    if (value)
        audioProc.flag |= flag;
    else
        audioProc.flag &= ~flag;
}
