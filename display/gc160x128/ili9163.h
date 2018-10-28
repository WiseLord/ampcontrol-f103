#ifndef ILI9163_H
#define ILI9163_H

#include <stdint.h>
#include "../dispdrv.h"

void ili9163Init(DispDriver **driver);

void ili9163Sleep(void);
void ili9163Wakeup(void);

void ili9163DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9163DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ili9163DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // ILI9163_H
