#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "colors.h"
#include "fonts.h"
#include "hwlibs.h"
#include "utils.h"

#ifdef _DISP_SPI
#include "spi.h"
#define SPI_DISPLAY             SPI2
#define DISP_WAIT_BUSY()        spiWaitBusy(SPI_DISPLAY)
#else
#define DISP_WAIT_BUSY()        (void)0
#endif

#define LCD_BR_MIN          1
#define LCD_BR_MAX          32

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
#define DISP_RD_Pin             LL_GPIO_PIN_10
#endif
#ifdef _DISP_RST_ENABLED
#define DISP_RST_Port           GPIOA
#define DISP_RST_Pin            LL_GPIO_PIN_15
#endif

typedef struct {
    void (*init)(void);
    void (*sleep)(void);
    void (*wakeup)(void);
    void (*setWindow)(int16_t x, int16_t y, int16_t w, int16_t h);
    void (*rotate)(bool rotate);
    void (*shift)(int16_t value);

    void *fb;
    void (*fbSync)(void);
    void (*fbSetPixel)(int16_t x, int16_t y, color_t data);

    int16_t width;
    int16_t height;
} DispDriver;

extern const DispDriver dispdrv;

void dispdrvReset(void);
void dispdrvInit(void);

uint8_t dispdrvGetBus(void);

void dispdrvSendData8(uint8_t data);
void dispdrvSendData16(uint16_t data);

void dispdrvSelectReg8(uint8_t reg);
void dispdrvWriteReg8(uint8_t reg, uint8_t data);

void dispdrvSelectReg16(uint16_t reg);
void dispdrvWriteReg16(uint16_t reg, uint16_t data);

uint16_t dispdrvReadData16(void);
void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs);

void dispdrvDrawPixel(int16_t x, int16_t y, color_t color);
void dispdrvDrawRect(int16_t x, int16_t y, int16_t w, int16_t h, color_t color);

void dispdrvDrawVertGrad(int16_t x, int16_t y, int16_t w, int16_t h, color_t *gr);

void dispdrvDrawImage(tImage *img, bool portrate, int16_t x, int16_t y,
                      color_t color, color_t bgColor,
                      int16_t xOft, int16_t yOft, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H
