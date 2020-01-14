#include "swtimers.h"

static int32_t swTimers[SW_TIM_END];

static void swTimUpdate(void)
{
    for (uint8_t i = 0; i < SW_TIM_END; i++) {
        if (swTimers[i] > 0) {
            swTimers[i]--;
        }
    }
}

void SysTick_Handler(void)
{
    swTimUpdate();
}

void swTimInit(void)
{
    for (uint8_t i = 0; i < SW_TIM_END; i++) {
        swTimers[i] = SW_TIM_OFF;
    }
}

void swTimSet(SwTimer timer, int32_t value)
{
    swTimers[timer] = value;
}

int32_t swTimGet(SwTimer timer)
{
    return swTimers[timer];
}
