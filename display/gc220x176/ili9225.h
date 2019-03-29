#ifndef ILI9225_H
#define ILI9225_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9225Init(DispDriver **driver);

void ili9225Sleep(void);
void ili9225Wakeup(void);

void ili9225SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ILI9225_H
