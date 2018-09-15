#ifndef DISPDRV_H
#define DISPDRV_H

#include "glcd.h"

void dispdrvInit(GlcdDriver *driver);
void dispdrvBusIRQ(void);

void dispdrvWaitOperation(void);
void dispdrvSendData8(uint8_t data);
void dispdrvSendData16(uint16_t data);
void dispdrvSendFill(uint32_t size, uint16_t color);
void dispdrvSendImage(tImage *img, uint16_t w, uint16_t h);

#endif // DISPDRV_H
