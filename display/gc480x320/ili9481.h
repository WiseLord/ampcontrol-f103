#ifndef ILI9481_H
#define ILI9481_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9481Init(DispDriver **driver);
void ili9481Rotate(uint8_t rotate);

void ili9481Sleep(void);
void ili9481Wakeup(void);

void ili9481DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9481DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ili9481DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ILI9481_H
