#include "timers.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_tim.h>

static volatile int16_t timDisplay = SW_TIM_OFF;
static volatile int16_t timSpConvert = SW_TIM_OFF;
static volatile int16_t timTunerPoll = SW_TIM_OFF;
static volatile int16_t timInitHw = SW_TIM_OFF;

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

static void TIM3_Init(void)
{
    LL_TIM_InitTypeDef TIM_InitStruct;

    // Peripheral clock enable
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    // TIM2 interrupt Init
    NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM_InitStruct.Prescaler = 71;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 65535;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    // 72MHz / 72 => 1MHz
    LL_TIM_Init(TIM3, &TIM_InitStruct);

    LL_TIM_DisableARRPreload(TIM3);
    LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM3);
}

void timersInit(void)
{
    LL_SYSTICK_EnableIT();

    TIM2_Init(); // 20kHz timer:Dsplay IRQ/PWM and ADC conversion trigger

    TIM3_Init(); // 1MHz timer for remote control handling

    LL_TIM_EnableCounter(TIM2);
    LL_TIM_EnableIT_UPDATE(TIM2);

    LL_TIM_EnableCounter(TIM3);
    LL_TIM_EnableIT_UPDATE(TIM3);
}


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
