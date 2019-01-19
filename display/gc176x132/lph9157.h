#ifndef LPH9157_H
#define LPH9157_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void lph9157Init(DispDriver **driver);

void lph9157Sleep(void);
void lph9157Wakeup(void);

void lph9157DrawPixel(int16_t x, int16_t y, uint16_t color);
void lph9157DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void lph9157DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // LPH9157_H
