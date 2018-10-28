#ifndef ILI9327_H
#define ILI9327_H

#include <stdint.h>
#include "../dispdrv.h"

void ili9327Init(DispDriver **driver);

void ili9327Sleep(void);
void ili9327Wakeup(void);

void ili9327DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9327DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ili9327DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // ILI9327_H
