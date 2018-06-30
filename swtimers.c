#include "swtimers.h"

static volatile int16_t timDisplay;

void swTimUpdate(void)
{
    if (timDisplay > 0)
        timDisplay--;
}


void swTimSetDisplay(int16_t value)
{
    timDisplay = value;
}

int16_t swTimGetDisplay()
{
    return timDisplay;
}
