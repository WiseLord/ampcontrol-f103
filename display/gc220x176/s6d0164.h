#ifndef S6D0164_H
#define S6D0164_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void s6d0164Init(DispDriver **driver);

void s6d0164Sleep(void);
void s6d0164Wakeup(void);

void s6d0164DrawPixel(int16_t x, int16_t y, uint16_t color);
void s6d0164DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void s6d0164DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // S6D0164_H
