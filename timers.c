#include "timers.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_tim.h>

static volatile int16_t timDisplay = SW_TIM_OFF;
static volatile int16_t timSpConvert = SW_TIM_OFF;

static void TIM2_Init(void)
{
    LL_TIM_InitTypeDef TIM_InitStruct;

    // Peripheral clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    // TIM2 interrupt Init
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM_InitStruct.Prescaler = 99;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 35;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    // 72MHz / 100 / 36 => 20kHz
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM2);
}

void timersInit(void)
{
    LL_SYSTICK_EnableIT();

    TIM2_Init();

    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableIT_UPDATE(TIM2);
}


void swTimUpdate(void)
{
    if (timDisplay > 0)
        timDisplay--;
    if (timSpConvert > 0)
        timSpConvert--;
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

int16_t swTimGetSpConvert()
{
    return timSpConvert;
}
