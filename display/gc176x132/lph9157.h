#ifndef LPH9157_H
#define LPH9157_H

#include <stdint.h>
#include "../dispdrv.h"

void lph9157Init(DispDriver **driver);

void lph9157Sleep(void);
void lph9157Wakeup(void);

void lph9157DrawPixel(int16_t x, int16_t y, uint16_t color);
void lph9157DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void lph9157DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // LPH9157_H
