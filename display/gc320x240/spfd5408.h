#ifndef SPFD5408_H
#define SPFD5408_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void spfd5408Init(DispDriver **driver);

void spfd5408Sleep(void);
void spfd5408Wakeup(void);

void spfd5408SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // SPFD5408_H
