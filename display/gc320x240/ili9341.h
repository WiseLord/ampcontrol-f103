#ifndef ILI9341_H
#define ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9341Init(DispDriver **driver);
void ili9341Rotate(uint8_t rotate);
void ili9341Shift(int16_t value);

void ili9341Sleep(void);
void ili9341Wakeup(void);

void ili9341SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ILI9341_H
