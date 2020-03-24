#include "swtimers.h"

#include "hwlibs.h"

static int32_t swTimers[SW_TIM_END];

static void swTimUpdate(void)
{
    for (uint8_t i = 0; i < SW_TIM_DEC_END; i++) {
        if (swTimers[i] > 0) {
            swTimers[i]--;
        }
    }
    swTimers[SW_TIM_SYSTEM]++;
}

void SysTick_Handler(void)
{
    swTimUpdate();
}

void swTimInit(void)
{
    LL_SYSTICK_EnableIT();

    for (uint8_t i = 0; i < SW_TIM_DEC_END; i++) {
        swTimers[i] = SW_TIM_OFF;
    }
    swTimers[SW_TIM_SYSTEM] = 0;
}

void swTimSet(SwTimer timer, int32_t value)
{
    swTimers[timer] = value;
}

int32_t swTimGet(SwTimer timer)
{
    return swTimers[timer];
}
