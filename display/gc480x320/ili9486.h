#ifndef ILI9486_H
#define ILI9486_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdrv.h"

void ili9486Init(void);
void ili9486Rotate(uint8_t rotate);
void ili9486Shift(int16_t value);

void ili9486Sleep(void);
void ili9486Wakeup(void);

void ili9486SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ILI9481_H
