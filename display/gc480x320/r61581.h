#ifndef R61581_H
#define R61581_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void r61581Init(DispDriver **driver);
void r61581Rotate(uint8_t rotate);
void r615811Shift(int16_t value);

void r61581Sleep(void);
void r61581Wakeup(void);

void r61581DrawPixel(int16_t x, int16_t y, uint16_t color);
void r61581DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void r61581DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // R61581_H
