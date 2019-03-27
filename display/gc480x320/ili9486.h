#ifndef ILI9486_H
#define ILI9486_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdrv.h"

void ili9486Init(DispDriver **driver);
void ili9486Rotate(uint8_t rotate);
void ili9486Shift(int16_t value);

void ili9486Sleep(void);
void ili9486Wakeup(void);

void ili9486DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9486DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ili9486DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ILI9481_H
