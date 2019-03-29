#ifndef R61581_H
#define R61581_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void r61581Init(DispDriver **driver);
void r61581Rotate(uint8_t rotate);
void r615811Shift(int16_t value);

void r61581Sleep(void);
void r61581Wakeup(void);

void r61581SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // R61581_H
