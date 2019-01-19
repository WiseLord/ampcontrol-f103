#ifndef MC2PA8201_H
#define MC2PA8201_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void mc2pa8201Init(DispDriver **driver);

void mc2pa8201Sleep(void);
void mc2pa8201Wakeup(void);

void mc2pa8201DrawPixel(int16_t x, int16_t y, uint16_t color);
void mc2pa8201DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void mc2pa8201DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // MC2PA8201_H
