#include "rc.h"
#include "pins.h"

#include <stm32f1xx_ll_bus.h>

static bool rc;

void rcInit(void)
{
    rc = false;

    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void rcIRQ()
{
    rc = !READ(RC);
}

bool rcGetValue(void)
{
    return rc;
}
