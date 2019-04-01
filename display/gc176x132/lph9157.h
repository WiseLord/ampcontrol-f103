#ifndef LPH9157_H
#define LPH9157_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void lph9157Init(void);

void lph9157Sleep(void);
void lph9157Wakeup(void);

void lph9157SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // LPH9157_H
