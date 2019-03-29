#ifndef LGDP4524_H
#define LGDP4524_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void lgdp4524Init(DispDriver **driver);

void lgdp4524Sleep(void);
void lgdp4524Wakeup(void);

void lgdp4524SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // LGDP4524_H
