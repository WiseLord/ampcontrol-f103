#ifndef SSD2119_H
#define SSD2119_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ssd2119Init(DispDriver **driver);
void ssd2119Rotate(uint8_t rotate);
void ssd2119Shift(int16_t value);

void ssd2119Sleep(void);
void ssd2119Wakeup(void);

void ssd2119DrawPixel(int16_t x, int16_t y, uint16_t color);
void ssd2119DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ssd2119DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // SSD2119_H
