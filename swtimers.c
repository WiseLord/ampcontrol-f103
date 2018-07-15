#include "swtimers.h"

static volatile int16_t timDisplay = SW_TIM_OFF;

void swTimUpdate(void)
{
    if (timDisplay > 0)
        timDisplay--;
}


void swTimSetDisplay(int16_t value)
{
    timDisplay = value;
}

int16_t swTimGetDisplay(void)
{
    return timDisplay;
}
