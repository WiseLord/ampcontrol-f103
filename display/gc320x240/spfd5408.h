#ifndef SPFD5408_H
#define SPFD5408_H

#include <inttypes.h>

#include "gc320x240.h"

void spfd5408Init(GlcdDriver **driver);
void spfd5408Clear(void);
void spfd5408BusIRQ(void);

void spfd5408Sleep(void);
void spfd5408Wakeup(void);

void spfd5408DrawPixel(int16_t x, int16_t y, uint16_t color);
void spfd5408DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void spfd5408DrawImage(tImage *img);

#endif // SPFD5408_H
