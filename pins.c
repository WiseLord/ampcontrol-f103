#include "pins.h"

#include "hwlibs.h"

#include "i2c.h"
#include "settings.h"

static void pinsInitButtons(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStructf;

    GPIO_InitStructf.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStructf.Pull = LL_GPIO_PULL_UP;

    GPIO_InitStructf.Pin = DISP_DATA_Pin;
    LL_GPIO_Init(DISP_DATA_Port, &GPIO_InitStructf);
}

static void pinsInitRc(void)
{
#ifdef _STM32F1
    LL_GPIO_AF_SetEXTISource(RC_AR_ExtiPort, RC_AR_ExtiLine);
    LL_GPIO_SetPinMode(RC_Port, RC_Pin, LL_GPIO_MODE_FLOATING);
#endif

    LL_EXTI_DisableEvent_0_31(RC_ExtiLine);
    LL_EXTI_EnableIT_0_31(RC_ExtiLine);
    LL_EXTI_EnableRisingTrig_0_31(RC_ExtiLine);
    LL_EXTI_EnableFallingTrig_0_31(RC_ExtiLine);
}

static void pinsInitDisplay(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = DISP_CS_Pin;
    LL_GPIO_Init(DISP_CS_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = DISP_BCKL_Pin;
    LL_GPIO_Init(DISP_BCKL_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = DISP_RS_Pin;
    LL_GPIO_Init(DISP_RS_Port, &GPIO_InitStruct);
#ifndef _DISP_SPI
    GPIO_InitStruct.Pin = DISP_WR_Pin;
    LL_GPIO_Init(DISP_WR_Port, &GPIO_InitStruct);
#endif
}

void pinsInitMuteStby(MuteStby value)
{
#ifdef SWD_FORCED
    value = MUTESTBY_SWD;
#endif

    if (value == MUTESTBY_SWD) {
        // JTAG-DP Disabled and SW-DP Enabled
#ifdef _STM32F1
        LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif
    } else {
        // JTAG-DP Disabled and SW-DP Disabled
#ifdef _STM32F1
        LL_GPIO_AF_DisableRemap_SWJ();
#endif
    }

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = MUTE_Pin;
    LL_GPIO_Init(MUTE_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = STBY_Pin;
    LL_GPIO_Init(STBY_Port, &GPIO_InitStruct);
}

static void pinsHwResetI2C(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
#endif

    GPIO_InitStruct.Pin = SI470X_RST_Pin;
    LL_GPIO_Init(SI470X_RST_Port, &GPIO_InitStruct);

    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;

    GPIO_InitStruct.Pin = SI470X_SCLK_Pin;
    LL_GPIO_Init(SI470X_SCLK_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = SI470X_SDIO_Pin;
    LL_GPIO_Init(SI470X_SDIO_Port, &GPIO_InitStruct);

    CLR(SI470X_RST);    // Start display and Si470x reset

    CLR(SI470X_SCLK);   // Put to zero in this sequence to avoid fake START

    LL_mDelay(1);       // Select 2-wire interface:
    CLR(SI470X_SDIO);   // SDIO = 0
    IN_P(SI470X_SCLK);  // SCLK = 1

    LL_mDelay(5);       // Reset

    SET(SI470X_RST);    // End of reset

    LL_mDelay(1);
#ifdef _STM32F1
    IN_F(SI470X_SDIO);  // SDIO = 1
#endif
}

void pinsDeInitAmpI2c(void)
{
    LL_I2C_DeInit(I2C_AMP);
}

void pinsInitAmpI2c(void)
{
    pinsHwResetI2C();
    i2cInit(I2C_AMP, 100000);
}

void pinsInit(void)
{
    // Enable clock for all GPIO peripherials
#ifdef _STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
#endif
#ifdef _STM32F3
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#endif

    pinsInitButtons();
    pinsInitRc();
    pinsInitDisplay();

    MuteStby muteStby = (MuteStby)settingsGet(PARAM_SYSTEM_MUTESTBY);
    pinsInitMuteStby(muteStby);
}

void pinsSetMute(bool value)
{
    MuteStby muteStby = (MuteStby)settingsGet(PARAM_SYSTEM_MUTESTBY);

#ifdef SWD_FORCED
    if (muteStby == MUTESTBY_SWD) {
        muteStby = MUTESTBY_POS;
    }
#endif

    if (muteStby == MUTESTBY_POS) {
        if (value) {
            CLR(MUTE);
        } else {
            SET(MUTE);
        }
    } else if (muteStby == MUTESTBY_NEG) {
        if (value) {
            SET(MUTE);
        } else {
            CLR(MUTE);
        }
    }
}

void pinsSetStby(bool value)
{
    MuteStby muteStby = (MuteStby)settingsGet(PARAM_SYSTEM_MUTESTBY);

#ifdef SWD_FORCED
    if (muteStby == MUTESTBY_SWD) {
        muteStby = MUTESTBY_POS;
    }
#endif

    // TODO: Remove MUTESTBY_SWD condition when debug finished
    if (muteStby == MUTESTBY_POS) {
        if (value) {
            CLR(STBY);
        } else {
            SET(STBY);
        }
    } else if (muteStby == MUTESTBY_NEG) {
        if (value) {
            SET(STBY);
        } else {
            CLR(STBY);
        }
    }
}
