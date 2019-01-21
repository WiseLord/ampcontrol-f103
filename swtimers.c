#include "swtimers.h"

static volatile int16_t timDisplay = SW_TIM_OFF;
static volatile int16_t timSpConvert = SW_TIM_OFF;
static volatile int16_t timTunerPoll = SW_TIM_OFF;
static volatile int16_t timInitHw = SW_TIM_OFF;
static volatile int16_t timRcRepeat = SW_TIM_OFF;

void swTimUpdate(void)
{
    if (timDisplay > 0)
        timDisplay--;
    if (timSpConvert > 0)
        timSpConvert--;
    if (timTunerPoll > 0)
        timTunerPoll--;
    if (timInitHw > 0)
        timInitHw--;
    if (timRcRepeat > 0)
        timRcRepeat--;
}

void swTimSetDisplay(int16_t value)
{
    timDisplay = value;
}

int16_t swTimGetDisplay(void)
{
    return timDisplay;
}

void swTimSetSpConvert(int16_t value)
{
    timSpConvert = value;
}

int16_t swTimGetSpConvert(void)
{
    return timSpConvert;
}

void swTimSetTunerPoll(int16_t value)
{
    timTunerPoll = value;
}

int16_t swTimGetTunerPoll(void)
{
    return timTunerPoll;
}

void swTimSetInitHw(int16_t value)
{
    timInitHw = value;
}

int16_t swTimGetInitHw(void)
{
    return timInitHw;
}

void swTimSetRcRepeat(int16_t value)
{
    timRcRepeat = value;
}

int16_t swTimGetRcRepeat(void)
{
    return timRcRepeat;
}
