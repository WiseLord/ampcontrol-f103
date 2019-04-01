#ifndef S6D04D1_H
#define S6D04D1_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void s6d04d1Init(void);
void s6d04d1Rotate(uint8_t rotate);

void s6d04d1Sleep(void);
void s6d04d1Wakeup(void);

void s6d04d1SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // S6D04D1_H
