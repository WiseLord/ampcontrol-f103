#ifndef HX8340_H
#define HX8340_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void hx8340Init(void);

void hx8340Sleep(void);
void hx8340Wakeup(void);

void hx8340SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // HX8340_H
