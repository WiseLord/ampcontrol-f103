#ifndef ILI9320_H
#define ILI9320_H

#include <inttypes.h>

#include "glcd.h"

void ili9320Init(GlcdDriver **driver);
void ili9320Clear(void);
void ili9320BusIRQ(void);

void ili9320Sleep(void);
void ili9320Wakeup(void);

void ili9320DrawPixel(int16_t x, int16_t y, uint16_t color);
void ili9320DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ili9320DrawImage(tImage *img);

#endif // ILI9320_H
