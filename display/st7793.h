#ifndef ST7793_H
#define ST7793_H

#include <inttypes.h>

#include "glcd.h"

void st7793Init(GlcdDriver **driver);
void st7793Clear(void);
void st7793BusIRQ(void);

void st7793Sleep(void);
void st7793Wakeup(void);

void st7793DrawPixel(int16_t x, int16_t y, uint16_t color);
void st7793DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void st7793DrawImage(tImage *img);

#endif // ST7793_H
