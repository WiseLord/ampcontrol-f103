#ifndef HX8347_H
#define HX8347_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8347Init(DispDriver **driver);

void hx8347Sleep(void);
void hx8347Wakeup(void);

void hx8347DrawPixel(int16_t x, int16_t y, uint16_t color);
void hx8347DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void hx8347DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // HX8347_H
