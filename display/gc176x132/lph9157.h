#ifndef LPH9157_H
#define LPH9157_H

#include <inttypes.h>

#include "gc176x132.h"

void lph9157Init(GlcdDriver **driver);
void lph9157Clear(void);

void lph9157Sleep(void);
void lph9157Wakeup(void);

void lph9157DrawPixel(int16_t x, int16_t y, uint16_t color);
void lph9157DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void lph9157DrawImage(tImage *img);

#endif // LPH9157_H
