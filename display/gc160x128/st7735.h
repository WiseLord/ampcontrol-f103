#ifndef ST7735_H
#define ST7735_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void st7735Init(DispDriver **driver);

void st7735Sleep(void);
void st7735Wakeup(void);

void st7735DrawPixel(int16_t x, int16_t y, uint16_t color);
void st7735DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void st7735DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // ST7735_H
