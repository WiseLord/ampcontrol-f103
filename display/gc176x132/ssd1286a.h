#ifndef SSD1286A_H
#define SSD1286A_H

#include <stdint.h>
#include "../dispdrv.h"

void ssd1286aInit(DispDriver **driver);

void ssd1286aSleep(void);
void ssd1286aWakeup(void);

void ssd1286aDrawPixel(int16_t x, int16_t y, uint16_t color);
void ssd1286aDrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ssd1286aDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#endif // SSD1286A_H
