#ifndef HX8340_H
#define HX8340_H

#include <inttypes.h>

#include "gc220x176.h"

void hx8340Init(GlcdDriver **driver);
void hx8340Clear(void);
void hx8340BusIRQ(void);

void hx8340Sleep(void);
void hx8340Wakeup(void);

void hx8340DrawPixel(int16_t x, int16_t y, uint16_t color);
void hx8340DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void hx8340DrawImage(tImage *img);

#endif // HX8340_H
