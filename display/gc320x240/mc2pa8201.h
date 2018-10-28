#ifndef MC2PA8201_H
#define MC2PA8201_H

#include <stdint.h>
#include "../dispdrv.h"

void mc2pa8201Init(DispDriver **driver);

void mc2pa8201Sleep(void);
void mc2pa8201Wakeup(void);

void mc2pa8201DrawPixel(int16_t x, int16_t y, uint16_t color);
void mc2pa8201DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void mc2pa8201DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // MC2PA8201_H
