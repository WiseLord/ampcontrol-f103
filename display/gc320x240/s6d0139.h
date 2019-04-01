#ifndef S6D0139_H
#define S6D0139_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void s6d0139Init(void);
void s6d0139Rotate(uint8_t rotate);

void s6d0139Sleep(void);
void s6d0139Wakeup(void);

void s6d0139SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // S6D0139_H
