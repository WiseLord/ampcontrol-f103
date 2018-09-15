#ifndef ST7735_H
#define ST7735_H

#include <inttypes.h>

#include "../glcd.h"

void st7735Init(GlcdDriver **driver);
void st7735Clear(void);

void st7735Sleep(void);
void st7735Wakeup(void);

void st7735DrawPixel(int16_t x, int16_t y, uint16_t color);
void st7735DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void st7735DrawImage(tImage *img);

#endif // ST7735_H
