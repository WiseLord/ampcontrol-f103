#ifndef ILI9320_H
#define ILI9320_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9320Init(void);
void ili9320Rotate(uint8_t rotate);
void ili9320Shift(int16_t value);

void ili9320Sleep(void);
void ili9320Wakeup(void);

void ili9320SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ILI9320_H
