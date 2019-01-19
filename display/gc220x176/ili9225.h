#ifndef ILI9225_H
#define ILI9225_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9225Init(DispDriver **driver);

void ili9225Sleep(void);
void ili9225Wakeup(void);

void ili9225DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9225DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ili9225DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ILI9225_H
