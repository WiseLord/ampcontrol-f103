#include "input.h"

#include "main.h"

void inputPoll()
{
    static volatile uint32_t i;
    if (i++ & 0x00000100) {
        LL_GPIO_ResetOutputPin(DISP_BL_GPIO_Port, DISP_BL_Pin);
    } else {
        LL_GPIO_SetOutputPin(DISP_BL_GPIO_Port, DISP_BL_Pin);
    }
}
