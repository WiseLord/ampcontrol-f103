#include "rc.h"
#include "pins.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_tim.h>

static bool rc;
static uint16_t ovfCnt;

void rcInit(void)
{
    rc = false;

    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void rcIRQ()
{
    static uint16_t timCntOld = 0;
    uint16_t timCnt = LL_TIM_GetCounter(TIM3);
    timCntOld = timCnt;

    uint16_t delay = timCnt - timCntOld;

    rc = !READ(RC);

    (void)delay;
}

void rcOvfIRQ(void)
{
    ovfCnt++;
}

bool rcGetValue(void)
{
    return rc;
}

uint16_t rcGetOvfCnt()
{
    return ovfCnt;
}
