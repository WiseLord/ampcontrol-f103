#include "pins.h"

#include <stm32f1xx_ll_bus.h>

void pinsInit(void)
{
    // NOJTAG: JTAG-DP Disabled and SW-DP Enabled
    LL_GPIO_AF_Remap_SWJ_NOJTAG();

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = DISP_D0_Pin | DISP_D1_Pin | DISP_D2_Pin | DISP_D3_Pin |
                          DISP_D4_Pin | DISP_D5_Pin | DISP_D6_Pin | DISP_D7_Pin |
                          DISP_DATA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    GPIO_InitStruct.Pin = DISP_CTRL1_Pin | DISP_CTRL2_Pin | DISP_STROB_Pin |
                          DISP_RESET_Pin | DISP_RW_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

    GPIO_InitStruct.Pin = DISP_BCKL_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
