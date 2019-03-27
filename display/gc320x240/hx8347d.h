#ifndef HX8347D_H
#define HX8347D_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8347dInit(DispDriver **driver);

void hx8347dSleep(void);
void hx8347dWakeup(void);

void hx8347dDrawPixel(int16_t x, int16_t y, uint16_t color);
void hx8347dDrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void hx8347dDrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

#ifdef __cplusplus
}
#endif

#endif // HX8347D_H
