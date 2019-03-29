#ifndef ST7735_H
#define ST7735_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void st7735Init(DispDriver **driver);

void st7735Sleep(void);
void st7735Wakeup(void);

void st7735SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ST7735_H
