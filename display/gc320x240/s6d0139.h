#ifndef S6D0139_H
#define S6D0139_H

#include <stdint.h>
#include "../dispdrv.h"

void s6d0139Init(DispDriver **driver);

void s6d0139Sleep(void);
void s6d0139Wakeup(void);

void s6d0139DrawPixel(int16_t x, int16_t y, uint16_t color);
void s6d0139DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void s6d0139DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // S6D0139_H
