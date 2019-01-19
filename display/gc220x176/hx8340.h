#ifndef HX8340_H
#define HX8340_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8340Init(DispDriver **driver);

void hx8340Sleep(void);
void hx8340Wakeup(void);

void hx8340DrawPixel(int16_t x, int16_t y, uint16_t color);
void hx8340DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void hx8340DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // HX8340_H
