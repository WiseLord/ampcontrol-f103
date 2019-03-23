#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "dispdefs.h"

#ifdef _DISP_SPI
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
void dispdrvSendFill(int32_t size, uint16_t color);
void dispdrvSendImage(tImage *img, uint16_t color, uint16_t bgColor);

#ifdef _DISP_READ_ENABLED
uint16_t dispdrvReadData16(void);
void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs);
#endif

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H
