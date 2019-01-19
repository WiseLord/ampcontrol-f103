#ifndef DISPDRV_H
#define DISPDRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "dispdefs.h"

#define DISPLAY_IRQ         dispdrvBusIRQ

#ifdef _DISP_SPI
#define DISP_WAIT_BUSY()    while(LL_SPI_IsActiveFlag_BSY(SPI2) || !LL_SPI_IsActiveFlag_TXE(SPI2))
#else
#define DISP_WAIT_BUSY();
#endif

void dispdrvInit(DispDriver **glcd);
void dispdrvPwm(void);
void dispdrvSetBrightness(int8_t value);
uint8_t dispdrvGetBus(void);

void dispdrvBusIRQ(void);

void dispdrvSendData8(uint8_t data);
void dispdrvSendData16(uint16_t data);
void dispdrvSendFill(int32_t size, uint16_t color);
void dispdrvSendImage(tImage *img, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // DISPDRV_H
