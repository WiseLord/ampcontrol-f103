#ifndef LS020_H
#define LS020_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../dispdefs.h"

void ls020Init(void);

void ls020Sleep(void);
void ls020Wakeup(void);

void ls020SetWindow(int16_t x, int16_t y, int16_t w, int16_t h);

#ifdef __cplusplus
}
#endif

#endif // LS020_H
