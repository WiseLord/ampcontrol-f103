#include "pins.h"

#include "hwlibs.h"

#include "i2c.h"
#include "settings.h"

static void pinsInitButtons(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_INPUT;
    initDef.Pull = LL_GPIO_PULL_UP;

    initDef.Pin = DISP_DATA_Pin;
    LL_GPIO_Init(DISP_DATA_Port, &initDef);
}

static void pinsInitRc(void)
{
#ifdef _STM32F1
    LL_GPIO_AF_SetEXTISource(RC_AR_ExtiPort, RC_AR_ExtiLine);
    IN_F(RC);
#endif

    LL_EXTI_DisableEvent_0_31(RC_ExtiLine);
    LL_EXTI_EnableIT_0_31(RC_ExtiLine);
    LL_EXTI_EnableRisingTrig_0_31(RC_ExtiLine);
    LL_EXTI_EnableFallingTrig_0_31(RC_ExtiLine);
}

static void pinsInitDisplay(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef _STM32F3
    initDef.Pull = LL_GPIO_PULL_NO;
#endif

    initDef.Pin = DISP_CS_Pin;
    LL_GPIO_Init(DISP_CS_Port, &initDef);
    initDef.Pin = DISP_BCKL_Pin;
    LL_GPIO_Init(DISP_BCKL_Port, &initDef);
    initDef.Pin = DISP_RS_Pin;
    LL_GPIO_Init(DISP_RS_Port, &initDef);
#ifndef _DISP_SPI
    initDef.Pin = DISP_WR_Pin;
    LL_GPIO_Init(DISP_WR_Port, &initDef);
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
}

static void pinsHwResetI2C(void)
{
    OUT_INIT(SI470X_RST, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);

    OUT_INIT(SI470X_SCLK, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(SI470X_SDIO, LL_GPIO_OUTPUT_OPENDRAIN, LL_GPIO_SPEED_FREQ_HIGH);

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

#ifdef _STM32F1
    LL_GPIO_AF_EnableRemap_I2C1();
#endif

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = AMP_I2C_SCK_Pin | AMP_I2C_SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
#ifdef _STM32F3
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
#endif

    LL_GPIO_Init(AMP_I2C_Port, &GPIO_InitStruct);
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

    OUT_INIT(MUTE, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(STBY, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
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
