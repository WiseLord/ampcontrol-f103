#ifndef ILI9225_H
#define ILI9225_H

#include <stdint.h>
#include "../dispdrv.h"

void ili9225Init(DispDriver **driver);

void ili9225Sleep(void);
void ili9225Wakeup(void);

void ili9225DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9225DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ili9225DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // ILI9225_H
