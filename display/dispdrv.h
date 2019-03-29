#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "dispdefs.h"

#ifdef _DISP_SPI
#include <stm32f1xx_ll_spi.h>
#define DISP_WAIT_BUSY()    while(LL_SPI_IsActiveFlag_BSY(SPI2) || !LL_SPI_IsActiveFlag_TXE(SPI2))
#else
#define DISP_WAIT_BUSY();
#endif

void dispdrvReset(void);
void dispdrvInit(DispDriver **glcd);

void dispdrvPwm(void);
void dispdrvSetBrightness(int8_t value);
uint8_t dispdrvGetBus(void);

void dispdrvBusIRQ(void);

void dispdrvSendData8(uint8_t data);
void dispdrvSendData16(uint16_t data);

void dispdrvSelectReg8(uint8_t reg);
void dispdrvWriteReg8(uint8_t reg, uint8_t data);

void dispdrvSelectReg16(uint16_t reg);
void dispdrvWriteReg16(uint16_t reg, uint16_t data);

#ifdef _DISP_READ_ENABLED
uint16_t dispdrvReadData16(void);
void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs);
#endif

void dispdrvDrawPixel(int16_t x, int16_t y, uint16_t color);
void dispdrvDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void dispdrvDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H
