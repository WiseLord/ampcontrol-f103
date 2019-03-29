#ifndef ST7793_H
#define ST7793_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void st7793Init(DispDriver **driver);
void st7793Rotate(uint8_t rotate);

void st7793Sleep(void);
void st7793Wakeup(void);

void st7793SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ST7793_H
