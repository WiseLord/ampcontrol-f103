#ifndef LS020_H
#define LS020_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ls020Init(DispDriver **driver);

void ls020Sleep(void);
void ls020Wakeup(void);

void ls020DrawPixel(int16_t x, int16_t y, uint16_t color);
void ls020DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ls020DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // LS020_H
