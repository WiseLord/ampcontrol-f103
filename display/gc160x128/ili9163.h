#ifndef ILI9163_H
#define ILI9163_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9163Init(DispDriver **driver);

void ili9163Sleep(void);
void ili9163Wakeup(void);

void ili9163DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9163DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ili9163DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ILI9163_H
