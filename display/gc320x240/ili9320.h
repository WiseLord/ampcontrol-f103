#ifndef ILI9320_H
#define ILI9320_H

#include <stdint.h>
#include "../dispdrv.h"

void ili9320Init(DispDriver **driver);
void ili9320Rotate(uint8_t rotate);
void ili9320Shift(uint16_t value);

void ili9320Sleep(void);
void ili9320Wakeup(void);

void ili9320DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9320DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ili9320DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // ILI9320_H
