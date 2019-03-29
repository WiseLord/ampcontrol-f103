#ifndef S6D0129_H
#define S6D0129_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void s6d0129Init(DispDriver **driver);
void s6d0129Rotate(uint8_t rotate);

void s6d0129Sleep(void);
void s6d0129Wakeup(void);

void s6d0129SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // S6D0129_H
