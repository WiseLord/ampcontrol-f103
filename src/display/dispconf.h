#ifndef DISPCONF_H
#define DISPCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F103xB)
#include "hw_stm32f1_ll.h"
#elif defined (STM32F303xC)
#include "hw_stm32f3_ll.h"
#endif

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
#define DISP_RD_Pin             LL_GPIO_PIN_11
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
