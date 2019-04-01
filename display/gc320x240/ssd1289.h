#ifndef SSD1289_H
#define SSD1289_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ssd1289Init(DispDriver **driver);
void ssd1289Rotate(uint8_t rotate);
void ssd1289Shift(int16_t value);

void ssd1289Sleep(void);
void ssd1289Wakeup(void);

void ssd1289SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // SSD1289_H
