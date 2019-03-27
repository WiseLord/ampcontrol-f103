#ifndef S6D0144_H
#define S6D0144_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void s6d0144Init(DispDriver **driver);

void s6d0144Sleep(void);
void s6d0144Wakeup(void);

void s6d0144DrawPixel(int16_t x, int16_t y, uint16_t color);
void s6d0144DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void s6d0144DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // S6D0144_H
