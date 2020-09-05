#ifndef DISPCONF_H
#define DISPCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F103xB)
#include <stm32f1xx_ll_gpio.h>
#elif defined (STM32F303xC)
#include <stm32f3xx_ll_gpio.h>
#endif

#define CONCAT(x,y)             x ## y

#define OUT_PIN(p)          \
    do {                    \
        LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT); \
        LL_GPIO_SetPinSpeed(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_SPEED_FREQ_HIGH); \
        LL_GPIO_SetPinOutputType(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_OUTPUT_PUSHPULL); \
    } while (0);
// TODO: consider setting LL_GPIO_PULL_NO on STM32F3

#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define TOG(p)                  (LL_GPIO_TogglePin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#ifdef STM32F1
#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)
#else
#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin)) & 0x0000FFFFU)
#endif

#define OUT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN(p)                   (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_INPUT))

#ifdef STM32F1
#define IS_GPIO_HI(x)           ((x ## _Pin) & 0x00FF0000U)
#define IS_GPIO_LO(x)           ((x ## _Pin) & 0x0000FF00U)
#endif
#ifdef STM32F3
#define IS_GPIO_HI(x)           ((x ## _Pin) & 0x0000FF00U)
#define IS_GPIO_LO(x)           ((x ## _Pin) & 0x000000FFU)
#endif

#define READ_BYTE(p)            (IS_GPIO_LO(p) ? (READ(p) & 0x00FF) : (READ(p) & 0xFF00) >> 8)
#define WRITE_BYTE(p, data)     (CONCAT(p, _Port)->BSRR = (IS_GPIO_LO(p) ? (0x00FF0000U | (uint32_t)data) : (0xFF000000U | (uint32_t)(data << 8))))

#ifdef _DISP_SPI
#include "spi.h"
#define SPI_DISPLAY             SPI2
#define DISP_WAIT_BUSY()        spiWaitBusy(SPI_DISPLAY)
#define DISP_SPI_INIT()         spiInit(SPI_DISPLAY, false)
#define DISP_SPI_SEND_BYTE(x)   spiSendByte(SPI_DISPLAY, x)
#else
#define DISP_WAIT_BUSY()        (void)0
#endif

#include "utils.h"
#define DISP_MDELAY(x)          utilmDelay(x)

// TFT LCD pins
#ifdef _DISP_16BIT
#define DISP_DATA_HI_Port       GPIOB
#define DISP_DATA_HI_Pin        (LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | \
                                 LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15)
#define DISP_DATA_LO_Port       GPIOC
#define DISP_DATA_LO_Pin        (LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)
#else
#define DISP_DATA_Port          GPIOB

#define DISP_DATA_Pin           (LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)
#endif

#define DISP_CS_Port            GPIOB
#define DISP_CS_Pin             LL_GPIO_PIN_12
#define DISP_RS_Port            GPIOB
#define DISP_RS_Pin             LL_GPIO_PIN_14
#define DISP_WR_Port            GPIOB
#define DISP_WR_Pin             LL_GPIO_PIN_15

#ifdef _DISP_READ_ENABLED
#define DISP_RD_Port            GPIOB
#define DISP_RD_Pin             LL_GPIO_PIN_14
#endif

#ifdef _DISP_RST_ENABLED
#define DISP_RST_Port           GPIOB
#define DISP_RST_Pin            LL_GPIO_PIN_10
#endif

#ifdef _DISP_BCKL_ENABLED
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_13
#endif

#ifdef __cplusplus
}
#endif

#endif // DISPCONF_H
