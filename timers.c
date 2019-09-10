#include "timers.h"

#include "hwlibs.h"

void timerInit(void *tim, uint32_t prescaler, uint32_t reload)
{
    TIM_TypeDef *TIMx = (TIM_TypeDef *)tim;

    if (TIMx == TIM2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
        NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(TIM2_IRQn);
    } else if (TIMx == TIM3) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
        NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(TIM3_IRQn);
    }

    LL_TIM_SetPrescaler(tim, prescaler);
    LL_TIM_SetCounterMode(tim, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(tim, reload);
    LL_TIM_SetClockDivision(tim, LL_TIM_CLOCKDIVISION_DIV1);

    LL_TIM_DisableARRPreload(tim);
    LL_TIM_SetClockSource(tim, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_SetTriggerOutput(tim, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(tim);

    LL_TIM_EnableCounter(tim);
    LL_TIM_EnableIT_UPDATE(tim);
}

void timersInit(void)
{
    timerInit(TIM2, 99, 35); // 20kHz timer:Dsplay IRQ/PWM and ADC conversion trigger
    timerInit(TIM3, 71, 65535); // 1MHz timer for remote control handling
}
