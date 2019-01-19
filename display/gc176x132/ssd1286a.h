#ifndef SSD1286A_H
#define SSD1286A_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ssd1286aInit(DispDriver **driver);

void ssd1286aSleep(void);
void ssd1286aWakeup(void);

void ssd1286aDrawPixel(int16_t x, int16_t y, uint16_t color);
void ssd1286aDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void ssd1286aDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // SSD1286A_H
