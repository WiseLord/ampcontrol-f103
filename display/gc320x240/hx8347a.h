#ifndef HX8347A_H
#define HX8347A_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8347aInit(DispDriver **driver);

void hx8347aSleep(void);
void hx8347aWakeup(void);

void hx8347aDrawPixel(int16_t x, int16_t y, uint16_t color);
void hx8347aDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void hx8347aDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // HX8347A_H
