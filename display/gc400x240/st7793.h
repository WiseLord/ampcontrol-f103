#ifndef ST7793_H
#define ST7793_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void st7793Init(DispDriver **driver);
void st7793Rotate(uint8_t rotate);

void st7793Sleep(void);
void st7793Wakeup(void);

void st7793DrawPixel(int16_t x, int16_t y, uint16_t color);
void st7793DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void st7793DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ST7793_H
