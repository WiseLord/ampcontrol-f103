#ifndef ILI9481_H
#define ILI9481_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9481Init(void);
void ili9481Rotate(uint8_t rotate);

void ili9481Sleep(void);
void ili9481Wakeup(void);

void ili9481SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ILI9481_H
