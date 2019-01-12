#include "pins.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>

#include "display/glcd.h"

#ifdef _SI470X
#include "tuner/si470x.h"
#endif

static void pinsInitButtons(void)
{
    LL_GPIO_InitTypeDef gpio;

    gpio.Mode = LL_GPIO_MODE_INPUT;
    gpio.Pull = LL_GPIO_PULL_UP;

    gpio.Pin = INPUT_Pin;
    LL_GPIO_Init(INPUT_Port, &gpio);
}

static void pinsInitRc(void)
{
    LL_EXTI_InitTypeDef exti;

    LL_GPIO_AF_SetEXTISource(RC_AR_ExtiPort, RC_AR_ExtiLine);

    exti.Line_0_31 = RC_ExtiLine;
    exti.LineCommand = ENABLE;
    exti.Mode = LL_EXTI_MODE_IT;
    exti.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
    LL_EXTI_Init(&exti);

    IN_F(RC);
}

static void pinsInitDisplay(void)
{
#if defined(_KS0108A) || defined(_KS0108B)
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_LOW;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = KS0108_DATA_Pin;
    LL_GPIO_Init(KS0108_DATA_Port, &initDef);

    OUT_INIT(KS0108_DI,   LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(KS0108_E,    LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(KS0108_CS1,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(KS0108_CS2,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(KS0108_BCKL, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);

#elif defined(_ST7920)
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_LOW;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = ST7920_DATA_Pin;
    LL_GPIO_Init(ST7920_DATA_Port, &initDef);

    OUT_INIT(ST7920_RS,   LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(ST7920_E,    LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(ST7920_PSB,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(ST7920_BCKL, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);

#elif defined(_DISP_8BIT)

    OUT_INIT(DISP_RS,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_CS,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_WR,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_BCKL, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);

#elif defined(_DISP_SPI)

    ALT_INIT(DISP_SPI_SDI, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    ALT_INIT(DISP_SPI_SCK, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_CS,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);

    OUT_INIT(DISP_SPI_DC,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_BCKL, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);

#elif defined(_DISP_I2C)

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = DISP_I2C_SCK_Pin | DISP_I2C_SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;

    LL_GPIO_Init(DISP_I2C_Port, &GPIO_InitStruct);

#else
#error "Unsupported display driver"
#endif
}

void pinsHwReset(void)
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
    IN_F(SI470X_SDIO);  // SDIO = 1
}

void pinsInitAmpI2c(void)
{
    LL_GPIO_AF_EnableRemap_I2C1();

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = AMP_I2C_SCK_Pin | AMP_I2C_SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;

    LL_GPIO_Init(AMP_I2C_Port, &GPIO_InitStruct);
}

void pinsInit(void)
{
    // NOJTAG: JTAG-DP Disabled and SW-DP Enabled
    LL_GPIO_AF_Remap_SWJ_NOJTAG();

    // Enable clock for all GPIO peripherials
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

    pinsInitButtons();
    pinsInitRc();
    pinsInitDisplay();

    OUT_INIT(MUTE, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(STBY, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
}
