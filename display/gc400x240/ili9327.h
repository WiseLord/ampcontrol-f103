#ifndef ILI9327_H
#define ILI9327_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9327Init(DispDriver **driver);
void ili9327Rotate(uint8_t rotate);

void ili9327Sleep(void);
void ili9327Wakeup(void);

void ili9327DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9327DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ili9327DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ILI9327_H
