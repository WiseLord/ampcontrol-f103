#include "pins.h"

#include "hwlibs.h"

#include "i2c.h"
#include "settings.h"

void pinsInitHwReset()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = SI470X_RST_Pin;
    LL_GPIO_Init(SI470X_RST_Port, &GPIO_InitStruct);
}

void pinsResetUSB()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;

    GPIO_InitStruct.Pin = USB_DM_Pin;
    LL_GPIO_Init(USB_DM_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = USB_DP_Pin;
    LL_GPIO_Init(USB_DP_Port, &GPIO_InitStruct);

    CLR(USB_DM);
    CLR(USB_DP);
    LL_mDelay(20);
    IN(USB_DM);
    IN(USB_DP);
}

void pinsHwResetI2c(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
#ifdef STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = SI470X_SCLK_Pin | SI470X_SDIO_Pin;
    LL_GPIO_Init(SI470X_SDIO_Port, &GPIO_InitStruct);

    CLR(SI470X_RST);    // Start display and Si470x reset

    CLR(SI470X_SCLK);   // Put to zero in this sequence to avoid fake START

    LL_mDelay(1);       // Select 2-wire interface:
    CLR(SI470X_SDIO);   // SDIO = 0
    IN(SI470X_SCLK);  // SCLK = 1

    LL_mDelay(5);       // Reset

    SET(SI470X_RST);    // End of reset

    LL_mDelay(1);

    IN(SI470X_SDIO);  // SDIO = 1
}

void pinsInit(void)
{
    pinsInitHwReset();
    pinsResetUSB();
}
