#ifndef R61581_H
#define R61581_H

#include <stdint.h>
#include "../dispdrv.h"

void r61581Init(DispDriver **driver);

void r61581Sleep(void);
void r61581Wakeup(void);

void r61581DrawPixel(int16_t x, int16_t y, uint16_t color);
void r61581DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void r61581DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // R61581_H
