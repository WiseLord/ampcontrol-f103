#ifndef S6D0144_H
#define S6D0144_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void s6d0144Init(void);

void s6d0144Sleep(void);
void s6d0144Wakeup(void);

void s6d0144SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // S6D0144_H
