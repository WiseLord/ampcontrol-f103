#ifndef ILI9341_H
#define ILI9341_H

#include <stdint.h>
#include "../dispdrv.h"

void ili9341Init(DispDriver **driver);
void ili9341Rotate(uint8_t rotate);
void ili9341Shift(uint16_t value);

void ili9341Sleep(void);
void ili9341Wakeup(void);

void ili9341DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9341DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ili9341DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // ILI9341_H
