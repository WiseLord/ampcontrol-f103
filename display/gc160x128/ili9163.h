#ifndef ILI9163_H
#define ILI9163_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ili9163Init(void);

void ili9163Sleep(void);
void ili9163Wakeup(void);

void ili9163SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // ILI9163_H
